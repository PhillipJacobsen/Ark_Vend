/********************************************************************************
  This file contains functions used to configure hardware perhipherals and various libraries.
********************************************************************************/



/********************************************************************************
  This routine waits for a connection to your WiFi network according to "ssid" and "password" defined previously
********************************************************************************/
void setupWiFi() {
  tft.setCursor(0, 20);
  tft.setTextColor(ILI9341_WHITE);
  //  tft.setTextSize(1);
  tft.println("WiFi Search");

  WiFi.begin(ssid, password); // This starts your boards connection to WiFi.
  while (WiFi.status() != WL_CONNECTED) // This will delay your board from continuing until a WiFi connection is established.
  {
    delay(400);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 20);
  tft.print("Connected to  ");
  tft.println(WiFi.localIP());
}


/********************************************************************************
  This routine configures the ESP32 internal clock to syncronize with NTP server.

  apparently this will enable the core to periodically sync the time with the NTP server. I don't really know how often this happens
  I am not sure if daylight savings mode works correctly. Previously it seems like this was not working on ESP2866
********************************************************************************/
void setupTime() {

  configTime(timezone * 3600, dst, "pool.ntp.org", "time.nist.gov");
  // printLocalTime();
  //  delay(100);

  //wait for time to sync from servers
  while (time(nullptr) <= 100000) {
    delay(100);
  }

  time_t now = time(nullptr);   //get current time
  Serial.print("time is: ");
  Serial.println(ctime(&now));

  tft.setTextColor(ILI9341_WHITE);
  //  tft.setTextSize(1);
  tft.print(ctime(&now));      //dislay the current time

  //  struct tm * timeinfo;
  //  time(&now);
  //  timeinfo = localtime(&now);
  //  Serial.println(timeinfo->tm_hour);
}

/****************************************/
//  https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/
//  void printLocalTime()
//  {
//    struct tm timeinfo;
//    if (!getLocalTime(&timeinfo)) {
//      Serial.println("Failed to obtain time");
//      return;
//    }
//    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
//  }



void ConfigureNeoPixels(RgbColor color) {
  strip.SetPixelColor(0, color);
  strip.SetPixelColor(1, color);
  strip.SetPixelColor(2, color);
  strip.SetPixelColor(3, color);
  strip.SetPixelColor(4, color);
  strip.SetPixelColor(5, color);
  strip.SetPixelColor(6, color);
  strip.SetPixelColor(7, color);
  strip.Show();
  Serial.print("writing new color to neopixels:");
}




/********************************************************************************
  Configure peripherals and system
********************************************************************************/
void setup()
{
  Serial.begin(115200);         // Initialize Serial Connection for debug / display
  while ( !Serial && millis() < 20 );

  //Serial.println(QRcodeArkAddress);
  //strcat(QRcodeArkAddress,"happy");
  //Serial.println(QRcodeArkAddress);
  //int esprandom = (random(256,32768));
  //Serial.println("test string");
  //String str = String(esprandom);     //int is now a string
  //Serial.println(str);
  //char charBuf[6];
  //str.toCharArray(charBuf,6);
  //strcat(QRcodeArkAddress,charBuf);
  //Serial.println(QRcodeArkAddress);




  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "dark:DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w?label=ArkVend&amount=0.3&vendorField=color red");    //dARK address 51 bytes.




  //  delay(3000);
  //  esp_deep_sleep_start();

  delay(300);
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");

  //--------------------------------------------
  //  setup 240x320 TFT display with custom font and clear screen
  // tft.setFont();    //configure standard adafruit font
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);  //clear screen
  tft.setFont(&FreeSans9pt7b);

  Serial.println("drawing stuff");

  drawHomeScreen();
  Serial.println("finished home screen");

  while (true) {
    // Retrieve a point

    if (ts.touched()) {
      TS_Point p = ts.getPoint(); //read raw touchscreen data from buffer
      //  delay(5);
      //p = ts.getPoint();

      //    Serial.print("X = "); Serial.print(p.x);
      //   Serial.print("\tY = "); Serial.print(p.y);
      //    Serial.print("\tPressure = "); Serial.println(p.z);


      // Scale from ~0->4000 to tft.width using the calibration #'s
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, 239);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, 319);

      p.x = constrain(p.x, 0, 239);
      p.y = constrain(p.y, 0, 319);

      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);

      //NOTE: We are not currently checking pressure data for points

      //check for M&Ms button being pressed
      if (p.x > 30 && p.x < (30 + 130) && p.y > 70 && p.y < (70 + 40))   {
        tft.fillRoundRect(30, 70, 130, 40, 7, ArkLightRed);     //M&Ms
        tft.setCursor(70, 95);
        tft.print("M&Ms");
        delay(50);
        while (ts.touched()) {}       //wait until screen is no longer being touched
        while (!ts.bufferEmpty()) {
          p = ts.getPoint();          //empty buffer
        }
        tft.fillRoundRect(30, 70, 130, 40, 7, ArkRed);     //M&Ms
        tft.drawRoundRect(30, 70, 130, 40, 7, ILI9341_WHITE);
        tft.setTextColor(ILI9341_BLACK);
        tft.setCursor(70, 95);
        tft.print("M&Ms");
        Serial.println("Selected M&Ms");
      }

      //check for smarties button being pressed
      else if (p.x > 30 && p.x < (30 + 130) && p.y > 120 && p.y < (120 + 40))   {
        tft.fillRoundRect(30, 120, 130, 40, 7, ArkLightRed);
        tft.setCursor(60, 145);
        tft.print("Smarties");
        delay(50);
        while (ts.touched()) {}       //wait until screen is no longer being touched
        while (!ts.bufferEmpty()) {
          p = ts.getPoint();          //empty buffer
        }
        tft.fillRoundRect(30, 120, 130, 40, 7, ArkRed);
        tft.drawRoundRect(30, 120, 130, 40, 7, ILI9341_WHITE);
        tft.setTextColor(ILI9341_BLACK);
        tft.setCursor(70, 95);
        tft.setCursor(60, 145);
        tft.print("Smarties");
        Serial.println("Selected Smarties");
      }

      //check for skittles button being pressed
      else if (p.x > 30 && p.x < (30 + 130) && p.y > 170 && p.y < (170 + 40))   {
        tft.fillRoundRect(30, 170, 130, 40, 7, ArkLightRed);
        tft.setCursor(67, 195);
        tft.print("Skittles");
        delay(50);
        while (ts.touched()) {}       //wait until screen is no longer being touched
        while (!ts.bufferEmpty()) {
          p = ts.getPoint();          //empty buffer
        }
        tft.fillRoundRect(30, 170, 130, 40, 7, ArkRed);
        tft.drawRoundRect(30, 170, 130, 40, 7, ILI9341_WHITE);
        tft.setTextColor(ILI9341_BLACK);
        tft.setCursor(67, 195);
        tft.print("Skittles");
        Serial.println("Selected Skittles");
      }




    }


    delay(10);
  }

  delay(3000);
  esp_deep_sleep_start();

  //--------------------------------------------
  //  Configure NeoPixels.
  //  NOTE! If useing the ESP8266 Make sure to call strip.Begin() after you call Serial.Begin because
  //    Din pin of NeoPixel is also connected to Serial RX pin(on ESP8266) and will configure the pin for usage by the DMA interface.
  strip.Begin();
  //  strip.Show(); // Initialize all pixels to 'off'
  strip.ClearTo(RgbColor(0, 0, 0)); // Initialize all pixels to 'off'

  //--------------------------------------------
  //setup WiFi connection
  setupWiFi();

  //--------------------------------------------
  //  sync local time to NTP server
  setupTime();

  //--------------------------------------------
  //  check to see if Ark Node is synced
  //  node is defined previously with "peer" and "port"
  //  returns True if node is synced to chain
  if (checkArkNodeStatus()) {
    Serial.print("\nNode is Synced: ");
    tft.println("Ark Node is synced");
  }
  else {
    Serial.print("\nNode is NOT Synced: ");
    tft.println("Ark Node is NOT synced");
  }



  //--------------------------------------------
  //  We are now going to find the last transaction received in wallet defined previously in "ArkAddress"
  //  We will start from the oldest received transaction and keep reading transactions one at a time until we have read them all.
  //  Because we read 1 trasactions at a time the page number returned by the API tells us how many transactions there are.
  //  Every transaction received will toggle the color of "searching wallet: " text between red and white.
  //  Once we have read all the transactions we will display the total number of transactions in wallet.
  //
  CursorX = tft.getCursorX();     //get current cursor position
  CursorY = tft.getCursorY();     //get current cursor position
  tft.println("searching wallet: ");
  tft.println(ArkAddress);

  lastRXpage = getMostRecentReceivedTransaction();  //lastRXpage is equal to the page number of the last received transaction in the wallet.

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(CursorX, CursorY);
  tft.println("searching wallet: ");
  tft.println(ArkAddress);

  tft.print("# of transactions in wallet: ");
  tft.println(lastRXpage);          //this is the page number of the last received transaction. This is also the total number of transactions in the wallet


  setupQRcode();

  //--------------------------------------------
  //  System is now configured! Set Neo Pixels to Green
  ConfigureNeoPixels(redgreen);

}

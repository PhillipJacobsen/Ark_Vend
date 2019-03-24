
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


void setupTime() {
  //this sets the core to periodically sync the time. I don't really know how often this happens
  //I am not sure if daylight savings mode works correctly. Previously it seems like this was not working on ESP2866
  configTime(timezone * 3600, dst, "pool.ntp.org", "time.nist.gov");
  // printLocalTime();
  delay(100);    //After connecting to WiFi network display the IP address for 2 seconds before displaying the time

  //wait for time to sync from servers
  while (time(nullptr) <= 100000) {
    delay(100);
  }

  time_t now = time(nullptr);   //get current time
  Serial.print("time is: ");
  Serial.println(ctime(&now));


  //  tft.println("Time is");


  tft.setTextColor(ILI9341_WHITE);
  //  tft.setTextSize(1);
  tft.print(ctime(&now));

  //  struct tm * timeinfo;
  //  time(&now);
  //  timeinfo = localtime(&now);
  //  Serial.println(timeinfo->tm_hour);

}





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



void setup()
{
  Serial.begin(115200);     // Initialize Serial Connection for debug / display

  tft.begin();              //setup TFT display
  tft.fillScreen(ILI9341_BLACK);  //clear screen
  tft.setFont(&FreeSans9pt7b);
  // tft.setFont();    //standard font

  setupWiFi();  //configure WiFi connection

  setupTime();  //sync local time to NTP server

  checkArkNodeStatus(); //check to see if Ark Node is synced

  //--------------------------------------------
  //  Configure NeoPixels.
  //NOTE! Make sure to call strip.Begin() after you call Serial.Begin because Din pin of NeoPixel is also connected to Serial RX pin(on ESP8266) and will configure the pin for usage by the DMA interface.
  strip.Begin();
  strip.Show(); // Initialize all pixels to 'off'
  strip.ClearTo(RgbColor(0, 0, 0));

  CursorX = tft.getCursorX();
  CursorY = tft.getCursorY();
  tft.println("searching wallet: ");
  tft.println(ArkAddress);

  lastRXpage = getMostRecentReceivedTransaction();  //lastRXpage is equal to the page number of the last received transaction.

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(CursorX, CursorY);
  tft.println("searching wallet: ");
  tft.println(ArkAddress);

  tft.print("# of transactions in wallet: ");
  tft.println(lastRXpage);


  ConfigureNeoPixels(redgreen);

}

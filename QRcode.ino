void setupQRcode() {

#define _240x320_TFT


#ifdef  _240x320_TFT
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 32);
  tft.setTextSize(2);  //(30 pixels tall I think)
  tft.println("Generating QR Code");
#endif


  // Start time
  uint32_t dt = millis();

  //--------------------------------------------
  // Allocate memory to store the QR code.
  // memory size depends on version number
  uint8_t qrcodeData[qrcode_getBufferSize(QRcode_Version)];

  //--------------------------------------------
  //configure the text string to code
  //https://github.com/ArkEcosystem/AIPs/blob/master/AIPS/aip-13.md#simpler-syntax

  // qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "{\"a\":\"DE6os4N86ef9bba6kVGurqxmhpBHKctoxY\"}"); //dARK address
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "DE6os4N86ef9bba6kVGurqxmhpBHKctoxY");   //dARK address
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "ark:AePNZAAtWhLsGFLXtztGLAPnKm98VVC8tJ?amount=20.3");    //ARK address
  qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "dark:DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt?amount=0.3");    //dARK address 51 bytes.
  //  qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "hello pj");
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "1BGJvqAuZvr23EixA65PEe5PMLAjVTeyMn");     //bitcoin address

  //--------------------------------------------
  // Print Code Generation Time
  dt = millis() - dt;
  Serial.print("QR Code Generation Time: ");
  Serial.print(dt);
  Serial.print("\n");

  //--------------------------------------------
  //    This prints the QR code to the serial monitor as solid blocks. Each module
  //    is two characters wide, since the monospace font used in the serial monitor
  //    is approximately twice as tall as wide.
  // Top quiet zone
  Serial.print("\n\n\n\n");
  for (uint8_t y = 0; y < qrcode.size; y++) {
    // Left quiet zone
    Serial.print("        ");
    // Each horizontal module
    for (uint8_t x = 0; x < qrcode.size; x++) {
      // Print each module (UTF-8 \u2588 is a solid block)
      Serial.print(qrcode_getModule(&qrcode, x, y) ? "\u2588\u2588" : "  ");
    }
    Serial.print("\n");
  }
  // Bottom quiet zone
  Serial.print("\n\n\n\n");


#ifdef _128x64_OLED
  //--------------------------------------------
  //display generation time to OLED display
  u8g2.drawStr(0, 30, "Generation Time(ms)"); // write something to the internal memory
  u8g2.setCursor(0, 50);
  u8g2.print(dt);             // display time it took to generate code
  u8g2.sendBuffer();
  delay(3000);
#endif



#ifdef  _240x320_TFT
  tft.setCursor(0, 60);
  tft.setTextSize(1);  //(20 pixels tall I think)
  tft.println();
  tft.print("Generation Time(ms)");
  tft.println(dt);
  delay(3000);
#endif

#ifdef _128x64_OLED
  //--------------------------------------------
  //Turn on all pixels
  for (uint8_t y = 0; y < 63; y++) {
    for (uint8_t x = 0; x < 127; x++) {
      u8g2.setDrawColor(1);       //change to 0 to make QR code with black background
      u8g2.drawPixel(x, y);
    }
  }
#endif

#ifdef  _240x320_TFT
  //--------------------------------------------
  //Turn on all pixels so screen has a white background
  tft.fillScreen(ILI9341_WHITE);
#endif

  //--------------------------------------------
  //this will put the QRcode in the middle of the screen
  //uint8_t x0 = (Lcd_X - qrcode.size) / 2;
  //uint8_t y0 = (Lcd_Y - qrcode.size) / 2;



#ifdef _128x64_OLED
  uint8_t x0 = 20;
  uint8_t y0 =  2;   //

  //--------------------------------------------
  //display QRcode
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {

      if (qrcode_getModule(&qrcode, x, y) == 0) {     //change to == 1 to make QR code with black background
        u8g2.setDrawColor(1);

#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        u8g2.drawPixel(x0 + x, y0 + y);
#endif

      } else {
        u8g2.setDrawColor(0);


#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        u8g2.drawPixel(x0 + x, y0 + y);
#endif

      }

    }
  }
  u8g2.sendBuffer();

#endif




#ifdef  _240x320_TFT
  //this will put the QRcode on the top left corner
  uint8_t x0 = 20;
  uint8_t y0 =  20;   //
  //--------------------------------------------
  //display QRcode
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {

      if (qrcode_getModule(&qrcode, x, y) == 0) {     //change to == 1 to make QR code with black background


#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y, ILI9341_WHITE);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y, ILI9341_WHITE);
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1, ILI9341_WHITE);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1, ILI9341_WHITE);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        tft.drawPixel(x0 + x, y0 + y, ILI9341_WHITE);
#endif

      } else {


#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y, ILI9341_BLACK);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y, ILI9341_BLACK);
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1, ILI9341_BLACK);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1, ILI9341_BLACK);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        tft.drawPixel(x0 + x, y0 + y, ILI9341_BLACK);
#endif
      }

    }
  }

#endif




}

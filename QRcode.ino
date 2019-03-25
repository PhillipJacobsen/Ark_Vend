/********************************************************************************
  This file contains functions used for generating QRcodes
********************************************************************************/


void setupQRcode() {


  //  tft.fillScreen(ILI9341_BLACK);
  //  tft.setTextColor(ILI9341_WHITE);
  // tft.setCursor(0, 32);
  //  tft.setTextSize(2);  //(30 pixels tall I think)
 
  //tft.println("Generating QR Code");
  //tft.println("Pay 0.3 dArk by scanning the QR code");

  //--------------------------------------------
  // Allocate memory to store the QR code.
  // memory size depends on version number
  uint8_t qrcodeData[qrcode_getBufferSize(QRcode_Version)];



  //Serial.println(esp_random()); //this is pseudorandom when the wifi or bluetooth does not have a connection. It can be considered "random" when the radios have a connection
  //Serial.println(random(256,32768));  //arduino random function is overloaded on to esp_random();  this provides a lower and upper bound
  
//uint32_t esprandom = (random(256,32768));

  // Start time
  uint32_t dt = millis();

  //--------------------------------------------
  //configure the text string to code
  //https://github.com/ArkEcosystem/AIPs/blob/master/AIPS/aip-13.md#simpler-syntax
  // Address with label, amount and vendor field
  // dark:DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w?label=PJ&amount=0.3&vendorField=ArkVend%123456789


  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "ark:AePNZAAtWhLsGFLXtztGLAPnKm98VVC8tJ?amount=10.3");    //ARK address
  //  qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "dark:DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w?amount=0.3");    //dARK address 51 bytes.
//  qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "dark:DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w?label=ArkVend&amount=0.3&vendorField=color red");    //dARK address 51 bytes.


int esprandom = (random(256,32768));
String str = String(esprandom);     //int is now a string
char charBuf[6];
str.toCharArray(charBuf,6); 
char VendorID[150];

strcpy(VendorID,"dark:");
strcat(VendorID,QRcodeArkAddress);
strcat(VendorID,"?label=ArkVend&amount=0.3&vendorField=ArkVend_");
strcat(VendorID,charBuf);
Serial.println(VendorID);

tft.print("VendorField: ArkVend_");
tft.println(charBuf);


qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, VendorID );    //dARK address 51 bytes.



  //--------------------------------------------
  //Turn on all pixels so screen has a white background
//  tft.fillScreen(ILI9341_WHITE);
  //--------------------------------------------
//  Turn on all pixels in the lower portion of the screen
   for (uint16_t y = 200; y < 320; y++) {
     for (uint16_t x = 0; x < 170; x++) {
       tft.drawPixel(x,y,ILI9341_WHITE);
     }
   }

  //--------------------------------------------
  //this will put the QRcode in the middle of the screen

  //uint8_t y0 = (Lcd_Y - qrcode.size) / 2;



  //this will put the QRcode on the top left corner
    uint16_t x0 = 30;
  //uint16_t x0 = (Lcd_X - qrcode.size) / 4; //this will put the QRcode centered horizontally (DOES NOT WORK CORRECTLY!!!!)
  
  uint16_t y0 =  210;   //
  //--------------------------------------------
  //display QRcode
  for (uint16_t y = 0; y < qrcode.size; y++) {
    for (uint16_t x = 0; x < qrcode.size; x++) {

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




}

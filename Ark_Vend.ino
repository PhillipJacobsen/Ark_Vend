/********************************************************************************
    Proof of Concept Project
	This projects illustrates a vending machine that accepts Ark Cryptocurrency for payment.

  NOT COMPLETE

    Ark_Vend.ino
    2019 @phillipjacobsen

    Program Features:
    This program has been tested with ESP32 Adafruit Huzzah however it should also work with ESP8266 modules with minor changes to hardware connections and wifi libraries.
    This sketch will use the ARK Cpp-Client API to interact with an Ark V2 Devnet node.
    Ark Cpp Client available from Ark Ecosystem <info@ark.io>
    Ark API documentation:  https://docs.ark.io/sdk/clients/usage.html

    Electronic Hardware Peripherals:
		Adafruit TFT FeatherWing 2.4" 320x240 Touchscreen
    Adafruit NeoPixels

Description of the current program flow.  status/debug info is also regularly sent to serial terminal
1. configure peripherals
  -setup wifi and display connection status and IP address on TFT Screen
  -setup time sync with NTP server and display current time
  -check to see if Ark node is synced and display status
2. search through all received transactions on wallet. Wallet address is displayed
  -"searching wallet + page#" will be displayed. text will toggle between red/white every received transaction
3. # of transactions in wallet will be displayed
4. QR code is displayed 

********************************************************************************/

/********************************************************************************
               Electronic Hardware Requirements and Pin Connections
   ESP32 Adafruit Huzzah
      Source: https://www.adafruit.com/product/3315


    TFT FeatherWing 2.4" 320x240 Touchscreen
      Touchscreen is designed to plug direction into ESP32 Huzzah module
		TFT_CS 	-> pin #15
		TFT_DC 	-> pin #33
		RT 		-> pin #32
		SD		-> pin #14
		SCK		-> SCK
		MISO	-> MISO
		MOSI	-> MOSI


		NEOPIXEL-> pin #12
		VCC -> 3.3V
		GND

********************************************************************************/


/********************************************************************************
                              Library Requirements
********************************************************************************/

/********************************************************************************

  // I NEED TO UPDATE COMMENTS FOR ESP32 module. These comments are for ESP8266

    Makuna NeoPixel Library - optimized for ESP8266
      Available through Arduino Library Manager however development is done using lastest Master Branch on Github
      https://github.com/Makuna/NeoPixelBus/

      This library is optimized to use the DMA on the ESP8266 for minimal cup usage. The standard Adafruit library has the potential to interfere with the
      WiFi processing done by the low level SDK
      NeoPixelBus<FEATURE, METHOD> strip(pixelCount, pixelPin);
       NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(16);
      On the ESP8266 the Neo800KbpsMethod method will use this underlying method: NeoEsp8266Dma800KbpsMethod
      The NeoEsp8266Dma800KbpsMethod is the underlying method that gets used if you use Neo800KbpsMethod on Esp8266 platforms. There should be no need to use it directly.
      The NeoEsp8266Dma800KbpsMethod only supports the RDX0/GPIO3 pin. The Pin argument is omitted. See other esp8266 methods below if you don't have this pin available.
      This method uses very little CPU for actually sending the data to NeoPixels but it requires an extra buffer for the DMA to read from.
      Thus there is a trade off of CPU use versus memory use. The extra buffer needed is four times the size of the primary pixel buffer.
       It also requires the use of the RDX0/GPIO3 pin. The normal feature of this pin is the "Serial" receive.
      Using this DMA method will not allow you to receive serial from the primary Serial object; but it will not stop you from sending output to the terminal program of a PC
      Due to the pin overlap, there are a few things to take into consideration.
      First, when you are flashing the Esp8266, some LED types will react to the flashing and turn on.
      This is important if you have longer strips of pixels where the power use of full bright might exceed your design.
      Second, the NeoPixelBus::Begin() MUST be called after the Serial.begin().
      If they are called out of order, no pixel data will be sent as the Serial reconfigured the RDX0/GPIO3 pin to its needs.
********************************************************************************/
#include <NeoPixelBus.h>
#define PixelPin 12        //Neopixel Data Pin  connected to DMA
#define PixelCount 8       //Length of Neopixel Strand
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin); //default on ESP8266 is to use the D9(GPIO3,RXD0) pin with DMA.

#define colorSaturation 128
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor off(0, 0, 0);
RgbColor redgreen(colorSaturation, colorSaturation, 0);
RgbColor greenblue(0, colorSaturation, colorSaturation);
RgbColor black(0);


/********************************************************************************
    QRCode by Richard Moore version 0.0.1
      Available through Arduino Library Manager
        https://github.com/ricmoo/QRCode

    The QR code data encoding algorithm defines a number of 'versions' that increase in size and store increasing amounts of data.
    The version (size) of a generated QR code depends on the amount of data to be encoded.
    Increasing the error correction level will decrease the storage capacity due to redundancy pixels being added.

    If you have a ? in your QR text then I think the QR code operates in "Byte" mode.
********************************************************************************/
#include "qrcode.h"
const int QRcode_Version = 8;   //  set the version (range 1->40)
const int QRcode_ECC = 0;       //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
#define _QR_doubleSize          //  This will double the display size of the generated code. Every pixel becomes a 4x4 square.

QRCode qrcode;                  // Create the QR code


/********************************************************************************
  GFX libraries for the Adafruit ILI9341 2.4" 240x320 TFT FeatherWing
  ----> http://www.adafruit.com/products/3315
********************************************************************************/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#ifdef ESP32
#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#endif

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
#include <Fonts/FreeSans9pt7b.h>        //add custom fonts here

#define Lcd_X  240       //configure your screen dimensions.  We aren't using an LCD for this project so I should rename to something more generic               
#define Lcd_Y  320       //configure your screen dimensions    

int CursorX = 0;         //used to store current cursor position of the display
int CursorY = 0;         //used to store current cursor position of the display


/********************************************************************************
   Ark Client Library
    Available through Arduino Library Manager
    https://github.com/ArkEcosystem/cpp-client
********************************************************************************/
#include <arkClient.h>
/**
    This is the IP address of an Ark Node
    Specifically, this is a Devnet V2 Node IP
    You can find more peers here: https://github.com/ArkEcosystem/peers

    The Public API port for the V2 Ark network is '4003'
*/
const char* peer = "167.114.29.55";
int port = 4003;

const char* ArkAddress = "DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w";
const char* ArkPublicKey = "029b2f577bd7afd878b258d791abfb379a6ea3c9436a73a77ad6a348ad48a5c0b9";

/**
   This is how you define a connection while speficying the API class as a 'template argument'
   You instantiate a connection by passing a IP address as a 'c_string', and the port as an 'int'.
*/
Ark::Client::Connection<Ark::Client::Api> connection(peer, port);
/**/


//I think a structure here for transaction details would be better form
//I need to do some work here to make things less hacky
//struct transactionDetails {
//   const char*  id;
//   int amount;
//   const char* senderAddress;
//   const char* vendorField;
//};

//--------------------------------------------
// these are used to store the received transation details returned from wallet search
const char*  id;            //transaction ID
int amount;                 //transactions amount
const char* senderAddress;  //transaction address of sender
const char* vendorField;    //vendor field

int lastRXpage;             //page number of the last received transaction in wallet
int searchRXpage;           //page number that is used for wallet search



/********************************************************************************
   Arduion Json Libary
    Available through Arduino Library Manager
    Data returned from Ark API is in JSON format.
    This libary is used to parse and deserialize the reponse
********************************************************************************/
#include <ArduinoJson.h>


/********************************************************************************
  Time Library
  required for internal clock to syncronize with NTP server.
  I need to do a bit more work in regards to Daylight savings time and the periodic sync time with the NTP service after initial syncronization
********************************************************************************/
#include "time.h"
//#include <TimeLib.h>    //https://github.com/PaulStoffregen/Time
int timezone = -6;        //set your timezone MST
int dst = 0;              //To enable Daylight saving time set it to 3600. Otherwise, set it to 0. This doesn't seem to work.


/********************************************************************************
  WiFi Library
  If using the ESP8266 I believe you will need to #include <ESP8266WiFi.h> instead of WiFi.h
********************************************************************************/
#include <WiFi.h>
//--------------------------------------------
//This is your WiFi network parameters that you need to configure
const char* ssid = "TELUS0183";
const char* password = "6z5g4hbdxi";
//const char* ssid = "xxxxxxxxxx";
//const char* password = "xxxxxxxxxx";



/********************************************************************************
  Function prototypes
  Arduino IDE normally does its automagic here and creates all the function prototypes for you.
  We have put functions in other files so we need to manually add some prototypes as the automagic doesn't work correctly
********************************************************************************/
void setup();

/********************************************************************************
  End Function Prototypes
********************************************************************************/


/********************************************************************************
  MAIN LOOP
********************************************************************************/
void loop() {

  //look for new transactions to arrive in wallet.
  Serial.print("\n\n\nLooking for new transaction\n");


  searchRXpage = lastRXpage + 1;
  if ( searchReceivedTransaction(ArkAddress, searchRXpage, id, amount, senderAddress, vendorField) ) {
    Serial.print("Page: ");
    Serial.println(searchRXpage);
    Serial.print("Transaction id: ");
    Serial.println(id);
    Serial.print("Amount(Arktoshi): ");
    Serial.println(amount);
    Serial.print("Amount(Ark): ");
    Serial.println(float(amount) / 100000000, 8);
    Serial.print("Sender address: ");
    Serial.println(senderAddress);
    Serial.print("Vendor Field: ");
    Serial.println(vendorField);

    //   if (vendorField == "LED ON") {
    if  (strcmp(vendorField, "led on") == 0) {

      //    u8g2.clearBuffer();
      //    u8g2.drawStr(0, 12, "Turn LED ON");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      //   u8g2.sendBuffer();                    // transfer internal memory to the display
    }
    // else if (vendorField == "led off") {
    else if  (strcmp(vendorField, "led off") == 0) {
      //    u8g2.clearBuffer();
      //    u8g2.drawStr(0, 12, "LEDs OFF");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      //    u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(off);
    }
    //  else if (vendorField == "color red") {
    else if  (strcmp(vendorField, "color red") == 0) {
      //   u8g2.clearBuffer();
      //   u8g2.drawStr(0, 12, "Glowing Red");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      //   u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(red);
    }
    //    else if (vendorField == "color green") {
    else if  (strcmp(vendorField, "color green") == 0) {
      //   u8g2.clearBuffer();
      //   u8g2.drawStr(0, 12, "Glowing green");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      //   u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(green);
    }
    //     else if (vendorField == "color blue") {
    else if  (strcmp(vendorField, "color blue") == 0) {
      //   u8g2.clearBuffer();
      //   u8g2.drawStr(0, 12, "Glowing blue");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      //    u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(blue);
    }

    else {
      //   u8g2.clearBuffer();
      //   u8g2.setCursor(0, 12);
      Serial.print("Unspecified VendorField: ");
      //   u8g2.print(vendorField);           // display unknown vendor field
      //   u8g2.sendBuffer();                    // transfer internal memory to the display

    }
    lastRXpage++;
  }


  //no new transaction found.
  else {

  }

  setupQRcode();

  delay(3000);
  esp_deep_sleep_start();

};

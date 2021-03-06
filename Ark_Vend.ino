/********************************************************************************
    Proof of Concept Vending Maching Project
	This projects illustrates a vending machine that accepts Ark Cryptocurrency (or a forked bridgechain) for payment.

    Ark_Vend.ino
    2019 @phillipjacobsen

    Program Features:
    This program has been tested with ESP32 Adafruit Huzzah however it should also work with ESP8266 modules with minor changes to hardware connections and wifi libraries.
    This sketch uses the ARK Cpp-Client API to interact with an Ark V2 Devnet node.
    Ark Cpp Client available from Ark Ecosystem <info@ark.io>
    Ark API documentation:  https://docs.ark.io/sdk/clients/usage.html

    Electronic Hardware Peripherals:
		Adafruit TFT FeatherWing 2.4" 320x240 Touchscreen
    Continuous Servo Motor
    Adafruit NeoPixels

  Description of the current program flow.  status/debug info is also regularly sent to serial terminal
  1. configure peripherals
  -setup wifi and display connection status and IP address on TFT Screen
  -setup time sync with NTP server and display current time
  -check to see if Ark node is synced and display status
  2. search through all received transactions on wallet. Wallet address is displayed
    -"searching wallet + page#" will be displayed. text will toggle between red/white every received transaction
  3. # of transactions in wallet will be displayed
  4. User Interface with 3 buttons are displayed(only 1 button currently functions("M&M").
  5. When user selects M&M's a QRcode is displayed along with a timeout displayed.
  4. QR code includes price, address and Vendor field = "ArkVend_(random number)" when scanned by Ark mobile wallet.
  5. wallet waits for transaction with vendor field to be received.
  6. If payment is received then it will indicated success and display in green text "Payment: ArkVend_(random_number)"
    if incorrect payment is received then received transaction will be ignored.
  7. Back to Step 4

********************************************************************************/
// conditional assembly
//
#define JAKEIOT   //this configures system for my custom bridgechain. If undefined then system will be configured for Ark Devnet.


/********************************************************************************
               Electronic Hardware Requirements and Pin Connections
    ESP32 Adafruit Huzzah
      Source: https://www.adafruit.com/product/3213
      https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

    TFT FeatherWing 2.4" 320x240 Touchscreen
      Source: https://www.adafruit.com/product/3315
      Touchscreen is designed to plug direction into ESP32 Huzzah module
    		TFT_CS 	-> pin #15
    		TFT_DC 	-> pin #33
    		RT 		-> pin #32
    		SD		-> pin #14
    		SCK		-> SCK
    		MISO	-> MISO
    		MOSI	-> MOSI

   Continuous servo - Connected to candy machine dial
       servo1Pin -> pin 21
       VCC  -> BAT
       GND  -> GND

  //NEOPIXELS NOT CONNECTED YET.
		NEOPIXEL-> pin
		VCC -> 3.3V
		GND

********************************************************************************/


/********************************************************************************
                              Library Requirements
********************************************************************************/

/********************************************************************************
  Servo control library
  // Recommended pins for attaching servo include 2,4,12-19,21-23,25-27,32-33
********************************************************************************/
#include <ESP32Servo.h>
Servo servo1;       //create servo object
// adjust the following according to motor specifications
int minUs = 540;    //default value if not specified: 540
int maxUs = 2400;   //default value if not specified: 2400

int servo1Pin = 21;
int pos = 0;      // position in degrees (or speed for continuous servo)

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
const int QRcode_Version = 8;   // set the version (range 1->40)
const int QRcode_ECC = 0;       // set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
#define _QR_doubleSize          // This will double the display size of the generated code. Every pixel becomes a 2x2 square.

QRCode qrcode;                  // Create the QR code object


/********************************************************************************
  GFX libraries for the Adafruit ILI9341 2.4" 240x320 TFT FeatherWing display + touchscreen
  ----> http://www.adafruit.com/products/3315
********************************************************************************/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>       //hardware specific library for display
#include <Adafruit_STMPE610.h>      //hardware specific library for the touch sensor

#ifdef ESP32
#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#endif
#ifdef ESP8266
#define STMPE_CS 16
#define TFT_CS   0
#define TFT_DC   15
#define SD_CS    2
#endif


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);    //create TFT display object
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);         //create Touchscreen object
#include <Fonts/FreeSans9pt7b.h>        //add custom fonts here

// This is default calibration data for the raw touch data to the screen coordinates
//#define TS_MINX 3800
//#define TS_MAXX 100
//#define TS_MINY 100
//#define TS_MAXY 3750

//my calibrated touchscreen data
#define TS_MINX 3800  //affect left side of screen
#define TS_MAXX 250   //affect right side of screen.
#define TS_MINY 205   //
#define TS_MAXY 3750

//  use these tools to get 16bit hex color definitions  "5-6-5 16-bit mode"
//  http://www.barth-dev.de/online/rgb565-color-picker/
//  http://henrysbench.capnfatz.com/henrys-bench/arduino-adafruit-gfx-library-user-guide/arduino-16-bit-tft-rgb565-color-basics-and-selection/

#define ArkRed 0xF1A7       // rgb(241, 55, 58)
#define ArkLightRed 0xFCD3  // rgb(248, 155, 156)

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define Lcd_X  240       //configure your screen dimensions.  We aren't using an LCD for this project so I should rename to something more generic               
#define Lcd_Y  320       //configure your screen dimensions    

int CursorX = 0;         //used to store current cursor position of the display
int CursorY = 0;         //used to store current cursor position of the display


/********************************************************************************
   Ark Client Library (version 1.2.0)
    Available through Arduino Library Manager
    https://github.com/ArkEcosystem/cpp-client
********************************************************************************/
#include <arkClient.h>
/**
    This is where you define the IP address of an Ark Node (Or bridgechain node).
    You can find more Ark Mainnet and Devnet peers here: https://github.com/ArkEcosystem/peers
    The Public API port for the V2 Ark network is '4003'
*/


#ifdef JAKEIOT
const char* peer = "138.197.140.234";  //jakeIOT Testnet Peer
#else
const char* peer = "167.114.29.55";  //Ark Devnet Peer
#endif

int port = 4003;

//Wallet Address on bridgechain
#ifdef JAKEIOT
const char* ArkAddress = "AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx";   //jakeIOT testnet address
char QRcodeArkAddress[] = "AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx";   //jakeIOT testnet address
const char* ArkPublicKey = "0274a45e0c087b2bc2f68a15a6aa5724b73db46b665a992495a1375c00c29d20a2";       //jakeIOT testnet public key

//Wallet Address on Ark Devnet
#else
const char* ArkAddress = "DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt";   //Ark Devnet address
char QRcodeArkAddress[] = "DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt";   //Ark Devnet address
const char* ArkPublicKey = "029b2f577bd7afd878b258d791abfb379a6ea3c9436a73a77ad6a348ad48a5c0b9";       //Ark Devnet public key
//char *QRcodeArkAddress = "DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w";  //compiler may place this string in a location in memory that cannot be modified 
#endif

char VendorID[64];

//define the payment timeout in ms
#define PAYMENT_WAIT_TIME 90000

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
   Arduino Json Libary - works with Version5.  NOT compatible with Version6
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
int timezone = -6;        //set timezone:  MST
int dst = 0;              //To enable Daylight saving time set it to 3600. Otherwise, set it to 0. Not sure if this works.


unsigned long timeNow;  //variable used to hold current millis() time.
unsigned long payment_Timeout;
unsigned long timeAPIfinish;  //variable used to measure API access time
unsigned long timeAPIstart;  //variable used to measure API access time

/********************************************************************************
  WiFi Library
  If using the ESP8266 I believe you will need to #include <ESP8266WiFi.h> instead of WiFi.h
********************************************************************************/
#include <WiFi.h>
//--------------------------------------------
//This is your WiFi network parameters that you need to configure

const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxxx";




/********************************************************************************
  State Machine

********************************************************************************/
enum VendingMachineStates {DRAW_HOME, WAIT_FOR_USER, WAIT_FOR_PAY, VEND_ITEM};   //The five possible states of the Vending state machine
VendingMachineStates vmState = DRAW_HOME;   //initialize the starting state.


/********************************************************************************
  Function prototypes
  Arduino IDE normally does its automagic here and creates all the function prototypes for you.
  We have put functions in other files so we need to manually add some prototypes as the automagic doesn't work correctly
********************************************************************************/
void setup();
int searchReceivedTransaction(const char *const address, int page, const char* &id, int &amount, const char* &senderAddress, const char* &vendorField );

//NeoPixels not yet connected.
//void ConfigureNeoPixels(RgbColor color);

void ArkVendingMachine();
/********************************************************************************
  End Function Prototypes
********************************************************************************/


/********************************************************************************
  MAIN LOOP
********************************************************************************/
void loop() {
  ArkVendingMachine();
}

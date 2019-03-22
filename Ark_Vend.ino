/********************************************************************************
    Demo IOT project that interacts with the Ark blockchain

    ArkJsonParse.ino
    2019 @phillipjacobsen

    Program Features:
    This program has been tested with ESP32 Adafruit Huzzah however it should also work with ESP8266 modules with minor changes to hardward connections and wifi libraries.
    This sketch will use the ARK Cpp-Client API to interact with an Ark V2 Devnet node.
    Ark Cpp Client available from Ark Ecosystem <info@ark.io>
    Ark API documentation:  https://docs.ark.io/sdk/clients/usage.html

    Status information is shown on small 0.96 128x64 OLED display



    Encodes text string into a matrix representing the QR Code
      -program does not check length of text to make sure the QRcode version is able to generate it
    Displays the resulting code on the OLED display
    The QRcode will be doubled in size so that each QR code pixel shows up as four pixels on the OLED display.
    Default is lit background with Black QRcode. Inverse can also be displayed however codes seem to scan much better with lit background.
********************************************************************************/

/********************************************************************************
               Electronic Hardware Requirements and Pin Connections
   ESP32 Adafruit Huzzah
      Source:


    0.96 I2C 128x64 OLED display
      Source:    https://www.aliexpress.com/store/product/Free-shipping-Matrix-360-NAND-Programmer-MTX-USB-SPI-Flasher-V1-0-For-XBOX-360-Game/334970_1735255916.html?spm=2114.12010612.0.0.73fe44c9mSwirS
      Pins cannot be remapped when using DMA mode
      SDA ->SDA
      SCL ->SCL
      VCC -> 3.3V
      GND ->GND
********************************************************************************/

/********************************************************************************

  // I NEED TO UPDATE COMMENTS FOR ESP32 module

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
  U8g2lib Monochrome Graphics Display Library
    Available through Arduino Library Manager
    https://github.com/olikraus/u8g2

  Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
  Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
  U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
********************************************************************************/
#include <U8g2lib.h>

// U8g2 Constructor List for Frame Buffer Mode.
// This uses the Hardware I2C peripheral on ESP32 with DMA interface
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define Lcd_X  128
#define Lcd_Y  64



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


const char*  id;
int amount;
const char* senderAddress;
const char* vendorField;

int lastRXpage;

/**
   This is how you define a connection while speficying the API class as a 'template argument'
   You instantiate a connection by passing a IP address as a 'c_string', and the port as an 'int'.
*/
Ark::Client::Connection<Ark::Client::Api> connection(peer, port);
/**/


/********************************************************************************
   Arduion Json Libary
    Available through Arduino Library Manager
********************************************************************************/
#include <ArduinoJson.h>


/********************************************************************************

********************************************************************************/
#include "time.h"
//#include <TimeLib.h>    //https://github.com/PaulStoffregen/Time
int timezone = -6;      //MST
int dst = 0;         //To enable Daylight saving time set it to 3600. Otherwise, set it to 0. This doesn't seem to work.


/********************************************************************************
   WiFi Library

********************************************************************************/
#include <WiFi.h>

/* This is the WiFi network you'd like your board to connect to. */
//const char* ssid = "TELUS6428";
//const char* password = "3mmkgc9gn2";
const char* ssid = "TELUS0183";
const char* password = "6z5g4hbdxi";
/**/



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


/********************/
void checkArkNodeStatus() {
  /**
     The following method can be used to get the Status of a Node.

     This is equivalant to calling '167.114.29.49:4003/api/v2/node/status'

     The response should be a json-formatted object
     The "pretty print" version would look something like this:

     {
      "data": {
        "synced": true,
        "now": 1178395,
        "blocksCount": 0
       }
     }
  */
  const auto nodeStatus = connection.api.node.status();
  Serial.print("\nNode Status: ");
  Serial.println(nodeStatus.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.

  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 50;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(nodeStatus.c_str());

  JsonObject& data = root["data"];
  bool data_synced = data["synced"]; // true
  //long data_now = data["now"]; // 1178395
  //int data_blocksCount = data["blocksCount"]; // 0

  if (data_synced) {
    Serial.print("\nNode is Synced: ");
  }
  else {
    Serial.print("\nNode is NOT Synced: ");
  }
  /****************************************/
}

//struct transactionDetails {
//   const char*  id;
//   int amount;
//   const char* senderAddress;
//   const char* vendorField;
//};

/**
    The response is a json-formatted object
   The "pretty print" version would look something like this:
  {
  "meta": {
      "count": 1,
      "pageCount": 16,
      "totalCount": 16,
      "next": "/api/v2/transactions/search?page=2&limit=1",
      "previous": null,
      "self": "/api/v2/transactions/search?page=1&limit=1",
      "first": "/api/v2/transactions/search?page=1&limit=1",
      "last": "/api/v2/transactions/search?page=16&limit=1"
  },
  "data": [
      {
          "id": "cf1aad5e14f4edb134269e0dc7f9457093f458a9785ea03914effa3932e7dffe",
          "blockId": "1196453921719185829",
          "version": 1,
          "type": 0,
          "amount": 1000000000,
          "fee": 1000000,
          "sender": "DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt",
          "recipient": "DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w",
          "signature": "3045022100d55a219edd8690e89a368399084aa8a468629b570e332e0e618e0af83b1a474602200f57b67e628389533b78db915b1139d8529fee133b9198576b30b98ea5a1ce28",
          "confirmations": 21771,
          "timestamp": {
              "epoch": 62689306,
              "unix": 1552790506,
              "human": "2019-03-17T02:41:46.000Z"
          }
      }
  ]
  }

*/


//void searchReceivedTransaction() {
//provide Receive address and page parameter
int searchReceivedTransaction(const char *const address, int page, const char* &id, int &amount, const char* &senderAddress, const char* &vendorField ) {


  //const std::map<std::string, std::string>& body_parameters, int limit = 5,
  std::string vendorFieldHexString;
  vendorFieldHexString = "6964647955";
  //std::string transactionSearchResponse = connection.api.transactions.search( {{"vendorFieldHex", vendorFieldHexString}, {"orderBy", "timestamp:asc"} },1,1);
  std::string transactionSearchResponse = connection.api.transactions.search( {{"recipientId", address}, {"orderBy", "timestamp:asc"} }, 1, page);

  Serial.print("\nSearch Result Transactions: ");
  Serial.println(transactionSearchResponse.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.


  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(11) + 810;
  DynamicJsonBuffer jsonBuffer(capacity);

  //const char* json = "{\"meta\":{\"count\":1,\"pageCount\":16,\"totalCount\":16,\"next\":\"/api/v2/transactions/search?page=2&limit=1\",\"previous\":null,\"self\":\"/api/v2/transactions/search?page=1&limit=1\",\"first\":\"/api/v2/transactions/search?page=1&limit=1\",\"last\":\"/api/v2/transactions/search?page=16&limit=1\"},\"data\":[{\"id\":\"cf1aad5e14f4edb134269e0dc7f9457093f458a9785ea03914effa3932e7dffe\",\"blockId\":\"1196453921719185829\",\"version\":1,\"type\":0,\"amount\":1000000000,\"fee\":1000000,\"sender\":\"DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt\",\"recipient\":\"DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w\",\"signature\":\"3045022100d55a219edd8690e89a368399084aa8a468629b570e332e0e618e0af83b1a474602200f57b67e628389533b78db915b1139d8529fee133b9198576b30b98ea5a1ce28\",\"confirmations\":21771,\"timestamp\":{\"epoch\":62689306,\"unix\":1552790506,\"human\":\"2019-03-17T02:41:46.000Z\"}}]}";

  JsonObject& root = jsonBuffer.parseObject(transactionSearchResponse.c_str());


  JsonObject& meta = root["meta"];
  int meta_count = meta["count"]; // 1
  int meta_pageCount = meta["pageCount"]; // 16
  int meta_totalCount = meta["totalCount"]; // 16
  const char* meta_next = meta["next"]; // "/api/v2/transactions/search?page=3&limit=1"
  const char* meta_previous = meta["previous"]; // "/api/v2/transactions/search?page=1&limit=1"
  const char* meta_self = meta["self"]; // "/api/v2/transactions/search?page=2&limit=1"
  const char* meta_first = meta["first"]; // "/api/v2/transactions/search?page=1&limit=1"
  const char* meta_last = meta["last"]; // "/api/v2/transactions/search?page=16&limit=1"

  JsonObject& data_0 = root["data"][0];
  const char* data_0_id = data_0["id"]; // "8990a1c7772731c1cc8f2671f070fb7919d1cdac54dc5de619447a6e88899585"
  const char* data_0_blockId = data_0["blockId"]; // "3154443675765724828"
  int data_0_version = data_0["version"]; // 1
  int data_0_type = data_0["type"]; // 0
  int data_0_amount = data_0["amount"]; // 1
  long data_0_fee = data_0["fee"]; // 10000000
  const char* data_0_sender = data_0["sender"]; // "DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt"
  const char* data_0_recipient = data_0["recipient"]; // "DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w"
  const char* data_0_signature = data_0["signature"]; // "30450221008ef28fe9020e1dd26836fc6a1c4d576c022bde9cc14278bc4d6779339c080f7902204946a3c3b2b37fbe4767a9e3d7cb4514faf194c89595cdb280d74af52a76ad21"
  const char* data_0_vendorField = data_0["vendorField"]; // "e2620f612a9b9abb96fee4d03391c51e597f8ffbefd7c8db2fbf84e6a5e26c99"
  long data_0_confirmations = data_0["confirmations"]; // 74713

  JsonObject& data_0_timestamp = data_0["timestamp"];
  long data_0_timestamp_epoch = data_0_timestamp["epoch"]; // 62262442
  long data_0_timestamp_unix = data_0_timestamp["unix"]; // 1552363642
  const char* data_0_timestamp_human = data_0_timestamp["human"]; // "2019-03-12T04:07:22.000Z"

  if (data_0_id == nullptr) {
    Serial.print("\n data_0_id is null");
    return 0;
  }
  else {
    Serial.print("\n data_0_id is available");
    id = data_0_id;
    amount = data_0_amount;
    senderAddress = data_0_sender;
    vendorField = data_0_vendorField;
  }
  return 1;

}





void searchTransaction() {
  //const std::map<std::string, std::string>& body_parameters, int limit = 5,
  std::string vendorFieldHexString;
  vendorFieldHexString = "6964647955";
  //std::string transactionSearchResponse = connection.api.transactions.search( {{"vendorFieldHex", vendorFieldHexString}, {"orderBy", "timestamp:asc"} },1,1);
  std::string transactionSearchResponse = connection.api.transactions.search( {{"recipientId", ArkAddress}, {"orderBy", "timestamp:asc"} }, 1, 1);

  Serial.print("\nSearch Result Transactions: ");
  Serial.println(transactionSearchResponse.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.
}





//gets received transaction
int getReceivedTransaction(const char *const address, int page, const char* &id, int &amount, const char* &senderAddress, const char* &vendorField ) {
  /********************/

  /**
     This method can be used to get a list of all received transactions of a wallet
     The '2' and '1' refer to the pagination (e.g. response limit and how many pages)
     NOTE!  the returned COUNT parameters are just approximate due to high processing requirements of node CPU
     "Yes, the totalCount that is returned is an estimate, so it could be inaccurate. A proper usage of that API is to cycle through the pages until a page with next: null is reached."
     https://github.com/ArkEcosystem/core/issues/2110
     https://github.com/ArkEcosystem/core/issues/1903

     This is equivalant to calling http://167.114.29.55:4003/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=1&limit=1'

     The response should be a json-formatted object
     The "pretty print" version would look something like this:

    {
    {
    "meta": {
        "count": 1,
        "pageCount": 16,
        "totalCount": 16,
        "next": "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=2&limit=1",
        "previous": null,
        "self": "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=1&limit=1",
        "first": "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=1&limit=1",
        "last": "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=16&limit=1"
    },
    "data": [
        {
            "id": "8990a1c7772731c1cc8f2671f070fb7919d1cdac54dc5de619447a6e88899585",
            "blockId": "3154443675765724828",
            "version": 1,
            "type": 0,
            "amount": 1,
            "fee": 10000000,
            "sender": "DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt",
            "recipient": "DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w",
            "signature": "30450221008ef28fe9020e1dd26836fc6a1c4d576c022bde9cc14278bc4d6779339c080f7902204946a3c3b2b37fbe4767a9e3d7cb4514faf194c89595cdb280d74af52a76ad21",
            "vendorField": "e2620f612a9b9abb96fee4d03391c51e597f8ffbefd7c8db2fbf84e6a5e26c99",
            "confirmations": 43054,
            "timestamp": {
                "epoch": 62262442,
                "unix": 1552363642,
                "human": "2019-03-12T04:07:22.000Z"
            }
        }
    ]
    }

  */
  const auto ReceivedWalletTransactions = connection.api.wallets.transactionsReceived(address, 1, page); //(Address,limit,page)

  Serial.print("\nReceived Wallet Transaction: ");
  Serial.println(ReceivedWalletTransactions.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.


  //const char* json = "{\"meta\":{\"count\":1,\"pageCount\":16,\"totalCount\":16,\"next\":\"/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=2&limit=1\",\"previous\":null,\"self\":\"/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=1&limit=1\",\"first\":\"/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=1&limit=1\",\"last\":\"/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=16&limit=1\"},\"data\":[{\"id\":\"8990a1c7772731c1cc8f2671f070fb7919d1cdac54dc5de619447a6e88899585\",\"blockId\":\"3154443675765724828\",\"version\":1,\"type\":0,\"amount\":1,\"fee\":10000000,\"sender\":\"DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt\",\"recipient\":\"DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w\",\"signature\":\"30450221008ef28fe9020e1dd26836fc6a1c4d576c022bde9cc14278bc4d6779339c080f7902204946a3c3b2b37fbe4767a9e3d7cb4514faf194c89595cdb280d74af52a76ad21\",\"vendorField\":\"e2620f612a9b9abb96fee4d03391c51e597f8ffbefd7c8db2fbf84e6a5e26c99\",\"confirmations\":43054,\"timestamp\":{\"epoch\":62262442,\"unix\":1552363642,\"human\":\"2019-03-12T04:07:22.000Z\"}}]}";

  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(12) + 1090;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(ReceivedWalletTransactions.c_str());

  JsonObject& meta = root["meta"];
  int meta_count = meta["count"]; // 1
  int meta_pageCount = meta["pageCount"]; // 16
  int meta_totalCount = meta["totalCount"]; // 16
  const char* meta_next = meta["next"]; // "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=2&limit=1"
  const char* meta_self = meta["self"]; // "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=1&limit=1"
  const char* meta_first = meta["first"]; // "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=1&limit=1"
  const char* meta_last = meta["last"]; // "/api/v2/wallets/DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w/transactions/received?page=16&limit=1"

  JsonObject& data_0 = root["data"][0];
  const char* data_0_id = data_0["id"]; // "8990a1c7772731c1cc8f2671f070fb7919d1cdac54dc5de619447a6e88899585"
  const char* data_0_blockId = data_0["blockId"]; // "3154443675765724828"
  int data_0_version = data_0["version"]; // 1
  int data_0_type = data_0["type"]; // 0
  int data_0_amount = data_0["amount"]; // 1
  long data_0_fee = data_0["fee"]; // 10000000
  const char* data_0_sender = data_0["sender"]; // "DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt"
  const char* data_0_recipient = data_0["recipient"]; // "DHy5z5XNKXhxztLDpT88iD2ozR7ab5Sw2w"
  const char* data_0_signature = data_0["signature"]; // "30450221008ef28fe9020e1dd26836fc6a1c4d576c022bde9cc14278bc4d6779339c080f7902204946a3c3b2b37fbe4767a9e3d7cb4514faf194c89595cdb280d74af52a76ad21"
  const char* data_0_vendorField = data_0["vendorField"]; // "e2620f612a9b9abb96fee4d03391c51e597f8ffbefd7c8db2fbf84e6a5e26c99"
  long data_0_confirmations = data_0["confirmations"]; // 43054

  JsonObject& data_0_timestamp = data_0["timestamp"];
  long data_0_timestamp_epoch = data_0_timestamp["epoch"]; // 62262442
  long data_0_timestamp_unix = data_0_timestamp["unix"]; // 1552363642
  const char* data_0_timestamp_human = data_0_timestamp["human"]; // "2019-03-12T04:07:22.000Z"
  /**/

  //  Serial.println(meta_count);
  //  Serial.println(data_0_id);

  //  Serial.print("\n Next = ");
  //  Serial.println(meta_next);

  // Serial.print("\n data_0_id =");
  // Serial.println(data_0_id);

  if (data_0_id == nullptr) {
    //  Serial.print("\n data_0_id is null");
    return 0;
  }
  else {
    //  Serial.print("\n data_0_id is available");
    id = data_0_id;
    amount = data_0_amount;
    senderAddress = data_0_sender;
    vendorField = data_0_vendorField;
  }
  return 1;

};


void setupWiFi() {
  u8g2.setFont(u8g2_font_9x15_tf );       // 10 pixel height  configure font used for OLED display
  u8g2.drawStr(0, 12, "WiFi Search");     // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
  u8g2.sendBuffer();                      // transfer internal memory to the display

  WiFi.begin(ssid, password); // This starts your boards connection to WiFi.
  while (WiFi.status() != WL_CONNECTED) // This will delay your board from continuing until a WiFi connection is established.
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Connected to");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
  u8g2.setCursor(0, 30);
  u8g2.print(WiFi.localIP());           // display IP address
  u8g2.sendBuffer();                    // transfer internal memory to the display
}


void setupTime() {
  //this sets the core to periodically sync the time. I don't really know how often this happens
  //I am not sure if daylight savings mode works correctly. Previously it seems like this was not working on ESP2866
  configTime(timezone * 3600, dst, "pool.ntp.org", "time.nist.gov");
  // printLocalTime();
  delay(1000);    //After connecting to WiFi network display the IP address for 2 seconds before displaying the time

  //wait for time to sync from servers
  while (time(nullptr) <= 100000) {
    delay(100);
  }

  time_t now = time(nullptr);   //get current time
  Serial.print("time is: ");
  Serial.println(ctime(&now));

  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Time is");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
  u8g2.setCursor(0, 30);
  //u8g2.setFont(u8g2_font_9x15_tf );   // 10 pixel height  configure font used for OLED display
  //u8g2.setFont(u8g2_font_6x13_te );   // 9 pixel height  configure font used for OLED display
  //u8g2.setFont(u8g2_font_ncenB08_tr); // 8 pixel height  configure font used for OLED display
  u8g2.setFont(u8g2_font_5x8_mf);       // configure font used for OLED display , font is a bit small

  u8g2.print(ctime(&now));              // display IP address
  u8g2.sendBuffer();                    // transfer internal memory to the display

}

/****************************************/

int getMostRecentReceivedTransaction() {
  Serial.print("\n\n\nHere are all the transactions in a wallet\n");

  int page = 1;
  while ( searchReceivedTransaction(ArkAddress, page, id, amount, senderAddress, vendorField) ) {
    Serial.print("Page: ");
    Serial.println(page);
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

    page++;
  };
  Serial.print("No more Transactions ");
  Serial.print("\nThe most recent transaction was page #: ");
  Serial.println(page - 1);

  return page - 1;
}




void setup()
{
  Serial.begin(115200);     // Initialize Serial Connection for debug / display





  //setup OLED display
  u8g2.begin();             // initialize OLED
  u8g2.clearBuffer();
  u8g2.setContrast(220);    // set OLED brightness(0->255)
  //u8g2.setFont(u8g2_font_ncenB12_tr );  // 12 pixel height  configure font used for OLED display
  //u8g2.setFont(u8g2_font_ncenB08_tr);   // 8 pixel height  configure font used for OLED display
  //u8g2.setFont(u8g2_font_9x15_tf );     // 10 pixel height  configure font used for OLED display
  //u8g2.setFont(u8g2_font_6x13_te );     // 9 pixel height  configure font used for OLED display

  setupWiFi();  //configure WiFi connection

  setupTime();  //sync local time to NTP server

  checkArkNodeStatus(); //check to see if Ark Node is synced

  //--------------------------------------------
  //  Configure NeoPixels.
  //NOTE! Make sure to call strip.Begin() after you call Serial.Begin because Din pin of NeoPixel is also connected to Serial RX pin(on ESP8266) and will configure the pin for usage by the DMA interface.
  strip.Begin();
  strip.Show(); // Initialize all pixels to 'off'
  strip.ClearTo(RgbColor(0, 0, 0));



  searchTransaction();

  if ( searchReceivedTransaction(ArkAddress, 7, id, amount, senderAddress, vendorField) ) {

    Serial.print("\nTransaction id: ");
    Serial.println(id);
    Serial.print("Amount(Arktoshi): ");
    Serial.println(amount);
    Serial.print("Amount(Ark): ");
    Serial.println(float(amount) / 100000000, 8);

    Serial.print("Sender address: ");
    Serial.println(senderAddress);
    Serial.print("Vendor Field: ");
    Serial.println(vendorField);
  }
  else {
    Serial.print("\nno valid transaction ");
  }



  //  getReceivedTransaction(ArkAddress,1);
  // ArkAddress and 6 are input parameters.  id, amount, senderaddress, vendorField are pass by reference and filled in by function)
  if ( getReceivedTransaction(ArkAddress, 6, id, amount, senderAddress, vendorField) ) {

    Serial.print("\nTransaction id: ");
    Serial.println(id);
    Serial.print("Amount(Arktoshi): ");
    Serial.println(amount);
    Serial.print("Amount(Ark): ");
    Serial.println(float(amount) / 100000000, 8);

    Serial.print("Sender address: ");
    Serial.println(senderAddress);
    Serial.print("Vendor Field: ");
    Serial.println(vendorField);
  }
  else {
    Serial.print("\nno valid transaction ");
  }



  lastRXpage = getMostRecentReceivedTransaction();
  //lastRXpage is equal to the page number of the last received transaction.

  ConfigureNeoPixels(redgreen);

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



void loop() {
  //look for new transactions to arrive in wallet.
  Serial.print("\n\n\nLooking for new transaction\n");


  u8g2.setFont(u8g2_font_9x15_tf );       // 10 pixel height  configure font used for OLED display

  int page = lastRXpage + 1;
  if ( searchReceivedTransaction(ArkAddress, page, id, amount, senderAddress, vendorField) ) {
    Serial.print("Page: ");
    Serial.println(page);
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

      u8g2.clearBuffer();
      u8g2.drawStr(0, 12, "Turn LED ON");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      u8g2.sendBuffer();                    // transfer internal memory to the display
    }
    // else if (vendorField == "led off") {
    else if  (strcmp(vendorField, "led off") == 0) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 12, "LEDs OFF");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(off);
    }
    //  else if (vendorField == "color red") {
    else if  (strcmp(vendorField, "color red") == 0) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 12, "Glowing Red");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(red);
    }
    //    else if (vendorField == "color green") {
    else if  (strcmp(vendorField, "color green") == 0) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 12, "Glowing green");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(green);
    }
    //     else if (vendorField == "color blue") {
    else if  (strcmp(vendorField, "color blue") == 0) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 12, "Glowing blue");  // write text string to the internal memory of OLED; (x coordinate, y coordinate, string)
      u8g2.sendBuffer();                    // transfer internal memory to the display
      ConfigureNeoPixels(blue);
    }

    else {
      u8g2.clearBuffer();
      u8g2.setCursor(0, 12);
      Serial.print("Unspecified VendorField: ");
      u8g2.print(vendorField);           // display unknown vendor field
      u8g2.sendBuffer();                    // transfer internal memory to the display

    }


    lastRXpage++;
  }

  else {

  }




  delay(3000);
  //  esp_deep_sleep_start();

};

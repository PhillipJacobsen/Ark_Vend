/********************************************************************************
  This file contains functions that interact with Ark client C++ API
  code here is a hack right now. Just learning the API and working on basic program flow and function
********************************************************************************/



/********************************************************************************
  This routine searches the block chain with receive address and page as input parameters
  It returns details for 1 transaction if available.
  Returns '0' if no transaction exist
  returns parameters in
  id -> transaction ID
  amount -> amount of Arktoshi
  senderAddress -> transaction sender address
  vendorfield -> 64 Byte vendor field

********************************************************************************/
int searchReceivedTransaction(const char *const address, int page, const char* &id, int &amount, const char* &senderAddress, const char* &vendorField ) {

  //Serial.print("\nSearch Received Address: ");
  //Serial.println(address);
  //Serial.print("\nSearch page: ");
  //Serial.println(page );

  //const std::map<std::string, std::string>& body_parameters, int limit = 5,
  std::string vendorFieldHexString;
  vendorFieldHexString = "6964647955";
  //std::string transactionSearchResponse = connection.api.transactions.search( {{"vendorFieldHex", vendorFieldHexString}, {"orderBy", "timestamp:asc"} },1,1);

  //--------------------------------------------
  //peform the API
  //sort by oldest transactions first.  For simplicity set limit = 1 so we only get 1 transaction returned
  std::string transactionSearchResponse = connection.api.transactions.search( {{"recipientId", address}, {"orderBy", "timestamp:asc"} }, 1, page);
  /**
     transactionSearchResponse return response is a json-formatted object
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

  //--------------------------------------------
  //  Print the entire return response string
  //  Serial.print("\nSearch Result Transactions: ");
  //  Serial.println(transactionSearchResponse.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.


  //--------------------------------------------
  //  Deserialize the returned JSON
  //  All of the returned parameters are parsed which is not necessary but may be usefull for testing.
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(11) + 810;
  DynamicJsonBuffer jsonBuffer(capacity);

  //this is an exapmle of the JSON string that is returned
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

  //--------------------------------------------
  //  The meta parameters that are returned are currently not reliable and are "estimates". Apparently this is due to lower performance nodes
  //  For this reason I will not use any of the meta parameters

  //--------------------------------------------
  //  the data_0_id parameter will be used to determine if a valid transaction was found.
  if (data_0_id == nullptr) {
    Serial.print("\n data_0_id is null");
    return 0;           //no transaction found
  }
  else {
    //   Serial.print("\n data_0_id is available");
    id = data_0_id;
    amount = data_0_amount;
    senderAddress = data_0_sender;
    vendorField = data_0_vendorField;
  }
  return 1;           //transaction found

}







/********************************************************************************
  This routine checks to see if Ark node is syncronized to the chain. 
  This is a maybe a good way to see if node communication is working correctly.
  This might be a good routine to run periodically  
  Returns True if node is synced
********************************************************************************/
bool checkArkNodeStatus() {
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
  
  return data_synced;   
  
  /****************************************/
}





/********************************************************************************
  This routine will search through all the received transactions of ArkAddress wallet starting from the oldest.
  "searching wallet + page#" will be displayed. text will toggle between red/white every received transaction
  The page number of the last transaction in the search will be displayed. 
  This is the page to the most newest receive transaction on the chain.
  The final page number is also equal to the total number of received transactions in the wallet. 

  The routine returns the page number of the most recent transaction.
  Empty wallet will return '0' (NOT YET TESTED)

********************************************************************************/
int getMostRecentReceivedTransaction() {
  Serial.print("\n\n\nHere are all the transactions in a wallet\n");

  int CursorXtemp;
  int CursorYtemp;

  int page = 1;
  while ( searchReceivedTransaction(ArkAddress, page, id, amount, senderAddress, vendorField) ) {
    Serial.print("Page: ");
    Serial.println(page);
    //   Serial.print("Transaction id: ");
    //    Serial.println(id);
    //    Serial.print("Amount(Arktoshi): ");
    //    Serial.println(amount);
    //    Serial.print("Amount(Ark): ");
    //   Serial.println(float(amount) / 100000000, 8);
    //    Serial.print("Sender address: ");
    //   Serial.println(senderAddress);
    Serial.print("Vendor Field: ");
    Serial.println(vendorField);

    tft.setCursor(CursorX, CursorY);
    if ( (page & 0x01) == 0) {
      tft.setTextColor(ILI9341_WHITE);
      tft.print("searching wallet: ");
      CursorXtemp = tft.getCursorX();
      CursorYtemp = tft.getCursorY();
      tft.setTextColor(ILI9341_BLACK);
      tft.print(page - 1);
      tft.setCursor(CursorXtemp, CursorYtemp);
      tft.setTextColor(ILI9341_WHITE);
      tft.println(page);


    }
    else {
      tft.setTextColor(ILI9341_RED);
      tft.print("searching wallet: ");
      CursorXtemp = tft.getCursorX();
      CursorYtemp = tft.getCursorY();
      tft.setTextColor(ILI9341_BLACK);
      tft.print(page - 1);
      tft.setCursor(CursorXtemp, CursorYtemp);
      tft.setTextColor(ILI9341_RED);
      tft.println(page);
      //We need to clear the pixels around the page number every time we refresh.
    }
    page++;
  };
  tft.setCursor(CursorXtemp, CursorYtemp);
  tft.setTextColor(ILI9341_BLACK);
  tft.println(page - 1);

  Serial.print("No more Transactions ");
  Serial.print("\nThe most recent transaction was page #: ");
  Serial.println(page - 1);

  return page - 1;
}








//quick test routine.
void searchTransaction() {
  //const std::map<std::string, std::string>& body_parameters, int limit = 5,
  std::string vendorFieldHexString;
  vendorFieldHexString = "6964647955";
  //std::string transactionSearchResponse = connection.api.transactions.search( {{"vendorFieldHex", vendorFieldHexString}, {"orderBy", "timestamp:asc"} },1,1);
  std::string transactionSearchResponse = connection.api.transactions.search( {{"recipientId", ArkAddress}, {"orderBy", "timestamp:asc"} }, 1, 1);

  Serial.print("\nSearch Result Transactions: ");
  Serial.println(transactionSearchResponse.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.
}

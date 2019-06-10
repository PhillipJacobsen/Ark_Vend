void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages: ");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    Serial.println(text);
    //Serial.println(chat_id);

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    //    if (bot.messages[i].longitude != 0 || bot.messages[i].latitude != 0) {
    //      Serial.print("Long: ");
    //      Serial.println(String(bot.messages[i].longitude, 6));
    //      Serial.print("Lat: ");
    //      Serial.println(String(bot.messages[i].latitude, 6));
    //
    //      String message = "Long: " + String(bot.messages[i].longitude, 6) + "\n";
    //     message += "Lat: " + String(bot.messages[i].latitude, 6) + "\n";
    //      bot.sendMessage(chat_id, message, "Markdown");
    //    }

    //response keyboard.
    if (text == "/options" || text == "/options@arkIOT_bot") {
     // String keyboardJson = "[[\"/ledon\", \"/ledoff\"],[\"/status\"]]";
    String keyboardJson = "[ [\"/ledon\", \"/ledoff\", \"/status\" ] , [\"/balance\", \"/transactions\"] ]";
      
      bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
    }

    //inline keyboard
    //     if (text == "/options") {
    //       String keyboardJson = "[[{ \"text\" : \"Go to Google\", \"url\" : \"https://www.google.com\" }],[{ \"text\" : \"Send\", \"callback_data\" : \"This was sent by inline\" }]]";
    //       bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
    //     }


    if (text == "/ledon" || text == "/ledon@arkIOT_bot") {
      //digitalWrite(ledPin, LOW);   // turn the LED on (HIGH is the voltage level)   //esp8266
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)     //Heather32
      ledStatus = 1;
      bot.sendMessage(chat_id, "LED is ON", "");
    }

    if (text == "/ledoff" || text == "/ledoff@arkIOT_bot") {
      ledStatus = 0;
      //digitalWrite(ledPin, HIGH);    // turn the LED off (LOW is the voltage level)  //esp8266
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)    //Heather32

      bot.sendMessage(chat_id, "LED is OFF", "");
    }


    if (text == "/name" || text == "/name@arkIOT_bot" ) {
      bot.sendMessage(chat_id, "I am ARK IOT Bot. What do you want master?", "");
    }


    if (text == "/time" || text == "/time@arkIOT_bot") {
      //  time_t is used to store the number of seconds since the epoch (normally 01/01/1970)
      time_t now = time(nullptr);
      bot.sendMessage(chat_id, ctime(&now), "");      //return the current time

      Serial.print("time is: ");
      Serial.println(ctime(&now));

      //The Arduino Time library returns a time_t to the now() function.  time_t t = now(); // store the current time in time variable t. I am not sure why this doesn't work.
      //bot.sendMessage(chat_id, String(minute(now)), "");    //return the current minute

    }



    if (text == "/status" || text == "/status@arkIOT_bot") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Status: Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Status: Led is OFF", "");
      }
    }

    if (text == "/start" || text == "/start@arkIOT_bot") {
      String welcome = "Welcome to Ark Vending Machine Demo Bot, " + from_name + ".\n";
      // welcome += "This is Ark Vending Machine Demo.\n\n";
      welcome += "/options : display option keyboard\n";
      welcome += "/ledon : to switch the LED ON\n";
      welcome += "/ledoff : to switch the LED OFF\n";
      welcome += "/time : Returns current Date and Time\n";
      welcome += "/name : Returns bot name\n";
      welcome += "/status : Returns current status of LED\n";
      welcome += "/balance : Returns balance of wallet\n";
      welcome += "/transactions : Returns # of received transactions\n"; 
      bot.sendMessage(chat_id, welcome, "Markdown");
    }

    if (text == "/balance" || text == "/balance@arkIOT_bot" ) {
      //  retrieve balance from wallet
      std::string walletResponse = connection.api.wallets.get(ArkAddress);
      Serial.print("\nWallet: ");
      Serial.println(walletResponse.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.
      //https://arduinojson.org/v5/assistant/
      /*
        {"data":{"address":"AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx","publicKey":null,"username":null,"secondPublicKey":null,"balance":4480000000,"isDelegate":false,"vote":null}}
      */
      //--------------------------------------------
      //  Deserialize the returned JSON
      //  All of the returned parameters are parsed which is not necessary but may be usefull for testing.
      const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(7) + 160;
      DynamicJsonBuffer jsonBuffer(capacity);

      //const char* json = "{\"data\":{\"address\":\"AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx\",\"publicKey\":null,\"username\":null,\"secondPublicKey\":null,\"balance\":4480000000,\"isDelegate\":false,\"vote\":null}}";

      JsonObject& root = jsonBuffer.parseObject(walletResponse.c_str());

      JsonObject& data = root["data"];
      //     const char* data_address = data["address"]; // "AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx"
      //long data_balance = data["balance"]; // 4480000000
      const char* data_balance2 = data["balance"]; // 4480000000    //I am not sure why this is a const char* instead of a long as generated by json parser tool
      //     bool data_isDelegate = data["isDelegate"]; // false


      Serial.print("balance: ");
      // Serial.println(data_balance);
      // Serial.println(String(data_balance));
      Serial.println(data_balance2);

      String balance = "balance: ";
      balance += String(data_balance2);
      bot.sendMessage(chat_id, balance, "");

    }

    if (text == "/transactions" || text == "/transactions@arkIOT_bot" ) {
      //  retrieve number of received transactions in wallet

      Serial.print("transactions: ");
      Serial.println(lastRXpage);

      String rxTransactions = "# of Rx transactions: ";
      rxTransactions += String(lastRXpage);
      bot.sendMessage(chat_id, rxTransactions, "");

    }




  }
}

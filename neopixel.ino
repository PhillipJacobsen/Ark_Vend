//test routines below. These are not being used.



/*
  void loop() {

  //look for new transactions to arrive in wallet.
  Serial.print("\n\n\nLooking for new transaction\n");

  delay(1000);

  searchRXpage = lastRXpage + 1;
  if ( searchReceivedTransaction(ArkAddress, searchRXpage, id, amount, senderAddress, vendorField) ) {

    //a new transaction has been received.
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

    //check to see if vendorField of new transaction matches the field in QRcode that we displayed
    if  (strcmp(vendorField, VendorID) == 0) {
      Serial.println("thanks for the payment!");
      tft.setTextColor(ILI9341_GREEN);
      tft.print("Payment:");
      tft.println(VendorID);
    }


    if  (strcmp(vendorField, "led on") == 0) {
    }

    else if  (strcmp(vendorField, "led off") == 0) {
      ConfigureNeoPixels(off);
    }

    else if  (strcmp(vendorField, "color red") == 0) {
      ConfigureNeoPixels(red);
    }

    else if  (strcmp(vendorField, "color green") == 0) {

      ConfigureNeoPixels(green);
    }

    else if  (strcmp(vendorField, "color blue") == 0) {
      ConfigureNeoPixels(blue);
    }

    else {
      Serial.print("Unspecified VendorField: ");
    }
    lastRXpage++;
  }


  //no new transaction found.
  else {

  }

  };
*/

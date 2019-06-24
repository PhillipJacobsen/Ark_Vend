bool checkPaymentTimer() {
  timeNow = millis();  //get current time
  if ((timeNow > payment_Timeout)) {

    return true;
  }
  Serial.print("timeout: ");
  Serial.println ((payment_Timeout - timeNow) / 1000);
//  tft.fillRect(150, 75, 80, 18, ILI9341_BLACK);     //delete the previous time
//  tft.setCursor(150, 90);
//  tft.setTextColor(ILI9341_RED);
//  tft.println((payment_Timeout - timeNow) / 1000);

  return false;
}

bool checkCancelButton() {
}

bool search_newRX() {
}



void ArkVendingMachine() {         //The Vending state machine
  // Serial.print("vmState: ");
  //  Serial.println(vmState);


  switch (vmState) {              //Depending on the state


    //--------------------------------------------
    case DRAW_HOME: {
//        drawHomeScreen();
        ARKscan_lasttime = millis();
        vmState = WAIT_FOR_USER;

        break;
      }


    case WAIT_FOR_USER: {

        if (millis() > ARKscan_lasttime + ARK_mtbs)  {
 
           //check to see if new new transaction has been received in wallet
          searchRXpage = lastRXpage + 1;
          if ( searchReceivedTransaction(ArkAddress, searchRXpage, id, amount, senderAddress, vendorField) ) {
            //a new unknown transaction has been received.
            lastRXpage++;
            Serial.print("Page: ");
            Serial.println(searchRXpage);
            Serial.print("Transaction id: ");
            Serial.println(id);
            Serial.print("Vendor Field: ");
            Serial.println(vendorField);

            ARKscan_lasttime = millis();
            vmState = WAIT_FOR_USER;     //stay in the same state
            break;
          }
          ARKscan_lasttime = millis();
        }

//        if (handleTouchscreenV2()) {       //check if touchscreen has been pressed
  //        Serial.print("button was pushed. display QRcode now");
     //     promptForPayment();
  //        timeNow = millis();  //get current time
  //        payment_Timeout = timeNow + PAYMENT_WAIT_TIME;

   //       tft.setCursor(0, 90);
   //       tft.setTextColor(ILI9341_RED);
   //       tft.println("Payment Timeout");
  //        vmState = WAIT_FOR_PAY;               //WAIT_FOR_PAY
   //       cancelButton();
  //        break;
  //      }

        vmState = WAIT_FOR_USER;     //default state
        break;
      }


    case WAIT_FOR_PAY: {
        //check to see if new new transaction has been received in wallet
        searchRXpage = lastRXpage + 1;
        if ( searchReceivedTransaction(ArkAddress, searchRXpage, id, amount, senderAddress, vendorField) ) {
          //a new transaction has been received.
          lastRXpage++;
          Serial.print("Page: ");
          Serial.println(searchRXpage);
          Serial.print("Transaction id: ");
          Serial.println(id);
          Serial.print("Vendor Field: ");
          Serial.println(vendorField);

          //check to see if vendorField of new transaction matches the field in QRcode that we displayed
          if  (strcmp(vendorField, VendorID) == 0) {
            Serial.println("Thanks for the payment!");
 
            vmState = VEND_ITEM;            //State is now VEND_ITEM
            break;

          }
          else {                            //transaction with incorrect vendor field received
 //           tft.println("Invalid Vendor Received");
            vmState = WAIT_FOR_PAY;         //stay in the current state
            break;
          }
        }

        //check to see if time has expired before payment has been received
        if (checkPaymentTimer()) {
          vmState = DRAW_HOME;              //timer has elapsed without receiving payment. go back to start of state machine
          break;
        }

        //check to see if cancel button was pressed
  //      if (handleTouchScreenWaitForPayment()) {
  //        vmState = DRAW_HOME;              //cancel button was pressed. Go back to start of state machine
   //       break;
   //     }


        vmState = WAIT_FOR_PAY;     //stay in the same state
        break;
      }


    case VEND_ITEM: {
         Serial.println("turn on servo");

        Serial.println("turn off servo");
        
        timeAPIstart = millis();  //get time that API read started

        bot.sendMessage("-228362617", "Dispensing Candy", "");

        timeNow = millis() - timeAPIstart;  //get current time
        Serial.print(" send message time:");
        Serial.println(timeNow);


        delay(3000);
        vmState = DRAW_HOME;             //
        break;                          //Get out of switch
      }


  }
}

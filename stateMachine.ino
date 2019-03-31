bool checkPaymentTimer() {
  timeNow = millis();  //get current time
  if ((timeNow > payment_Timeout)) {

    return true;
  }
  Serial.print("timeout: ");
  Serial.println ((payment_Timeout - timeNow)/1000);
  tft.fillRect(150, 95, 80, 18, ILI9341_BLACK);     //delete the previous time
  tft.setCursor(150, 110);
  tft.setTextColor(ILI9341_RED);
  tft.println((payment_Timeout - timeNow)/1000);

  return false;
}


bool search_newRX() {
}



void ArkVendingMachine() {         //The Vending state machine
  switch (vmState) {              //Depending on the state


    //--------------------------------------------
    case DRAW_HOME: {
        drawHomeScreen();
        vmState = WAIT_FOR_USER;
        break;
      }


    case WAIT_FOR_USER: {
        //check to see if new new transaction has been received in wallet
        searchRXpage = lastRXpage + 1;
        if ( searchReceivedTransaction(ArkAddress, searchRXpage, id, amount, senderAddress, vendorField) ) {
          lastRXpage++;
          //a new transaction has been received.
          Serial.print("Page: ");
          Serial.println(searchRXpage);
          Serial.print("Transaction id: ");
          Serial.println(id);
          Serial.print("Vendor Field: ");
          Serial.println(vendorField);
          vmState = WAIT_FOR_USER;     //stay in the same state
          break;
        }

        if (handleTouchscreenV2()) {       //check if touchscreen has been pressed
          Serial.print("button was pushed. display QRcode now");
          promptForPayment();
          timeNow = millis();  //get current time
          payment_Timeout = timeNow + PAYMENT_WAIT_TIME;

          tft.setCursor(0, 110);
          tft.setTextColor(ILI9341_RED);
          tft.println("Payment Timeout");
          vmState = WAIT_FOR_PAY;               //WAIT_FOR_PAY

          break;
        }

        vmState = WAIT_FOR_USER;     //default state
        break;
      }


    case WAIT_FOR_PAY: {
        //check to see if new new transaction has been received in wallet
        searchRXpage = lastRXpage + 1;
        if ( searchReceivedTransaction(ArkAddress, searchRXpage, id, amount, senderAddress, vendorField) ) {
          //a new transaction has been received.
          Serial.print("Page: ");
          Serial.println(searchRXpage);
          Serial.print("Transaction id: ");
          Serial.println(id);
          Serial.print("Vendor Field: ");
          Serial.println(vendorField);

          //check to see if vendorField of new transaction matches the field in QRcode that we displayed
          if  (strcmp(vendorField, VendorID) == 0) {
            Serial.println("Thanks for the payment!");
            tft.fillRoundRect(0, 160, 150, 150, 2, ILI9341_BLACK);     //remove the QRcode
            tft.setCursor(0, 180);
            tft.setTextColor(ILI9341_GREEN);
            tft.println("Thanks for the payment");
            tft.print("Vendor:");
            tft.println(VendorID);
            tft.println("Enjoy the candy!");

            lastRXpage++;
            vmState = VEND_ITEM;            //State is now VEND_ITEM
            break;                          //Get out of switch
          }
          else {
            vmState = WAIT_FOR_PAY;           //
            break;                            //Get out of switch
          }
        }

        if (checkPaymentTimer()) {        //check to see if time has expired before payment has been received
          vmState = DRAW_HOME;             //timer has elapsed without receiving payment. go back to start of state machine
          break;                          //Get out of switch
        }


        vmState = WAIT_FOR_PAY;     //stay in the same state
        break;
      }


    case VEND_ITEM: {
        delay(12000);
        vmState = DRAW_HOME;             //State is now DROP_CAN
        break;                          //Get out of switch
      }


  }
}

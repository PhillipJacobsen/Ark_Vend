
bool checkPaymentTimer() {
}

//bool search_newRX() {
//}

bool handleTouchscreenV2() {

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
           vmState = WAIT_FOR_PAY;               //WAIT_FOR_PAY
          break;
        }

        vmState = WAIT_FOR_USER;     //default state
        break;
      }


    case WAIT_FOR_PAY: {                   //State is FIFTY
        if (search_newRX()) {             //check to see if payment has been received in wallet
          vmState = VEND_ITEM;             //State is now VEND_ITEM
          break;                          //Get out of switch
        }

        if (checkPaymentTimer()) {        //check to see if time has expired before payment has been received
          vmState = DRAW_HOME;             //timer has elapsed without receiving payment. go back to start of state machine
          break;                          //Get out of switch
        }


        vmState = WAIT_FOR_PAY;     //stay in the same state
        break;
      }


    case VEND_ITEM: {

        vmState = DRAW_HOME;             //State is now DROP_CAN
        break;                          //Get out of switch
      }


  }
}

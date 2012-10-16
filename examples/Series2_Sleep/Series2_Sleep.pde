/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <XBee.h>
#include <SoftwareSerial.h>

/*
This example is for Series 2 (ZigBee) XBee Radios only, though Series 1 also support sleep mode.
This example demonstrates the XBee pin sleep setting, by allowing the Arduino
to sleep/wake the XBee.  In this example and end device is attached to the Arduino.
 
The end device sleep mode must be set to 1 (SM=1), to enable pin sleep.
Set SP=AF0 (28 seconds) on the coordinator.  This will instruct the coordinator to buffer any packets, for up to 28 seconds,
while the end device is sleeping.  When the end device wakes, it will poll the coordinator and receive the packet.

Note: I'm using the SoftSerial library to communicate with the Arduino since the Arduino's Serial is being used by the XBee

How it works:
When you send a "1", the Arduino will sleep the XBee.
Sending "2" wakes the XBee and "3" will send an arbitrary TX packet.
Of course if the XBee is sleeping, the TX packet will not be delivered.
Connect the Arduino Serial Monitor to the usb-serial device to send the commands.
Connect an LED to the XBee Module Status (pin 13).  This will turn on when the XBee is awake and off when it's sleeping
Attach the coordinator to your computer and send a TX packet ever 28 seconds.  You should be able to verify the 
end device receives the packet when it wakes from sleep.

Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
*/

// create the XBee object
XBee xbee = XBee();

// create an arbitrary payload -- what we're sending is not relevant
uint8_t payload[] = { 0, 0 };

// SH + SL Address of a remote XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x403e0f30);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
ZBRxResponse rx = ZBRxResponse();

// note: xbee sleep pin doesn't need 3.3. to sleep -- open circuit also will sleep it, but of course it needs 0V to wake!
// connect Arduino digital 7 to XBee sleep pin (9) through a voltage divider.  I'm using a 10K resistor. 
uint8_t sleepPin = 7;

// SoftSerial RX: connect Arduino digitial 8 to the TX of of usb-serial device.  note: I'm using Modern Device's USB BUB (set to 5V).  You can use a 3.3V usb-serial with a voltage divider on RX (TX does not require since Arduino is 3.3V tolerant)
uint8_t ssRX = 8;
// SoftSerial TX: connect Arduino digital 9 to RX of usb-serial device
uint8_t ssTX = 9;

SoftwareSerial nss(ssRX, ssTX);

void setup() {
  pinMode(sleepPin, OUTPUT);
  // set to LOW (wake)
  digitalWrite(sleepPin, LOW);
  
  // start XBee communication
  Serial.begin(9600);
  xbee.setSerial(Serial);
  // start soft serial
  nss.begin(9600);
}

void sendPacket() {

    nss.println("Sending a packet");
      
    xbee.send(zbTx);
    
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(5000)) {
        // got a response!

        // should be a znet tx status            	
    	if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
    	   xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
           if (txStatus.getDeliveryStatus() == SUCCESS) {
            	// success.  time to celebrate
             	nss.println("packet was delivered");
           } else {
            	// the remote XBee did not receive our packet. is it powered on?
                nss.println("packet delivery failed");
           }
        } else if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        	// we received a packet something!
            xbee.getResponse().getZBRxResponse(rx);
        	// print the payload. lets assume it's text.
        	for (uint8_t i = 0; i < rx.getDataLength(); i++) {
        		nss.println(rx.getData(i));
        	}
        }      
    } else {
      // local xbee is sleeping
       nss.println("no response -- is local xbee sleeping?");
    }  
}

void loop() {   
  if (nss.available()) {
    int cmd = nss.read();
    
    // ascii 1 == 49
    if (cmd == 49) {   // 1 (ASCII)
      // sleep XBee
      digitalWrite(sleepPin, HIGH);
      nss.println("sleeping xbee");
    } else if (cmd == 50) {  // 2 (ASCII)
      digitalWrite(sleepPin, LOW);
      nss.println("waking xbee");
    } else if (cmd == 51) {  // 3 (ASCII)
      // send packet
      sendPacket();
    } else {
       nss.println("I didn't understand"); 
    }
  }
  
  readPacket();
}

// when XBee powers up it sends a modem status 0 (hardware reset), followed by a 2 (Joined Network), assumming it's configured correctly
// when XBee is woken up it sends a modem status 2 (Joined Network)

void readPacket() {
  xbee.readPacket();
  
    if (xbee.getResponse().isAvailable()) {
        // got something.. print packet to nss
        nss.print("API=");
        nss.print(xbee.getResponse().getApiId(), HEX);
        nss.print(",frame=");
        
        // print frame data
        for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
          nss.print(xbee.getResponse().getFrameData()[i], HEX);
          nss.print(" ");
        }
        
        nss.println("");
    } else if (xbee.getResponse().isError()) {
     nss.print("XBee error. error code is");
     nss.println(xbee.getResponse().getErrorCode(), DEC);
    }
}


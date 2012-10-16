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
This example is for Series 2 (ZigBee) XBee Radios only
Receives I/O samples from a remote radio.
The remote radio must have IR > 0 and at least one digital or analog input enabled.
The XBee coordinator should be connected to the Arduino.
 
This example uses the SoftSerial library to view the XBee communication.  I am using a 
Modern Device USB BUB board (http://moderndevice.com/connect) and viewing the output
with the Arduino Serial Monitor.
*/

// Define NewSoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
SoftwareSerial nss(ssRX, ssTX);

XBee xbee = XBee();

ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

XBeeAddress64 test = XBeeAddress64();

void setup() { 
  Serial.begin(9600);
  xbee.setSerial(Serial);
  // start soft serial
  nss.begin(9600);
}

void loop() {
  //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      nss.print("Received I/O Sample from: ");
      
      nss.print(ioSample.getRemoteAddress64().getMsb(), HEX);  
      nss.print(ioSample.getRemoteAddress64().getLsb(), HEX);  
      nss.println("");
      
      if (ioSample.containsAnalog()) {
        nss.println("Sample contains analog data");
      }

      if (ioSample.containsDigital()) {
        nss.println("Sample contains digtal data");
      }      

      // read analog inputs
      for (int i = 0; i <= 4; i++) {
        if (ioSample.isAnalogEnabled(i)) {
          nss.print("Analog (AI");
          nss.print(i, DEC);
          nss.print(") is ");
          nss.println(ioSample.getAnalog(i), DEC);
        }
      }

      // check digital inputs
      for (int i = 0; i <= 12; i++) {
        if (ioSample.isDigitalEnabled(i)) {
          nss.print("Digital (DI");
          nss.print(i, DEC);
          nss.print(") is ");
          nss.println(ioSample.isDigitalOn(i), DEC);
        }
      }
      
      // method for printing the entire frame data
      //for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
      //  nss.print("byte [");
      //  nss.print(i, DEC);
      //  nss.print("] is ");
      //  nss.println(xbee.getResponse().getFrameData()[i], HEX);
      //}
    } 
    else {
      nss.print("Expected I/O Sample, but got ");
      nss.print(xbee.getResponse().getApiId(), HEX);
    }    
  } else if (xbee.getResponse().isError()) {
    nss.print("Error reading packet.  Error code: ");  
    nss.println(xbee.getResponse().getErrorCode());
  }
}

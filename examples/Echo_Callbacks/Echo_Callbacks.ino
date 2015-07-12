/**
 * Copyright (c) 2015 Matthijs Kooijman
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
#include <Printers.h>

/*
 This example is for Series 1 and 2 XBee (no changes needed for either).

 It listens for incoming packets and echoes back any data received back
 to the sender. This example shows how to use XBeeWithCallbacks to
 concisely express reading of response packets.

 This example assumes an Arduino with two serial ports (like the
 Leonardo or Mega). Replace Serial and Serial1 below appropriately for
 your hardware.
*/

// create the XBee object
XBeeWithCallbacks xbee;

void zbReceive(ZBRxResponse& rx, uintptr_t) {
  // Create a reply packet containing the same data
  // This directly reuses the rx data array, which is ok since the tx
  // packet is sent before any new response is received
  ZBTxRequest tx;
  tx.setAddress64(rx.getRemoteAddress64());
  tx.setAddress16(rx.getRemoteAddress16());
  tx.setPayload(rx.getFrameData() + rx.getDataOffset(), rx.getDataLength());

  // Send the reply, but do not wait for the tx status reply. If an
  // error occurs, the global onTxStatusResponse handler will print an
  // error message, but no message is printed on succes.
  xbee.send(tx);
  Serial.println(F("Sending ZBTxRequest"));
}

void receive16(Rx16Response& rx, uintptr_t) {
  // Create a reply packet containing the same data
  // This directly reuses the rx data array, which is ok since the tx
  // packet is sent before any new response is received
  Tx16Request tx;
  tx.setAddress16(rx.getRemoteAddress16());
  tx.setPayload(rx.getFrameData() + rx.getDataOffset(), rx.getDataLength());

  // Send the reply, but do not wait for the tx status reply. If an
  // error occurs, the global onTxStatusResponse handler will print an
  // error message, but no message is printed on succes.
  xbee.send(tx);
  Serial.println(F("Sending Tx16Request"));
}

void receive64(Rx64Response& rx, uintptr_t) {
  // Create a reply packet containing the same data
  // This directly reuses the rx data array, which is ok since the tx
  // packet is sent before any new response is received
  Tx64Request tx;
  tx.setAddress64(rx.getRemoteAddress64());
  tx.setPayload(rx.getFrameData() + rx.getDataOffset(), rx.getDataLength());

  // Send the reply, but do not wait for the tx status reply. If an
  // error occurs, the global onTxStatusResponse handler will print an
  // error message, but no message is printed on succes.
  xbee.send(tx);
  Serial.println(F("Sending Tx64Request"));
}
void setup() {
  Serial.begin(9600);

  Serial1.begin(9600);
  xbee.setSerial(Serial1);

  // Make sure that any errors are logged to Serial. The address of
  // Serial is first cast to Print*, since that's what the callback
  // expects, and then to uintptr_t to fit it inside the data parameter.
  xbee.onPacketError(printErrorCb, (uintptr_t)(Print*)&Serial);
  xbee.onTxStatusResponse(printErrorCb, (uintptr_t)(Print*)&Serial);
  xbee.onZBTxStatusResponse(printErrorCb, (uintptr_t)(Print*)&Serial);

  // These are called when an actual packet received
  xbee.onZBRxResponse(zbReceive);
  xbee.onRx16Response(receive16);
  xbee.onRx64Response(receive64);

  // Print any unhandled response with proper formatting
  xbee.onOtherResponse(printResponseCb, (uintptr_t)(Print*)&Serial);

  // Enable this to print the raw bytes for _all_ responses before they
  // are handled
  //xbee.onResponse(printRawResponseCb, (uintptr_t)(Print*)&Serial);

  // Set AO=0 to make sure we get ZBRxResponses, not
  // ZBExplicitRxResponses (only supported on series2). This probably
  // isn't needed, but nicely shows how to use sendAndWait().
  uint8_t value = 0;
  AtCommandRequest req((uint8_t*)"AO", &value, sizeof(value));
  req.setFrameId(xbee.getNextFrameId());
  // Send the command and wait up to 150ms for a response
  uint8_t status = xbee.sendAndWait(req, 150);
  if (status == 0)
    Serial.println(F("Set AO=0"));
  else
    Serial.println(F("Failed to set AO (this is expected on series1)"));
}

void loop() {
  // Continuously let xbee read packets and call callbacks.
  xbee.loop();
}


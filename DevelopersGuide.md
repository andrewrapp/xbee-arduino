# Developers Guide #

Before getting started, you should understand the difference between AT (Transparent), and API (Packet) mode.  This software is designed for, and only supports API mode!  Refer to [WhyApiMode](http://code.google.com/p/xbee-api/wiki/WhyApiMode) to understand the difference.

This guide assumes you have already configured at least two radios and have one of them connected to an Arduino.

Create a new sketch.  At the top of the sketch import the XBee library

```
#include <XBee.h>
```

Then, before setup and loop functions, create an XBee object.  The XBee object provides functions for sending and receiving packets.

```
XBee xbee = XBee();
```

In the setup function, call the begin function with the baud rate, for example:

```
void setup() {
  xbee.begin(9600);
}
```

XBees are duplex radios, meaning they can both transmit and receive data.

## Transmit Packets ##

There a several different types of transmit (TX) packets available.  A list of all TX packets can be found in the API [documentation](http://xbee-arduino.googlecode.com/svn/trunk/docs/api/annotated.html).  All classes that end in "Request" are TX packets.

I'm going to describe how to send a data/payload packet, for transmitting data to other radios.  First, we need to create an array to contain the data to be sent

```
uint8_t payload[] = { 0, 0 };
```

The data type is `uint8_t`, which is simply an unsigned byte.  This array has a length of two.  You should create an array that is properly sized to support that amount of data you want to send.  Keep in mind not to exceed the maximum payload size, which depends on the type of radio/firmware.  For Series 1 it is 100 bytees; Series 2 varies depending on firmware and configuration, but is generally at least 80 bytes.

Now you'll need to create a TX packet.  The TX packet contains the payload, address of the remote XBee that should receive it, and a few other options.  The TX packet is specific to the radio type: Series 1 or Series 2.  In this example I'll use a Series 1 TX packet.  Series 1 radios can be addressed by their 16-bit or 64-bit address.  For 16-bit addressing (recommended) we would use:

```
Tx16Request tx = Tx16Request(0x1874, payload, sizeof(payload));
```

Here we are specifying the payload to send and the 16-bit address of the radio that will receive the packet.  This address must equal the MY value of the receiving radio.

### Populate the Payload ###

In the loop function we get an analog reading and store it in the payload:

```
      pin5 = analogRead(5);
      payload[0] = pin5 >> 8 & 0xff;
      payload[1] = pin5 & 0xff;
```

Analog input readings are 10-bit, but the xbee can only send bytes (8-bit), so we need to breakdown the 10-bit reading into two bytes.

For example, the reading might look like this in binary:

10 10001000

We assign the lower 8 bits (10001000) to the first index of the payload array  Then, we shift bits 9 and 10 down into the bits 0 and 1 of the second byte (00000010).  When we receive the payload we will need to reassemble the 10-bit value.

Now we can send the packet to the remote radio:

```
xbee.send(tx);
```

The XBee returns a TX Status packet after sending, indicating if the transmission was successful.

Refer to the TX examples for a complete sketch [Series 1](http://code.google.com/p/xbee-arduino/source/browse/trunk/examples/Series1_Tx/Series1_Tx.pde) or [Series 2](http://code.google.com/p/xbee-arduino/source/browse/trunk/examples/Series2_Tx/Series2_Tx.pde)

## Receive Packets ##

By default, the library allocates an array of size 110 bytes for receiving the packet.  Since we are not using dynamic memory, it must do this since it has no way of knowing the size of the packets it will be receiving.  The packet array size is specified in XBee.h

```
#define MAX_FRAME_DATA_SIZE 110
```

This allows for the largest payload size (100 bytes), plus packet overhead.  **Important**: If you are using smaller packets you can save memory by reducing this value.

As with Transmit packets, the type of RX packets depends on the radio: Series 1 or Series 2. On the receiving XBee, create a RX object.  This example uses a Series 1 RX packet:

```
Rx16Response rx16 = Rx16Response();
```

### Reading a Packet ###

In the loop function call the readPacket function

```
xbee.readPacket();
```

This will read any data that is available and should be very quick.  Another option is to call readPacket and specify a timeout:

```
xbee.readPacket(100) ;
```

This will wait up to 100 milliseconds for a packet to arrive before returning.

**Important**: Arduino uses interrupts for receiving serial data.  This allows the sketch to do other things (e.g. read sensors, activate outputs) and receive data at the same time without losing data.  Internally, Arduino uses a 100 byte buffer for receiving data.  It's very important that you are consuming data off the buffer at a faster rate  than data is transmitted, or you risk losing packets.  In xbee-arduino terms, this means that you must call readPacket faster than the XBee is receiving packets.  For example, if a TX sketch is sending packets every 100ms, the RX sketch must attempt a read a packet at a faster rate (e.g. every 80 ms), so that it does not lose data.

Now, to check if a packet was received:

```
if (xbee.getResponse().isAvailable()) {
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                xbee.getResponse().getRx16Response(rx16);
        }
}
```


It's very important to check the type of packet that was received, since there are several different packet types, which require special handling.  The function checks if the api id equals RX\_16\_RESPONSE.  If so it is a 16-bit address RX packet and the packet is populated by called getRx16Response and passing the object.

Now we can check the source address (the radio that sent the packet):

```
rx16.getRemoteAddress16();
```

and get the RSSI (signal strength the radio detected when receiving the packet)

```
rx16.getRssi()
```

and access the payload.  If you had New Soft Serial (NSS) configured, you could print the contents of the payload as follows:

```
       for (int i = 0; i < rx.getDataLength(); i++) { 
         nss.print(rx.getData(i), BYTE); 
       } 
```

You may also want to access the payload as an array, possibly to pass it to a function for processing:

```
       processPayload(rx.getData(), rx.getDataLength());
```

### Error Handling ###

It's always a good idea to check if an error occurred after a packet read attempt.

```
if (xbee.getResponse().isError()) {
// get the error code
xbee.getResponse().getErrorCode()
} 
```

Then check the error code to get more information.  Error codes are as follows:

1 - CHECKSUM\_FAILURE
2 - PACKET\_EXCEEDS\_BYTE\_ARRAY\_LENGTH
3 - UNEXPECTED\_START\_BYTE

These are defined in XBee.h
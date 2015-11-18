# xbee-arduino

Arduino library for communicating with XBees in API mode, with support for both Series 1 (802.15.4) and Series 2 (ZB Pro/ZNet). This library Includes support for the majority of packet types, including: TX/RX, AT Command, Remote AT, I/O Samples and Modem Status.


## News

* 11/18/15 Matthijs Kooijman's new book, [Building Wireless Sensor Networks Using Arduino](http://www.amazon.com/gp/product/1784395587/ref=as_li_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=1784395587&linkCode=as2&tag=xbapra-20&linkId=CEH23GT6ZPOT4ZH4) is now available. He covers the fundamentals of working with XBees in API mode, including some advanced topics (encryption, security, sleep), as well as creating projects with this library.
* 9/15/15 Matthijs Kooijman has contributed numerous enhancements to the library, including callbacks, enhanced debugging, added features and bug fixes! These can be found in the 0.6.0 release.
* 2/28/15 The code is now on github, although some documentation is still on googlecode https://code.google.com/p/xbee-arduino/
* 2/1/14 Release 0.5 is available. This is essentially the 0.4 Software Serial release with a bug fix or two. If upgrading from a version prior to 0.4, please note that the method for specifying the serial port has changed; see See SoftwareSerialReleaseNotes. Along with this release I have converted the repository from Subversion to Git
* 10/15/12 Release 0.4 (beta) is available. Paul Stoffregen (Teensy creator) has contributed a patch that allows for using SoftwareSerial? for XBee communication! This frees up the Serial port for debug or to use with other hardware. Try it out and report any issues on the Google group page. Important: See SoftwareSerialReleaseNotes as it was necessary to change the API to support this feature.
* 12/21/11 Release 0.3 is now available. This release includes support for Arduino 1.0 along with some bug fixes and a new setSerial function for using alternate serial ports (e.g. Mega). This release is compatible with previous Arduino releases as well.
* 4/3/11 I have created an XBeeUseCases wiki on XBee API that describes several use cases for communicating with XBees.
* 11/14/09 Version 0.2.1 is available. This release contains a bug fix for Remote AT
* 10/26/09 XBee-Arduino 0.2 is now available. This release adds support for AT Command, Remote AT, and I/O sample (series 1 and 2) packets. Along with this release I have created several new examples.
* 8/09/09 I have released Droplet, a wireless LCD display/remote control with support for Twitter, Google Calendar, weather etc. It uses this software to send and receive XBee packets.
* 4/19/09 Release 0.1.2: In this release I added some abbreviated constructors for creating basic Requests and get/set methods to facilitate the reuse of Requests
* 3/29/09 Initial Release

## Documentation
Doxygen API documentation is available in the downloads. Unfortunately it is not available online anymore as Git does not support the html mime type as Subversion does

[Developer's Guide](https://github.com/andrewrapp/xbee-arduino/blob/wiki/DevelopersGuide.md)

[XBee API (Java) Project](https://github.com/andrewrapp/xbee-api) Although this project is a Java implementation, it contains a few wikis relevant to this project, including xbee configuration and use cases.

[Google Group](https://groups.google.com/forum/#!forum/xbee-api)

## Example
I have created several sketches of sending/receiving packets with Series 1 and 2 XBee radios. You can find these in the examples folder. Here's an example of sending a packet with a Series 2 radio:

```
// Create an XBee object at the top of your sketch
XBee xbee = XBee();

// Start the serial port
Serial.begin(9600);
// Tell XBee to use Hardware Serial. It's also possible to use SoftwareSerial
xbee.setSerial(Serial);

// Create an array for holding the data you want to send.
uint8_t payload[] = { 'H', 'i' };

// Specify the address of the remote XBee (this is the SH + SL)
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x403e0f30);

// Create a TX Request
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

// Send your request
xbee.send(zbTx);
```

See the examples folder for the full source. There are more examples in the download.

See the XBee API project for Arduino < - > Computer communication.

To add XBee support to a new sketch, add "#include <XBee.h>" (without quotes) to the top of your sketch. You can also add it by selecting the "sketch" menu, and choosing "Import Library->XBee".

## Learning/Books
Check out these books to learn more about Arduino and XBee:

  * [Building Wireless Sensor Networks Using Arduino](http://www.amazon.com/gp/product/1784395587/ref=as_li_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=1784395587&linkCode=as2&tag=xbapra-20&linkId=CEH23GT6ZPOT4ZH4) (Kindle version available)
  * [Wireless Sensor Networks: with ZigBee, XBee, Arduino, and Processing](http://www.amazon.com/gp/product/0596807732?ie=UTF8&tag=xbapra-20&linkCode=as2&camp=1789&creative=9325&creativeASIN=0596807732Building) (Kindle version available)
  * Programming Arduino Getting Started with Sketches
  * Making Things Talk
  * Getting Started with Arduino (Make: Projects (Available in Kindle)
  * Arduino Cookbook (Oreilly Cookbooks) (Available in Kindle)

## Hardware

For development and general tinkering I highly recommend using an Arduino that has 2 serial ports, such as the Arduino Leonardo. The reason is the XBee requires serial port access and it is useful to have another serial port available for debugging via the Arduino serial console.

* Arduino Leonardo (recommended)
* Arduino UNO R3 (single serial port)
* Arduino Pro (single serial port)

XBee radios come in two models: Series 1 (S1) and Series 2 (S2). Series 1 are the best choice for most people as they are the easiest to configure. Series 2 XBee radios feature ZigBee and require a firmware update to use this software. Series 1 and 2 are not compatible with each other.

* XBee Series 1
* XBee Series 2 (ZigBee)

The Arduino XBee Shield is the easiest option for connecting the XBee to an Arduino. You can find XBee Shields from several vendors and even on ebay. Keep in mind if you select the SparkFun XBee Shield, it requires soldering headers (not included) to connect to an Arduino board.

If not using an XBee shield you'll need a 3.3V regulator and logic shifting to convert from 5V (Arduino) to 3.3V (XBee). The Arduino is 3.3V tolerant.

An XBee Explorer is highly recommended for updating firmware and configuring the radio. This is also useful for interfacing an XBee with a computer. If you are using Series 2 radios you'll need an XBee Explorer to upload API firmware to the radio, via X-CTU (they ship with AT firmware).


## Installation
Arduino 1.5 and later

Arduino now includes a library manager for easier library installation. From the Sketch menu select include library->Manage Libraries, then type "xbee" in the filter and install.

Prior to Arduino 1.5 installation is a manual

Download a .zip or .tar.gz release from github. Determine the location of your sketchbook by selecting "preferences" on the Arduino menu. Create a "libraries" folder in your sketchbook and unzip the release file in that location.

## Uploading Sketches

Uploading sketches with a Leonardo is as simple as connecting the Arduino to your computer and uploading. When using a single serial port Arduino, such as the UNO, the jumpers on the XBee Shield must be set to USB. Then, after upload, set back to the XBee position for the XBee to have access to the serial port. Always remember to power off the Arduino before moving the jumpers.

## Configuration

To use this library your XBee must be configured in API mode (AP=2). Take a look at this for information on configuring your radios to form a network.

## Questions/Feedback

Questions about this project should be posted to http://groups.google.com/group/xbee-api?pli=1 Be sure to provide as much detail as possible (e.g. what radios s1 or s2, firmware versions, configuration and code).

## Consulting/Commercial Licenses

I'm available for consulting to assist businesses or entrepreneurs that need help getting their projects up and running. I can also provide a commercial license for situations where you need to distribute code to clients/third parties that would otherwise conflict with GPL. For these matters I can be contacted at andrew.rapp [at] gmail.

## Overview ##

This is an Arduino library for communicating with XBees in API mode, with support for both Series 1 (802.15.4) and Series 2 (ZB Pro/ZNet).  This library Includes support for the majority of packet types, including: TX/RX, AT Command, Remote AT, I/O Samples and Modem Status.

**Note: This software requires API mode, by setting  AP=2. If you are using Series 2 XBee, you'll need to install API Firmware with [X-CTU](http://www.digi.com/support/productdetail?pid=3352&osvid=57&type=utilities) (Series 2 are manufactured with with AT firmware), then set AP=2.  This software will not work correctly with AP=1**  Refer to [XBeeConfiguration](http://code.google.com/p/xbee-api/wiki/XBeeConfiguration) and [WhyApiMode](http://code.google.com/p/xbee-api/wiki/WhyApiMode) for more info.

## News ##
  * 2/28/15 The code is now on [github](https://github.com/andrewrapp/xbee-arduino). I will attempt to keep this repo up to date as well but I recommend cloning or downloading from github. The documentation will stay here for now.
  * 2/1/14 Release 0.5 is [available](https://drive.google.com/folderview?id=0B_wZzfmSl2b0NmdoVzdNR1F4R00&usp=sharing). This is essentially the 0.4 Software Serial release with a bug fix or two. If upgrading from a version prior to 0.4, please note that the method for specifying the serial port has changed; see See SoftwareSerialReleaseNotes. Along with this release I have converted the repository from Subversion to Git
  * 10/15/12 Release 0.4 (beta) is [available](http://xbee-arduino.googlecode.com/files/xbee-arduino-0.4-softwareserial-beta.zip). Paul Stoffregen ([Teensy](http://www.pjrc.com/teensy/) creator) has contributed a patch that allows for using SoftwareSerial for XBee communication! This frees up the Serial port for debug or to use with other hardware. Try it out and report any issues on the Google group page. Important: See SoftwareSerialReleaseNotes as it was necessary to change the API to support this feature.
  * 12/21/11 Release 0.3 is now available.  This release includes support for Arduino 1.0 along with some bug fixes and a new setSerial function for using alternate serial ports (e.g. Mega).  This release is compatible with previous Arduino releases as well.
  * 4/3/11 I have created an [XBeeUseCases](http://code.google.com/p/xbee-api/wiki/XBeeUseCases) wiki on XBee API that describes several use cases for communicating with XBees.
  * 11/14/09 Version 0.2.1 is available.  This release contains a bug fix for Remote AT
  * 10/26/09 XBee-Arduino 0.2 is now available.  This release adds support for AT Command, Remote AT, and I/O sample (series 1 and 2) packets.  Along with this release I have created several new examples.
  * 8/09/09 I have released [Droplet](http://rapplogic.blogspot.com/2009/08/droplet.html), a wireless LCD display/remote control with support for Twitter, Google Calendar, weather etc.  It uses this software to send and receive XBee packets.
  * 4/19/09 Release 0.1.2: In this release I added some abbreviated constructors for creating basic Requests and get/set methods to facilitate the reuse of Requests
  * 3/29/09 Initial Release

## Documentation ##

Doxygen API documentation is available in the downloads. Unfortunately it is not available online anymore as Git does not support the html mime type as Subversion does


## Download ##

Google Project Hosting no longer supports downloads. You can download via [Google Drive](https://drive.google.com/folderview?id=0B_wZzfmSl2b0NmdoVzdNR1F4R00&usp=sharing) or clone the git repository

## Example ##

I have created several sketches of sending/receiving packets with Series 1 and 2 XBee radios. You can find these in the [examples](http://code.google.com/p/xbee-arduino/source/browse/#svn/trunk/examples) folder.  Here's an example of sending a packet with a Series 2 radio:

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

See the examples folder for the full source.  There are more examples in the download.

See the [XBee API](http://code.google.com/p/xbee-api/) project for Arduino < - > Computer communication.

To add XBee support to a new sketch, add "#include <XBee.h>" (without quotes) to the top of your sketch.  You can also add it by selecting the "sketch" menu, and choosing "Import Library->XBee".

## Learning/Books ##

If you want to learn more about Arduino and XBee, check out these books:

  * [Wireless Sensor Networks: with ZigBee, XBee, Arduino, and Processing](http://www.amazon.com/gp/product/0596807732?ie=UTF8&tag=xbapra-20&linkCode=as2&camp=1789&creative=9325&creativeASIN=0596807732Building) (Available in Kindle)
  * [Programming Arduino Getting Started with Sketches](http://www.amazon.com/gp/product/0071784225/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=0071784225&linkCode=as2&tag=xbapra-20)
  * [Making Things Talk](http://www.amazon.com/gp/product/1449392431/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=1449392431&linkCode=as2&tag=xbapra-20)
  * [Getting Started with Arduino (Make: Projects](http://www.amazon.com/gp/product/B0027HY20I/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=B0027HY20I&linkCode=as2&tag=xbapra-20) (Available in Kindle)
  * [Arduino Cookbook (Oreilly Cookbooks)](http://www.amazon.com/gp/product/1449313876/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=1449313876&linkCode=as2&tag=xbapra-20) (Available in Kindle)

## Hardware ##

I highly recommend using an Arduino that has 2 serial ports, such as the Arduino Leonardo. The reason is the XBee requires one serial port and it's useful to have another serial port available for debugging. The Arduino Leonard actually has one hardware serial port and one virtual (usb serial port). The hardware serial port is for use with XBee (Serial1) and the virtual serial port (Serial) may be used to send debug info to the console. The Arduino Mega is another popular choice as it has 4 hardware serial ports, along with additional memory and I/O. Both boards are compatible with XBee Shields. The [Teensy](http://www.pjrc.com/teensy/) board, by PJRC,  may be a good choice for some advanced users. It is not 100% Arduino compatible but PJRC provides a plugin to the Arduino IDE. They also sell an [XBee Adapter](http://www.pjrc.com/store/xbee_adaptor.html) to interface XBee with Teensy (requires soldering).

  * [Arduino Leonardo](http://www.amazon.com/gp/product/B008A36HQA/ref=as_li_qf_sp_asin_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B008A36HQA&linkCode=as2&tag=xbapra-20) (recommended)
  * [Arduino UNO R3](http://www.amazon.com/gp/product/B006H06TVG/ref=as_li_qf_sp_asin_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B006H06TVG&linkCode=as2&tag=xbapra-20) (note: only has one serial port)
  * [Arduino Mega 2560](http://www.amazon.com/gp/product/B006H0DWZW/ref=as_li_qf_sp_asin_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B006H0DWZW&linkCode=as2&tag=xbapra-20)

XBee radios come in two models: Series 1 (S1) and Series 2 (S2). Series 1 are the best choice for most people as they are the easiest to configure. Series 2 XBee radios feature ZigBee and require a firmware update to use this software (requires X-CTU on Windows). Series 1 and 2 are not compatible with each other.

  * [XBee Series 1 ](http://www.amazon.com/gp/product/B004G4ZHK4/ref=as_li_qf_sp_asin_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B004G4ZHK4&linkCode=as2&tag=xbapra-20)
  * [XBee Series 2 (ZigBee)](http://www.amazon.com/gp/product/B007R9U1QA/ref=as_li_qf_sp_asin_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B007R9U1QA&linkCode=as2&tag=xbapra-20)

The Arduino XBee Shield is the best choice for connecting the XBee to an Arduino

  * [XBee Shield](http://www.amazon.com/gp/product/B006TQ30TW/ref=as_li_qf_sp_asin_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B006TQ30TW&linkCode=as2&tag=xbapra-20)

You can find XBee Shields on ebay, often for less, but keep in mind delivery may be slow when shipped from China. I don't recommend the SparkFun XBee Shield since it requires soldering headers (not included) to connect to an Arduino board

An [XBee Explorer](http://www.amazon.com/gp/product/B004G4XUXU/ref=as_li_ss_tl?ie=UTF8&tag=xbapra-20&linkCode=as2&camp=217145&creative=399373&creativeASIN=B004G4XUXU) is highly recommended for updating firmware and configuring the radio.  This is also useful for interfacing an XBee with a computer. If you are using Series 2 radios you'll need an XBee Explorer to upload API firmware to the radio (they ship with AT firmware).

See the xbee-api [Hardware](https://code.google.com/p/xbee-api/wiki/Hardware?ts=1371045159&updated=Hardware) wiki for more parts links.

For interfacing with Arduino clones that are not shield compatible, such as Modern Device's http://shop.moderndevice.com/products/rbbb-kit, you will need 3.3V power and logic shifter or voltage divider to convert Arduino's 5V to an XBee safe 3.3V.

## Installation ##

Arduino 16 (or earlier):

Download the zip file, extract and copy the XBee folder to ARDUINO\_HOME/hardware/libraries
If you are upgrading from a previous version, be sure to delete XBee.o

Arduino 17 (or later):

Determine the location of your sketchbook by selecting "preferences" on the Arduino menu.  Create a "libraries" folder in your sketchbook and unzip the download there.  See [this](http://arduino.cc/blog/?p=313) for more information.

## Uploading Sketches ##

The Arduino has only one serial port which must be connected to USB (FTDI) for uploading sketches and to the XBee for running sketches.  The Arduino XBee Shield provides a set of jumpers to direct Serial communication to either the USB (Arduino IDE) or the XBee.  When using the XBee Shield you will need to place both the jumpers in the USB position prior to uploading your sketch.  Then after a successful upload, place the jumpers in the "XBEE" position to run your sketch.  Always remember to power off the Arduino before moving the jumpers.

## Configuration ##

To use this library your XBee must be configured in API mode (AP=2).  Take a look at [this](http://code.google.com/p/xbee-api/wiki/XBeeConfiguration) for information on configuring your radios to form a network.

## Other Micros ##

Not using Arduino?  It should be easy to port this library to any microcontroller that supports C++ and serial available/read/write/flush.  The only other dependency is the `millis()` function for milliseconds.

## Support ##

Please report any bugs on the Issue Tracker.

## Questions/Feedback ##

Questions about this project should be posted to  http://groups.google.com/group/xbee-api?pli=1  Be sure to provide as much detail as possible (e.g. what radios s1 or s2, firmware versions, configuration and code).

## Consulting/Commercial Licenses ##

I'm available for consulting to assist businesses or entrepreneurs that need help getting their projects up and running. I can also provide a commercial license for situations where you need to distribute code to clients/third parties that would otherwise conflict with GPL. For these matters I can be contacted at andrew.rapp `[`at`]` gmail.
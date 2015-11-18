Adding support for SoftwareSerial (>= release 0.4 beta) required a change to the API. Previously, the library was responsible for starting the serial port, with a call to the begin function:

```
xbee.begin(9600);
```

Now, the serial port is started with an explicit call and the setSerial function must be called to tell the Library which serial port object to use:

```
Serial.begin(9600);
xbee.setSerial(Serial);
```

The reason this change was necessary is you can't have XBee.begin(baud), due to the way Arduino's stream class is defined.

The same syntax is used for SoftwareSerial, e.g:

```
  softserial.begin(9600);
  xbee.setSerial(softserial);
```

When using Arduino Leonardo or similar variants that have two serial ports: USB serial and UART. You must specify the UART (Serial1) for use with XBee. Serial goes to the serial monitor and is useful debugging

```
  Serial1.begin(9600);
  xbee.setSerial(Serial1);

  // Use Serial for printing to the serial monitor
  Serial.begin(9600);
  // wait for leonardo Serial
  while (!Serial);
  Serial.println("XBee sketch is running!");
```

The examples in the download have been updated to reflect this API change.

Hardware serial is still recommended for the best performance, however, if you use SoftwareSerial, it's recommended to use [AltSoftSerial](http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) (by Paul Stoffregen). This library offers better performance than the SoftwareSerial library, included with the Arduino software. It does have a few limitations which are explained on the AltSoftSerial site.
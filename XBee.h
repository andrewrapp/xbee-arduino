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

#ifndef XBee_h
#define XBee_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <inttypes.h>

#define SERIES_1
#define SERIES_2

// set to ATAP value of XBee. AP=2 is recommended
#define ATAP 2

#define START_BYTE 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13

// This value determines the size of the byte array for receiving RX packets
// Most users won't be dealing with packets this large so you can adjust this
// value to reduce memory consumption. But, remember that
// if a RX packet exceeds this size, it cannot be parsed!

// This value is determined by the largest packet size (100 byte payload + 64-bit address + option byte and rssi byte) of a series 1 radio
#define MAX_FRAME_DATA_SIZE 110

#define BROADCAST_ADDRESS 0xffff
#define ZB_BROADCAST_ADDRESS 0xfffe

// the non-variable length of the frame data (not including frame id or api id or variable data size (e.g. payload, at command set value)
#define ZB_TX_API_LENGTH 12
#define ZB_EXPLICIT_TX_API_LENGTH 18
#define TX_16_API_LENGTH 3
#define TX_64_API_LENGTH 9
#define AT_COMMAND_API_LENGTH 2
#define REMOTE_AT_COMMAND_API_LENGTH 13
// start/length(2)/api/frameid/checksum bytes
#define PACKET_OVERHEAD_LENGTH 6
// api is always the third byte in packet
#define API_ID_INDEX 3

// frame position of rssi byte
#define RX_16_RSSI_OFFSET 2
#define RX_64_RSSI_OFFSET 8

#define DEFAULT_FRAME_ID 1
#define NO_RESPONSE_FRAME_ID 0

// These are the parameters used by the XBee ZB modules when you do a
// regular "ZB TX request".
#define DEFAULT_ENDPOINT 232
#define DEFAULT_CLUSTER_ID 0x0011
#define DEFAULT_PROFILE_ID 0xc105

// TODO put in tx16 class
#define ACK_OPTION 0
#define DISABLE_ACK_OPTION 1
#define BROADCAST_OPTION 4

// RX options
#define ZB_PACKET_ACKNOWLEDGED 0x01
#define ZB_BROADCAST_PACKET 0x02

// not everything is implemented!
/**
 * Api Id constants
 */
#define TX_64_REQUEST 0x0
#define TX_16_REQUEST 0x1
#define AT_COMMAND_REQUEST 0x08
#define AT_COMMAND_QUEUE_REQUEST 0x09
#define REMOTE_AT_REQUEST 0x17
#define ZB_TX_REQUEST 0x10
#define ZB_EXPLICIT_TX_REQUEST 0x11
#define RX_64_RESPONSE 0x80
#define RX_16_RESPONSE 0x81
#define RX_64_IO_RESPONSE 0x82
#define RX_16_IO_RESPONSE 0x83
#define AT_RESPONSE 0x88
#define TX_STATUS_RESPONSE 0x89
#define MODEM_STATUS_RESPONSE 0x8a
#define ZB_RX_RESPONSE 0x90
#define ZB_EXPLICIT_RX_RESPONSE 0x91
#define ZB_TX_STATUS_RESPONSE 0x8b
#define ZB_IO_SAMPLE_RESPONSE 0x92
#define ZB_IO_NODE_IDENTIFIER_RESPONSE 0x95
#define AT_COMMAND_RESPONSE 0x88
#define REMOTE_AT_COMMAND_RESPONSE 0x97


/**
 * TX STATUS constants
 */
#define	SUCCESS 0x0
#define CCA_FAILURE 0x2
#define INVALID_DESTINATION_ENDPOINT_SUCCESS 0x15
#define	NETWORK_ACK_FAILURE 0x21
#define NOT_JOINED_TO_NETWORK 0x22
#define	SELF_ADDRESSED 0x23
#define ADDRESS_NOT_FOUND 0x24
#define ROUTE_NOT_FOUND 0x25
#define PAYLOAD_TOO_LARGE 0x74
// Returned by XBeeWithCallbacks::waitForStatus on timeout
#define XBEE_WAIT_TIMEOUT 0xff

// modem status
#define HARDWARE_RESET 0
#define WATCHDOG_TIMER_RESET 1
#define ASSOCIATED 2
#define DISASSOCIATED 3
#define SYNCHRONIZATION_LOST 4
#define COORDINATOR_REALIGNMENT 5
#define COORDINATOR_STARTED 6

#define ZB_BROADCAST_RADIUS_MAX_HOPS 0

#define ZB_TX_UNICAST 0
#define ZB_TX_BROADCAST 8

#define AT_OK 0
#define AT_ERROR  1
#define AT_INVALID_COMMAND 2
#define AT_INVALID_PARAMETER 3
#define AT_NO_RESPONSE 4

#define NO_ERROR 0
#define CHECKSUM_FAILURE 1
#define PACKET_EXCEEDS_BYTE_ARRAY_LENGTH 2
#define UNEXPECTED_START_BYTE 3

/**
 * C++11 introduced the constexpr as a hint to the compiler that things
 * can be evaluated at compiletime. This can help to remove
 * startup code for global objects, or otherwise help the compiler to
 * optimize. Since the keyword is introduced in C++11, but supporting
 * older compilers is a matter of removing the keyword, we use a macro
 * for this.
 */
#if __cplusplus >= 201103L
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

/**
 * The super class of all XBee responses (RX packets)
 * Users should never attempt to create an instance of this class; instead
 * create an instance of a subclass
 * It is recommend to reuse subclasses to conserve memory
 */
class XBeeResponse {
public:
	//static const int MODEM_STATUS = 0x8a;
	/**
	 * Default constructor
	 */
	XBeeResponse();
	/**
	 * Returns Api Id of the response
	 */
	uint8_t getApiId();
	void setApiId(uint8_t apiId);
	/**
	 * Returns the MSB length of the packet
	 */
	uint8_t getMsbLength();
	void setMsbLength(uint8_t msbLength);
	/**
	 * Returns the LSB length of the packet
	 */
	uint8_t getLsbLength();
	void setLsbLength(uint8_t lsbLength);
	/**
	 * Returns the packet checksum
	 */
	uint8_t getChecksum();
	void setChecksum(uint8_t checksum);
	/**
	 * Returns the length of the frame data: all bytes after the api id, and prior to the checksum
	 * Note up to release 0.1.2, this was incorrectly including the checksum in the length.
	 */
	uint8_t getFrameDataLength();
	void setFrameData(uint8_t* frameDataPtr);
	/**
	 * Returns the buffer that contains the response.
	 * Starts with byte that follows API ID and includes all bytes prior to the checksum
	 * Length is specified by getFrameDataLength()
	 * Note: Unlike Digi's definition of the frame data, this does not start with the API ID..
	 * The reason for this is all responses include an API ID, whereas my frame data
	 * includes only the API specific data.
	 */
	uint8_t* getFrameData();

	void setFrameLength(uint8_t frameLength);
	// to support future 65535 byte packets I guess
	/**
	 * Returns the length of the packet
	 */
	uint16_t getPacketLength();
	/**
	 * Resets the response to default values
	 */
	void reset();
	/**
	 * Initializes the response
	 */
	void init();
#ifdef SERIES_2
	/**
	 * Call with instance of ZBTxStatusResponse class only if getApiId() == ZB_TX_STATUS_RESPONSE
	 * to populate response
	 */
	void getZBTxStatusResponse(XBeeResponse &response);
	/**
	 * Call with instance of ZBRxResponse class only if getApiId() == ZB_RX_RESPONSE
	 * to populate response
	 */
	void getZBRxResponse(XBeeResponse &response);
	/**
	 * Call with instance of ZBExplicitRxResponse class only if getApiId() == ZB_EXPLICIT_RX_RESPONSE
	 * to populate response
	 */
	void getZBExplicitRxResponse(XBeeResponse &response);
	/**
	 * Call with instance of ZBRxIoSampleResponse class only if getApiId() == ZB_IO_SAMPLE_RESPONSE
	 * to populate response
	 */
	void getZBRxIoSampleResponse(XBeeResponse &response);
#endif
#ifdef SERIES_1
	/**
	 * Call with instance of TxStatusResponse only if getApiId() == TX_STATUS_RESPONSE
	 */
	void getTxStatusResponse(XBeeResponse &response);
	/**
	 * Call with instance of Rx16Response only if getApiId() == RX_16_RESPONSE
	 */
	void getRx16Response(XBeeResponse &response);
	/**
	 * Call with instance of Rx64Response only if getApiId() == RX_64_RESPONSE
	 */
	void getRx64Response(XBeeResponse &response);
	/**
	 * Call with instance of Rx16IoSampleResponse only if getApiId() == RX_16_IO_RESPONSE
	 */
	void getRx16IoSampleResponse(XBeeResponse &response);
	/**
	 * Call with instance of Rx64IoSampleResponse only if getApiId() == RX_64_IO_RESPONSE
	 */
	void getRx64IoSampleResponse(XBeeResponse &response);
#endif
	/**
	 * Call with instance of AtCommandResponse only if getApiId() == AT_COMMAND_RESPONSE
	 */
	void getAtCommandResponse(XBeeResponse &responses);
	/**
	 * Call with instance of RemoteAtCommandResponse only if getApiId() == REMOTE_AT_COMMAND_RESPONSE
	 */
	void getRemoteAtCommandResponse(XBeeResponse &response);
	/**
	 * Call with instance of ModemStatusResponse only if getApiId() == MODEM_STATUS_RESPONSE
	 */
	void getModemStatusResponse(XBeeResponse &response);
	/**
	 * Returns true if the response has been successfully parsed and is complete and ready for use
	 */
	bool isAvailable();
	void setAvailable(bool complete);
	/**
	 * Returns true if the response contains errors
	 */
	bool isError();
	/**
	 * Returns an error code, or zero, if successful.
	 * Error codes include: CHECKSUM_FAILURE, PACKET_EXCEEDS_BYTE_ARRAY_LENGTH, UNEXPECTED_START_BYTE
	 */
	uint8_t getErrorCode();
	void setErrorCode(uint8_t errorCode);
protected:
	// pointer to frameData
	uint8_t* _frameDataPtr;
private:
	void setCommon(XBeeResponse &target);
	uint8_t _apiId;
	uint8_t _msbLength;
	uint8_t _lsbLength;
	uint8_t _checksum;
	uint8_t _frameLength;
	bool _complete;
	uint8_t _errorCode;
};

class XBeeAddress {
public:
	CONSTEXPR XBeeAddress() {};
};

/**
 * Represents a 64-bit XBee Address
 *
 * Note that avr-gcc as of 4.9 doesn't optimize uint64_t very well, so
 * for the smallest and fastest code, use msb and lsb separately. See
 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66511
 */
class XBeeAddress64 : public XBeeAddress {
public:
	CONSTEXPR XBeeAddress64(uint64_t addr) : _msb(addr >> 32), _lsb(addr) {}
	CONSTEXPR XBeeAddress64(uint32_t msb, uint32_t lsb) : _msb(msb), _lsb(lsb) {}
	CONSTEXPR XBeeAddress64() : _msb(0), _lsb(0) {}
	uint32_t getMsb() {return _msb;}
	uint32_t getLsb() {return _lsb;}
	uint64_t get() {return (static_cast<uint64_t>(_msb) << 32) | _lsb;}
	operator uint64_t() {return get();}
	void setMsb(uint32_t msb) {_msb = msb;}
	void setLsb(uint32_t lsb) {_lsb = lsb;}
	void set(uint64_t addr) {
		_msb = addr >> 32;
		_lsb = addr;
	}
private:
	// Once https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66511 is
	// fixed, it might make sense to merge these into a uint64_t.
	uint32_t _msb;
	uint32_t _lsb;
};

//class XBeeAddress16 : public XBeeAddress {
//public:
//	XBeeAddress16(uint16_t addr);
//	XBeeAddress16();
//	uint16_t getAddress();
//	void setAddress(uint16_t addr);
//private:
//	uint16_t _addr;
//};

/**
 * This class is extended by all Responses that include a frame id
 */
class FrameIdResponse : public XBeeResponse {
public:
	FrameIdResponse();
	uint8_t getFrameId();
private:
	uint8_t _frameId;
};

/**
 * Common functionality for both Series 1 and 2 data RX data packets
 */
class RxDataResponse : public XBeeResponse {
public:
	RxDataResponse();
	/**
	 * Returns the specified index of the payload.  The index may be 0 to getDataLength() - 1
	 * This method is deprecated; use uint8_t* getData()
	 */
	uint8_t getData(int index);
	/**
	 * Returns the payload array.  This may be accessed from index 0 to getDataLength() - 1
	 */
	uint8_t* getData();
	/**
	 * Returns the length of the payload
	 */
	virtual uint8_t getDataLength() = 0;
	/**
	 * Returns the position in the frame data where the data begins
	 */
	virtual uint8_t getDataOffset() = 0;
};

// getResponse to return the proper subclass:
// we maintain a pointer to each type of response, when a response is parsed, it is allocated only if NULL
// can we allocate an object in a function?

#ifdef SERIES_2
/**
 * Represents a Series 2 TX status packet
 */
class ZBTxStatusResponse : public FrameIdResponse {
	public:
		ZBTxStatusResponse();
		uint16_t getRemoteAddress();
		uint8_t getTxRetryCount();
		uint8_t getDeliveryStatus();
		uint8_t getDiscoveryStatus();
		bool isSuccess();

	static const uint8_t API_ID = ZB_TX_STATUS_RESPONSE;
};

/**
 * Represents a Series 2 RX packet
 */
class ZBRxResponse : public RxDataResponse {
public:
	ZBRxResponse();
	XBeeAddress64& getRemoteAddress64();
	uint16_t getRemoteAddress16();
	uint8_t getOption();
	uint8_t getDataLength();
	// frame position where data starts
	uint8_t getDataOffset();

	static const uint8_t API_ID = ZB_RX_RESPONSE;
private:
	XBeeAddress64 _remoteAddress64;
};

/**
 * Represents a Series 2 Explicit RX packet
 *
 * Note: The receive these responses, set AO=1. With the default AO=0,
 * you will receive ZBRxResponses, not knowing exact details.
 */
class ZBExplicitRxResponse : public ZBRxResponse {
public:
	ZBExplicitRxResponse();
	uint8_t getSrcEndpoint();
	uint8_t getDstEndpoint();
	uint16_t getClusterId();
	uint16_t getProfileId();
	uint8_t getOption();
	uint8_t getDataLength();
	// frame position where data starts
	uint8_t getDataOffset();

	static const uint8_t API_ID = ZB_EXPLICIT_RX_RESPONSE;
};

/**
 * Represents a Series 2 RX I/O Sample packet
 */
class ZBRxIoSampleResponse : public ZBRxResponse {
public:
	ZBRxIoSampleResponse();
	bool containsAnalog();
	bool containsDigital();
	/**
	 * Returns true if the pin is enabled
	 */
	bool isAnalogEnabled(uint8_t pin);
	/**
	 * Returns true if the pin is enabled
	 */
	bool isDigitalEnabled(uint8_t pin);
	/**
	 * Returns the 10-bit analog reading of the specified pin.
	 * Valid pins include ADC:xxx.
	 */
	uint16_t getAnalog(uint8_t pin);
	/**
	 * Returns true if the specified pin is high/on.
	 * Valid pins include DIO:xxx.
	 */
	bool isDigitalOn(uint8_t pin);
	uint8_t getDigitalMaskMsb();
	uint8_t getDigitalMaskLsb();
	uint8_t getAnalogMask();

	static const uint8_t API_ID = ZB_IO_SAMPLE_RESPONSE;
};

#endif

#ifdef SERIES_1
/**
 * Represents a Series 1 TX Status packet
 */
class TxStatusResponse : public FrameIdResponse {
	public:
		TxStatusResponse();
		uint8_t getStatus();
		bool isSuccess();

	static const uint8_t API_ID = TX_STATUS_RESPONSE;
};

/**
 * Represents a Series 1 RX packet
 */
class RxResponse : public RxDataResponse {
public:
	RxResponse();
	// remember rssi is negative but this is unsigned byte so it's up to you to convert
	uint8_t getRssi();
	uint8_t getOption();
	bool isAddressBroadcast();
	bool isPanBroadcast();
	uint8_t getDataLength();
	uint8_t getDataOffset();
	virtual uint8_t getRssiOffset() = 0;
};

/**
 * Represents a Series 1 16-bit address RX packet
 */
class Rx16Response : public RxResponse {
public:
	Rx16Response();
	uint8_t getRssiOffset();
	uint16_t getRemoteAddress16();

	static const uint8_t API_ID = RX_16_RESPONSE;
protected:
	uint16_t _remoteAddress;
};

/**
 * Represents a Series 1 64-bit address RX packet
 */
class Rx64Response : public RxResponse {
public:
	Rx64Response();
	uint8_t getRssiOffset();
	XBeeAddress64& getRemoteAddress64();

	static const uint8_t API_ID = RX_64_RESPONSE;
private:
	XBeeAddress64 _remoteAddress;
};

/**
 * Represents a Series 1 RX I/O Sample packet
 */
class RxIoSampleBaseResponse : public RxResponse {
	public:
		RxIoSampleBaseResponse();
		/**
		 * Returns the number of samples in this packet
		 */
		uint8_t getSampleSize();
		bool containsAnalog();
		bool containsDigital();
		/**
		 * Returns true if the specified analog pin is enabled
		 */
		bool isAnalogEnabled(uint8_t pin);
		/**
		 * Returns true if the specified digital pin is enabled
		 */
		bool isDigitalEnabled(uint8_t pin);
		/**
		 * Returns the 10-bit analog reading of the specified pin.
		 * Valid pins include ADC:0-5.  Sample index starts at 0
		 */
		uint16_t getAnalog(uint8_t pin, uint8_t sample);
		/**
		 * Returns true if the specified pin is high/on.
		 * Valid pins include DIO:0-8.  Sample index starts at 0
		 */
		bool isDigitalOn(uint8_t pin, uint8_t sample);
		uint8_t getSampleOffset();

		/**
		 * Gets the offset of the start of the given sample.
		 */
		uint8_t getSampleStart(uint8_t sample);
	private:
};

class Rx16IoSampleResponse : public RxIoSampleBaseResponse {
public:
	Rx16IoSampleResponse();
	uint16_t getRemoteAddress16();
	uint8_t getRssiOffset();

	static const uint8_t API_ID = RX_16_IO_RESPONSE;
};

class Rx64IoSampleResponse : public RxIoSampleBaseResponse {
public:
	Rx64IoSampleResponse();
	XBeeAddress64& getRemoteAddress64();
	uint8_t getRssiOffset();

	static const uint8_t API_ID = RX_64_IO_RESPONSE;
private:
	XBeeAddress64 _remoteAddress;
};

#endif

/**
 * Represents a Modem Status RX packet
 */
class ModemStatusResponse : public XBeeResponse {
public:
	ModemStatusResponse();
	uint8_t getStatus();

	static const uint8_t API_ID = MODEM_STATUS_RESPONSE;
};

/**
 * Represents an AT Command RX packet
 */
class AtCommandResponse : public FrameIdResponse {
	public:
		AtCommandResponse();
		/**
		 * Returns an array containing the two character command
		 */
		uint8_t* getCommand();
		/**
		 * Returns the command status code.
		 * Zero represents a successful command
		 */
		uint8_t getStatus();
		/**
		 * Returns an array containing the command value.
		 * This is only applicable to query commands.
		 */
		uint8_t* getValue();
		/**
		 * Returns the length of the command value array.
		 */
		uint8_t getValueLength();
		/**
		 * Returns true if status equals AT_OK
		 */
		bool isOk();

		static const uint8_t API_ID = AT_COMMAND_RESPONSE;
};

/**
 * Represents a Remote AT Command RX packet
 */
class RemoteAtCommandResponse : public AtCommandResponse {
	public:
		RemoteAtCommandResponse();
		/**
		 * Returns an array containing the two character command
		 */
		uint8_t* getCommand();
		/**
		 * Returns the command status code.
		 * Zero represents a successful command
		 */
		uint8_t getStatus();
		/**
		 * Returns an array containing the command value.
		 * This is only applicable to query commands.
		 */
		uint8_t* getValue();
		/**
		 * Returns the length of the command value array.
		 */
		uint8_t getValueLength();
		/**
		 * Returns the 16-bit address of the remote radio
		 */
		uint16_t getRemoteAddress16();
		/**
		 * Returns the 64-bit address of the remote radio
		 */
		XBeeAddress64& getRemoteAddress64();
		/**
		 * Returns true if command was successful
		 */
		bool isOk();

		static const uint8_t API_ID = REMOTE_AT_COMMAND_RESPONSE;
	private:
		XBeeAddress64 _remoteAddress64;
};


/**
 * Super class of all XBee requests (TX packets)
 * Users should never create an instance of this class; instead use an subclass of this class
 * It is recommended to reuse Subclasses of the class to conserve memory
 * <p/>
 * This class allocates a buffer to
 */
class XBeeRequest {
public:
	/**
	 * Constructor
	 * TODO make protected
	 */
	XBeeRequest(uint8_t apiId, uint8_t frameId);
	/**
	 * Sets the frame id.  Must be between 1 and 255 inclusive to get a TX status response.
	 */
	void setFrameId(uint8_t frameId);
	/**
	 * Returns the frame id
	 */
	uint8_t getFrameId();
	/**
	 * Returns the API id
	 */
	uint8_t getApiId();
	// setting = 0 makes this a pure virtual function, meaning the subclass must implement, like abstract in java
	/**
	 * Starting after the frame id (pos = 0) and up to but not including the checksum
	 * Note: Unlike Digi's definition of the frame data, this does not start with the API ID.
	 * The reason for this is the API ID and Frame ID are common to all requests, whereas my definition of
	 * frame data is only the API specific data.
	 */
	virtual uint8_t getFrameData(uint8_t pos) = 0;
	/**
	 * Returns the size of the api frame (not including frame id or api id or checksum).
	 */
	virtual uint8_t getFrameDataLength() = 0;
	//void reset();
protected:
	void setApiId(uint8_t apiId);
private:
	uint8_t _apiId;
	uint8_t _frameId;
};

// TODO add reset/clear method since responses are often reused
/**
 * Primary interface for communicating with an XBee Radio.
 * This class provides methods for sending and receiving packets with an XBee radio via the serial port.
 * The XBee radio must be configured in API (packet) mode (AP=2)
 * in order to use this software.
 * <p/>
 * Since this code is designed to run on a microcontroller, with only one thread, you are responsible for reading the
 * data off the serial buffer in a timely manner.  This involves a call to a variant of readPacket(...).
 * If your serial port is receiving data faster than you are reading, you can expect to lose packets.
 * Arduino only has a 128 byte serial buffer so it can easily overflow if two or more packets arrive
 * without a call to readPacket(...)
 * <p/>
 * In order to conserve resources, this class only supports storing one response packet in memory at a time.
 * This means that you must fully consume the packet prior to calling readPacket(...), because calling
 * readPacket(...) overwrites the previous response.
 * <p/>
 * This class creates an array of size MAX_FRAME_DATA_SIZE for storing the response packet.  You may want
 * to adjust this value to conserve memory.
 *
 * \author Andrew Rapp
 */
class XBee {
public:
	XBee();
	/**
	 * Reads all available serial bytes until a packet is parsed, an error occurs, or the buffer is empty.
	 * You may call <i>xbee</i>.getResponse().isAvailable() after calling this method to determine if
	 * a packet is ready, or <i>xbee</i>.getResponse().isError() to determine if
	 * a error occurred.
	 * <p/>
	 * This method should always return quickly since it does not wait for serial data to arrive.
	 * You will want to use this method if you are doing other timely stuff in your loop, where
	 * a delay would cause problems.
	 * NOTE: calling this method resets the current response, so make sure you first consume the
	 * current response
	 */
	void readPacket();
	/**
	 * Waits a maximum of <i>timeout</i> milliseconds for a response packet before timing out; returns true if packet is read.
	 * Returns false if timeout or error occurs.
	 */
	bool readPacket(int timeout);
	/**
	 * Reads until a packet is received or an error occurs.
	 * Caution: use this carefully since if you don't get a response, your Arduino code will hang on this
	 * call forever!! often it's better to use a timeout: readPacket(int)
	 */
	void readPacketUntilAvailable();
	/**
	 * Starts the serial connection on the specified serial port
	 */
	void begin(Stream &serial);
	void getResponse(XBeeResponse &response);
	/**
	 * Returns a reference to the current response
	 * Note: once readPacket is called again this response will be overwritten!
	 */
	XBeeResponse& getResponse();
	/**
	 * Sends a XBeeRequest (TX packet) out the serial port
	 */
	void send(XBeeRequest &request);
	//uint8_t sendAndWaitForResponse(XBeeRequest &request, int timeout);
	/**
	 * Returns a sequential frame id between 1 and 255
	 */
	uint8_t getNextFrameId();
	/**
	 * Specify the serial port.  Only relevant for Arduinos that support multiple serial ports (e.g. Mega)
	 */
	void setSerial(Stream &serial);
private:
	bool available();
	uint8_t read();
	void flush();
	void write(uint8_t val);
	void sendByte(uint8_t b, bool escape);
	void resetResponse();
	XBeeResponse _response;
	bool _escape;
	// current packet position for response.  just a state variable for packet parsing and has no relevance for the response otherwise
	uint8_t _pos;
	// last byte read
	uint8_t b;
	uint8_t _checksumTotal;
	uint8_t _nextFrameId;
	// buffer for incoming RX packets.  holds only the api specific frame data, starting after the api id byte and prior to checksum
	uint8_t _responseFrameData[MAX_FRAME_DATA_SIZE];
	Stream* _serial;
};


/**
 * This class can be used instead of the XBee class and allows
 * user-specified callback functions to be called when responses are
 * received, simplifying the processing code and reducing boilerplate.
 *
 * To use it, first register your callback functions using the onXxx
 * methods. Each method has a uintptr_t data argument, that can be used to
 * pass arbitrary data to the callback (useful when using the same
 * function for multiple callbacks, or have a generic function that can
 * behave differently in different circumstances). Supplying the data
 * parameter is optional, but the callback must always accept it (just
 * ignore it if it's unused). The uintptr_t type is an integer type
 * guaranteed to be big enough to fit a pointer (it is 16-bit on AVR,
 * 32-bit on ARM), so it can also be used to store a pointer to access
 * more data if required (using proper casts).
 *
 * There can be only one callback of each type registered at one time,
 * so registering callback overwrites any previously registered one. To
 * unregister a callback, pass NULL as the function.
 *
 * To ensure that the callbacks are actually called, call the loop()
 * method regularly (in your loop() function, for example). This takes
 * care of calling readPacket() and getResponse() other methods on the
 * XBee class, so there is no need to do so directly (though it should
 * not mess with this class if you do, it would only mean some callbacks
 * aren't called).
 *
 * Inside callbacks, you should generally not be blocking / waiting.
 * Since callbacks can be called from inside waitFor() and friends, a
 * callback that doesn't return quickly can mess up the waitFor()
 * timeout.
 *
 * Sending packets is not a problem inside a callback, but avoid
 * receiving a packet (e.g. calling readPacket(), loop() or waitFor()
 * and friends) inside a callback (since that would overwrite the
 * current response, messing up any pending callbacks and waitFor() etc.
 * methods already running).
 */
class XBeeWithCallbacks : public XBee {
public:

	/**
	 * Register a packet error callback. It is called whenever an
	 * error occurs in the packet reading process. Arguments to the
	 * callback will be the error code (as returned by
	 * XBeeResponse::getErrorCode()) and the data parameter.  while
	 * registering the callback.
	 */
	void onPacketError(void (*func)(uint8_t, uintptr_t), uintptr_t data = 0) { _onPacketError.set(func, data); }

	/**
	 * Register a response received callback. It is called whenever
	 * a response was succesfully received, before a response
	 * specific callback (or onOtherResponse) below is called.
	 *
	 * Arguments to the callback will be the received response and
	 * the data parameter passed while registering the callback.
	 */
	void onResponse(void (*func)(XBeeResponse&, uintptr_t), uintptr_t data = 0) { _onResponse.set(func, data); }

	/**
	 * Register an other response received callback. It is called
	 * whenever a response was succesfully received, but no response
	 * specific callback was registered using the functions below
	 * (after the onResponse callback is called).
	 *
	 * Arguments to the callback will be the received response and
	 * the data parameter passed while registering the callback.
	 */
	void onOtherResponse(void (*func)(XBeeResponse&, uintptr_t), uintptr_t data = 0) { _onOtherResponse.set(func, data); }

	// These functions register a response specific callback. They
	// are called whenever a response of the appropriate type was
	// succesfully received (after the onResponse callback is
	// called).
	//
	// Arguments to the callback will be the received response
	// (already converted to the appropriate type) and the data
	// parameter passed while registering the callback.
	void onZBTxStatusResponse(void (*func)(ZBTxStatusResponse&, uintptr_t), uintptr_t data = 0) { _onZBTxStatusResponse.set(func, data); }
	void onZBRxResponse(void (*func)(ZBRxResponse&, uintptr_t), uintptr_t data = 0) { _onZBRxResponse.set(func, data); }
	void onZBExplicitRxResponse(void (*func)(ZBExplicitRxResponse&, uintptr_t), uintptr_t data = 0) { _onZBExplicitRxResponse.set(func, data); }
	void onZBRxIoSampleResponse(void (*func)(ZBRxIoSampleResponse&, uintptr_t), uintptr_t data = 0) { _onZBRxIoSampleResponse.set(func, data); }
	void onTxStatusResponse(void (*func)(TxStatusResponse&, uintptr_t), uintptr_t data = 0) { _onTxStatusResponse.set(func, data); }
	void onRx16Response(void (*func)(Rx16Response&, uintptr_t), uintptr_t data = 0) { _onRx16Response.set(func, data); }
	void onRx64Response(void (*func)(Rx64Response&, uintptr_t), uintptr_t data = 0) { _onRx64Response.set(func, data); }
	void onRx16IoSampleResponse(void (*func)(Rx16IoSampleResponse&, uintptr_t), uintptr_t data = 0) { _onRx16IoSampleResponse.set(func, data); }
	void onRx64IoSampleResponse(void (*func)(Rx64IoSampleResponse&, uintptr_t), uintptr_t data = 0) { _onRx64IoSampleResponse.set(func, data); }
	void onModemStatusResponse(void (*func)(ModemStatusResponse&, uintptr_t), uintptr_t data = 0) { _onModemStatusResponse.set(func, data); }
	void onAtCommandResponse(void (*func)(AtCommandResponse&, uintptr_t), uintptr_t data = 0) { _onAtCommandResponse.set(func, data); }
	void onRemoteAtCommandResponse(void (*func)(RemoteAtCommandResponse&, uintptr_t), uintptr_t data = 0) { _onRemoteAtCommandResponse.set(func, data); }

	/**
	 * Regularly call this method, which ensures that the serial
	 * buffer is processed and the appropriate callbacks are called.
	 */
	void loop();

	/**
	 * Wait for a API response of the given type, optionally
	 * filtered by the given match function.
	 *
	 * If a match function is given it is called for every response
	 * of the right type received, passing the response and the data
	 * parameter passed to this method. If the function returns true
	 * (or if no function was passed), waiting stops and this method
	 * returns 0. If the function returns false, waiting
	 * continues. After the given timeout passes, this method
	 * returns XBEE_WAIT_TIMEOUT.
	 *
	 * If a valid frameId is passed (e.g. 0-255 inclusive) and a
	 * status API response frame is received while waiting, that has
	 * a *non-zero* status, waiting stops and that status is
	 * received. This is intended for when a TX packet was sent and
	 * you are waiting for an RX reply, which will most likely never
	 * arrive when TX failed. However, since the status reply is not
	 * guaranteed to arrive before the RX reply (a remote module can
	 * send a reply before the ACK), first calling waitForStatus()
	 * and then waitFor() can sometimes miss the reply RX packet.
	 *
	 * Note that when the intended response is received *before* the
	 * status reply, the latter will not be processed by this
	 * method and will be subsequently processed by e.g. loop()
	 * normally.
	 *
	 * While waiting, any other responses received are passed to the
	 * relevant callbacks, just as if calling loop() continuously
	 * (except for the response sought, that one is only passed to
	 * the OnResponse handler and no others).
	 *
	 * After this method returns, the response itself can still be
	 * retrieved using getResponse() as normal.
	 */
	template <typename Response>
	uint8_t waitFor(Response& response, uint16_t timeout, bool (*func)(Response&, uintptr_t) = NULL, uintptr_t data = 0, int16_t frameId = -1) {
		return waitForInternal(Response::API_ID, &response, timeout, (void*)func, data, frameId);
	}

	/**
	 * Sends a XBeeRequest (TX packet) out the serial port, and wait
	 * for a status response API frame (up until the given timeout).
	 * Essentially this just calls send() and waitForStatus().
	 * See waitForStatus for the meaning of the return value and
	 * more details.
	 */
	uint8_t sendAndWait(XBeeRequest &request, uint16_t timeout) {
		send(request);
		return waitForStatus(request.getFrameId(), timeout);
	}

	/**
	 * Wait for a status API response with the given frameId and
	 * return the status from the packet (for ZB_TX_STATUS_RESPONSE,
	 * this returns just the delivery status, not the routing
	 * status). If the timeout is reached before reading the
	 * response, XBEE_WAIT_TIMEOUT is returned instead.
	 *
	 * While waiting, any other responses received are passed to the
	 * relevant callbacks, just as if calling loop() continuously
	 * (except for the status response sought, that one is only
	 * passed to the OnResponse handler and no others).
	 *
	 * After this method returns, the response itself can still be
	 * retrieved using getResponse() as normal.
	 */
	uint8_t waitForStatus(uint8_t frameId, uint16_t timeout);
private:
	/**
	 * Internal version of waitFor that does not need to be
	 * templated (to prevent duplication the implementation for
	 * every response type you might want to wait for). Instead of
	 * using templates, this accepts the apiId to wait for and will
	 * cast the given response object and the argument to the given
	 * function to the corresponding type. This means that the
	 * void* given must match the api id!
	 */
	uint8_t waitForInternal(uint8_t apiId, void *response, uint16_t timeout, void *func, uintptr_t data, int16_t frameId);

	/**
	 * Helper that checks if the current response is a status
	 * response with the given frame id. If so, returns the status
	 * byte from the response, otherwise returns 0xff.
	 */
	uint8_t matchStatus(uint8_t frameId);

	/**
	 * Top half of a typical loop(). Calls readPacket(), calls
	 * onPacketError on error, calls onResponse when a response is
	 * available. Returns in the true in the latter case, after
	 * which a caller should typically call loopBottom().
	 */
	bool loopTop();

	/**
	 * Bottom half of a typical loop. Call only when a valid
	 * response was read, will call all response-specific callbacks.
	 */
	void loopBottom();

	template <typename Arg> struct Callback {
		void (*func)(Arg, uintptr_t);
		uintptr_t data;
		void set(void (*func)(Arg, uintptr_t), uintptr_t data) {
			this->func = func;
			this->data = data;
		}
		bool call(Arg arg) {
			if (this->func) {
				this->func(arg, this->data);
				return true;
			}
			return false;
		}
	};

	Callback<uint8_t> _onPacketError;
	Callback<XBeeResponse&> _onResponse;
	Callback<XBeeResponse&> _onOtherResponse;
	Callback<ZBTxStatusResponse&> _onZBTxStatusResponse;
	Callback<ZBRxResponse&> _onZBRxResponse;
	Callback<ZBExplicitRxResponse&> _onZBExplicitRxResponse;
	Callback<ZBRxIoSampleResponse&> _onZBRxIoSampleResponse;
	Callback<TxStatusResponse&> _onTxStatusResponse;
	Callback<Rx16Response&> _onRx16Response;
	Callback<Rx64Response&> _onRx64Response;
	Callback<Rx16IoSampleResponse&> _onRx16IoSampleResponse;
	Callback<Rx64IoSampleResponse&> _onRx64IoSampleResponse;
	Callback<ModemStatusResponse&> _onModemStatusResponse;
	Callback<AtCommandResponse&> _onAtCommandResponse;
	Callback<RemoteAtCommandResponse&> _onRemoteAtCommandResponse;
};

/**
 * All TX packets that support payloads extend this class
 */
class PayloadRequest : public XBeeRequest {
public:
	PayloadRequest(uint8_t apiId, uint8_t frameId, uint8_t *payload, uint8_t payloadLength);
	/**
	 * Returns the payload of the packet, if not null
	 */
	uint8_t* getPayload();
	/**
	 * Sets the payload array
	 */
	void setPayload(uint8_t* payloadPtr);

	/*
	 * Set the payload and its length in one call.
	 */
	void setPayload(uint8_t* payloadPtr, uint8_t payloadLength) {
		setPayload(payloadPtr);
		setPayloadLength(payloadLength);
	}

	/**
	 * Returns the length of the payload array, as specified by the user.
	 */
	uint8_t getPayloadLength();
	/**
	 * Sets the length of the payload to include in the request.  For example if the payload array
	 * is 50 bytes and you only want the first 10 to be included in the packet, set the length to 10.
	 * Length must be <= to the array length.
	 */
	void setPayloadLength(uint8_t payloadLength);
private:
	uint8_t* _payloadPtr;
	uint8_t _payloadLength;
};

#ifdef SERIES_1

/**
 * Represents a Series 1 TX packet that corresponds to Api Id: TX_16_REQUEST
 * <p/>
 * Be careful not to send a data array larger than the max packet size of your radio.
 * This class does not perform any validation of packet size and there will be no indication
 * if the packet is too large, other than you will not get a TX Status response.
 * The datasheet says 100 bytes is the maximum, although that could change in future firmware.
 */
class Tx16Request : public PayloadRequest {
public:
	Tx16Request(uint16_t addr16, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId);
	/**
	 * Creates a Unicast Tx16Request with the ACK option and DEFAULT_FRAME_ID
	 */
	Tx16Request(uint16_t addr16, uint8_t *payload, uint8_t payloadLength);
	/**
	 * Creates a default instance of this class.  At a minimum you must specify
	 * a payload, payload length and a destination address before sending this request.
	 */
	Tx16Request();
	uint16_t getAddress16();
	void setAddress16(uint16_t addr16);
	uint8_t getOption();
	void setOption(uint8_t option);
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();
protected:
private:
	uint16_t _addr16;
	uint8_t _option;
};

/**
 * Represents a Series 1 TX packet that corresponds to Api Id: TX_64_REQUEST
 *
 * Be careful not to send a data array larger than the max packet size of your radio.
 * This class does not perform any validation of packet size and there will be no indication
 * if the packet is too large, other than you will not get a TX Status response.
 * The datasheet says 100 bytes is the maximum, although that could change in future firmware.
 */
class Tx64Request : public PayloadRequest {
public:
	Tx64Request(XBeeAddress64 &addr64, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId);
	/**
	 * Creates a unicast Tx64Request with the ACK option and DEFAULT_FRAME_ID
	 */
	Tx64Request(XBeeAddress64 &addr64, uint8_t *payload, uint8_t payloadLength);
	/**
	 * Creates a default instance of this class.  At a minimum you must specify
	 * a payload, payload length and a destination address before sending this request.
	 */
	Tx64Request();
	XBeeAddress64& getAddress64();
	void setAddress64(XBeeAddress64& addr64);
	// TODO move option to superclass
	uint8_t getOption();
	void setOption(uint8_t option);
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();
private:
	XBeeAddress64 _addr64;
	uint8_t _option;
};

#endif


#ifdef SERIES_2

/**
 * Represents a Series 2 TX packet that corresponds to Api Id: ZB_TX_REQUEST
 *
 * Be careful not to send a data array larger than the max packet size of your radio.
 * This class does not perform any validation of packet size and there will be no indication
 * if the packet is too large, other than you will not get a TX Status response.
 * The datasheet says 72 bytes is the maximum for ZNet firmware and ZB Pro firmware provides
 * the ATNP command to get the max supported payload size.  This command is useful since the
 * maximum payload size varies according to certain settings, such as encryption.
 * ZB Pro firmware provides a PAYLOAD_TOO_LARGE that is returned if payload size
 * exceeds the maximum.
 */
class ZBTxRequest : public PayloadRequest {
public:
	/**
	 * Creates a unicast ZBTxRequest with the ACK option and DEFAULT_FRAME_ID
	 */
	ZBTxRequest(const XBeeAddress64 &addr64, uint8_t *payload, uint8_t payloadLength);
	ZBTxRequest(const XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId);
	/**
	 * Creates a default instance of this class.  At a minimum you must specify
	 * a payload, payload length and a 64-bit destination address before sending
	 * this request.
	 */
	ZBTxRequest();
	XBeeAddress64& getAddress64();
	uint16_t getAddress16();
	uint8_t getBroadcastRadius();
	uint8_t getOption();
	void setAddress64(const XBeeAddress64& addr64);
	void setAddress16(uint16_t addr16);
	void setBroadcastRadius(uint8_t broadcastRadius);
	void setOption(uint8_t option);
protected:
	// declare virtual functions
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();
	XBeeAddress64 _addr64;
	uint16_t _addr16;
	uint8_t _broadcastRadius;
	uint8_t _option;
};

/**
 * Represents a Series 2 TX packet that corresponds to Api Id: ZB_EXPLICIT_TX_REQUEST
 *
 * See the warning about maximum packet size for ZBTxRequest above,
 * which probably also applies here as well.
 *
 * Note that to distinguish reply packets from non-XBee devices, set
 * AO=1 to enable reception of ZBExplicitRxResponse packets.
 */
class ZBExplicitTxRequest : public ZBTxRequest {
public:
	/**
	 * Creates a unicast ZBExplicitTxRequest with the ACK option and
	 * DEFAULT_FRAME_ID.
	 *
	 * It uses the Maxstream profile (0xc105), both endpoints 232
	 * and cluster 0x0011, resulting in the same packet as sent by a
	 * normal ZBTxRequest.
	 */
	ZBExplicitTxRequest(XBeeAddress64 &addr64, uint8_t *payload, uint8_t payloadLength);
	/**
	 * Create a ZBExplicitTxRequest, specifying all fields.
	 */
	ZBExplicitTxRequest(XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId, uint8_t srcEndpoint, uint8_t dstEndpoint, uint16_t clusterId, uint16_t profileId);
	/**
	 * Creates a default instance of this class.  At a minimum you
	 * must specify a payload, payload length and a destination
	 * address before sending this request.
	 *
	 * Furthermore, it uses the Maxstream profile (0xc105), both
	 * endpoints 232 and cluster 0x0011, resulting in the same
	 * packet as sent by a normal ZBExplicitTxRequest.
	 */
	ZBExplicitTxRequest();
	uint8_t getSrcEndpoint();
	uint8_t getDstEndpoint();
	uint16_t getClusterId();
	uint16_t getProfileId();
	void setSrcEndpoint(uint8_t endpoint);
	void setDstEndpoint(uint8_t endpoint);
	void setClusterId(uint16_t clusterId);
	void setProfileId(uint16_t profileId);
protected:
	// declare virtual functions
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();
private:
	uint8_t _srcEndpoint;
	uint8_t _dstEndpoint;
	uint16_t _profileId;
	uint16_t _clusterId;
};

#endif

/**
 * Represents an AT Command TX packet
 * The command is used to configure the serially connected XBee radio
 */
class AtCommandRequest : public XBeeRequest {
public:
	AtCommandRequest();
	AtCommandRequest(uint8_t *command);
	AtCommandRequest(uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength);
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();
	uint8_t* getCommand();
	void setCommand(uint8_t* command);
	uint8_t* getCommandValue();
	void setCommandValue(uint8_t* command);
	uint8_t getCommandValueLength();
	void setCommandValueLength(uint8_t length);
	/**
	 * Clears the optional commandValue and commandValueLength so that a query may be sent
	 */
	void clearCommandValue();
	//void reset();
private:
	uint8_t *_command;
	uint8_t *_commandValue;
	uint8_t _commandValueLength;
};

/**
 * Represents an Remote AT Command TX packet
 * The command is used to configure a remote XBee radio
 */
class RemoteAtCommandRequest : public AtCommandRequest {
public:
	RemoteAtCommandRequest();
	/**
	 * Creates a RemoteAtCommandRequest with 16-bit address to set a command.
	 * 64-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength);
	/**
	 * Creates a RemoteAtCommandRequest with 16-bit address to query a command.
	 * 64-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command);
	/**
	 * Creates a RemoteAtCommandRequest with 64-bit address to set a command.
	 * 16-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength);
	/**
	 * Creates a RemoteAtCommandRequest with 16-bit address to query a command.
	 * 16-bit address defaults to broadcast and applyChanges is true.
	 */
	RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command);
	uint16_t getRemoteAddress16();
	void setRemoteAddress16(uint16_t remoteAddress16);
	XBeeAddress64& getRemoteAddress64();
	void setRemoteAddress64(XBeeAddress64 &remoteAddress64);
	bool getApplyChanges();
	void setApplyChanges(bool applyChanges);
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();
	static XBeeAddress64 broadcastAddress64;
//	static uint16_t broadcast16Address;
private:
	XBeeAddress64 _remoteAddress64;
	uint16_t _remoteAddress16;
	bool _applyChanges;
};



#endif //XBee_h

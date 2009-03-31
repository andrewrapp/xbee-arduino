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

#include <WProgram.h>
#include <inttypes.h>

#define SERIES_1
#define SERIES_2

// set to ATAP value of XBee. AP=2 is recommended
#define ATAP 2

#define START_BYTE 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13

#define MAX_PACKET_SIZE 100

#define BROADCAST_ADDRESS 0xffff
#define ZB_BROADCAST_ADDRESS 0xfffe

// the constant size of the api frame (not including frame id or api id or variable data size for packets that support payload)
#define ZB_TX_API_LENGTH 12
#define TX_16_API_LENGTH 3
#define TX_64_API_LENGTH 9
// start/length(2)/api/frameid/checksum bytes
#define PACKET_OVERHEAD_LENGTH 6
// api is always the third byte in packet
#define API_ID_INDEX 3

// frame position of rssi byte
#define RX_16_RSSI_OFFSET 2
#define RX_64_RSSI_OFFSET 8

#define DEFAULT_FRAME_ID 1
#define NO_RESPONSE_FRAME_ID 0

// TODO put in tx16 class
#define ACK_OPTION 0
#define DISABLE_ACK_OPTION 1
#define BROADCAST_OPTION 4

// RX options
#define ZB_PACKET_ACKNOWLEDGED 0x01
#define ZB_BROADCAST_PACKET 0x02

// not everything is implemented!
#define TX_64_REQUEST 0x0
#define TX_16_REQUEST 0x1
#define AT_COMMAND 0x08
#define AT_COMMAND_QUEUE 0x09
#define ZNET_REMOTE_AT_REQUEST 0x17
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
#define ZB_REMOTE_AT_RESPONSE 0x97
#define ZB_IO_SAMPLE_RESPONSE 0x92
#define ZB_IO_NODE_IDENTIFIER_RESPONSE 0x95

// TX STATUS
#define	SUCCESS 0x0
#define CCA_FAILURE 0x2
#define INVALID_DESTINATION_ENDPOINT_SUCCESS 0x15
#define	NETWORK_ACK_FAILURE 0x21
#define NOT_JOINED_TO_NETWORK 0x22
#define	SELF_ADDRESSED 0x23
#define ADDRESS_NOT_FOUND 0x24
#define ROUTE_NOT_FOUND 0x25
#define PAYLOAD_TOO_LARGE 0x74

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

class XBeeResponse {
public:
	//static const int MODEM_STATUS = 0x8a;
	XBeeResponse();
	uint8_t getApiId();
	void setApiId(uint8_t apiId);
	uint8_t& getMsbLength();
	void setMsbLength(uint8_t msbLength);
	uint8_t& getLsbLength();
	void setLsbLength(uint8_t lsbLength);
	uint8_t& getChecksum();
	void setChecksum(uint8_t checksum);
	uint8_t getFrameDataLength();
	void setFrameLength(uint8_t frameLength);
	// to support future 65535 byte packets I guess
	uint16_t getPacketLength();
	void reset();
	void init();
#ifdef SERIES_2
	void getZBTxStatusResponse(XBeeResponse &response);
	void getZBRxResponse(XBeeResponse &response);
#endif
#ifdef SERIES_1
	void getTxStatusResponse(XBeeResponse &response);
	void getRx16Response(XBeeResponse &response);
	void getRx64Response(XBeeResponse &response);
#endif
	void getModemStatusResponse(XBeeResponse &response);
	bool isAvailable();
	void setAvailable(bool complete);
	bool isError();
	void setError(bool error);
	void setFrameData(uint8_t* frameDataPtr);
	// returns the buffer that contains the response.  starts with byte that follows API ID and includes all bytes prior to the checksum
	uint8_t* getFrameData();
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
	bool _error;
};

class XBeeAddress {
public:
	XBeeAddress();
};

class XBeeAddress64 : public XBeeAddress {
public:
	XBeeAddress64(uint32_t msb, uint32_t lsb);
	XBeeAddress64();
	uint32_t getMsb();
	uint32_t getLsb();
	void setMsb(uint32_t msb);
	void setLsb(uint32_t lsb);
private:
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

class RxDataResponse : public XBeeResponse {
public:
	RxDataResponse();
	// returns the data value at given index (0 : getDataLength() - 1)
	uint8_t& getData(int index);
	virtual uint8_t getDataOffset() = 0;
};

// getResponse to return the proper subclass:
// we maintain a pointer to each type of response, when a response is parsed, it is allocated only if NULL
// can we allocate an object in a function?

#ifdef SERIES_2
// WTF: this can't be defined above XBeeResponse so it can't be referenced in the XBeeResponse class???
class ZBTxStatusResponse : public XBeeResponse {
	public:
		ZBTxStatusResponse();
		uint8_t getFrameId();
		uint16_t getRemoteAddress();
		uint8_t getTxRetryCount();
		uint8_t getDeliveryStatus();
		uint8_t getDiscoveryStatus();
};

class ZBRxResponse : public RxDataResponse {
public:
	ZBRxResponse();
	XBeeAddress64& getRemoteAddress64();
	uint16_t getRemoteAddress16();
	uint8_t getOption();
	uint8_t getDataLength();
	// frame position where data starts
	uint8_t getDataOffset();
private:
	XBeeAddress64 _remoteAddress64;
};

#endif

#ifdef SERIES_1

class TxStatusResponse : public XBeeResponse {
	public:
		TxStatusResponse();
		uint8_t getFrameId();
		uint8_t getStatus();
};

class RxResponse : public RxDataResponse {
public:
	RxResponse();
	// remember rssi is negative but this is unsigned byte so it's up to you to convert
	uint8_t getRssi();
	uint8_t getOption();
	bool isAddressBroadcast();
	bool isPanBroadcast();
	virtual uint8_t getRssiOffset() = 0;
	uint8_t getDataLength();
	uint8_t getDataOffset();

//	XBeeAddress& getRemoteAddress();
//protected:
//	XBeeAddress _remoteAddress;
};

class Rx16Response : public RxResponse {
public:
	Rx16Response();
	uint8_t getRssiOffset();
	uint16_t getRemoteAddress16();
protected:
	uint16_t _remoteAddress;
};

class Rx64Response : public RxResponse {
public:
	Rx64Response();
	uint8_t getRssiOffset();
	XBeeAddress64& getRemoteAddress64();
private:
	XBeeAddress64 _remoteAddress;
};
#endif

class ModemStatusResponse : public XBeeResponse {
public:
	ModemStatusResponse();
	uint8_t& getStatus();
};

class XBeeRequest {
public:
	XBeeRequest(uint8_t apiId, uint8_t frameId, uint16_t frameDataLength, uint8_t* _payloadPtr, uint8_t _payloadLength);
	// called in XBee.send()
	void assemblePacket();
	// size of the api frame (not including frame id or api id or variable data size for packets that support payload)
	uint8_t getFrameDataLength();
	uint8_t* getPacket();
	void setPacket(uint8_t* frameDataPtr);
	// returns the payload array, if not null
	uint8_t* getPayload();
	uint8_t getPayloadLength();
	void setFrameId(uint8_t frameId);
	uint8_t getFrameId();
protected:
	uint8_t computeChecksum(uint8_t* arr, int len, int offset);
	// setting = 0 makes this a pure virtual function, meaning the subclass must implement, like abstract in java
	virtual void assembleFrame() = 0;
private:
	// NOTE: length of data stored in the array; does not include apiId and frameId, so add two for true frame data length
	uint8_t _apiId;
	uint8_t _frameId;
	uint8_t _packetLength;
	uint8_t* _packetPtr;
	uint16_t _frameDataLength;
	uint8_t* _payloadPtr;
	uint8_t _payloadLength;
};

// TODO add reset/clear method since responses are often reused
/**
 * Concept of operation:
 *
 * Since this code runs on a microcontroller, with only one thread, you are responsible for reading the
 * data off the serial buffer in a timely manner.  This involves a call to a variant of readPacket(...).
 * If your serial port is receiving data faster than you are reading, you can expect to lose packets
 */
class XBee {
public:
	XBee();
	// reads all available serial bytes until a packet has been parsed or it runs out of serial data
	// this method most always returns quickly since it does not wait for serial data to arrive
	// you will want to use this method if you are doing other timely stuff in your loop
	// NOTE: calling this method resets the current response, so make sure you consume the response before calling this
	void readPacket();
	// waits a maximum of timeout milliseconds for a response packet before timing out; returns true if packet is read before timeout
	bool readPacket(int timeout);
	// reads until a packet is received
	// Caution: use this carefully since if you don't get a response, your Arduino code will hang on this call forever!! often it's better to use a timeout: readPacket(int)
	void readPacketUntilAvailable();
	// start your serial connection at the supplied baud rate
	void begin(int baud);
	// call this method only after response.isAvailable() returns true
	// supply a subclass of XBeeResponse that is appropriate for response.getApiId()
	// after calling this method, your subclass will be populated
	void getResponse(XBeeResponse &response);
	// returns a reference to the current response
	XBeeResponse& getResponse();
	// sends a TX request out the serial port
	void send(XBeeRequest &request);
	//uint8_t sendAndWaitForResponse(XBeeRequest &request, int timeout);
	// returns frame id between 1 and 255
	uint8_t getNextFrameId();
private:
	void sendByte(uint8_t &b, bool escape);
	void resetResponse();
	XBeeResponse _response;
	bool _escape;
	// current packet position for response.  just a state variable for packet parsing and has no relevance for the response otherwise
	uint8_t _pos;
	// last byte read
	uint8_t b;
	uint8_t _checksumTotal;
	uint8_t _nextFrameId;
	// TODO considering creating response subclass pointers
	//ZBTxStatusResponse* txStatusResponse;
	//ZBRxResponse* rxResponse;
	// this is just the api portion of the frame, starting after the api id byte
	// TODO dynamic memory?
	uint8_t _responseFrameData[MAX_PACKET_SIZE];
	// TODO use macro to compute max length of all APIs for array size
	uint8_t _requestPacket[PACKET_OVERHEAD_LENGTH + ZB_TX_API_LENGTH];
};

#ifdef SERIES_1

// TODO super class to share common variables

class Tx16Request : public XBeeRequest {
public:
	Tx16Request(uint16_t addr16, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId);
	void assembleFrame();
private:
	uint16_t _addr16;
	uint8_t _option;
};

class Tx64Request : public XBeeRequest {
public:
	Tx64Request(XBeeAddress64 &addr64, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId);
	void assembleFrame();
private:
	XBeeAddress64 _addr64;
	uint8_t _option;
};

#endif


#ifdef SERIES_2

class ZBTxRequest : public XBeeRequest {
public:
	// TODO abbreviated constructors and set/get methods
	ZBTxRequest(XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId);
protected:
	// why do I have to declare this when it's defined in the superclass?
	void assembleFrame();
private:
	XBeeAddress64 _addr64;
	uint16_t _addr16;
	uint8_t _broadcastRadius;
	uint8_t _option;
};

#endif


#endif //XBee_h

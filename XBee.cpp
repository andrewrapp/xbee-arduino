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

#include "XBee.h"
#include "WConstants.h"
#include "HardwareSerial.h"


XBeeResponse::XBeeResponse() {

}

uint8_t XBeeResponse::getApiId() {
	return _apiId;
}

void XBeeResponse::setApiId(uint8_t apiId) {
	_apiId = apiId;
}

uint8_t& XBeeResponse::getMsbLength() {
	return _msbLength;
}

void XBeeResponse::setMsbLength(uint8_t msbLength) {
	_msbLength = msbLength;
}

uint8_t& XBeeResponse::getLsbLength() {
	return _lsbLength;
}

void XBeeResponse::setLsbLength(uint8_t lsbLength) {
	_lsbLength = lsbLength;
}

uint8_t& XBeeResponse::getChecksum() {
	return _checksum;
}

void XBeeResponse::setChecksum(uint8_t checksum) {
	_checksum = checksum;
}

uint8_t XBeeResponse::getFrameDataLength() {
	return _frameLength;
}

void XBeeResponse::setFrameLength(uint8_t frameLength) {
	_frameLength = frameLength;
}

bool XBeeResponse::isAvailable() {
	return _complete;
}

void XBeeResponse::setAvailable(bool complete) {
	_complete = complete;
}

bool XBeeResponse::isError() {
	return _error;
}

void XBeeResponse::setError(bool error) {
	_error = error;
}

// copy common fields from xbee response to target response
void XBeeResponse::setCommon(XBeeResponse &target) {
	target.setApiId(getApiId());
	target.setAvailable(isAvailable());
	target.setChecksum(getChecksum());
	target.setError(isError());
	target.setFrameLength(getFrameDataLength());
	target.setMsbLength(getMsbLength());
	target.setLsbLength(getLsbLength());
}

#ifdef SERIES_2

ZBRxResponse::ZBRxResponse(): RxDataResponse() {
	_remoteAddress64 = XBeeAddress64();
}

ZBTxStatusResponse::ZBTxStatusResponse() : XBeeResponse() {

}

uint8_t ZBTxStatusResponse::getFrameId() {
	return getFrameData()[0];
}

uint16_t ZBTxStatusResponse::getRemoteAddress() {
	return  (getFrameData()[1] << 8) + getFrameData()[2];
}

uint8_t ZBTxStatusResponse::getTxRetryCount() {
	return getFrameData()[3];
}

uint8_t ZBTxStatusResponse::getDeliveryStatus() {
	return getFrameData()[4];
}
uint8_t ZBTxStatusResponse::getDiscoveryStatus() {
	return getFrameData()[5];
}

void XBeeResponse::getZBTxStatusResponse(XBeeResponse &zbXBeeResponse) {

	// way off?
	ZBTxStatusResponse* zb = static_cast<ZBTxStatusResponse*>(&zbXBeeResponse);
	// pass pointer array to subclass
	zb->setFrameData(getFrameData());
	setCommon(zbXBeeResponse);
}

XBeeAddress64& ZBRxResponse::getRemoteAddress64() {
	return _remoteAddress64;
}

uint16_t ZBRxResponse::getRemoteAddress16() {
	return 	(getFrameData()[8] << 8) + getFrameData()[9];
}

uint8_t ZBRxResponse::getOption() {
	return getFrameData()[10];
}

// markers to read data from packet array.  this is the index, so the 12th item in the array
uint8_t ZBRxResponse::getDataOffset() {
	return 11;
}

uint8_t ZBRxResponse::getDataLength() {
	return getPacketLength() - getDataOffset() - 1;
}

void XBeeResponse::getZBRxResponse(XBeeResponse &rxResponse) {

	// way off?
	ZBRxResponse* zb = static_cast<ZBRxResponse*>(&rxResponse);

	//TODO verify response api id matches this api for this response

	// pass pointer array to subclass
	zb->setFrameData(getFrameData());
	setCommon(rxResponse);

	zb->getRemoteAddress64().setMsb((uint32_t(getFrameData()[0]) << 24) + (uint32_t(getFrameData()[1]) << 16) + (uint16_t(getFrameData()[2]) << 8) + getFrameData()[3]);
	zb->getRemoteAddress64().setLsb((uint32_t(getFrameData()[4]) << 24) + (uint32_t(getFrameData()[5]) << 16) + (uint16_t(getFrameData()[6]) << 8) + (getFrameData()[7]));
}

#endif

RxDataResponse::RxDataResponse() : XBeeResponse() {

}

uint8_t& RxDataResponse::getData(int index) {
	return getFrameData()[getDataOffset() + index];
}

#ifdef SERIES_1

RxResponse::RxResponse() : RxDataResponse() {

}

uint16_t Rx16Response::getRemoteAddress16() {
	return (getFrameData()[0] << 8) + getFrameData()[1];
}

XBeeAddress64& Rx64Response::getRemoteAddress64() {
	return _remoteAddress;
}

Rx64Response::Rx64Response() : RxResponse() {
	_remoteAddress = XBeeAddress64();
}

Rx16Response::Rx16Response() : RxResponse() {

}

TxStatusResponse::TxStatusResponse() : XBeeResponse() {

}

uint8_t TxStatusResponse::getFrameId() {
	return getFrameData()[0];
}

uint8_t TxStatusResponse::getStatus() {
	return getFrameData()[1];
}

void XBeeResponse::getTxStatusResponse(XBeeResponse &txResponse) {

	TxStatusResponse* txStatus = static_cast<TxStatusResponse*>(&txResponse);

	// pass pointer array to subclass
	txStatus->setFrameData(getFrameData());
	setCommon(txResponse);
}

uint8_t RxResponse::getRssi() {
	return getFrameData()[getRssiOffset()];
}

uint8_t RxResponse::getOption() {
	return getFrameData()[getRssiOffset() + 1];
}

bool RxResponse::isAddressBroadcast() {
	return (getOption() & 2) == 2;
}

bool RxResponse::isPanBroadcast() {
	return (getOption() & 4) == 4;
}

uint8_t RxResponse::getDataLength() {
	return getPacketLength() - getDataOffset() - 1;
}

uint8_t RxResponse::getDataOffset() {
	//TODO constant
	// data starts at byte 9 minus 4 (start/length(2)/apiId) - 1 (index starts at zero)
	return getRssiOffset() + 2;
}

uint8_t Rx16Response::getRssiOffset() {
	return RX_16_RSSI_OFFSET;
}

void XBeeResponse::getRx16Response(XBeeResponse &rx16Response) {

	Rx16Response* rx16 = static_cast<Rx16Response*>(&rx16Response);

	// pass pointer array to subclass
	rx16->setFrameData(getFrameData());
	setCommon(rx16Response);

//	rx16->getRemoteAddress16().setAddress((getFrameData()[0] << 8) + getFrameData()[1]);
}

uint8_t Rx64Response::getRssiOffset() {
	return RX_64_RSSI_OFFSET;
}

void XBeeResponse::getRx64Response(XBeeResponse &rx64Response) {

	Rx64Response* rx64 = static_cast<Rx64Response*>(&rx64Response);

	// pass pointer array to subclass
	rx64->setFrameData(getFrameData());
	setCommon(rx64Response);

	rx64->getRemoteAddress64().setMsb((uint32_t(getFrameData()[0]) << 24) + (uint32_t(getFrameData()[1]) << 16) + (uint16_t(getFrameData()[2]) << 8) + getFrameData()[3]);
	rx64->getRemoteAddress64().setLsb((uint32_t(getFrameData()[4]) << 24) + (uint32_t(getFrameData()[5]) << 16) + (uint16_t(getFrameData()[6]) << 8) + getFrameData()[7]);
}

#endif

ModemStatusResponse::ModemStatusResponse() {

}

uint8_t& ModemStatusResponse::getStatus() {
	return getFrameData()[0];
}

void XBeeResponse::getModemStatusResponse(XBeeResponse &modemStatusResponse) {

	ModemStatusResponse* modem = static_cast<ModemStatusResponse*>(&modemStatusResponse);

	// pass pointer array to subclass
	modem->setFrameData(getFrameData());
	setCommon(modemStatusResponse);

}

uint16_t XBeeResponse::getPacketLength() {
	return ((_msbLength << 8) & 0xff) + (_lsbLength & 0xff);
}

uint8_t* XBeeResponse::getFrameData() {
	return _frameDataPtr;
}

void XBeeResponse::setFrameData(uint8_t* frameDataPtr) {
	_frameDataPtr = frameDataPtr;
}

void XBeeResponse::init() {
	_complete = false;
	_error = false;
	_checksum = 0;
}

void XBeeResponse::reset() {
	init();
	_apiId = 0;
	_msbLength = 0;
	_lsbLength = 0;
	_checksum = 0;
	_frameLength = 0;

	for (int i = 0; i < MAX_PACKET_SIZE; i++) {
		getFrameData()[i] = 0;
	}
}

void XBee::resetResponse() {
	_pos = 0;
	_escape = false;
	_response.reset();
}

XBee::XBee(HardwareSerial &serial): _response(XBeeResponse()) {
	_serial = serial;
	_pos = 0;
	_escape = false;
	_checksumTotal = 0;
	_nextFrameId = 0;

	_response.init();
	_response.setFrameData(_responseFrameData);
}

uint8_t XBee::getNextFrameId() {

	_nextFrameId++;

	if (_nextFrameId == 0) {
		// can't send 0 because that disables status response
		_nextFrameId = 1;
	}

	return _nextFrameId;
}

void XBee::begin(int baud) {
	_serial.begin(baud);
}

XBeeResponse& XBee::getResponse() {
	return _response;
}

// TODO how to convert response to proper subclass?
// TODO clone response?
void XBee::getResponse(XBeeResponse &response) {

	response.setMsbLength(_response.getMsbLength());
	response.setLsbLength(_response.getLsbLength());
	response.setApiId(_response.getApiId());
	response.setFrameLength(_response.getFrameDataLength());

	response.setFrameData(_response.getFrameData());
}

void XBee::readPacketUntilAvailable() {
	while (!getResponse().isAvailable()) {
		// read some more
		readPacket();
	}
}

bool XBee::readPacket(int timeout) {

	if (timeout < 0) {
		return false;
	}

	unsigned long start = millis();

    while (int((millis() - start)) < timeout) {

     	readPacket();

     	if (getResponse().isAvailable()) {
     		return true;
     	}
    }

    // timed out
    return false;
}
void XBee::readPacket() {
	// reset previous response
	if (_response.isAvailable() || _response.isError()) {
		resetResponse();
	}

    while (_serial.available()) {

#ifdef DEBUG
    	std::cout << "pos is " << static_cast<unsigned int>(_pos) << std::endl;
#endif
        b = _serial.read();

        if (_pos > 0 && b == START_BYTE) {
#ifdef WARN
        	std::cout << "found start byte at pos " << static_cast<unsigned int>(_pos) << " resetting" << std::endl;
#endif
        	// new packet start before previous packeted completed -- discard previous packet and start over
        	// TODO set warn byte
        	// TODO ATAP must be set to 2
        	resetResponse();
        }

#ifdef DEBUG
        std::cout << "read " << static_cast<unsigned int>(b) << " from serial\n";
#endif

		if (_pos > 0 && b == ESCAPE) {
#ifdef DEBUG
			std::cout << "escape byte\n";
#endif
			if (_serial.available()) {
				b = _serial.read();
				b = 0x20 ^ b;
#ifdef DEBUG
				std::cout << "un-escaped byte is" << b << "\n";
#endif
			} else {
				// escape byte.  next byte will be
#ifdef DEBUG
				std::cout << "next byte will be un-escaped\n";
#endif
				_escape = true;
				continue;
			}
		}

		if (_escape == true) {
			b = 0x20 ^ b;
			_escape = false;
#ifdef DEBUG
			std::cout << "un-escaped next byte is " << static_cast<unsigned int>(b) << "\n";
#endif
		}

		// checksum includes all bytes starting with api id
		if (_pos >= API_ID_INDEX) {
			_checksumTotal+= b;
#ifdef DEBUG
			std::cout << "adding to checksum " << static_cast<unsigned int>(b) << ", total is " << static_cast<unsigned int>(_checksumTotal) << std::endl;
#endif
		}

        switch(_pos) {
			case 0:
		        if (b == START_BYTE) {
		        	_pos++;
#ifdef DEBUG
		        	std::cout << "found start byte\n";
#endif
		        } else {
#ifdef WARN
		        	std::cout << "ignoring non-start byte\n";
#endif
		        }

		        break;
			case 1:
				// length msb
				_response.setMsbLength(b);
				_pos++;
#ifdef DEBUG
				std::cout << "read msb length\n";
#endif
				break;
			case 2:
				// length lsb
				_response.setLsbLength(b);
				_pos++;
#ifdef DEBUG
				std::cout << "read lsb length\n";
#endif
				break;
			case 3:
				_response.setApiId(b);
				_pos++;
#ifdef DEBUG
				std::cout << "read apiId\n";
#endif
				break;
			default:

				if (_pos > MAX_PACKET_SIZE) {
					// exceed max size.  should never occur.  reset
#ifdef ERROR
					std::cout << "exceeded max packet size.  resetting" << std::endl;
#endif
					_response.setError(true);
					return;
				}

				// check if we're at the end of the packet
				// packet length does not include start, length, or checksum bytes, so add 3
				if (_pos == (_response.getPacketLength() + 3)) {
#ifdef DEBUG
					std::cout << "reached end of packet\n";
#endif
					// verify checksum
					if ((_checksumTotal & 0xff) == 0xff) {
#ifdef DEBUG
						std::cout << "verified checksum at pos " << static_cast<unsigned int>(_pos) << "\n";
#endif
						// perhaps not very useful but set the checksum anyhow
#ifdef DEBUG
						std::cout << "readPacket: checksum is " << static_cast<unsigned int>(b) << "\n";
#endif
						_response.setChecksum(b);
						_response.setAvailable(true);
					} else {
						// checksum failed
						_response.setError(true);
#ifdef ERROR
						std::cout << "checksum failed\n";
#endif
					}

					// minus 4 because we start after start,msb,lsb,api
					_response.setFrameLength(_pos - 4);

					// reset state vars
					_pos = 0;

					_checksumTotal = 0;

					return;
				} else {
					// add to packet array, starting with the fourth byte of the apiFrame
#ifdef DEBUG
					std::cout << "applying frame data to pos: " << static_cast<unsigned int>(_pos - 4) << " to " << static_cast<unsigned int>(b) << "\n";
#endif
					_response.getFrameData()[_pos - 4] = b;
					_pos++;
				}
        }
    }

#ifdef DEBUG
    std::cout << "exiting readPacket" << std::endl;
#endif
}

//// make static function?
//// compute checksum on pre-escaped frame data only
//uint8_t XBeeRequest::computeChecksum(uint8_t* arr, int len, int offset) {
//		int checksum = 0;
//
//		for (int i = 0; i < len; i++) {
//			checksum+= *(arr + i + offset);
//		}
//
//#ifdef ECLIPSE
//		std::cout << "checksum total is " << checksum << "\n";
//#endif
//
//		checksum = 0xff & checksum;
//
//		// perform 2s complement
//		checksum = 0xff - checksum;
//
//#ifdef DEBUG
//		std::cout << "checksum is " << checksum << "\n";
//#endif
//
//		return checksum;
//}

XBeeRequest::XBeeRequest(uint8_t apiId, uint8_t frameId, uint16_t frameDataLength, uint8_t* payloadPtr, uint8_t payloadLength) {
	_apiId = apiId;
	_frameId = frameId;
	_frameDataLength = frameDataLength;
	_payloadPtr = payloadPtr;
	_payloadLength = payloadLength;
}

void XBeeRequest::setFrameId(uint8_t frameId) {
	_frameId = frameId;
}

uint8_t XBeeRequest::getFrameId() {
	return _frameId;
}

uint8_t* XBeeRequest::getPayload() {
	return _payloadPtr;
}

uint8_t XBeeRequest::getPayloadLength() {
	return _payloadLength;
}

void XBeeRequest::setPacket(uint8_t* packetPtr) {
	_packetPtr = packetPtr;
}

uint8_t* XBeeRequest::getPacket() {
	return _packetPtr;
}

// TODO make pointer?
uint8_t XBeeRequest::getFrameDataLength() {
	return _frameDataLength;
}

XBeeAddress::XBeeAddress() {

}

//XBeeAddress16::XBeeAddress16() : XBeeAddress() {
//
//}

//XBeeAddress16::XBeeAddress16(uint16_t addr) : XBeeAddress() {
//	_addr = addr;
//}
//
//uint16_t XBeeAddress16::getAddress() {
//	return _addr;
//}
//
//void XBeeAddress16::setAddress(uint16_t addr) {
//	_addr = addr;
//}


XBeeAddress64::XBeeAddress64() : XBeeAddress() {

}

XBeeAddress64::XBeeAddress64(uint32_t msb, uint32_t lsb) : XBeeAddress() {
	_msb = msb;
	_lsb = lsb;
}

uint32_t XBeeAddress64::getMsb() {
	return _msb;
}

void XBeeAddress64::setMsb(uint32_t msb) {
	_msb = msb;
}

uint32_t XBeeAddress64::getLsb() {
	return _lsb;
}

void XBeeAddress64::setLsb(uint32_t lsb) {
	_lsb = lsb;
}


#ifdef SERIES_2
// calls superclass constructor
ZBTxRequest::ZBTxRequest(XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId): XBeeRequest(ZB_TX_REQUEST, frameId, ZB_TX_API_LENGTH, data, dataLength) {
	_addr64 = addr64;
	_addr16 = addr16;
	_broadcastRadius = broadcastRadius;
	_option = option;
}

void ZBTxRequest::assembleFrame() {
		// build frame data from components and returns
		XBeeRequest::getPacket()[5] = (_addr64.getMsb() >> 24) & 0xff;
		XBeeRequest::getPacket()[6] = (_addr64.getMsb() >> 16) & 0xff;
		XBeeRequest::getPacket()[7] = (_addr64.getMsb() >> 8) & 0xff;
		XBeeRequest::getPacket()[8] = _addr64.getMsb() & 0xff;
		XBeeRequest::getPacket()[9] = (_addr64.getLsb() >> 24) & 0xff;
		XBeeRequest::getPacket()[10] = (_addr64.getLsb() >> 16) & 0xff;
		XBeeRequest::getPacket()[11] = (_addr64.getLsb() >> 8) & 0xff;
		XBeeRequest::getPacket()[12] = _addr64.getLsb() & 0xff;
		XBeeRequest::getPacket()[13] = (_addr16 >> 8) & 0xff;
		XBeeRequest::getPacket()[14] = _addr16 & 0xff;
		XBeeRequest::getPacket()[15] = _broadcastRadius;
		XBeeRequest::getPacket()[16] = _option;
}

#endif

#ifdef SERIES_1
Tx16Request::Tx16Request(uint16_t addr16, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId) : XBeeRequest(TX_16_REQUEST, frameId, TX_16_API_LENGTH, data, dataLength) {
	_addr16 = addr16;
	_option = option;
}

void Tx16Request::assembleFrame() {
	// build frame data
	XBeeRequest::getPacket()[5] = (_addr16 >> 8) & 0xff;
	XBeeRequest::getPacket()[6] = _addr16 & 0xff;
	XBeeRequest::getPacket()[7] = _option;
}

Tx64Request::Tx64Request(XBeeAddress64 &addr64, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId) : XBeeRequest(TX_64_REQUEST, frameId, TX_64_API_LENGTH, data, dataLength) {
	_addr64 = addr64;
	_option = option;
}

void Tx64Request::assembleFrame() {
	// build frame data
	XBeeRequest::getPacket()[5] = (_addr64.getMsb() >> 24) & 0xff;
	XBeeRequest::getPacket()[6] = (_addr64.getMsb() >> 16) & 0xff;
	XBeeRequest::getPacket()[7] = (_addr64.getMsb() >> 8) & 0xff;
	XBeeRequest::getPacket()[8] = _addr64.getMsb() & 0xff;
	XBeeRequest::getPacket()[9] = (_addr64.getLsb() >> 24) & 0xff;
	XBeeRequest::getPacket()[10] = (_addr64.getLsb() >> 16) & 0xff;
	XBeeRequest::getPacket()[11] = (_addr64.getLsb() >> 8) & 0xff;
	XBeeRequest::getPacket()[12] = _addr64.getLsb() & 0xff;

	XBeeRequest::getPacket()[13] = _option;
}


#endif

//GenericRequest::GenericRequest(uint8_t* frame, uint8_t len, uint8_t apiId): XBeeRequest(apiId, *(frame), len) {
//	_frame = frame;
//}

void XBeeRequest::assemblePacket() {

#ifdef DEBUG
		std::cout << "assemblePacket(): frameDataLength is " <<  static_cast<unsigned int>(getFrameDataLength()) << ", payloadLen is " <<  static_cast<unsigned int>(getPayloadLength()) << "\n";
#endif
		getPacket()[0] = START_BYTE;
		// compute packet length
		// frameData length does not include apiId and frameId, so add 2
		getPacket()[1] = ((getPayloadLength() + getFrameDataLength() + 2) >> 8) & 0xff;
		getPacket()[2] = (getPayloadLength() + getFrameDataLength() + 2) & 0xff;

		// calls subclass
		assembleFrame();

		getPacket()[3] = _apiId;
		getPacket()[4] = _frameId;

		// checksum includes all bytes except start/length and of course checksum
		//uint8_t checksum = computeChecksum(getPacket(), _frameDataLength + 2, 3);

		uint8_t checksum = 0;

		// compute checksum, start at api id
		for (int i = 0; i < getFrameDataLength() + 2; i++) {
			checksum+= getPacket()[i + API_ID_INDEX];
		}

		// add payload to checksum
		if (getPayloadLength() > 0) {
			for (int i = 0; i < getPayloadLength(); i++) {
#ifdef DEBUG
//		std::cout << "adding payload to checksum\n";
#endif
				checksum+= getPayload()[i];
			}
		}

#ifdef DEBUG
		std::cout << "checksum total is " << static_cast<unsigned int>(checksum) << "\n";
#endif

		//checksum = 0xff & checksum;

		// perform 2s complement
		checksum = 0xff - checksum;

#ifdef DEBUG
		std::cout << "checksum is " << static_cast<unsigned int>(checksum) << "\n";
#endif

		// set checksum as last byte in packet
		getPacket()[getFrameDataLength() + PACKET_OVERHEAD_LENGTH - 1] = checksum;
}

// TODO need subclass of XBeeRequest that supports frame id and
// to get frame id from response subclass, if exists;
//int XBee::sendAndWaitForResponse(XBeeRequest &request, int timeout) {
//	// TODO first check if this request api type results in a response w/ frame id
//	send(request);
//
//	unsigned long start = millis();
//
//	// read packet until response matches frame id or timeout occurs
//	do {
//		if (readPacket(timeout) && (_response.getFrameId() == request.getFrameId())) {
//			return 0;
//		} else if (_response.isAvailable()) {
//			// got a response
//			return 1;
//		}
//	} while ((millis() - start) > timeout);
//
//	// timeout
//	return 2;
//}

void XBee::send(XBeeRequest &request) {

	// provide request with buffer
	request.setPacket(_requestPacket);

	request.assemblePacket();

	// first send everything but the checksum
	for (int i = 0; i < request.getFrameDataLength() + PACKET_OVERHEAD_LENGTH - 1; i++) {

#ifdef DEBUG
		std::cout << "XBee send: packet["<< i << "] is " << static_cast<unsigned int>(request.getPacket()[i]) << "\n";
#endif
		if (i == 0) {
			// escape special bytes, but not the start byte!
			sendByte(request.getPacket()[i], false);
		} else {
			sendByte(request.getPacket()[i], true);
		}
	}

	// if payload exists, send it now
	if (request.getPayloadLength() > 0) {

#ifdef DEBUG
//		std::cout << "XBee send: payload exists and is size " << static_cast<unsigned int>(request.getPayloadLength()) << std::endl;
#endif
		for (int i = 0; i < request.getPayloadLength(); i++) {
			sendByte(request.getPayload()[i], true);
		}
	}

	// send checksum
	sendByte(request.getPacket()[request.getFrameDataLength() + PACKET_OVERHEAD_LENGTH - 1], true);

	// send packet
	_serial.flush();
}

void XBee::sendByte(uint8_t &b, bool escape) {

#ifdef DEBUG
		std::cout << "XBee sendByte, byte is " << static_cast<unsigned int>(b) << std::endl;
#endif

	if (escape && (b == START_BYTE || b == ESCAPE || b == XON || b == XOFF)) {
		// output ESCAPE
		// output 0x20 ^ packet[i];
		_serial.print(ESCAPE, BYTE);
		_serial.print(b ^ 0x20, BYTE);
	} else {
		_serial.print(b, BYTE);
	}
}

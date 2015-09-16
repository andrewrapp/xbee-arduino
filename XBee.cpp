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

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "HardwareSerial.h"

XBeeResponse::XBeeResponse() {

}

uint8_t XBeeResponse::getApiId() {
	return _apiId;
}

void XBeeResponse::setApiId(uint8_t apiId) {
	_apiId = apiId;
}

uint8_t XBeeResponse::getMsbLength() {
	return _msbLength;
}

void XBeeResponse::setMsbLength(uint8_t msbLength) {
	_msbLength = msbLength;
}

uint8_t XBeeResponse::getLsbLength() {
	return _lsbLength;
}

void XBeeResponse::setLsbLength(uint8_t lsbLength) {
	_lsbLength = lsbLength;
}

uint8_t XBeeResponse::getChecksum() {
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
	return _errorCode > 0;
}

uint8_t XBeeResponse::getErrorCode() {
	return _errorCode;
}

void XBeeResponse::setErrorCode(uint8_t errorCode) {
	_errorCode = errorCode;
}

// copy common fields from xbee response to target response
void XBeeResponse::setCommon(XBeeResponse &target) {
	target.setApiId(getApiId());
	target.setAvailable(isAvailable());
	target.setChecksum(getChecksum());
	target.setErrorCode(getErrorCode());
	target.setFrameLength(getFrameDataLength());
	target.setMsbLength(getMsbLength());
	target.setLsbLength(getLsbLength());
}

#ifdef SERIES_2

ZBTxStatusResponse::ZBTxStatusResponse() : FrameIdResponse() {

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

bool ZBTxStatusResponse::isSuccess() {
	return getDeliveryStatus() == SUCCESS;
}

void XBeeResponse::getZBTxStatusResponse(XBeeResponse &zbXBeeResponse) {

	// way off?
	ZBTxStatusResponse* zb = static_cast<ZBTxStatusResponse*>(&zbXBeeResponse);
	// pass pointer array to subclass
	zb->setFrameData(getFrameData());
	setCommon(zbXBeeResponse);
}

ZBRxResponse::ZBRxResponse(): RxDataResponse() {
	_remoteAddress64 = XBeeAddress64();
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

XBeeAddress64& ZBRxResponse::getRemoteAddress64() {
	return _remoteAddress64;
}

void XBeeResponse::getZBRxResponse(XBeeResponse &rxResponse) {

	ZBRxResponse* zb = static_cast<ZBRxResponse*>(&rxResponse);

	//TODO verify response api id matches this api for this response

	// pass pointer array to subclass
	zb->setFrameData(getFrameData());
	setCommon(rxResponse);

	zb->getRemoteAddress64().setMsb((uint32_t(getFrameData()[0]) << 24) + (uint32_t(getFrameData()[1]) << 16) + (uint16_t(getFrameData()[2]) << 8) + getFrameData()[3]);
	zb->getRemoteAddress64().setLsb((uint32_t(getFrameData()[4]) << 24) + (uint32_t(getFrameData()[5]) << 16) + (uint16_t(getFrameData()[6]) << 8) + (getFrameData()[7]));
}

ZBExplicitRxResponse::ZBExplicitRxResponse(): ZBRxResponse() {
}

uint8_t ZBExplicitRxResponse::getSrcEndpoint() {
	return getFrameData()[10];
}

uint8_t ZBExplicitRxResponse::getDstEndpoint() {
	return getFrameData()[11];
}

uint16_t ZBExplicitRxResponse::getClusterId() {
	return (uint16_t)(getFrameData()[12]) << 8 | getFrameData()[13];
}

uint16_t ZBExplicitRxResponse::getProfileId() {
	return (uint16_t)(getFrameData()[14]) << 8 | getFrameData()[15];
}

uint8_t ZBExplicitRxResponse::getOption() {
	return getFrameData()[16];
}

// markers to read data from packet array.
uint8_t ZBExplicitRxResponse::getDataOffset() {
	return 17;
}

uint8_t ZBExplicitRxResponse::getDataLength() {
	return getPacketLength() - getDataOffset() - 1;
}

void XBeeResponse::getZBExplicitRxResponse(XBeeResponse &rxResponse) {
	// Nothing to add to that
	getZBRxResponse(rxResponse);
}


ZBRxIoSampleResponse::ZBRxIoSampleResponse() : ZBRxResponse() {

}

// 64 + 16 addresses, sample size, option = 12 (index 11), so this starts at 12
uint8_t ZBRxIoSampleResponse::getDigitalMaskMsb() {
	return getFrameData()[12] & 0x1c;
}

uint8_t ZBRxIoSampleResponse::getDigitalMaskLsb() {
	return getFrameData()[13];
}

uint8_t ZBRxIoSampleResponse::getAnalogMask() {
	return getFrameData()[14] & 0x8f;
}

bool ZBRxIoSampleResponse::containsAnalog() {
	return getAnalogMask() > 0;
}

bool ZBRxIoSampleResponse::containsDigital() {
	return getDigitalMaskMsb() > 0 || getDigitalMaskLsb() > 0;
}

bool ZBRxIoSampleResponse::isAnalogEnabled(uint8_t pin) {
	return ((getAnalogMask() >> pin) & 1) == 1;
}

bool ZBRxIoSampleResponse::isDigitalEnabled(uint8_t pin) {
	if (pin <= 7) {
		// added extra parens to calm avr compiler
		return ((getDigitalMaskLsb() >> pin) & 1) == 1;
	} else {
		return ((getDigitalMaskMsb() >> (pin - 8)) & 1) == 1;
	}
}

uint16_t ZBRxIoSampleResponse::getAnalog(uint8_t pin) {
	// analog starts 13 bytes after sample size, if no dio enabled
	uint8_t start = 15;

	if (containsDigital()) {
		// make room for digital i/o
		start+=2;
	}

//	std::cout << "spacing is " << static_cast<unsigned int>(spacing) << std::endl;

	// start depends on how many pins before this pin are enabled
	for (int i = 0; i < pin; i++) {
		if (isAnalogEnabled(i)) {
			start+=2;
		}
	}

//	std::cout << "start for analog pin ["<< static_cast<unsigned int>(pin) << "]/sample " << static_cast<unsigned int>(sample) << " is " << static_cast<unsigned int>(start) << std::endl;

//	std::cout << "returning index " << static_cast<unsigned int>(getSampleOffset() + start) << " and index " <<  static_cast<unsigned int>(getSampleOffset() + start + 1) << ", val is " << static_cast<unsigned int>(getFrameData()[getSampleOffset() + start] << 8) <<  " and " <<  + static_cast<unsigned int>(getFrameData()[getSampleOffset() + start + 1]) << std::endl;

	return (uint16_t)((getFrameData()[start] << 8) + getFrameData()[start + 1]);
}

bool ZBRxIoSampleResponse::isDigitalOn(uint8_t pin) {
	if (pin <= 7) {
		// D0-7
		// DIO LSB is index 5
		return ((getFrameData()[16] >> pin) & 1) == 1;
	} else {
		// D10-12
		// DIO MSB is index 4
		return ((getFrameData()[15] >> (pin - 8)) & 1) == 1;
	}
}

void XBeeResponse::getZBRxIoSampleResponse(XBeeResponse &response) {
	ZBRxIoSampleResponse* zb = static_cast<ZBRxIoSampleResponse*>(&response);


	// pass pointer array to subclass
	zb->setFrameData(getFrameData());
	setCommon(response);

	zb->getRemoteAddress64().setMsb((uint32_t(getFrameData()[0]) << 24) + (uint32_t(getFrameData()[1]) << 16) + (uint16_t(getFrameData()[2]) << 8) + getFrameData()[3]);
	zb->getRemoteAddress64().setLsb((uint32_t(getFrameData()[4]) << 24) + (uint32_t(getFrameData()[5]) << 16) + (uint16_t(getFrameData()[6]) << 8) + (getFrameData()[7]));
}

#endif

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

RxIoSampleBaseResponse::RxIoSampleBaseResponse() : RxResponse() {

}

uint8_t RxIoSampleBaseResponse::getSampleOffset() {
	// sample starts 2 bytes after rssi
	return getRssiOffset() + 2;
}


uint8_t RxIoSampleBaseResponse::getSampleSize() {
	return getFrameData()[getSampleOffset()];
}

bool RxIoSampleBaseResponse::containsAnalog() {
	return (getFrameData()[getSampleOffset() + 1] & 0x7e) > 0;
}

bool RxIoSampleBaseResponse::containsDigital() {
	return (getFrameData()[getSampleOffset() + 1] & 0x1) > 0 || getFrameData()[getSampleOffset() + 2] > 0;
}

//uint16_t RxIoSampleBaseResponse::getAnalog0(uint8_t sample) {
//	return getAnalog(0, sample);
//}

bool RxIoSampleBaseResponse::isAnalogEnabled(uint8_t pin) {
	return (((getFrameData()[getSampleOffset() + 1] >> (pin + 1)) & 1) == 1);
}

bool RxIoSampleBaseResponse::isDigitalEnabled(uint8_t pin) {
	if (pin < 8) {
		return ((getFrameData()[getSampleOffset() + 2] >> pin) & 1) == 1;
	} else {
		return (getFrameData()[getSampleOffset() + 1] & 1) == 1;
	}
}

//	// verified (from XBee-API)
//	private int getSampleWidth() {
//		int width = 0;
//
//		// width of sample depends on how many I/O pins are enabled. add one for each analog that's enabled
//		for (int i = 0; i <= 5; i++) {
//			if (isAnalogEnabled(i)) {
//				// each analog is two bytes
//				width+=2;
//			}
//		}
//		
//		if (this.containsDigital()) {
//			// digital enabled takes two bytes, no matter how many pins enabled
//			width+= 2;
//		}
//		
//		return width;
//	}
//
//	private int getStartIndex() {
//
//		int startIndex;
//
//		if (this.getSourceAddress() instanceof XBeeAddress16) {
//			// 16 bit
//			startIndex = 7;
//		} else {
//			// 64 bit
//			startIndex = 13;
//		}
//		
//		return startIndex;
//	}
//	
//	public int getDigitalMsb(int sample) {
//		// msb digital always starts 3 bytes after sample size
//		return this.getProcessedPacketBytes()[this.getStartIndex() + 3 + this.getSampleWidth() * sample];
//	}
//	
//	public int getDigitalLsb(int sample) {
//		return this.getProcessedPacketBytes()[this.getStartIndex() + 3 + this.getSampleWidth() * sample + 1];
//	}	
//
//	public Boolean isDigitalOn(int pin, int sample) {
//		
//		if (sample < 0 || sample >= this.getSampleSize()) {
//			throw new IllegalArgumentException("invalid sample size: " + sample);
//		}
//		
//		if (!this.containsDigital()) {
//			throw new RuntimeException("Digital is not enabled");
//		}
//		
//		if (pin >= 0 && pin < 8) {
//			return ((this.getDigitalLsb(sample) >> pin) & 1) == 1;
//		} else if (pin == 8) {
//			// uses msb dio line
//			return (this.getDigitalMsb(sample) & 1) == 1;
//		} else {
//			throw new IllegalArgumentException("Invalid pin: " + pin);
//		}		
//	}
//	
//	public Integer getAnalog(int pin, int sample) {
//		
//		if (sample < 0 || sample >= this.getSampleSize()) {
//			throw new IllegalArgumentException("invalid sample size: " + sample);
//		}
//		
//		// analog starts 3 bytes after start of sample, if no dio enabled
//		int startIndex = this.getStartIndex() + 3;
//		
//		if (this.containsDigital()) {
//			// make room for digital i/o sample (2 bytes per sample)
//			startIndex+= 2;
//		}
//		
//		startIndex+= this.getSampleWidth() * sample;
//
//		// start depends on how many pins before this pin are enabled
//		// this will throw illegalargumentexception if invalid pin
//		for (int i = 0; i < pin; i++) {
//			if (isAnalogEnabled(i)) {
//				startIndex+=2;
//			}
//		}
//
//		return (this.getProcessedPacketBytes()[startIndex] << 8) + this.getProcessedPacketBytes()[startIndex + 1];		
//	}
				
uint8_t RxIoSampleBaseResponse::getSampleStart(uint8_t sample) {
	uint8_t spacing = 0;

	if (containsDigital()) {
		// make room for digital i/o sample (2 bytes per sample)
		spacing += 2;
	}

	// spacing between samples depends on how many are enabled. add
	// 2 bytes for each analog that's enabled
	for (int i = 0; i <= 5; i++) {
		if (isAnalogEnabled(i)) {
			// each analog is two bytes
			spacing+=2;
		}
	}

	// Skip 3-byte header and "sample" full samples
	return getSampleOffset() + 3 + sample * spacing;
}

uint16_t RxIoSampleBaseResponse::getAnalog(uint8_t pin, uint8_t sample) {
	uint8_t start = getSampleStart(sample);

	if (containsDigital()) {
		// Skip digital sample info
		start += 2;
	}

	// Skip any analog samples before this pin
	for (int i = 0; i < pin; i++) {
		if (isAnalogEnabled(i)) {
			start+=2;
		}
	}

	return (uint16_t)((getFrameData()[start] << 8) + getFrameData()[start + 1]);
}

bool RxIoSampleBaseResponse::isDigitalOn(uint8_t pin, uint8_t sample) {
	if (pin < 8) {
		return ((getFrameData()[getSampleStart(sample) + 1] >> pin) & 1) == 1;
	} else {
		return (getFrameData()[getSampleStart(sample)] & 1) == 1;
	}
}


//bool RxIoSampleBaseResponse::isDigital0On(uint8_t sample) {
//	return isDigitalOn(0, sample);
//}

Rx16IoSampleResponse::Rx16IoSampleResponse() : RxIoSampleBaseResponse() {

}

uint16_t Rx16IoSampleResponse::getRemoteAddress16() {
	return (uint16_t)((getFrameData()[0] << 8) + getFrameData()[1]);
}

uint8_t Rx16IoSampleResponse::getRssiOffset() {
	return 2;
}

void XBeeResponse::getRx16IoSampleResponse(XBeeResponse &response) {
	Rx16IoSampleResponse* rx = static_cast<Rx16IoSampleResponse*>(&response);

	rx->setFrameData(getFrameData());
	setCommon(response);
}


Rx64IoSampleResponse::Rx64IoSampleResponse() : RxIoSampleBaseResponse() {
	_remoteAddress = XBeeAddress64();
}

XBeeAddress64& Rx64IoSampleResponse::getRemoteAddress64() {
	return _remoteAddress;
}

uint8_t Rx64IoSampleResponse::getRssiOffset() {
	return 8;
}

void XBeeResponse::getRx64IoSampleResponse(XBeeResponse &response) {
	Rx64IoSampleResponse* rx = static_cast<Rx64IoSampleResponse*>(&response);

	rx->setFrameData(getFrameData());
	setCommon(response);

	rx->getRemoteAddress64().setMsb((uint32_t(getFrameData()[0]) << 24) + (uint32_t(getFrameData()[1]) << 16) + (uint16_t(getFrameData()[2]) << 8) + getFrameData()[3]);
	rx->getRemoteAddress64().setLsb((uint32_t(getFrameData()[4]) << 24) + (uint32_t(getFrameData()[5]) << 16) + (uint16_t(getFrameData()[6]) << 8) + getFrameData()[7]);
}

TxStatusResponse::TxStatusResponse() : FrameIdResponse() {

}

uint8_t TxStatusResponse::getStatus() {
	return getFrameData()[1];
}

bool TxStatusResponse::isSuccess() {
	return getStatus() == SUCCESS;
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

RemoteAtCommandResponse::RemoteAtCommandResponse() : AtCommandResponse() {

}

uint8_t* RemoteAtCommandResponse::getCommand() {
	return getFrameData() + 11;
}

uint8_t RemoteAtCommandResponse::getStatus() {
	return getFrameData()[13];
}

bool RemoteAtCommandResponse::isOk() {
	// weird c++ behavior.  w/o this method, it calls AtCommandResponse::isOk(), which calls the AtCommandResponse::getStatus, not this.getStatus!!!
	return getStatus() == AT_OK;
}

uint8_t RemoteAtCommandResponse::getValueLength() {
	return getFrameDataLength() - 14;
}

uint8_t* RemoteAtCommandResponse::getValue() {
	if (getValueLength() > 0) {
		// value is only included for query commands.  set commands does not return a value
		return getFrameData() + 14;
	}

	return NULL;
}

uint16_t RemoteAtCommandResponse::getRemoteAddress16() {
	return uint16_t((getFrameData()[9] << 8) + getFrameData()[10]);
}

XBeeAddress64& RemoteAtCommandResponse::getRemoteAddress64() {
	return _remoteAddress64;
}

void XBeeResponse::getRemoteAtCommandResponse(XBeeResponse &response) {

	// TODO no real need to cast.  change arg to match expected class
	RemoteAtCommandResponse* at = static_cast<RemoteAtCommandResponse*>(&response);

	// pass pointer array to subclass
	at->setFrameData(getFrameData());
	setCommon(response);

	at->getRemoteAddress64().setMsb((uint32_t(getFrameData()[1]) << 24) + (uint32_t(getFrameData()[2]) << 16) + (uint16_t(getFrameData()[3]) << 8) + getFrameData()[4]);
	at->getRemoteAddress64().setLsb((uint32_t(getFrameData()[5]) << 24) + (uint32_t(getFrameData()[6]) << 16) + (uint16_t(getFrameData()[7]) << 8) + (getFrameData()[8]));

}

RxDataResponse::RxDataResponse() : XBeeResponse() {

}

uint8_t RxDataResponse::getData(int index) {
	return getFrameData()[getDataOffset() + index];
}

uint8_t* RxDataResponse::getData() {
	return getFrameData() + getDataOffset();
}

FrameIdResponse::FrameIdResponse() {

}

uint8_t FrameIdResponse::getFrameId() {
	return getFrameData()[0];
}


ModemStatusResponse::ModemStatusResponse() {

}

uint8_t ModemStatusResponse::getStatus() {
	return getFrameData()[0];
}

void XBeeResponse::getModemStatusResponse(XBeeResponse &modemStatusResponse) {

	ModemStatusResponse* modem = static_cast<ModemStatusResponse*>(&modemStatusResponse);

	// pass pointer array to subclass
	modem->setFrameData(getFrameData());
	setCommon(modemStatusResponse);

}

AtCommandResponse::AtCommandResponse() {

}

uint8_t* AtCommandResponse::getCommand() {
	return getFrameData() + 1;
}

uint8_t AtCommandResponse::getStatus() {
	return getFrameData()[3];
}

uint8_t AtCommandResponse::getValueLength() {
	return getFrameDataLength() - 4;
}

uint8_t* AtCommandResponse::getValue() {
	if (getValueLength() > 0) {
		// value is only included for query commands.  set commands does not return a value
		return getFrameData() + 4;
	}

	return NULL;
}

bool AtCommandResponse::isOk() {
	return getStatus() == AT_OK;
}

void XBeeResponse::getAtCommandResponse(XBeeResponse &atCommandResponse) {

	AtCommandResponse* at = static_cast<AtCommandResponse*>(&atCommandResponse);

	// pass pointer array to subclass
	at->setFrameData(getFrameData());
	setCommon(atCommandResponse);
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
	_errorCode = NO_ERROR;
	_checksum = 0;
}

void XBeeResponse::reset() {
	init();
	_apiId = 0;
	_msbLength = 0;
	_lsbLength = 0;
	_checksum = 0;
	_frameLength = 0;

	_errorCode = NO_ERROR;
}

void XBee::resetResponse() {
	_pos = 0;
	_escape = false;
	_checksumTotal = 0;
	_response.reset();
}

XBee::XBee(): _response(XBeeResponse()) {
        _pos = 0;
        _escape = false;
        _checksumTotal = 0;
        _nextFrameId = 0;

        _response.init();
        _response.setFrameData(_responseFrameData);
		// Contributed by Paul Stoffregen for Teensy support
#if defined(__AVR_ATmega32U4__) || defined(__MK20DX128__)
        _serial = &Serial1;
#else
        _serial = &Serial;
#endif
}

uint8_t XBee::getNextFrameId() {

	_nextFrameId++;

	if (_nextFrameId == 0) {
		// can't send 0 because that disables status response
		_nextFrameId = 1;
	}

	return _nextFrameId;
}

// Support for SoftwareSerial. Contributed by Paul Stoffregen
void XBee::begin(Stream &serial) {
	_serial = &serial;
}

void XBee::setSerial(Stream &serial) {
	_serial = &serial;
}

bool XBee::available() {
	return _serial->available();
}

uint8_t XBee::read() {
	return _serial->read();
} 

void XBee::flush() {
	_serial->flush();
} 

void XBee::write(uint8_t val) {
	_serial->write(val);
}

XBeeResponse& XBee::getResponse() {
	return _response;
}

// TODO how to convert response to proper subclass?
void XBee::getResponse(XBeeResponse &response) {

	response.setMsbLength(_response.getMsbLength());
	response.setLsbLength(_response.getLsbLength());
	response.setApiId(_response.getApiId());
	response.setFrameLength(_response.getFrameDataLength());

	response.setFrameData(_response.getFrameData());
}

void XBee::readPacketUntilAvailable() {
	while (!(getResponse().isAvailable() || getResponse().isError())) {
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
     	} else if (getResponse().isError()) {
     		return false;
     	}
    }

    // timed out
    return false;
}

void XBee::readPacket() {
	// reset previous response
	if (_response.isAvailable() || _response.isError()) {
		// discard previous packet and start over
		resetResponse();
	}

    while (available()) {

        b = read();

        if (_pos > 0 && b == START_BYTE && ATAP == 2) {
        	// new packet start before previous packeted completed -- discard previous packet and start over
        	_response.setErrorCode(UNEXPECTED_START_BYTE);
        	return;
        }

		if (_pos > 0 && b == ESCAPE) {
			if (available()) {
				b = read();
				b = 0x20 ^ b;
			} else {
				// escape byte.  next byte will be
				_escape = true;
				continue;
			}
		}

		if (_escape == true) {
			b = 0x20 ^ b;
			_escape = false;
		}

		// checksum includes all bytes starting with api id
		if (_pos >= API_ID_INDEX) {
			_checksumTotal+= b;
		}

        switch(_pos) {
			case 0:
		        if (b == START_BYTE) {
		        	_pos++;
		        }

		        break;
			case 1:
				// length msb
				_response.setMsbLength(b);
				_pos++;

				break;
			case 2:
				// length lsb
				_response.setLsbLength(b);
				_pos++;

				break;
			case 3:
				_response.setApiId(b);
				_pos++;

				break;
			default:
				// starts at fifth byte

				if (_pos > MAX_FRAME_DATA_SIZE) {
					// exceed max size.  should never occur
					_response.setErrorCode(PACKET_EXCEEDS_BYTE_ARRAY_LENGTH);
					return;
				}

				// check if we're at the end of the packet
				// packet length does not include start, length, or checksum bytes, so add 3
				if (_pos == (_response.getPacketLength() + 3)) {
					// verify checksum

					//std::cout << "read checksum " << static_cast<unsigned int>(b) << " at pos " << static_cast<unsigned int>(_pos) << std::endl;

					if ((_checksumTotal & 0xff) == 0xff) {
						_response.setChecksum(b);
						_response.setAvailable(true);

						_response.setErrorCode(NO_ERROR);
					} else {
						// checksum failed
						_response.setErrorCode(CHECKSUM_FAILURE);
					}

					// minus 4 because we start after start,msb,lsb,api and up to but not including checksum
					// e.g. if frame was one byte, _pos=4 would be the byte, pos=5 is the checksum, where end stop reading
					_response.setFrameLength(_pos - 4);

					// reset state vars
					_pos = 0;

					return;
				} else {
					// add to packet array, starting with the fourth byte of the apiFrame
					_response.getFrameData()[_pos - 4] = b;
					_pos++;
				}
        }
    }
}

// it's peanut butter jelly time!!

XBeeRequest::XBeeRequest(uint8_t apiId, uint8_t frameId) {
	_apiId = apiId;
	_frameId = frameId;
}

void XBeeRequest::setFrameId(uint8_t frameId) {
	_frameId = frameId;
}

uint8_t XBeeRequest::getFrameId() {
	return _frameId;
}

uint8_t XBeeRequest::getApiId() {
	return _apiId;
}

void XBeeRequest::setApiId(uint8_t apiId) {
	_apiId = apiId;
}

//void XBeeRequest::reset() {
//	_frameId = DEFAULT_FRAME_ID;
//}

//uint8_t XBeeRequest::getPayloadOffset() {
//	return _payloadOffset;
//}
//
//uint8_t XBeeRequest::setPayloadOffset(uint8_t payloadOffset) {
//	_payloadOffset = payloadOffset;
//}


PayloadRequest::PayloadRequest(uint8_t apiId, uint8_t frameId, uint8_t *payload, uint8_t payloadLength) : XBeeRequest(apiId, frameId) {
	_payloadPtr = payload;
	_payloadLength = payloadLength;
}

uint8_t* PayloadRequest::getPayload() {
	return _payloadPtr;
}

void PayloadRequest::setPayload(uint8_t* payload) {
	_payloadPtr = payload;
}

uint8_t PayloadRequest::getPayloadLength() {
	return _payloadLength;
}

void PayloadRequest::setPayloadLength(uint8_t payloadLength) {
	_payloadLength = payloadLength;
}

#ifdef SERIES_2

ZBTxRequest::ZBTxRequest() : PayloadRequest(ZB_TX_REQUEST, DEFAULT_FRAME_ID, NULL, 0) {
	_addr16 = ZB_BROADCAST_ADDRESS;
	_broadcastRadius = ZB_BROADCAST_RADIUS_MAX_HOPS;
	_option = ZB_TX_UNICAST;
}

ZBTxRequest::ZBTxRequest(const XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId): PayloadRequest(ZB_TX_REQUEST, frameId, data, dataLength) {
	_addr64 = addr64;
	_addr16 = addr16;
	_broadcastRadius = broadcastRadius;
	_option = option;
}

ZBTxRequest::ZBTxRequest(const XBeeAddress64 &addr64, uint8_t *data, uint8_t dataLength): PayloadRequest(ZB_TX_REQUEST, DEFAULT_FRAME_ID, data, dataLength) {
	_addr64 = addr64;
	_addr16 = ZB_BROADCAST_ADDRESS;
	_broadcastRadius = ZB_BROADCAST_RADIUS_MAX_HOPS;
	_option = ZB_TX_UNICAST;
}

uint8_t ZBTxRequest::getFrameData(uint8_t pos) {
	if (pos == 0) {
		return (_addr64.getMsb() >> 24) & 0xff;
	} else if (pos == 1) {
		return (_addr64.getMsb() >> 16) & 0xff;
	} else if (pos == 2) {
		return (_addr64.getMsb() >> 8) & 0xff;
	} else if (pos == 3) {
		return _addr64.getMsb() & 0xff;
	} else if (pos == 4) {
		return (_addr64.getLsb() >> 24) & 0xff;
	} else if (pos == 5) {
		return  (_addr64.getLsb() >> 16) & 0xff;
	} else if (pos == 6) {
		return (_addr64.getLsb() >> 8) & 0xff;
	} else if (pos == 7) {
		return _addr64.getLsb() & 0xff;
	} else if (pos == 8) {
		return (_addr16 >> 8) & 0xff;
	} else if (pos == 9) {
		return _addr16 & 0xff;
	} else if (pos == 10) {
		return _broadcastRadius;
	} else if (pos == 11) {
		return _option;
	} else {
		return getPayload()[pos - ZB_TX_API_LENGTH];
	}
}

uint8_t ZBTxRequest::getFrameDataLength() {
	return ZB_TX_API_LENGTH + getPayloadLength();
}

XBeeAddress64& ZBTxRequest::getAddress64() {
	return _addr64;
}

uint16_t ZBTxRequest::getAddress16() {
	return _addr16;
}

uint8_t ZBTxRequest::getBroadcastRadius() {
	return _broadcastRadius;
}

uint8_t ZBTxRequest::getOption() {
	return _option;
}

void ZBTxRequest::setAddress64(const XBeeAddress64& addr64) {
	_addr64 = addr64;
}

void ZBTxRequest::setAddress16(uint16_t addr16) {
	_addr16 = addr16;
}

void ZBTxRequest::setBroadcastRadius(uint8_t broadcastRadius) {
	_broadcastRadius = broadcastRadius;
}

void ZBTxRequest::setOption(uint8_t option) {
	_option = option;
}



ZBExplicitTxRequest::ZBExplicitTxRequest() : ZBTxRequest() {
	_srcEndpoint = DEFAULT_ENDPOINT;
	_dstEndpoint = DEFAULT_ENDPOINT;
	_profileId = DEFAULT_PROFILE_ID;
	_clusterId = DEFAULT_CLUSTER_ID;
	setApiId(ZB_EXPLICIT_TX_REQUEST);
}

ZBExplicitTxRequest::ZBExplicitTxRequest(XBeeAddress64 &addr64, uint16_t addr16, uint8_t broadcastRadius, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId, uint8_t srcEndpoint, uint8_t dstEndpoint, uint16_t clusterId, uint16_t profileId)
: ZBTxRequest(addr64, addr16, broadcastRadius, option, payload, payloadLength, frameId) {
	_srcEndpoint = srcEndpoint;
	_dstEndpoint = dstEndpoint;
	_profileId = profileId;
	_clusterId = clusterId;
	setApiId(ZB_EXPLICIT_TX_REQUEST);
}

ZBExplicitTxRequest::ZBExplicitTxRequest(XBeeAddress64 &addr64, uint8_t *payload, uint8_t payloadLength)
: ZBTxRequest(addr64, payload, payloadLength) {
	_srcEndpoint = DEFAULT_ENDPOINT;
	_dstEndpoint = DEFAULT_ENDPOINT;
	_profileId = DEFAULT_PROFILE_ID;
	_clusterId = DEFAULT_CLUSTER_ID;
	setApiId(ZB_EXPLICIT_TX_REQUEST);
}

uint8_t ZBExplicitTxRequest::getFrameData(uint8_t pos) {
	if (pos < 10) {
		return ZBTxRequest::getFrameData(pos);
	} else if (pos == 10) {
		return _srcEndpoint;
	} else if (pos == 11) {
		return _dstEndpoint;
	} else if (pos == 12) {
		return (_clusterId >> 8) & 0xff;
	} else if (pos == 13) {
		return _clusterId & 0xff;
	} else if (pos == 14) {
		return (_profileId >> 8) & 0xff;
	} else if (pos == 15) {
		return _profileId & 0xff;
	} else if (pos == 16) {
		return _broadcastRadius;
	} else if (pos == 17) {
		return _option;
	} else {
		return getPayload()[pos - ZB_EXPLICIT_TX_API_LENGTH];
	}
}

uint8_t ZBExplicitTxRequest::getFrameDataLength() {
	return ZB_EXPLICIT_TX_API_LENGTH + getPayloadLength();
}

uint8_t ZBExplicitTxRequest::getSrcEndpoint() {
	return _srcEndpoint;
}

uint8_t ZBExplicitTxRequest::getDstEndpoint() {
	return _dstEndpoint;
}

uint16_t ZBExplicitTxRequest::getClusterId() {
	return _clusterId;
}

uint16_t ZBExplicitTxRequest::getProfileId() {
	return _profileId;
}

void ZBExplicitTxRequest::setSrcEndpoint(uint8_t endpoint) {
	_srcEndpoint = endpoint;
}

void ZBExplicitTxRequest::setDstEndpoint(uint8_t endpoint) {
	_dstEndpoint = endpoint;
}

void ZBExplicitTxRequest::setClusterId(uint16_t clusterId) {
	_clusterId = clusterId;
}

void ZBExplicitTxRequest::setProfileId(uint16_t profileId) {
	_profileId = profileId;
}
#endif

#ifdef SERIES_1

Tx16Request::Tx16Request() : PayloadRequest(TX_16_REQUEST, DEFAULT_FRAME_ID, NULL, 0) {
	_option = ACK_OPTION;
}

Tx16Request::Tx16Request(uint16_t addr16, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId) : PayloadRequest(TX_16_REQUEST, frameId, data, dataLength) {
	_addr16 = addr16;
	_option = option;
}

Tx16Request::Tx16Request(uint16_t addr16, uint8_t *data, uint8_t dataLength) : PayloadRequest(TX_16_REQUEST, DEFAULT_FRAME_ID, data, dataLength) {
	_addr16 = addr16;
	_option = ACK_OPTION;
}

uint8_t Tx16Request::getFrameData(uint8_t pos) {

	if (pos == 0) {
		return (_addr16 >> 8) & 0xff;
	} else if (pos == 1) {
		return _addr16 & 0xff;
	} else if (pos == 2) {
		return _option;
	} else {
		return getPayload()[pos - TX_16_API_LENGTH];
	}
}

uint8_t Tx16Request::getFrameDataLength() {
	return TX_16_API_LENGTH + getPayloadLength();
}

uint16_t Tx16Request::getAddress16() {
	return _addr16;
}

void Tx16Request::setAddress16(uint16_t addr16) {
	_addr16 = addr16;
}

uint8_t Tx16Request::getOption() {
	return _option;
}

void Tx16Request::setOption(uint8_t option) {
	_option = option;
}

Tx64Request::Tx64Request() : PayloadRequest(TX_64_REQUEST, DEFAULT_FRAME_ID, NULL, 0) {
	_option = ACK_OPTION;
}

Tx64Request::Tx64Request(XBeeAddress64 &addr64, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId) : PayloadRequest(TX_64_REQUEST, frameId, data, dataLength) {
	_addr64 = addr64;
	_option = option;
}

Tx64Request::Tx64Request(XBeeAddress64 &addr64, uint8_t *data, uint8_t dataLength) : PayloadRequest(TX_64_REQUEST, DEFAULT_FRAME_ID, data, dataLength) {
	_addr64 = addr64;
	_option = ACK_OPTION;
}

uint8_t Tx64Request::getFrameData(uint8_t pos) {

	if (pos == 0) {
		return (_addr64.getMsb() >> 24) & 0xff;
	} else if (pos == 1) {
		return (_addr64.getMsb() >> 16) & 0xff;
	} else if (pos == 2) {
		return (_addr64.getMsb() >> 8) & 0xff;
	} else if (pos == 3) {
		return _addr64.getMsb() & 0xff;
	} else if (pos == 4) {
		return (_addr64.getLsb() >> 24) & 0xff;
	} else if (pos == 5) {
		return (_addr64.getLsb() >> 16) & 0xff;
	} else if (pos == 6) {
		return(_addr64.getLsb() >> 8) & 0xff;
	} else if (pos == 7) {
		return _addr64.getLsb() & 0xff;
	} else if (pos == 8) {
		return _option;
	} else {
		return getPayload()[pos - TX_64_API_LENGTH];
	}
}

uint8_t Tx64Request::getFrameDataLength() {
	return TX_64_API_LENGTH + getPayloadLength();
}

XBeeAddress64& Tx64Request::getAddress64() {
	return _addr64;
}

void Tx64Request::setAddress64(XBeeAddress64& addr64) {
	_addr64 = addr64;
}

uint8_t Tx64Request::getOption() {
	return _option;
}

void Tx64Request::setOption(uint8_t option) {
	_option = option;
}

#endif

AtCommandRequest::AtCommandRequest() : XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID) {
	_command = NULL;
	clearCommandValue();
}

AtCommandRequest::AtCommandRequest(uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) : XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID) {
	_command = command;
	_commandValue = commandValue;
	_commandValueLength = commandValueLength;
}

AtCommandRequest::AtCommandRequest(uint8_t *command) : XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID) {
	_command = command;
	clearCommandValue();
}

uint8_t* AtCommandRequest::getCommand() {
	return _command;
}

uint8_t* AtCommandRequest::getCommandValue() {
	return _commandValue;
}

uint8_t AtCommandRequest::getCommandValueLength() {
	return _commandValueLength;
}

void AtCommandRequest::setCommand(uint8_t* command) {
	_command = command;
}

void AtCommandRequest::setCommandValue(uint8_t* value) {
	_commandValue = value;
}

void AtCommandRequest::setCommandValueLength(uint8_t length) {
	_commandValueLength = length;
}

uint8_t AtCommandRequest::getFrameData(uint8_t pos) {

	if (pos == 0) {
		return _command[0];
	} else if (pos == 1) {
		return _command[1];
	} else {
		return _commandValue[pos - AT_COMMAND_API_LENGTH];
	}
}

void AtCommandRequest::clearCommandValue() {
	_commandValue = NULL;
	_commandValueLength = 0;
}

//void AtCommandRequest::reset() {
//	 XBeeRequest::reset();
//}

uint8_t AtCommandRequest::getFrameDataLength() {
	// command is 2 byte + length of value
	return AT_COMMAND_API_LENGTH + _commandValueLength;
}

XBeeAddress64 RemoteAtCommandRequest::broadcastAddress64 = XBeeAddress64(0x0, BROADCAST_ADDRESS);

RemoteAtCommandRequest::RemoteAtCommandRequest() : AtCommandRequest(NULL, NULL, 0) {
	_remoteAddress16 = 0;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) : AtCommandRequest(command, commandValue, commandValueLength) {
	_remoteAddress64 = broadcastAddress64;
	_remoteAddress16 = remoteAddress16;
	_applyChanges = true;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(uint16_t remoteAddress16, uint8_t *command) : AtCommandRequest(command, NULL, 0) {
	_remoteAddress64 = broadcastAddress64;
	_remoteAddress16 = remoteAddress16;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) : AtCommandRequest(command, commandValue, commandValueLength) {
	_remoteAddress64 = remoteAddress64;
	// don't worry.. works for series 1 too!
	_remoteAddress16 = ZB_BROADCAST_ADDRESS;
	_applyChanges = true;
	setApiId(REMOTE_AT_REQUEST);
}

RemoteAtCommandRequest::RemoteAtCommandRequest(XBeeAddress64 &remoteAddress64, uint8_t *command) : AtCommandRequest(command, NULL, 0) {
	_remoteAddress64 = remoteAddress64;
	_remoteAddress16 = ZB_BROADCAST_ADDRESS;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}

uint16_t RemoteAtCommandRequest::getRemoteAddress16() {
	return _remoteAddress16;
}

void RemoteAtCommandRequest::setRemoteAddress16(uint16_t remoteAddress16) {
	_remoteAddress16 = remoteAddress16;
}

XBeeAddress64& RemoteAtCommandRequest::getRemoteAddress64() {
	return _remoteAddress64;
}

void RemoteAtCommandRequest::setRemoteAddress64(XBeeAddress64 &remoteAddress64) {
	_remoteAddress64 = remoteAddress64;
}

bool RemoteAtCommandRequest::getApplyChanges() {
	return _applyChanges;
}

void RemoteAtCommandRequest::setApplyChanges(bool applyChanges) {
	_applyChanges = applyChanges;
}


uint8_t RemoteAtCommandRequest::getFrameData(uint8_t pos) {
	if (pos == 0) {
		return (_remoteAddress64.getMsb() >> 24) & 0xff;
	} else if (pos == 1) {
		return (_remoteAddress64.getMsb() >> 16) & 0xff;
	} else if (pos == 2) {
		return (_remoteAddress64.getMsb() >> 8) & 0xff;
	} else if (pos == 3) {
		return _remoteAddress64.getMsb() & 0xff;
	} else if (pos == 4) {
		return (_remoteAddress64.getLsb() >> 24) & 0xff;
	} else if (pos == 5) {
		return (_remoteAddress64.getLsb() >> 16) & 0xff;
	} else if (pos == 6) {
		return(_remoteAddress64.getLsb() >> 8) & 0xff;
	} else if (pos == 7) {
		return _remoteAddress64.getLsb() & 0xff;
	} else if (pos == 8) {
		return (_remoteAddress16 >> 8) & 0xff;
	} else if (pos == 9) {
		return _remoteAddress16 & 0xff;
	} else if (pos == 10) {
		return _applyChanges ? 2: 0;
	} else if (pos == 11) {
		return getCommand()[0];
	} else if (pos == 12) {
		return getCommand()[1];
	} else {
		return getCommandValue()[pos - REMOTE_AT_COMMAND_API_LENGTH];
	}
}

uint8_t RemoteAtCommandRequest::getFrameDataLength() {
	return REMOTE_AT_COMMAND_API_LENGTH + getCommandValueLength();
}


// TODO
//GenericRequest::GenericRequest(uint8_t* frame, uint8_t len, uint8_t apiId): XBeeRequest(apiId, *(frame), len) {
//	_frame = frame;
//}

void XBee::send(XBeeRequest &request) {
	// the new new deal

	sendByte(START_BYTE, false);

	// send length
	uint8_t msbLen = ((request.getFrameDataLength() + 2) >> 8) & 0xff;
	uint8_t lsbLen = (request.getFrameDataLength() + 2) & 0xff;

	sendByte(msbLen, true);
	sendByte(lsbLen, true);

	// api id
	sendByte(request.getApiId(), true);
	sendByte(request.getFrameId(), true);

	uint8_t checksum = 0;

	// compute checksum, start at api id
	checksum+= request.getApiId();
	checksum+= request.getFrameId();

	//std::cout << "frame length is " << static_cast<unsigned int>(request.getFrameDataLength()) << std::endl;

	for (int i = 0; i < request.getFrameDataLength(); i++) {
//		std::cout << "sending byte [" << static_cast<unsigned int>(i) << "] " << std::endl;
		sendByte(request.getFrameData(i), true);
		checksum+= request.getFrameData(i);
	}

	// perform 2s complement
	checksum = 0xff - checksum;

//	std::cout << "checksum is " << static_cast<unsigned int>(checksum) << std::endl;

	// send checksum
	sendByte(checksum, true);

	// send packet (Note: prior to Arduino 1.0 this flushed the incoming buffer, which of course was not so great)
	flush();
}

void XBee::sendByte(uint8_t b, bool escape) {

	if (escape && (b == START_BYTE || b == ESCAPE || b == XON || b == XOFF)) {
//		std::cout << "escaping byte [" << toHexString(b) << "] " << std::endl;
		write(ESCAPE);
		write(b ^ 0x20);
	} else {
		write(b);
	}
}


void XBeeWithCallbacks::loop() {
	if (loopTop())
		loopBottom();
}

bool XBeeWithCallbacks::loopTop() {
	readPacket();
	if (getResponse().isAvailable()) {
		_onResponse.call(getResponse());
		return true;
	} else if (getResponse().isError()) {
		_onPacketError.call(getResponse().getErrorCode());
	}
	return false;
}

void XBeeWithCallbacks::loopBottom() {
	bool called = false;
	uint8_t id = getResponse().getApiId();

	if (id == ZB_TX_STATUS_RESPONSE) {
		ZBTxStatusResponse response;
		getResponse().getZBTxStatusResponse(response);
		called = _onZBTxStatusResponse.call(response);
	} else if (id == ZB_RX_RESPONSE) {
		ZBRxResponse response;
		getResponse().getZBRxResponse(response);
		called = _onZBRxResponse.call(response);
	} else if (id == ZB_EXPLICIT_RX_RESPONSE) {
		ZBExplicitRxResponse response;
		getResponse().getZBExplicitRxResponse(response);
		called = _onZBExplicitRxResponse.call(response);
	} else if (id == ZB_IO_SAMPLE_RESPONSE) {
		ZBRxIoSampleResponse response;
		getResponse().getZBRxIoSampleResponse(response);
		called = _onZBRxIoSampleResponse.call(response);
	} else if (id == TX_STATUS_RESPONSE) {
		TxStatusResponse response;
		getResponse().getTxStatusResponse(response);
		called = _onTxStatusResponse.call(response);
	} else if (id == RX_16_RESPONSE) {
		Rx16Response response;
		getResponse().getRx16Response(response);
		called = _onRx16Response.call(response);
	} else if (id == RX_64_RESPONSE) {
		Rx64Response response;
		getResponse().getRx64Response(response);
		called = _onRx64Response.call(response);
	} else if (id == RX_16_IO_RESPONSE) {
		Rx16IoSampleResponse response;
		getResponse().getRx16IoSampleResponse(response);
		called = _onRx16IoSampleResponse.call(response);
	} else if (id == RX_64_IO_RESPONSE) {
		Rx64IoSampleResponse response;
		getResponse().getRx64IoSampleResponse(response);
		called = _onRx64IoSampleResponse.call(response);
	} else if (id == MODEM_STATUS_RESPONSE) {
		ModemStatusResponse response;
		getResponse().getModemStatusResponse(response);
		called = _onModemStatusResponse.call(response);
	} else if (id == AT_COMMAND_RESPONSE) {
		AtCommandResponse response;
		getResponse().getAtCommandResponse(response);
		called = _onAtCommandResponse.call(response);
	} else if (id == REMOTE_AT_COMMAND_RESPONSE) {
		RemoteAtCommandResponse response;
		getResponse().getRemoteAtCommandResponse(response);
		called = _onRemoteAtCommandResponse.call(response);
	}

	if (!called)
		_onOtherResponse.call(getResponse());
}

uint8_t XBeeWithCallbacks::matchStatus(uint8_t frameId) {
	uint8_t id = getResponse().getApiId();
	uint8_t *data = getResponse().getFrameData();
	uint8_t len = getResponse().getFrameDataLength();
	uint8_t offset = 0;

	// Figure out if this frame has a frameId and if so, where the
	// status byte to return is located
	if (id == AT_COMMAND_RESPONSE)
		offset = 3;
	else if (id == REMOTE_AT_COMMAND_RESPONSE)
		offset = 13;
	else if (id == TX_STATUS_RESPONSE)
		offset = 1;
	else if (id == ZB_TX_STATUS_RESPONSE)
		offset = 4;

	// If this is an API frame that contains a status, the frame is
	// long enough to contain it and the frameId matches the one
	// given, return the status byte
	if (offset && offset < len && data[0] == frameId)
		return data[offset];
	return 0xff;
}

uint8_t XBeeWithCallbacks::waitForInternal(uint8_t apiId, void *response, uint16_t timeout, void *func, uintptr_t data, int16_t frameId) {
	unsigned long start = millis();
	do {
		// Wait for a packet of the right type
		if (loopTop()) {
			if (frameId >= 0) {
				uint8_t status = matchStatus(frameId);
				// If a status was found, but it was not
				// a zero success status, stop waiting
				if (status != 0xff && status != 0)
					return status;
			}

			if (getResponse().getApiId() == apiId) {
				// If the type is right, call the right
				// conversion function based on the
				// ApiId and call the match function.
				// Because the match function is
				// essentially called in the same way,
				// regardless of the subclass used, the
				// compiler can reduce most of the below
				// mess into a single piece of code
				// (though for fully optimizing, the
				// separate getXxxResponse() methods
				// must be unified as well).
				switch(apiId) {
					case ZBTxStatusResponse::API_ID: {
						ZBTxStatusResponse *r = (ZBTxStatusResponse*)response;
						bool(*f)(ZBTxStatusResponse&,uintptr_t) = (bool(*)(ZBTxStatusResponse&,uintptr_t))func;
						getResponse().getZBTxStatusResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case ZBRxResponse::API_ID: {
						ZBRxResponse *r = (ZBRxResponse*)response;
						bool(*f)(ZBRxResponse&,uintptr_t) = (bool(*)(ZBRxResponse&,uintptr_t))func;
						getResponse().getZBRxResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case ZBExplicitRxResponse::API_ID: {
						ZBExplicitRxResponse *r = (ZBExplicitRxResponse*)response;
						bool(*f)(ZBExplicitRxResponse&,uintptr_t) = (bool(*)(ZBExplicitRxResponse&,uintptr_t))func;
						getResponse().getZBExplicitRxResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case ZBRxIoSampleResponse::API_ID: {
						ZBRxIoSampleResponse *r = (ZBRxIoSampleResponse*)response;
						bool(*f)(ZBRxIoSampleResponse&,uintptr_t) = (bool(*)(ZBRxIoSampleResponse&,uintptr_t))func;
						getResponse().getZBRxIoSampleResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case TxStatusResponse::API_ID: {
						TxStatusResponse *r = (TxStatusResponse*)response;
						bool(*f)(TxStatusResponse&,uintptr_t) = (bool(*)(TxStatusResponse&,uintptr_t))func;
						getResponse().getTxStatusResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case Rx16Response::API_ID: {
						Rx16Response *r = (Rx16Response*)response;
						bool(*f)(Rx16Response&,uintptr_t) = (bool(*)(Rx16Response&,uintptr_t))func;
						getResponse().getRx16Response(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case Rx64Response::API_ID: {
						Rx64Response *r = (Rx64Response*)response;
						bool(*f)(Rx64Response&,uintptr_t) = (bool(*)(Rx64Response&,uintptr_t))func;
						getResponse().getRx64Response(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case Rx16IoSampleResponse::API_ID: {
						Rx16IoSampleResponse *r = (Rx16IoSampleResponse*)response;
						bool(*f)(Rx16IoSampleResponse&,uintptr_t) = (bool(*)(Rx16IoSampleResponse&,uintptr_t))func;
						getResponse().getRx16IoSampleResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case Rx64IoSampleResponse::API_ID: {
						Rx64IoSampleResponse *r = (Rx64IoSampleResponse*)response;
						bool(*f)(Rx64IoSampleResponse&,uintptr_t) = (bool(*)(Rx64IoSampleResponse&,uintptr_t))func;
						getResponse().getRx64IoSampleResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case ModemStatusResponse::API_ID: {
						ModemStatusResponse *r = (ModemStatusResponse*)response;
						bool(*f)(ModemStatusResponse&,uintptr_t) = (bool(*)(ModemStatusResponse&,uintptr_t))func;
						getResponse().getModemStatusResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case AtCommandResponse::API_ID: {
						AtCommandResponse *r = (AtCommandResponse*)response;
						bool(*f)(AtCommandResponse&,uintptr_t) = (bool(*)(AtCommandResponse&,uintptr_t))func;
						getResponse().getAtCommandResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
					case RemoteAtCommandResponse::API_ID: {
						RemoteAtCommandResponse *r = (RemoteAtCommandResponse*)response;
						bool(*f)(RemoteAtCommandResponse&,uintptr_t) = (bool(*)(RemoteAtCommandResponse&,uintptr_t))func;
						getResponse().getRemoteAtCommandResponse(*r);
						if(!f || f(*r, data))
							return 0;
						break;
					}
				}
			}
			// Call regular callbacks
			loopBottom();
		}
	} while (millis() - start < timeout);
	return XBEE_WAIT_TIMEOUT;
}

uint8_t XBeeWithCallbacks::waitForStatus(uint8_t frameId, uint16_t timeout) {
	unsigned long start = millis();
	do {
		if (loopTop()) {
			uint8_t status = matchStatus(frameId);
			if (status != 0xff)
				return status;

			// Call regular callbacks
			loopBottom();
		}
	} while (millis() - start < timeout);
	return XBEE_WAIT_TIMEOUT ;
}


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

#ifndef XBee_Printers_h
#define XBee_Printers_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "XBee.h"

// Need to define global variables to allow PROGMEM pointers as default
// arguments below. Since these variables are const, there won't be any
// linker conflicts from defining these in a header file.
const char default_byte_sep_arr[] PROGMEM = " ";
const char default_group_sep_arr[] PROGMEM = "\r\n";
const __FlashStringHelper * const default_byte_sep = (const __FlashStringHelper*)default_byte_sep_arr;
const __FlashStringHelper * const default_group_sep = (const __FlashStringHelper*)default_group_sep_arr;

/**
 * Print a buffer byte-by-byte. Each byte is separated by byte_sep and
 * every group_by bytes are separated by group_sep instead.
 *
 * For example, to print 8 bytes per line, each byte separated by a
 * newline, to Serial:
 *
 * printHex(Serial, buf, len, F(" "), F("\r\n"), 8);
 *
 * Values shown are also the defaults.
 *
 * Pass NULL as group_by or byte_sep to not have that separator.
 */
void printHex(Print& p, const uint8_t* buf, size_t len, const __FlashStringHelper* byte_sep = default_byte_sep, const __FlashStringHelper* group_sep = default_group_sep, size_t group_by = 8);

/**
 * Print a single byte, in hex, using a leading zero if needed.
 */
inline void printHex(Print& p, uint8_t v) {
	// Add leading zero if needed
	if (v < 0x10)
		p.write('0');
	p.print(v, HEX);
}

/**
 * Print a 16 bit integer, in hex, using leading zeroes if needed.
 */
inline void printHex(Print& p, uint16_t v) {
	printHex(p, (uint8_t)(v >> 8));
	printHex(p, (uint8_t)v);
}

/**
 * Print a 32 bit integer, in hex, using leading zeroes if needed.
 */
inline void printHex(Print& p, uint32_t v) {
	printHex(p, (uint16_t)(v >> 16));
	printHex(p, (uint16_t)v);
}

/**
 * Print a 64-bit address, in hex, using leading zeroes if needed.
 */
inline void printHex(Print& p, XBeeAddress64 v) {
	printHex(p, v.getMsb());
	printHex(p, v.getLsb());
}

// The following functions are intended to be used as callbacks, to
// print various information about received responses. All of the
// require a Print* to be passed as the data parameter. For example, to
// print to Serial any TxStatusResponses that contain errors, do:
//
// xbee.onTxStatusResponse(printErrorCb, (uintptr_t)(Print*)&Serial);
//
// Most of these callbacks can either be used as a response-specific
// callback, to only work that specific API response type, or as a
// generic callback (onResponse or onOtherResponse), in which case the
// relevant version of the callback will be called automatically.

// printErrorCb prints any error messages in status responses.
void printErrorCb(uint8_t code, uintptr_t data);
void printErrorCb(ZBTxStatusResponse& r, uintptr_t data);
void printErrorCb(TxStatusResponse& r, uintptr_t data);
void printErrorCb(AtCommandResponse& r, uintptr_t data);
void printErrorCb(RemoteAtCommandResponse& r, uintptr_t data);
void printErrorCb(XBeeResponse& r, uintptr_t data);

// printRawResponseCb prints the raw bytes of a response.
void printRawResponseCb(XBeeResponse& response, uintptr_t data);

// printResponseCb prints a human-readable version of a response, showing
// the values of all fields individually.
void printResponseCb(ZBTxStatusResponse& status, uintptr_t data);
void printResponseCb(ZBRxResponse& rx, uintptr_t data);
void printResponseCb(ZBExplicitRxResponse& rx, uintptr_t data);
void printResponseCb(ZBRxIoSampleResponse& rx, uintptr_t data);
void printResponseCb(TxStatusResponse& status, uintptr_t data);
void printResponseCb(Rx16Response& rx, uintptr_t data);
void printResponseCb(Rx64Response& rx, uintptr_t data);
void printResponseCb(Rx16IoSampleResponse& rx, uintptr_t data);
void printResponseCb(Rx64IoSampleResponse& rx, uintptr_t data);
void printResponseCb(ModemStatusResponse& status, uintptr_t data);
void printResponseCb(AtCommandResponse& at, uintptr_t data);
void printResponseCb(RemoteAtCommandResponse& at, uintptr_t data);
void printResponseCb(XBeeResponse& r, uintptr_t data);

// The following functions are non-callback version of the above,
// intended to be called with Print instance (such as Serial) directly,
// saving the casts.

// Print any error in these status response (prints nothing when the
// status is ok).
inline void printError(ZBTxStatusResponse& r, Print& print) { printErrorCb(r, (uintptr_t)(Print*)&print); }
inline void printError(TxStatusResponse& r, Print& print) { printErrorCb(r, (uintptr_t)(Print*)&print); }
inline void printError(AtCommandResponse& r, Print& print) { printErrorCb(r, (uintptr_t)(Print*)&print); }
inline void printError(RemoteAtCommandResponse& r, Print& print) { printErrorCb(r, (uintptr_t)(Print*)&print); }
inline void printError(XBeeResponse& r, Print& print) { printErrorCb(r, (uintptr_t)(Print*)&print); }

// Print the raw bytes of a response
inline void printRawResponse(XBeeResponse& r, Print& print) { printRawResponseCb(r, (uintptr_t)(Print*)&print); }

// Print a human-readable version of a response
inline void printResponse(ZBTxStatusResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(ZBRxResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(ZBExplicitRxResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(ZBRxIoSampleResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(TxStatusResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(Rx16Response& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(Rx64Response& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(Rx16IoSampleResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(Rx64IoSampleResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(ModemStatusResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(AtCommandResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(RemoteAtCommandResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
inline void printResponse(XBeeResponse& r, Print& print) { printResponseCb(r, (uintptr_t)(Print*)&print); }
#endif // XBee_Printers_h

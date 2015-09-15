#include "Printers.h"

void printHex(Print& p, const uint8_t* buf, size_t len, const __FlashStringHelper* byte_sep, const __FlashStringHelper* group_sep, size_t group_by) {
	size_t cur_group = 0;
	while (len--) {
		// Print the group separator whenever starting a new
		// group
		if (group_by && group_sep && cur_group == group_by) {
			p.print(group_sep);
			cur_group = 0;
		}

		// Print the byte separator, except when at the start of
		// a new group (this also excludes the first byte)
		if (cur_group != 0 && byte_sep)
			p.print(byte_sep);

		printHex(p, *buf);

		buf++;
		cur_group++;
	}
}

void printErrorCb(uint8_t code, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->print(F("Error reading API packet. Error code: 0x"));
	p->println(code, HEX);
}

void printErrorCb(ZBTxStatusResponse& r, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	if (!r.isSuccess()) {
		p->print(F("Error sending Zigbee packet. Delivery status: 0x"));
		p->println(r.getDeliveryStatus(), HEX);
	}
}

void printErrorCb(TxStatusResponse& r, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	if (!r.isSuccess()) {
		p->print(F("Error sending packet. Delivery status: 0x"));
		p->println(r.getStatus(), HEX);
	}
}

void printErrorCb(AtCommandResponse& r, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	if (!r.isOk()) {
		p->print(F("Error sending "));
		p->write(r.getCommand(), 2);
		p->print(F(" command. Status: 0x"));
		p->println(r.getStatus(), HEX);
	}
}

void printErrorCb(RemoteAtCommandResponse& r, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	if (!r.isOk()) {
		p->print(F("Error sending remote "));
		p->write(r.getCommand(), 2);
		p->print(F(" command. Status: 0x"));
		p->println(r.getStatus(), HEX);
	}
}

void printErrorCb(XBeeResponse& r, uintptr_t data) {
	uint8_t id = r.getApiId();
	// Figure out the API type and call the corresonding function
	if (id == ZB_TX_STATUS_RESPONSE) {
		ZBTxStatusResponse response;
		r.getZBTxStatusResponse(response);
		printErrorCb(response, data);
	} else if (id == TX_STATUS_RESPONSE) {
		TxStatusResponse response;
		r.getTxStatusResponse(response);
		printErrorCb(response, data);
	} else if (id == AT_COMMAND_RESPONSE) {
		AtCommandResponse response;
		r.getAtCommandResponse(response);
		printErrorCb(response, data);
	} else if (id == REMOTE_AT_COMMAND_RESPONSE) {
		RemoteAtCommandResponse response;
		r.getRemoteAtCommandResponse(response);
		printErrorCb(response, data);
	}
}



void printRawResponseCb(XBeeResponse& response, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->print("Response: ");
	// Reconstruct the original packet
	uint8_t header[] = {START_BYTE, response.getMsbLength(), response.getLsbLength(), response.getApiId()};
	printHex(*p, header, sizeof(header), F(" "), NULL);
	p->write(' ');
	printHex(*p, response.getFrameData(), response.getFrameDataLength(), F(" "), NULL);
	p->println();
}



/**
 * Helper function to print a field name, followed by the hexadecimal
 * value and a newline.
 */
template <typename T>
static void printField(Print* p, const __FlashStringHelper *prefix, T data) {
	p->print(prefix);
	printHex(*p, data);
	p->println();
}

void printResponseCb(ZBTxStatusResponse& status, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println(F("ZBTxStatusResponse:"));
	printField(p, F("  FrameId: 0x"), status.getFrameId());
	printField(p, F("  To: 0x"), status.getRemoteAddress());
	printField(p, F("  Delivery status: 0x"), status.getDeliveryStatus());
	printField(p, F("  Discovery status: 0x"), status.getDiscoveryStatus());
}

void printResponseCb(ZBRxResponse& rx, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println(F("ZBRxResponse:"));
	printField(p, F("  From: 0x"), rx.getRemoteAddress64());
	printField(p, F("  From: 0x"), rx.getRemoteAddress16());
	printField(p, F("  Receive options: 0x"), rx.getOption());
	if (rx.getDataLength() > 8)
		p->print("  Payload:\r\n    ");
	else
		p->print("  Payload: ");
	printHex(*p, rx.getFrameData() + rx.getDataOffset(), rx.getDataLength(), F(" "), F("\r\n    "), 8);
	p->println();
}

void printResponseCb(ZBExplicitRxResponse& rx, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println(F("ZBExplicitRxResponse:"));
	printField(p, F("  From: 0x"), rx.getRemoteAddress64());
	printField(p, F("  From: 0x"), rx.getRemoteAddress16());
	printField(p, F("  Receive options: 0x"), rx.getOption());
	printField(p, F("  Src endpoint: 0x"), rx.getSrcEndpoint());
	printField(p, F("  Dst endpoint: 0x"), rx.getDstEndpoint());
	printField(p, F("  Cluster id: 0x"), rx.getClusterId());
	printField(p, F("  Profile id: 0x"), rx.getProfileId());
	if (rx.getDataLength() > 8)
		p->print("  Payload:\r\n    ");
	else
		p->print("  Payload: ");
	printHex(*p, rx.getFrameData() + rx.getDataOffset(), rx.getDataLength(), F(" "), F("\r\n    "), 8);
	p->println();
}

void printResponseCb(ZBRxIoSampleResponse& rx, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println(F("ZBRxIoSampleResponse:"));
	printField(p, F("  From: 0x"), rx.getRemoteAddress64());
	printField(p, F("  From: 0x"), rx.getRemoteAddress16());
	printField(p, F("  Receive options: 0x"), rx.getOption());
	for (uint8_t i = 0; i < 16; ++i) {
		if (rx.isDigitalEnabled(i)) {
			p->print(F("  Digital pin "));
			p->print(i);
			p->print(F(": "));
			p->print(rx.isDigitalOn(i) ? "HIGH" : "LOW");
			p->println();
		}
	}
	for (uint8_t i = 0; i < 8; ++i) {
		if (rx.isAnalogEnabled(i)) {
			p->print(F("  Analog pin "));
			p->print(i);
			p->print(F(": 0x"));
			printHex(*p, rx.getAnalog(i));
			p->println();
		}
	}
}

void printResponseCb(TxStatusResponse& status, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println(F("TxStatusResponse:"));
	printField(p, F("  FrameId: 0x"), status.getFrameId());
	printField(p, F("  Status: 0x"), status.getStatus());
}

void printResponseCb(Rx16Response& rx, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println("Rx16Response:");
	printField(p, F("  From: 0x"), rx.getRemoteAddress16());
	printField(p, F("  Rssi: 0x"), rx.getRssi());
	printField(p, F("  Receive options: 0x"), rx.getOption());
	if (rx.getDataLength() > 8)
		p->print("  Payload:\r\n    ");
	else
		p->print("  Payload: ");
	printHex(*p, rx.getFrameData() + rx.getDataOffset(), rx.getDataLength(), F(" "), F("\r\n    "), 8);
	p->println();
}

void printResponseCb(Rx64Response& rx, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println("Rx64Response:");
	printField(p, F("  From: 0x"), rx.getRemoteAddress64());
	printField(p, F("  Rssi: 0x"), rx.getRssi());
	printField(p, F("  Receive options: 0x"), rx.getOption());
	if (rx.getDataLength() > 8)
		p->print("  Payload:\r\n    ");
	else
		p->print("  Payload: ");
	printHex(*p, rx.getFrameData() + rx.getDataOffset(), rx.getDataLength(), F(" "), F("\r\n    "), 8);
	p->println();
}

/**
 * Helper function to share common functionality between the two sample
 * resonses.
 */
static void printSamples(Print* p, RxIoSampleBaseResponse& rx) {
	for (uint8_t s = 0; s < rx.getSampleSize(); ++s) {
		p->print(F("  Sample "));
		p->print(s);
		p->println(F(":"));

		for (uint8_t i = 0; i < 9; ++i) {
			if (rx.isDigitalEnabled(i)) {
				p->print(F("    Digital pin "));
				p->print(i);
				p->print(F(": "));
				p->print(rx.isDigitalOn(i, s) ? "HIGH" : "LOW");
				p->println();
			}
		}
		for (uint8_t i = 0; i < 7; ++i) {
			if (rx.isAnalogEnabled(i)) {
				p->print(F("    Analog pin "));
				p->print(i);
				p->print(F(": 0x"));
				printHex(*p, rx.getAnalog(i, s));
				p->println();
			}
		}
	}
}

void printResponseCb(Rx16IoSampleResponse& rx, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println("Rx16IoSampleResponse:");
	printField(p, F("  From: 0x"), rx.getRemoteAddress16());
	printField(p, F("  Rssi: 0x"), rx.getRssi());
	printField(p, F("  Receive options: 0x"), rx.getOption());
	printField(p, F("  Number of samples: 0x"), rx.getSampleSize());
	printSamples(p, rx);
}

void printResponseCb(Rx64IoSampleResponse& rx, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println("Rx64IoSampleResponse:");
	printField(p, F("  From: 0x"), rx.getRemoteAddress64());
	printField(p, F("  Rssi: 0x"), rx.getRssi());
	printField(p, F("  Receive options: 0x"), rx.getOption());
	printField(p, F("  Number of samples: 0x"), rx.getSampleSize());
	printSamples(p, rx);
}

void printResponseCb(ModemStatusResponse& status, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println("ModemStatusResponse:");
	printField(p, F("  Status: 0x"), status.getStatus());
}

void printResponseCb(AtCommandResponse& at, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println("AtCommandResponse:");
	p->print(F("  Command: "));
	p->write(at.getCommand(), 2);
	p->println();
	printField(p, F("  Status: 0x"), at.getStatus());
	if (at.getValueLength()) {
		p->print(F("  Value: "));
		printHex(*p, at.getValue(), at.getValueLength(), F(" "), NULL);
		p->println();
	}
}

void printResponseCb(RemoteAtCommandResponse& at, uintptr_t data) {
	Print *p = (Print*)data;
	if (!p) return;
	p->println("AtRemoteCommandResponse:");
	printField(p, F("  To: 0x"), at.getRemoteAddress64());
	printField(p, F("  To: 0x"), at.getRemoteAddress16());
	p->print(F("  Command: "));
	p->write(at.getCommand(), 2);
	p->println();
	printField(p, F("  Status: 0x"), at.getStatus());
	if (at.getValueLength()) {
		p->print(F("  Value: "));
		printHex(*p, at.getValue(), at.getValueLength(), F(" "), NULL);
		p->println();
	}
}

void printResponseCb(XBeeResponse& r, uintptr_t data) {
	uint8_t id = r.getApiId();
	// Figure out the API type and call the corresponding function
	if (id == ZB_TX_STATUS_RESPONSE) {
		ZBTxStatusResponse response;
		r.getZBTxStatusResponse(response);
		printResponseCb(response, data);
	} else if (id == ZB_RX_RESPONSE) {
		ZBRxResponse response;
		r.getZBRxResponse(response);
		printResponseCb(response, data);
	} else if (id == ZB_EXPLICIT_RX_RESPONSE) {
		ZBExplicitRxResponse response;
		r.getZBExplicitRxResponse(response);
		printResponseCb(response, data);
	} else if (id == ZB_IO_SAMPLE_RESPONSE) {
		ZBRxIoSampleResponse response;
		r.getZBRxIoSampleResponse(response);
		printResponseCb(response, data);
	} else if (id == TX_STATUS_RESPONSE) {
		TxStatusResponse response;
		r.getTxStatusResponse(response);
		printResponseCb(response, data);
	} else if (id == RX_16_RESPONSE) {
		Rx16Response response;
		r.getRx16Response(response);
		printResponseCb(response, data);
	} else if (id == RX_64_RESPONSE) {
		Rx64Response response;
		r.getRx64Response(response);
		printResponseCb(response, data);
	} else if (id == RX_16_IO_RESPONSE) {
		Rx16IoSampleResponse response;
		r.getRx16IoSampleResponse(response);
		printResponseCb(response, data);
	} else if (id == RX_64_IO_RESPONSE) {
		Rx64IoSampleResponse response;
		r.getRx64IoSampleResponse(response);
		printResponseCb(response, data);
	} else if (id == MODEM_STATUS_RESPONSE) {
		ModemStatusResponse response;
		r.getModemStatusResponse(response);
		printResponseCb(response, data);
	} else if (id == AT_COMMAND_RESPONSE) {
		AtCommandResponse response;
		r.getAtCommandResponse(response);
		printResponseCb(response, data);
	} else if (id == REMOTE_AT_COMMAND_RESPONSE) {
		RemoteAtCommandResponse response;
		r.getRemoteAtCommandResponse(response);
		printResponseCb(response, data);
	}
}

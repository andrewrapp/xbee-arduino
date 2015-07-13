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

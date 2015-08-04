// This file contains defines and structs, taken from
// https://github.com/digidotcom/xbee_ansic_library/blob/master/include/
// Response headers were modified to include a transaction field and
// some new content was added for the xbee-arduino library.

/*
 * Copyright (c) 2010-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */


#ifndef __XBEE_ZIGBEE_H
#define __XBEE_ZIGBEE_H

#define PACKED_STRUCT struct __attribute__((__packed__))
typedef uint64_t addr64;

/**
 * ZigBee Stack Profile IDs
 * 4-bit values used in ZigBee beacons
 */
/// Network Specific
#define WPAN_STACK_PROFILE_PROPRIETARY				0x0
/// ZigBee (2006)
#define WPAN_STACK_PROFILE_ZIGBEE					0x1
/// ZigBee PRO (2007)
#define WPAN_STACK_PROFILE_ZIGBEE_PRO				0x2

/**
 * Profile IDs
 */
/// ZigBee Device Object (aka ZigBee Device Profile)
#define WPAN_PROFILE_ZDO				0x0000

/// Smart Energy Profile
#define WPAN_PROFILE_SMART_ENERGY	0x0109

/// Digi International, mfg-specific
#define WPAN_PROFILE_DIGI				0xC105

/**
 * List of fixed endpoints
 */
/// ZigBee Device Object/Profile
#define WPAN_ENDPOINT_ZDO			0x00
/// Digi Smart Energy
#define WPAN_ENDPOINT_DIGI_SE		0x5E
/// Digi Device Objects
#define WPAN_ENDPOINT_DDO			0xE6
/// Digi Data
#define WPAN_ENDPOINT_DIGI_DATA	0xE8
/// Broadcast Endpoint
#define WPAN_ENDPOINT_BROADCAST	0xFF
//@}

// Status values
#define ZDO_STATUS_SUCCESS					0x00
// 0x01 to 0x7F are reserved
#define ZDO_STATUS_INV_REQUESTTYPE		0x80
#define ZDO_STATUS_DEVICE_NOT_FOUND		0x81
#define ZDO_STATUS_INVALID_EP				0x82
#define ZDO_STATUS_NOT_ACTIVE				0x83
#define ZDO_STATUS_NOT_SUPPORTED			0x84
#define ZDO_STATUS_TIMEOUT					0x85
#define ZDO_STATUS_NO_MATCH				0x86
// 0x87 is reserved
#define ZDO_STATUS_NO_ENTRY				0x88
#define ZDO_STATUS_NO_DESCRIPTOR			0x89
#define ZDO_STATUS_INSUFFICIENT_SPACE	0x8A
#define ZDO_STATUS_NOT_PERMITTED			0x8B
#define ZDO_STATUS_TABLE_FULL				0x8C
#define ZDO_STATUS_NOT_AUTHORIZED		0x8D

/// Cluster IDs with the high bit set are responses.
#define ZDO_CLUST_RESPONSE_MASK			0x8000
#define ZDO_CLUST_IS_RESPONSE(c)		(c & ZDO_CLUST_RESPONSE_MASK)

/*********************************************************
					Simple Descriptor
**********************************************************/
/// cluster ID for ZDO Simple_Desc request
#define ZDO_SIMPLE_DESC_REQ	0x0004
/// cluster ID for ZDO Simple_Desc response
#define ZDO_SIMPLE_DESC_RSP	0x8004
/// frame format for ZDO Simple_Desc request
typedef PACKED_STRUCT zdo_simple_desc_req_t {
	uint8_t		transaction;
	uint16_t		network_addr_le;
	uint8_t		endpoint;			///< 0x01 to 0xFE
} zdo_simple_desc_req_t;

/// header for ZDO Simple_Desc response, followed by a SimpleDescriptor
typedef PACKED_STRUCT zdo_simple_desc_resp_header_t {
	uint8_t		transaction;
	uint8_t		status;				///< see ZDO_STATUS_* macros
	uint16_t		network_addr_le;	///< device's network address (little-endian)
	uint8_t		length;				///< length of simple descriptor
	// variable-length simple descriptor follows
} zdo_simple_desc_resp_header_t;

/// header for ZDO SimpleDescriptor (part of a Simple_Desc response), followed
/// by uint8_t input cluster count, multiple uint16_t input cluster IDs,
/// uint8_t output cluster count, multiple uint16_t output cluster IDs
typedef PACKED_STRUCT zdo_simple_desc_header_t {
	uint8_t		endpoint;			///< 0x01 to 0xFE
	uint16_t		profile_id_le;		///< endpoint's profile ID (little-endian)
	uint16_t		device_id_le;		///< endpoint's device ID (little-endian)
	uint8_t		device_version;	///< upper 4 bits are reserved
	// variable-length cluster counts and ids follow
} zdo_simple_desc_header_t;


/*********************************************************
					Active EP Descriptor
**********************************************************/
/// cluster ID for ZDO Active_EP request
#define ZDO_ACTIVE_EP_REQ		0x0005
/// cluster ID for ZDO Active_EP response
#define ZDO_ACTIVE_EP_RSP		0x8005
/// frame format for ZDO Active_EP request
/// @see zdo_send_descriptor_req()
typedef PACKED_STRUCT zdo_active_ep_req_t {
	uint8_t		transaction;
	uint16_t		network_addr_le;
} zdo_active_ep_req_t;

/// header for ZDO Active_EP response, followed by \c .ep_count
/// uint8_t endpoints
typedef PACKED_STRUCT zdo_active_ep_rsp_header_t {
	uint8_t		transaction;
	uint8_t		status;					///< see ZDO_STATUS_* macros
	uint16_t		network_addr_le;
	uint8_t		ep_count;
	uint8_t endpoints[];
} zdo_active_ep_rsp_header_t;

/*********************************************************
					Management LQI Request
**********************************************************/

/// cluster ID for ZDO LQI Request
#define ZDO_MGMT_LQI_REQ			0x0031
/// cluster ID for ZDO LQI Response
#define ZDO_MGMT_LQI_RSP			0x8031

/// frame format for a ZDO LQI Request
typedef PACKED_STRUCT zdo_mgmt_lqi_req_t {
	uint8_t		transaction;
	uint8_t		start_index;
} zdo_mgmt_lqi_req_t;

/// frame format for a ZDO LQI Table Entry
typedef PACKED_STRUCT zdo_mgmt_lqi_entry_t {
	uint64_t extended_pan_id_le;
	uint64_t extended_addr_le;
	uint16_t nwk_addr_le;
	uint8_t flags0;
		#define ZDO_MGMT_LQI_REQ_TYPE_COORDINATOR 0x0
		#define ZDO_MGMT_LQI_REQ_TYPE_ROUTER 0x1
		#define ZDO_MGMT_LQI_REQ_TYPE_ENDDEVICE 0x2
		#define ZDO_MGMT_LQI_REQ_TYPE_UNKNOWN 0x3
	uint8_t flags1;
	uint8_t depth;
	uint8_t lqi;
} zdo_mgt_lqi_entry_t;

/// frame format for a ZDO LQI Response
typedef PACKED_STRUCT zdo_mgmt_lqi_rsp_t {
	uint8_t		transaction;
	uint8_t		status;
	uint8_t		table_entries;
	uint8_t		start_index;
	uint8_t		list_count;
	zdo_mgmt_lqi_entry_t		entries[];
} zdo_mgmt_lqi_rsp_t;


#endif		// __XBEE_ZIGBEE_H

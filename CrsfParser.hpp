#ifndef CRSF_H
#define CRSF_H

#include <stdbool.h>
#include <stdint.h>

#define CRSF_CHANNEL_COUNT 16

struct CrsfChannelData_t {
	float channels[CRSF_CHANNEL_COUNT];
};

struct CrsfLinkStatistics_t {
	uint8_t uplink_rssi_1;
	uint8_t uplink_rssi_2;
	uint8_t uplink_link_quality;
	int8_t uplink_snr;
	uint8_t active_antenna;
	uint8_t rf_mode;
	uint8_t uplink_tx_power;
	uint8_t downlink_rssi;
	uint8_t downlink_link_quality;
	int8_t downlink_snr;
};

enum CRSF_MESSAGE_TYPE {
	CRSF_MESSAGE_TYPE_RC_CHANNELS,
	CRSF_MESSAGE_TYPE_LINK_STATISTICS,
};

typedef struct {
	CRSF_MESSAGE_TYPE message_type;

	union {
		CrsfChannelData_t channel_data;
		CrsfLinkStatistics_t link_statistics;
	};
} CrsfPacket_t;

typedef struct
{
  uint32_t disposed_bytes;
  uint32_t crcs_valid_known_packets;
  uint32_t crcs_valid_unknown_packets;
  uint32_t crcs_invalid;
  uint32_t invalid_known_packet_sizes;
} CrsfParserStatistics_t;

void Crsf_Init(void);
bool Crsf_LoadBuffer(const uint8_t *buffer, const uint32_t size);
bool Crsf_TryParseCrsfPacket(CrsfPacket_t *const new_packet, CrsfParserStatistics_t* const parser_statistics);
uint32_t Csrf_FreeQueueSize(void);

#endif
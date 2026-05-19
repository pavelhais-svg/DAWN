#include "ieee80211_utils.h"

double iee80211_calculate_bitrate(uint8_t supp_rate_val) {
    return ((double) supp_rate_val) / 2;
}

double iee80211_calculate_expected_throughput_mbit(int exp_thr) {
    return (((double) exp_thr) / 1000);
}

/* IEEE 802.11-2020 §9.4.2.21.7: RCPI is encoded as (2*dBm + 220) in the range
 * 0..220 (0.5 dB steps). Values outside that range are reserved/invalid in the
 * standard, and callers in this code base also use (uint32_t)-1 / 0xFFFFFFFF as
 * an "absent" sentinel which arrives here as a negative int. Return 0 (matches
 * the "no measurement" sentinel used by probe_entry->signal) in those cases so
 * scoring paths can uniformly treat 0 as "skip". */
int rcpi_to_rssi(int rcpi)
{
    if (rcpi < 0 || rcpi > 220)
        return 0;
    return rcpi / 2 - 110;
}

static int get_rrm_mode_val(char mode) {
    int ret = 0;
    switch (mode) {
    case 'A':
    case 'a':
        ret = WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE;
        break;
    case 'P':
    case 'p':
        ret = WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE;
        break;
    case 'B':
    case 'b':
    case 'T':
    case 't':
        ret = WLAN_RRM_CAPS_BEACON_REPORT_TABLE;
        break;
    }
    return ret;
}

int parse_rrm_mode(int* rrm_mode_order, const char* mode_string) {
    if (!mode_string)
        mode_string = DEFAULT_RRM_MODE_ORDER;

    int mask = 0;
    int order = 0;
    while ((*mode_string != 0) && (order < __RRM_BEACON_RQST_MODE_MAX)) {
        int mode_val = get_rrm_mode_val(*mode_string);
        if (mode_val && (mask & mode_val) == 0)
        {
            rrm_mode_order[order++] = mode_val;
            mask |= mode_val;
        }

        mode_string++;
    }

    while (order < __RRM_BEACON_RQST_MODE_MAX)
    {
        rrm_mode_order[order++] = 0;
    }

    return mask;
}

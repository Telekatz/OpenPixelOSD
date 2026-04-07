/* SPDX-License-Identifier: GPL-2.0-only */
/**
 * Copyright (C) 2025 Vitaliy N <vitaliy.nimych@gmail.com>
 */
#include "vtx_msp.h"
#include "main.h"

#if defined(USE_VTX)

#include "msp.h"
#include "msp_displayport.h"
#include "msp_protocol.h"
#include "rf_pa.h"
#include "rtc6705.h"
#include "uart.h"
#include "usb.h"
#include "rf_pa.h"
#include "flash.h"
#include "led.h"

#include <string.h>
#include <stdio.h>

#ifndef BAND_TABLE 
#define BAND_TABLE          BAND_TABLE_DEFAULT
#endif

static void vtx_apply_hw(const vtx_config_t *cfg);
static const vtx_band_t g_bands[] = BAND_TABLE;

#define NUM_BANDS           (sizeof(g_bands)/sizeof(g_bands[0]))

static vtx_config_t g_cfg = {
    .band = 5,
    .channel = 1,
    .frequency = 5800,
    .power = 1,
    .pitmode = 0,
    .configSet = 0,
};

/* ------------------------- MSP payload definitions -------------------------- */
/* Structures match Betaflight MSP_VTX_CONFIG payload layout (compact). */
#pragma pack(push,1)
typedef struct {
    uint8_t device_type;   /* 0=NONE/unknown, keep 0 */
    uint8_t band;          /* 1..5 or 0 if using frequency */
    uint8_t channel;       /* 1..8 */
    uint8_t power;         /* power index (0..N-1) */
    uint16_t frequency;    /* MHz, non-zero overrides band/channel */
    uint8_t pitmode;       /* 0/1 */
} msp_vtx_config_t;

typedef struct {
    uint8_t band;          /* 1..5 */
    uint8_t channel;       /* 1..8 */
    uint16_t frequency;    /* MHz */
} msp_vtx_table_freq_t;

typedef struct {
    uint8_t index;         /* 0..N-1 */
    uint16_t power_mw;     /* mW */
    char label[16];        /* "25", "100", ... */
} msp_vtx_table_power_t;
#pragma pack(pop)

const vtx_config_t* vtx_get_config(void)
{
    return &g_cfg;
}

const char* vtx_get_band_name(uint8_t band)
{
    return (char*)&g_bands[band].band_name;
}

uint8_t vtx_get_band_count(void)
{
    return NUM_BANDS;
}

uint16_t vtx_get_power_mw(void)
{
    return powerTable[g_cfg.power].mW;
}

uint16_t vtx_get_frequency(uint8_t band, uint8_t channel)
{
    return g_bands[band].freq[channel];
}

void vtx_set_pitmode(uint8_t pitmode)
{
    g_cfg.pitmode = pitmode;
    vtx_apply_hw(&g_cfg);
}

static inline void msp_tx_send(uint8_t owner, const uint8_t *buf, uint16_t len)
{
    if (owner == MSP_OWNER_USB) {
        #if defined(USE_USB)
        usb_uart_write_bytes((const char*)buf, len);
        #endif
    } else if (owner == MSP_OWNER_UART) {
        uart1_tx_dma((uint8_t*)buf, len);
    }
}

static inline bool freq_is_in_58ghz(uint16_t mhz)
{
    return (mhz >= 5600 && mhz <= 6000);
}

void vtx_set_band_channel(int8_t band, uint8_t channel)
{
    if(freq_is_in_58ghz(g_bands[band].freq[channel])) {
        g_cfg.band = band;
        g_cfg.channel = channel;
        g_cfg.frequency = g_bands[band-1].freq[channel-1];
        vtx_apply_hw(&g_cfg);
    }
    
}

void vtx_set_power(int8_t power)
{
    g_cfg.power = power;
    vtx_apply_hw(&g_cfg);
}

static void vtx_apply_hw(const vtx_config_t *cfg)
{
    printf("vtx_apply_hw: band=%d ch=%d freq=%d power=%d pit=%d\n",
           cfg->band, cfg->channel, cfg->frequency, cfg->power, cfg->pitmode);
    
    /* disable external RF Power Amplifier */
    rf_pa_set_power_level(RF_PA_PWR_OFF);

    /* Set internal RTC6705 PA to minimum */
    rtc6705_allow_power_writes(true);
    rtc6705_set_power(RTC6705_LOW);
    rtc6705_allow_power_writes(false);
    if(g_cfg.configSet)
      led_set(0,RGB_BLUE);
    else
      led_set(0,RGB_RED);

    /* Program synthesizer frequency (MHz) */
    if (freq_is_in_58ghz(cfg->frequency)) {
      rtc6705_set_frequency(cfg->frequency);
      /* Set power */
      if (!cfg->pitmode) {
        /* Set internal RTC6705 PA power */
        rtc6705_allow_power_writes(true);
        rtc6705_set_power(powerTable[cfg->power].rtcPA);
        rtc6705_allow_power_writes(false);

        /* Set external RF Power Amplifier */
        rf_pa_set_power_level(cfg->power);
        led_set(0,RGB_GREEN);
      }
    } else {
      rtc6705_set_frequency(0);
      led_set(0,RGB_RED);
    }
    
}

static void handle_msp_set_vtx_config(uint8_t owner, const uint8_t *payload, uint16_t data_size)
{
    /* MSP_VTX_CONFIG payload (Betaflight): 15 bytes
       [0] vtxType
       [1] band (1..N)
       [2] channel (1..8)
       [3] power (1..P)  <-- BF is 1-based
       [4] pit mode (0/1)
       [5] freq LSB
       [6] freq MSB      <-- MHz (non-zero overrides band/channel)
       [7] deviceIsReady
       [8] lowPowerDisarm (0/1)
       [9]  pitModeFreq LSB
       [10] pitModeFreq MSB
       [11] vtxTableAvailable (0/1)
       [12] bands
       [13] channels
       [14] powerLevels
    */

    if (!payload || data_size < 15) {
        return; // malformed
    }
    (void)owner;

    /* Parse all raw fields */
    const uint8_t vtx_type          = payload[0];
    uint8_t band_raw                = payload[1];
    uint8_t ch_raw                  = payload[2];
    uint8_t power_1based            = payload[3];
    uint8_t pitmode                 = payload[4];
    const uint16_t freq_mhz         = (uint16_t)payload[5] | ((uint16_t)payload[6] << 8);
    const uint8_t device_ready      = payload[7];
    const uint8_t low_power_disarm  = payload[8];
    uint16_t pit_mode_freq          = (uint16_t)payload[9] | ((uint16_t)payload[10] << 8);
    uint8_t vtx_table_available     = payload[11];
    uint8_t vtx_table_bands         = payload[12];
    uint8_t vtx_table_channels      = payload[13];
    uint8_t vtx_table_power_levels  = payload[14];

    if (!vtx_table_available) {
        return; // ignore if no VTX table
    }


    /* If LPD is active, force the lowest power level. */
    if (low_power_disarm) {
        power_1based = 1;
    }

    /* Clamp power index to our table */
    if (power_1based < 1) power_1based = 1;
    if ((unsigned)power_1based > rf_pa_power_count()) power_1based = (int)rf_pa_power_count();

    /* Update runtime config */
    g_cfg.pitmode = pitmode ? 1 : 0;
    g_cfg.power = (uint8_t)power_1based;

    g_cfg.channel = ch_raw;
    g_cfg.band = band_raw;

    if(band_raw) {
      g_cfg.frequency = g_bands[band_raw - 1].freq[ch_raw - 1];
    } else {
      g_cfg.frequency = freq_mhz;
    }
    
    if(vtx_table_bands != vtx_get_band_count() || vtx_table_power_levels != rf_pa_power_count()) {
      g_cfg.vtx_table_available = 0;
    } else {
      g_cfg.vtx_table_available = vtx_table_available;
    }

    g_cfg.configSet = 1;
    
    /* Apply to hardware */
    static uint16_t last_freq;
    static uint8_t last_power;
    static uint8_t last_pitmode;
    if (last_freq != g_cfg.frequency || last_power != g_cfg.power || last_pitmode != g_cfg.pitmode) {
        vtx_apply_hw(&g_cfg);
        last_freq = g_cfg.frequency;
        last_power = g_cfg.power;
        last_pitmode = g_cfg.pitmode;
    }

#if 0 // debug print
    printf("MSP_VTX_CONFIG parsed: type=%u band=%u ch=%u power=%u pit=%u freq=%u avail=%u\r\n",
           vtxType,
           g_cfg.band,
           g_cfg.channel,
           g_cfg.power,
           g_cfg.pitmode,
           g_cfg.frequency,
           vtx_table_available);
#endif
    // Silence unused variable warnings
    (void) vtx_type;
    (void) device_ready;
    (void) pit_mode_freq;
    (void) vtx_table_bands;
    (void) vtx_table_channels;
    (void) vtx_table_power_levels;
}

void vtx_msp_clear_table_and_set_defaults(uint8_t owner)
{
    //if (g_cfg.vtx_table_available == 1) {
    //    return; // VTX table already present, do nothing
    //}

    // Reset VTX table to defaults
    uint8_t p[15] = {0};
    p[0]  = 0;                          /* idx LSB (legacy BF field, keep 0) */
    p[1]  = 0;                          /* idx MSB */
    p[2]  = 1;                          /* power index */
    p[3]  = 0;                          /* pitmode (0/1) */
    p[4]  = 0;                          /* lowPowerDisarm */
    p[5]  = 0; p[6]  = 0;               /* pitModeFreq (LSB/MSB), 0 if unused */
    p[7]  = vtx_get_band_count();       /* newBand (1..NUM_BANDS) */
    p[8]  = VTX_CHANNEL_COUNT;          /* newChannel (1..8) */
    p[9]  = 0; p[10] = 0;               /* newFreq LSB/MSB, 0 => use band/channel */
    p[11] = vtx_get_band_count();       /* newBandCount: BF expects "6"*/
    p[12] = VTX_CHANNEL_COUNT;          /* newChannelCount (8) */
    p[13] = rf_pa_power_count();        /* newPowerCount: */
    p[14] = 1;                          /* vtx table should be cleared */

    uint8_t tx_buff[64];
    const uint16_t len = construct_msp_command_v1(tx_buff, MSP_SET_VTX_CONFIG, p, sizeof(p), MSP_OUTBOUND);
    msp_tx_send_owner(owner, tx_buff, len);

    // Push new full VTX tables && save to FC EEPROM
    vtx_msp_push_power_table(owner);
    vtx_msp_push_band_table(owner);
    vtx_msp_eeprom_write(owner);

    g_cfg.vtx_table_available = 1;
}

/* Power table
 * Send MSP_SET_VTXTABLE_POWERLEVEL for each entry.
 * Payload:
 *   [0] index (1..N)
 *   [1..2] power_mW (uint16 LE)
 *   [3] label_len
 *   [4..] ASCII label (e.g. "25","100","800")
 */
void vtx_msp_push_power_table(uint8_t owner)
{
    for (uint8_t i = 1; i <= rf_pa_power_count(); i++) {
        const uint16_t mw  = powerTable[i].mW;

        uint8_t p[1 + 2 + 1 + 16] = {0};
        p[0] = i;
        p[1] = (uint8_t)(mw & 0xFF);
        p[2] = (uint8_t)((mw >> 8) & 0xFF);
        p[3] = sizeof(powerTable[i].label);
        for(uint8_t c = 0; c < p[3]; c++) {
          p[4 + c] = powerTable[i].label[c];
        }

        uint8_t tx_buff[64];
        const uint16_t len = construct_msp_command_v1(tx_buff,
                            MSP_SET_VTXTABLE_POWERLEVEL,
                            p, (uint8_t)(4 + p[3]),
                            MSP_OUTBOUND);

        msp_tx_send_owner(owner, tx_buff, len);
    }
}

void vtx_msp_push_band_table(uint8_t owner)
{
    for (uint8_t b = 1; b <= vtx_get_band_count(); b++) {
        const vtx_band_t *band = &g_bands[b-1];

        /* Payload layout (29 bytes):
           [0]=band(1..N), [1]=nameLen(=8), [2..9]=name8,
           [10]=letter, [11]=isFactory(1), [12]=channels(8),
           [13..28]=8×freq LE16
        */
        uint8_t p[29] = {0};
        p[0] = b;
        p[1] = VTX_CH_LABEL_COUNT;          /* 8 */

        /* Name (exactly 8 bytes) */
        for (uint8_t i = 0; i < VTX_CH_LABEL_COUNT; i++) {
            p[2 + i] = band->band_name[i];
        }

        p[10] = (uint8_t)band->letter;      /* single ASCII letter */
        p[11] = 1;                          /* factory band flag */
        p[12] = VTX_CHANNEL_COUNT;          /* 8 */

        /* 8 frequencies, little-endian MHz */
        for (uint8_t ch = 0; ch < VTX_CHANNEL_COUNT; ch++) {
            const uint16_t f = band->freq[ch];
            p[13 + ch*2 + 0] = (uint8_t)(f & 0xFF);
            p[13 + ch*2 + 1] = (uint8_t)(f >> 8);
        }

        uint8_t tx_buff[64];
        const uint16_t len = construct_msp_command_v2(tx_buff,MSP_SET_VTXTABLE_BAND, p, (uint8_t)sizeof(p), MSP_PACKET_COMMAND);

        msp_tx_send_owner(owner, tx_buff, len);
    }
}

void vtx_msp_push_calibration_table(uint8_t owner)
{
    for (uint8_t i = 0; i <= rf_pa_power_count(); i++) {
        const uint16_t mw  = powerTable[i].mW;

        uint8_t p[1 + 2 + 1 + 32] = {0};
        p[0] = i;
        p[1] = (uint8_t)(mw & 0xFF);
        p[2] = (uint8_t)((mw >> 8) & 0xFF);
        for(uint8_t c = 0; c < 7; c++) {
          p[3 + (c * 2)] = (uint8_t)(powerTable[i].calibration[c] & 0xFF);
          p[4 + (c * 2)] =(uint8_t)((powerTable[i].calibration[c] >> 8) & 0xFF);
        }
        for(uint8_t c = 0; c < 7; c++) {
          p[17 + (c * 2)] = (uint8_t)(powerTable[i].detector[c] & 0xFF);
          p[18 + (c * 2)] = (uint8_t)((powerTable[i].detector[c] >> 8) & 0xFF);
        }

        uint8_t tx_buff[64];
        const uint16_t len = construct_msp_command_v2(tx_buff,
                            MSP_SET_PACALTABLE,
                            p, (uint8_t)(3 + 14 + 14),
                            MSP_PACKET_COMMAND);

        msp_tx_send_owner(owner, tx_buff, len);
    }
}

void vtx_msp_set_calibration_table(uint8_t owner, const uint8_t *payload, uint16_t data_size)
{
    if (!payload || data_size < 17) {
        return; // malformed
    }
    (void)owner;

    const uint16_t level  = payload[0];

    if (!level || level > rf_pa_power_count()) {
        return;
    }

    TRACE_INFO("SET PA table %i\n", level);

    for(uint8_t c = 0; c < 7; c++) {
        uint16_t pa_mv = payload[3 + (c * 2)] + (uint16_t)(payload[4 + (c * 2)]<<8);
        powerTable[level].calibration[c] = pa_mv;
    }

    if ( data_size >= 31) {
        for(uint8_t c = 0; c < 7; c++) {
        uint16_t rf_detector = payload[17 + (c * 2)] + (uint16_t)(payload[18 + (c * 2)]<<8);
        powerTable[level].detector[c] = rf_detector;
      }  
    }
    rf_pa_write_eeprom(level);
}

extern double rf_detector;

void vtx_msp_push_calibration(uint8_t owner)
{
    uint8_t p[5] = {0};
    uint16_t pa_int = rf_detector;

    p[0] = g_cfg.power;
    p[1] = (uint8_t)(rf_pa_get_vref_mv() & 0xFF);
    p[2] = (uint8_t)((rf_pa_get_vref_mv() >> 8) & 0xFF);
    p[3] = (uint8_t)(pa_int & 0xFF);
    p[4] = (uint8_t)((pa_int >> 8) & 0xFF);

    uint8_t tx_buff[16];
    const uint16_t len = construct_msp_command_v2(tx_buff,
                        MSP_PACALIBRATION,
                        p, (uint8_t)(5), 
                        MSP_PACKET_COMMAND);

    msp_tx_send_owner(owner, tx_buff, len);

}

void vtx_msp_set_calibration(uint8_t owner, const uint8_t *payload, uint16_t data_size)
{
    static uint8_t counter = 49;  

    if (!payload || data_size < 3) {
        return; // malformed
    }
    (void)owner;

    uint8_t level = payload[0];
    uint16_t pa_mv = payload[1] + (uint16_t)(payload[2]<<8);;

    if (level && level != g_cfg.power && level <= rf_pa_power_count()) {
      g_cfg.power = level;
      rtc6705_allow_power_writes(true);
      rtc6705_set_power(powerTable[g_cfg.power].rtcPA);
      rtc6705_allow_power_writes(false);
      TRACE_INFO("Calibration power %i\n", level);
    }
    
    if (pa_mv) {
      rf_pa_set_calibration(pa_mv);
      if(!counter--) {
        TRACE_INFO("Calibration mv %i\n", pa_mv);
        counter = 49;
      }
    }

    vtx_msp_push_calibration(owner);
}

void vtx_msp_eeprom_write(uint8_t owner)
{
    uint8_t tx_buff[64];
    const uint16_t len = construct_msp_command_v1(tx_buff, MSP_EEPROM_WRITE, NULL, 0, MSP_OUTBOUND);
    msp_tx_send_owner(owner, tx_buff, len);
}

void vtx_msp_request_config(uint8_t owner)
{
    uint8_t tx_buff[64];
    const uint16_t len = construct_msp_command_v1(tx_buff, MSP_VTX_CONFIG, NULL, 0, MSP_OUTBOUND);
    msp_tx_send_owner(owner, tx_buff, len);
}

bool vtx_msp_handle_msp(uint8_t owner, uint16_t msp_cmd, uint16_t data_size, const uint8_t *payload)
{
    switch (msp_cmd) {
    case MSP_VTX_CONFIG:
        handle_msp_set_vtx_config(owner, payload, data_size);
        break;
    
    case MSP_PACALTABLE:
        vtx_msp_push_calibration_table(owner);
        break;
    
    case MSP_SET_PACALIBRATION:
        vtx_msp_set_calibration(owner, payload, data_size);
        break;

    case MSP_SET_PACALTABLE:
        vtx_msp_set_calibration_table(owner, payload, data_size);
        break;
    
    case MSP_EEPROM_WRITE:
        eeprom_save();
        break;

    case MSP_SET_VTX_CONFIG:
    case MSP_VTXTABLE_BAND:
    case MSP_VTXTABLE_POWERLEVEL:
    default:
        return false;
    }

    const vtx_config_t *vtx_config = vtx_get_config();
    if (!vtx_config->vtx_table_available) {
        TRACE_INFO("Set Table defaults\n");
        vtx_msp_clear_table_and_set_defaults(owner);
        
    }
    return true;
}
#endif
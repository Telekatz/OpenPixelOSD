#pragma once

#define USE_VTX
#define USE_HD
#define USE_USB
//#define USE_RUNCAM

#define SPI2_CS_Pin                             LL_GPIO_PIN_7
#define SPI2_CS_GPIO_Port                       GPIOB
#define SPI2_SCK_Pin                            LL_GPIO_PIN_6
#define SPI2_SCK_GPIO_Port                      GPIOB
#define SPI2_MOSI_Pin                           LL_GPIO_PIN_9
#define SPI2_MOSI_GPIO_Port                     GPIOB

#define ADC_RESERVED_Pin                        LL_GPIO_PIN_14
#define ADC_RESERVED_GPIO_Port                  GPIOB
#define ADC_RESERVED_Channel                    LL_ADC_CHANNEL_5

#define ADC_PA_VDET_Pin                         LL_GPIO_PIN_0
#define ADC_PA_VDET_GPIO_Port                   GPIOB
#define ADC_PA_VDET_Channel                     LL_ADC_CHANNEL_15

#define USER_KEY_Pin                            LL_GPIO_PIN_15
#define USER_KEY_GPIO_Port                      GPIOC
#define LED_STATE_Pin                           LL_GPIO_PIN_13
#define LED_STATE_GPIO_Port                     GPIOC
#define LED2_Pin                                LL_GPIO_PIN_14
#define LED2_GPIO_Port                          GPIOC
#define BOOT_KEY_Pin                            LL_GPIO_PIN_8
#define BOOT_KEY_GPIO_Port                      GPIOB

//Test point
#define TP1_Pin                                 LL_GPIO_PIN_6
#define TP1_GPIO_Port                           GPIOA
#define TP2_Pin                                 LL_GPIO_PIN_1
#define TP2_GPIO_Port                           GPIOB

//Set input gain and total gain to 2 if FMS6141 video filter is used
#define VIDEO_TOTAL_GAIN                        2

// Video input 1 PA3
#define VIDEO1_INPUT_ENABLED                    true
#define VIDEO1_INPUT_GAIN                       2

// Video input 2 PA7
#define VIDEO2_INPUT_ENABLED                    true
#define VIDEO2_INPUT_GAIN                       1


// PA
#define PA_CONTROL_Kp                           0.2
#define PA_CONTROL_Ki                           0.009
#define PA_CONTROL_Kd                           0.3

#define PA_CONTROL_OFFSET_MV                    1600
#define PA_LIMIT                                3000

#define POWER_TABLE     { {0,  {' ', ' ', '0'}, RTC6705_PA_3dBm,  { 5650, 5700, 5750, 5800, 5850, 5900, 5950 },  \
                                                                  {    0,    0,    0,    0,    0,    0,    0 }}, \
                          {1,  {' ', ' ', '1'}, RTC6705_PA_3dBm,  {    1,    1,    1,    1,    1,    1,    1 },  \
                                                                  {    0,    0,    0,    0,    0,    0,    0 }}, \
                          {10, {' ', '1', '0'}, RTC6705_PA_3dBm,  {    1,    1,    1,    1,    1,    1,    1 },  \
                                                                  {    0,    0,    0,    0,    0,    0,    0 }}, \
                          {25, {' ', '2', '5'}, RTC6705_PA_3dBm,  {    1,    1,    1,    1,    1,    1,    1 },  \
                                                                  {    0,    0,    0,    0,    0,    0,    0 }}, \
                          {50, {' ', '5', '0'}, RTC6705_PA_3dBm,  {    1,    1,    1,    1,    1,    1,    1 },  \
                                                                  {    0,    0,    0,    0,    0,    0,    0 }}, \
                        }

                        /* VTX bands table (letter + 8-char name + 8 channel freqs (MHz)).
                        * These are standard bands used in Betaflight and iNav.
                        * You can add custom bands here if needed. */
#define BAND_TABLE      {   /* Band A (Boscam A) */                           \
                            { 'A', { 'B','O','S','C','A','M',' ','A' },       \
                              { 5865,5845,5825,5805,5785,5765,5745,   0 } },  \
                            /* Band B (Boscam B) */                           \
                            { 'B', { 'B','O','S','C','A','M',' ','B' },       \
                              { 5733,5752,5771,5790,5809,5828,5847,5866 } },  \
                            /* Band E */                                      \
                            { 'E', { 'B','A','N','D',' ','E',' ',' ' },       \
                              {    0,   0,   0,   0,   0,   0,   0,   0 } },  \
                            /* Band F (FatShark) */                           \
                            { 'F', { 'F','A','T','S','H','A','R','K' },       \
                              { 5740,5760,5780,5800,5820,5840,5860,   0 } },  \
                            /* Band R (Raceband) */                           \
                            { 'R', { 'R','A','C','E','B','A','N','D' },       \
                              {    0,   0,5732,5769,5806,5843,   0,   0 } },  \
                        }
                        
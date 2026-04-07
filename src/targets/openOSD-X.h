#pragma once

#define USE_VTX
#define USE_HD
#define HSI_ENABLED

#define SPI2_CS_Pin                             LL_GPIO_PIN_0
#define SPI2_CS_GPIO_Port                       GPIOF
#define SPI2_SCK_Pin                            LL_GPIO_PIN_1
#define SPI2_SCK_GPIO_Port                      GPIOF
#define SPI2_MOSI_Pin                           LL_GPIO_PIN_11
#define SPI2_MOSI_GPIO_Port                     GPIOA

#define ADC_RESERVED_Pin                        LL_GPIO_PIN_0
#define ADC_RESERVED_GPIO_Port                  GPIOB
#define ADC_RESERVED_Channel                    LL_ADC_CHANNEL_15

#define ADC_PA_VDET_Pin                         LL_GPIO_PIN_0
#define ADC_PA_VDET_GPIO_Port                   GPIOA
#define ADC_PA_VDET_Channel                     LL_ADC_CHANNEL_1

//Set input gain and total gain to 2 if FMS6141 video filter is used
#define VIDEO_TOTAL_GAIN                        1

// Video input 1 PA3
#define VIDEO1_INPUT_ENABLED                    false
#define VIDEO1_INPUT_GAIN                       1

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
                          {2,  {'R', 'C', 'E'}, RTC6705_PA_3dBm,  {    1,    1,    1,    1,    1,    1,    1 },  \
                                                                  {    0,    0,    0,    0,    0,    0,    0 }}, \
                          {25, {' ', '2', '5'}, RTC6705_PA_3dBm,  {    1,    1,    1,    1,    1,    1,    1 },  \
                                                                  { 1330, 1345, 1400, 1480, 1590, 1670, 1710 }}, \
                          {100,{'1', '0', '0'}, RTC6705_PA_3dBm,  {    1,    1,    1,    1,    1,    1,    1 },  \
                                                                  { 1970, 1980, 2120, 2270, 2430, 2540, 2620 }}, \
                        }


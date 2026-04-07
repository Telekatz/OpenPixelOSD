#pragma once

//#define USE_COLOR
//#define USE_HD
#define USE_GRAPHICS
#define USE_USB


//General
#define ADC_RESERVED_Pin                        LL_GPIO_PIN_1
#define ADC_RESERVED_GPIO_Port                  GPIOB
#define ADC_RESERVED_Channel                    LL_ADC_CHANNEL_12

#define USER_KEY_Pin                            LL_GPIO_PIN_13
#define USER_KEY_GPIO_Port                      GPIOC
#define LED_STATE_Pin                           LL_GPIO_PIN_6
#define LED_STATE_GPIO_Port                     GPIOC
#define BOOT_KEY_Pin                            LL_GPIO_PIN_8
#define BOOT_KEY_GPIO_Port                      GPIOB


//Set input gain and total gain to 2 if FMS6141 video filter is used
#define VIDEO_TOTAL_GAIN                        1

// Video input 1 PA3
#define VIDEO1_INPUT_ENABLED                    false
#define VIDEO1_INPUT_GAIN                       1

// Video input 2 PA7
#define VIDEO2_INPUT_ENABLED                    true
#define VIDEO2_INPUT_GAIN                       1


// PA
#define ADC_PA_VDET_Pin                         LL_GPIO_PIN_11
#define ADC_PA_VDET_GPIO_Port                   GPIOB
#define ADC_PA_VDET_Channel                     LL_ADC_CHANNEL_14

#define PA_CONTROL_Kp                           0.2
#define PA_CONTROL_Ki                           0.009
#define PA_CONTROL_Kd                           0.3

#define PA_CONTROL_OFFSET_MV                    1600
#define PA_LIMIT                                3000


//VTX
#define SPI2_CS_Pin                             LL_GPIO_PIN_12
#define SPI2_CS_GPIO_Port                       GPIOB
#define SPI2_SCK_Pin                            LL_GPIO_PIN_13
#define SPI2_SCK_GPIO_Port                      GPIOB
#define SPI2_MISO_Pin                           LL_GPIO_PIN_14
#define SPI2_MISO_GPIO_Port                     GPIOB
#define SPI2_MOSI_Pin                           LL_GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port                     GPIOB

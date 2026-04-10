/* SPDX-License-Identifier: GPL-2.0-only */
/**
 * Copyright (C) 2025 Vitaliy N <vitaliy.nimych@gmail.com>
 */
#include "main.h"

void gpio_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

#ifdef LED_STATE_Pin
    /**/
    // LED on by default, so that correct flashing and the boot process can be observed.
    LL_GPIO_SetOutputPin(LED_STATE_GPIO_Port, LED_STATE_Pin);

    /**/
    GPIO_InitStruct.Pin = USER_KEY_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(USER_KEY_GPIO_Port, &GPIO_InitStruct);
#endif

#if defined(USE_VTX)
    LL_GPIO_ResetOutputPin(SPI2_CS_GPIO_Port, SPI2_CS_Pin);

    /* Soft SPI for RTC6705 */
    GPIO_InitStruct.Pin = SPI2_CS_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI2_CS_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI2_MOSI_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI2_MOSI_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI2_SCK_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(SPI2_SCK_GPIO_Port, &GPIO_InitStruct);
#endif

#ifdef LED_STATE_Pin
    /**/
    GPIO_InitStruct.Pin = LED_STATE_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_STATE_GPIO_Port, &GPIO_InitStruct);
#endif

#if defined(PA_ON_Pin)
    LL_GPIO_ResetOutputPin(PA_ON_GPIO_Port, PA_ON_Pin);
    GPIO_InitStruct.Pin = PA_ON_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(PA_ON_GPIO_Port, &GPIO_InitStruct);
#endif

#if defined(LED2_Pin)
    LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);
    GPIO_InitStruct.Pin = LED2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);
#endif

#if defined(TP1_Pin)
    LL_GPIO_ResetOutputPin(TP1_GPIO_Port, TP1_Pin);
    GPIO_InitStruct.Pin = TP1_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(TP1_GPIO_Port, &GPIO_InitStruct);
#endif

#if defined(TP2_Pin)
    LL_GPIO_ResetOutputPin(TP2_GPIO_Port, TP2_Pin);
    GPIO_InitStruct.Pin = TP2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(TP2_GPIO_Port, &GPIO_InitStruct);
#endif

#if defined(BOOT_KEY_Pin)
    /**/
    GPIO_InitStruct.Pin = BOOT_KEY_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(BOOT_KEY_GPIO_Port, &GPIO_InitStruct);
#endif

    /* Pull down PA12 to create USB disconnect pulse */
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_12);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);
    LL_mDelay(200);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_12);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11);
}

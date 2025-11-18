/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_CONFIG_HPP
#define __TIMER_CONFIG_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "main.h"
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_tim.h"

/* Defines -------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/
typedef void (*RCC_APBPeriphClockCmd_Typedef)(uint32_t, FunctionalState);
typedef void (*CallbackFunc)(void);

// -----------------------------------------------------------------------------
// Структура для настройки таймера
struct TimerConfig{
    RCC_APBPeriphClockCmd_Typedef APBPeriphClockCmd;    // Функция для включения тактирования
    uint32_t RCC_APBPeriph_TIMx;    // Шина, на которой включено тактирование таймера
    IRQn_Type TIM_IRQn;             // Номер канала прерывания
    uint16_t TimPrescaler;          // Делитель частоты (по умолчанию 720, что соответствует частоте 10 кГц)
    uint32_t TimPeriod;             // Период генерации прерывания в тактах (по умолчанию 1000)
};

#endif /*   __TIMER_CONFIG_HPP   */
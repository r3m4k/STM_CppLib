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
typedef void (*RCC_PeriphClockCmd_Typedef)(uint32_t, FunctionalState);
typedef void (*CallbackFunc)(void);

// -----------------------------------------------------------------------------
namespace STM_CppLib{
    
    namespace STM_Timer{
                
    // Структура для настройки таймера
    struct TimerConfig{
        RCC_PeriphClockCmd_Typedef PeriphClockCmd;      // Функция для включения тактирования
        uint32_t RCC_PeriphClock;                       // Шина, на которой включено тактирование таймера
        uint16_t TimPrescaler;                          // Делитель частоты
        uint32_t TimPeriod;                             // Период генерации прерывания в тактах
    };

    }
}
#endif /*   __TIMER_CONFIG_HPP   */
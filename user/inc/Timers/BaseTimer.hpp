/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BASE_TIMER_HPP
#define __BASE_TIMER_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "main.h"
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_tim.h"

#include "Periphery.hpp"
#include "TimerConfig.hpp"

/* Defines -------------------------------------------------------------------*/
#define     Prescaller_1kHz         72000
#define     Prescaller_10kHz        7200
#define     Prescaller_100kHz       720

/* Typedef -------------------------------------------------------------------*/
typedef void (*RCC_PeriphClockCmd_Typedef)(uint32_t, FunctionalState);

/* Global variables ----------------------------------------------------------*/

// -----------------------------------------------------------------------------

namespace STM_CppLib{

    namespace STM_Timer{

        // Базовый таймер
        class BaseTimer{
        protected:
            TIM_TypeDef* TIMx;      // Структура инициализации таймера. Она используется почти во всех
                                    // функциях таймеров, так что сохраним ей, как поле базового таймера
            
        public:

            BaseTimer(){}
            ~BaseTimer(){
                TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);
                TIM_Cmd(TIMx, DISABLE);
            };

            void Init(TimerConfig* timer_config){
                /* Init structures */
                TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

                /* Enable TIM clock */
                timer_config->PeriphClockCmd(timer_config->RCC_PeriphClock, ENABLE);

                /* Set the timer configuration */
                TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
                TIM_TimeBaseStructure.TIM_Period = timer_config->TimPeriod;
                TIM_TimeBaseStructure.TIM_Prescaler = timer_config->TimPrescaler;
                TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

            }
            
            void Start() {
                TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
                TIM_Cmd(TIMx, ENABLE);
            }
        };
    }
}


#endif /*   __BASE_TIMER_HPP   */
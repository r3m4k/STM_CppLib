/** ***************************************************************************
 * @file    GPTimers.hpp
 * @author  Романовский Роман
 * @brief   Файл для работы с General Purpose Timer.
 * 
 * @details
 * Классы для работы с таймерами общего назначения TIM2-TIM4 микроконтроллера.
 * 
 * Архитектура:
 * - GeneralPurposeTimer наследуется от BaseTimer и содержит статическую
 *   переменную PeriphClockCmd для хранения шины тактирования всех дочерних
 *   классов
 * - Timer2, Timer3, Timer4 наследуются от GeneralPurposeTimer и BaseIRQDevice
 *************************************************************************** */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_HPP
#define __TIMER_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "main.h"
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_tim.h"

#include "Consts.hpp"
#include "Periphery.hpp"
#include "BaseTimer.hpp"
#include "BaseIRQDevice.hpp"
#include "Leds.hpp"

/* Defines -------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
extern __user_pHandler __user_vector_table[];
extern STM_CppLib::Leds leds;
extern uint32_t tick_counter;

// -----------------------------------------------------------------------------

namespace STM_CppLib{
    namespace STM_Timer{
        
    // -----------------------------------------------------------------------------
    class GeneralPurposeTimer: public BaseTimer{
    protected:
        inline static RCC_PeriphClockCmd_Type PeriphClockCmd = RCC_APB1PeriphClockCmd;
    };

    // -----------------------------------------------------------------------------

    class Timer2: public GeneralPurposeTimer, public BaseIRQDevice<Timer2, TIM2_IRQn>{
    public:
        Timer2(){
            irq_device_ptr = this;      // Указатель на себя для отработки прерывания
            TIMx = TIM2;                // Указатель на структуру соответствующего таймера
        }

        void Init(
            uint32_t tim_period = 1000,                             // Количество тиков таймера для генерации прерывания
            uint16_t prescaller = Prescaller_10kHz,                 // Делитель частоты шины 
            TimerConfig* timer_config_ptr = nullptr,                // Указатель на конфигурацию таймера 
            NVIC_InitTypeDef* NVIC_InitStructure_ptr = nullptr      // Указатель на структуру инициализации прерывания 
        ){
            /* *************************************************************************
            *  Данный метод позволяет гибко настроить таймер для работы и отработки прерываний.
            *  Если timer_config_ptr == nullptr, то таймер инициализируется с прописанными в коде параметрами,
            *  чтобы их изменить необходимо самостоятельно заполнить конфигурацию таймера и передать в функцию
            *  в качестве параметра. Аналогично реализована инициализация прерывания.
            ** ********************************************************************** */

            if (!timer_config_ptr){
                TimerConfig timer_config = {
                    .PeriphClockCmd = PeriphClockCmd,
                    .RCC_PeriphClock = RCC_APB1Periph_TIM2,
                    .TimPrescaler = prescaller,
                    .TimPeriod = tim_period
                };

                BaseTimer::Init(&timer_config);
            }
            else{   BaseTimer::Init(timer_config_ptr);   }

            BaseIRQDevice::InitInterrupt(NVIC_InitStructure_ptr);
        }

        void irq_handler(){
            /*  Код для отработки прерывания  */

            // Очистим регистр наличия прерывания от датчика
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);     
        }
    };

    // -----------------------------------------------------------------------------

    class Timer3: public GeneralPurposeTimer, public BaseIRQDevice<Timer3, TIM3_IRQn>{
    public:
        Timer3(){
            irq_device_ptr = this;
            TIMx = TIM3;
        }

        void Init(
            uint32_t tim_period = 1000,
            uint16_t prescaller = Prescaller_10kHz,
            TimerConfig* timer_config_ptr = nullptr,
            NVIC_InitTypeDef* NVIC_InitStructure_ptr = nullptr){
            
            if (!timer_config_ptr){
                TimerConfig timer_config = {
                    .PeriphClockCmd = PeriphClockCmd,
                    .RCC_PeriphClock = RCC_APB1Periph_TIM3,
                    .TimPrescaler = prescaller,
                    .TimPeriod = tim_period
                };

                BaseTimer::Init(&timer_config);
            }
            else{   BaseTimer::Init(timer_config_ptr);   }

            BaseIRQDevice::InitInterrupt(NVIC_InitStructure_ptr);
        }

        void irq_handler(){
            tick_counter += 1;
            leds.ChangeLedStatus(LED8);
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
        }
    };

    // -----------------------------------------------------------------------------

    class Timer4: public GeneralPurposeTimer, public BaseIRQDevice<Timer4, TIM4_IRQn>{
    public:
        Timer4(){
            irq_device_ptr = this;
            TIMx = TIM4;
        }

        void Init(
            uint32_t tim_period = 1000,
            uint16_t prescaller = Prescaller_10kHz,
            TimerConfig* timer_config_ptr = nullptr,
            NVIC_InitTypeDef* NVIC_InitStructure_ptr = nullptr
        ){
            if (!timer_config_ptr){
                TimerConfig timer_config = {
                    .PeriphClockCmd = PeriphClockCmd,
                    .RCC_PeriphClock = RCC_APB1Periph_TIM4,
                    .TimPrescaler = prescaller,
                    .TimPeriod = tim_period
                };

                BaseTimer::Init(&timer_config);
            }
            else{   BaseTimer::Init(timer_config_ptr);   }

            BaseIRQDevice::InitInterrupt(NVIC_InitStructure_ptr);
        }

        void irq_handler(){
            leds.ChangeLedStatus(LED6);
            leds.ChangeLedStatus(LED7);
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
        }
    };

    }
}


#endif /*   __TIMER_HPP   */
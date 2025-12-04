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

// -----------------------------------------------------------------------------

namespace STM_CppLib{
    namespace STM_Timer{
        
        // ---------------------------------------------------------------------
        // Шаблонный класс для работы с General Purpose Timer
        template<TimerTypes timer_type, auto external_irq_handler>
        class GPTimer: public BaseTimer, public BaseIRQDevice<GPTimer<timer_type, external_irq_handler>, 
                                                                TimerDescriptor<timer_type>::IRQn>{
        public:
            GPTimer(){
                this->irq_device_ptr = this;
                this->TIMx = TimerDescriptor<timer_type>::get_TIMx();
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
                        .PeriphClockCmd = RCC_APB1PeriphClockCmd,
                        .RCC_PeriphClock = TimerDescriptor<timer_type>::RCC_Periph,
                        .TimPrescaler = prescaller,
                        .TimPeriod = tim_period
                    };

                    this->InitBaseTimer(&timer_config);
                }
                else{   this->InitBaseTimer(timer_config_ptr);   }

                this->InitInterrupt(NVIC_InitStructure_ptr);
            }

            void irq_handler(){
                // Вызов внешнего обработчика прерывания
                external_irq_handler();

                // Очистим регистр наличия прерывания от датчика
                TIM_ClearITPendingBit(TIMx, TIM_IT_Update);     
            }

        };

        // ---------------------------------------------------------------------

        // Псевдонимы таймеров для удобства использования 
        template<auto external_irq_handler>
        using Timer2 = GPTimer<TimerTypes::Timer2, external_irq_handler>;

        template<auto external_irq_handler>
        using Timer3 = GPTimer<TimerTypes::Timer3, external_irq_handler>;

        template<auto external_irq_handler>
        using Timer4 = GPTimer<TimerTypes::Timer4, external_irq_handler>;

        // ---------------------------------------------------------------------

    } // namespace STM_Timer
} // namespace STM_CppLib


#endif /*   __TIMER_HPP   */
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

#include "Periphery.hpp"
#include "BaseTimer.hpp"
#include "BaseIRQDevice.hpp"
#include "Leds.hpp"

/* Defines -------------------------------------------------------------------*/
#define     Prescaller_1kHz         72000
#define     Prescaller_10kHz        7200
#define     Prescaller_100kHz       720

/* Typedef -------------------------------------------------------------------*/
typedef void (*RCC_PeriphClockCmd_Typedef)(uint32_t, FunctionalState);
typedef void (*CallbackFunc)(void);

typedef void (* __user_pHandler)(void);

/* Global variables ----------------------------------------------------------*/
extern __user_pHandler __user_vector_table[];
extern Leds leds;

// -----------------------------------------------------------------------------

namespace STM_CppLib{
    
    namespace STM_Timer{
        
    // -----------------------------------------------------------------------------
    class GeneralPurposeTimer: public BaseTimer{
    protected:
        // Сделаем переменную для хранения шины тактирования static, чтобы память
        // под неё выделилась один раз вне зависимости от количества наследуемых классов
        inline static RCC_PeriphClockCmd_Typedef PeriphClockCmd = RCC_APB1PeriphClockCmd;
    };

    // -----------------------------------------------------------------------------

    class Timer2: public GeneralPurposeTimer, public BaseIRQDevice<Timer2, TIM2_IRQn>{
    public:
        Timer2(){
            irq_device_ptr = this;
            TIMx = TIM2;
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
                    .RCC_PeriphClock = RCC_APB1Periph_TIM2,
                    .TimPrescaler = prescaller,
                    .TimPeriod = tim_period
                };

                BaseTimer::Init(&timer_config);
            }
            else{   BaseTimer::Init(timer_config_ptr);   }

            BaseIRQDevice::init_interrupt(NVIC_InitStructure_ptr);

        }

        void irq_handler(){
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);     // Очистим регистр наличия прерывания от датчика
        }
    };

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

            BaseIRQDevice::init_interrupt(NVIC_InitStructure_ptr);
        }

        void irq_handler(){
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);     // Очистим регистр наличия прерывания от датчика
        }
    };

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

            BaseIRQDevice::init_interrupt(NVIC_InitStructure_ptr);
        }

        void irq_handler(){
            leds.ChangeLedStatus(LED6);
            leds.ChangeLedStatus(LED7);
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);     // Очистим регистр наличия прерывания от датчика
        }

    };

    }
}


#endif /*   __TIMER_HPP   */
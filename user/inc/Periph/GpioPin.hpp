/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_PIN_HPP
#define __GPIO_PIN_HPP

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"

#include "Consts.hpp"
#include "Exti.hpp"
#include "GpioPort.hpp"
#include "BaseIRQDevice.hpp"

/* Defines -------------------------------------------------------------------*/
#define USE_EXTI        true
#define NOT_USE_EXTI    false

// -----------------------------------------------------------------------------
namespace STM_CppLib{
    namespace STM_GPIO{

    // Базовый класс для работы с пином GPIO
    template <GPIO_Port gpio_port, uint8_t GPIO_PinSource, bool UseEXTI = NOT_USE_EXTI>
    class GPIO_Pin{

    public:
        void Init(GPIOMode_TypeDef GPIO_Mode = GPIO_Mode_OUT,
                  GPIOPuPd_TypeDef GPIO_PuPd = GPIO_PuPd_DOWN,
                  GPIO_InitTypeDef* GPIO_InitStructure_ptr = nullptr
        ){
            
            RCC_AHBPeriphClockCmd(get_RCC_Periph(gpio_port), ENABLE);
            
            if (!GPIO_InitStructure_ptr){                
                GPIO_InitTypeDef GPIO_InitStructure;
        
                /* Configure the GPIOx pin */
                GPIO_InitStructure.GPIO_Pin = (1U << GPIO_PinSource);
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
                GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
                GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                GPIO_Init(get_GPIO_type(gpio_port), &GPIO_InitStructure);
            }
            else{   GPIO_Init(get_GPIO_type(gpio_port), GPIO_InitStructure_ptr);   }
        }

    };

    // Базовый класс для работы с портом GPIO с поддержкой EXTI
    template <GPIO_Port gpio_port, uint8_t GPIO_PinSource>
    class GPIO_Pin<gpio_port, GPIO_PinSource, USE_EXTI>: 

        public STM_EXTI::GPIO_EXTI<
                    STM_EXTI::get_EXTI_PortSource(gpio_port), GPIO_PinSource>, 

        public BaseIRQDevice<GPIO_Pin<gpio_port, GPIO_PinSource, USE_EXTI>, 
                                STM_EXTI::get_EXTI_IRQn(GPIO_PinSource)>
    {

    public:
        GPIO_Pin(){
            this->irq_device_ptr = this;
        }

        void Init(
            GPIOMode_TypeDef GPIO_Mode = GPIO_Mode_IN,
            GPIOPuPd_TypeDef GPIO_PuPd = GPIO_PuPd_DOWN,
            GPIO_InitTypeDef* GPIO_InitStructure_ptr = nullptr,
            EXTI_InitTypeDef* EXTI_InitStructure_ptr = nullptr,
            NVIC_InitTypeDef* NVIC_InitStructure_ptr = nullptr
        ){
            
            RCC_AHBPeriphClockCmd(get_RCC_Periph(gpio_port), ENABLE);
            
            if (!GPIO_InitStructure_ptr){                
                GPIO_InitTypeDef GPIO_InitStructure;
        
                /* Configure the GPIOx pin */
                GPIO_InitStructure.GPIO_Pin = (1U << GPIO_PinSource);
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
                GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
                GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                GPIO_Init(get_GPIO_type(gpio_port), &GPIO_InitStructure);
            }
            else{   GPIO_Init(get_GPIO_type(gpio_port), GPIO_InitStructure_ptr);   }

            this->init_exti(EXTI_InitStructure_ptr);
            this->init_interrupt(NVIC_InitStructure_ptr);
        }
        
        void irq_handler(){
            /*  Код для отработки прерывания  */
            EXTI_ClearFlag(static_cast<uint32_t>(GPIO_PinSource));
        }
    };
    
    } // namespace STM_GPIO
} // namespace STM_CppLib

#endif /*   __GPIO_PIN_HPP   */
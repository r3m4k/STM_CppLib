/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_PORT_HPP
#define __GPIO_PORT_HPP

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"

#include "Consts.hpp"

/* Defines -------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
namespace STM_CppLib{
    namespace STM_GPIO{

        enum class GPIO_Port{
            PortA, PortB, PortC, PortD, PortE, PortF
        };

        constexpr GPIO_TypeDef* get_GPIO_type(GPIO_Port port) noexcept {
            switch(port) {
                case GPIO_Port::PortA: return GPIOA;
                case GPIO_Port::PortB: return GPIOB;
                case GPIO_Port::PortC: return GPIOC;
                case GPIO_Port::PortD: return GPIOD;
                case GPIO_Port::PortE: return GPIOE;
                case GPIO_Port::PortF: return GPIOF;
            }
        }

        constexpr uint32_t get_RCC_Periph(GPIO_Port port) noexcept{
            switch (port) {
                case GPIO_Port::PortA: return RCC_AHBPeriph_GPIOA;
                case GPIO_Port::PortB: return RCC_AHBPeriph_GPIOB;
                case GPIO_Port::PortC: return RCC_AHBPeriph_GPIOC;
                case GPIO_Port::PortD: return RCC_AHBPeriph_GPIOD;
                case GPIO_Port::PortE: return RCC_AHBPeriph_GPIOE;
                case GPIO_Port::PortF: return RCC_AHBPeriph_GPIOF;
            }
        };

    } // namespace STM_GPIO
} // namespace STM_CppLib
#endif /*   __GPIO_PORT_HPP   */
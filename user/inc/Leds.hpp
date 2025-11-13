/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDS_HPP
#define __LEDS_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "main.h"
#include "stm32f3_discovery.h"

/* Defines -------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
// Class comment
class Leds{
    // Байт, в котором будем побитово хранить информацию о состоянии светодиода (вкл/выкл)
    volatile uint8_t leds_status;

public:
    Leds(): leds_status(0x00) {}
    ~Leds(){}

    void Init(){
        STM_EVAL_LEDInit(LED4);
        STM_EVAL_LEDInit(LED3);
        STM_EVAL_LEDInit(LED5);
        STM_EVAL_LEDInit(LED7);
        STM_EVAL_LEDInit(LED9);
        STM_EVAL_LEDInit(LED10);
        STM_EVAL_LEDInit(LED8);
        STM_EVAL_LEDInit(LED6);
    }

    void LedOn(Led_TypeDef Led){
        STM_EVAL_LEDOn(Led);
        leds_status |= (1 << static_cast<uint8_t>(Led));
    }

    void LedOff(Led_TypeDef Led){  
        STM_EVAL_LEDOff(Led);
        leds_status &= ~(1 << static_cast<uint8_t>(Led));
    }

    void ChangeLedStatus(Led_TypeDef Led){
        if (leds_status & (1 << static_cast<uint8_t>(Led))){
            // Светодиод включен - выключим
            LedOff(Led);
        }
        else{
            // Светодиод выключен - включим
            LedOn(Led);
        }
    }

    void LedsOn(){
        LedOn(LED3);
        LedOn(LED4);
        LedOn(LED5);
        LedOn(LED6);
        LedOn(LED7);
        LedOn(LED8);
        LedOn(LED9);
        LedOn(LED10);
    }

    void LedsOff(){
        LedOff(LED3);
        LedOff(LED4);
        LedOff(LED5);
        LedOff(LED6);
        LedOff(LED7);
        LedOff(LED8);
        LedOff(LED9);
        LedOff(LED10);
    }

    void Toggle_Leds(void){
        LedOn(LED3);
        Delay(100);
        LedOff(LED3);

        LedOn(LED4);
        Delay(100);
        LedOff(LED4);

        LedOn(LED6);
        Delay(100);
        LedOff(LED6);

        LedOn(LED8);
        Delay(100);
        LedOff(LED8);

        LedOn(LED10);
        Delay(100);
        LedOff(LED10);

        LedOn(LED9);
        Delay(100);
        LedOff(LED9);

        LedOn(LED7);
        Delay(100);
        LedOff(LED7);

        LedOn(LED5);
        Delay(100);
        LedOff(LED5);
    }
};

#endif /*   __LEDS_HPP   */
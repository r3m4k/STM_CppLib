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

/* Defines -------------------------------------------------------------------*/
#define     Prescaller_1kHz         72000
#define     Prescaller_10kHz        7200
#define     Prescaller_100kHz       720

/* Typedef -------------------------------------------------------------------*/
typedef void (*RCC_APBPeriphClockCmd_Typedef)(uint32_t, FunctionalState);
typedef void (*CallbackFunc)(void);

typedef void (* __user_pHandler)(void);

/* Global variables ----------------------------------------------------------*/
extern __user_pHandler __user_vector_table[];

// -----------------------------------------------------------------------------

class GeneralPurposeTimer: public BaseTimer{
protected:
    // Сделаем переменную для хранения шины тактирования static, чтобы память
    // под неё выделилась один раз вне зависимости от количества наследуемых классов
    inline static RCC_APBPeriphClockCmd_Typedef APBPeriphClockCmd = RCC_APB1PeriphClockCmd;
public:
    GeneralPurposeTimer(){}
};

// -----------------------------------------------------------------------------

class Timer2: public GeneralPurposeTimer{
public:
    Timer2(){
        TIMx = TIM2;
        RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM2;
        TIM_IRQn = TIM2_IRQn;
    }
};

class Timer3: public GeneralPurposeTimer{
public:
    Timer3(){
        TIMx = TIM3;
        RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM3;
        TIM_IRQn = TIM3_IRQn;
    }
};

class Timer4: public GeneralPurposeTimer{
    inline static Timer4* instance;
    CallbackFunc callback_func;     // Функция для отработки прерывания

public:
    Timer4(){
        instance = this;

        APBPeriphClockCmd = RCC_APB1PeriphClockCmd;
        TIMx = TIM4;
        RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM4;
        TIM_IRQn = TIM4_IRQn;

    }

    void Init(){
        __user_vector_table[PeriphIRQnBase + TIM_IRQn] = static_irq_handler;
        BaseTimer::Init();
    }

    static void static_irq_handler(){
        instance->CallBack();
    }

    void CallBack(){
        if (callback_func){
            callback_func();
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);     // Очистим регистр наличия прерывания от датчика
        }
    }

};

#endif /*   __TIMER_HPP   */
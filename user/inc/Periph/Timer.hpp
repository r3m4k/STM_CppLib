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
// Базовый таймер
class BaseTimer{
protected:
    RCC_APBPeriphClockCmd_Typedef APBPeriphClockCmd;    // Функция для включения тактирования
    TIM_TypeDef* TIMx;              // Структура инициализации таймера
    uint32_t RCC_APBPeriph_TIMx;    // Шина, на которой включено тактирование таймера
    IRQn_Type TIM_IRQn;             // Номер канала прерывания
    
public:
    uint16_t TimPrescaler;          // Делитель частоты (по умолчанию 720, что соответствует частоте 10 кГц)
    uint32_t TimPeriod;             // Период генерации прерывания в тактах (по умолчанию 1000)
    CallbackFunc callback_func;     // Функция для отработки прерывания

    BaseTimer(): 
        TimPrescaler(720),          // При таком предделителе таймера получается один тик таймера на 10 мкс
        TimPeriod(1000),            // Количество тиков таймера с частотой 10 кГц перед вызовом прерывания --> 10 мс период
        callback_func(nullptr) {}

    ~BaseTimer(){
        TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);
        TIM_Cmd(TIMx, DISABLE);
    };

    void Init(){
        /* Init structures */
        NVIC_InitTypeDef NVIC_InitStructure;
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

        /* Enable TIM clock */
        APBPeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);

        /* Enable the Timer Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = TIM_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        /* Set the timer configuration */
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        TIM_TimeBaseStructure.TIM_Period = TimPeriod;
        TIM_TimeBaseStructure.TIM_Prescaler = TimPrescaler - 1;
        TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

    }
    
    void Start() {
        TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
        TIM_Cmd(TIMx, ENABLE);
    }
    void CallBack(){
        if (callback_func){
            callback_func();
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);     // Очистим регистр наличия прерывания от датчика
        }
    }
};

// -----------------------------------------------------------------------------

class GeneralPurposeTimer: public BaseTimer{
public:
    GeneralPurposeTimer(){
        APBPeriphClockCmd = RCC_APB1PeriphClockCmd;
    }
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
public:
    Timer4(){
        APBPeriphClockCmd = RCC_APB1PeriphClockCmd;
        TIMx = TIM4;
        RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM4;
        TIM_IRQn = TIM4_IRQn;
    }
};

#endif /*   __TIMER_HPP   */
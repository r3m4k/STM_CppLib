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
typedef void (*RCC_APBPeriphClockCmd_Typedef)(uint32_t, FunctionalState);
typedef void (*CallbackFunc)(void);

/* Global variables ----------------------------------------------------------*/

// -----------------------------------------------------------------------------
// Базовый таймер
class BaseTimer{
protected:
    TimerConfig* timer_config;      // Указатель на структуру настройки таймера
    TIM_TypeDef* TIMx;              // Структура инициализации таймера
    
public:

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

};

#endif /*   __BASE_TIMER_HPP   */
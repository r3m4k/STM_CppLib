/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __L3GD20_HPP
#define __L3GD20_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f30x.h"
#include "stm32f3_discovery_l3gd20.h"
#include "TriaxialData.hpp"

/* Defines -------------------------------------------------------------------*/
#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */
	 
// -----------------------------------------------------------------------------
// Класс для работы с гироскопом L3GD20
class L3GD20{
    float gyro_multiplier;

public:
    TriaxialData gyro_data;

    void Init(){
        L3GD20_InitTypeDef InitStruct;
        L3GD20_FilterConfigTypeDef FilterStruct;

        InitStruct.Power_Mode = L3GD20_MODE_ACTIVE;                     /* Power-down/Sleep/Normal Mode */
        InitStruct.Output_DataRate = L3GD20_OUTPUT_DATARATE_2;          /* OUT data rate */
        InitStruct.Axes_Enable = L3GD20_AXES_ENABLE;                    /* Axes enable */
        InitStruct.Band_Width = L3GD20_BANDWIDTH_2;                     /* Bandwidth selection */
        InitStruct.BlockData_Update = L3GD20_BlockDataUpdate_Continous; /* Block Data Update */
        InitStruct.Endianness = L3GD20_BLE_LSB;                         /* Endian Data selection */
        InitStruct.Full_Scale = L3GD20_FULLSCALE_500;                   /* Full Scale selection */

        L3GD20_Init(&InitStruct);

        /* High Pass Filter Configuration Functions */
        FilterStruct.HighPassFilter_Mode_Selection = L3GD20_HPM_NORMAL_MODE; /* Internal filter mode */
        FilterStruct.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_5;       /* High pass filter cut-off frequency */

        L3GD20_FilterConfig(&FilterStruct);

        // Зададим множитель для гироскопа в соответствии с документацией
        // (см. документацию для L3GD20, таблица 3 "Mechanical characteristics", стр. 9)
        switch (InitStruct.Full_Scale)
        {
        case L3GD20_FULLSCALE_250:
            gyro_multiplier = 8.75;
            break;

        case L3GD20_FULLSCALE_500:
            gyro_multiplier = 17.5;
            break;    

        case L3GD20_FULLSCALE_2000:
            gyro_multiplier = 70.0;
            break; 
        }
    }

    void ReadData(){
        uint8_t high_bit = 0;
        uint8_t low_bit = 0;
        
        // x_coord
        L3GD20_Read(&high_bit, L3GD20_OUT_X_H_ADDR, 1);
        L3GD20_Read(&low_bit,  L3GD20_OUT_X_L_ADDR, 1);
        gyro_data.x_coord = static_cast<float>(static_cast<int16_t>(high_bit << 8) + low_bit) / gyro_multiplier;
        
        // y_coord
        L3GD20_Read(&high_bit, L3GD20_OUT_Y_H_ADDR, 1);
        L3GD20_Read(&low_bit,  L3GD20_OUT_Y_L_ADDR, 1);
        gyro_data.y_coord = static_cast<float>(static_cast<int16_t>(high_bit << 8) + low_bit) / gyro_multiplier;
        
        // z_coord
        L3GD20_Read(&high_bit, L3GD20_OUT_Z_H_ADDR, 1);
        L3GD20_Read(&low_bit,  L3GD20_OUT_Z_L_ADDR, 1);
        gyro_data.z_coord = static_cast<float>(static_cast<int16_t>(high_bit << 8) + low_bit) / gyro_multiplier;
    }

};

#endif /*   __L3GD20_HPP   */
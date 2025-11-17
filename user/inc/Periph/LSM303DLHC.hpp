/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LSM303DLHC_HPP
#define __LSM303DLHC_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f30x.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "TriaxialData.hpp"

/* Defines -------------------------------------------------------------------*/
#define USE_MAGNETIC_SENSOR
// #define USE_TEMPERATURE_SENSOR

// -----------------------------------------------------------------------------
// Класс для работы с LSM303DLHC, совмещающий в себе акселерометр, магнетометр и температурный датчик 
class LSM303DLHC{
    float LSM_Acc_Sensitivity;

public:
    TriaxialData acc_data;

#ifdef USE_MAGNETIC_SENSOR
    TriaxialData mag_data;
#endif /*   USE_MAGNETIC_SENSOR   */

#ifdef USE_TEMPERATURE_SENSOR
    float temperature;
#endif /*    USE_TEMPERATURE_SENSOR   */

    // ------------------------------
    // Инициализация датчиков
    // ------------------------------

    void Init(){
        AccInit();
#ifdef USE_MAGNETIC_SENSOR
        MagInit();
#endif /*   USE_MAGNETIC_SENSOR   */
    }

    void AccInit(){
        LSM303DLHCAcc_InitTypeDef AInitStruct;
        LSM303DLHCAcc_FilterConfigTypeDef FInitStructure;

        /* Fill the accelerometer structure */
        AInitStruct.Power_Mode = LSM303DLHC_NORMAL_MODE;                    // NORMAL or LOWPOWER MODE (CTRL_REG1 ODR[3])
        AInitStruct.AccOutput_DataRate = LSM303DLHC_ODR_400_HZ;             // output data rate				(CTRL_REG1) //400Hz - less zero values
        AInitStruct.Axes_Enable = LSM303DLHC_AXES_ENABLE;                   // enable x, y and z axes	(CTRL_REG1)
        AInitStruct.AccFull_Scale = LSM303DLHC_FULLSCALE_16G;               // full scale (CTRL_REG4)
        AInitStruct.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;    // Block data update. Default value: 0; (0: continuous update, 1: output registers not updated until MSB and LSB have been read (CTRL_REG4)
        AInitStruct.Endianness = LSM303DLHC_BLE_LSB;                        // Big/little endian data selection. Default value 0.(0: data LSB @ lower address, 1: data MSB @ lower address) AInitStruct.High_Resolution=LSM303DLHC_HR_ENABLE; (CTRL_REG4)
        AInitStruct.High_Resolution = LSM303DLHC_HR_ENABLE;
        /* Configure the accelerometer main parameters */
        LSM303DLHC_AccInit(&AInitStruct);

        /* Fill the accelerometer LPF structure ; CTRL_REG2 register*/
        /* mode, cutoff frquency, Filter status, Click, AOI1 and AOI2 */

        FInitStructure.HighPassFilter_Mode_Selection = LSM303DLHC_HPM_NORMAL_MODE;      //??? rejim filtra verhnih chastot 00 Normal mode (reset reading HP_RESET_FILTER) 01 Reference signal for filtering 10 Normal mode 11 Autoreset on interrupt event
        FInitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_32;           // vybor chastoty sreza (8, 16, 32, 64)
        FInitStructure.HighPassFilter_FDS = LSM303DLHC_HIGHPASSFILTER_ENABLE;           // LSM303DLHC_HIGHPASSFILTER_DISABLE;
        FInitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_ENABLE;
        FInitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_ENABLE;

        /* Configure the accelerometer LPF main parameters */
        LSM303DLHC_AccFilterConfig(&FInitStructure);
    }

    void MagInit(){
        LSM303DLHCMag_InitTypeDef InitStruct;

        InitStruct.MagFull_Scale = LSM303DLHC_FS_2_5_GA;                /*!< Full scale = �2.5 Gauss */
        InitStruct.MagOutput_DataRate = LSM303DLHC_ODR_220_HZ;          /*!< Output Data Rate = 220 Hz */
        InitStruct.Working_Mode = LSM303DLHC_CONTINUOS_CONVERSION;      /*!< Continuous-Conversion Mode */

#ifdef USE_TEMPERATURE_SENSOR
        InitStruct.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_ENABLE;   /*!< Temp sensor Enable */
#else
        InitStruct.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;  /*!< Temp sensor Disable */
#endif

        LSM303DLHC_MagInit(&InitStruct);
    }

    // ------------------------------
    // Чтение данных датчиков
    // ------------------------------

    void ReadData(){
        ReadAcc();
        
#ifdef USE_MAGNETIC_SENSOR
        ReadMag();
#endif /*   USE_MAGNETIC_SENSOR   */

#ifdef USE_TEMPERATURE_SENSOR
        ReadTemp();
#endif /*   USE_TEMPERATURE_SENSOR   */
    }

    void ReadAcc(){
        int16_t pnRawData[3];
        uint8_t ctrlx[2];
        uint8_t buffer[6] = {0};
        uint8_t cDivider;
        float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;

        /* Read the register content */
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx, 2);
        // LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);
        
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 1);
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_H_A, buffer + 1, 1);
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_Y_L_A, buffer + 2, 1);
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_Y_H_A, buffer + 3, 1);
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_Z_L_A, buffer + 4, 1);
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_Z_H_A, buffer + 5, 1);

        if (ctrlx[1] & 0x40)
            cDivider = 64;
        else
            cDivider = 16;

        /* check in the control register4 the data alignment*/
        if (!(ctrlx[0] & 0x40) || (ctrlx[1] & 0x40)) /* Little Endian Mode or FIFO mode */
        {
            for (uint8_t i = 0; i < 3; i++)
            {
                pnRawData[i] = ((int16_t)((uint16_t)buffer[2 * i + 1] << 8) + buffer[2 * i]) / cDivider; //       pfData[i]=(float)((int16_t)((((int16_t)buffer[2*i]) << 8) + buffer[2*i+1]));
            }
        }
        else /* Big Endian Mode */
        {
            for (uint8_t i = 0; i < 3; i++)
                pnRawData[i] = ((int16_t)((uint16_t)buffer[2 * i] << 8) + buffer[2 * i + 1]) / cDivider;
        }
        /* Read the register content */
        LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx, 2);

        if (ctrlx[1] & 0x40)
        {
            /* FIFO mode */
            LSM_Acc_Sensitivity = 0.25;
        }
        else
        {
            /* normal mode */
            /* switch the sensitivity value set in the CRTL4*/
            switch (ctrlx[0] & 0x30)
            {
            case LSM303DLHC_FULLSCALE_2G:
                LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
                break;
            case LSM303DLHC_FULLSCALE_4G:
                LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_4g;
                break;
            case LSM303DLHC_FULLSCALE_8G:
                LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_8g;
                break;
            case LSM303DLHC_FULLSCALE_16G:
                LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_16g;
                break;
            }
        }

        /* Obtain the mg value for the three axis */
        for (uint8_t i = 0; i < 3; i++){
            acc_data[i] = static_cast<float>(pnRawData[i]) / LSM_Acc_Sensitivity;
        }
    }

    void ReadMag(){
        uint8_t CTRLB = 0;
        uint16_t Magn_Sensitivity_XY, Magn_Sensitivity_Z;
        uint8_t high_bit, low_bit;

        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_CRB_REG_M, &CTRLB, 1);

        /* Switch the sensitivity set in the CRTLB*/
        switch (CTRLB & 0xE0)
        {
        case LSM303DLHC_FS_1_3_GA:
            Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_3Ga;
            Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_3Ga;
            break;
        case LSM303DLHC_FS_1_9_GA:
            Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_1_9Ga;
            Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_1_9Ga;
            break;
        case LSM303DLHC_FS_2_5_GA:
            Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_2_5Ga;
            Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_2_5Ga;
            break;
        case LSM303DLHC_FS_4_0_GA:
            Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4Ga;
            Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4Ga;
            break;
        case LSM303DLHC_FS_4_7_GA:
            Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_4_7Ga;
            Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_4_7Ga;
            break;
        case LSM303DLHC_FS_5_6_GA:
            Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_5_6Ga;
            Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_5_6Ga;
            break;
        case LSM303DLHC_FS_8_1_GA:
            Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_8_1Ga;
            Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_8_1Ga;
            break;
        }

        // x_coord
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_H_M, &high_bit, 1);
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_X_L_M, &low_bit,  1);
        mag_data.x_coord = static_cast<float>(static_cast<int16_t>(high_bit << 8) + low_bit) / Magn_Sensitivity_XY;
        
        // y_coord
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_H_M, &high_bit, 1);
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Y_L_M, &low_bit,  1);
        mag_data.y_coord = static_cast<float>(static_cast<int16_t>(high_bit << 8) + low_bit) / Magn_Sensitivity_XY;

        // z_coord
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_H_M, &high_bit, 1);
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_OUT_Z_L_M, &low_bit,  1);
        mag_data.z_coord = static_cast<float>(static_cast<int16_t>(high_bit << 8) + low_bit) / Magn_Sensitivity_Z;

    }

#ifdef USE_TEMPERATURE_SENSOR
    void ReadTemp(){
        uint8_t high_bit, low_bit;

        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_TEMP_OUT_H_M, &high_bit, 1);
        LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_TEMP_OUT_L_M, &low_bit,  1);

        temperature = static_cast<float>(static_cast<int16_t>(static_cast<uint16_t>(high_bit << 8) + low_bit) >> 4);
    }
#endif /*   USE_TEMPERATURE_SENSOR   */

};

#endif /*   __LSM303DLHC_HPP   */
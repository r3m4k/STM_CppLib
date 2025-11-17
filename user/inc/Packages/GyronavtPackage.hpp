/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GYRONAVT_PACKAGE_HPP
#define __GYRONAVT_PACKAGE_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "BasePackage.hpp"
#include "TriaxialData.hpp"
#include "L3GD20.hpp"
#include "LSM303DLHC.hpp"

/* Defines -------------------------------------------------------------------*/
#define Header_FirstBit     0xAA
#define Header_SecondBit    0xBB
#define PackageType         0xCC

#define AccCoefficient      100
#define GyroCoefficient     100
#define MagCoefficient      100

#define BarConst            0xBA
#define DefaultStatus       0xDD

#define GyronavtPackageLen  33
#define GyronavtHeaderLen   4

#define TimeBaseIndex       4
#define StatusIndex         8
#define AccBaseIndex        12
#define GyroBaseIndex       18
#define MagBaseIndex        24
#define BarBaseIndex        30
#define ControlSumIndex     32

/* Global variables ----------------------------------------------------------*/
extern L3GD20       gyro_sensor;
extern LSM303DLHC   acc_sensor;
extern uint32_t     tick_counter;

// -----------------------------------------------------------------------------

enum class PackageStatus{
    default_status = DefaultStatus,
};

// -----------------------------------------------------------------------------
// Посылка, согласно протоколу Гиронавт
class GyronavtPackage: public BasePackage{
    uint8_t data_arr[GyronavtPackageLen] = {0};

public:
    uint32_t time;
	uint8_t status;
    TriaxialData acc_data, gyro_data, mag_data;
    float bar;

    GyronavtPackage(): 
        time(0), 
        status(static_cast<uint8_t>(PackageStatus::default_status)), 
        bar(BarConst){
            len = GyronavtPackageLen;
            data_ptr = data_arr;

            data_arr[0] = Header_FirstBit;
            data_arr[1] = Header_SecondBit;
            data_arr[2] = GyronavtPackageLen;
            data_arr[3] = PackageType;
    }

    uint8_t CountControlSum() override {
        return 0xFF;
    }
    
    void UpdateData() override {
        time = tick_counter;
        acc_data = acc_sensor.acc_data;
        gyro_data = gyro_sensor.gyro_data;
        mag_data = acc_sensor.mag_data;
    }

    void DataPackaging() override {
        
        // -------------------------------------
        // Заполним данные времени
        data_arr[TimeBaseIndex + 0] = static_cast<uint8_t>(time >> 0);      // 1 разряд
        data_arr[TimeBaseIndex + 1] = static_cast<uint8_t>(time >> 8);      // 2 разряд
        data_arr[TimeBaseIndex + 2] = static_cast<uint8_t>(time >> 16);     // 3 разряд
        data_arr[TimeBaseIndex + 3] = static_cast<uint8_t>(time >> 24);     // 4 разряд
        
        // -------------------------------------
        // Заполним статус
        data_arr[StatusIndex] = status;

        // -------------------------------------
        // Заполним данные с датчиков
        acc_data *= AccCoefficient;
        gyro_data *= GyroCoefficient;
        mag_data *= MagCoefficient;

        // -------------------------------------

        data_arr[AccBaseIndex + 0] = static_cast<uint8_t>(static_cast<int16_t>(acc_data.x_coord));
        data_arr[AccBaseIndex + 1] = static_cast<uint8_t>(static_cast<int16_t>(acc_data.x_coord) >> 8);

        data_arr[AccBaseIndex + 2] = static_cast<uint8_t>(static_cast<int16_t>(acc_data.y_coord));
        data_arr[AccBaseIndex + 3] = static_cast<uint8_t>(static_cast<int16_t>(acc_data.y_coord) >> 8);

        data_arr[AccBaseIndex + 4] = static_cast<uint8_t>(static_cast<int16_t>(acc_data.z_coord));
        data_arr[AccBaseIndex + 5] = static_cast<uint8_t>(static_cast<int16_t>(acc_data.z_coord) >> 8);

        // -------------------------------------

        data_arr[GyroBaseIndex + 0] = static_cast<uint8_t>(static_cast<int16_t>(gyro_data.x_coord));
        data_arr[GyroBaseIndex + 1] = static_cast<uint8_t>(static_cast<int16_t>(gyro_data.x_coord) >> 8);

        data_arr[GyroBaseIndex + 2] = static_cast<uint8_t>(static_cast<int16_t>(gyro_data.y_coord));
        data_arr[GyroBaseIndex + 3] = static_cast<uint8_t>(static_cast<int16_t>(gyro_data.y_coord) >> 8);

        data_arr[GyroBaseIndex + 4] = static_cast<uint8_t>(static_cast<int16_t>(gyro_data.z_coord));
        data_arr[GyroBaseIndex + 5] = static_cast<uint8_t>(static_cast<int16_t>(gyro_data.z_coord) >> 8);

        // -------------------------------------

        data_arr[MagBaseIndex + 0] = static_cast<uint8_t>(static_cast<int16_t>(mag_data.x_coord));
        data_arr[MagBaseIndex + 1] = static_cast<uint8_t>(static_cast<int16_t>(mag_data.x_coord) >> 8);

        data_arr[MagBaseIndex + 2] = static_cast<uint8_t>(static_cast<int16_t>(mag_data.y_coord));
        data_arr[MagBaseIndex + 3] = static_cast<uint8_t>(static_cast<int16_t>(mag_data.y_coord) >> 8);

        data_arr[MagBaseIndex + 4] = static_cast<uint8_t>(static_cast<int16_t>(mag_data.z_coord));
        data_arr[MagBaseIndex + 5] = static_cast<uint8_t>(static_cast<int16_t>(mag_data.z_coord) >> 8);

        // -------------------------------------
        // Заполним данные барометра
        data_arr[BarBaseIndex + 0] = static_cast<uint8_t>(static_cast<int16_t>(bar));
        data_arr[BarBaseIndex + 1] = static_cast<uint8_t>(static_cast<int16_t>(bar) >> 8);

        // -------------------------------------
        // Заполним контрольную сумму
        data_arr[ControlSumIndex] = CountControlSum();        
    }
};


#endif /*   __GYRONAVT_PACKAGE_HPPs   */
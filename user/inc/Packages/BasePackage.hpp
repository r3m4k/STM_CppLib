/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BASE_PACKAGE_HPP
#define __BASE_PACKAGE_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

// -----------------------------------------------------------------------------
// Класс для описания базового пакета информации
class BasePackage{
    // virtual void DataPackaging() = 0;
    // virtual uint8_t CountControlSum() = 0;

public:
    uint8_t *data_ptr;
    uint8_t len;

    BasePackage(): data_ptr(nullptr), len(0) {}
    ~BasePackage(){};

    // virtual void UpdateData() = 0;

};

#endif /*   __BASE_PACKAGE_HPP   */
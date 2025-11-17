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
public:
    uint8_t *data_ptr;
    uint8_t len;

    BasePackage(): data_ptr(nullptr), len(0) {}
    ~BasePackage(){};

    virtual uint8_t CountControlSum() = 0;
    virtual void UpdateData() = 0;
    virtual void DataPackaging() = 0;
};

#endif /*   __BASE_PACKAGE_HPP   */
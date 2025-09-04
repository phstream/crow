/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc8.c
 * @author    phstream
 * @copyright 2024, phstream
 * @license   MIT
 * @date      19 maj 2025
 ****************************************************************************/
#include <string.h>
#include "crc8.h" 

/* Version constants */
#define CRC8_VERSION_MAJOR (1)   /* Step if interface or function changes */
#define CRC8_VERSION_MINOR (1)   /* Step if bug fix or refactoring */
#define CRC8_VERSION_PATCH (0)   /* Step for minor fixes */

int
crc8_version() 
{
    return (CRC8_VERSION_MAJOR << 16) | (CRC8_VERSION_MINOR << 8) | CRC8_VERSION_PATCH;
}

/* Dummy implentaion keeping api same. */
uint8_t 
crc8_initialize() { return 0; }

/* Dummy implentaion keeping api same. */
uint8_t 
crc8_finalize(uint8_t crc_in) { return crc_in; }

uint8_t
crc8_update(const uint8_t* p_data, size_t length, uint8_t crc_in) 
{ 
    for (size_t i = 0; i < length; ++i)
    {
        uint8_t byte = p_data[i];
        for (int b = 0; b < 8; ++b) 
        {
            uint8_t sum = (crc_in ^ byte) & 0x01;
            crc_in >>= 1;
            if (sum) crc_in ^= 0x8C;
            byte >>= 1;
        }
    }
    return crc_in;
}

uint8_t 
crc8_calc(const uint8_t* p_data, size_t length)
{
    return crc8_finalize(crc8_update(p_data, length, crc8_initialize()));
}
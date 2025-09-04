/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc16.c
 * @author    phstream
 * @copyright 2024, phstream
 * @license   MIT
 * @date      6 Jul 2025
 ****************************************************************************/
#include <string.h>
#include "crc16.h" 

/* Version constants */
#define CRC16_VERSION_MAJOR (1)   /* Step if interface or function changes */
#define CRC16_VERSION_MINOR (1)   /* Step if bug fix or refactoring */
#define CRC16_VERSION_PATCH (0) /* Step for minor fixes */

/* CRC16 algorithm selection defines */
#define CRC16_POLY   (0x1021u)
#define CRC16_INIT   (0xffffu)
#define CRC16_XOROUT (0x0000u)
//#define CRC16_REFIN
//#define CRC16_REFOUT

#ifdef CRC16_REFIN
/** Reflects (mirror) input byte */
static inline uint8_t reflect8(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}
#endif
#ifdef CRC16_REFOUT
/** Reflects (mirror) input word */
static inline uint16_t reflect16(uint16_t w) {
    w = (w & 0xFF00) >> 8 | (w & 0x00FF) << 8;
    w = (w & 0xF0F0) >> 4 | (w & 0x0F0F) << 4;
    w = (w & 0xCCCC) >> 2 | (w & 0x3333) << 2;
    w = (w & 0xAAAA) >> 1 | (w & 0x5555) << 1;
    return w;
}
#endif

int
crc16_version() 
{
    return (CRC16_VERSION_MAJOR << 16) | (CRC16_VERSION_MINOR << 8) | CRC16_VERSION_PATCH;
}

uint16_t 
crc16_initialize() { return CRC16_INIT; }

uint16_t 
crc16_finalize(uint16_t crc_in) { return crc_in ^ CRC16_XOROUT; }

uint16_t
crc16_update(const uint8_t* p_data, size_t length, uint16_t crc_in) 
{ 
    for (size_t i = 0; i < length; ++i) {
        crc_in ^= 
#ifdef CRC16_REFIN
            ((uint16_t)reflect8(p_data[i])) << 8;
#else 
            ((uint16_t)p_data[i]) << 8;
#endif
        for (int b = 0; b < 8; ++b) {
            if (crc_in & 0x8000)
                crc_in = (crc_in << 1) ^ CRC16_POLY;
            else
                crc_in <<= 1;
        }
    }
#ifdef CRC16_REFOUT
    return reflect16(crc_in);
#else
    return crc_in;
#endif
}

uint16_t 
crc16_calc(const uint8_t* p_data, size_t length)
{
    return crc16_finalize(crc16_update(p_data, length, crc16_initialize()));
}
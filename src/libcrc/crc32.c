/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc32.c
 * @author    phstream
 * @copyright 2024, phstream
 * @license   MIT
 * @date      12 Jul 2024
 ****************************************************************************/
#include <string.h>
#include "crc32.h" 

/* Version constants */
#define CRC32_VERSION_MAJOR (1)   /* Step if interface or function changes */
#define CRC32_VERSION_MINOR (1)   /* Step if bug fix or refactoring */
#define CRC32_VERSION_PATCH (0)   /* Step for minor fixes */

/* CRC32 algorithm selection defines */
#define CRC32_POLY    (0x04C11DB7ul)
#define CRC32_INIT    (0xfffffffful)
#define CRC32_XOROUT  (0xfffffffful)
#define CRC32_REFIN
#define CRC32_REFOUT

/** Holds poly or recalculated reflected (mirrored) poly */
static uint32_t crc32_poly = CRC32_POLY;

#if defined(CRC32_REFIN) || defined(CRC32_REFOUT)
/** Reflects (mirror) input word */
static inline uint32_t reflect32(uint32_t w) {
    w = ((w & 0x55555555ul) << 1) | ((w & 0xAAAAAAAAul) >> 1);
    w = ((w & 0x33333333ul) << 2) | ((w & 0xCCCCCCCCul) >> 2);
    w = ((w & 0x0F0F0F0Ful) << 4) | ((w & 0xF0F0F0F0ul) >> 4);
    w = ((w & 0x00FF00FFul) << 8) | ((w & 0xFF00FF00ul) >> 8);
    return (w << 16) | (w >> 16);
}
#endif

int
crc32_version() 
{
    return (CRC32_VERSION_MAJOR << 16) | (CRC32_VERSION_MINOR << 8) | CRC32_VERSION_PATCH;
}

uint32_t 
crc32_initialize() {
    /* Reflect POLY instead of input bytes for performance (if REFIN defined) */ 
#ifdef CRC32_REFIN
    /* Hope optimastion makes these constants */
    if (crc32_poly == CRC32_POLY) crc32_poly = reflect32(CRC32_POLY);
    return reflect32(CRC32_INIT);
#else
    return CRC32_INIT;
#endif
}

uint32_t 
crc32_finalize(uint32_t crc_in)
{
#if (defined(CRC32_REFIN) && !defined(CRC32_REFOUT)) || (!defined(CRC32_REFIN) && defined(CRC32_REFOUT))
    /* Only needs reflection if CRC32_REFIN and REFOUT differs */
    crc_in = reflect32(crc_in);    
#endif
    return (crc_in ^ CRC32_XOROUT);
}

uint32_t
crc32_update(const uint8_t* p_data, size_t length, uint32_t crc_in)
{
    for(size_t i = 0; i < length; ++i) {
#ifdef CRC32_REFIN /* Do the calculation mirrored instead of reflecting all indata */
        crc_in ^= p_data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc_in & 1) crc_in = (crc_in >> 1) ^ crc32_poly;
            else            crc_in = (crc_in >> 1);
        }
#else
        crc_in ^= (uint32_t)(p_data[i]) << 24;
        for (int b = 0; b < 8; ++b) {
            if (crc_in & 0x80000000) crc_in = (crc_in << 1) ^ crc32_poly;
            else                     crc_in = (crc_in << 1);
        }
#endif
    }
    return crc_in;
}

uint32_t
crc32_calc(const uint8_t* p_data, size_t length)
{
    return crc32_finalize(crc32_update(p_data, length, crc32_initialize()));
}

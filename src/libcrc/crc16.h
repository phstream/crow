/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc16.h
 * @author    phstream
 * @copyright 2024, phstream
 * @license   MIT
 * @date      6 Jul 2025
 ****************************************************************************/
/** @defgroup  CRC CRC Calculation
 * @brief     Cyclic Redundancy Checksum 8bit calculation
 * @details
 *
 * @pre       common/lib_defs.h
 * @bug
 * @warning
 * @ingroup   CROW
 ****************************************************************************/
#ifndef CRC16_H_ /* Guard against multiple inclusion */
#define	CRC16_H_

#include "../common/lib_defs.h"

#ifdef __cplusplus    /* C++ compability */
#  include <cstdint>
#  include <cstddef>
extern "C" {
#else
#  include <stdint.h>
#  include <stddef.h>
#endif

/**
 * @ingroup CRC
 * @brief CRC16 version as integer number.
 * @return Version as major# * 1000000 + minor# * 10000 + build#.
 */
LIB_EXPORT int
crc16_version();

/**
 * @ingroup CRC
 * @brief Initializes the CRC16 calculation.
 * @return Initial CRC16 sum.
 */
LIB_EXPORT uint16_t
crc16_initialize();

/**
 * @ingroup CRC
 * @brief Finalizes the CRC16 calculation after processing the data.
 * @param crc_in Current CRC16 sum to be finalized.
 * @return Final CRC16 sum.
 */
LIB_EXPORT uint16_t 
crc16_finalize(uint16_t crc_in);

/**
 * @ingroup CRC
 * @brief Processes a block of data to update the accumulated CRC16 sum.
 * @param p_data Pointer to the data block.
 * @param length Length of the data block.
 * @param crc_in Current accumulated CRC16 sum.
 * @return Updated CRC16 sum.
 */
LIB_EXPORT uint16_t
crc16_update(const uint8_t* p_data, size_t length, uint16_t crc_in);

/**
 * @ingroup CRC
 * @brief Calculates the CRC16 sum over the whole data.
 * @param p_data Pointer to the data.
 * @param length Length of the data.
 * @return CRC16 sum.
 */
LIB_EXPORT uint16_t 
crc16(const uint8_t* p_data, size_t length);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // CRC16_H
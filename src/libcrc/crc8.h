/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc8.h
 * @author    phstream
 * @copyright 2024, phstream
 * @license   MIT
 * @date      19 maj 2025
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
#ifndef CRC8_H_ /* Guard against multiple inclusion */
#define	CRC8_H_

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
 * @brief CRC8 version as integer number.
 * @return Version as major# * 1000000 + minor# * 10000 + build#.
 */
LIB_EXPORT int
crc8_version();

/**
 * @ingroup CRC
 * @brief Initializes the CRC8 calculation.
 * @return Initial CRC8 sum.
 */
LIB_EXPORT uint8_t
crc8_initialize();

/**
 * @ingroup CRC
 * @brief Finalizes the CRC8 calculation after processing the data.
 * @param crc_in Current CRC8 sum to be finalized.
 * @return Final CRC8 sum.
 */
LIB_EXPORT uint8_t 
crc8_finalize(uint8_t crc_in);

/**
 * @ingroup CRC
 * @brief Processes a block of data to update the accumulated CRC8 sum.
 * @param p_data Pointer to the data block.
 * @param length Length of the data block.
 * @param crc_in Current accumulated CRC8 sum.
 * @return Updated CRC8 sum.
 */
LIB_EXPORT uint8_t
crc8_update(const uint8_t* p_data, size_t length, uint8_t crc_in);

/**
 * @ingroup CRC
 * @brief Calculates the CRC8 sum over the whole data.
 * @param p_data Pointer to the data.
 * @param length Length of the data.
 * @return CRC8 sum.
 */
LIB_EXPORT uint8_t 
crc8(const uint8_t* p_data, size_t length);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // CRC8_H

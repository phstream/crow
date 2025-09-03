/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc32.h
 * @author    phstream
 * @copyright 2024, phstream
 * @license   MIT
 * @date      20 Jul 2024
 ****************************************************************************/
/** @defgroup  CRC CRC Calculation
 * @brief     Cyclic Redundancy Checksum 32bit calculation
 * @details
 *
 * @pre       common/lib_defs.h
 * @bug       
 * @warning
 * @ingroup   CROW
 ****************************************************************************/
#ifndef CRC32_H_ /* Guard against multiple inclusion */
#define	CRC32_H_

#include "lib_defs.h"

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
 * @brief CRC32 version as integer number.
 * @return Version as major# * 1000000 + minor# * 10000 + build#.
 */
LIB_EXPORT int
crc32_version();

/**
 * @ingroup CRC
 * @brief Initializes the CRC32 calculation.
 * @return Initial CRC32 sum.
 */
LIB_EXPORT uint32_t
crc32_initialize();

/**
 * @ingroup CRC
 * @brief Finalizes the CRC32 calculation after processing the data.
 * @param crc_in Current CRC32 sum to be finalized.
 * @return Final CRC32 sum.
 */
LIB_EXPORT uint32_t 
crc32_finalize(uint32_t crc_in);

/**
 * @ingroup CRC
 * @brief Processes a block of data to update the accumulated CRC32 sum.
 * @param p_data Pointer to the data block.
 * @param length Length of the data block.
 * @param crc_in Current accumulated CRC32 sum.
 * @return Updated CRC32 sum.
 */
LIB_EXPORT uint32_t
crc32_update(const uint8_t* p_data, size_t length, uint32_t crc_in);

/**
 * @ingroup CRC
 * @brief Calculates the CRC32 sum over the whole data.
 * @param p_data Pointer to the data.
 * @param length Length of the data.
 * @return CRC32 sum.
 */
LIB_EXPORT uint32_t
crc32_calc(const uint8_t* p_data, size_t length);

#ifdef __cplusplus
}
#endif

#endif    /* End of multiple inclusion guard CRC32_H_ */ 

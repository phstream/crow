/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc32builder.hpp
 * @author    phatream
 * @copyright 2025, phstream
 * @license   MIT
 * @date      4 Jul 2025
 ****************************************************************************/
/** @defgroup  CRC CRC Calculation
 * @brief     Cyclic Redundancy Checksum 32bit calculation builder class
 * @details
 *
 * @pre       
 * @bug       
 * @warning
 * @ingroup   CROW
 ****************************************************************************/
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <ostream>
#include <vector>

namespace crow {

/// @brief CRC32 sum builder class.
class crc32builder {
public:
    /// @brief Constructor. Initializes CRC32 sum to default start value.
    crc32builder();

    /// @brief Resets the CRC32 sum to the initial state.
    void reset();
    
    /// @brief Updates the CRC32 sum with a single byte.
    /// @param byte input byte to include in the CRC sum
    void update(uint8_t byte);

    /// @brief Updates the CRC32 with a blob of data.
    /// @param data pointer to data blob
    /// @param len length of the blob in bytes
    void update(const void* data, size_t len);

    /// @brief Retrieve final CRC sum.
    /// @return CRC sum
    uint32_t get() const;

    /// @brief Converts final CRC32 sum to string.
    /// @param hex format as hexadecimal (default: true), otherwise decimal
    /// @param with_prefix add "0x" prefix if hex (default: true)
    /// @return CRC sum as formatted string
    std::string to_string(bool hex = true, bool with_prefix = true) const;

    /// @brief Direct full calculation of CRC32 over data blob.
    /// @param data pointer to data blob
    /// @param len length of the blob in bytes
    /// @return final CRC sum
    static uint32_t calculate(const void* data, size_t len);

    /// @brief Appends a byte to the CRC32 sum via stream operator.
    crc32builder& operator<<(uint8_t byte);

    /// @brief Appends a C-string's contents to the CRC32 sum via stream operator.
    crc32builder& operator<<(const char* c_str);

    /// @brief Appends a string's contents to the CRC32 sum via stream operator.
    crc32builder& operator<<(const std::string& str);

    /// @brief Appends a vector of chars to the CRC32 sum via stream operator.
    crc32builder& operator<<(const std::vector<char>& vec);

    /// @brief Appends a vector of bytes to the CRC32 sum via stream operator.
    crc32builder& operator<<(const std::vector<uint8_t>& vec);

    /// @brief Converts the CRC32 sum to a string implicitly.
    operator std::string() const;

private:
    /// @brief Internal CRC32 sum state.
    uint32_t _crc_sum;
};

/// @brief Outputs the final CRC32 sum as big-endian bytes (MSB first).
/// @param os Output stream.
/// @param c CRC32 builder to serialize.
/// @return Reference to the output stream.
std::ostream& operator<<(std::ostream& os, const crc32builder& c);

} // namespace crow
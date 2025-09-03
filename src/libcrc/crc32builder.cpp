/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      crc32builder.cpp
 * @author    phstream
 * @copyright 2025, phstream
 * @license   MIT
 * @date      4 Jul 2025
 ****************************************************************************/
#include <sstream>
#include <iomanip>
#include <cstring>

#include "crc32builder.hpp"
#include "crc32.h"

namespace crow {

crc32builder::crc32builder() {
    reset();
}

void crc32builder::reset() {
    _crc_sum = crc32_initialize();
}

void crc32builder::update(uint8_t byte) {
    _crc_sum = crc32_update(&byte, 1, _crc_sum);
}

void crc32builder::update(const void* data, size_t len) {
    _crc_sum = crc32_update(reinterpret_cast<const uint8_t*>(data), len, _crc_sum);
}

uint32_t crc32builder::get() const {
    return crc32_finalize(_crc_sum);
}

std::string crc32builder::to_string(bool hex, bool with_prefix) const {
    std::ostringstream oss;
    if (hex) {
        if (with_prefix) oss << "0x";
        oss << std::hex << std::uppercase << get();
    } else {
        oss << get();
    }
    return oss.str();
}

crc32builder::operator std::string() const {
    return to_string();
}

uint32_t crc32builder::calculate(const void* data, size_t len) {
    return crc32_calc(reinterpret_cast<const uint8_t*>(data), len);
}

crc32builder& crc32builder::operator<<(uint8_t byte) {
    update(byte);
    return *this;
}

crc32builder& crc32builder::operator<<(const char* c_str) {
    update(c_str, std::strlen(c_str));
    return *this;
}

crc32builder& crc32builder::operator<<(const std::string& str) {
    update(str.data(), str.size());
    return *this;
}

crc32builder& crc32builder::operator<<(const std::vector<char>& vec) {
    if (!vec.empty())
        update(vec.data(), vec.size());
    return *this;
}

crc32builder& crc32builder::operator<<(const std::vector<uint8_t>& vec) {
    if (!vec.empty())
        update(vec.data(), vec.size());
    return *this;
}

std::ostream& operator<<(std::ostream& os, const crc32builder& c) {
    os << c.to_string(true, true);
    return os;
}

} // namespace crow

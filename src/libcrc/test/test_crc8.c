#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "../crc8.h"

int main(void) {    
    int version = crc8_version();
    assert(version >= 1000000);
    printf("✅ Test passed: Version = %i\n", version);
    
    uint32_t result = crc32_initialize();
    result = crc32_finalize(result);
    assert(result == 0UL);
    printf("✅ Test passed: Initialize, Finalize\n");

    const char* data1 = "Lorem ipsum dolor sit amet consectetur adipiscing elit. ";
    const char* data2 = "Quisque faucibus ex sapien vitae pellentesque sem placerat. ";
    const char* data3 = "In id cursus mi pretium tellus duis convallis. ";
    const char* data4 = "Tempus leo eu aenean sed diam urna tempor. ";
    const char* data5 = "Pulvinar vivamus fringilla lacus nec metus bibendum egestas. ";
    const char* data6 = "Iaculis massa nisl malesuada lacinia integer nunc posuere. ";
    const char* data7 = "Ut hendrerit semper vel class aptent taciti sociosqu. ";
    const char* data8 = "Ad litora torquent per conubia nostra inceptos himenaeos.";
    result = crc32_initialize();
    result = crc32_update((const unsigned char*) data1, strlen(data1), result);
    result = crc32_update((const unsigned char*) data2, strlen(data2), result);
    result = crc32_update((const unsigned char*) data3, strlen(data3), result);
    result = crc32_update((const unsigned char*) data4, strlen(data4), result);
    result = crc32_update((void*) 0, 0, result);
    result = crc32_update((const unsigned char*) data5, strlen(data5), result);
    result = crc32_update((const unsigned char*) data6, strlen(data6), result);
    result = crc32_update((const unsigned char*) data7, strlen(data7), result);
    result = crc32_update((const unsigned char*) data8, strlen(data8), result);
    result = crc32_finalize(result);
    assert(result == 0xBF4A2172UL);
    printf("✅ Test passed: Updated CRC32 = 0x%08X\n", result);

    const char* input = "123456789";
    result = crc32_calc((const unsigned char*)input, strlen(input));
    assert(result == 0xCBF43926UL);
    printf("✅ Test passed: CRC32(\"%s\") = 0x%08X\n", input, result); */
    return 0;
}

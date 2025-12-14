#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <charconv>
#include <iomanip>

std::string hex_to_utf8(std::string hex_string);

std::string utf8_to_hex(std::string utf8_string);


std::string hex_to_utf8(std::string hex_string) {
    if (hex_string.size() % 2 == 1) hex_string = "0" + hex_string;
   
    std::string utf8_string = "";
    uint8_t utf_code;
    for (unsigned long i = 0; i < hex_string.size(); i += 2) {
        std::from_chars(hex_string.data() + i, hex_string.data() + i + 2, utf_code, 16);
        utf8_string += static_cast<unsigned char>(utf_code);
    }

    return utf8_string;
}


std::string utf8_to_hex(std::string utf8_string) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned long i = 0; i < utf8_string.size(); i++) {
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(utf8_string[i]));
    }
    return ss.str();
}

#endif

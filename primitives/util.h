#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <charconv>
#include <iomanip>
#include <vector>

std::string hex_to_utf8(std::string hex_string);

std::string utf8_to_hex(std::string utf8_string);

std::vector<std::string> split(std::string utf8_string, char delim);

std::vector<uint8_t> vectorize(std::string s);

std::vector<uint8_t> vectorize_hex(std::string s);

std::string vector_to_hex(std::vector<uint8_t> v);


std::string hex_to_utf8(std::string hex_string) {
    if (hex_string.size() % 2 == 1) hex_string = "0" + hex_string;
   
    std::string utf8_string = "";
    for (int i = 0; i < hex_string.size(); i += 2) {
        utf8_string += static_cast<unsigned char>(stoi(hex_string.substr(i, 2), nullptr, 16));
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

std::vector<std::string> split(std::string s, char delim) {
    std::vector<std::string> res;
    std::string cur = "";

    for (char c : s) {
        if (c == delim) {
            res.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) res.push_back(cur);
    return res;
}

std::string increment_hex_string(std::string s) {
    bool overflow = true; 
    int pos = s.size()-1;
    while (overflow && pos >= 0) {
        if (s[pos] == '9') {
            s[pos] = 'a';
            overflow = false;
        } else if (s[pos] == 'f') {
            s[pos] = '0';
            pos--;
        } else {
            s[pos]++;
            overflow = false;
        }
    }
    return s;
}

std::vector<uint8_t> vectorize(std::string s) {
    std::vector<uint8_t> v;
    for (char c : s) {
        v.push_back(static_cast<uint8_t>(c));
    }
    return v;
}

std::vector<uint8_t> vectorize_hex(std::string s) {
    if (s.size() % 2 == 1) s = '0' + s;
    
    std::vector<uint8_t> v;
    for (int i = 0; i < s.size(); i += 2) {
        v.push_back(static_cast<uint8_t>(stoi(s.substr(i, 2), nullptr, 16)));
    } 
    return v;
}

std::string vector_to_hex(std::vector<uint8_t> v) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto& i : v) {
        ss << std::setw(2) << static_cast<unsigned int>(i);
    }
    return ss.str();
}
#endif

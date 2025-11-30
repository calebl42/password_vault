#include "aes.h"
#include <iostream>

int main() {
    uint8_t tester[4][4] = {{1,2,3,4}, {1,2,3,4}, {1,2,3,4}, {1,2,3,4}};
    shift_rows(tester);
    std::cout << "testing shift_rows\n";
    for (int i = 0; i < 4; i++) {
       for (int j = 0; j < 4; j++) {
           std::cout << static_cast<uint32_t>(tester[i][j]);
       }
       std::cout << '\n';
    }
  
    std::cout << "\ntesting mult_gf\n"; 
    std::cout << std::hex << std::setfill('0'); 
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x01)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x02)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x04)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x08)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x10)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x20)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x40)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x80)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x57, 0x13)) << '\n';
    std::cout << std::setw(2) << static_cast<uint32_t>(mult_gf(0x03, 0x5d)) << '\n';
   
    std::cout << "\ntesting sub_word\n"; 
    std::cout << sub_word(0xfae23401) << '\n';
   
    uint32_t* keys = key_expansion("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
    std::cout << "\ntesting key_expansion\n";
    for (int i = 0; i < 60; i++) {
        std::cout << std::dec << i << ": " << std::hex << std::setw(8) << keys[i] << '\n';
    }

    std::cout << "\ntesting aes cipher\n";  
    std::cout << aes_cipher("2b7e151628aed2a6abf7158809cf4f3c", hex_to_utf8("3243f6a8885a308d313198a2e0370734")) << '\n';
    std::cout << aes_cipher("c7b29cd399258d694a50a2e89ec6055ce5487f0706fcb86a5bb7466e824bc264", hex_to_utf8("3243f6a8885a308d313198a2e0370734")) << '\n';
    std::cout << (static_cast<uint8_t>(0xd4) ^ mult_gf(0x02, 0xbf) ^ mult_gf(0x03, 0x5d) ^ static_cast<uint8_t>(0x30)) << '\n';
    return 0;
}

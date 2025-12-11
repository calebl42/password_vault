#include "aes.h"
#include <iostream>
#include <ctime>

int main() {
    srand(time(NULL));

    uint8_t tester[4][4] = {{1,2,3,4}, {1,2,3,4}, {1,2,3,4}, {1,2,3,4}};
    shift_rows_inv(tester);
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

    for (int i = 0; i < 10; i++) {
        std::stringstream key;
        std::stringstream message;

        for (int i = 0; i < 64; i++) {
            key << std::hex << (rand() % 16);
        }
        
        for (int i = 0; i < 32; i++) {
            message << std::hex << (rand() % 16);
        }

        std::cout << "encrypting plaintext " << message.str() << " using 256-bit key: " << key.str() << "\n";
        std::string ciphertext = aes_cipher(key.str(), hex_to_utf8(message.str()));
        std::cout << "resulting ciphertext: " << ciphertext << "\n"; 
        std::string decrypted = utf8_to_hex(aes_cipher_inv(key.str(), ciphertext));
        std::cout << "decrypted plaintext: " << decrypted << "\n\n";
        if (message.str() != decrypted) {
            std::cout << "TEST FAILED, DECRYPTED MESSAGE NOT EQUIVAL TO PLAINTEXT.";
            return -1;
        }
    } 

    std::cout << "Tests Passed!\n";
    return 0;
}

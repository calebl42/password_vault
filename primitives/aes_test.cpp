#include "aes.h"
#include <iostream>

int main() {
    int tester[4][4] = {{1,2,3,4}, {1,2,3,4}, {1,2,3,4}, {1,2,3,4}};
    shift_rows(tester);

    for (int i = 0; i < 4; i++) {
       for (int j = 0; j < 4; j++) {
           std::cout << tester[i][j];
       }
       std::cout << '\n';
    }
   
    std::cout << '\n' << std::hex << std::setfill('0'); 
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x01)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x02)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x04)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x08)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x10)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x20)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x40)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x80)) << '\n';
    std::cout << std::setw(2) << static_cast<unsigned int>(mult_gf(0x57, 0x13)) << '\n';
    return 0;
}

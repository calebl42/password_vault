#include "hmac.h"
#include "pbkdf2.h"
#include <iostream>
#include "aes.h"

int main() {
    //std::string true_password = pbkdf2(utf8_to_hex("pumpkin"), utf8_to_hex("essalado"), 10000, 256);
    //std::cout << true_password;
    std::cout << counter_mode_encrypt("a3dc034d1776e30bdfc00978378b3e2d9b97127bc5aedf5196410aa4a28ed719", utf8_to_hex("This is the verification message, good luck trying to crack my passwords!"), "00000000000000000000000000000000") << '\n'; 
    std::cout << utf8_to_hex("This is the verification message, good luck trying to crack my passwords!") << '\n';
    return 0;
}

#include "hmac.h"
#include "pbkdf2.h"
#include <iostream>
#include "aes.h"

int main() {
    std::cout << hex_to_utf8(utf8_to_hex("I love hamsters alot, they are really cute. I wish I was a little orange hamster in my burrow, eating apples and peanuts, sleeping all day. Oh how peaceful life would be!"));
    return 0;
}

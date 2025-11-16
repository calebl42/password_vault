#include <iostream>
#include "hmac.h"


int main() {
    std::cout << hmac_sha256("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "This is just a test for generating HMAC") << '\n';
    return 0;
}

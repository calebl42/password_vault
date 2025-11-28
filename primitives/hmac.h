#ifndef HMAC_H
#define HMAC_H

#include "sha.h"
#include "util.h"

// Both k and m are to be utf-8 encoded strings.
// preferred length of k is 32 bytes
std::string hmac_sha256(std::string k, std::string m);


std::string hmac_sha256(std::string k, std::string m) {
    while (k.size() < 64) {
        k += static_cast<unsigned char>(0); 
    }

    std::string opadded_k = "";
    for (char& c : k) {
        opadded_k += static_cast<unsigned char>(static_cast<uint8_t>(c) ^ 0x5C);
    }
    
    std::string ipadded_k = "";
    for (char& c : k) {
        ipadded_k += static_cast<unsigned char>(static_cast<uint8_t>(c) ^ 0x36);
    }

    std::string inner_hash = sha256(ipadded_k + m);
    std::string outer_hash = sha256(opadded_k + hex_to_utf8(inner_hash));
    return outer_hash;
}

#endif

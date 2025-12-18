#ifndef HMAC_H
#define HMAC_H

#include "sha.h"
#include "util.h"

// k is preferably 32 bytes long
// m and k must be hex-strings
// m can be at most 2^64-1 bits long
std::string hmac_sha256(std::string k, std::string m);


std::string hmac_sha256(std::string k, std::string m) {
    std::vector<uint8_t> k_vec = vectorize_hex(k);
    std::vector<uint8_t> m_vec = vectorize_hex(m);

    while (k_vec.size() < 64) k_vec.push_back(0); 
    std::vector<uint8_t> opadded_k;
    for (uint8_t& i : k_vec) {
        opadded_k.push_back(i ^ 0x5C);
    }
    
    std::vector<uint8_t> ipadded_k;
    for (uint8_t& i : k_vec) {
        ipadded_k.push_back(i ^ 0x36);
    }

    for (uint8_t& i : m_vec) {
        ipadded_k.push_back(i);
    }
    std::string inner_hash = sha256(ipadded_k);

    for (uint8_t& i : vectorize_hex(inner_hash)) {
        opadded_k.push_back(i);
    }

    std::string outer_hash = sha256(opadded_k);
    return outer_hash;
}

#endif

#ifndef PBKDF2_H
#define PBKDF2_H

#include "hmac.h"
#include "util.h"

// c is the number of iterations; should be at least 600,000 according to owasp
// salt value should be at least 8 bytes long, and randomly generated 
// password and salt must be hex-strings
// returns hex-string key of length dkLen
std::string pbkdf2(std::string password, std::string salt, uint32_t c, uint32_t dkLen) {
    uint32_t hLen = hmac_sha256(utf8_to_hex("test key"), utf8_to_hex("test message")).size() * 4;
    
    std::vector<uint8_t> dk;
    for (uint32_t i = 1; i <= dkLen/hLen; i++) {
        std::vector<uint8_t> augmented_salt = vectorize_hex(salt);
        augmented_salt.push_back(static_cast<uint8_t>(i >> 24));
        augmented_salt.push_back(static_cast<uint8_t>(i >> 16));
        augmented_salt.push_back(static_cast<uint8_t>(i >> 8));
        augmented_salt.push_back(static_cast<uint8_t>(i));
        std::vector<uint8_t> T = vectorize_hex(hmac_sha256(password, vector_to_hex(augmented_salt)));
        std::vector<uint8_t> prev = T;
        for (uint32_t j = 2; j <= c; j++) {
            std::vector<uint8_t> next = vectorize_hex(hmac_sha256(password, vector_to_hex(prev))); 
            prev = next;
            for (unsigned long k = 0; k < T.size(); k++) {
                T[k] = T[k] ^ next[k]; 
            }
        }
    
        for (auto& a : T) dk.push_back(a);
    }
    
    return vector_to_hex(dk);
}

#endif

#ifndef PBKDF2_H
#define PBKDF2_H

#include "hmac.h"
#include "util.h"

typedef std::string (*prf)(std::string, std::string);
prf current_prf;

// c is the number of iterations; should be at least 600,000 according to owasp
// dkLen is the bitlength of the derived key
// salt should be at least 8 chars long, and randomly generated 
std::string pbkdf2(prf prf_param, std::string password, std::string salt, int c, int dkLen) {
    current_prf = prf_param;
    int hLen = current_prf("test key", "test message").size() * 4;
    
    std::string dk = "";
    for (int i = 1; i <= dkLen/hLen; i++) {
        std::string augmented_salt = salt;
        augmented_salt += static_cast<unsigned char>(i >> 24);
        augmented_salt += static_cast<unsigned char>(i >> 16);
        augmented_salt += static_cast<unsigned char>(i >> 8);
        augmented_salt += static_cast<unsigned char>(i);
        std::string T = hex_to_utf8(current_prf(password, augmented_salt));
        std::string prev = T;
        for (int j = 2; j <= c; j++) {
            std::string next = hex_to_utf8(current_prf(password, prev)); 
            prev = next;
            for (unsigned long k = 0; k < T.size(); k++) {
                T[k] = static_cast<unsigned char>(T[k] ^ next[k]); 
            }
        }

        dk += T;
    }
    
    return utf8_to_hex(dk);
}

#endif

#ifndef SHA_H
#define SHA_H

#include <iostream>
#include <string>
#include <cmath>
#include <bitset>
#include <cstdint>
#include <sstream>
#include <iomanip>

static uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// logical functions
//----------------------------------------------
uint32_t Ch(uint32_t x, uint32_t y, uint32_t z);

uint32_t Maj(uint32_t x, uint32_t y, uint32_t z);

uint32_t rotr(uint32_t x, uint32_t shamt);

uint32_t sum_0(uint32_t x);

uint32_t sum_1(uint32_t x);

uint32_t s_0(uint32_t x);

uint32_t s_1(uint32_t x);

// returns the array of hash values as a hex string, assuming H consists of 8 32-bit words
std::string to_hexstring(uint32_t* H); 

// returns the padding size of the message in bits
uint64_t get_padding_size(std::string message);

void pad256(std::string& message);

/* 
 * INPUT: utf-8 encoded message (max length allowed is 2^64-1 bits)
 * RETURNS: sha256 hash value as a hex character string
 */
std::string sha256(std::string message);

//----------------------------------------------
inline uint32_t Ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);     
}

inline uint32_t Maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

inline uint32_t rotr(uint32_t x, uint32_t shamt) {
    return (x >> shamt) | (x << (32 - shamt));
}

inline uint32_t sum_0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline uint32_t sum_1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

inline uint32_t s_0(uint32_t x) {
   return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline uint32_t s_1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

std::string to_hexstring(uint32_t* H) {
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (int i = 0; i < 8; ++i) {
		ss << std::setw(8) << H[i];
	}
	return ss.str();	
}

void pad256(std::string& message) {
    const uint64_t original_size = message.size() * 8; 
    uint64_t k = 0;
    while ((original_size + 1 + k) % 512 != 448) ++k;
	const uint64_t padding_size = 1 + k + 64;
	
	message += static_cast<unsigned char>(0x80);
	for (uint8_t i = 1; i < (padding_size - 64) >> 3; ++i) {
		message += static_cast<unsigned char>(0);
	}
	
	message += static_cast<unsigned char>(original_size >> 56);
	message += static_cast<unsigned char>(original_size >> 48);
	message += static_cast<unsigned char>(original_size >> 40);
	message += static_cast<unsigned char>(original_size >> 32);
	message += static_cast<unsigned char>(original_size >> 24);
	message += static_cast<unsigned char>(original_size >> 16);
	message += static_cast<unsigned char>(original_size >> 8);
	message += static_cast<unsigned char>(original_size);
}

std::string sha256(std::string message) {
	pad256(message);
	uint32_t H[8] = {
		0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };

	uint32_t a; 
	uint32_t b;
	uint32_t c;
	uint32_t d;
	uint32_t e;
	uint32_t f;
	uint32_t g;
	uint32_t h;
	uint32_t t_1;
	uint32_t t_2;

    uint64_t N = message.size() >> 6;		
	uint64_t message_index = 0;	
    for (uint64_t i = 0; i < N; ++i) {
        uint32_t W[64];
        for (uint8_t j = 0; j < 16; ++j) {	
			W[j] = 0;
			W[j] = W[j] | static_cast<unsigned char>(message[message_index]);
			W[j] <<= 8;
			W[j] = W[j] | static_cast<unsigned char>(message[message_index+1]);
			W[j] <<= 8;
			W[j] = W[j] | static_cast<unsigned char>(message[message_index+2]);
			W[j] <<= 8;
			W[j] = W[j] | static_cast<unsigned char>(message[message_index+3]);
           	message_index += 4;
        }
        for (uint8_t j = 16; j < 64; ++j) {
            W[j] = s_1(W[j-2]) + W[j-7] + s_0(W[j-15]) + W[j-16];
        }

        a = H[0]; 
        b = H[1];
        c = H[2];
        d = H[3];
        e = H[4];
        f = H[5];
        g = H[6];
        h = H[7];
        
        for (uint8_t j = 0; j < 64; ++j) {
            t_1 = h + sum_1(e) + Ch(e, f, g) + K[j] + W[j];
            t_2 = sum_0(a) + Maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + t_1;
            d = c; 
            c = b; 
            b = a;
            a = t_1 + t_2;
        }

        H[0] = a + H[0];
        H[1] = b + H[1];
        H[2] = c + H[2];
        H[3] = d + H[3];
        H[4] = e + H[4];
        H[5] = f + H[5];
        H[6] = g + H[6];
        H[7] = h + H[7];
    }
	
	return to_hexstring(H);
}

#endif

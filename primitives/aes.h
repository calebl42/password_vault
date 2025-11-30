#ifndef AES_H
#define AES_H

#include "util.h"

uint32_t Rcon[10] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 
    0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000
};

uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// message must be 128 bits (utf-8), key must be a hex string
// returns a hex-string ciphertext
std::string aes_cipher(std::string key, std::string message);

std::string aes_inv_cipher(std::string key, std::string ciphertext);

// polynomial mutliplication in GF(2^8)
// we reduce modulo x^8 + x^4 + x^3 + x + 1
uint8_t mult_gf(uint8_t a, uint8_t b);

void sub_bytes(uint8_t state[4][4]);

uint32_t sub_word(uint32_t word);

uint32_t rot_word(uint32_t word);

void shift_rows(uint8_t state[4][4]);

void mix_columns(uint8_t state[4][4]);

void add_round_key(uint8_t state[4][4], uint32_t* round_keys, int round);

// returns the round keys as an array of 4*(num_rounds + 1) words 
// key must be a hex string
uint32_t* key_expansion(std::string key);


uint8_t mult_gf(uint8_t a, uint8_t b) {
    uint8_t res = 0;
    uint8_t cur_product = a;
    
    // treat b as sum of powers of 2
    // distribute a onto each element of b
    for (int i = 0; i < 8; i++) { 
        // xTimes
        if (i > 0) cur_product = (cur_product >> 7) ? ((cur_product << 1) ^ 0x1b) : (cur_product << 1); 
        if (b & 1) {
            res ^= cur_product;
        }   
        b >>= 1;
    }

    return res;
}

void sub_bytes(uint8_t state[4][4]) { 
    for (int i = 0; i < 4; i++) {
        for (int j =0; j < 4; j++) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}

void shift_rows(uint8_t state[4][4]) {
    uint8_t temp1, temp2;
    temp1 = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp1;

    temp1 = state[2][0];
    temp2 = state[2][1];
    state[2][0] = state[2][2];
    state[2][1] = state[2][3];
    state[2][2] = temp1;
    state[2][3] = temp2;

    temp1 = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp1;
}

void mix_columns(uint8_t state[4][4]) {
    for (int j = 0; j < 4; j++) {
        uint8_t next_state_0j = (mult_gf(0x02, state[0][j]) ^ mult_gf(0x03, state[1][j]) ^ state[2][j] ^ state[3][j]);
        uint8_t next_state_1j = (state[0][j] ^ mult_gf(0x02, state[1][j]) ^ mult_gf(0x03, state[2][j]) ^ state[3][j]);
        uint8_t next_state_2j = (state[0][j] ^ state[1][j] ^ mult_gf(0x02, state[2][j]) ^ mult_gf(0x03, state[3][j]));
        uint8_t next_state_3j = (mult_gf(0x03, state[0][j]) ^ state[1][j] ^ state[2][j] ^ mult_gf(0x02, state[3][j])); 
        
        state[0][j] = next_state_0j;
        state[1][j] = next_state_1j;
        state[2][j] = next_state_2j;
        state[3][j] = next_state_3j;
    }
}

uint32_t sub_word(uint32_t word) {
    return (
        (static_cast<uint32_t>(sbox[static_cast<uint8_t>((word & 0xff000000) >> 24)]) << 24) |
        (static_cast<uint32_t>(sbox[static_cast<uint8_t>((word & 0x00ff0000) >> 16)]) << 16) |
        (static_cast<uint32_t>(sbox[static_cast<uint8_t>((word & 0x0000ff00) >> 8)]) << 8) |
        (static_cast<uint32_t>(sbox[static_cast<uint8_t>(word & 0x000000ff)]))  
    );
}

uint32_t rot_word(uint32_t word) {
    return (
        ((word & 0x00ff0000) << 8) | 
        ((word & 0x0000ff00) << 8) | 
        ((word & 0x000000ff) << 8) |
        ((word & 0xff000000) >> 24)
    );
}

uint32_t* key_expansion(std::string key) {
    key = hex_to_utf8(key);
    int Nk = key.size() / 4; 
    int num_rounds;   
    switch(Nk) {
        case 4:
            num_rounds = 10;
            break;
        case 6:
            num_rounds = 12; 
            break;
        case 8:
            num_rounds = 14;
            break;
        default:
            num_rounds = 0;
            break;
    }

    const int arr_size = 4 * (num_rounds + 1);
    uint32_t* round_keys = new uint32_t[arr_size];
    
    for (int i = 0; i < Nk; i++) { 
        round_keys[i] = ( 
            (static_cast<uint32_t>(static_cast<uint8_t>(key[4*i])) << 24) | 
            (static_cast<uint32_t>(static_cast<uint8_t>(key[4*i+1])) << 16) | 
            (static_cast<uint32_t>(static_cast<uint8_t>(key[4*i+2])) << 8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(key[4*i+3])))
        );
    }

    for (int i = Nk; i < arr_size; i++) {
        if (i % Nk == 0) {
            round_keys[i] = round_keys[i-Nk] ^ sub_word(rot_word(round_keys[i-1])) ^ Rcon[i/Nk-1];
        } else if ((i + 4) % 8 == 0) {
            round_keys[i] = round_keys[i-Nk] ^ sub_word(round_keys[i-1]);
        } else {
            round_keys[i] = round_keys[i-Nk] ^ round_keys[i-1];
        }
    }

    return round_keys;
}

void add_round_key(uint8_t state[4][4], uint32_t* round_keys, int round) {
    for (int j = 0; j < 4; j++) {
        state[0][j] ^= static_cast<uint8_t>((round_keys[4*round+j] & 0xff000000) >> 24);
        state[1][j] ^= static_cast<uint8_t>((round_keys[4*round+j] & 0x00ff0000) >> 16);
        state[2][j] ^= static_cast<uint8_t>((round_keys[4*round+j] & 0x0000ff00) >> 8);
        state[3][j] ^= static_cast<uint8_t>((round_keys[4*round+j] & 0x000000ff));
    }
}

void print_state(uint8_t state[4][4]) {
    std::cout << '\n';
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(state[i][j]) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

std::string aes_cipher(std::string key, std::string message) {
    uint8_t state[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = static_cast<uint8_t>(message[i + 4*j]);
        }
    }
    // print_state(state);
    uint32_t* round_keys = key_expansion(key);
    int num_rounds; 
    switch(key.size() * 4) {
        case 128:
            num_rounds = 10;
            break;
        case 192:
            num_rounds = 12; 
            break;
        case 256:
            num_rounds = 14;
            break;
        default:
            num_rounds = 0;
            break;
    }
    add_round_key(state, round_keys, 0);
    
    for (int round = 1; round < num_rounds; round++) {
        // std::cout << "start of round: " << round << '\n'; 
        // print_state(state);
        sub_bytes(state); 
        //print_state(state); 
        shift_rows(state); 
        //print_state(state); 
        mix_columns(state); 
        //print_state(state);
        add_round_key(state, round_keys, round);
    }
    // std::cout << "start of round: 10" << '\n';
    // print_state(state);
    sub_bytes(state);
    // print_state(state);
    shift_rows(state); 
    // print_state(state);
    add_round_key(state, round_keys, num_rounds);
    // print_state(state);
    std::string state_utf8 = "";

    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            state_utf8 += static_cast<unsigned char>(state[i][j]);
        }
    }

    return utf8_to_hex(state_utf8);
}

#endif

#include <iostream>
#include <string>
#include <cmath>
#include <bitset>
#include "sha.h"
#include <openssl/evp.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

std::string sha256_openssl(const std::string& input) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    
	EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_sha256(), NULL);
    EVP_DigestUpdate(context, input.data(), input.size());
	EVP_DigestFinal_ex(context, hash, &hash_len);
    EVP_MD_CTX_free(context);

   	std::stringstream ss; 
	ss << std::hex << std::setfill('0');
    for(unsigned int i = 0; i < hash_len; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }

    return ss.str();
}

int main(int argc, char* argv[]) {
	srand(time(0));
	const long num_messages = 20000;
	const int num_bars = 60;
	std::string test_messages[num_messages];	

	for (int i = 0; i < num_messages; i++) {
		std::string cur = "";
		for (int j = 0; j < 10000; j++) {
			cur += static_cast<unsigned char>(rand() % 0xFF);
		}
		test_messages[i] = cur;
	}

	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "=============== CUSTOM HASHES ================\n";
    for (int i = 0; i < num_messages; i++) {
        sha256(test_messages[i]);

		std::string bar = "";
		for (int j = 0; j < static_cast<int>(ceil(static_cast<double>(i) / num_messages * num_bars)); j++) bar += '$';
		while (bar.size() < num_bars) bar += '_';	
		std::cout << "\rProgress " << static_cast<int>(ceil(static_cast<double>(i) / num_messages * 100)) << "%: [" << bar << "]" << std::flush;
	}	
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = end - start;
	std::cout << "\ntotal runtime duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";

	start = std::chrono::high_resolution_clock::now();
	std::cout << "\n\n=============== OPENSSL HASHES ================\n";
	for (int i = 0; i < num_messages; i++) {
		sha256_openssl(test_messages[i]);

		std::string bar = "";
		for (int j = 0; j < static_cast<int>(ceil(static_cast<double>(i) / num_messages * num_bars)); j++) bar += '$';
		while (bar.size() < num_bars) bar += '_';	
		std::cout << "\rProgress " << static_cast<int>(ceil(static_cast<double>(i) / num_messages * 100)) << "%: [" << bar << "]" << std::flush;
	}
	end = std::chrono::high_resolution_clock::now();
	duration = end - start;
	std::cout << "\ntotal runtime duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
	return 0;
}

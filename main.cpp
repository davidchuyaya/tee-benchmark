#include <iostream>
#include "algo.hpp"

int main() {
    std::string input = "Hello World!";

    // Test hashing
    std::cout << input << std::endl;
    std::cout << sha256(input) << std::endl;

    // Test random byte generation
    ENGINE* eng = randomSetup();
    std::cout << "Random bytes: " << random(64) << std::endl;
    
    // Test AES-128 encryption
    unsigned char* key = random(256);
    unsigned char* iv = random(128);
    unsigned char ciphertext[128];
    int ciphertextLen = aes128Encrypt((const unsigned char*) input.c_str(), input.size(), ciphertext, key, iv);
    std::cout << "Key: " << key << ", IV: " << iv << std::endl;
    std::cout << "Ciphertext length: " << ciphertextLen << std::endl;
    std::cout << "Ciphertext: " << ciphertext << std::endl;

    // Test AES-128 decryption
    unsigned char plaintext[128];
    int plaintextLen = aes128Decrypt(ciphertext, ciphertextLen, plaintext, key, iv);
    plaintext[plaintextLen] = '\0';
    std::cout << "Plaintext: " << plaintext << std::endl;

    randomCleanup(eng);
    return 0;
}
#include <openssl/evp.h>
#include <openssl/err.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "algo.hpp"

/**
 * From https://stackoverflow.com/a/40155962/4028758
*/
std::string sha256(const unsigned char* input, const int inputLen) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == NULL) {
        std::cerr << "EVP_MD_CTX_new() failed." << std::endl;
        abort();
    }
    int rc;
    rc = EVP_DigestInit_ex(context, EVP_sha256(), NULL);
    if (rc != 1) {
        std::cerr << "EVP_DigestInit_ex() failed." << std::endl;
        abort();
    }
    rc = EVP_DigestUpdate(context, input, inputLen);
    if (rc != 1) {
        std::cerr << "EVP_DigestUpdate() failed." << std::endl;
        abort();
    }
    rc = EVP_DigestFinal_ex(context, hash, &lengthOfHash);
    if (rc != 1) {
        std::cerr << "EVP_DigestFinal_ex() failed." << std::endl;
        abort();
    }
    std::stringstream ss;
    for(int i = 0; i < lengthOfHash; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    EVP_MD_CTX_free(context);
    return ss.str();
}

/**
 * From https://wiki.openssl.org/index.php/Random_Numbers
*/
// extern void OPENSSL_cpuid_setup(void);
ENGINE* randomSetup() {
    // OPENSSL_cpuid_setup();
    ENGINE_load_rdrand();
    ENGINE* eng = ENGINE_by_id("rdrand");
    if (eng == NULL) {
        std::cerr << "ENGINE_by_id(\"rdrand\") failed." << std::endl;
        abort();
    }
    int rc;
    rc = ENGINE_init(eng);
    if (rc == 0) {
        std::cerr << "ENGINE_init() failed." << std::endl;
        abort();
    }
    rc = ENGINE_set_default(eng, ENGINE_METHOD_RAND);
    if (rc == 0) {
        std::cerr << "ENGINE_set_default() failed." << std::endl;
        abort();
    }
    return eng;
}

unsigned char* random(const int bits) {
    int bytes = bits / 8;
    unsigned char* buffer = new unsigned char[bytes];
    RAND_bytes(buffer, bytes);
    return buffer;
}

void randomCleanup(ENGINE* eng) {
    ENGINE_finish(eng);
    ENGINE_free(eng);
    ENGINE_cleanup();
}

/**
 * From https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
*/
int aes128Encrypt(const unsigned char* input, const int inputLen, unsigned char* output, const unsigned char* key, const unsigned char* iv) {
    EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();
    if (context == NULL) {
        std::cerr << "EVP_CIPHER_CTX_new() failed." << std::endl;
        abort();
    }
    int rc;
    rc = EVP_EncryptInit_ex(context, EVP_aes_128_cbc(), NULL, key, iv);
    if (rc != 1) {
        std::cerr << "EVP_EncryptInit_ex() failed." << std::endl;
        abort();
    }
    int len;
    int outputLen;
    rc = EVP_EncryptUpdate(context, output, &len, input, inputLen);
    if (rc != 1) {
        std::cerr << "EVP_EncryptUpdate() failed." << std::endl;
        abort();
    }
    outputLen = len;
    rc = EVP_EncryptFinal_ex(context, output + len, &len);
    if (rc != 1) {
        std::cerr << "EVP_EncryptFinal_ex() failed." << std::endl;
        abort();
    }
    outputLen += len;
    EVP_CIPHER_CTX_free(context);
    return outputLen;
}

int aes128Decrypt(const unsigned char* input, const int inputLen, unsigned char* output, const unsigned char* key, const unsigned char* iv) {
   EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();
    if (context == NULL) {
        std::cerr << "EVP_CIPHER_CTX_new() failed." << std::endl;
        abort();
    }
    int rc;
    rc = EVP_DecryptInit_ex(context, EVP_aes_128_cbc(), NULL, key, iv);
    if (rc != 1) {
        std::cerr << "EVP_DecryptInit_ex() failed." << std::endl;
        abort();
    }
    int len;
    int outputLen;
    rc = EVP_DecryptUpdate(context, output, &len, input, inputLen);
    if (rc != 1) {
        std::cerr << "EVP_DecryptUpdate() failed." << std::endl;
        abort();
    }
    outputLen = len;
    rc = EVP_DecryptFinal_ex(context, output + len, &len);
    if (rc != 1) {
        std::cerr << "EVP_DecryptFinal_ex() failed." << std::endl;
        abort();
    }
    outputLen += len;
    EVP_CIPHER_CTX_free(context);
    return outputLen;
}

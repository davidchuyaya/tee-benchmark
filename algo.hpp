#include <openssl/engine.h>
#include <string>

std::string sha256(const unsigned char* input, const int inputLen);

ENGINE* randomSetup();
unsigned char* random(const int bits);
void randomCleanup(ENGINE* eng);
/* Returns the length of the output. */
int aes128Encrypt(const unsigned char* input, const int inputLen, unsigned char* output, const unsigned char* key, const unsigned char* iv);
int aes128Decrypt(const unsigned char* input, const int inputLen, unsigned char* output, const unsigned char* key, const unsigned char* iv);
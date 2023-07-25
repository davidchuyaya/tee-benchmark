#include <openssl/engine.h>
#include <string>
#include <fstream>

std::string sha256(const std::string input);

ENGINE* randomSetup();
unsigned char* random(const int bits);
void randomCleanup(ENGINE* eng);
/* Returns the length of the output. */
int aes128Encrypt(const unsigned char* input, const int inputLen, unsigned char* output, const unsigned char* key, const unsigned char* iv);
int aes128Decrypt(const unsigned char* input, const int inputLen, unsigned char* output, const unsigned char* key, const unsigned char* iv);

FILE* open(std::string filename);
void write(FILE* file, const unsigned char* buffer, const int bufferLen);
void fileSeekToHead(FILE* file);
void read(FILE* file, unsigned char* buffer, const int bufferLen);
void close(FILE* file);
#include <iostream>
#include <chrono>
#include <unistd.h>
#include "algo.hpp"

static int DEFAULT_WARMUP = 5;
static int DEFAULT_RUN = 10;
static int DEFAULT_BLOCK_SIZE = 4096;

struct config {
    int blockSize;
    int warmupSecs;
    int runSecs;
    bool hash;
    bool encrypt;
    bool decrypt;
};

std::chrono::steady_clock::time_point getTime() {
    return std::chrono::steady_clock::now();
}

int secondsSince(std::chrono::steady_clock::time_point startTime) {
    return std::chrono::duration_cast<std::chrono::seconds>(getTime() - startTime).count();
}


/** Human-readable test of each function, not meant for benchmarking performance. */
void miniTest() {
    std::string input = "Hello World!";

    // Test hashing
    std::cout << input << std::endl;
    std::cout << sha256((const unsigned char*) input.c_str(), input.size()) << std::endl;

    // Test random byte generation
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
}

void hashTest(const config conf) {
    unsigned char* input = random(conf.blockSize);
    std::string output;

    auto start = getTime();
    int numHashes = 0;
    while (true) {
        output = sha256(input, conf.blockSize);
        int elapsedTime = secondsSince(start);
        if (elapsedTime > conf.warmupSecs)
            numHashes++;
        if (elapsedTime > conf.warmupSecs + conf.runSecs)
            break;
    }

    std::cout << "Hashed " << numHashes << " blocks of " << conf.blockSize << " bits in " << conf.runSecs << "s." << std::endl;
    std::cout << "Hash throughput: " << numHashes / conf.runSecs << " /s." << std::endl;
}

void encryptionTest(const config conf) {
    const unsigned char* input = random(conf.blockSize);
    const unsigned char* key = random(256);
    unsigned char output[conf.blockSize*2];

    auto start = getTime();
    int numEncryptions = 0;
    while (true) {
        // generate different IV for each encryption, otherwise it's insecure
        const unsigned char* iv = random(128);
        aes128Encrypt(input, conf.blockSize, output, key, iv);
        int elapsedTime = secondsSince(start);
        if (elapsedTime > conf.warmupSecs)
            numEncryptions++;
        if (elapsedTime > conf.warmupSecs + conf.runSecs)
            break;
    }

    std::cout << "Encrypted " << numEncryptions << " blocks of " << conf.blockSize << " bits in " << conf.runSecs << "s." << std::endl;
    std::cout << "Encryption throughput: " << numEncryptions / conf.runSecs << " /s." << std::endl;
}

void decryptionTest(const config conf) {
    // encrypt first (otherwise decryption will fail)
    const unsigned char* input = random(conf.blockSize);
    const unsigned char* key = random(256);
    const unsigned char* iv = random(128);
    unsigned char encrypted[conf.blockSize*2];
    int encryptedLen = aes128Encrypt(input, conf.blockSize, encrypted, key, iv);
    // to store decrypted output
    unsigned char output[conf.blockSize*2];

    auto start = getTime();
    int numDecryptions = 0;
    while (true) {
        aes128Decrypt(encrypted, encryptedLen, output, key, iv);
        int elapsedTime = secondsSince(start);
        if (elapsedTime > conf.warmupSecs)
            numDecryptions++;
        if (elapsedTime > conf.warmupSecs + conf.runSecs)
            break;
    }

    std::cout << "Decrypted " << numDecryptions << " blocks of " << conf.blockSize << " bits in " << conf.runSecs << "s." << std::endl;
    std::cout << "Decryption throughput: " << numDecryptions / conf.runSecs << " /s." << std::endl;
}

config parseArgs(int argc, char* argv[]) {
    config conf = {
        .blockSize = DEFAULT_BLOCK_SIZE,
        .warmupSecs = DEFAULT_WARMUP,
        .runSecs = DEFAULT_RUN,
    };

    // set default values
    if (argc == 1) {
        conf.hash = true;
        conf.encrypt = true;
        conf.decrypt = true;
        std::cout << "Running all benchmarks with " << DEFAULT_BLOCK_SIZE << " bit blocks, " << DEFAULT_WARMUP
            << "s warmup, and " << DEFAULT_RUN << "s run time." << std::endl;
        return conf;
    }

    // Parse command line agruments
    int opt;
    while ((opt = getopt(argc, argv, "b:w:r:hed")) != -1) {
        switch (opt) {
            case 'b':
                conf.blockSize = atoi(optarg);
                break;
            case 'w':
                conf.warmupSecs = atoi(optarg);
                break;
            case 'r':
                conf.runSecs = atoi(optarg);
                break;
            case 'h':
                conf.hash = true;
                break;
            case 'e':
                conf.encrypt = true;
                break;
            case 'd':
                conf.decrypt = true;
                break;
            case '?':
                std::cout << "Usage: " << argv[0] << " [-b blockSize] [-w warmupSecs] [-r runSecs] [-h] [-e]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    return conf;
}

int main(int argc, char* argv[]) {
    config conf = parseArgs(argc, argv);
    // set up RNG to use RDRAND (safe on TEEs)
    ENGINE* eng = randomSetup();

    if (conf.hash)
        hashTest(conf);
    if (conf.encrypt)
        encryptionTest(conf);
    if (conf.decrypt)
        decryptionTest(conf);
    
    // clean up RNG
    randomCleanup(eng);
    return 0;
}
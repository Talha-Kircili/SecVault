#include "crypto.hpp"

using namespace std;

string ecall_sha(string data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha;
    SHA1_Init(&sha);
    SHA1_Update(&sha, data.c_str(), data.size());
    SHA1_Final(hash, &sha);
    return ocall_toHex(hash, SHA_DIGEST_LENGTH);
}

string ecall_sha256(string data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);
    return ocall_toHex(hash, SHA256_DIGEST_LENGTH);
}

sgx_status_t seal(uint8_t* plaintext, size_t plaintext_len, sgx_sealed_data_t* sealed_data, size_t sealed_size) {
    return status = sgx_seal_data(0, NULL, plaintext_len, plaintext, sealed_size, sealed_data);
}

sgx_status_t unseal(sgx_sealed_data_t* sealed_data, size_t sealed_size, uint8_t* plaintext, uint32_t plaintext_len) {
    return status = sgx_unseal_data(sealed_data, NULL, NULL, (uint8_t*)plaintext, &plaintext_len);
}

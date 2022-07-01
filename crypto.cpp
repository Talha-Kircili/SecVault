#include "crypto.hpp"

using namespace std;

void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

string sha(string data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha;
    SHA1_Init(&sha);
    SHA1_Update(&sha, data.c_str(), data.size());
    SHA1_Final(hash, &sha);
    return toHex(hash, SHA_DIGEST_LENGTH);
}

string sha256(string data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);
    return toHex(hash, SHA256_DIGEST_LENGTH);
}

int aes256_cbc_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len, ciphertext_len;
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int aes256_cbc_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len, plaintext_len;
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

tuple<string,string> hkdf(string secret) {
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char key[EVP_MAX_KEY_LENGTH];
    const EVP_CIPHER *cipher = EVP_aes_256_cbc();
    int ivlen = EVP_CIPHER_iv_length(cipher);
    int iklen = EVP_CIPHER_key_length(cipher);
    int iter = 1337;
    unsigned char salt[8] = {8};
    unsigned char keyivpair[iklen + ivlen]; 
    PKCS5_PBKDF2_HMAC((const char *)secret.c_str(), -1, salt, 0, iter, EVP_sha512(), iklen + ivlen, keyivpair);
    memcpy(key, keyivpair, iklen);
    memcpy(iv, keyivpair + iklen, ivlen);
    return {toHex(iv, ivlen), toHex(key, iklen)};
}

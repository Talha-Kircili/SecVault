#pragma once
/** librarys **/
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include "password.hpp"
#include "login.hpp"
#include "utils.hpp"
#include <string.h>
#include <iomanip>
#include <string>
#include <tuple>
/** func declarations **/
void handleErrors(void);
std::string sha(std::string data);
std::string sha256(std::string data);
std::tuple<std::string, std::string> hkdf();
int aes256_cbc_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int aes256_cbc_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);
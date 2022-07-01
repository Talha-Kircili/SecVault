#pragma once
/** librarys **/
#include <string>
#include <iomanip>
#include "string.h"
#include <string.h>
#include "enclave.h"
#include "utils.hpp"
#include "sealing.h"
#include "sgx_trts.h"
#include "enclave_t.h"
#include "sgx_tseal.h"
#include <openssl/err.h>
#include <openssl/sha.h>
/** func declarations **/
std::string ecall_sha(std::string data);
std::string ecall_sha256(std::string data);
sgx_status_t seal(uint8_t* plaintext, size_t plaintext_len, sgx_sealed_data_t* sealed_data, size_t sealed_size);
sgx_status_t unseal(sgx_sealed_data_t* sealed_data, size_t sealed_size, uint8_t* plaintext, uint32_t plaintext_len);

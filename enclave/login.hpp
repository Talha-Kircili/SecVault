#pragma once
/** librarys **/
#include <sstream>
#include <iostream>
#include "utils.hpp"
#include "crypto.hpp"
#include "storage.hpp"
#include "password.hpp"
/** function declarations  **/
void ecall_authenticate(void);
void ecall_change_masterkey(void);
void ecall_create_masterkey(void);
void ecall_recover_masterkey(void);

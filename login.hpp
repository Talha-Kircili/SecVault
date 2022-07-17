#pragma once
/** librarys **/
#include "password.hpp"
#include "storage.hpp"
#include "crypto.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <sstream>
/** function declarations  **/
void authenticate(void);
void change_masterkey(void);
void create_masterkey(void);
void recover_masterkey(void);
/** variables **/
extern std::string master_pwd;

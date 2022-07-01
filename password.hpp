#pragma once
/** librarys **/
#include <tuple>
#include <limits>
#include <string>
#include <sstream>
#include <iostream>
#include "utils.hpp"
#include "crypto.hpp"
#include "storage.hpp"
#include <curl/curl.h>
#include <bits/stdc++.h>
/** function declarations **/
void check_breach(void);
void handleErrors(void);
void remove_password(void);
void recover_masterkey(void);
std::string password_generator(int len);
void add_password(std::string domain="");
bool password_policy(std::string password);
void change_password(std::string account="");
std::string read_password(std::string account, bool output);

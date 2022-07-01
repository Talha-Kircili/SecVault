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
void ecall_check_breach(void);
void ecall_remove_password(void);
void ecall_recover_masterkey(void);
std::string ecall_password_generator(int len);
void ecall_add_password(std::string domain="");
bool ecall_password_policy(std::string password);
void ecall_change_password(std::string account="");
std::string ecall_read_password(std::string account, bool output);
size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp);

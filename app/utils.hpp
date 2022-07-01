#pragma once
/** librarys **/
#include <map>
#include <limits>
#include <sstream>
#include <iostream>
#include "storage.hpp"
/** function declarations **/
void ocall_art(void);
void ocall_sanitize(void);
void ocall_clear_console(int num);
void ocall_logger(std::string data);
void ocall_cout_string(const char *str);
std::string ocall_toHex(unsigned char* data, int len);
void ocall_printColored(std::string text, std::string color);

#pragma once
/** librarys **/
#include <map>
#include <limits>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "storage.hpp"
/** function declarations **/
void art(void);
void sanitize(void);
void clear_console(int num);
void logger(std::string data);
std::string toHex(unsigned char* data, int len);
void printColored(std::string text, std::string color);

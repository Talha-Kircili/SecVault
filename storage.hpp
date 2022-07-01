#pragma once
/** librarys **/
#include <string>
#include <fstream>
#include <iostream>
#include "utils.hpp"
/** function declarations **/
void remove_all(void);
int storage_size(void);
std::string get_account(void);
std::string* all_accounts(void);
void write_storage(std::string data, std::string filename="");
std::string read_storage(std::string domain, std::string filename);
std::string remove_line(std::string filename, std::string account);

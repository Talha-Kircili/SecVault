#pragma once
/** librarys **/
#include <string>
#include <fstream>
#include <iostream>
/** function declarations **/
void ocall_remove_all(void);
int ocall_storage_size(void);
std::string ocall_get_account(void);
std::string* ocall_all_accounts(void);
void ocall_write_storage(std::string data, std::string filename="");
std::string ocall_read_storage(std::string domain, std::string filename);
std::string ocall_remove_line(std::string filename, std::string account);

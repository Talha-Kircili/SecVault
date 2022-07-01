#include "login.hpp"

#define MAX_ATTEMPTS 3

using namespace std;

void ecall_recover_masterkey() {
    /** read seeds **/
    string temp, hash, input_seed, seed[5];
    for (int i=0; i<5; i++) {
        stringstream master(ocall_read_storage("seed" + to_string(i+1), "masterkey"));
        getline(master, temp, ':');
        getline(master, temp, ':');
        seed[i] = temp;
    }
    for (int attempt=1; attempt<=MAX_ATTEMPTS; attempt++) {
        ocall_cout_string("Enter recovery seed: ");
        cin >> input_seed;
        /** hash seed **/
        hash = ecall_sha256(input_seed);
        /** verify seed **/
        for (int i=0; i<5; i++) 
            if (!seed[i].compare(hash)) {
                ocall_logger("successfull recovery!");
                ocall_printColored("Access granted!\n", "green");
                ecall_change_masterkey();
                return;
            }
        ocall_logger("Wrong seed");
        ocall_cout_string("Wrong seed; Attemps left: " + MAX_ATTEMPTS-attempt + "\n\n");
    }
    ocall_printColored("Access denied!", "red");
    exit(0);
}

void ecall_authenticate() {
    char recover;
    string key, password, hash;
    /** read masterkey **/
    stringstream master(ocall_read_storage("master", "masterkey"));
    getline(master, key, ':');
    getline(master, key, ':');
    /** password authentication **/
    for (int attempt=1; attempt<=MAX_ATTEMPTS; attempt++) {
        ocall_cout_string("Enter Master-Key: ");
        cin >> password;
        /** hash password **/
        hash = ecall_sha256(password);
        /** verify password **/
        if (!key.compare(hash)) {
            ocall_clear_console(attempt*3);
            ocall_logger("successfull login!");
            ocall_printColored("Access granted!\n", "green");
            return ;
        }
        ocall_logger("Wrong password");
        ocall_cout_string("Wrong seed; Attemps left: " + MAX_ATTEMPTS-attempt + "\n\n");
    }
    /** recover masterkey **/
    ocall_cout_string("Do you want to enter a recovery seed?\n(Y) Yes (N) No\n");
    cin >> recover;
    if (tolower(recover) == 'y')
        ecall_recover_masterkey();
    else {
        ocall_clear_console(100);
        ocall_printColored("Access denied!", "red");
        exit(0);
    }
}

void ecall_create_masterkey() {
    stringstream temp;
    string key, seed;
    do {
        ocall_cout_string("Set Master-Key: ");
    cin >> key;
    } while (!ecall_password_policy(key));
    ocall_write_storage((const char*)"master:" + ecall_sha256(key), "masterkey");
    ocall_printColored("\nThe following 5 strings are your recovery seeds.\n"
        "These will only become displayed once, make sure to write them down and store them at a safe place.\n"
        "Recovery seeds can be used in order to recover the masterkey in case that you forget it!\n"
        , "yellow");
    for (int i=0; i<5; i++) {
        seed = ecall_password_generator(16);
        temp << "Seed " << to_string(i+1) << ": " << seed;
        ocall_printColored(temp.str(), "blue");
        temp.str("");
        ocall_write_storage((const char*)"seed" + to_string(i+1) + (const char*)":" + ecall_sha256(seed), "masterkey");
    }
    ocall_cout_string("\n");
}

void ecall_change_masterkey() {
    string confirm;
    ocall_cout_string("Are you sure that you want to reset your masterkey? (Y) Yes (N) No: ");
    cin >> confirm;
    if (confirm != "Yes") {
        ocall_cout_string("\n");
        return;
    }
    remove("masterkey");
    ecall_create_masterkey();
    ocall_cout_string("\nHave a wonderful day!👋");
    exit(0);
}

#include "login.hpp"

#define MAX_ATTEMPTS 3

using namespace std;

string master_pwd;

void recover_masterkey() {
    /** read seeds **/
    string temp, hash, input_seed, seed[5];
    for (int i=0; i<5; i++) {
        stringstream master(read_storage("seed" + to_string(i+1), "masterkey"));
        getline(master, temp, ':');
        getline(master, temp, ':');
        seed[i] = temp;
    }
    for (int attempt=1; attempt<=MAX_ATTEMPTS; attempt++) {
        cout << "Enter recovery seed: ";
        cin >> input_seed;
        /** hash seed **/
        hash = sha256(input_seed);
        /** verify seed **/
        for (int i=0; i<5; i++) 
            if (!seed[i].compare(hash)) {
                logger("successfull recovery!");
                printColored("Access granted!\n", "green");
                change_masterkey();
                return;
            }
        logger("Wrong seed");
        cout << "Wrong seed; Attemps left: " << MAX_ATTEMPTS-attempt << "\n\n";
    }
    printColored("Access denied!", "red");
    exit(0);
}

void authenticate() {
    char recover;
    string key, password, hash;
    /** read masterkey **/
    stringstream master(read_storage("master", "masterkey"));
    getline(master, key, ':');
    getline(master, key, ':');
    /** password authentication **/
    for (int attempt=1; attempt<=MAX_ATTEMPTS; attempt++) {
        cout << "Enter Master-Key: ";
        cin >> password;
        /** hash password **/
        hash = sha256(password);
        /** verify password **/
        if (!key.compare(hash)) {
            clear_console(attempt*3);
            logger("successfull login!");
            printColored("Access granted!\n", "green");
            master_pwd = password;
            return ;
        }
        logger("Wrong password");
        cout << "Wrong password; Attemps left: " << MAX_ATTEMPTS-attempt << "\n\n";
    }
    /** recover masterkey **/
    cout << "Do you want to enter a recovery seed?\n(Y) Yes (N) No\n";
    cin >> recover;
    if (tolower(recover) == 'y')
        recover_masterkey();
    else {
        clear_console(100);
        printColored("Access denied!", "red");
        exit(0);
    }
}

void create_masterkey() {
    stringstream temp;
    string key, seed;
    do {
    cout << "Set Master-Key: ";
    cin >> key;
    } while (!password_policy(key));
    master_pwd = key;
    write_storage((const char*)"master:" +sha256(key), "masterkey");
    printColored("\nThe following 5 strings are your recovery seeds.\n"
        "These will only become displayed once, make sure to write them down and store them at a safe place.\n"
        "Recovery seeds can be used in order to recover the masterkey in case that you forget it!\n"
        , "yellow");
    for (int i=0; i<5; i++) {
        seed = password_generator(16);
        temp << "Seed " << to_string(i+1) << ": " << seed;
        printColored(temp.str(), "blue");
        temp.str("");
        write_storage((const char*)"seed" + to_string(i+1) + (const char*)":" + sha256(seed), "masterkey");
    }
    hkdf();
    cout << endl;
}

void change_masterkey() {
    string confirm;
    cout << "Are you sure that you want to reset your masterkey? (Y) Yes (N) No: ";
    cin >> confirm;
    if (confirm != "Yes") {
        cout << endl;
        return;
    }
    remove("masterkey");
    create_masterkey();
    cout << "\nHave a wonderful day!👋";
    exit(0);
}

#include "storage.hpp"

using namespace std;

int ocall_storage_size() {
    int count=0;
    string line;
    ifstream storage("vault");
    while (getline(storage, line))
        count++;
    storage.close();
    return count;
}

string ocall_read_storage(string domain, string filename) {
    string line;
    string found;
    ifstream storage(filename);
    if (storage.good()) {
        while(getline(storage, line))
            if (line.substr(0, line.find(":")) == domain) {
                found = line;
                break;
            }
    }
    storage.close();
    return found;
}

void ocall_remove_all() {
    string confirm;
    cout << "Are you sure, that you want do delete everything? (Yes) or (No): ";
    cin >> confirm;
    if (confirm == ("Yes")) {
        remove("vault");
        remove("masterkey");
        cout << "\nGoodbye👋!";
        exit(0);
    }
}

string* ocall_all_accounts() {
    string line;
    int size = ocall_storage_size();
    string* accounts = new string[size];
    ifstream storage("vault");
    if (storage.good()) {
        for (int i=0; i<size; i++) {
            getline(storage, line);
            accounts[i] = line.substr(0, line.find(":"));
        }
    }
    storage.close();
    return accounts;
}

string ocall_get_account() {
    if (!ocall_storage_size()) {
        cout << "\nStorage is empty!\n\n";
        return "";
    }
    int choice;
    string *accs = ocall_all_accounts();
    for (int i=0; i<ocall_storage_size(); i++)
        cout << "(" << i << ") " << accs[i] << endl;
    cout << "\nChoose account: ";
    cin >> choice;
    ocall_sanitize();
    return accs[choice];
}

void ocall_write_storage(string data, string filename) {
    fstream storage;
    storage.open(filename, ios::app);
    storage << data << endl;
    storage.close();
}

string ocall_remove_line(string filename, string account) {
    string line, strReplace;
    string temp = filename + (const char*) ".temp";
    ifstream filein(filename); //File to read from
    ofstream fileout(temp); //Temporary file
    if (account.size())
        strReplace = account;
    else
        strReplace = ocall_get_account(); // Choose account to remove
    while(filein >> line)
    {
        if(line.substr(0, line.find(":")) == strReplace)
            continue;
        fileout << line << "\n";
    }
    remove(filename.c_str());
    rename(temp.c_str(), filename.c_str());
    return strReplace;
}

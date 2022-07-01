#include "password.hpp"

#define ERR_FAIL_SEAL 9
#define ERR_FAIL_UNSEAL 10

using namespace std;

bool ecall_password_policy(string password) {
    char restart;
    size_t special, lower, upper;
    int len = password.size();
    int token = 0;
    /** check length **/
    if (len < 8) {
        ocall_printColored("Passwort too short!\n", "red");
        token++;
    } else if (len > 20) {
        ocall_printColored("Passwort too long!\n", "red");
        token++;
    }
    /** check numbers **/
    if (!any_of(password.begin(), password.end(), ::isdigit)) {
        ocall_printColored("Passwort doesn´t have any numbers!\n", "red");
        token++;
    }
    /** check small letters **/
    if (!any_of(password.begin(), password.end(), ::islower)) {
        ocall_printColored("Passwort doesn´t have any Lowercase-Letter!\n", "red");
        token++;
    }
    /** check big letters **/
    if (!any_of(password.begin(), password.end(), ::isupper)) {
        ocall_printColored("Passwort doesn´t have any Uppercase-Letter!\n", "red");
        token++;
    }
    /** check special chars **/
    if (!any_of(password.begin(), password.end(), ::ispunct)) {
        ocall_printColored("Passwort doesn´t have any Special-Character!\n", "red");
        token++;
    }
    /** strength meter **/
    if (token == 0) {
        ocall_printColored("Very strong password!", "green");
        return true;
    } else if (token == 1) {
        ocall_printColored("Moderate password!", "yellow");
        ocall_cout_string("Do you want to change it: (Y) Yes, (N) No: ");
        cin >> restart;
        if (tolower(restart) == 'y')
            return false;
        return true;
    } else {
        ocall_printColored("Insecure password!\n", "red");
        return false;
    }
    return true;
}

void ecall_add_password(string domain) {
    int choice;
    string username, password;
    /** get data **/
    if (!domain.size()) {
        ocall_cout_string("Enter domain name: ");
        cin >> domain;
    }
    domain.append(":");
    ocall_cout_string("Enter username: ");
    cin >> username;
    ocall_cout_string("\n(0) enter password\n(1) generate password\n\nChoose type of password: ");
    cin >> choice;
    ocall_sanitize();
    cout << endl;
    if (choice) {
        int pass_length;
        do{
            ocall_cout_string("Choose password length between 8-20 characters: ");
            cin >> pass_length;
            ocall_sanitize();
            ocall_cout_string("\n");
        } while(pass_length < 8 || pass_length > 20);
        password = ecall_password_generator(pass_length);
        ocall_cout_string("Generated password: " + password + "\n\n");
    }
    else {
        ocall_printColored("Note: A strong passwort contains each of the following characters atleast once:\n"
            "* Password length 8-20 characters\n"
            "* Numbers\n"
            "* Lower- and Uppercase Letters\n"
            "* Special-Characters: !\"#$%&'()*+,-./:;<?=@>[\\]^_`{|}~\n", "yellow");
        do {
            ocall_cout_string("\nEnter password: ");
            cin >> password;
        } while(!ecall_password_policy(password));
    }
    /** seal data **/
    sgx_status_t sealing_status;
    string account = username + ":" + password;
    size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(account);
	uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);
    sealing_status = seal(account, (sgx_sealed_data_t*)sealed_data, sealed_size);
    free(account);
    if (sealing_status != SGX_SUCCESS) {
		free(sealed_data);
		return ERR_FAIL_SEAL;
    }
    ecall_write_storage(domain + ":" + sealed_data, "vault");
}

string ecall_read_password(string account, bool output) {
    if (!account.size())
        return "";
    string domain = ocall_read_storage(account, "vault");
    /** read cipher **/
    stringstream ss(domain);
    string cipher, username, password;
    getline(ss, domain, ':');
    getline(ss, cipher, ':');
    /** unseal data **/
    sgx_status_t sealing_status;
    uint32_t plaintext_size = 128;
    string unsealed_data = (const char*)malloc(plaintext_size);
    sealing_status = unseal((sgx_sealed_data_t*)cipher, unsealed_data, plaintext_size);
    free(cipher);
    if (sealing_status != SGX_SUCCESS) {
		free(unsealed_data);
		return ERR_FAIL_UNSEAL;
    }
    /** read username & password **/
    stringstream dd((const char*)unsealed_data);
    getline(dd, username, ':');
    getline(dd, password, ':');
    if (output)
        ocall_cout_string("\nDomain: " + domain + "\nUsername: " + username + "\nPassword: " + password + "\n\n");
    return password;
}

void ecall_change_password(string account) {
    ecall_add_password(ocall_remove_line("vault", account));
}

string password_generator(int len) {
    char rand[len];
    string password = "";
    unsigned long long int random_value;
    const char numbers[] = "0123456789";
    const char letter[] = "abcdefghijklmnopqrstuvwxyz";
    const char LETTER[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char symbols[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    ifstream random("/dev/urandom", ios::in|ios::binary);
    for (int i=0; i<len; i++) {
        random.read(reinterpret_cast<char*>(&random_value), 20); 
        switch(random_value%4) {
            case 0:
                password += letter[random_value % 26];
                break;
            case 1:
                password += numbers[random_value % 10];
                break;
            case 2:
                password += symbols[random_value % 8];
                break;
            case 3:
                password += LETTER[random_value % 26];
                break;
        }
    }
    random.close();
    return password;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void ecall_check_breach() {
    char confirm;
    long response_code;
    string response, password, hash, url;
    string *account = ocall_all_accounts();
    for (int i=0; i<ocall_storage_size(); i++) {
        password = ecall_read_password(account[i], false);
        hash = ecall_sha(password);
        url = "https://api.pwnedpasswords.com/range/";
        url.append(hash.substr(0, 5));
        hash = hash.substr(5, 50);
        CURL* curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);        
            curl_easy_cleanup(curl);
            curl = NULL;
        }
        if (response_code == 200) {
            size_t pos = 0;
            string token;
            while ((pos = response.find("\n")) != string::npos) {
                token = response.substr(0, pos);
                stringstream ss(token);
                getline(ss, token, ':');
                if (!strcasecmp(token.c_str(), hash.c_str())) {
                    getline(ss, token, ':');
                    ocall_cout_string("'" + password + "' from '" + account[i] + "' has been seen " + stoi(token) + " times before!\n\n");
                    ocall_cout_string("Do you want to change this password? (Y) (N): ");
                    cin >> confirm;
                    if (tolower(confirm) == 'y')
                        ecall_change_password(account[i]);
                    return;
                }
                response.erase(0, pos + 1);
            }
        }
    }
}

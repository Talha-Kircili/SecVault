#include "password.hpp"

using namespace std;

bool password_policy(string password) {
    char restart;
    size_t special, lower, upper;
    int len = password.size();
    int token = 0;
    /** check length **/
    if (len < 8) {
        printColored("Passwort too short!\n", "red");
        token++;
    } else if (len > 20) {
        printColored("Passwort too long!\n", "red");
        token++;
    }
    /** check numbers **/
    if (!any_of(password.begin(), password.end(), ::isdigit)) {
        printColored("Passwort doesn´t have any numbers!\n", "red");
        token++;
    }
    /** check small letters **/
    if (!any_of(password.begin(), password.end(), ::islower)) {
        printColored("Passwort doesn´t have any Lowercase-Letter!\n", "red");
        token++;
    }
    /** check big letters **/
    if (!any_of(password.begin(), password.end(), ::isupper)) {
        printColored("Passwort doesn´t have any Uppercase-Letter!\n", "red");
        token++;
    }
    /** check special chars **/
    if (!any_of(password.begin(), password.end(), ::ispunct)) {
        printColored("Passwort doesn´t have any Special-Character!\n", "red");
        token++;
    }
    /** strength meter **/
    if (token == 0) {
        printColored("Very strong password!", "green");
        return true;
    } else if (token == 1) {
        printColored("Moderate password!", "yellow");
        cout << "Do you want to change it: (Y) Yes, (N) No: ";
        cin >> restart;
        if (tolower(restart) == 'y')
            return false;
        return true;
    } else {
        printColored("Insecure password!\n", "red");
        return false;
    }
    return true;
}

void add_password(string domain) {
    int choice;
    string username, password;
    /** get data **/
    if (!domain.size()) {
        cout << "Enter domain name: ";
        cin >> domain;
    }
    domain.append(":");
    cout << "Enter username: ";
    cin >> username;
    cout << "\n(0) enter password\n(1) generate password\n\nChoose type of password: ";
    cin >> choice;
    sanitize();
    cout << endl;
    if (choice) {
        int pass_length;
        do{
            cout << "Choose password length between 8-20 characters: ";
            cin >> pass_length;
            sanitize();
            cout << endl;
        } while(pass_length < 8 || pass_length > 20);
        password = password_generator(pass_length);
        cout << "Generated password: " << password << "\n\n";
    }
    else {
        printColored("Note: A strong passwort contains each of the following characters atleast once:\n"
            "* Password length 8-20 characters\n"
            "* Numbers\n"
            "* Lower- and Uppercase Letters\n"
            "* Special-Characters: !\"#$%&'()*+,-./:;<?=@>[\\]^_`{|}~\n", "yellow");
        do {
            cout << "\nEnter password: ";
            cin >> password;
        } while(!password_policy(password));
    }
    /** encrypt data **/
    int ciphertext_len;
    unsigned char ciphertext[128];
    string result = username + ":" + password;
    auto [ivv, keyy] = hkdf();
    unsigned char *plaintext = (unsigned char *)result.c_str();
    unsigned char *key = (unsigned char *)keyy.c_str();
    unsigned char *iv = (unsigned char *)ivv.c_str();
    ciphertext_len = aes256_cbc_encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext);
    write_storage(domain + (const char*)iv + ":"  + toHex(ciphertext, ciphertext_len), "vault");
}

string read_password(string account, bool output) {
    int decryptedtext_len;
    unsigned char decryptedtext[128];
    if (!account.size())
        return "";
    string domain = read_storage(account, "vault");
    /** read iv & cipher **/
    stringstream ss(domain);
    string cipher, iv_str, username, password;
    getline(ss, domain, ':');
    getline(ss, iv_str, ':');
    getline(ss, cipher, ':');
    /** account information **/
    int len = cipher.length();
    string newString;
    for(int i=0; i<len; i+=2)
    {
        string byte = cipher.substr(i,2);
        char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    unsigned char *ciphertext = (unsigned char *)newString.c_str();
    /** decrypt ciphertext **/
    auto [ivv, keyy] = hkdf();
    unsigned char *iv = (unsigned char *)iv_str.c_str();
    unsigned char *key = (unsigned char *)keyy.c_str();
    decryptedtext_len = aes256_cbc_decrypt(ciphertext, len/2, key, iv, decryptedtext);
    decryptedtext[decryptedtext_len] = '\0';
    /** read username & password **/
    stringstream dd((const char*)decryptedtext);
    getline(dd, username, ':');
    getline(dd, password, ':');
    if (output)
        cout << "\nDomain: " << domain << "\nUsername: " << username << "\nPassword: " << password << "\n\n";
    return password;
}

void change_password(string account) {
    add_password(remove_line("vault", account));
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

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void check_breach() {
    char confirm;
    long response_code;
    string response, password, hash, url;
    string *account = all_accounts();
    for (int i=0; i<storage_size(); i++) {
        password = read_password(account[i], false);
        hash = sha(password);
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
                    cout << "'" << password << "' from '" + account[i] + "' has been seen " << stoi(token) << " times before!\n\n";
                    cout << "Do you want to change this password? (Y) (N): ";
                    cin >> confirm;
                    if (tolower(confirm) == 'y')
                        change_password(account[i]);
                    return;
                }
                response.erase(0, pos + 1);
            }
        }
    }
}
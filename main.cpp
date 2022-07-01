#include "main.hpp"

using namespace std;

int main()
{
    logger("programm started");
    int choice;
    ifstream f("masterkey");
    /** ascii art **/
    clear_console(100);
    art();
    /** check if 'masterkey' exist **/
    if (!f.good())
        create_masterkey();
    else
        authenticate();
    if (storage_size())
        check_breach();
    while (true) {
        /** choose operation **/
        cout << "(0) Exit" << endl;
        cout << "(1) Generate password" << endl;
        cout << "(2) Add new account" << endl;
        cout << "(3) Change password" << endl;
        cout << "(4) Remove account" << endl;
        cout << "(5) Read password" << endl;
        cout << "(6) Change masterkey" << endl;
        cout << "(7) Clear console" << endl;
        printColored("(8) Reset everything", "red");
        cout << "\nChoose an operation: ";
        cin >> choice;
        sanitize();
        /** methods of operations **/
        switch(choice) {
            case 0:
                logger("programm exited");
                cout << "Have a wonderful day!👋";
                return 0;
            case 1:
                int key_len;
                cout << "Enter password length: ";
                cin >> key_len;
                sanitize();
                cout << "Generated Password: " << password_generator(key_len) << "\n\n";
                break;
            case 2:
                logger("added password");
                add_password();
                break;
            case 3:
                logger("changed password");
                change_password();
                break;
            case 4:
                logger("deleted password");
                remove_line("vault", "");
                break;
            case 5:
                logger("read password");
                read_password(get_account(), true);
                break;
            case 6:
                logger("changed masterkey");
                change_masterkey();
                break;
            case 7:
                clear_console(200);
                art();
                break;
            case 8:
                logger("deleted everything");
                remove_all();
                break;
        }
    }
}

#include "utils.hpp"

using namespace std;

string toHex(unsigned char* data, int len) {
    stringstream stream;
    for(int i = 0; i < len; i++)
        stream << hex << setw(2) << setfill('0') << (int)data[i];
    return stream.str();
}

void clear_console(int num) {
    cout << "\x1b[2K";
    for (int i=0; i<num; i++)
        cout << "\x1b[1A" << "\x1b[2K";
    cout << "\r";
}

void art() {
    ifstream art("art");
    if (art.is_open())
        cout << art.rdbuf();
}

void printColored(string text, string color) {
    map<string, int> colors = {{"red", 1}, {"green", 2 }, {"yellow", 3}, {"blue", 4}};
    cout << "\x1B[3" + to_string(colors[color]) + "m"  + text + "\033[0m" << endl;
}

void sanitize() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void logger(string data) {
    time_t rawtime;
    char buffer[50];
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 50, "%d.%m.%y %R ", timeinfo);
    write_storage((const char*) buffer + data + (const char*)"\n", "access.log");
}

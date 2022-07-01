#include "enclave_u.h"
#include "sgx_urts.h"
#include <cstring>
#include <fstream>
#include <getopt.h>
#include "enclave.h"
#include "main.hpp"
#include <stdio.h>
#include <string.h>
#include <assert.h>
# include <unistd.h>
# include <pwd.h>

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
    {
        SGX_ERROR_MEMORY_MAP_FAILURE,
        "Failed to reserve memory for the enclave.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave("enclave.signed.so", SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }
    return 0;
}

using namespace std;

int main()
{
    sgx_enclave_id_t eid = 0;
    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }
    ocall_logger("programm started");
    int choice;
    ifstream f("masterkey");
    /** ascii art **/
    ocall_clear_console(100);
    ocall_art();
    /** check if 'masterkey' exist **/
    if (!f.good())
        ecall_create_masterkey(eid);
    else
        ecall_authenticate(eid);
    if (storage_size())
        ecall_check_breach(eid);
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
        ocall_sanitize();
        /** methods of operations **/
        switch(choice) {
            case 0:
                ocall_logger("programm exited");
                cout << "Have a wonderful day!👋";
                return 0;
            case 1:
                int key_len;
                cout << "Enter password length: ";
                cin >> key_len;
                ocall_sanitize();
                cout << "Generated Password: " << ecall_password_generator(eid, key_len) << "\n\n";
                break;
            case 2:
                ocall_logger("added password");
                ecall_add_password(eid);
                break;
            case 3:
                ocall_logger("changed password");
                ecall_change_password(eid);
                break;
            case 4:
                ocall_logger("deleted password");
                ocall_remove_line("vault", "");
                break;
            case 5:
                ocall_logger("read password");
                ecall_read_password(eid, ocall_get_account(), true);
                break;
            case 6:
                ocall_logger("changed masterkey");
                ecall_change_masterkey(eid);
                break;
            case 7:
                ocall_clear_console(200);
                ocall_art();
                break;
            case 8:
                ocall_logger("deleted everything");
                ocall_remove_all();
                break;
        }
    }
    /* Destroy the enclave */
    sgx_destroy_enclave(eid);
    printf("Info: SampleEnclave successfully returned.\n");
    return 0;
}

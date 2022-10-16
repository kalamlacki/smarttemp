//#define GETSTATUSJS "{\"system\":{\"get_sysinfo\":{}}}"
#define GETSTATUSJS  "{\"system\": {\"get_sysinfo\": null}}"
#define SETSTATUSONJS "{\"system\": {\"set_relay_state\": {\"state\": 1}}}"
#define SETSTATUSOFFJS "{\"system\": {\"set_relay_state\": {\"state\": 0}}}"

int readHS110Status(const char* url);
int writeHS110Status(const char* url, int status);
unsigned int xorEncrypt(const unsigned char* message, char* cryptogram, char code);
char* xorDecrypt(const unsigned char* cryptogram, char* message, unsigned int size);

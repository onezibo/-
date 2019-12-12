#include "config.h"

char *FILE_WORKING_DIR = "./files";
char *LISTENING_ADDRESS = "0.0.0.0";
ev_int16_t LISTENING_PORT = 80;
int CHUNK_SIZE = 64 * 1024; // 64KB
int DOWNLOAD_FILE_CHUNK_OPTION = 1;
char *CERTIFICATE_CHAIN_FILE_NAME = "server-certificate-chain.pem";
char *PRIVATE_KEY_FILE_NAME = "server-private-key.pem";
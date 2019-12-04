#include "config.h"

char FILE_WORKING_DIR[100] = "./files";
char LISTENING_ADDRESS[100] = "0.0.0.0";
ev_int16_t LISTENING_PORT = 80;
int CHUNK_SIZE = 64 * 1024; // 64KB
int DOWNLOAD_FILE_CHUNK_OPTION = 1;
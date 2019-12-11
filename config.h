#include <event2/event.h>

#ifndef CONFIG_H
#define CONFIG_H
extern char *FILE_WORKING_DIR;
extern char *LISTENING_ADDRESS;
extern ev_int16_t LISTENING_PORT;
extern int CHUNK_SIZE;
extern int DOWNLOAD_FILE_CHUNK_OPTION;
extern char *CERTIFICATE_CHAIN_FILE_NAME;
extern char *PRIVATE_KEY_FILE_NAME;
#endif
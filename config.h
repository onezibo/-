#include <event2/event.h>

#ifndef CONFIG_H
#define CONFIG_H
extern char FILE_WORKING_DIR[100];
extern char LISTENING_ADDRESS[100];
extern ev_int16_t LISTENING_PORT;
extern int CHUNK_SIZE;
extern int DOWNLOAD_FILE_CHUNK_OPTION;
#endif
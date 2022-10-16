#define PY_SSIZE_T_CLEAN
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <Python.h>

#include "tuya.h"

const char *commonScript = "import tinytuya\n"
"d = tinytuya.OutletDevice(\'%s\', \'%s\', \'%s\')\n"
"d.set_version(3.3)\n";

const char* getStatus = "data = d.status()\n"
"print(int(data[\'dps\'][\'1\']))\n";

const char* powerOn = "d.turn_on()\n";

const char* powerOff = "d.turn_off()\n";


char tuya_buff[1024];

int runInFork(int* mpipe, const char* theScript) {
    if(fork() == 0)  {
       if(mpipe != 0) {
          close(STDOUT_FILENO);  
          dup(mpipe[1]);       
          close(mpipe[0]);     
          close(mpipe[1]);
       }
       Py_Initialize();
       PyRun_SimpleString(theScript);
       if (Py_FinalizeEx() < 0) {
          return -1;
       }
       exit(-1);
    }
    return 0;
}

void constructScript(const char* url, const char* credentials, const char* addition) {
    char ID[256], password[256];
    const char* spacePtr = strchr(credentials, ' ');
    strncpy(ID, credentials, spacePtr - credentials);
    ID[spacePtr - credentials] = 0;
    strcpy(password, spacePtr + 1);
    
    sprintf(tuya_buff, commonScript, ID, url, password);
    
    strcat(tuya_buff, addition);
    
}

int readTuyaStatus(const char* url, const char* credentials) {
    char status_buff[256];
    status_buff[0] = 0;
    constructScript(url, credentials, getStatus);
    int mpipe[2];
    if(pipe(mpipe) == -1) {
      perror("Pipe1  failed");
      exit(1);
    }
    runInFork(mpipe, tuya_buff);
    FILE* fpyout = fdopen(mpipe[0], "rt");
    fgets(status_buff, 256, fpyout);
    close(mpipe[0]);
    close(mpipe[1]);
    //printf("buffer=%s\n", status_buff);
    sleep(1);
    return atoi(status_buff);
}


int writeTuyaStatus(const char* url, const char* credentials, int status) {
    constructScript(url, credentials, status == 1 ? powerOn : powerOff);
    runInFork(0, tuya_buff);
    sleep(1);
    return 0;
}


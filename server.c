#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <regex.h>
#include <ctype.h>
#include<stdbool.h>
#include <math.h>

#define BUFSZ 1024
#define BUFSZ2 500

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

typedef struct{
    double velocity;
    double acceleration;
    double pressure; 
    double current;
} processVariables;

typedef struct{
    int equipmentId;
    float sensors[4][2];
    
}equipment;


bool existSensor(equipment* eq, int snsr, int numEq, int sensor){
      
        for (int j = 0; j < 4; j++){
            if(eq[numEq].sensors[j][0] == sensor){
                return true;
            }
        }    
 return false;
}

void generateRandom(equipment *eq, int numEq, int positionSnsr){
    float a = 10.00;
    float readValue = (float)rand()/(float)(RAND_MAX) * a;
    eq[numEq].sensors[positionSnsr][1] = readValue;
}

char* addSensor(char *buff, equipment *eq, int *numSensors, char *strRetr){

    const char str[] = "in";
    char *ptr;
    int snsr = 0;
    int machn = 0;
    int sensors[3];
    int machines[4];
    bool evaluateSns = false;
    bool evaluateMchn = false;
    char aux[1024];

   /*  BLOCK TO EXTRACT WHAT SENSORS AND WHAT MACHINES ONE WANTS TO CHANGE */

    while (*buff){
        ptr = strstr(buff, str);
        if (ptr != NULL && snsr != 4){
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10);
                if ((int)val > 0 && (int)val < 5){
                    sensors[snsr] = (int)val;
                    snsr++;
                    evaluateSns = true;
                }else{
                    sprintf(strRetr, "invalid sensor\n");
                    return strRetr;
                }
            }
            else{
                buff++;
            }
        }
        else{
            if (ptr != NULL && snsr == 4){
                sprintf(strRetr,"You must declare until 3 sensors in one command line.\n");
                return strRetr;
            }
            else{
                if (ptr == NULL){
                    if (isdigit(*buff) && machn <= 3 && evaluateSns){
                        long val = strtol(buff, &buff, 10);
                        if ((int)val > 0 && (int)val < 5){
                            machines[machn] = (int)val;
                            machn++;
                            evaluateMchn = true;
                        }else{
                            sprintf(strRetr, "invalid equipment\n");
                            return strRetr;
                            
                        }
                    }
                    else{
                        buff++;
                    }
                }
            }
        }
    }

    if (evaluateSns && !evaluateMchn){
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }
    if (!evaluateMchn){
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }

    for (int numEq = 0; numEq < 4; numEq++){
        int numMachnChange = 0;
        while (numMachnChange < machn){
            if (eq[numEq].equipmentId == machines[numMachnChange]){
                for (int h = 0; h < snsr; h++){
                    for (int j = 0; j < 4; j++){
                        bool exist = existSensor(eq, snsr, numEq, sensors[h]);
                        if ((eq[numEq].sensors[j][0] == 0) && !exist){
                            if(*numSensors < 15){
                                eq[numEq].sensors[j][0] = sensors[h];
                                generateRandom(eq, numEq, j);
                                sprintf(aux, "Sensor %d added\n", sensors[h]);
                                strcat(strRetr, aux);
                                *numSensors = *numSensors + 1;
                                break;
                            }else{
                                sprintf(aux, "limit exceeded\n");
                                strcat(strRetr, aux);
                                break;
                            }
                        }
                        else if (exist){
                            sprintf(aux, "Sensor %d already exist in %d\n", sensors[h], eq[numEq].equipmentId);
                            strcat(strRetr, aux);
                            break;
                        }
                    }
                }
            }
            numMachnChange++;
        }
    }
    return strRetr;
    
}

char* listSensors(char *buff, equipment *eq, char *strRetr){
    const char str[] = "in";
    char *ptr;
    bool evaluate = false;
    char aux[1024];
    
    while (*buff){
        ptr = strstr(buff, str);
        if ((ptr == NULL)){
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10);
                if (((int)val > 0 && (int)val < 5) && (!evaluate)){
                    for (int i = 0; i < 4; i++){
                        if (eq[i].equipmentId == (int)val){
                            for (int j = 0; j < 4; j++){
                                
                                if (eq[i].sensors[j][0] != 0){
                                    sprintf(aux, "%d ", (int)eq[i].sensors[j][0]);
                                    strcat(strRetr, aux);
                                }
                                else{
                                    sprintf(aux, "none ");
                                    strcat(strRetr, aux);
                                }
                            }
                        }
                    }
                    evaluate = true;
                }
                else{
                    exit(EXIT_FAILURE);
                }
            }
            else{
                buff++;
            }
        }
        else{
            buff++;
        }
    }
    if(!evaluate){
        exit(EXIT_FAILURE);
    }
    return strRetr;
}


char* removeSensor(char *buff, equipment *eq, int *numSensors, char *strRetr){

    const char str[] = "in";
    char *ptr;
    int snsr = 0;
    int machn = 0;
    int sensors[3];
    int machines[4];
    bool evaluateSns = false;
    bool evaluateMchn = false;
    char aux[1024];
    
    while (*buff){
        ptr = strstr(buff, str);
        if (ptr != NULL && snsr != 4){
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10);
                if ((int)val > 0 && (int)val < 5){
                    sensors[snsr] = (int)val;
                    snsr++;
                    evaluateSns = true;
                }else{
                    sprintf(strRetr, "invalid sensor\n");
                    return strRetr;
                    
                }
            }
            else{
                buff++;
            }
        }
        else{
            if (ptr != NULL && snsr == 4){
                sprintf(strRetr, "You must declare until 3 sensors in one command line.\n");
                return strRetr;
            }
            else{
                if (ptr == NULL){
                    if (isdigit(*buff) && machn <= 3 && evaluateSns){
                        long val = strtol(buff, &buff, 10);
                        if ((int)val > 0 && (int)val < 5){
                            machines[machn] = (int)val;
                            machn++;
                            evaluateMchn = true;
                        }else{
                            sprintf(strRetr, "invalid equipment\n");
                            return strRetr;
                            
                        }
                    }
                    else{
                        buff++;
                    }
                }
            }
        }
    }

    if (evaluateSns && !evaluateMchn){
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }
    if (!evaluateMchn){
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }
    
    for (int numEq = 0; numEq < 4; numEq++){
        int numMachnChange = 0;
        while (numMachnChange < machn){
            if (eq[numEq].equipmentId == machines[numMachnChange]){
                for (int h = 0; h < snsr; h++){
                    for (int j = 0; j < 4; j++){
                        bool exist = existSensor(eq, snsr, numEq, sensors[h]);
                        if (exist){
                            eq[numEq].sensors[j][0] = 0;
                            sprintf(aux, "Sensor %d removed\n", sensors[h]);
                            strcat(strRetr, aux);
                            *numSensors = *numSensors - 1;
                            break;
                        }
                        else if (!exist){
                            sprintf(aux, "sensor %d does not exist in equipment %d\n", sensors[h], eq[numEq].equipmentId);
                            strcat(strRetr, aux);
                            break;
                        }
                    }
                }
            }
            numMachnChange++;
        }
    }
    return strRetr;
}

char* consultVariables(char *buff, equipment *eq, char *strRetr){

    const char str[] = "in";
    char *ptr;
    int snsr = 0;
    int machn = 0;
    int sensors[3];
    int machines[4];
    bool evaluateSns = false;
    bool evaluateMchn = false;
    char aux[1024];
   
    while (*buff){
        ptr = strstr(buff, str);
        if (ptr != NULL && snsr != 4){
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10);
                if ((int)val > 0 && (int)val < 5){
                    sensors[snsr] = (int)val;
                    snsr++;
                    evaluateSns = true;
                }else{
                    sprintf(strRetr, "invalid sensor\n");
                    return strRetr;
                    
                }
            }
            else{
                buff++;
            }
        }
        else{
            if (ptr != NULL && snsr == 4){
                sprintf(strRetr, "You must declare until 3 sensors in one command line.\n");
                return strRetr;
            }
            else{
                if (ptr == NULL){
                    if (isdigit(*buff) && machn <= 3 && evaluateSns){
                        long val = strtol(buff, &buff, 10);
                        if ((int)val > 0 && (int)val < 5){
                            machines[machn] = (int)val;
                            machn++;
                            evaluateMchn = true;
                        }else{
                            sprintf(strRetr, "invalid equipment\n");
                            return strRetr;
                        }
                    }
                    else{
                        buff++;
                    }
                }
            }
        }
    }

    if (evaluateSns && !evaluateMchn){
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }
    if (!evaluateMchn){
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }
    bool installed = true;
    for (int numEq = 0; numEq < 4; numEq++){
        int numMachnChange = 0;
        
        while (numMachnChange < machn){
            if (eq[numEq].equipmentId == machines[numMachnChange]){
                for (int h = 0; h < snsr; h++){
                    for (int j = 0; j < 4; j++){
                        if ((eq[numEq].sensors[j][0] != 0) && (eq[numEq].sensors[j][0] == sensors[h])){
                            sprintf(aux, "%.2f ", eq[numEq].sensors[j][1]);
                            strcat(strRetr, aux);
                            break;
                        }
                        else if (eq[numEq].sensors[j][0] == 0){
                            sprintf(aux, " %d ", sensors[h]);
                            strcat(strRetr, aux);
                            installed = false;
                            break;
                        }
                    }
                }
                if(!installed){
                    sprintf(aux, " not installed in %d\n", eq[numEq].equipmentId);
                    strcat(strRetr, aux);
                }
                                
            }
            numMachnChange++;
        }
    }
    return strRetr;
}



int main(int argc, char **argv) {
     
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    equipment esteira = {01, {{0,0},{0,0},{0,0},{0,0}}};
   
    equipment guindaste = {02, {{0,0},{0,0},{0,0},{0,0}}};
    
    equipment ponteRolante = {03, {{0,0},{0,0},{0,0},{0,0}}};
   
    equipment empilhadeira = {04, {{0,0},{0,0},{0,0},{0,0}}};

    equipment equipments[4];

    equipments[0] = esteira;
    equipments[1] = guindaste;
    equipments[2] = ponteRolante;
    equipments[3] = empilhadeira;  

    int numSensors = 0;
    char bufRet[BUFSZ2];
    memset(bufRet, 0, BUFSZ2);

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1){
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(csock, buf, BUFSZ - 1, 0);

        if (((int)count) == 0){
            printf("Received %d bytes. \n", (int)count);
        }
        else{

            /* ADD SENSOR BLOCK */

            regex_t regex1;
            int verifyRegx1 = regcomp(&regex1, "add sensor", REG_EXTENDED);

            if (verifyRegx1 != 0){
                logexit("Regex error compilation. \n");
            }
            else{
                int matchRegx = regexec(&regex1, buf, 0, NULL, 0);
                if (matchRegx == 0){
                    if(numSensors < 15){
                        memset(bufRet, 0, BUFSZ2);
                        addSensor(buf, equipments, &numSensors, bufRet);
                        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                    }else{
                        sprintf( bufRet, "limit exceeded\n");
                    }
                }
                else{

                    /*  LIST SENSORS BLOCK */

                    verifyRegx1 = regcomp(&regex1, "list sensors", REG_EXTENDED);
                    if (verifyRegx1 != 0){
                        logexit("Regex error compilation. \n");
                    }
                    else{
                        matchRegx = regexec(&regex1, buf, 0, NULL, 0);
                        if (matchRegx == 0){
                            memset(bufRet, 0, BUFSZ2);
                            listSensors(buf, equipments, bufRet);
                            printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                        }
                        else{

                            /*  KILL BLOCK */

                            verifyRegx1 = regcomp(&regex1, "kill", REG_EXTENDED);
                            if (verifyRegx1 != 0){
                                logexit("Regex error compilation. \n");
                            }
                            else{
                                matchRegx = regexec(&regex1, buf, 0, NULL, 0);
                                if (matchRegx == 0){
                                    printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                                    break;
                                }
                                else{
                                    verifyRegx1 = regcomp(&regex1, "remove sensor", REG_EXTENDED);
                                    if (verifyRegx1 != 0){
                                        logexit("Regex error compilation. \n");
                                    }
                                    else{
                                        matchRegx = regexec(&regex1, buf, 0, NULL, 0);
                                        if (matchRegx == 0){
                                            memset(bufRet, 0, BUFSZ2);
                                            removeSensor(buf, equipments, &numSensors, bufRet);
                                            printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                                        }
                                        else{

                                            /* READ BLOCK */
                                            verifyRegx1 = regcomp(&regex1, "read", REG_EXTENDED);
                                            if (verifyRegx1 != 0){
                                                logexit("Regex error compilation. \n");
                                            }
                                            else{
                                                matchRegx = regexec(&regex1, buf, 0, NULL, 0);
                                                if (matchRegx == 0){
                                                    memset(bufRet, 0, BUFSZ2);
                                                    consultVariables(buf, equipments, bufRet);
                                                    printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                                                    
                                                }
                                                else{
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);

            count = send(csock, bufRet, strlen(bufRet) + 1, 0);
            if (count != strlen(bufRet) + 1){
                logexit("send");
            }
            close(csock);
        }
    }
        exit(EXIT_SUCCESS);
    
}

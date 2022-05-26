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

#define BUFSZ 1024

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
    int sensors[4];
    processVariables variables[4];
}equipment;

void addSensor(int equipmentId, int sensorId, equipment* eq){
    for (int i = 0; i <= 3; i++) {
        //printf("%d\n", eq[i].sensors[i]);
    }
}

bool existSensor(equipment* eq, int snsr, int numEq, int sensor){
      
        for (int j = 0; j < 4; j++){
            if(eq[numEq].sensors[j] == sensor){
                
                printf("Sensor %d already exist in %d\n", sensor, eq[numEq].equipmentId);
                return true;
            }
        }    
 return false;
}

equipment *extractNumber(char *buff, equipment *eq){

    const char str[] = "in";
    char *ptr;
    int snsr = 0;
    int machn = 0;
    int sensors[3];
    int machines[4];
    bool evaluateSns = false;
    bool evaluateMchn = false;

    while (*buff){
        ptr = strstr(buff, str);
        if (ptr != NULL && snsr != 4){
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10);
                if ((int)val > 0 && (int)val < 5){
                    sensors[snsr] = (int)val;
                    snsr++;
                    evaluateSns = true;
                }
            }
            else{
                buff++;
            }
        }
        else{
            if (ptr != NULL && snsr == 4){
                printf("You must add until 3 sensors in one command line.\n");
                exit(EXIT_FAILURE);
            }
            else{
                if (ptr == NULL){
                    if (isdigit(*buff) && machn <= 3 && evaluateSns){
                        long val = strtol(buff, &buff, 10);
                        if ((int)val > 0 && (int)val < 5){
                            machines[machn] = (int)val;
                            machn++;
                            evaluateMchn = true;
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
        printf("Sintax error\n");
        exit(EXIT_FAILURE);
    }
    if (!evaluateMchn){
        printf("Sintax error\n");
        exit(EXIT_FAILURE);
    }

    for (int numEq = 0; numEq < 4; numEq++){
        int numMachnChange = 0;
        while (numMachnChange < machn){
            if (eq[numEq].equipmentId == machines[numMachnChange]){
                for (int h = 0; h < snsr; h++){
                    for (int j = 0; j < 4; j++){
                        bool exist = existSensor(eq, snsr, numEq, sensors[h]);
                        if ((eq[numEq].sensors[j] == 0) && !exist){
                            eq[numEq].sensors[j] = sensors[h];
                            printf("Sensor %d added\n", sensors[h]);
                            break;
                        }
                        else if (exist){
                            break;
                        }
                    }
                }
            }
            numMachnChange++;
        }
    }
    return eq;
}

void listSensors(char *buff, equipment* eq){
    const char str[] = "in";
    char *ptr;
     while (*buff){
        ptr = strstr(buff, str);
        if (ptr != NULL){
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10);
                for (int i = 0; i < 4; i++){
                    if (eq[i].equipmentId == (int)val){
                        for(int j = 0; j < 4; j++){
                            if(eq[i].equipmentId != 0){
                                printf("%d", eq[i].sensors[j]);
                            }else{
                                printf("none");
                            }
                            printf("\n");
                        }
                    }
                    
                }
                
            }
        }
        buff++;
    } 

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

    equipment esteira = {01, {0,0,0,0}};
   
    equipment guindaste = {02, {0,0,0,0}};
    
    equipment ponteRolante = {03, {0,0,0,0}};
   
    equipment empilhadeira = {04, {0,0,0,0}};

    equipment equipments[4];

    equipments[0] = esteira;
    equipments[1] = guindaste;
    equipments[2] = ponteRolante;
    equipments[3] = empilhadeira;  

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

            regex_t regex1;
            int verifyRegx1 = regcomp(&regex1, "add sensor", REG_EXTENDED);
            
            if (verifyRegx1 != 0){
                logexit("Regex error compilation. \n");
            }
            else{
                int matchRegx = regexec(&regex1, buf, 0, NULL, 0);
                if (matchRegx == 0){
                    extractNumber(buf, equipments);
                    printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                }else{
                    regex_t regex2;
                    int verifyRegx2 = regcomp(&regex2, "list sensors", REG_EXTENDED);

                    if (verifyRegx2 != 0){
                        logexit("Regex error compilation. \n");
                    }
                    else{
                        int matchRegx2 = regexec(&regex2, buf, 0, NULL, 0);
                        if (matchRegx2 == 0){
                            listSensors(buf, equipments);
                            printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                        }
                    }
                }
            }

            sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
            count = send(csock, buf, strlen(buf) + 1, 0);
            if (count != strlen(buf) + 1){
                logexit("send");
            }
            close(csock);
        }
    }
        exit(EXIT_SUCCESS);
    
}

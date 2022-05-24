#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <regex.h>
 #include <ctype.h>

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

int extractNumber(char* buff){
    const char str[] = "in";
    char *ptr;
    int snsr = 0;
    int machn = 0;
    int sensores[4];
    int machines[4];
    
    while (*buff){
        ptr = strstr(buff, str);
        if(ptr != NULL && snsr != 4){
            if (isdigit(*buff)) {
                long val = strtol(buff, &buff, 10);
                sensores[snsr] = (int)val;
                printf("Sensors: %d\n", sensores[snsr]);
                snsr++;
            } else {
                buff++;
            }
        }else{
            if(ptr == NULL && machn != 4){
                if (isdigit(*buff)) {
                    long val = strtol(buff, &buff, 10);
                    machines[machn] = (int)val;
                    printf("Machines: %d\n", machines[machn]);
                    machn++;
                }
            }else{
                buff++;
            }           
        }


        /* while(*sensors){
            printf("%d", *sensors);
            sensors++;
        } */
        
    }
    return 0;
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

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    equipment esteira;
    esteira.equipmentId = 01;
    
    equipment guindaste;
    guindaste.equipmentId = 02;

    equipment ponteRolante;
    ponteRolante.equipmentId = 03;

    equipment empilhadeira;
    empilhadeira.equipmentId = 04;

    equipment equipments[4];

    equipments[0] = esteira;
    equipments[1] = guindaste;
    equipments[2] = ponteRolante;
    equipments[3] = empilhadeira;
   for (int i = 0; i <= 3; i++)
   {
        equipments[i].sensors[i] = 0;
        //printf("%d\n", equipments[i].equipmentId);
        /* printf("%d\n", equipments[i].sensors[i]); */
   }

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
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
        }else{
            
            regex_t regex1;
            int verifyRegx1 = regcomp(&regex1, "add sensor", 0);

            if (verifyRegx1 != 0){
                logexit("Regex error compilation. \n");
            }
            else{
                int matchRegx = regexec(&regex1, buf, 0, NULL, 0);
                if (matchRegx == 0){
                    extractNumber(buf);
                    addSensor(5, 2, equipments);
                    printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
                }
            }
        }

        
        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        count = send(csock, buf, strlen(buf) + 1, 0);
        if (count != strlen(buf) + 1) {
            logexit("send");
        }
        close(csock);
    }

    exit(EXIT_SUCCESS);
}
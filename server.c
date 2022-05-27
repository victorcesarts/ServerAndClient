/* VÍCTOR CÉSAR TEIXEIRA SANTOS
MATRÍCULA 2020021832
EMAIL: victorcesarts@gmail.com */

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

/* Equipments are defined by having an Id and sensors, each sensor has a value associated */
typedef struct{
    int equipmentId;
    float sensors[4][2];
    
}equipment;

/* Sometimes you want to see if a sensor already exists in an equipment */
bool existSensor(equipment* eq, int snsr, int numEq, int sensor){
    for (int j = 0; j < 4; j++){
        if(eq[numEq].sensors[j][0] == sensor){
            return true;
        }
    }    
 return false;
}

/* Well, sensors needs a value to read and display */
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

   /*  BLOCK TO EXTRACT WHICH SENSORS AND WHICH MACHINES ONE WANTS TO CHANGE */

    while (*buff){
        ptr = strstr(buff, str);
        if (ptr != NULL && snsr != 4){  /* SEARCHING FOR THE SENSORS THAT ARE BEFORE STRING 'IN' */
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10); /* HERE WE ARE TRANSFORMING THE VALUE OF SENSOR IN OUR STRING TO AN LONG INT */
                if ((int)val > 0 && (int)val < 5){ /* SENSORS HAVE VALUES ONLY BETWEEN 1 AND 4 */
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
            if (ptr != NULL && snsr == 4){ /* IF YOU TRY TO ADD A HUGE NUMBER OF SENSORS (MORE THAN 3) PROGRAM WILL COMPLAIN */
                sprintf(strRetr,"You must declare until 3 sensors in one command line.\n");
                return strRetr;
            }
            else{
                if (ptr == NULL){
                    if (isdigit(*buff) && machn <= 3 && evaluateSns){ /* ONCE WE HAVE FOUND THE STRING 'IN' AND WE'VE HAD NO PROBLEM WITH SENSORS WE WANT THOSE EQUIPMENTS */
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

    if (evaluateSns && !evaluateMchn){ /* IF WE HAVE ANALISED SENSORS, BUT NOT THE MACHINES, WE MIGHT HAVE HAD A PROBLEM AFTER STRING 'IN' */
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }
    if (!evaluateMchn){ /* IF WE'VE GOT A PROBLEM ON EVERYTHING, WE'RE GOING TO GET OUT */
        sprintf(strRetr, "Sintax error\n");
        exit(EXIT_FAILURE);
    }
    /* NOW WE HAVE AN ARRAY WITH THE SENSORS sensors[] AND MACHINES machines[] WE WANT TO ADD, LET'S DO THAT BELOW */

    for (int numEq = 0; numEq < 4; numEq++){ /* WE WILL SEARCH THROUGH ALL OUR 4 EQUIPMENTS IN THE FACTORY */
        int numMachnChange = 0;
        while (numMachnChange < machn){
            if (eq[numEq].equipmentId == machines[numMachnChange]){ /* IF WE'VE FOUND AN EQUIPMENT THAT WE WANT TO DO SOMETHING... */
                for (int h = 0; h < snsr; h++){
                    for (int j = 0; j < 4; j++){
                        bool exist = existSensor(eq, snsr, numEq, sensors[h]); /* WE WILL NEED TO VERIFY IF THE MACHINE THAT WE'RE MODIFYING ALREADY HAVE THE SENSOR WE WANT TO ADD */
                        if ((eq[numEq].sensors[j][0] == 0) && !exist){
                            if(*numSensors < 15){ //limit of sensors is 15
                                eq[numEq].sensors[j][0] = sensors[h]; /* ADDING A SENSOR */
                                generateRandom(eq, numEq, j); /* AFTER ADDED THE SENSOR, WE LINK A FLOAT NUMBER THAT WILL SIMULATE THE VALUE IT WILL READ */
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
        if ((ptr == NULL)){ /* WE ARE SEARCHING FOR THE EQUIPMENT AFTER OUR "IN" */
            if (isdigit(*buff)){
                long val = strtol(buff, &buff, 10);
                if (((int)val > 0 && (int)val < 5) && (!evaluate)){
                    for (int i = 0; i < 4; i++){
                        if (eq[i].equipmentId == (int)val){ /* WE'VE FOUND THE EQUIPMENT */
                            for (int j = 0; j < 4; j++){
                                
                                if (eq[i].sensors[j][0] != 0){ /* WE'RE SEEING IF THE EQUIPMENT EXIST IN THE EQUIPMENT */
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
    if(!evaluate){ /* IF WE'VE HAD SOME PROBLEM WITH THE STRING, WE DIDN'T EVALUATE THE EQUIPMENTS AND THEN WE'RE GETTING OUT */
        exit(EXIT_FAILURE);
    }
    return strRetr;
}

/* FUNCTION BELOW USES THE SAME LOGIC OF EXTRACTING NUMBERS OF SENSORS AND MACHINES THAT WAS SHOWED AT FUNCTION addSensor() */
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
                        if ((exist) && (eq[numEq].sensors[j][0] == sensors[h])){ 
                            eq[numEq].sensors[j][0] = 0;
                            sprintf(aux, "Sensor %d removed\n", sensors[h]);
                            strcat(strRetr, aux);
                            *numSensors = *numSensors - 1;                  /* DECREASING NUMBERS OF SENSORS */
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

/* FUNCTION BELOW ALSO USES THE SAME LOGIC OF EXTRACTING NUMBER OF SENSORS AND MACHINES */
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

    bool installed = true; /* JUST TO HELP FORMATING THE OUTPUT WITH NEW LINE AT THE END */
    for (int numEq = 0; numEq < 4; numEq++){
        int numMachnChange = 0;
        
        while (numMachnChange < machn){
            if (eq[numEq].equipmentId == machines[numMachnChange]){
                for (int h = 0; h < snsr; h++){ /* SEARCHING THROUG ARRAY OF SENSORS ONE COULD SPECIFIED IN THE COMMAND LINE */
                    for (int j = 0; j < 4; j++){    /* SEARCHING IN OUR EQUIPMENTS */
                        if ((eq[numEq].sensors[j][0] != 0) && (eq[numEq].sensors[j][0] == sensors[h])){ /*  WE FOUND OUT THE SENSOR IN OUR EQUIPMENTS, NOW WE WILL JUST PRINT THE VALUE READED */
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
                if(!installed){ /* AS I SAID, HELPING TO PRINT OUT... */
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

    /* HERE WE ARE DECLARING SOME EQUIPMENTS, WE INITIALIZE EVERYTHING WITH VALUE 0
    EXCEPT THE ID FROM THE EQUIPMENTS */

    equipment esteira = {01, {{0,0},{0,0},{0,0},{0,0}}};
   
    equipment guindaste = {02, {{0,0},{0,0},{0,0},{0,0}}};
    
    equipment ponteRolante = {03, {{0,0},{0,0},{0,0},{0,0}}};
   
    equipment empilhadeira = {04, {{0,0},{0,0},{0,0},{0,0}}};

    /* WE CREATE AN ARRAY OF EQUIPMENTS TO OUR INDUSTRY */

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
            /* EVERY COMMAND IS EVALUATED USING REGULAR EXPRESSION */

            regex_t regex1;
            int verifyRegx1 = regcomp(&regex1, "add sensor", REG_EXTENDED);

            if (verifyRegx1 != 0){ /* IF WE HAVE SOME PROBLEM WITH REGEX */
                logexit("Regex error compilation. \n");
            }
            else{
                int matchRegx = regexec(&regex1, buf, 0, NULL, 0); /* VERIFYING OUR ENTRY WITH THE REGEX TO BE MATCHED */
                if (matchRegx == 0){
                    if(numSensors < 15){ /* SENSORS LIMIT IS 15 */
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

            /* EVERY FUNCTION RETURN A STRING WITH THE RESULT OF THE EVALUATION MADED,
            THEN, WE SEND THIS STRING TO CLIENT. */
            count = send(csock, bufRet, strlen(bufRet) + 1, 0);
            if (count != strlen(bufRet) + 1){
                logexit("send");
            }
            close(csock);
        }
    }
        exit(EXIT_SUCCESS);
    /* IT'S TIME TO RETURN TO CLIENT */
}

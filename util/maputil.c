/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   maputil.c
 * Author: norips
 *
 * Created on 29 octobre 2016, 14:41
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
/*
 * 
 */
static struct option long_options[] = {
    {"getwidth", no_argument, NULL, 'w'},
    {"getheight", no_argument, NULL, 'h'},
    {"getobjects", no_argument, NULL, 'o'},
    {"getinfo", no_argument, NULL, 'i'},
    {NULL, 0, NULL, 0}
};
void printWidth(int file);
void printHeight(int file);
void printObject(int file);
void printInfo(int file);
char* getLine(int fd) {
    char buffer[1024];
    buffer[0] = '\0';
    int i = 0;
    int n = 0;
    while (i < 1024 && (n = read(fd, buffer + i, 1)) > 0) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
            break;
        }
        i += n;
    }
    char *buff = malloc(sizeof (char)*(i + 1));
    memcpy(buff, buffer, sizeof (char)*(i + 1));
    return buff;
}

int main(int argc, char** argv) {
    int ch;
    int file = open(argv[1],O_RDWR);
    while ((ch = getopt_long(argc, argv, "whoi", long_options, NULL)) != -1) {
        // check to see if a single character or long option came through
        switch (ch) {
            case 'w':
                printWidth(file);
                break;
            case 'h':
                printHeight(file);
                break;
            case 'o':
                printObject(file);
                break;
            case 'i':
                printInfo(file);
                break;
        }
    }
    close(file);
    return (EXIT_SUCCESS);
}

void printWidth(int file){
    lseek(file,0,SEEK_SET);
    char *width = getLine(file);
    printf("Width : %s\n",width);
    free(width);
}

void printHeight(int file){
    lseek(file,0,SEEK_SET);
    free(getLine(file));
    char *height = getLine(file);
    printf("Height : %s\n",height);
    free(height);
}

void printObject(int file){
    lseek(file,0,SEEK_SET);
    free(getLine(file));
    free(getLine(file));
    char *obj = getLine(file);
    printf("Objects : %s\n",obj);
    free(obj);
}

void printInfo(int file){
    printWidth(file);
    printHeight(file);
    printObject(file);
}
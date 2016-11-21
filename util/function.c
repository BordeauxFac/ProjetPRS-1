/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "function.h"

void printObject(int file) {
    unsigned int nb_obj = getObject(file);
    printf("Objects : %u\n", nb_obj);
}

void printInfo(int file) {
    printWidth(file);
    printHeight(file);
    printObject(file);
}

void printWidth(int file) {
    printf("Width : %d\n", getWidth(file));
}

void printHeight(int file) {
    printf("Height : %d\n", getHeight(file));
}

unsigned int getObject(int file) {
    lseek(file, sizeof(unsigned int) + sizeof(unsigned int), SEEK_SET);
    unsigned int nb_obj;
    read(file,&nb_obj,sizeof(unsigned int));
    return nb_obj;
}

unsigned int getWidth(int file){
    lseek(file, 0, SEEK_SET);
    unsigned int width;
    read(file,&width,sizeof(unsigned int));
    return width;
}

unsigned int getHeight(int file){
    lseek(file, sizeof(unsigned int), SEEK_SET);
    unsigned int height;
    read(file,&height,sizeof(unsigned int));
    return height;
}

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

void copyAndTruncate(int src, int dst){
    char buf[4096];
    int size = 0;
    lseek(src, 0, SEEK_SET);
    lseek(dst, 0, SEEK_SET);
    int n;
    while ((n = read(src, buf, 4096)) > 0) {
        write(dst, buf, n);
        size+=n;
    }
    ftruncate(dst, size);
}
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
#define _XOPEN_SOURCE 500
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
    {"setwidth", required_argument, NULL, 'x'},
    {"setheight", required_argument, NULL, 'y'},
    {NULL, 0, NULL, 0}
};
void printWidth(int file);
void printHeight(int file);
void printObject(int file);
void printInfo(int file);

void setWidth(int file, char* width);
void setHeight(int file, char* height);

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
    int file = open(argv[1], O_RDWR);
    if(file == -1){
        fprintf(stderr,"Need a valid file\n");
        exit(EXIT_FAILURE);
    }
    while ((ch = getopt_long(argc, argv, "whoixy", long_options, NULL)) != -1) {
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
            case 'x':
                setWidth(file, optarg);
                break;
            case 'y':
                setHeight(file, optarg);
                break;
        }
    }
    close(file);
    return (EXIT_SUCCESS);
}

void printWidth(int file) {
    lseek(file, 0, SEEK_SET);
    char *width = getLine(file);
    printf("Width : %s\n", width);
    free(width);
}

void printHeight(int file) {
    lseek(file, 0, SEEK_SET);
    free(getLine(file));
    char *height = getLine(file);
    printf("Height : %s\n", height);
    free(height);
}

void printObject(int file) {
    lseek(file, 0, SEEK_SET);
    free(getLine(file));
    free(getLine(file));
    char *obj = getLine(file);
    printf("Objects : %s\n", obj);
    free(obj);
}

void printInfo(int file) {
    printWidth(file);
    printHeight(file);
    printObject(file);
}

void setWidth(int file, char *width) {
    lseek(file, 0, SEEK_SET);
    char* w = getLine(file);
    int widt = atoi(w);
    int new_width = atoi(width);
    int f_out = dup(1);
    dup2(file, 1);
    lseek(file, 0, SEEK_SET);
    char *buff = malloc(sizeof (char) * (strlen(width) + 2));
    memset(buff, 0, sizeof (char) * (strlen(width) + 2));
    snprintf(buff,(strlen(width)+2),"%d\n",new_width);
    write(file,buff,(strlen(width) + 1));

    if (new_width < widt) {
        lseek(file, 0, SEEK_SET);
        int file2 = open("/tmp/tmpFile", O_CREAT | O_TRUNC | O_RDWR, 0666);
        if(dup2(file2, 1) == -1)
            perror("dup2");

        char *tmp = getLine(file);
        printf("%s\n", tmp);
        free(tmp);
        tmp = getLine(file);
        printf("%s\n", tmp);
        free(tmp);
        tmp = getLine(file);
        int nbobj = atoi(tmp);
        printf("%s\n", tmp);
        free(tmp);
        tmp = getLine(file);
        int i = 0;
        while (strcmp(tmp,"END") != 0) {
            if (i >= nbobj) {
                fprintf(stderr, "%s\n", tmp);
                int width_obj = atoi(tmp);
                if (width_obj <= new_width) {
                    printf("%s\n", tmp);
                }
            } else {
                printf("%s\n", tmp);
                fprintf(stderr, "%s\n", tmp);
            }
            free(tmp);
            tmp = getLine(file);
            i++;
        }
        printf("END\n");
        fflush(stdout);
        dup2(f_out,1);
        char c;
        int size = 0;
        lseek(file,0,SEEK_SET);
        lseek(file2,0,SEEK_SET);
        while(read(file2,&c,1) > 0){
            write(file,&c,1);
            size++;
        }
        ftruncate(file,size);
        close(file2);
    }
    dup2(f_out,1);
}

void setHeight(int file, char *height) {
    lseek(file, 0, SEEK_SET);
    free(getLine(file));
    char* h = getLine(file);
    int heig = atoi(h);
    int new_height = atoi(height);
    int f_out = dup(1);
    dup2(file, 1);
    lseek(file, 0, SEEK_SET);
    free(getLine(file));
    int lenHeight = strlen(height) + 2; //\n\0
    char *buff = malloc(sizeof (char) * lenHeight);
    memset(buff, 0, sizeof (char) * lenHeight);
    snprintf(buff,lenHeight,"%d\n",new_height);
    write(file,buff,lenHeight-1);

    if (new_height < heig) {
        lseek(file, 0, SEEK_SET);
        int file2 = open("/tmp/tmpFile", O_CREAT | O_TRUNC | O_RDWR, 0666);
        if(dup2(file2, 1) == -1)
            perror("dup2");

        char *tmp = getLine(file);
        printf("%s\n", tmp);
        free(tmp);
        tmp = getLine(file);
        printf("%s\n", tmp);
        free(tmp);
        tmp = getLine(file);
        int nbobj = atoi(tmp);
        printf("%s\n", tmp);
        free(tmp);
        tmp = getLine(file);
        int i = 0;
        while (strcmp(tmp,"END") != 0) {
            if (i >= nbobj) {
                fprintf(stderr, "%s\n", tmp);
                char *tk = tmp;
                char *x = strtok(tk, "\t");
                char *y = strtok(NULL, "\t");
                int height_obj = atoi(y);
                if (height_obj <= new_height) {
                    printf("%s\n", tmp);
                }
            } else {
                printf("%s\n", tmp);
                fprintf(stderr, "%s\n", tmp);
            }
            free(tmp);
            tmp = getLine(file);
            i++;
        }
        printf("END\n");
        fflush(stdout);
        dup2(f_out,1);
        char c;
        int size = 0;
        lseek(file,0,SEEK_SET);
        lseek(file2,0,SEEK_SET);
        while(read(file2,&c,1) > 0){
            write(file,&c,1);
            size++;
        }
        ftruncate(file,size);
        close(file2);
    }
    dup2(f_out,1);

}
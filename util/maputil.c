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

#include "../include/map.h"
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
    {"setobjects", required_argument, NULL, 'a'},
    {"pruneobjects", no_argument, NULL, 'p'},
    {NULL, 0, NULL, 0}
};
/**
 * Print width of the file pointed by the file descriptor file
 * @param fd_file Open file descriptor to valid save file
 */
void printWidth(int fd_file);
/**
 * Print height of the file pointed by the file descriptor file
 * @param fd_file Open file descriptor to valid save file
 */
void printHeight(int fd_file);
/**
 * Print number of objects of the file pointed by the file descriptor file
 * @param fd_file Open file descriptor to valid save file RD
 */
void printObject(int fd_file);
/**
 * Call @ref printWidth @ref printHeight @ref printObject
 * @param fd_file Open file descriptor to valid save file RD
 */
void printInfo(int fd_file);

/**
 * Get number of objects
 * @param fd_file Open file descriptor to valid save file RD
 * @return \0 terminated string with number of objects encoded in ASCII
 */
char* getObject(int fd_file);

/**
 * Set new width to the file
 * @param fd_file Open file descriptor to valid save file RDWR
 * @param width
 */
void setWidth(int fd_file, char* width);

/**
 * Set new height to the file
 * @param fd_file
 * @param height
 */
void setHeight(int fd_file, char* height);

/**
 * Reset and add argc-optind numbers of objects to save file
 * @param fd_file
 * @param argv
 * @param argc
 * @param optind Starting indice of parameters position in argv
 */
void addObject(int fd_file, char **argv, int argc, int optind);

/**
 * Remove unused object in save file
 * @param file
 */
void removeUnused(int file);

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

int main(int argc, char** argv) {
    int ch;
    int file = open(argv[1], O_RDWR);
    if (file == -1) {
        fprintf(stderr, "Need a valid file\n");
        exit(EXIT_FAILURE);
    }
    while ((ch = getopt_long(argc, argv, "w:h:o:i:x:y:a:p", long_options, NULL)) != -1) {
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
            case 'a':; //
                char* _curr_nb_obj = getObject(file);
                int curr_nb_obj = atoi(_curr_nb_obj);
                free(_curr_nb_obj);
                if (curr_nb_obj > ((argc - (optind-1)) / 6)) {
                    fprintf(stderr, "Need more or equal number of objects than %d", curr_nb_obj);
                    exit(EXIT_FAILURE);
                }
                addObject(file, argv, argc, optind);
                break;
            case 'p': ;//
                removeUnused(file);
                break;
        }
    }
    close(file);
    return (EXIT_SUCCESS);
}

void printLine(int file){
    char *tmp = getLine(file);
    printf("%s\n", tmp); //width
    free(tmp);
}

void removeUnused(int file) {
    char *_nbObj = getObject(file);
    int nbObj = atoi(_nbObj);
    if(nbObj<=0) {
        fprintf(stderr,"removeUnused : nb_obj<0");
        return;
    }
    free(_nbObj);
    char *objInUse = malloc(sizeof(char)*nbObj);
    memset(objInUse,0,nbObj);
    for(int i = 0; i < nbObj; i++)
        free(getLine(file));
    char *tmp = getLine(file);
    int nbInUse = 0;
    while(strcmp(tmp,"END")){
        strtok(tmp,"\t");
        strtok(NULL,"\t");
        char *_objId = strtok(NULL,"\t");
        int objId = atoi(_objId);
        if(!objInUse[objId]){
            objInUse[objId] = 1;
            nbInUse++;
        }
        free(tmp);
        tmp = getLine(file);
    }
    free(tmp);
    int tmpFile = open("/tmp/tmpFileremoveUnused", O_CREAT | O_TRUNC | O_RDWR, 0666);
    int saveOut = dup(1);
    dup2(tmpFile, 1);
    lseek(file,0,SEEK_SET);
    printLine(file);
    printLine(file);
    printf("%d\n",nbInUse);
    free(getLine(file));
    int newObjID[nbObj];
    int currID = 0;
    for(int i = 0; i < nbObj;i++){
        char *tmp = getLine(file);
        if(objInUse[i]){
            newObjID[i] = currID++;
            printf("%s\n",tmp);
        }
        free(tmp);
    }
    tmp = getLine(file);
    while(strcmp(tmp,"END")){
        char *tk = tmp;
        char *x = strtok(tk, "\t");
        char *y = strtok(NULL, "\t");
        char *_oldId = strtok(NULL, "\n");
        int oldId = atoi(_oldId);
        printf("%s\t%s\t%d\n",x,y,newObjID[oldId]);
        free(tmp);
        tmp = getLine(file);
    }
    free(tmp);
    printf("END\n");
    fflush(stdout);
    copyAndTruncate(tmpFile,file);
    free(objInUse);
    close(tmpFile);
    dup2(saveOut,1);
    
    
}

void addObject(int file, char **argv, int argc, int optind) {
    char * _nb_obj = getObject(file);
    int nb_obj = atoi(_nb_obj);
    free(_nb_obj);
    lseek(file, 0, SEEK_SET);
    int tmpFile = open("/tmp/tmpFileObjMapUtil", O_CREAT | O_TRUNC | O_RDWR, 0666);
    int save_out = dup(1);
    dup2(tmpFile, 1);
    lseek(file, 0, SEEK_SET);
    char *tmp = getLine(file);
    printf("%s\n", tmp); //width
    free(tmp);
    tmp = getLine(file);
    printf("%s\n", tmp); //Height
    free(getLine(file)); //nb obj

    optind--;

    printf("%d\n", (argc - optind) / 6);
    for(int i=0; i < nb_obj;i++){
        free(getLine(file));
    }
    for (; optind < argc && *argv[optind] != '-'; optind += 6) {//Next obj
        printf("%s\t", argv[optind]);
        printf("%s\t", argv[optind + 1]);
        if (strcmp("solid", argv[optind + 2]) == 0) {
            printf("%d\t", MAP_OBJECT_SOLID);
        } else if (strcmp("semi-solid", argv[optind + 2]) == 0) {
            printf("%d\t", MAP_OBJECT_SEMI_SOLID);
        } else if (strcmp("air", argv[optind + 2]) == 0) {
            printf("%d\t", MAP_OBJECT_AIR);
        }
        if (strcmp("destructible", argv[optind + 3]) == 0) {
            printf("%d\t", MAP_OBJECT_DESTRUCTIBLE);
        } else if (strcmp("not-destructible", argv[optind + 3]) == 0) {
            printf("0\t");
        }
        if (strcmp("collectible", argv[optind + 4]) == 0) {
            printf("%d\t", MAP_OBJECT_COLLECTIBLE);
        } else if (strcmp("not-collectible", argv[optind + 4]) == 0) {
            printf("0\t");
        }
        if (strcmp("generator", argv[optind + 5]) == 0) {
            printf("%d\t\n", MAP_OBJECT_GENERATOR);
        } else if (strcmp("not-generator", argv[optind + 5]) == 0) {
            printf("0\t\n");
        }
    }
    tmp = getLine(file);
    while(strcmp(tmp,"END")){
        printf("%s\n",tmp);
        free(tmp);
        tmp = getLine(file);
    }
    printf("END\n");
    fflush(stdout);
    dup2(save_out, 1);
    copyAndTruncate(tmpFile,file);
    close(tmpFile);


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

char* getObject(int file) {
    lseek(file, 0, SEEK_SET);
    free(getLine(file));
    free(getLine(file));
    char *obj = getLine(file);
    return obj;
}

void printObject(int file) {
    char *obj = getObject(file);
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
    free(w);
    int new_width = atoi(width);
    int f_out = dup(1);
    dup2(file, 1);
    lseek(file, 0, SEEK_SET);
    char *buff = malloc(sizeof (char) * (strlen(width) + 2));
    memset(buff, 0, sizeof (char) * (strlen(width) + 2));
    snprintf(buff, (strlen(width) + 2), "%d\n", new_width);
    write(file, buff, (strlen(width) + 1));
    free(buff);
    if (new_width < widt) {
        lseek(file, 0, SEEK_SET);
        int file2 = open("/tmp/tmpFile", O_CREAT | O_TRUNC | O_RDWR, 0666);
        if (dup2(file2, 1) == -1)
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
        while (strcmp(tmp, "END") != 0) {
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
        free(tmp);
        printf("END\n");
        fflush(stdout);
        dup2(f_out, 1);
        copyAndTruncate(file2,file);
        close(file2);
    }
    dup2(f_out, 1);
}

void setHeight(int file, char *height) {
    lseek(file, 0, SEEK_SET);
    free(getLine(file));
    char* h = getLine(file);
    int heig = atoi(h);
    free(h);
    int new_height = atoi(height);
    int f_out = dup(1);
    dup2(file, 1);
    lseek(file, 0, SEEK_SET);
    free(getLine(file));
    int lenHeight = strlen(height) + 2; //\n\0
    char *buff = malloc(sizeof (char) * lenHeight);
    memset(buff, 0, sizeof (char) * lenHeight);
    snprintf(buff, lenHeight, "%d\n", new_height);
    write(file, buff, lenHeight - 1);
    free(buff);
    if (new_height < heig) {
        lseek(file, 0, SEEK_SET);
        int file2 = open("/tmp/tmpFile", O_CREAT | O_TRUNC | O_RDWR, 0666);
        if (dup2(file2, 1) == -1)
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
        while (strcmp(tmp, "END") != 0) {
            if (i >= nbobj) {
                fprintf(stderr, "%s\n", tmp);
                char *tk = tmp;
                strtok(tk, "\t");
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
        free(tmp);
        printf("END\n");
        fflush(stdout);
        dup2(f_out, 1);
        copyAndTruncate(file2,file);
        close(file2);
    }
    dup2(f_out, 1);

}
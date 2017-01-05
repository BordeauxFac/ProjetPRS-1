/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "function.h"

void printObject(int file) {
    unsigned int nb_obj = getObject(file); //récupération du nombre d'objet à partir du fichier file
    printf("Objects : %u\n", nb_obj); //affichage sur la sortie standard
}

void printInfo(int file) {
    printWidth(file);
    printHeight(file);
    printObject(file);
}

void printWidth(int file) {
    /*récupération et affichage de la largeur à partir du fichier file */
    printf("Width : %d\n", getWidth(file));
}

void printHeight(int file) {
    /*récupération et affichage de la hauteur à partir du fichier file */
    printf("Height : %d\n", getHeight(file));
}

unsigned int getObject(int file) {
    lseek(file, sizeof (unsigned int) + sizeof (unsigned int), SEEK_SET); //positionnement au troisième argument de la première ligne du fichier */
    unsigned int nb_obj;
    read(file, &nb_obj, sizeof (unsigned int)); /* récupération du nombre d'objets */
    return nb_obj;
}

unsigned int getWidth(int file) {
    lseek(file, 0, SEEK_SET); /* positionnement au début du fichier */
    unsigned int width;
    read(file, &width, sizeof (unsigned int)); // récupération de la largeur */
    return width;
}

unsigned int getHeight(int file) {
    lseek(file, sizeof (unsigned int), SEEK_SET); // positionnement après la première donnée */
    unsigned int height;
    read(file, &height, sizeof (unsigned int)); //récupération de la hauteur */
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

void copyAndTruncate(int src, int dst) {
    char buf[4096];
    int size = 0;
    lseek(src, 0, SEEK_SET);
    lseek(dst, 0, SEEK_SET);
    int n;
    while ((n = read(src, buf, 4096)) > 0) {
        write(dst, buf, n);
        size += n;
    }
    ftruncate(dst, size);
}
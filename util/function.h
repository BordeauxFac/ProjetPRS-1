/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   function.h
 * Author: norips
 *
 * Created on 21 novembre 2016, 15:33
 */

#ifndef FUNCTION_H
#define FUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif

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
     * @return unsigned int number of objects
     */
    unsigned int getObject(int fd_file);



    /**
     * Remove unused object in save file
     * @param file
     */
    void removeUnused(int file);

    unsigned int getWidth(int file);
    unsigned int getHeight(int file);
    char* getLine(int fd);
    void copyAndTruncate(int src, int dst);
    unsigned int getObject(int file);
    void printObject(int file);
    void printInfo(int file);
    void printWidth(int file);
    void printHeight(int file);
    void setWidth(int file, char *width);
    void setHeight(int file, char *height);



#ifdef __cplusplus
}
#endif

#endif /* FUNCTION_H */


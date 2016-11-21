## -*- Makefile -*-
##
## User: norips
## Time: 29 oct. 2016 15:07:02
## Makefile created by Oracle Developer Studio.
##
## This file is generated automatically.
##


#### Compiler and tool definitions shared by all build targets #####
CC = gcc
CFLAGS := -g -std=c99 -Wall -Wno-unused-function
	
all: maputil
maputil: maputil.o function.o
function.o : function.c function.h
maputil.o: maputil.c function.h

.PHONY: clean
clean:
	rm -f maputil.o
	rm -f maputil

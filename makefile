.RECIPEPREFIX = >

EXE    = engine
CC     = gcc
CFLAGS = -Wall -Wextra -std=c17 -O3 -march=native -flto

all:
> $(CC) $(CFLAGS) *.c -o $(EXE)
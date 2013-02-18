CC = gcc
CFLAGS = -g -Wall

all: puzzle

puzzle: puzzle_solver.o
	${CC} ${CFLAGS} -o puzzle puzzle_backend.o puzzle_solver.o

puzzle_solver.o: puzzle_solver.c
	${CC} ${CFLAGS} -c puzzle_solver.c

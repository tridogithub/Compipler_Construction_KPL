CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 
OUT = ./out

all: parser

parser: main.c scanner.o reader.o charcode.o token.o error.o parser.o
	${CC} main.c ${OUT}/scanner.o ${OUT}/reader.o ${OUT}/charcode.o ${OUT}/token.o ${OUT}/error.o ${OUT}/parser.o -o ${OUT}/parser

scanner.o: ./scanner/scanner.c
	${CC} ${CFLAGS} scanner/scanner.c -o ${OUT}/scanner.o
	
reader.o: ./reader/reader.c
	${CC} ${CFLAGS} reader/reader.c -o ${OUT}/reader.o

charcode.o: ./lib/charcode/charcode.c
	${CC} ${CFLAGS} lib/charcode/charcode.c -o ${OUT}/charcode.o

token.o: ./lib/token/token.c
	${CC} ${CFLAGS} lib/token/token.c -o ${OUT}/token.o

error.o: ./lib/error/error.c
	${CC} ${CFLAGS} lib/error/error.c -o ${OUT}/error.o
	
parser.o: ./parser/parser.c
	${CC} ${CFLAGS} parser/parser.c -o ${OUT}/parser.o

clean:
	rm -f *.o *~


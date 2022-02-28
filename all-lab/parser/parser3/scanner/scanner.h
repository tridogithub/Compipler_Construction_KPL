#ifndef __SCANNER_H__
#define __SCANNER_H__



#include "../reader/reader.h"
#include "../lib/charcode/charcode.h"
#include "../lib/token/token.h"
#include "../lib/error/error.h"

#define IO_ERROR 0
#define IO_SUCCESS 1

void skipBlank();
void skipComment();
Token* readIdentKeyword(void); 
Token* readNumber(void);
Token* readConstChar(void);

Token* getToken(void);
void printToken(Token *token);

int scanFile(char *fileName);
Token *getValidToken();

#endif
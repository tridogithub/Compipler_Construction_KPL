/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "./scanner.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

Token *getValidToken() {
  Token *token = getToken();
  while (token->tokenType == TK_NONE) {
    free(token);
    token = getToken();
  }
  return token;
}

void skipBlank() {
  // we check currentChar to see if it's a space
  // if it's a space, continue reading until currentChar is not a space anymore
  while((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE)) {
    readChar();
  }
}

void skipComment() {
  // A comment will be like this: (*This is a comment*)
  // suppose that each comment is put on a separated line
  // then we will only have to check if the end of line is *)

  // In this case, we check currentChar to see if it's an asterisk
  // if it's an asterisk, continue to see if the next character is a right parenthesis
  // if the next character is a right parenthesis, then conclude that this is a comment
  // then we read next character and return
  // else, we continue to read normally until EOF

  int isComment = 0; // 1 for asterisk, 2 for asterisk + right parenthesis
  while((currentChar != EOF) && (isComment < 2)) {
    // if isComment == 2, then we break to conclude that this is a line of comment
    switch(charCodes[currentChar]) {
      case CHAR_TIMES: {
        isComment = 1;
        readChar();
        break;
      }
      case CHAR_RPAR: {
        if (isComment == 1) {
          isComment = 2;
        } else {
          isComment = 0;
        }
        readChar();
        break;
      }
      default: {
        readChar();
      }
    }
  }

  if (isComment == 2) {
    readChar();
  } else {
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  }

  // NOTES: if the lines is not of comment only
  // Then we need another mechanism to check (* and *)
  // The above algorithm is according to skipComment() diagram in the slides
}

Token* readIdentKeyword(void) {
  // Make a token for later return
  Token *resTok = makeToken(TK_NONE, lineNo, colNo);
  // save currentChar to string
  resTok->string[0] = currentChar;
  // for pointing to positions in string
  int strIdx = 1;

  readChar();
  while((currentChar != EOF) && 
        (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_LETTER)) {
          if(strIdx <= MAX_IDENT_LEN) {
            resTok->string[strIdx] = (char)currentChar;
            strIdx++;
          }
          readChar();
        }
  
  if(strIdx > MAX_IDENT_LEN) {
    // the obtained string has length larger than maximum
      error(ERR_IDENTTOOLONG, resTok->lineNo, resTok->lineNo);
      return resTok;
  }

  resTok->string[strIdx] = '\0';
  resTok->tokenType = checkKeyword(resTok->string);

  if(resTok->tokenType == TK_NONE) {
    resTok->tokenType = TK_IDENT;
  }
  return resTok;
}

Token* readNumber(void) {
  // Make a token for later return
  Token *resTok = makeToken(TK_NUMBER, lineNo, colNo);
  int isFloat = 0; // a flag for checking float

  int strIdx = 0; // for pointing to character in string of resTok
  // Start reading string of number
  while((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_PERIOD)) {
    resTok->string[strIdx] = (char)currentChar;
    if(charCodes[currentChar] == CHAR_PERIOD) {
      isFloat++;
    }
    strIdx++;
    readChar();
  }
  
  // put terminate character to end the string
  resTok->string[strIdx] = '\0';
  if (isFloat == 1) {
    // this is a float
    resTok->value = atof(resTok->string);
    resTok->tokenType = TK_FLOAT;
  } else if (isFloat == 0) {
    // convert the string to integer value
    resTok->value = atoi(resTok->string);
  } else {
    resTok->tokenType = ERR_INVALIDSYMBOL;
  }
  return resTok;
}

Token* readConstChar(void) {
  // Make a token for later return
  Token *resTok = makeToken(TK_CHAR, lineNo, colNo);

  // Model:
  // character
  // -------- '       => OK (3)
  // -------- others  => ERROR (2)
  // EOF => ERROR (1)

  // currently we have a character
  // read the next character to check
  readChar();

  if(currentChar == EOF) { // (3)
    resTok->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, resTok->lineNo, resTok->colNo);
    return resTok;
  }
  
  resTok->string[0] = currentChar;
  resTok->string[1] = '\0';

  readChar();

  if(charCodes[currentChar] != CHAR_SINGLEQUOTE) { // (2)
    resTok->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, resTok->lineNo, resTok->colNo);
  }

  readChar();
  return resTok; // (1) & return anyway
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: {
    skipBlank(); 
    return getToken();
  }

  case CHAR_LETTER: {
    return readIdentKeyword();
  }

  case CHAR_DIGIT: {
    return readNumber();
  }

  case CHAR_SINGLEQUOTE: {
    return readConstChar();
  }

  case CHAR_PLUS: {
    // token = makeToken(SB_PLUS, lineNo, colNo);
    // readChar(); 
    // return token;
    // ln = lineNo;
    // cn = colNo;
    // readChar();
    // if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_PLUS)) {
    //   // check for +=
    //     readChar();
    //     return makeToken(SB_ASSIGN_PLUS, ln, cn);
    // } else return makeToken(SB_PLUS, ln, cn);
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      // check for :=
        readChar();
        return makeToken(SB_ASSIGN_MINUS, ln, cn);
    }  
    else return makeToken(SB_PLUS, ln, cn);
  }

  case CHAR_MINUS: {
    // token = makeToken(SB_MINUS, lineNo, colNo);
    // readChar();
    // return token;
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      // check for -=
        readChar();
        return makeToken(SB_ASSIGN_MINUS, ln, cn);
    } else return makeToken(SB_MINUS, ln, cn);
  }

  case CHAR_TIMES: {
    // token = makeToken(SB_TIMES, lineNo, colNo);
    // readChar();
    // return token;
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      // check for *=
        readChar();
        return makeToken(SB_ASSIGN_TIMES, ln, cn);
    } else return makeToken(SB_TIMES, ln, cn);
  }

  case CHAR_SLASH: {
    // token = makeToken(SB_SLASH, lineNo, colNo);
    // readChar();
    // return token;
    ln = lineNo;
    cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      // check for /=
        readChar();
        return makeToken(SB_ASSIGN_SLASH, ln, cn);
    } else return makeToken(SB_SLASH, ln, cn);
  }

  case CHAR_MOD: {
    token = makeToken(SB_MOD, lineNo, colNo);
    readChar();
    return token;
  }

  case CHAR_COMMA: {
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  }

  case CHAR_SEMICOLON: {
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  }

  case CHAR_RPAR: {
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar(); 
    return token;
  }

  case CHAR_LBRACKET: {
    token = makeToken(SB_LBRACKET, lineNo, colNo);
    readChar(); 
    return token;
  }

  case CHAR_RBRACKET: {
    token = makeToken(SB_RBRACKET, lineNo, colNo);
    readChar(); 
    return token;
  }
  case CHAR_EQ: {
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  }

  case CHAR_GT: {
    // greater than
    ln = lineNo;
    cn = colNo;
    readChar();
    // check for GE
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return token = makeToken(SB_GE, ln, cn);
    } else return token = makeToken(SB_GT, ln, cn);
  }

  case CHAR_LT: {
    // less than
    ln = lineNo;
    cn = colNo;
    readChar();
    // check for LE
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return token = makeToken(SB_LE, ln, cn);
    } else return token = makeToken(SB_LT, ln, cn);
  }

  case CHAR_EXCLAIMATION: {
    // exclaimation
    ln = lineNo;
    cn = colNo;
    readChar();
    // check for GE
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return token = makeToken(SB_NEQ, ln, cn);
    } else {
      error(ERR_INVALIDSYMBOL, ln, cn);
      return token = makeToken(TK_NONE, ln, cn);
    }
  }
  
  case CHAR_COLON: {
    ln = lineNo;
    cn = colNo;
    readChar();

    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      // check for :=
        readChar();
        return makeToken(SB_ASSIGN, ln, cn);
    }  
    else return makeToken(SB_COLON, ln, cn);
  }

  case CHAR_PERIOD: {
    ln = lineNo;
    cn = colNo;
    readChar();
    // if((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR)) {
    //   readChar();
    //   retur makeToken(SB_RSEL, ln, cn);
    // }
    return makeToken(SB_PERIOD, ln, cn);
  }

  case CHAR_LPAR: {
    ln = lineNo;
    cn = colNo;
    readChar();

    if (currentChar == EOF) 
      return makeToken(SB_LPAR, ln, cn);

    switch (charCodes[currentChar]) {
    // case CHAR_PERIOD: {
    //   // (..)
    //   readChar();
    //   return makeToken(SB_LBRACKET, ln, cn);
    // }
    case CHAR_TIMES: { 
      // (**)
      readChar();
      skipComment();
      return getToken();
    }
    default: {
      return makeToken(SB_LPAR, ln, cn);
    }
    }
  }
  
  default:{
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_FLOAT: printf("TK_FLOAT(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  case KW_FLOAT: printf("KW_FLOAT\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_MOD: printf("SB_MOD\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LBRACKET: printf("SB_LBRACKET\n"); break;
  case SB_RBRACKET: printf("SB_RBRACKET\n"); break;
  case SB_ASSIGN_PLUS: printf("SB_ASSIGN_PLUS\n"); break;
  case SB_ASSIGN_MINUS: printf("SB_ASSIGN_MINUS\n"); break;
  case SB_ASSIGN_TIMES: printf("SB_ASSIGN_TIMES\n"); break;
  case SB_ASSIGN_SLASH: printf("SB_ASSIGN_SLASH\n"); break;
  }
}

int scanFile(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}






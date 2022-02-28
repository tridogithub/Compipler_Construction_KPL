/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "./error.h"

void error(ErrorCode err, int lineNo, int colNo) {
  switch (err) {
  case ERR_ENDOFCOMMENT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_ENDOFCOMMENT);
    break;
  case ERR_IDENTTOOLONG:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_IDENTTOOLONG);
    break;
  case ERR_INVALIDCHARCONSTANT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCHARCONSTANT);
    break;
  case ERR_INVALIDSYMBOL:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDSYMBOL);
    break;
  case ERR_INVALIDCONSTANT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCONSTANT);
    break;
  case ERR_INVALIDTYPE:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDTYPE);
    break;
  case ERR_INVALIDBASICTYPE:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDBASICTYPE);
    break;
  case ERR_INVALIDPARAM:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDPARAM);
    break;
  case ERR_INVALIDSTATEMENT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDSTATEMENT);
    break;
  case ERR_INVALIDARGUMENTS:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDARGUMENTS);
    break;
  case ERR_INVALIDCOMPARATOR:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCOMPARATOR);
    break;
  case ERR_INVALIDEXPRESSION:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDEXPRESSION);
    break;
  case ERR_INVALIDTERM:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDTERM);
    break;
  case ERR_INVALIDFACTOR:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDFACTOR);
    break;
  case ERR_INVALIDCONSTDECL:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCONSTDECL);
    break;
  case ERR_INVALIDTYPEDECL:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDTYPEDECL);
    break;
  case ERR_INVALIDVARDECL:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDVARDECL);
    break;
  case ERR_INVALIDSUBDECL:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDSUBDECL);
    break;
  }
  exit(0);
}

void missingToken(TokenType tokenType, int lineNo, int colNo) {
  printf("%d-%d:Missing %s\n", lineNo, colNo, tokenToString(tokenType));
  exit(0);
}

void assert(char *msg) {
  printf("%s\n", msg);
}

char *tokenToString(TokenType tokenType) {
  switch (tokenType) {
  case TK_NONE: return "None";
  case TK_IDENT: return "an identification";
  case TK_NUMBER: return "a number";
  case TK_CHAR: return "a constant char";
  case TK_FLOAT: return "a float";
  case TK_EOF: return "end of file";

  case KW_PROGRAM: return "keyword PROGRAM";
  case KW_CONST: return "keyword CONST";
  case KW_TYPE: return "keyword TYPE";
  case KW_VAR: return "keyword VAR";
  case KW_INTEGER: return "keyword INTEGER";
  case KW_CHAR: return "keyword CHAR";
  case KW_ARRAY: return "keyword ARRAY";
  case KW_OF: return "keyword OF";
  case KW_FUNCTION: return "keyword FUNCTION";
  case KW_PROCEDURE: return "keyword PROCEDURE";
  case KW_BEGIN: return "keyword BEGIN";
  case KW_END: return "keyword END";
  case KW_CALL: return "keyword CALL";
  case KW_IF: return "keyword IF";
  case KW_THEN: return "keyword THEN";
  case KW_ELSE: return "keyword ELSE";
  case KW_WHILE: return "keyword WHILE";
  case KW_DO: return "keyword DO";
  case KW_FOR: return "keyword FOR";
  case KW_TO: return "keyword TO";
  case KW_FLOAT: return "keyword FLOAT";

  case SB_SEMICOLON: return "\';\'";
  case SB_COLON: return "\':\'";
  case SB_PERIOD: return "\'.\'";
  case SB_COMMA: return "\',\'";
  case SB_ASSIGN: return "\':=\'";
  case SB_EQ: return "\'=\'";
  case SB_NEQ: return "\'!=\'";
  case SB_LT: return "\'<\'";
  case SB_LE: return "\'<=\'";
  case SB_GT: return "\'>\'";
  case SB_GE: return "\'>=\'";
  case SB_PLUS: return "\'+\'";
  case SB_MINUS: return "\'-\'";
  case SB_TIMES: return "\'*\'";
  case SB_SLASH: return "\'/\'";
  case SB_MOD: return "\'%'\'";
  case SB_LPAR: return "\'(\'";
  case SB_RPAR: return "\')\'";
  // case SB_LSEL: return "\'(.\'";
  // case SB_RSEL: return "\'.)\'";
  // case SB_MOD: return "\%";
  case SB_ASSIGN_PLUS: return "+=";
  case SB_ASSIGN_MINUS: return "-=";
  case SB_ASSIGN_TIMES: return "*=";
  case SB_ASSIGN_SLASH: return "/=";
  case SB_LBRACKET: return "[";
  case SB_RBRACKET: return "]";
  default: return "";
  }
}
/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "./parser.h"


Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {
  while(lookAhead->tokenType == TK_IDENT) {
    compileConstDecl();
  }
}

void compileConstDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {
  while(lookAhead->tokenType == TK_IDENT) {
    compileTypeDecl();
  }
}

void compileTypeDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) {
  while(lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
  }
}

void compileVarDecl(void) {
  eat(TK_IDENT);
  if(lookAhead->tokenType == SB_COLON) {
    eat(SB_COLON);
    compileType();
    eat(SB_SEMICOLON); 
  }
}

void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  while(lookAhead->tokenType == KW_FUNCTION ||
        lookAhead->tokenType == KW_PROCEDURE) {
          switch(lookAhead->tokenType) {
            case KW_FUNCTION: 
            {
              compileFuncDecl();
              break;
            }
            case KW_PROCEDURE: 
            {
              compileProcDecl();
              break;
            }
            default:
            {
              error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
              break;
            }
          }
        }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
  if(lookAhead->tokenType == TK_NUMBER ||
      lookAhead->tokenType == TK_IDENT ||
      lookAhead->tokenType == TK_CHAR ||
      lookAhead->tokenType == TK_FLOAT) {
        eat(lookAhead->tokenType);
      } else {
        error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
      }
}

void compileConstant(void) {
  if(lookAhead->tokenType == SB_PLUS ||
    lookAhead->tokenType == SB_MINUS) {
      eat(lookAhead->tokenType);
      compileConstant2();
    } else if (lookAhead->tokenType == TK_NUMBER) {
      compileConstant2();
    } else if(lookAhead->tokenType == TK_CHAR) {
      eat(TK_CHAR);
    } else if(lookAhead->tokenType == TK_FLOAT) {
      compileConstant2();
    } else if (lookAhead->tokenType == TK_IDENT) {
      return;
    } else {
      error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileConstant2(void) {
  if(lookAhead->tokenType == TK_IDENT ||
    lookAhead->tokenType == TK_NUMBER ||
    lookAhead->tokenType == TK_FLOAT) {
      eat(lookAhead->tokenType);
    } else {
      error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
}

void compileType(void) {
  if(lookAhead->tokenType == KW_INTEGER ||
      lookAhead->tokenType == KW_CHAR ||
      lookAhead->tokenType == KW_FLOAT ||
      lookAhead->tokenType == TK_IDENT) {
        eat(lookAhead->tokenType);
      } else if (lookAhead->tokenType == KW_ARRAY) {
        eat(lookAhead->tokenType);
        eat(SB_LBRACKET);
        eat(TK_NUMBER);
        eat(SB_RBRACKET);
        eat(KW_OF);
        compileType();
      } else {
        error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
      }
}

void compileBasicType(void) {
  if(lookAhead->tokenType == KW_INTEGER ||
      lookAhead->tokenType == KW_CHAR ||
      lookAhead->tokenType == KW_FLOAT) {
        eat(lookAhead->tokenType);
      } else {
        error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
      }
}

void compileParams(void) {
  if(lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    if(lookAhead->tokenType == SB_RPAR) {
      eat(SB_RPAR);
    } else {
      error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
  }
}

void compileParams2(void) {
  while (lookAhead->tokenType == SB_SEMICOLON)
  {
    eat(SB_SEMICOLON);
    compileParam();
  }
}

void compileParam(void) {
  if(lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  } else if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  } else {
    error(ERR_INVALIDCHARCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileStatements(void) {
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  while(lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT: {
    compileAssignSt();
    break;
  }
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileVariable(void) {
  eat(TK_IDENT);
  compileIndexes();
}

void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  compileVariable();
  char* holder;
  switch(lookAhead->tokenType) {
    case SB_ASSIGN: {
      eat(SB_ASSIGN);
      holder = ":=";
      break;
    }
    case SB_ASSIGN_PLUS: {
      eat(SB_ASSIGN_PLUS);
      holder = "+=";
      break;
    }
    case SB_ASSIGN_MINUS: {
      eat(SB_ASSIGN_MINUS);
      holder = "-=";
      break;
    }
    case SB_ASSIGN_TIMES: {
      eat(SB_ASSIGN_TIMES);
      holder = "*=";
      break;
    }
    case SB_ASSIGN_SLASH: {
      eat(SB_ASSIGN_SLASH);
      holder = "/=";
      break;
    }
    default: {
      holder = "nil";
      break;
    }
  }
  compileExpression();
  // assert("Assign %s statement parsed ....", holder);
  printf("Assign %s statement parsed ....\n", holder);
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileArguments(void) {
  while(lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
  }
}

void compileArguments2(void) {
  while(lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
  }
}

void compileCondition(void) {
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  if(lookAhead->tokenType == SB_EQ ||
      lookAhead->tokenType == SB_NEQ ||
      lookAhead->tokenType == SB_GT ||
      lookAhead->tokenType == SB_GE ||
      lookAhead->tokenType == SB_LT ||
      lookAhead->tokenType == SB_LE) {
        eat(lookAhead->tokenType);
        compileExpression();
      } else {
        error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
      }
}

void compileExpression(void) {
  assert("Parsing an expression");
  // if(lookAhead->tokenType == SB_MINUS ||
  //     lookAhead->tokenType == SB_PLUS) {
  //       eat(lookAhead->tokenType);
  //     } 
  //     compileExpression2();
  // assert("Expression parsed");
  if(lookAhead->tokenType == SB_MINUS) {
    eat(SB_MINUS);
  } else if (lookAhead->tokenType == SB_PLUS) {
    eat(SB_PLUS);
  }
  compileExpression2();
  assert("Expression parsed");
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  switch(lookAhead->tokenType){
    case SB_PLUS: 
    case SB_MINUS:
    {
      eat(lookAhead->tokenType);
      compileTerm();
      compileExpression3();
    }
    // check the FOLLOW set
    // Fo(expression3) = Fo(expression2) = Fo(expression)
    // rule 63
    case KW_TO:
    case KW_DO:
    // rule 66
    case SB_COMMA:
    // rule 68 -> Fo(expression) = Fi(condition2)
    // rule 69 -> 74
    case SB_EQ:
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:
    // rule 56 -> Fo(expression) = Fo(AssignSt)
    // rule 49 -> Fo(AssignSt) = Fo(Statement)
    case SB_SEMICOLON:
    case KW_END:
    case KW_ELSE:
    // rule 69 -> Fo(expression) = Fo(condition2)
    // rule 68 -> Fo(condition2) = Fo(condition)
    // rule 59
    case KW_THEN:
    // rule 90
    case SB_RPAR:
    // rule 93
    case SB_RBRACKET:
      break;
    default:
    {
      error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
    }
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  switch (lookAhead->tokenType) {
  case SB_TIMES:
  case SB_SLASH:
  case SB_MOD:
  {
    eat(lookAhead->tokenType);
    compileFactor();
    compileTerm2();
    break;
  }
  // rule 82 -> Fo(Term2) = fo(term)
  // rule 78 -> Fo(Term) = fi(expression3)
  // rule 79
  case SB_PLUS:
  // rule 80
  case SB_MINUS:
  // rule 79 + 81 -> Fo(term) = fo (expression3)
  case KW_TO:
  case KW_DO:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
  case SB_RPAR:
  case SB_RBRACKET:
    break;
  default:
  {
    error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
  }
  }
}

void compileFactor(void) {
  switch(lookAhead->tokenType) {
    case TK_NUMBER:
    case TK_CHAR:
    case TK_FLOAT:
    {
      eat(lookAhead->tokenType);
      break;
    }
    case TK_IDENT: 
    {
      eat(lookAhead->tokenType);
      if (lookAhead->tokenType == SB_LBRACKET)
      {
        compileIndexes();
      }
      else if (lookAhead->tokenType == SB_LPAR)
      {
        compileArguments();
      }
      break;
    }
    case SB_LPAR:
    {
      eat(lookAhead->tokenType);
      compileExpression();
      eat(SB_RPAR);
      break;
    }
    default:
    {
      error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
      break;
    }
  }
}

void compileIndexes(void) {
  while(lookAhead->tokenType == SB_LBRACKET) {
    eat(SB_LBRACKET);
    compileExpression();
    eat(SB_RBRACKET);
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}

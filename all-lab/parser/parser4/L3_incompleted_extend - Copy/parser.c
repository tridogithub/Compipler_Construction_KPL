/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"


int checkSemicolon= 0;
Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken(); // lookAhead means the next token
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
  //TODO
  // assert("Parsing a constant decleration...");
  if (lookAhead->tokenType == TK_IDENT){
    compileConstDecl();
    compileConstDecls();
  }
  // else assert("Constant decleration parsed.");
}

void compileConstDecl(void) {
  // TODO
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {
  // TODO
  if (lookAhead->tokenType == TK_IDENT){
    compileTypeDecl();
    compileTypeDecls();
  }
  
  
}

void compileTypeDecl(void) {
  // TODO
  
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) {
  // TODO
  if (lookAhead->tokenType == TK_IDENT){
    compileVarDecl();
    compileVarDecls();

  }
}

void compileVarDecl(void) {
  // TODO
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) { //////////////////////////
  assert("Parsing subtoutines ....");
  // TODO
  if( lookAhead->tokenType == KW_FUNCTION){
    compileFuncDecl();
    compileSubDecls();
  }
  else if (lookAhead->tokenType == KW_PROCEDURE){
    compileProcDecl();
    compileSubDecls();
      
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  assert("Parsing a function ....");
  // TODO
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
  // TODO
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
  // TODO
  switch(lookAhead->tokenType){
    case TK_FLOAT:{
      eat(TK_FLOAT);
      break;
    }

    case TK_NUMBER :{ 
        eat(TK_NUMBER);
        break;
    }

    case TK_IDENT: {
      eat(TK_IDENT);
      break;
    }

    case TK_CHAR: {
      eat(TK_CHAR);
      break;
    }
    default:
      error(ERR_INVALIDCONSTANT,lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileConstant(void) {
  // TODO
  if (lookAhead -> tokenType == SB_PLUS){
    eat(SB_PLUS);
    compileConstant2();

  }

  else if ( lookAhead->tokenType == SB_MINUS){
    eat(SB_MINUS);
    compileConstant2();
  }
  else if ( lookAhead ->tokenType == TK_CHAR){
    eat(TK_CHAR);
  }
  else {
    compileConstant2();
  }
}

void compileConstant2(void) {
  // TODO
  switch(lookAhead->tokenType){
    case TK_FLOAT:
      eat(TK_FLOAT);
      break;

    case TK_IDENT:
      eat(TK_IDENT);
      break;
    case TK_NUMBER:
      eat(TK_NUMBER);
      break;

    default:
      error(ERR_INVALIDCONSTANT,lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileType(void) {
  // TODO
  switch(lookAhead->tokenType){
    case  KW_INTEGER:
      eat(KW_INTEGER);
      break;

    case KW_FLOAT:
      eat(KW_FLOAT);
      break;
    
    case KW_CHAR:
      eat(KW_CHAR);
      break;
    
    case TK_IDENT:
      eat(TK_IDENT);
      break;
    
    case KW_ARRAY:
      eat(KW_ARRAY);
      do {
        eat(SB_LBRA);
        eat(TK_NUMBER);
        eat(SB_RBRA);
      } while (lookAhead->tokenType == SB_LBRA);
      eat(KW_OF);
      compileType();
      break;

    default: 
      error(ERR_INVALIDTYPE, lookAhead ->lineNo, lookAhead->colNo);
      break;

  }
}

void compileBasicType(void) {
  // TODO

  switch(lookAhead->tokenType){
    case KW_INTEGER:
      eat(KW_INTEGER);
      break;
    
    case KW_FLOAT:
      eat(KW_FLOAT);
      break;
    
    case KW_CHAR:
      eat(KW_CHAR);
      break;

    default:
      error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileParams(void) {
  // TODO
  if (lookAhead ->tokenType == SB_LPAR){
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
  
}

void compileParams2(void) {
  // TODO

  if ( lookAhead ->tokenType == SB_SEMICOLON){
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
}

void compileParam(void) {
  // TODO
  switch (lookAhead->tokenType){
    case TK_IDENT:
      eat(TK_IDENT);
      eat(SB_COLON);
      compileBasicType();
      break;

    case KW_VAR:
      eat(KW_VAR);
      eat(TK_IDENT);
      eat(SB_COLON);
      compileBasicType();
      break;

    default:
      error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead ->colNo);
      break;
  }
  
}

void compileStatements(void) {
  // TODO
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  // TODO
  
  if (lookAhead->tokenType == SB_SEMICOLON){
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  }
  if (lookAhead ->tokenType != SB_SEMICOLON  && lookAhead->tokenType != KW_END){
    missingToken(SB_SEMICOLON,lookAhead->lineNo,lookAhead->colNo);
  }
  
  
}

void compileStatement(void) {
  checkSemicolon++;
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
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

void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  // TODO
  compileVariable();
  compileAssignSt_2();
  compileExpression();
  
  assert("Assign statement parsed ....");
}

void compileAssignSt_2 (void){
  switch(lookAhead->tokenType){

    case SB_ASSIGN :
      eat(SB_ASSIGN);
      break;
    case SB_ASSIGN_PLUS:
      eat(SB_ASSIGN_PLUS);
      break;
    case  SB_ASSIGN_SUBTRACT :
      eat(SB_ASSIGN_SUBTRACT);
      break;

    case SB_ASSIGN_TIMES :
      eat(SB_ASSIGN_TIMES);
      break;
    case SB_ASSIGN_DIVIDE:
      eat(SB_ASSIGN_DIVIDE);
      break;
  }
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  // TODO
  eat(KW_CALL);
  eat(TK_IDENT); // eat ProcedureIdent
  compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  // TODO
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
  // TODO
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  // TODO
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
  // TODO
  if (lookAhead-> tokenType == SB_LPAR){
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
  }
}

void compileArguments2(void) {
  // TODO
  if (lookAhead-> tokenType == SB_COMMA){
  eat(SB_COMMA);
  compileExpression();
  compileArguments2();
  }
}

void compileCondition(void) {
  // TODO
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  // TODO
  switch(lookAhead -> tokenType ){
    case SB_EQ:
      eat(SB_EQ);
      compileExpression();
      break;
    case SB_NEQ:
      eat(SB_NEQ);
      compileExpression();
      break;
    case SB_LE:
      eat(SB_LE);
      compileExpression();
      break;
    case SB_LT:
      eat(SB_LT);
      compileExpression();
      break;
    case SB_GE:
      eat(SB_GE);
      compileExpression();
      break;
    case SB_GT:
      eat(SB_GT);
      compileExpression();
      break;
    
  
  }
}

void compileExpression(void) {
  assert("Parsing an expression");
  // TODO

  // switch (lookAhead->tokenType)
  // {
  //   case SB_PLUS:
  //     eat(SB_PLUS);
  //     compileExpression2();
  //     break;
  //   case SB_MINUS:
  //     eat(SB_MINUS);
  //     compileExpression2();
  //     break;
  //   case SB_PERCENT:
  //     eat(SB_PERCENT);
  //     compileExpression2();
  //     break;
    
    
  // }
  if (lookAhead ->tokenType != SB_LPAR){
    if (lookAhead ->tokenType == SB_PLUS){
      eat(SB_PLUS);
      
      
    }
    else if (lookAhead ->tokenType == SB_MINUS){
      eat(SB_MINUS);
    
      
    }

    else if (lookAhead->tokenType == SB_PERCENT){
      eat(SB_PERCENT);
      
    }
    compileExpression2();
  }
  else {
    if (lookAhead->tokenType == SB_LPAR){
    eat(SB_LPAR);
    // compileExpression();
    compileCondition();
    eat(SB_RPAR);
    eat(SB_INTER);
    compileExpression();
    eat(SB_COLON); 
    compileExpression();
    }
  }



  
  
  assert("Expression parsed");
}

void compileExpression2(void) {
  // TODO
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  // TODO
  if ( lookAhead ->tokenType == SB_PLUS){
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
  }
  else if (lookAhead ->tokenType ==  SB_MINUS){
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
  }
  else if (lookAhead->tokenType == SB_PERCENT){
    eat(SB_PERCENT);
    compileTerm();
    compileExpression3();
  }
}

void compileTerm(void) {
  // TODO

  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  // TODO
  if (lookAhead ->tokenType == SB_TIMES){
    eat(SB_TIMES);
    compileFactor();
    compileTerm2();
  }
  else if (lookAhead ->tokenType == SB_SLASH){
    eat(SB_SLASH);
    compileFactor();
    compileTerm2();
  }
  else if (lookAhead ->tokenType == SB_PERCENT){
    eat(SB_PERCENT);
    compileFactor();
    compileTerm2();
  }
}

void compileFactor(void) {
  // TODO
  switch(lookAhead->tokenType){
    case TK_FLOAT:
      eat(TK_FLOAT);
      break;

    case TK_NUMBER:
      eat(TK_NUMBER);
      break;
    case TK_CHAR:
      eat(TK_CHAR);
      break;
    case TK_IDENT: ///////////////
      eat(TK_IDENT);
      if (lookAhead->tokenType == SB_LBRA){
        compileIndexes();
      }
      else {
        compileArguments();
      }
      break;
    case SB_LPAR:
      eat(SB_LPAR);
      compileExpression();
      eat(SB_RPAR);
      break;

    default: 
      error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
      break;
      
    
  }
}

void compileIndexes(void) {
  // TODO
  // if (lookAhead ->tokenType == SB_LSEL){
  //   eat(SB_LSEL);
  //   compileExpression();
  //   eat(SB_RSEL);
  //   compileIndexes();
  // }

  if (lookAhead ->tokenType == SB_LBRA){
    eat(SB_LBRA);
    compileExpression();
    eat(SB_RBRA);
    compileIndexes();

  }

}

void compileVariable(void){
  eat(TK_IDENT);
  compileIndexes();
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

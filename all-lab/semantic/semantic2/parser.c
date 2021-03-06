/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "semantics.h"
#include "error.h"
#include "debug.h"

Token *currentToken;
Token *lookAhead;

extern Type* intType;
extern Type* floatType;
extern Type* charType;
extern SymTab* symtab;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
  printToken(lookAhead);
}

void compileProgram(void) {
  Object* program;

  eat(KW_PROGRAM);
  eat(TK_IDENT);

  program = createProgramObject(currentToken->string);
  enterBlock(program->progAttrs->scope);

  eat(SB_SEMICOLON);

  compileBlock();
  eat(SB_PERIOD);

  exitBlock();
}

void compileBlock(void) {
  Object* constObj;
  ConstantValue* constValue;

  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);

    do {
      eat(TK_IDENT);
      // TODO: Check if a constant identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      // Create a constant object
      constObj = createConstantObject(currentToken->string);
      
      eat(SB_EQ);
      // Get the constant value
      constValue = compileConstant();
      constObj->constAttrs->value = constValue;
      // Declare the constant object 
      declareObject(constObj);
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock2();
  } 
  else compileBlock2();
}

void compileBlock2(void) {
  Object* typeObj;
  Type* actualType;

  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);

    do {
      eat(TK_IDENT);
      // TODO: Check if a type identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      // create a type object
      typeObj = createTypeObject(currentToken->string);
      
      eat(SB_EQ);
      // Get the actual type
      actualType = compileType();
      typeObj->typeAttrs->actualType = actualType;
      // Declare the type object
      declareObject(typeObj);
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  Object* varObj;
  Type* varType;

  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);

    do {
      eat(TK_IDENT);
      // TODO: Check if a variable identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      // Create a variable object      
      varObj = createVariableObject(currentToken->string);

      eat(SB_COLON);
      // Get the variable type
      varType = compileType();
      varObj->varAttrs->type = varType;
      // Declare the variable object
      declareObject(varObj);
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

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
  if(symtab->currentScope->owner->kind == OBJ_FUNCTION)
    compileStatements_f();
  compileStatements();
  eat(KW_END);
}

void compileSubDecls(void) {
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else compileProcDecl();
  }
}


void compileFuncDecl(void) {
  Object* funcObj;
  Type* returnType;
  int count = 0;
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  // TODO: Check if a function identifier is fresh in the block
  checkFreshIdent(currentToken->string);
  // create the function object
  funcObj = createFunctionObject(currentToken->string);
  // declare the function object
  declareObject(funcObj);
  // enter the function's block
  enterBlock(funcObj->funcAttrs->scope);
  // parse the function's parameters
  compileParams();
  eat(SB_COLON);
  // get the funtion's return type
  returnType = compileBasicType();
  funcObj->funcAttrs->returnType = returnType;

  eat(SB_SEMICOLON);

  compileBlock();

  eat(SB_SEMICOLON);
  // exit the function block
  exitBlock();
}

////////////////////////////////////
void compileStatements_f() {
  int count = 0;
  int i = 0;
  i = compileStatement_f();
  count += i;
  while (lookAhead->tokenType == SB_SEMICOLON) {
    i = 0;
    eat(SB_SEMICOLON);
    i = compileStatement_f();
    count += i;
  }
  // if (count == 0)
    // error(ERR_FUNCTION_RETURN, lookAhead->lineNo, lookAhead->colNo);
    // printf("WARNING: Missing function return\n");
}

int compileStatement_f(void) {
  Object* obj;
  int count = 0;
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    if(strcmp(lookAhead->string, symtab->currentScope->owner->name) == 0)
        count = 1;
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    count = compileIfSt_f();
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
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return count;
}

int compileIfSt_f(void) {
  int i;
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  i = compileStatement_f();
  if (lookAhead->tokenType == KW_ELSE) 
    i = compileElseSt_f();
  return i;
}

int compileElseSt_f(void) {
  eat(KW_ELSE);
  int i = compileStatement_f();
  return i;
}


///////////////////////////////////

void compileProcDecl(void) {
  Object* procObj;

  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  // TODO: Check if a procedure identifier is fresh in the block
  checkFreshIdent(currentToken->string);
  // create a procedure object
  procObj = createProcedureObject(currentToken->string);
  // declare the procedure object
  declareObject(procObj);
  // enter the procedure's block
  enterBlock(procObj->procAttrs->scope);
  // parse the procedure's parameters
  compileParams();

  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  // exit the block
  exitBlock();
}

ConstantValue* compileUnsignedConstant(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value.intvalue);
    break;
  case TK_FLOAT:
    eat(TK_FLOAT);
    constValue = makeFloatConstant(currentToken->value.floatvalue);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // TODO: check if the constant identifier is declared and get its value
    obj = checkDeclaredConstant(currentToken->string);
    if(obj != NULL)
      constValue = duplicateConstantValue(obj->constAttrs->value);
    else 
      error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

ConstantValue* compileConstant(void) {
  ConstantValue* constValue;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    constValue = compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    constValue = compileConstant2();
    constValue->intValue = - constValue->intValue;
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

ConstantValue* compileConstant2(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value.intvalue);
    break;
  case TK_FLOAT:
    eat(TK_FLOAT);
    constValue = makeFloatConstant(currentToken->value.floatvalue);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // TODO: check if the integer constant identifier is declared and get its value
    obj = checkDeclaredConstant(currentToken->string);
    if(obj != NULL)
      constValue = duplicateConstantValue(obj->constAttrs->value);
    else 
      error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

Type* compileType(void) {
  Type* type;
  Type* elementType;
  int arraySize;
  Object* obj;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER);
    type =  makeIntType();
    break;
  case KW_FLOAT: 
    eat(KW_FLOAT);
    type =  makeFloatType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    // eat(SB_LSEL);
    eat(SB_LBRACKET);
    // if(lookAhead->tokenType != TK_NUMBER)
    //   error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    eat(TK_NUMBER);

    arraySize = currentToken->value.intvalue;

    // eat(SB_RSEL);
    eat(SB_RBRACKET);
    eat(KW_OF);
    elementType = compileType();
    type = makeArrayType(arraySize, elementType);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // TODO: check if the type idntifier is declared and get its actual type
    obj = checkDeclaredType(currentToken->string);
    if(obj != NULL)
      type = duplicateType(obj->typeAttrs->actualType);
    else 
      error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

Type* compileBasicType(void) {
  Type* type;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER); 
    type = makeIntType();
    break;
  case KW_FLOAT: 
    eat(KW_FLOAT); 
    type = makeFloatType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON) {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
}

void compileParam(void) {
  Object* param;
  Type* type;
  enum ParamKind paramKind;

  switch (lookAhead->tokenType) {
  case TK_IDENT:
    paramKind = PARAM_VALUE;
    break;
  case KW_VAR:
    eat(KW_VAR);
    paramKind = PARAM_REFERENCE;
    break;
  default:
    error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
    break;
  }

  eat(TK_IDENT);
  // TODO: check if the parameter identifier is fresh in the block
  checkFreshIdent(currentToken->string);
  param = createParameterObject(currentToken->string, paramKind, symtab->currentScope->owner);
  eat(SB_COLON);
  type = compileBasicType();
  param->paramAttrs->type = type;
  declareObject(param);
}

void compileStatements(void) {
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

void compileStatement(void) {
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
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

Type* compileLValue(void) {
  Object* var;
  Type* varType;
  eat(TK_IDENT);
  // check if the identifier is a function identifier, or a variable identifier, or a parameter  
  var = checkDeclaredLValueIdent(currentToken->string);
  if (var->kind == OBJ_VARIABLE)
    varType = compileIndexes(var->varAttrs->type);
  else if(var->kind == OBJ_FUNCTION)
    varType = var->funcAttrs->returnType;
  else if(var->kind == OBJ_PARAMETER)
    varType = var->paramAttrs->type;
  
  return varType;
}

void compileAssignSt(void) {
  // TODO: parse the assignment and check type consistency
  Type* ltype;
  Type* rtype;
  ltype = compileLValue();
  switch(lookAhead->tokenType)
    {
      case SB_ASSIGN:
        eat(SB_ASSIGN);
        break;
      case SB_ASSIGN_MINUS:
        eat(SB_ASSIGN_MINUS);
        break;
      case SB_ASSIGN_PLUS:
        eat(SB_ASSIGN_PLUS);
        break;
      case SB_ASSIGN_SLASH:
        eat(SB_ASSIGN_SLASH);
        break;
      case SB_ASSIGN_TIMES:
        eat(SB_ASSIGN_TIMES);
        break;
    }
  rtype = compileExpression();
  checkTypeEquality(ltype, rtype);
}

void compileCallSt(void) {
  eat(KW_CALL);
  eat(TK_IDENT);
  // TODO: check if the identifier is a declared procedure
  Object* obj = checkDeclaredProcedure(currentToken->string);
  if( obj == NULL)
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  compileArguments(obj->procAttrs->paramList);
}

void compileGroupSt(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void) {
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void) {
  eat(KW_FOR);
  eat(TK_IDENT);
  Object* var = checkDeclaredVariable(currentToken->string);
  // TODO: check if the identifier is a variable
  if(var == NULL)
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  // checkBasicType_f(var->varAttrs->type);
  if(var->varAttrs->type->typeClass == TP_FLOAT)
    error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);

  eat(SB_ASSIGN);
  Type* typeEx = compileExpression();
  // checkBasicType_f(type1);
  if(typeEx->typeClass == TP_FLOAT)
    error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);

  eat(KW_TO);
  Type* type2 = compileExpression();
  // checkBasicType_f(type2);
  if(type2->typeClass == TP_FLOAT)
    error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);
  checkTypeEquality(var->varAttrs->type, typeEx);
  checkTypeEquality(typeEx, type2);

  eat(KW_DO);
  compileStatement();
}

void compileArgument(Object* param) {
  // TODO: parse an argument, and check type consistency
  //       If the corresponding parameter is a reference, the argument must be a lvalue
  if(param->paramAttrs->kind == PARAM_REFERENCE)
    {
      if(lookAhead->tokenType == TK_IDENT)
        checkDeclaredLValueIdent(lookAhead->string);
      else 
        error(ERR_TYPE_INCONSISTENCY, lookAhead->lineNo, lookAhead->colNo);
    }
  Type* arg = compileExpression();
  checkTypeEquality(arg, param->paramAttrs->type);
}

void compileArguments(ObjectNode* param) {
  switch (lookAhead->tokenType) {
  case SB_LPAR:
    eat(SB_LPAR);
    compileArgument(param->object);

    while (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      param = param->next;
      if(param != NULL)
        compileArgument(param->object);
      else 
        error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    }

    if(param->next != NULL)
      error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    
    eat(SB_RPAR);
    break;
    // Check FOLLOW set 
  case SB_TIMES:
  case SB_SLASH:
  case SB_MOD:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void) {
  Type* ex1 = compileExpression();
  checkBasicType(ex1);

  switch (lookAhead->tokenType) {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  Type* ex2 = compileExpression();
  checkBasicType(ex2);
  checkTypeEquality(ex1, ex2);
}

Type* compileExpression(void) {
  Type* type;
  
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    type = compileExpression2();
    if(type->typeClass != TP_INT && type->typeClass !=TP_FLOAT)
      error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    type = compileExpression2();
    if(type->typeClass != TP_INT && type->typeClass !=TP_FLOAT)
      error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    // checkIntType(type);
    break;
  default:
    type = compileExpression2();
  }
  return type;
}

Type* compileExpression2(void) {
  Type* type1;
  Type* type2;

  type1 = compileTerm();
  type2 = compileExpression3();
  if (type2 == NULL) return type1;
  else {
    checkTypeEquality(type1,type2);
    return type1;
  }
}


Type* compileExpression3(void) {
  Type* type1;
  Type* type2;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    type1 = compileTerm();
    // checkIntType(type1);
    check_int_float(type1);
    type2 = compileExpression3();
    if (type2 != NULL)
      // checkIntType(type2);
      check_int_float(type2);
    return type1;
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    type1 = compileTerm();
    // checkIntType(type1);
    check_int_float(type1);
    type2 = compileExpression3();
    if (type2 != NULL)
      // checkIntType(type2);
      check_int_float(type2);
    return type1;
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_RBRACKET:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    return NULL;
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

Type* compileTerm(void) {
  // TODO: check type of Term2
  Type* type;

  type = compileFactor();
  compileTerm2();

  return type;
}

void compileTerm2(void) {
  Type* type;
  switch (lookAhead->tokenType) {
  case SB_TIMES:
    eat(SB_TIMES);
    type = compileFactor();
    // checkIntType(type);
    check_int_float(type);
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    type = compileFactor();
    check_int_float(type);
    compileTerm2();
    break;
    // check the FOLLOW set
  case SB_MOD:
    eat(SB_MOD);
    type = compileFactor();
    compileTerm2();
    break;
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_RBRACKET:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

Type* compileFactor(void) {
  Object* obj;
  Type* type;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    type = makeIntType();
    break;
  case TK_FLOAT:
    eat(TK_FLOAT);
    type = makeFloatType();
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    type = makeCharType();
    break;
  case SB_LPAR:
    eat(SB_LPAR);
    type = compileExpression();
    eat(SB_RPAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // check if the identifier is declared
    if(lookAhead->tokenType == SB_LPAR)
    obj = checkDeclaredFunction(currentToken->string);
    obj = checkDeclaredIdent(currentToken->string);

    switch (obj->kind) {
    case OBJ_CONSTANT:
      // type = makeIntType();
      if(obj->constAttrs->value->type == TP_INT)
        type = makeIntType();
      if(obj->constAttrs->value->type == TP_FLOAT)
        type = makeFloatType();
      if(obj->constAttrs->value->type == TP_CHAR)
        type = makeCharType();
      type->typeClass = obj->constAttrs->value->type;
      break;
    case OBJ_VARIABLE:
      if( obj->varAttrs->type->typeClass != TP_ARRAY)
        type = obj->varAttrs->type;
      else 
        type = compileIndexes(obj->varAttrs->type);
      break;
    case OBJ_PARAMETER:
      type = obj->paramAttrs->type;
      break;
    case OBJ_FUNCTION:
      type = obj->funcAttrs->returnType;
      compileArguments(obj->funcAttrs->paramList);
      break;
    default:
      printf("%d", 1); 
      error(ERR_INVALID_FACTOR,currentToken->lineNo, currentToken->colNo);
      break;
    }
    break;
  default:
    printf("%d", 1); 
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
  return type;
}

Type* compileIndexes(Type* arrayT) {
  Type *idx;
  Type *elm;
  while (lookAhead->tokenType == SB_LBRACKET) {
    // eat(SB_LSEL);
    eat(SB_LBRACKET);
    checkArrayType(arrayT);
    idx = compileExpression();
    checkIntType(idx);
    // eat(SB_RSEL);
    eat(SB_RBRACKET);
    arrayT = arrayT->elementType;
  }
  elm = arrayT;
  return elm;
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  initSymTab();

  compileProgram();

  printObject(symtab->program,0);

  cleanSymTab();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
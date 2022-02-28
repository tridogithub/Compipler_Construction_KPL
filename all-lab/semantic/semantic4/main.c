/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "parser.h"

// #include "charcode.c"
// #include "debug.c"
// #include "error.c"
// #include "parser.c"
// #include "reader.c"
// #include "scanner.c"
// #include "symtab.c"
// #include "token.c"
// #include "semantics.c"

/******************************************************************/

int main(int argc, char *argv[]) {
  // compile("E:\\Desktop\\D&A Advanced\\day4_extend\\tests\\example4.kpl");
  // compile("E:\\Desktop\\D&A Advanced\\CHECK_me.txt");
  if (argc <= 1) {
    printf("parser: no input file.\n");
    return -1;
  }

  if (compile(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}

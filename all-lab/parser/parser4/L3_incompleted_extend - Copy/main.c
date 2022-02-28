/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

// #include "reader.h"
// #include "parser.h"

#include "reader.h"
#include "charcode.h"
#include "error.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"


// #include "reader.c"
// #include "charcode.c"
// #include "error.c"
// #include "parser.c"
// #include "scanner.c"
// #include "token.c"




/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    // compile("E:\\Desktop\\D&A Advanced\\L3_incompleted_extend - Copy\\test.kpl");

    
    printf("parser: no input file.\n");
    return -1;
  }

  if (compile(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}

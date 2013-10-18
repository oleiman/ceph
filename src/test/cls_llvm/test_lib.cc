#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

/*
 * This file is compiled into LLVM IR.
 */
extern "C" {

int log(void) 
{
   return -5;
}

char *retStr(string name)
{
  printf("Hello, %s, your number is\n", name.c_str());
  return strcat("Hello, ", name.c_str());
}

}

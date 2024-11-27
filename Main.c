#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include "encryption.c"
#include "operations.c"

int main(int argc, char const *argv[])
{
  if (strncmp(argv[1], "-cpy", 4) == 0){
    if (argc < 4){
      printf("Expecting 2 Files for Copy function \n");
      return 1;
    }
    copy(argv[2], argv[3]);
  } else if (strncmp(argv[1], "-df", 3) == 0){
    deleteFile(argv[2]);
  } else if (strncmp(argv[1], "-N", 2) == 0){
    makeFile(argv[2]);
  } else if (strncmp(argv[1], "-o", 2) == 0){
    readFile(argv[2]);
  } else if (strncmp(argv[1], "-a", 2) == 0){
    append(argv[2], argv[3]);
  } else if (strncmp(argv[1], "-i", 2) == 0){
    insert(argv[2], atoi(argv[3]), argv[4]);
  } else if (strncmp(argv[1], "-rl", 3) == 0){
    showLine(argv[2], atoi(argv[3]));
  } else if (strncmp(argv[1], "-dl", 3) == 0){
    deleteLine(argv[2], atoi(argv[3]));
  } else if (strncmp(argv[1], "-cl", 3) == 0){
    printf("%s contains %d lines\n", argv[1], countLines(argv[2]));
  } else if (strncmp(argv[1], "-h", 2) == 0){
    displayHelp();
  } else if (strncmp(argv[1], "-r", 2) == 0){
    if (copy(argv[2], argv[3]) == 0)
      deleteFile(argv[2]);
  } else if (strncmp(argv[1], "-cmp", 4) == 0){
    compareFiles(argv[2], argv[3]);
  } else if (strncmp(argv[1], "-enc", 4) == 0){
    encrypt((char* )argv[2]);
  } else if (strncmp(argv[1], "-dec", 4) == 0){
    decrypt((char*)argv[2]);
  } else if (strncmp(argv[1], "-log", 4) == 0){
    if (fileExists("log.txt") == 1)
      readFile("log.txt");
  } else {
    printf("Incorrect argument. Type -h for help\n");
  }

  

  return 0;
}




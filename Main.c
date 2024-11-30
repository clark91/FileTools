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
    for (int i = 2; i < argc; i++){
      deleteFile(argv[i]);
    }
  } else if (strncmp(argv[1], "-n", 2) == 0){
    for (int i = 2; i < argc; i++){
      makeFile(argv[2]);
    }
  } else if (strncmp(argv[1], "-o", 2) == 0){
    if (argc != 3){
      printf("Please enter the correct number of arguments (1)\n");
      return 1;
    }
    readFile(argv[2]);
  } else if (strncmp(argv[1], "-a", 2) == 0){
    if (argc != 4){
      printf("Please enter the correct number of arguments (2)\n");
      return 1;
    }
    append(argv[2], argv[3]);
  } else if (strncmp(argv[1], "-i", 2) == 0){
    if (argc != 5){
      printf("Please enter the correct number of arguments (3)\n");
      return 1;
    }
    insert(argv[2], atoi(argv[3]), argv[4]);
  } else if (strncmp(argv[1], "-rl", 3) == 0){
    if (argc != 4){
      printf("Please enter the correct number of arguments (2)\n");
      return 1;
    }
    showLine(argv[2], atoi(argv[3]));
  } else if (strncmp(argv[1], "-dl", 3) == 0){
    if (argc != 4){
      printf("Please enter the correct number of arguments (2)\n");
      return 1;
    }
    deleteLine(argv[2], atoi(argv[3]));
  } else if (strncmp(argv[1], "-cl", 3) == 0){
    for (int i = 2; i < argc; i++){
      printf("%s contains %d lines\n", argv[i], countLines(argv[i]));
    }
  } else if (strncmp(argv[1], "-h", 2) == 0){
    if (argc != 2){
      printf("Help command does not expect any arguments\n");
    }
    displayHelp();
  } else if (strncmp(argv[1], "-r", 2) == 0){
    if (argc != 4){
      printf("Please enter the correct number of arguments (3)\n");
    }
    if (copy(argv[2], argv[3]) == 0)
      deleteFile(argv[2]);
  } else if (strncmp(argv[1], "-cmp", 4) == 0){
    if (argc < 3){
      printf("Please enter at least 2 arguments\n");
      return 1;
    }
    for (int i = 3; i < argc; i++){
      compareFiles(argv[2], argv[i]);
    }
  } else if (strncmp(argv[1], "-enc", 4) == 0){
    if (argc != 3){
      printf("Please enter the correct number of arguments (2)\n");
      return 1;
    }
    encrypt((char* )argv[2]);
  } else if (strncmp(argv[1], "-dec", 4) == 0){
    if (argc != 3){
      printf("Please enter the correct number of arguments (2)\n");
      return 1;
    }
    decrypt((char*)argv[2]);
  } else if (strncmp(argv[1], "-log", 4) == 0){
    if (fileExists("log.txt") == 1)
      readFile("log.txt");
  } else {
    printf("Incorrect argument. Type -h for help\n");
    return 1;
  }

  

  return 0;
}




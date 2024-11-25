#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include "encryption.c"

int copy(const char* copyFrom, const char* copyTo);
int fileExists(const char* fileName);
int dirSplit(const char *target, char* dir, char* fileName);
int makeFile(const char* fileName);
int deleteFile(const char* fileName);
int readFile(const char* fileName);
int append(const char* fileName, const char* text);
int insert(const char* fileName, int line, const char* text);
int showLine(const char* fileName, int line);
int deleteLine(const char* fileName, int line);
int countLines(const char* fileName);
void displayHelp();
int compareFiles(const char* fileName1, const char* fileName2);

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
  } else {
    printf("Incorrect argument. Type -h for help\n");
  }

  

  return 0;
}

int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int copy(const char* copyFrom, const char* copyTo){

  // Checks that the file to copy exists and that you aren't overwriting a file by copying
  if (!fileExists(copyFrom)){
    printf("Cannot find file: %s \n", copyFrom);
    return 1;
  }
  
  if (fileExists(copyTo)){
    printf("File already exists: %s\n", copyTo);
    return 1;
  }

  FILE *copyFile = fopen(copyFrom, "r");
  fseek(copyFile, 0L, SEEK_END);
  int fileSize = ftell(copyFile);
  rewind(copyFile);
  
  char *fileBuffer = malloc(fileSize + 1);
  if (fileBuffer == NULL){
    printf("Malloc failed\n ");
    fclose(copyFile);
    return 1;
  }

  // Reads all of the file's data into a buffer 
  fread(fileBuffer, 1, fileSize, copyFile);
  fileBuffer[fileSize] = '\0';
  fclose(copyFile);

  // Writes the Buffer into a new file of the name specified by the user
  FILE *targetFile = fopen(copyTo, "w");
  fprintf(targetFile, "%s", fileBuffer);
  fclose(targetFile);
  free(fileBuffer);

  // Writes the user's operations to a log file.
  FILE *logFile = fopen("log.txt", "a"); 
  fprintf(logFile,"Successfully Copied %d lines from %s to %s \n", countLines(copyFrom), copyFrom, copyTo);
  printf("Successfully Copied %d lines from %s to %s \n", countLines(copyFrom), copyFrom, copyTo);
  fclose(logFile);
  return 0;
}

// This function tries to open the files that the user is trying to access. If they exist it returns 1 (true) otherwise, 0 (false)
int fileExists(const char* fileName){
  FILE *file = fopen(fileName, "r");
  if(file == NULL){
    return 0;
  }
  else{
    fclose(file);
    return 1;
  }
}

int makeFile(const char* fileName){
  // Ensures that the file doesn't already exist
  if(!fileExists(fileName)){
    // Creates the File with the name the user gave
    FILE *file = fopen(fileName, "w");
    fclose(file);

    // Checks that the file was correctly created
    if (fileExists(fileName)){
      printf("Successfully created file: %s\n", fileName);

      // If it is correctly created the operation is appended to the log file
      FILE *logFile = fopen("log.txt", "a"); 
      fprintf(logFile, "Successfully created file: %s with %d lines\n", fileName, countLines(fileName));  
      fclose(logFile);

      return 1;
    }
    printf("Failed to create file %s\n", fileName);
    return 0;
  }
  printf("File already exists: %s\n", fileName);
  return 0;
}

int deleteFile(const char* fileName){
  if (fileExists(fileName)){
    remove(fileName);
    
    FILE *logFile = fopen("log.txt", "a"); 
    fprintf(logFile, "File deleted: %s\n", fileName);  
    fclose(logFile);

    printf("File deleted: %s\n", fileName);
    return 0;
  }
  printf("Could not delete file\n");
  return 2;
}

int readFile(const char* fileName){
  FILE *file = fopen(fileName, "r");
  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  // Reads the file's content into a buffer and prints it out to the user
  char *fileBuffer = malloc(size + 1);
  fread(fileBuffer, 1, size, file);
  fileBuffer[size] = '\0';

  fclose(file);
  printf("%s\n",fileBuffer);

  free(fileBuffer);
  return 1;
}

int append(const char* fileName, const char* text){
  if (fileExists(fileName)){
    FILE *file = fopen(fileName, "a");
    fprintf(file,"%s\n", text);
    fclose(file);
    
    // Log append operation
    FILE *logFile = fopen("log.txt", "a"); 
    fprintf(logFile, "Appended following text to %s: %s. There are now %d lines\n", fileName, text, countLines(fileName));  
    fclose(logFile);

    return 1;
  }
  return 0;
}

int insert(const char* fileName, int line, const char* text){
  if (fileExists(fileName)){
    FILE *file = fopen(fileName, "r");
    
    fseek(file, 0L, SEEK_END);
    int fileSize = ftell(file);
    rewind(file);

    char *lineBuf = malloc(fileSize + 1);
    char *fileBuf = malloc(fileSize + strnlen(text, strlen(text)) + 1);

    // Iterates up to the user's specified line and appends each line to the file buffer
    int i = 0;
    for (i = 0; i < line; i++){
      fgets(lineBuf, fileSize + 1, file);
      strncat(fileBuf, lineBuf, strlen(lineBuf));
    }

    // When at the desired line, appends the user's text to the buffer
    strncat(fileBuf, text, strlen(text));
    fileBuf[strlen(fileBuf)] = '\n';

    // Iterates through the remaining lines in the file and appends them to the filebuffer
    while (fgets(lineBuf, fileSize+1, file)){
      strncat(fileBuf, lineBuf, strlen(lineBuf));
    }

    // Prints the buffer out to the file (Overrwrites all data in the file with the buffer)
    fclose(file);
    file = fopen(fileName, "w");
    fprintf(file, "%s", fileBuf);
    fclose(file);
    
    // Logs the operation
    FILE *logFile = fopen("log.txt", "a"); 
    fprintf(logFile, "Inserted the following text at line %d of %s: %s\nThere are now %d lines", line, fileName, text, countLines(fileName));  
    fclose(logFile);


    free(lineBuf);
    free(fileBuf);
    return 0;
  }
  return 1;
}

int showLine(const char* fileName, int line){
  if (fileExists(fileName)){
    FILE *file = fopen(fileName, "r");

    fseek(file, 0L, SEEK_END);
    int fileSize = ftell(file);
    rewind(file);

    // Iterates through each line in the file and stores it in a buffer (This way if the user's line is out of rage it will just read the last line in the file)
    char* lineBuf = malloc(fileSize + 1);
    for (int i = 0; i < line; i++){
      fgets(lineBuf, fileSize + 1, file);
    }

    printf("%s", lineBuf);
    free(lineBuf);
    fclose(file);
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

int deleteLine(const char* fileName, int line){
  if (fileExists(fileName)){
    FILE *file = fopen(fileName, "r");
    
    fseek(file, 0L, SEEK_END);
    int fileSize = ftell(file);
    rewind(file);

    char *lineBuf = malloc(fileSize + 1);
    char *fileBuf = malloc(fileSize + 1);

    // Iterates through each line in the file and stores it in a line buffer that is appended to the file buffer only if it is not from the line specified by the user
    int i = 1;
    while (fgets(lineBuf, fileSize+1, file)){
      if (i != line)
        strncat(fileBuf, lineBuf, strlen(lineBuf));
      i++;
    }

    // The file buffer is printed out to file, missing the one line specified by the user
    fclose(file);
    file = fopen(fileName, "w");
    fprintf(file, "%s", fileBuf);
    fclose(file);
    
    FILE *logFile = fopen("log.txt", "a"); 
    fprintf(logFile, "Line %d deleted from %s. There are now %d lines\n", line, fileName, countLines(fileName));  
    fclose(logFile);


    free(lineBuf);
    free(fileBuf);
    return 0;
  }
  return 1;
}

int countLines(const char* fileName){
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    printf("Could not find file\n");
    return -1;
  }
  
  size_t fileSize = 0;
  // Iterates through each line in the file and increments the counter until it reaches the end of the file
  int count = 0;
  for (char c = getc(file); c != EOF; c = getc(file)){
    fileSize ++;
    if (c == '\n') 
      count = count + 1;
  }
    
  
  fclose(file);

  return fileSize == 0 ? 0 : count + 1;
}

void displayHelp(){
  printf(" -N <File Name> : creates a new file\n -cpy <File to copy from> <File to copy to> : copy file\n -df <File Name> : Deletes the file \n -o <File Name> : Displays the content of the file \n "
  "-a <File Name> <Text> : Appends the text to the end of the specified file \n -dl <File Name> <Line number> : Deletes the specified line from the file \n -i <File Name> <Line> <Text> : Inserts the text into the specified line of the file\n "
  "-rl <File Name> <Line> : Displays the content of the specific line of a file.\n -cl <File Name> : Prints the number in the file \n "
  "-r <Old File Name> <New File Name> : Renames a file from its old name to its new name \n -cmp <File 1> <File2> : Compares two files and checks if they are identical\n "
  "-enc <File Name> : Encrypts the file\n -dec <File Name> : Decrypts the file\n");
}

int compareFiles(const char* fileName1, const char* fileName2){
  FILE *f1 = fopen(fileName1, "r");
  FILE *f2 = fopen(fileName2, "r");
  if (!f1 || !f2) {
    printf("Error opening files");
    fclose(f1);
    fclose(f2);
    return 2;
  }

  fseek(f1, 0L, SEEK_END);
  size_t f1Size = ftell(f1);
  rewind(f1);
  fseek(f2, 0L, SEEK_END);
  size_t f2Size = ftell(f2);
  rewind(f2);

  char* line1 = malloc(f1Size);
  char* line2 = malloc(f2Size);

  // This iterates through each line in both files and stores each line in a buffer. It uses strncmp to check if the two lines are identical.
  int line = 1;
  int differences = 0;
  while(fgets(line1, f1Size, f1) && fgets(line2, f2Size, f2)){
    if(strncmp(line1, line2, max(strlen(line1), strlen(line2))) != 0){
      printf("Difference on line %d\n", line);
      differences++;
    }
    line++;
  }

  free(line1);
  free(line2);

  if (differences != 0){
    printf("These files have differences on %d lines\n", differences);
    return 1;
  }

  printf("These files are identical\n");
  return 0;
}
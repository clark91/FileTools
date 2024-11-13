#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

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
  /*if (argc < 3){ // Ensuring the correct number of arguments is given.
    printf("Expecting at least 2 arguments\n");
    return 1;
  }*/

  char arg = argv[1][1];
  const char *fileMain = argv[2];

  switch (arg)
  {
  case 'D': // Delete File
    deleteFile(fileMain);
    break;

  case 'n':
    makeFile(fileMain);
    break;

  case 'C':
    const char *fileChild = argv[3];
    copy(fileMain, fileChild);
    break;

  case 'o':
    readFile(fileMain);
    break;

  case 'a':
    const char* text = argv[3];
    append(fileMain, text);
    break;

  case 'i':
    text = argv[4];
    int line = atoi(argv[3]);
    insert(fileMain, line, text);
    break; 

  case 's':
    line = atoi(argv[3]);
    showLine(fileMain, line);
    break;

  case 'd':
    line = atoi(argv[3]);
    deleteLine(fileMain, line);
    break;

  case 'l':
    countLines(fileMain);
    break;

  case 'h':
    displayHelp();  
    break;

  case 'r':
    fileChild = argv[3];
    if (copy(fileMain, fileChild) == 0)
      deleteFile(fileMain);
    break;
  
  case 'c':
    fileChild = argv[3];
    compareFiles(fileMain, fileChild);
    break;

  default:
    printf("Incorrect argument: %s\n", argv[1] + 1);
    break;
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

// The function for copying the contents of one file to another
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
  FILE *logFile = fopen("log", "a"); 
  fprintf(logFile, "Successfully Copied %s to %s \n", copyFrom, copyTo);
  printf("Successfully Copied %s to %s \n", copyFrom, copyTo);
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
      FILE *logFile = fopen("log", "a"); 
      fprintf(logFile, "Successfully created file: %s\n", fileName);  
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
    
    FILE *logFile = fopen("log", "a"); 
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
    FILE *logFile = fopen("log", "a"); 
    fprintf(logFile, "Appended following text to %s: %s\n", fileName, text);  
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
    FILE *logFile = fopen("log", "a"); 
    fprintf(logFile, "Inserted the following text at line %d of %s: %s\n", line, fileName, text);  
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
    
    FILE *logFile = fopen("log", "a"); 
    fprintf(logFile, "Line %d deleted from %s\n", line, fileName);  
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
    return 2;
  }

  // Iterates through each line in the file and increments the counter until it reaches the end of the file
  int count = 0;
  for (char c = getc(file); c != EOF; c = getc(file))
        if (c == '\n') 
            count = count + 1;
  
  printf("%s contains %d lines \n", fileName, count);
  fclose(file);
}

void displayHelp(){
  printf(" -n <File Name> : creates a new file\n -C <File to copy from> <File to copy to> : copy file\n -D <File Name> : Deletes the file \n -o <File Name> : Displays the content of the file \n "
  "-a <File Name> <Text> : Appends the text to the end of the specified file \n -d <File Name> <Line number> : Deletes the specified line from the file \n -i <File Name> <Line> <Text> : Inserts the text into the specified line of the file\n "
  "-s <File Name> <Line> : Displays the content of the specific line of a file.\n log : Displays a log of all recent operations peformed on files.\n -l <File Name> : Prints the number in the file \n "
  "-r <Old File Name> <New File Name> : Renames a file from its old name to its new name \n -c <File 1> <File2> : Compares two files and checks if they are identical\n");
}

// This compares two files and checks whether or not they are identical
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
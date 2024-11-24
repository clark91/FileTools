//#include "fileTools.c"
#include <string.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

int keyGen(unsigned char salt[], char* out);
int keyGenNoSalt(char* outKey, char* outSalt);
int encrypt(char* fileName);
int decrypt(char* fileName);

int main(int argc, char const *argv[]){
  char *fileName = "test.txt";
  if(!strcmp(argv[1], "e")){
    printf("Encrypt\n");
    encrypt(fileName);
  } else if (!strcmp(argv[1], "d")){
    printf("Decrypt\n");
    decrypt(fileName);
  }
}

void to_hex_string(const unsigned char *data, size_t len, char *out) {
    for (size_t i = 0; i < len; i++) {
        sprintf(out + (i * 2), "%02x", data[i]);  
    }
    out[len * 2] = '\0';  
}

int keyGen(unsigned char salt[16], char* out){

  //unsigned char salt[16];
  char password[256];
  printf("Enter Password: ");
  scanf("%s", password);
 

  int iterations = 100000;
  int key_len = 16;
  int salt_len = 16;

  const EVP_MD *hash_function = EVP_sha256();

  char key[key_len];
  PKCS5_PBKDF2_HMAC(password, strlen(password), salt,salt_len, iterations, hash_function, key_len, key);

  char key_hex[key_len * 2 + 1];
  char salt_hex[salt_len * 2 + 1];

  to_hex_string(key, key_len, key_hex);
  to_hex_string(salt, salt_len, salt_hex);

  printf("Key: %s\nSalt: %s\n", key_hex, salt_hex);
  strncpy(out, key, 16);

  return 0;
}

int keyGenNoSalt(char *outKey, char *outSalt){
  unsigned char salt[16];
  if (!RAND_bytes(salt, sizeof(salt))) {
    fprintf(stderr, "Error generating salt.\n");
    return 1;
  }
  
  char key[16];
  keyGen(salt, key);

  strncpy(outKey, key, 16);
  strncpy(outSalt, salt, 16);
  return 0;
}

// XOR encrytpion
int encrypt(char* fileName){

  char key[16];
  unsigned char salt[16];
  keyGenNoSalt(key, salt);

  // Opens and reads plaintext file to buffer
  FILE* file = fopen(fileName, "rb");

  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *fileBuf = malloc(fileSize * sizeof(char));
  fread(fileBuf, fileSize, 1, file);
  fclose(file);

  // Reads Buffer and does XOR encryption. Stores to an encoded buffer
  char *encBuf = malloc(fileSize * sizeof(char));

  for (int i = 0; i < fileSize; i++){
    encBuf[i] = fileBuf[i] ^ key[i % 16];
  }
  encBuf[fileSize] = '\0';

  printf("Key used: %s \n", key);
  printf("Salt used: %s \n", salt);
  // Reverse Test
  for (int i = 0; i < fileSize; i++){
    printf("%c\n", encBuf[i] ^ key[i % 16]);
  }

  printf("Output: %s \n", encBuf);

  // Creates a final buffer with the encrypted message and the salt used.
  char *outputBuf = malloc((fileSize + 16) * sizeof(char));
  
  memcpy(outputBuf, salt, 16);
  memcpy(outputBuf + 16, encBuf, fileSize);
  outputBuf[fileSize + 16] = '\0';

  file = fopen(fileName, "wb");
  fprintf(file, "%s", outputBuf);
  fclose(file);


  free(fileBuf);
  free(encBuf);
  free(outputBuf);
  printf("%s Successfully encrypted\n", fileName);
  return 0;
}

int decrypt(char* fileName){
  FILE *file = fopen(fileName, "rb");
  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  if(fileSize < 16){
    printf("File cannot be decrypted\n");
    return 1;
  }

  char salt[16];
  char salt_hex[33];
  fread(salt, 1, 16, file);

  to_hex_string(salt, 16, salt_hex);
  printf("%s\n", salt_hex);

  char key[16];
  keyGen(salt, key);

  char *fileBuf = malloc((fileSize - 16) * sizeof(char));
  char *outBuf = malloc((fileSize - 16) * sizeof(char));

  fread(fileBuf, 1, fileSize - 16, file);
  fclose(file);

  fileBuf[fileSize -16] = '\0';
  printf("%s\n", fileBuf);

  for (int i = 0; i < fileSize - 16; i++){
    printf("%d : %c ^ %c = %c\n", i, fileBuf[i], key[i % 16], fileBuf[i] ^ key[i % 16]);
    outBuf[i] = fileBuf[i] ^ key[i % 16];
  }

  outBuf[fileSize - 16] = '\0';
  printf("%s\n", outBuf);

  file = fopen(fileName, "wb");
  fprintf(file, "%s", outBuf);
  fclose(file);

  free(fileBuf);
  free(outBuf);
  return 0;
}
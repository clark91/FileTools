#include <string.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

// Converts a string of bytes into hex (Used for debugging primarily)
void to_hex_string(const unsigned char *data, size_t len, char *out) {
    for (size_t i = 0; i < len; i++) {
        sprintf(out + (i * 2), "%02x", data[i]);  
    }
    out[len * 2] = '\0';  
}

// A function to generate a 128 bit key from a password and a salt
int keyGen(unsigned char salt[16], char* out){

  // Takes in a password of up to 256 B 
  char password[256];
  printf("Enter Password: ");
  scanf("%s", password);
 
  // Constants for the PBKDF2 function
  const int iterations = 100000;
  const int key_len = 16;
  const int salt_len = 16;

  // The Hashing type used (in this case sha256)
  const EVP_MD *hash_function = EVP_sha256();

  char key[key_len];
  PKCS5_PBKDF2_HMAC(password, strlen(password), salt,salt_len, iterations, hash_function, key_len, key);

  //printf("Key: %s\nSalt: %s\n", key_hex, salt_hex);
  strncpy(out, key, 16);

  return 0;
}

// Generates the 128 bit key from a password and generates a salt
int keyGenNoSalt(char *outKey, char *outSalt){

  // Generates a salt of 16 random bytes 
  unsigned char salt[16];
  if (!RAND_bytes(salt, sizeof(salt))) {
    fprintf(stderr, "Error generating salt.\n");
    return 1;
  }
  
  char key[16];
  keyGen(salt, key);

  memcpy(outKey, key, 16);
  memcpy(outSalt, salt, 16);
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

  // Creates a final buffer with the encrypted message and the salt used. The first 16 bytes will be the salt
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

  FILE *logFile = fopen("log", "a"); 
  fprintf(logFile, "Encrypted file %s\n", fileName);  
  fclose(logFile);

  return 0;
}

// XOR decryption 
int decrypt(char* fileName){

  FILE *file = fopen(fileName, "rb");
  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  // If the file is less than 16 bytes there is no salt at the start as this creates a minimum of 16 bytes. It must not be encrypted
  if(fileSize < 16){
    printf("File cannot be decrypted\n");
    return 1;
  }

  // The first 16 bytes are read as the salt and a key is generated from this
  char salt[16];
  char key[16];
  fread(salt, 1, 16, file);
  keyGen(salt, key);

  char *fileBuf = malloc((fileSize - 16) * sizeof(char));
  char *outBuf = malloc((fileSize - 16) * sizeof(char));

  // The remaining bytes of the file are read to a buffer
  fread(fileBuf, 1, fileSize - 16, file);
  fclose(file);
  fileBuf[fileSize -16] = '\0';

  // Iterates through the file and performs an XOR decryption. 
  for (int i = 0; i < fileSize - 16; i++){
    outBuf[i] = fileBuf[i] ^ key[i % 16];
  }
  outBuf[fileSize - 16] = '\0';

  file = fopen(fileName, "wb");
  fprintf(file, "%s", outBuf);
  fclose(file);

  free(fileBuf);
  free(outBuf);

  FILE *logFile = fopen("log", "a"); 
  fprintf(logFile, "Decrypted file %s\n", fileName);  
  fclose(logFile);

  return 0;
}


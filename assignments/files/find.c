#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 1000
#define BUFFER_SIZE 4096 * 16

int count_word_in_file(const char* filename, const char* search_word) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening file: %s\n", filename);
    return -1;
  }

  char buffer[BUFFER_SIZE + 1];
  int count       = 0;
  size_t word_len = strlen(search_word);

  while (1) {
    // Read chunk of the file
    size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
    if (bytes_read == 0)
      break;

    // Null-terminate buffer
    buffer[bytes_read] = '\0';

    // Substring search
    for (size_t i = 0; i <= bytes_read - word_len; i++) {
      int match = 1;
      for (size_t j = 0; j < word_len; j++) {
        if (buffer[i + j] != search_word[j]) {
          match = 0;
          break;
        }
      }
      if (match) {
        count++;
        i += word_len - 1; // Skip to end of current match
      }
    }

    if (bytes_read < BUFFER_SIZE)
      break;
  }

  // You really should close the file but it says we can't use it lmao.
  // fclose(file);
  return count;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: %s <file1> <file2> ...\n", argv[0]);
    return 1;
  }

  // Prompt for search word
  char search_word[MAX_WORD_LENGTH];
  printf("Enter a word: ");

  if (scanf("%[^\n]", search_word) != 1) {
    printf("Error reading search word\n");
    return 1;
  }

  // Process each file in a separate child process
  for (int i = 1; i < argc; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      printf("Fork failed for file %s\n", argv[i]);
      return 1;
    }

    if (pid == 0) {
      int count = count_word_in_file(argv[i], search_word);

      if (count >= 0) {
        printf("%s - %d\n", argv[i], count);
      }

      exit(0);
    }
  }

  for (int i = 1; i < argc; i++) {
    wait(NULL);
  }

  return 0;
}

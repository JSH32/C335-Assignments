#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// THIS IS A BETTER IMPLEMENTATION
// Uploaded after my original submission just to see if can be made better
// There is an original implementation before due date so handgraders can grade
// that one if needed. Tis is for me (and you if you want)

#define MAX_WORD_LENGTH 1000
#define BUFFER_SIZE 4096 * 16

int count_word_in_file(const char* filename, const char* search_word) {
  // Open file
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    return -1;
  }

  char buffer[BUFFER_SIZE + 1];
  int count    = 0;
  int word_len = strlen(search_word);

  char leftover[MAX_WORD_LENGTH] = { 0 };
  int leftover_len               = 0;

  while (1) {
    // Read chunk of the file
    int bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
    if (bytes_read == 0) {
      break;
    }

    buffer[bytes_read] = '\0';

    // Combine leftover and the buffer
    char combined[BUFFER_SIZE + MAX_WORD_LENGTH + 1];
    for (int i = 0; i < leftover_len; i++) {
      combined[i] = leftover[i];
    }
    for (int i = 0; i < bytes_read; i++) {
      combined[leftover_len + i] = buffer[i];
    }

    int combined_len       = leftover_len + bytes_read;
    combined[combined_len] = '\0';

    // Search for the word manually in the combined array
    for (int i = 0; i <= combined_len - word_len; i++) {
      int match = 1;
      for (int j = 0; j < word_len; j++) {
        if (combined[i + j] != search_word[j]) {
          match = 0;
          break;
        }
      }
      if (match) {
        count++;
        i += word_len - 1; // Skip to the end of the word
      }
    }

    // Update leftover for the next iteration
    if (combined_len >= word_len) {
      leftover_len = word_len - 1;
      for (int i = 0; i < leftover_len; i++) {
        leftover[i] = combined[combined_len - word_len + 1 + i];
      }
      leftover[leftover_len] = '\0';
    } else {
      leftover_len = combined_len;
      for (int i = 0; i < leftover_len; i++) {
        leftover[i] = combined[i];
      }
      leftover[leftover_len] = '\0';
    }

    if (bytes_read < BUFFER_SIZE) {
      break;
    }
  }

  // Close the file
  fclose(file);
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

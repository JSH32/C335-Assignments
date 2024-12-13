#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 1000
#define BUFFER_SIZE 4096 * 16

typedef struct {
  const char* filename;
  size_t start;
  size_t end;
  const char* search_word;
  size_t word_len;
  int count;
} ThreadArgs;

void* thread_search(void* arg) {
  ThreadArgs* args = (ThreadArgs*)arg;
  int count        = 0;

  FILE* file = fopen(args->filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    args->count = -1;
    return NULL;
  }

  fseek(file, args->start, SEEK_SET);
  size_t segment_size = args->end - args->start;

  char* buffer = malloc(segment_size + 1);
  if (buffer == NULL) {
    fclose(file);
    printf("Memory allocation failed for file: %s\n", args->filename);
    args->count = -1;
    return NULL;
  }

  fread(buffer, 1, segment_size, file);
  buffer[segment_size] = '\0';
  fclose(file);

  for (size_t i = 0; i <= segment_size - args->word_len; i++) {
    if (strncmp(buffer + i, args->search_word, args->word_len) == 0) {
      count++;
      i += args->word_len - 1;
    }
  }

  free(buffer);
  args->count = count;
  return NULL;
}

int count_word_in_file(const char* filename, const char* search_word, int num_threads) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening file: %s\n", filename);
    return -1;
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fclose(file);

  size_t word_len = strlen(search_word);
  pthread_t threads[num_threads];
  ThreadArgs thread_args[num_threads];

  // Divide the file data in threads
  size_t segment_size = file_size / num_threads;
  int total_count     = 0;

  for (int i = 0; i < num_threads; i++) {
    thread_args[i].filename    = filename;
    thread_args[i].start       = i * segment_size;
    thread_args[i].end         = (i == num_threads - 1) ? file_size : (i + 1) * segment_size + word_len - 1;
    thread_args[i].search_word = search_word;
    thread_args[i].word_len    = word_len;
    thread_args[i].count       = 0;

    pthread_create(&threads[i], NULL, thread_search, &thread_args[i]);
  }

  // Join and sum results from threads
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    total_count += thread_args[i].count;
  }

  return total_count;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: %s <num_threads> <file1> <file2> ...\n", argv[0]);
    return 1;
  }

  int num_threads = atoi(argv[1]);
  if (num_threads <= 0) {
    printf("Invalid number of threads: %d\n", num_threads);
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
  for (int i = 2; i < argc; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      printf("Fork failed for file %s\n", argv[i]);
      return 1;
    }

    if (pid == 0) {
      int count = count_word_in_file(argv[i], search_word, num_threads);

      if (count >= 0) {
        printf("%s - %d\n", argv[i], count);
      }

      exit(0);
    }
  }

  for (int i = 2; i < argc; i++) {
    wait(NULL);
  }

  return 0;
}

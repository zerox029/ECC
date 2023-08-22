//
// Created by emma on 21/08/23.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"


void error_at(char* loc, char* input, char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - input;
  fprintf(stderr, "%s\n", input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool startsWith(char* p, char* q) {
  return memcmp(p, q, strlen(q)) == 0;
}

bool isAlphanum(char p) {
  return ('a' <= p && p <= 'z') ||
      ('A' <= p && p <= 'Z') ||
      ('0' <= p && p <= '9') ||
      (p == '_');
}

char* generateRandomLabel() {
  const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  const int charsetSize = sizeof(charset) - 1;

  srand(time(NULL));

  char* label = (char*)malloc(4);  // Allocate memory for the label (3 characters + null terminator)
  if (!label) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < 3; ++i) {
    int randomIndex = rand() % charsetSize;  // Generate a random index within the charset
    label[i] = charset[randomIndex];         // Add a random character to the label
  }
  label[3] = '\0';  // Null-terminate the label

  return label;
}
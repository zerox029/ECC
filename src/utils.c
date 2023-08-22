//
// Created by emma on 21/08/23.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
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
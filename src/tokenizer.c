//
// Created by emma on 8/16/23.
//

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "tokenizer.h"
#include "generator.h"
#include "utils.h"

Token* token;

struct Symbol {
  char* name;
  TokenKind kind;
};

struct Symbol symbols[] = {
{"==", TK_EQ}, {"!=", TK_NE}, {"<=", TK_LTE},
{">=", TK_GTE}, {"++", TK_INCREMENT }, {"--", TK_DECREMENT},
{"<", TK_LT}, {">", TK_GT}, {"+", TK_PLUS}, {"-", TK_MINUS},
{"*", TK_STAR}, {"/", TK_SLASH}, {"=", TK_ASSIGN},
{"(", TK_OP_PAR}, {")", TK_CL_PAR}, {";", TK_SMCOLON},
{"{", TK_OP_BLK}, {"}", TK_CL_BLK}, {",", TK_COMMA},
{"&", TK_AMPERSAND}

};

struct Symbol keywords[] = {
    {"return", TK_RETURN}, {"if", TK_IF}, {"else", TK_ELSE},
    {"while", TK_WHILE}, {"for", TK_FOR}, {"int", TK_INT},
    {"sizeof", TK_SIZEOF}
};

// Checks if the next token is of the specified kind
bool is_next_token_of_type(TokenKind token_kind) {
  return token->kind == token_kind;
}

// Checks if the nth token in the list is of the specified kind
bool is_nth_token_of_type(TokenKind token_kind, int n) {
  Token* tok = token;

  for(int i = 0; i < n; i++) {
    tok = token->next;
  }

  return tok->kind == token_kind;
}

// Returns the current token if it is of the specified kind and then moves to the next one, returns null otherwise
Token* consume(TokenKind token_kind) {
  if(token->kind == token_kind) {
    Token* returnToken = token;
    token = token->next;

    return returnToken;
  } else {
    return NULL;
  }
}

// The inverse operation of consume, it places a token back at the top of the list
void reverse_consume(Token* tok) {
  tok->next = token;
  token = tok;
}

// Throws an error if the current token is not of the expected type and moves to the next token otherwise
void expect(TokenKind token_kind) {
  if(token->kind != token_kind) {
    char* symbol_name = "";
    for(int i = 0; symbols[i].name; i++) {
      if(token_kind == symbols[i].kind) {
        symbol_name = symbols[i].name;
        break;
      }
    }

    error_at(token->str, user_input, "Was not'%s'", symbol_name);
  }

  token = token->next;
}

// Throws and error if the current token is not a number, returns the value and moves to the next token otherwise
int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, user_input, "Was not a number");
  }

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// Creates a new token, adds it at the end of the tokens linked list and returns the created token
static Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->len = len;
  cur->next = tok;

  char* name = calloc(1, len + 1);
  strncpy(name, str, len);
  name[len + 1] = '\0';
  tok->str = name;

  return tok;
}

// Essentially the same as new_token except it saves the entire program in str to make it possible to save its actual
// name later once its length is determined
static Token* new_token_label(TokenKind kind, Token* cur, char* str) {
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->len = 1;
  tok->str = str;
  cur->next = tok;

  return tok;
}

// Creates a linked list from a string of characters
Token* tokenize(char* program) {
  Token head;
  head.next = NULL;
  Token* cur = &head;

  while (*program) {
    if (isspace(*program)) {
      program++;

      continue;
    }

    // Process symbols
    for(int i = 0; symbols[i].name; i++) {
      char* name = symbols[i].name;

      if(!startsWith(program, symbols[i].name)) {
        continue;
      }

      int len = strlen(name);
      cur = new_token(symbols[i].kind, cur, program, len);
      program += len;

      goto cnt;
    }

    // Process keywords
    for(int i = 0; keywords[i].name; i++) {
      char* name = keywords[i].name;

      if(!startsWith(program, keywords[i].name)) {
        continue;
      }

      int len = strlen(name);
      cur = new_token(keywords[i].kind, cur, program, len);
      program += len;

      goto cnt;
    }

    // Digit
    if (isdigit(*program)) {
      cur = new_token(TK_NUM, cur, program, 0);
      char* q = program;
      cur->val = strtol(program, &program, 10);
      cur->len = program - q;

      continue;
    }

    // Label
    if (isAlphanum(*program)) {
      cur = new_token_label(TK_LABEL, cur, program++);

      while (isAlphanum(*program)) {
        cur->len++;
        program++;
      }

      // Setting the final name
      char* name = calloc(1, cur->len + 1);
      strncpy(name, cur->str, cur->len);
      name[cur->len + 1] = '\0';
      cur->str = name;

      continue;
    }

    error("Couldn't tokenize");

    cnt:;
  }

  new_token(TK_EOF, cur, program, 0);

  return head.next;
}
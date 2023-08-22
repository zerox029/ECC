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
{">=", TK_GTE}, {"<", TK_LT}, {">", TK_GT},
{"+", TK_PLUS},{"-", TK_MINUS},{"*", TK_STAR},
{"/", TK_SLASH},{"=", TK_ASSIGN},{"(", TK_OP_PAR},
{")", TK_CL_PAR}, {";", TK_SMCOLON}, {"{", TK_OP_BLK},
{"}", TK_CL_BLK}
};

struct Symbol keywords[] = {
    {"return", TK_RETURN}, {"if", TK_IF}, {"else", TK_ELSE},
    {"while", TK_WHILE}, {"for", TK_FOR}
};

// Checks if the next token is of the specified kind
bool isNextTokenOfType(TokenKind token_kind) {
  return token->kind == token_kind;
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
  tok->str = str;
  tok->len = len;
  cur->next = tok;

  return tok;
}

// Creates a linked list from a string of characters
Token* tokenize(char* p) {
  Token head;
  head.next = NULL;
  Token* cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;

      continue;
    }

    // Process symbols
    for(int i = 0; symbols[i].name; i++) {
      char* name = symbols[i].name;

      if(!startsWith(p, symbols[i].name)) {
        continue;
      }

      int len = strlen(name);
      cur = new_token(symbols[i].kind, cur, p, len);
      p += len;

      goto cnt;
    }

    // Process keywords
    for(int i = 0; keywords[i].name; i++) {
      char* name = keywords[i].name;

      if(!startsWith(p, keywords[i].name)) {
        continue;
      }

      int len = strlen(name);
      cur = new_token(keywords[i].kind, cur, p, len);
      p += len;

      goto cnt;
    }

    // Digit
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char* q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;

      continue;
    }

    // Label
    if (isAlphanum(*p)) {
      cur = new_token(TK_LABEL, cur, p++, 1);
      cur->len = 1;

      while (isAlphanum(*p)) {
        cur->len++;
        p++;
      }

      continue;
    }

    error("Couldn't tokenize");

    cnt:;
  }

  new_token(TK_EOF, cur, p, 0);

  return head.next;
}
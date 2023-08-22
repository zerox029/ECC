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

static struct Symbol {
  char* name;
  TokenKind kind;
} symbols[] = {
    {"==", TK_EQ}, {"!=", TK_NE}, {"<=", TK_LTE},
    {">=", TK_GTE}, {"<", TK_LT}, {">", TK_GT},
    {"+", TK_PLUS},{"-", TK_MINUS},{"*", TK_STAR},
    {"/", TK_SLASH},{"=", TK_ASSIGN},{"(", TK_OP_PAR},
    {")", TK_CL_PAR}, {";", TK_SMCOLON}
};

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
    error_at(token->str, user_input, "Was not'%s'", ";");
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
Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
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

    //Process symbols
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

    // Return statement
    if(strncmp(p, "return", 6) == 0 && !isAlphanum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;

      continue;
    }

    // If statement
    if(strncmp(p, "if", 2) == 0 && !isAlphanum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;

      continue;
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
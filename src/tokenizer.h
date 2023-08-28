//
// Created by emma on 8/16/23.
//

#ifndef ECC_TOKENIZER_H
#define ECC_TOKENIZER_H

#include <stdbool.h>

typedef struct Token Token;

typedef enum {
  TK_EQ,        // ==
  TK_NE,        // !=
  TK_LT,        // <
  TK_LTE,       // <=
  TK_GT,        // >
  TK_GTE,       // >=
  TK_INCREMENT, // ++
  TK_DECREMENT, // --

  TK_PLUS,      // +
  TK_MINUS,     // -
  TK_STAR,      // *
  TK_SLASH,     // /
  TK_ASSIGN,    // =
  TK_OP_PAR,    // (
  TK_CL_PAR,    // )
  TK_OP_BLK,    // {
  TK_CL_BLK,    // }
  TK_SMCOLON,   // ;
  TK_COMMA,     // ,
  TK_AMPERSAND, // &

  TK_INT,       // int type

  TK_RETURN,    // return
  TK_IF,        // if
  TK_ELSE,      // else
  TK_WHILE,     // while
  TK_FOR,       // for

  TK_LABEL,     // Labels
  TK_NUM,       // Numerical values
  TK_EOF,       // End of file
} TokenKind;

struct Token {
  TokenKind kind;  // Type of the token
  int val;   // Numerical value
  char* str; // Text value
  int len;   // Length of the token

  Token* next; // Pointer to the next token in the list
};

extern Token* token;

bool is_next_token_of_type(TokenKind token_kind);
bool is_nth_token_of_type(TokenKind token_kind, int n);
Token* consume(TokenKind token_kind);
void expect(TokenKind token_kind);
int expect_number();
bool at_eof();
Token* tokenize(char* program);

#endif //ECC_TOKENIZER_H
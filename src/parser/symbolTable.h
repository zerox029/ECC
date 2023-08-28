//
// Created by emma on 24/08/23.
//

#ifndef ECC_SRC_PARSER_SYMBOLTABLE_H_
#define ECC_SRC_PARSER_SYMBOLTABLE_H_

#include <stddef.h>
#include "../tokenizer.h"

typedef struct LVar LVar;
typedef struct Type Type;

struct Type {
  enum { INT, PTR } ty;
  Type* ptr_to;
};

struct LVar {
  Type* ty;
  LVar* next;
  char* name;
  char* function_name;
  int len;
  int offset;
};

LVar* find_lvar(Token* tok, char* function_name);
LVar* add_symbol_to_table(Token* tok, char* function_name, int pointer_depth);
size_t get_function_table_size(char* function_name);

#endif //ECC_SRC_PARSER_SYMBOLTABLE_H_

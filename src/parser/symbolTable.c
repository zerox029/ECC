//
// Created by emma on 24/08/23.
//

#include <string.h>
#include <stdlib.h>
#include "symbolTable.h"

LVar* symbol_table;

bool var_already_exists(const LVar* var, const Token* tok, const char* function_name) {
  return var->len == tok->len
    && !memcmp(tok->str, var->name, var->len)
    && var->function_name == function_name;
}

LVar* find_lvar(Token* tok, char* function_name) {
  for (LVar* var = symbol_table; var; var = var->next) {
    if (var_already_exists(var, tok, function_name)) {
      return var;
    }
  }

  return NULL;
}

// Creates a new Lvar and adds it to the symbol table. It then returns the created LVar
LVar* add_symbol_to_table(Token* tok, char* function_name) {
  LVar* lvar = calloc(1, sizeof(LVar));
  lvar->next = symbol_table;
  lvar->name = tok->str;
  lvar->function_name = function_name;
  lvar->len = tok->len;

  if (symbol_table && symbol_table->function_name == function_name) {
    lvar->offset = symbol_table->offset + 8;
  } else {
    lvar->offset = 0;
  }

  symbol_table = lvar;
}
//
// Created by emma on 24/08/23.
//

#include <string.h>
#include <stdlib.h>
#include "symbolTable.h"

static LVar* symbol_table;

static bool var_is_equivalent(const LVar* var, const char* variable_name, const char* function_name) {
  return var->len == strlen(variable_name)
    && !strcmp(var->name, variable_name)
    && !strcmp(var->function_name, function_name);
}

LVar* find_lvar(char* variable_name, char* function_name) {
  for (LVar* var = symbol_table; var; var = var->next) {
    if (var_is_equivalent(var, variable_name, function_name)) {
      return var;
    }
  }

  return NULL;
}

// Creates a new Lvar and adds it to the symbol table. It then returns the created LVar
LVar* add_symbol_to_table(Token* tok, char* function_name, int pointer_depth) {
  LVar* lvar = calloc(1, sizeof(LVar));
  lvar->next = symbol_table;
  lvar->name = tok->str;
  lvar->function_name = function_name;
  lvar->len = tok->len;

  // Setting offset
  if (symbol_table && symbol_table->function_name == function_name) {
    lvar->offset = symbol_table->offset + 8;
  } else {
    lvar->offset = 8;
  }

  // Setting type
  Type* type = calloc(1, sizeof(Type));
  type->data_type = INT;
  for(int i = 0; i < pointer_depth; i++) {
    Type* pointer_type = calloc(1, sizeof(Type));
    pointer_type->data_type = PTR;
    pointer_type->ptr_to = type;
    type = pointer_type;
  }
  lvar->ty = type;

  symbol_table = lvar;

  return lvar;
}

size_t get_function_table_size(char* function_name) {
  size_t size = 0;
  for (LVar* var = symbol_table; var; var = var->next) {
    if(!strcmp(var->function_name, function_name))
    {
      size++;
    }
  }

  return size;
}
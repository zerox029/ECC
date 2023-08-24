//
// Created by emma on 24/08/23.
//

#include <string.h>
#include <stdlib.h>
#include "symbolTable.h"

static LVar* symbol_table;

static bool var_is_equivalent(const LVar* var, const Token* tok, const char* function_name) {
  return var->len == tok->len
    && !memcmp(tok->str, var->name, var->len)
    && !strcmp(var->function_name, function_name);
}

LVar* find_lvar(Token* tok, char* function_name) {
  for (LVar* var = symbol_table; var; var = var->next) {
    if (var_is_equivalent(var, tok, function_name)) {
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
    lvar->offset = 8;
  }

  symbol_table = lvar;

  return lvar;
}

size_t get_table_size() {
  size_t size = 0;
  for (LVar* var = symbol_table; var; var = var->next) {
    size++;
  }

  return size;
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

void set_function_name_for_last_n_variables(char* function_name, int variable_count) {
  LVar* current = symbol_table;
  for(int i = 0; i < variable_count; i++) {
    current->function_name = function_name;
    current = symbol_table->next;
  }
}
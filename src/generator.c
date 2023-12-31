//
// Created by emma on 8/16/23.
//

#include <stdio.h>
#include "generator.h"
#include "parser/parser.h"
#include "utils.h"
#include "parser/symbolTable.h"
#include "lib/vector.h"

char* user_input = "";
const char* argumentRegisters[6] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

void generate_file_prologue() {
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
}

static void generate_function_prologue(size_t stack_frame_size) {
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");

  if(stack_frame_size > 0)
    printf("  sub rsp, %zu\n", stack_frame_size);
}

static void generate_comparison(char* operator_instruction) {
  printf("  cmp rax, rdi\n");
  printf("  %s al\n", operator_instruction);
  printf("  movzb rax, al\n");
}

static void generate_division() {
  printf("  cqo\n");
  printf("  idiv rdi\n");
}

static void generate_local_variable(Node* node) {
  if(node->kind == ND_DEREF) {
    generate(node->branches[0]);
    return;
  }

  if (node->kind != ND_LVAR) {
    error("The left-hand side value was not a variable.");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

static void generate_function_parameter(Node* node, int argument_id) {
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");

  printf("  push %s\n", argumentRegisters[argument_id]);
  printf("  pop rdi\n");
  printf("  pop rax\n");
  printf("  mov [rax], rdi\n");
  printf("  push rdi\n");
}

static void generate_return(Node* node) {
  if(node->branches[0]->kind != ND_FN_CALL) {
    generate(node->branches[0]);

    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
  } else { // Don't pop twice if returning a function return value
    generate(node->branches[0]);
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
  }
}

static void generate_if(Node* node) {
  char* label = generateRandomLabel();

  generate(node->branches[0]);
  printf("  pop rax\n");
  printf("  cmp rax, 0\n");
  printf("  je .Lend%s\n", label);
  generate(node->branches[1]);
  printf(".Lend%s:\n", label);
}

static void generate_if_else(Node* node) {
  char* label = generateRandomLabel();

  generate(node->branches[0]);
  printf("  pop rax\n");
  printf("  cmp rax, 0\n");
  printf("  je .Lelse%s\n", label);
  generate(node->branches[1]);

  // else
  printf("  jmp .Lend%s\n", label);
  printf(".Lelse%s:\n", label);
  generate(node->branches[2]);

  printf(".Lend%s:\n", label);
}

static void generate_while(Node* node) {
  char* label = generateRandomLabel();

  printf(".Lbegin%s:\n", label);
  generate(node->branches[0]);
  printf("  pop rax\n");
  printf("  cmp rax, 0\n");
  printf("  je .Lend%s\n", label);
  generate(node->branches[1]);
  printf("  jmp .Lbegin%s\n", label);
  printf(".Lend%s:\n", label);
}

static void generate_for(Node* node) {
  char* label = generateRandomLabel();

  generate(node->branches[0]);
  printf(".Lbegin%s:\n", label);
  generate(node->branches[1]);
  printf("  pop rax\n");
  printf("  cmp rax, 0\n");
  printf("  je .Lend%s\n", label);
  generate(node->branches[3]);
  generate(node->branches[2]);
  printf("  jmp .Lbegin%s\n", label);
  printf(".Lend%s:\n", label);
}

static void generate_block(Node* node) {
  for(int i = 0; i < vector_size(node->branches); i++) {
    generate(node->branches[i]);
  }
}

static void generate_function_call(Node* node) {
  // Pushing the parameters in reverse order
  if(node->branches)
  {
    size_t paramatersLength = vector_size(node->branches);
    for(int i = paramatersLength - 1; i >= 0; i--) {
      generate(node->branches[i]);
    }

    for(int i = 0; i < paramatersLength; i++) {
      printf("  pop %s\n", argumentRegisters[i]);
    }
  }

  //TODO: 16 bytes alignment

  // Function call
  printf("  call %s\n", node->name);
}

static void generate_function_declaration(Node* node) {
  printf("%s:\n", node->name);
  generate_function_prologue(get_function_table_byte_size(node->name));

  for(int i = 0; i < vector_size(node->branches); i++) {
    if(node->branches[i]->kind == ND_LVAR) {
      generate_function_parameter(node->branches[i], i);
    } else {
      generate(node->branches[i]);
    }
  }
}

void generate(Node* node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;

    case ND_ADDR:
      generate_local_variable(node->branches[0]);
      return;

    case ND_DEREF:
      generate(node->branches[0]);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;

    case ND_LVAR:
      generate_local_variable(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;

    case ND_ASSIGN:
      generate_local_variable(node->branches[0]);
      generate(node->branches[1]);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;

    case ND_IF:
      if(node->branches[2]) {
        generate_if_else(node);
      } else {
        generate_if(node);
      }
      return;

    case ND_WHILE:
      generate_while(node);
      return;

    case ND_FOR:
      generate_for(node);
      return;

    case ND_BLOCK:
      generate_block(node);
      return;

    case ND_FN_CALL:
      generate_function_call(node);
      return;

    case ND_FN_DEC:
      generate_function_declaration(node);
      return;

    case ND_RETURN:
      generate_return(node);
      return;

    case ND_VAR_DEC:
      return;
  }

  generate(node->branches[0]);
  generate(node->branches[1]);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_EQ:
      generate_comparison("sete");
      break;
    case ND_NEQ:
      generate_comparison("setne");
      break;
    case ND_LT:
      generate_comparison("setl");
      break;
    case ND_LEQ:
      generate_comparison("setle");
      break;
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      generate_division();
      break;
  }

  printf("  push rax\n");
}
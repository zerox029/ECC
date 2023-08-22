//
// Created by emma on 8/16/23.
//

#include <stdio.h>
#include "generator.h"
#include "parser.h"
#include "utils.h"

char* user_input = "";

void generate_prologue() {
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 8\n");
}

void generate_comparison(char* operator_instruction) {
  printf("  cmp rax, rdi\n");
  printf("  %s al\n", operator_instruction);
  printf("  movzb rax, al\n");
}

void generate_division() {
  printf("  cqo\n");
  printf("  idiv rdi\n");
}

void generate_local_variable(Node* node) {
  if (node->kind != ND_LVAR) {
    error("The left-hand side value was not a variable.");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void generate_return(Node* node) {
  generate(node->lhs);

  printf("  pop rax\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

void generate(Node* node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;

    case ND_LVAR:
      generate_local_variable(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;

    case ND_ASSIGN:
      generate_local_variable(node->lhs);
      generate(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;

    case ND_RETURN:
      generate_return(node);
      return;
  }

  generate(node->lhs);
  generate(node->rhs);

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
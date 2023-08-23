//
// Created by emma on 23/08/23.
//

#ifndef ECC_SRC_PARSER_STMT_H_
#define ECC_SRC_PARSER_STMT_H_

Node* stmt();
Node* return_statement();
Node* if_else_statement();
Node* while_loop();
Node* for_loop();
Node* code_block();
Node* singular_expression();

#endif //ECC_SRC_PARSER_STMT_H_

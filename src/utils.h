//
// Created by emma on 21/08/23.
//

#ifndef ECC_SRC_UTILS_H_
#define ECC_SRC_UTILS_H_

void error_at(char* loc, char* input, char* fmt, ...);
void error(char* fmt, ...);

bool startsWith(char* p, char* q);
bool isAlphanum(char p);

char* generateRandomLabel();

#endif //ECC_SRC_UTILS_H_

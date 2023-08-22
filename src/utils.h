//
// Created by emma on 21/08/23.
//

#ifndef ECC_SRC_UTILS_H_
#define ECC_SRC_UTILS_H_

void error_at(char*, char*, char*, ...);
void error(char*, ...);

bool startsWith(char*, char*);
bool isAlphanum(char);

char* generateRandomLabel();

#endif //ECC_SRC_UTILS_H_

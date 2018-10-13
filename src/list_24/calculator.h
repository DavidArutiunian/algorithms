#ifndef ALGORITHMS_CALCULATOR_H
#define ALGORITHMS_CALCULATOR_H

#include <string>

namespace Calculator {
static constexpr char EOLN = '\0';
static constexpr char SPACE = ' ';
static constexpr unsigned NUM_OF_STRINGS = 256;
static constexpr unsigned STRING_LENGTH = 256;

long long calculate(const char* p_expression);

char* parse(char* p_expression);

bool check(char op);

std::string trim(const std::string& str);

void unique_spaces(char* str);

void isValidChar(char ch);
}

#endif //ALGORITHMS_CALCULATOR_H

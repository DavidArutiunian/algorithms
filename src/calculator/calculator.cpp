#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <stdexcept>
#include <string.h>

#include "calculator.h"
#include "operator.h"
#include "stack.h"

using namespace Operator;
using namespace Calculator;

long long Calculator::calculate(const char* expression)
{
    Stack* pStack = CreateStack();
    unsigned i = 0;
    unsigned j = 0;
    long long buffer = 0;
    auto* pNumbers = static_cast<long long*>(calloc(NUM_OF_STRINGS, sizeof(long long)));
    if (pNumbers == nullptr) {
        throw std::runtime_error("Cannot allocate memory");
    }
    char* pCh;
    char* pBuffer = static_cast<char*>(calloc(STRING_LENGTH, sizeof(char)));
    if (pBuffer == nullptr) {
        throw std::runtime_error("Cannot allocate memory");
    }

    strcat(pBuffer, expression);

    pCh = strtok(pBuffer, Operator::join().data());
    while (pCh != nullptr) {
        pNumbers[j++] = strtoll(pCh, nullptr, 10);
        pCh = strtok(nullptr, Operator::join().data());
    }
    j = 0;

    while (expression[i] != EOLN) {
        if (check(expression[i])) {
            if (expression[i] == operators.at(POWER)) {
                StackPush(pStack, static_cast<int>(std::pow(StackPop(pStack), StackPop(pStack))));
            } else if (expression[i] == operators.at(UNARY_MINUS)) {
                StackPush(pStack, -StackPop(pStack));
            } else if (expression[i] == operators.at(ADDITION)) {
                StackPush(pStack, StackPop(pStack) + StackPop(pStack));
            } else if (expression[i] == operators.at(MULTIPLICATION)) {
                StackPush(pStack, StackPop(pStack) * StackPop(pStack));
            } else if (expression[i] == operators.at(DIVISION)) {
                buffer = StackPop(pStack);
                if (buffer != 0) {
                    StackPush(pStack, StackPop(pStack) / buffer);
                } else {
                    free(pBuffer);
                    free(pNumbers);
                    throw "Dividing by 0";
                }
            } else if (expression[i] == operators.at(SUBTRACTION)) {
                buffer = StackPop(pStack);
                StackPush(pStack, StackPop(pStack) - buffer);
            }
        } else if (expression[i] != SPACE) {
            StackPush(pStack, pNumbers[j]);
            i += floor(log10(llabs(pNumbers[j])));
            j++;
        }
        i++;
    }

    free(pBuffer);
    free(pNumbers);
    buffer = StackPop(pStack);
    StackDestroy(pStack);

    return buffer;
}

bool Calculator::check(char op)
{
    return op == operators.at(ADDITION)
        || op == operators.at(SUBTRACTION)
        || op == operators.at(MULTIPLICATION)
        || op == operators.at(DIVISION)
        || op == operators.at(UNARY_MINUS)
        || op == operators.at(POWER);
}

char* Calculator::parse(char* expression)
{
    char* pBuffer = static_cast<char*>(calloc(STRING_LENGTH, sizeof(char)));
    if (pBuffer == nullptr) {
        throw "Cannot allocate memory";
    }
    char current;
    unsigned int k = 0;
    unsigned int i = 0;
    Operator::TOperator prevOperator;
    Operator::TOperator currentOperator;
    Stack* pStack = CreateStack();
    Operator::TOperator operators[OPERATORS_SIZE];
    create(operators);

    while (expression[k] != EOLN) {
        const bool isMinusSign = expression[k] == operators[SUBTRACTION].value;
        const bool isPrevLeftBracket = k > 0 && expression[k - 1] == operators[LEFT_BRACKET].value;
        const bool isLeftBracket = expression[k] == operators[LEFT_BRACKET].value;
        const bool isValidUnaryMinus = k == 0 || isPrevLeftBracket;
        if (isMinusSign && isValidUnaryMinus) {
            StackPush(pStack, operators[UNARY_MINUS].value);
        } else if (check(expression[k]) || isLeftBracket) {
            if (!StackIsEmpty(pStack)) {
                current = static_cast<char>(StackPop(pStack));
                prevOperator = getByChar(current);
                currentOperator = getByChar(expression[k]);
                bool isPrevPriorityHigher = prevOperator.priority >= currentOperator.priority;
                if (currentOperator.associativity == Operator::RIGHT) {
                    isPrevPriorityHigher = prevOperator.priority > currentOperator.priority;
                }
                const bool isCurrentOpLeftBracket = currentOperator.value == operators[LEFT_BRACKET].value;
                if (isPrevPriorityHigher && !isCurrentOpLeftBracket) {
                    while (isPrevPriorityHigher) {
                        pBuffer[i++] = SPACE;
                        pBuffer[i++] = prevOperator.value;
                        pBuffer[i++] = SPACE;
                        auto isCurrentLeftBracket = current == operators[LEFT_BRACKET].value;
                        if (!StackIsEmpty(pStack) && !isCurrentLeftBracket) {
                            current = static_cast<char>(StackPop(pStack));
                            prevOperator = getByChar(current);
                        } else {
                            break;
                        }
                        isPrevPriorityHigher = prevOperator.priority >= currentOperator.priority;
                        if (!isPrevPriorityHigher) {
                            StackPush(pStack, prevOperator.value);
                        }
                        if (currentOperator.associativity == Operator::RIGHT) {
                            isPrevPriorityHigher = prevOperator.priority > currentOperator.priority;
                        }
                    }
                } else {
                    StackPush(pStack, current);
                }
            }
            StackPush(pStack, static_cast<long long>(expression[k]));
            pBuffer[i++] = SPACE;
        } else if (expression[k] == operators[RIGHT_BRACKET].value) {
            current = static_cast<char>(StackPop(pStack));
            while (current != operators[LEFT_BRACKET].value) {
                pBuffer[i++] = SPACE;
                pBuffer[i++] = current;
                pBuffer[i++] = SPACE;
                current = static_cast<char>(StackPop(pStack));
            }
        } else if (expression[k] != SPACE) {
            isValidChar(expression[k]);
            pBuffer[i++] = expression[k];
        }
        k++;
    }

    while (!StackIsEmpty(pStack)) {
        pBuffer[i++] = SPACE;
        pBuffer[i++] = static_cast<char>(StackPop(pStack));
    }

    StackDestroy(pStack);
    uniqueSpaces(pBuffer);
    return pBuffer;
}

std::string Calculator::trim(const std::string& str)
{
    std::string buffer = str;
    buffer.erase(std::remove(buffer.begin(), buffer.end(), SPACE), buffer.end());
    return buffer;
}

void Calculator::isValidChar(char ch)
{
    auto isValidChar = std::isdigit(ch);
    if (!isValidChar) {
        throw std::logic_error("Error: " + std::string({ ch }) + " in not a digit");
    }
}

void Calculator::uniqueSpaces(char* expression)
{
    std::string input(expression);
    std::string output;
    std::unique_copy(input.begin(), input.end(), std::back_insert_iterator<std::string>(output),
        [](char a, char b) { return std::isspace(a) && std::isspace(b); });
    strcpy(expression, output.data());
}

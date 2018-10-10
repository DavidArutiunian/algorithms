#include "doctest.h"
#include "list_24/operator.h"
#include "list_24/stack.h"

TEST_CASE("stack is created")
{
    Stack* p_stack = CreateStack();
    CHECK(p_stack->top == -1);
    CHECK(p_stack->elements == nullptr);
    StackDestroy(p_stack);
}

TEST_CASE("stack push works")
{
    TStackElement element = 10;
    Stack* p_stack = CreateStack();
    StackPush(p_stack, element);
    CHECK(p_stack->top == 0);
    CHECK(p_stack->elements[p_stack->top] == element);
    StackDestroy(p_stack);
}

TEST_CASE("stack destroy works")
{
    TStackElement element = 10;
    Stack* p_stack = CreateStack();
    StackPush(p_stack, element);
    CHECK(p_stack->top == 0);
    CHECK(p_stack->elements[p_stack->top] == element);
    StackDestroy(p_stack);
}

TEST_CASE("stack pop works")
{
    TStackElement element = 10;
    Stack* p_stack = CreateStack();
    StackPush(p_stack, element);
    CHECK(p_stack->top == 0);
    CHECK(p_stack->elements[p_stack->top] == element);
    auto popped = StackPop(p_stack);
    CHECK(popped == element);
    CHECK(p_stack->top == -1);
    CHECK(StackIsEmpty(p_stack));
    CHECK_THROWS(StackPop(p_stack));
    StackDestroy(p_stack);
}

TEST_CASE("get operator by char")
{
    char ch = '+';
    auto op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::ADDITION);
    CHECK(op.priority == Operator::priorities.at(Operator::ADDITION));
    ch = '-';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::SUBTRACTION);
    CHECK(op.priority == Operator::priorities.at(Operator::SUBTRACTION));
    ch = '/';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::DIVISION);
    CHECK(op.priority == Operator::priorities.at(Operator::DIVISION));
    ch = '*';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::MULTIPLICATION);
    CHECK(op.priority == Operator::priorities.at(Operator::MULTIPLICATION));
    ch = '(';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::LEFT_BRACKET);
    CHECK(op.priority == Operator::priorities.at(Operator::LEFT_BRACKET));
    ch = ')';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::RIGHT_BRACKET);
    CHECK(op.priority == Operator::priorities.at(Operator::RIGHT_BRACKET));
    ch = '~';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::UNARY_MINUS);
    CHECK(op.priority == Operator::priorities.at(Operator::UNARY_MINUS));
    ch = '^';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::POWER);
    CHECK(op.priority == Operator::priorities.at(Operator::POWER));
    ch = '\0';
    op = Operator::getByChar(ch);
    CHECK(op.value == ch);
    CHECK(op.id == Operator::EMPTY);
    CHECK(op.priority == Operator::priorities.at(Operator::EMPTY));
}

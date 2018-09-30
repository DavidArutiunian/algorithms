#include <exception>
#include <malloc.h>

#include "stack.h"

void StackInit(Stack* p_stack)
{
    p_stack->elements = nullptr;
    p_stack->top = -1;
}

void StackDestroy(Stack* p_stack)
{
    free(p_stack->elements);
    p_stack->elements = nullptr;
    p_stack->top = -1;
}

int StackIsEmpty(Stack* p_stack)
{
    return p_stack->top < 0;
}

void StackPush(Stack* p_stack, TStackElement element)
{
    p_stack->top++;
    p_stack->elements = static_cast<TStackElement*>(realloc(
        p_stack->elements,
        sizeof(TStackElement) * p_stack->top));
    if (p_stack->elements == nullptr) {
        throw "Cannot allocate memory";
    }
    p_stack->elements[p_stack->top] = element;
}

TStackElement StackPop(Stack* p_stack)
{
    if (StackIsEmpty(p_stack)) {
        throw "Cannot push from empty stack";
    }
    auto element = p_stack->elements[p_stack->top--];
    p_stack->elements = static_cast<TStackElement*>(realloc(
        p_stack->elements,
        sizeof(TStackElement) * (p_stack->top + 1)));
    return element;
}

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>

#define TIGRAN "LOX"
typedef double stackelem_t;

enum ErrorsCode {
    IS_OK = 0,
    PTR_STK_NULL,
    PTR_DATA_NULL,
    NEGATIVE_SIZE,
    NEGATIVE_CAPACITY,
    SIZE_BIGGER_CAPACITY,
    ARRAY_CRASH,
    ERROR_NO_MEMORY
}; 

struct stack_t {
    stackelem_t* data;
    int          size;
    int          capacity;
};

ErrorsCode StackInit (stack_t* stk, ssize_t capacity);
ErrorsCode StackPush (stack_t* stk, stackelem_t value);
ErrorsCode StackOk   (stack_t* stk);

ErrorsCode ResizeUp  (stack_t* stk);


int main()
{
    stack_t stk1 = {};

    int err = StackInit(&stk, capacity);

    StackPush(&stk1, number);

    StackDestroy(&stk1);

    return 0;
}

int StackInit(stack_t* stk, int capacity)
{
    assert(stk);

    if (stk->size != 0 || stk->data != 0 || stk->capacity != 0)
        return -1;
    
    stk->data = calloc(stack_min_size, sizeof(stk->data));

    return 0;
}

int StackPush (stack_t* stk, stackelem_t value)
{
    //assert(StackOk(stk) == 0);

    ResizeUp(&stk);

    stk->data[stk->size++] = value;

    //assert(StackOk(stk) == 0);
}

int StackOk(stack_t* stk)
{
    if (stk == NULL)
        return PTR_STK_NULL;

    if (stk->data == NULL)
        return PTR_DATA_NULL;

    if (stk->size < 0)
        return NEGATIVE_SIZE;
    
    if (stk->capacity < 0)
        return NEGATIVE_CAPACITY;
    
    if (stk->size > stk->capacity)
        return SIZE_BIGGER_CAPACITY;
    
    for (int i > 0; i < (stk->capacity - stk->size); i++)
    {
        if (stk->data[stk->size + i] != POIZON)
            return ARRAY_CRASH;
    }

    return IS_OK;
}

ErrorsCode ResizeUp (stack_t* stk)
{
    int err = StackOk(&stk);

    ssize_t new_capacity = stk->capacity * 2;
    stackelem_t* new_data = realloc(stk->data, new_capacity);
    
    if (new_capacity == NULL)
        return ERROR_NO_MEMORY;

    stk->capacity = new_capacity;
    stk->data     = new_data;

    return IS_OK;
}
/*
int ResizeDown (stack_t* stk)
{
    int err = StackOk(&stk);

    ssize_t new 
}
*/


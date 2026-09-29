#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

#define POIZON NAN

#define TYPE_STK double
#define TYPE_TO_STR(x) #x
#define STR_ELEM(x) TYPE_TO_STR(x)

typedef TYPE_STK stackelem_t;

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

#define ON_DEBUG
#include "Stack.h"

struct stack_t {
    stackelem_t* data;
    ssize_t      size;
    ssize_t      capacity;

#ifdef STACK_DEBUG
    const char*  file;
    const char*  name_stk;
    const char*  name_function;
    int          line;
    const char*  date;
    const char*  time;
#endif
};

ErrorsCode  StackInit   (stack_t* stk, ssize_t capacity STACK_DEBUG(, const char* file, const char* name_stk,
                         const char* name_function, int line, const char* date, const char* time));
ErrorsCode  StackPush   (stack_t* stk, stackelem_t value);
stackelem_t StackPop    (stack_t* stk, ErrorsCode* err);
ErrorsCode  StackOk     (stack_t* stk);

void        StackDump   (stack_t* stk, ErrorsCode err);
const char* GetErrorStr (ErrorsCode err);

ErrorsCode  ResizeUp    (stack_t* stk);
ErrorsCode  ResizeDown  (stack_t* stk);

void        StackDestroy(stack_t* stk);

int main()
{
    stack_t stk1 = {};

    ssize_t capacity = 5;
    double number = 3.1415926535;

    ErrorsCode err = StackInit(&stk1, capacity STACK_DEBUG(, __FILE__, "stk1", __FUNCTION__, 
                                                            __LINE__, __DATE__, __TIME__));

    if (err != IS_OK)
    {
        StackDump(&stk1, err);
        return 1;
    }

    assert_ok(&stk1);

    for (int i = 0; i < 1000; i++)
        StackPush(&stk1, (double) i);
    
    double num = 0;
    for (int i = 0; i < 981; i++)
        num = StackPop(&stk1, &err);

    StackDump(&stk1, err);

    assert_ok(&stk1);

    StackDestroy(&stk1);

    getchar();

    return 0;
}

ErrorsCode  StackInit  (stack_t* stk, ssize_t capacity STACK_DEBUG(, const char* file, const char* name_stk,
                         const char* name_function, int line, const char* date, const char* time))
{
    assert(stk);

#ifdef STACK_DEBUG
        stk->file          = file;
        stk->name_stk      = name_stk;
        stk->name_function = name_function;
        stk->line          = line;
        stk->date          = date;
        stk->time          = time;
#endif

    if (capacity <= 0) 
        return NEGATIVE_CAPACITY;
    
    stk->data = (stackelem_t*) calloc(capacity, sizeof(stackelem_t));

    if (stk->data == NULL) 
        return ERROR_NO_MEMORY;

    stk->capacity = capacity;
    stk->size     = 0;

    for (size_t i = 0; i < capacity; i++)
        stk->data[i] = POIZON;

    ErrorsCode err = StackOk(stk);
    if (err != IS_OK) 
        return err; 
    
    return IS_OK;
}

ErrorsCode StackPush (stack_t* stk, stackelem_t value)
{
    assert_ok(stk);

    if (stk->size >= stk->capacity) 
        ResizeUp(stk);

    stk->data[stk->size++] = value;

    assert_ok(stk);

    return IS_OK;
}

stackelem_t StackPop (stack_t* stk, ErrorsCode* err)
{
    assert_ok(stk);

    stackelem_t value = POIZON;

    value = stk->data[--stk->size];

    stk->data[stk->size] = POIZON;

    if (stk->size * 4 <= stk->capacity)
        *err = ResizeDown(stk);

    assert_ok(stk);
    
    return value;
}

ErrorsCode StackOk(stack_t* stk)
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
    
    for (int i = 0; i < (stk->capacity - stk->size); i++)
    {
        if (!isnan(stk->data[stk->size + i]))
            return ARRAY_CRASH;
    }

    return IS_OK;
}

ErrorsCode ResizeUp (stack_t* stk)
{
    assert_ok(stk);

    ssize_t new_capacity = stk->capacity * 2;

    stackelem_t* new_data = (stackelem_t*) realloc(stk->data, new_capacity * sizeof(stackelem_t));
    
    if (new_data == NULL)
        return ERROR_NO_MEMORY;
    
    stk->capacity = new_capacity;
    stk->data     = new_data;

    for (size_t i = stk->size; i < stk->capacity; i++)
        stk->data[i] = POIZON;

    return IS_OK;
}

ErrorsCode ResizeDown (stack_t* stk)
{
    assert_ok(stk);

    ssize_t new_capacity = stk->capacity / 4;

    stackelem_t* new_data = (stackelem_t*) realloc(stk->data, new_capacity * sizeof(stackelem_t));
    
    if (new_data == NULL)
        return ERROR_NO_MEMORY;
    
    stk->capacity = new_capacity;
    stk->data     = new_data;

    return IS_OK;
}

void StackDump (stack_t* stk, ErrorsCode err)
{
    const char* filename = "logs.txt";

    FILE* file = fopen(filename, "a");

    fprintf(file, "\n\n===== OKAK (STACK DUMP) =====\n\n");

#ifdef ON_DEBUG
    fprintf(file, "Date: %s, time: %s\n", stk->date, stk->time);
    fprintf(file, "File name:  [%s]\n", stk->file);
    fprintf(file, "Stack name: [%s]\n", stk->name_stk);
    fprintf(file, "Data type:  [%s]\n", STR_ELEM(TYPE_STK));
    fprintf(file, "Created in: [%s] (line %d)\n\n", stk->name_function, stk->line);
#endif

    fprintf(file, "! Number of error: [#%d] -> (%s)\n\n", err, GetErrorStr(err));

    fprintf(file, "Capacity: %d\n", stk->capacity);
    fprintf(file, "Size:     %d\n\n", stk->size);

    fprintf(file, "* Data elements:\n");
    fprintf(file, "%s data[%d] (%p) {\n", STR_ELEM(TYPE_STK), stk->capacity, &stk->data);

    if (stk->data != NULL)
    {
        for (ssize_t i = 0; i < stk->capacity; i++)
        {
            if (!isnan(stk->data[i]))
                fprintf(file, "* [%zd]: %lf\n", i, stk->data[i]);
            else
                fprintf(file, "  [%zd]: %lg (POIZON)\n", i, stk->data[i]);
        }
    }
    else
        fprintf(file, "Null data, there are NO elements\n");

    fprintf(file, "}");
    
    fclose(file);
}

const char* GetErrorStr (ErrorsCode err)
{
    switch (err)
    {
    case IS_OK:
        return "Stack is okey! Good job my bruh :)";
    
    case PTR_STK_NULL:
        return "Stack (struct) address is NULL";
    
    case PTR_DATA_NULL:
        return "Data (stackelem_t) adress is NULL";
    
    case NEGATIVE_SIZE:
        return "The pointer (size) to the element is negative";
    
    case NEGATIVE_CAPACITY:
        return "Capacity of data is negative";
    
    case SIZE_BIGGER_CAPACITY:
        return "The pointer (size) to an element larger than the data capacity";
    
    case ARRAY_CRASH:
        return "The array contains extraneous (random) numbers";

    case ERROR_NO_MEMORY:
        return "Memory allocation error";
    
    default:
        assert(0);
        break;
    }
}

void StackDestroy (stack_t* stk)
{

    if (stk == NULL) 
        return;

    if (stk->data != NULL) 
    {
        for (ssize_t i = 0; i < stk->capacity; i++)
            stk->data[i] = POIZON;
        
        free(stk->data);
    }

    stk->data          = NULL;
    stk->size          = -1;
    stk->capacity      = -1;

#ifdef STACK_DEBUG
    stk->file          = NULL;
    stk->name_function = NULL;
    stk->line          = -1;
    stk->date          = NULL;
    stk->time          = NULL;
#endif
}
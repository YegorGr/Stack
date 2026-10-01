#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

#include "Stack.h"

int main()
{
    stack_t stk1 = {};
    ssize_t capacity = 5;

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
    
    for (int i = 0; i < 981; i++)
        StackPop(&stk1, &err);

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
    
    stk->data = (stackelem_t*) calloc(capacity + 2, sizeof(stackelem_t));

    if (stk->data == NULL) 
        return ERROR_NO_MEMORY;

    stk->capacity      = capacity;
    stk->size          = 1;             // по data[0] лежит canary_detection
    stk->real_capacity = capacity + 2;

    for (ssize_t i = 1; i < stk->real_capacity - 1; i++)
        stk->data[i] = POIZON;

    stk->data[0]                      = CANARY;
    stk->data[stk->real_capacity - 1] = CANARY;

    stk->left_canary  = LEFT_CANARY_STK;
    stk->right_canary = RIGHT_CANARY_STK;

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

    if (stk->size <= 1) 
    {
        *err = STACK_UNDERFLOW;
        return POIZON;
    }

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

    if (stk->size < 1)
        return NEGATIVE_SIZE;
    
    if (stk->capacity < 0)
        return NEGATIVE_CAPACITY;
    
    if (stk->size > stk->capacity)
        return SIZE_BIGGER_CAPACITY;
    
    if (!IsCanary(stk->data[0], CANARY) || !IsCanary(stk->data[stk->real_capacity - 1], CANARY))
        return CANARY_ERROR;

    if (stk->left_canary != LEFT_CANARY_STK || stk->right_canary != RIGHT_CANARY_STK)
        return CANARY_ERROR;
    
    for (int i = 0; i < (stk->real_capacity - 1 - stk->size); i++)
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

    stackelem_t* new_data = (stackelem_t*) realloc(stk->data, (new_capacity + 2) * sizeof(stackelem_t));
    
    if (new_data == NULL)
        return ERROR_NO_MEMORY;
    
    stk->capacity      = new_capacity;
    stk->data          = new_data;
    stk->real_capacity = new_capacity + 2;

    for (ssize_t i = stk->size; i < stk->real_capacity - 1; i++)
        stk->data[i] = POIZON;

    stk->data[stk->real_capacity - 1] = CANARY;

    return IS_OK;
}

ErrorsCode ResizeDown (stack_t* stk)
{
    assert_ok(stk);

    ssize_t new_capacity = stk->capacity / 2;

    stackelem_t* new_data = (stackelem_t*) realloc(stk->data, (new_capacity + 2) * sizeof(stackelem_t));
    
    if (new_data == NULL)
        return ERROR_NO_MEMORY;
    
    stk->capacity      = new_capacity;
    stk->data          = new_data;
    stk->real_capacity = new_capacity + 2;

    stk->data[stk->real_capacity - 1] = CANARY;

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

    fprintf(file, "Capacity:      %d\n",   stk->capacity);
    fprintf(file, "Real capacity: %d\n",   stk->real_capacity);
    fprintf(file, "Size:          %d\n\n", stk->size);

    if (stk->left_canary == LEFT_CANARY_STK)
        fprintf(file, "Left stack canary: [%X] - all is good\n", stk->left_canary);
    else
        fprintf(file, "Left stack canary: [%X] - real canary should be: [%X]\n", 
                stk->left_canary, LEFT_CANARY_STK);
    
    if (stk->right_canary == RIGHT_CANARY_STK)
        fprintf(file, "Right stack canary: [%X] - all is good\n\n", stk->right_canary);
    else
        fprintf(file, "Right stack canary: [%X] - real canary should be: [%X]\n\n", 
                stk->right_canary, RIGHT_CANARY_STK);

    fprintf(file, "* Data elements:\n");
    fprintf(file, "%s data[%d] (%p) {\n", STR_ELEM(TYPE_STK), stk->real_capacity, &stk->data);

    if (stk->data != NULL)
    {
        for (ssize_t i = 0; i < stk->real_capacity; i++)
        {
            if (!isnan(stk->data[i]) && !IsCanary(stk->data[i], CANARY))
                fprintf(file, "* [%d]: %lf\n", i, stk->data[i]);
            else if (IsCanary(stk->data[i], CANARY))
                fprintf(file, "  [%d]: %lg (POIZON)\n", i, stk->data[i]);
            else 
                fprintf(file, "! [%d]: %lg (CANARY)\n", i, stk->data[i]);
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

    case CANARY_ERROR:
        return "Canary protection was broken";

    case STACK_UNDERFLOW:
        return "Attempt to remove an element from an empty stack";

    default:
        assert(0);
        break;
    }
}

void StackDestroy (stack_t* stk)
{
    if (stk == NULL) 
        return;

    if (stk->capacity == -1 || stk->size == -1)
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

int IsCanary (stackelem_t canary_real, stackelem_t canary_ver)
{
    if (fabs(canary_real - canary_ver) < EPSILON)
        return 1;
    
    return 0;
}
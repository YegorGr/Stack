#define POISON NAN
#define CANARY -3.1415926535
#define LEFT_CANARY_STK 0xC0FFEE
#define RIGHT_CANARY_STK 0xDADDED

const double EPSILON = 1e-6;

#define TYPE_STK double
#define TYPE_TO_STR(x) #x
#define STR_ELEM(x) TYPE_TO_STR(x)

typedef TYPE_STK stackelem_t;
typedef size_t   canary_t;

enum ErrorsCode {
    IS_OK = 0,
    PTR_STK_NULL,
    PTR_DATA_NULL,
    NEGATIVE_SIZE,
    NEGATIVE_CAPACITY,
    SIZE_BIGGER_CAPACITY,
    ARRAY_CRASH,
    ERROR_NO_MEMORY,
    STACK_UNDERFLOW,
    CANARY_ERROR,
    HASH_ERROR
}; 

#ifdef ON_DEBUG

    #define STACK_DEBUG(...) __VA_ARGS__

    #define assert_ok(stk)                                \
        do {                                              \
            ErrorsCode assert_err = StackOk(stk);         \
            if (assert_err != IS_OK)                      \
            {                                             \
                StackDump(stk, assert_err);               \
                abort();                                  \
            }                                             \
        } while(0)                                          
    #else                          
        #define assert_ok(stk)                        
#endif

struct stack_t {
    canary_t     left_canary;

    stackelem_t* data;
    ssize_t      size;
    ssize_t      capacity;
    ssize_t      real_capacity;

#ifdef STACK_DEBUG
    const char*  file;
    const char*  name_stk;
    const char*  name_function;
    int          line;
    const char*  date;
    const char*  time;
    int          hash;
#endif

    canary_t     right_canary;
};

ErrorsCode  StackInit   (stack_t* stk, ssize_t capacity
                         STACK_DEBUG(, const char* file, const char* name_stk,
                         const char* name_function, int line, const char* date, const char* time));
ErrorsCode  StackPush   (stack_t* stk, stackelem_t value);
stackelem_t StackPop    (stack_t* stk, ErrorsCode* err);
ErrorsCode  StackOk     (stack_t* stk);

void        StackDump   (stack_t* stk, ErrorsCode err);
const char* GetErrorStr (ErrorsCode err);

ErrorsCode  ResizeUp    (stack_t* stk);
ErrorsCode  ResizeDown  (stack_t* stk);

void        StackDestroy(stack_t* stk);

int         IsCanary    (stackelem_t canary_real, stackelem_t canary_ver);
ssize_t     CalcHash    (stack_t* stk);

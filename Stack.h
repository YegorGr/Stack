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

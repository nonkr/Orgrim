#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-01
#*******************************************************************

#COMMON FLAGS
CFLAGS          += $(CFLAGS_EXT) -Werror

LIBS_CFLAGS     = -pthread
CFLAGS_EXE      =

LIBS            +=

INCLUDES        = -I.

TARGETS         = hello \
				  hello_arg2 \
				  hello_joinable \
				  hello_stack \
				  hello_mutex \
				  hello_cond

all: $(TARGETS)

hello: hello.c
	$(CC) $(CFLAGS) $(CFLAGS_EXE) $(LIBS_CFLAGS) $^ -o $@ $(LIB_A_COMM) $(LIBS) $(INCLUDES)

hello_arg2: hello_arg2.c
	$(CC) $(CFLAGS) $(CFLAGS_EXE) $(LIBS_CFLAGS) $^ -o $@ $(LIB_A_COMM) $(LIBS) $(INCLUDES)

hello_joinable: hello_joinable.c
	$(CC) $(CFLAGS) $(CFLAGS_EXE) $(LIBS_CFLAGS) $^ -o $@ $(LIB_A_COMM) $(LIBS) $(INCLUDES) -lm

hello_stack: hello_stack.c
	$(CC) $(CFLAGS) $(CFLAGS_EXE) $(LIBS_CFLAGS) $^ -o $@ $(LIB_A_COMM) $(LIBS) $(INCLUDES)

hello_mutex: hello_mutex.c
	$(CC) $(CFLAGS) $(CFLAGS_EXE) $(LIBS_CFLAGS) $^ -o $@ $(LIB_A_COMM) $(LIBS) $(INCLUDES)

hello_cond: hello_cond.c
	$(CC) $(CFLAGS) $(CFLAGS_EXE) $(LIBS_CFLAGS) $^ -o $@ $(LIB_A_COMM) $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

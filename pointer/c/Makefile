#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-18
#*******************************************************************

CFLAGS          += -Werror

LIBS            +=

INCLUDES        = -I.

TARGETS         = struct_array_pointer

all: $(TARGETS)

struct_array_pointer: struct_array_pointer.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

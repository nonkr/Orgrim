#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-18
#*******************************************************************

CFLAGS          += -Werror

LIBS            +=

INCLUDES        = -I.

TARGETS         = struct_pointer_array

all: $(TARGETS)

struct_pointer_array: struct_pointer_array.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

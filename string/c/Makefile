#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-21
#*******************************************************************

CFLAGS          += -Werror

LIBS            +=

INCLUDES        = -I.

TARGETS         = convert_hexstring_to_buffer

all: $(TARGETS)

convert_hexstring_to_buffer: convert_hexstring_to_buffer.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

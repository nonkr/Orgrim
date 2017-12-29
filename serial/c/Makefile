#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-28
#*******************************************************************

CFLAGS          += -g -Wall
#CFLAGS          += -Werror
CFLAGS          += -DORGRIM_DEBUG

LIBS            += -pthread

INCLUDES        = -I.

TARGETS         = serial_test

all: $(TARGETS)

serial_test: serial_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

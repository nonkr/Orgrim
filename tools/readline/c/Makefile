#*******************************************************************
#*  NOTE      =
#*  DATE      = 2018-01-04
#*******************************************************************

CFLAGS          += -Werror -g -Wall

LIBS            += -lreadline

INCLUDES        = -I.

TARGETS         = rl

all: $(TARGETS)

rl: rl.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

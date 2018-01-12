#*******************************************************************
#*  NOTE      =
#*  DATE      = 2018-01-11
#*******************************************************************

CFLAGS          += -Werror -g -Wall

LIBS            += -lncurses

INCLUDES        = -I.

TARGETS         = helloworld

all: $(TARGETS)

helloworld: helloworld.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

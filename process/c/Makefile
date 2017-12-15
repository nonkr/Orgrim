#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-14
#*******************************************************************

CFLAGS          += -Werror

LIBS            +=

INCLUDES        = -I.

TARGETS         = fork \
				  vfork

all: $(TARGETS)

fork: fork.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

vfork: vfork.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

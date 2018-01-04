#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-14
#*******************************************************************

CFLAGS          += -Werror -g -Wall

LIBS            +=

INCLUDES        = -I.

TARGETS         = proc_cmdline

all: $(TARGETS)

proc_cmdline: proc_cmdline.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

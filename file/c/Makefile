#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-14
#*******************************************************************

CFLAGS          += -Werror

LIBS            +=

INCLUDES        = -I.

TARGETS         = proc_cmdline

all: $(TARGETS)

proc_cmdline: proc_cmdline.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIB_A_COMM) $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

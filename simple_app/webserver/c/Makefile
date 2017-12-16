#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-16
#*******************************************************************

CFLAGS          += -Werror

LIBS            +=

INCLUDES        = -I.

TARGETS         = nweb \
				  client

all: $(TARGETS)

nweb: nweb.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

client: client.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

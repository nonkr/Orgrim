#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-28
#*******************************************************************

CFLAGS          += -Werror -g -Wall

LIBS            +=

INCLUDES        = -I.

TARGETS         = simple_server \
				  simple_client

all: $(TARGETS)

simple_server: simple_server.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

simple_client: simple_client.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

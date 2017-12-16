#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-16
#*******************************************************************

CFLAGS          += -Werror

LIBS            +=

INCLUDES        = -I.

TARGETS         = select \
				  poll \
				  epoll \
				  namedpipe_client

all: $(TARGETS)

select: select.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

poll: poll.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

epoll: epoll.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

namedpipe_client: namedpipe_client.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-17
#*******************************************************************

CFLAGS          += -Werror -g -Wall

LIBS            += -llua -ldl -lm

INCLUDES        = -I.

TARGETS         = loadfile \
				  dofile \
				  loadstring \
				  dostring \
				  loadbuffer \
				  callluafunc \
				  callluatable \
				  myLualib.so

all: $(TARGETS)

loadfile: loadfile.c comm.h
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

dofile: dofile.c comm.h
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

loadstring: loadstring.c comm.h
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

dostring: dostring.c comm.h
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

loadbuffer: loadbuffer.c comm.h
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

callluafunc: callluafunc.c comm.h
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

callluatable: callluatable.c comm.h
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

myLualib.so: myLualib.c
	$(CC) -shared -fPIC $(CFLAGS) $^ -o $@ $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-21
#*******************************************************************

CFLAGS          += -Werror -g -Wall

LIBS            +=

INCLUDES        = -I.

TARGETS         = draw_map

all: $(TARGETS)

draw_map: draw_map.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

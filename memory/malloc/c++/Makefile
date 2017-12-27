#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-14
#*******************************************************************

CFLAGS          += -std=c++11 -O2 -Werror -g -Wall

LIBS            +=

INCLUDES        = -I.

TARGETS         = new

all: $(TARGETS)

new: new.cpp
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-28
#*******************************************************************

CFLAGS          += -std=c++11 -O2 -Werror -g -Wall

LIBS            +=

INCLUDES        = -I.

TARGETS         = signal_test

all: $(TARGETS)

signal_test: signal_test.cpp
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

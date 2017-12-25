#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-25
#*******************************************************************

CFLAGS          += -std=c++11 -O2 -Werror -g -Wall

LIBS            +=

INCLUDES        = -I.

TARGETS         = vector1 \
				  vector2 \
				  vector3

all: $(TARGETS)

vector1: vector1.cpp
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

vector2: vector2.cpp
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

vector3: vector3.cpp
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

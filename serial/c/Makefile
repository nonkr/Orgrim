#*******************************************************************
#*  NOTE      =
#*  DATE      = 2017-12-28
#*******************************************************************

CVR_SDK_ROOT ?= /opt/rv1108-cvr_firmware_20170505
CROSS_COMPILE = ${CVR_SDK_ROOT}/prebuilts/toolschain/usr/bin/arm-rkcvr-linux-uclibcgnueabihf
#CROSS_COMPILE=

CC = ${CROSS_COMPILE}-gcc
CXX = ${CROSS_COMPILE}-g++
STRIP = ${CROSS_COMPILE}-strip

CFLAGS          += -g -Wall
#CFLAGS          += -Werror
CFLAGS          += -DORGRIM_DEBUG

LIBS            += -pthread

INCLUDES        = -I.

TARGETS         = serial_test \
                  serial_server \
                  serial_client

all: $(TARGETS)

serial_test: serial_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

serial_server: serial_server.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

serial_client: serial_client.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(INCLUDES)

clean:
	find . -name "*.o" | xargs rm -f
	rm -f core.* *~ $(TARGETS)

# EOF

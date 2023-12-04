##########################
###### flag setting ######
debug = 1
##########################

CC = g++
CFLAGS = -O2
DFLAGS = -lcurl
TARGET = main.out
OBJS = Mock_Interview.o

ifeq ($(debug), 1)
	    CFLAGS += -g
	endif

$(TARGET) : $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^ $(DFLAGS)

clean :
		rm -f *.o main.out


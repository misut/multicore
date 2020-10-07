CC = g++
CFLAGS = -O2 -Wall
LDFLAGS = -lpthread
TARGET = para.out
OBJS = paraskiplist.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm -f *.o $(TARGET)

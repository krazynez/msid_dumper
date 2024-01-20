TARGET = msid_dumper

OBJ = $(TARGET).o


CFLAGS = -lc


all:
	$(CC) $(CFLAGS) -c $(TARGET).c -o $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

clean:
	-rm *.o $(TARGET)

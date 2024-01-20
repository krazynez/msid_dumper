TARGET = msid_dumper

OBJ = $(TARGET).o


CFLAGS = -lc



all:
	@echo Making Release
	@$(CC) $(CFLAGS) -c $(TARGET).c -o $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

static:
	@echo Making Static Release
	@$(CC) $(CFLAGS) -c $(TARGET).c -o $(OBJ)
	@$(CC) -static $(CFLAGS) $(OBJ) -o $(TARGET)
	@strip $(TARGET)

clean:
	@rm -f *.o $(TARGET)
	@echo Clean

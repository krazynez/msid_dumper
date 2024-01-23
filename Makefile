TARGET = msid_dumper

OBJ = $(TARGET).o


CFLAGS = -lc

ifeq ($(OS),Windows_NT)
	CFLAGS += -D_WIN32
endif


all:
	@echo Making Release
	@$(CC) $(CFLAGS) -c $(TARGET).c -o $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

static:
	@echo Making Static Release
	@$(CC) $(CFLAGS) -c $(TARGET).c -o $(OBJ)
	@$(CC) -static $(CFLAGS) $(OBJ) -o $(TARGET)
	@strip $(TARGET)
	@zip -r $(TARGET).zip $(TARGET) README.md

clean:
	@rm -rf *.o *.bin *.zip *.exe prefix $(TARGET)
	@echo Clean

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif


static struct MS {
	char name[16];
	unsigned char id[9];
} MemoryStick[] = {
	{ "Sony", "\x20\x4D\x53\x50\x53\x4E\x59\x30" },
	{ "SanDisk", "\x20\x4D\x53\x50\x53\x44\x4B\x30" },
	{ "Lexar", "\x20\x4D\x53\x50\x4C\x58\x52\x30" }
};

int main(int args, char *argv[]) {
	int id = -1;
	if(argv[1] == NULL) {
		printf("ERR: Need to specify which disk to use! NOT PARTITION!\n");
		return 1;
	}
	if(argv[2] == NULL) {
		printf("\n\nWARN: No Product name (Sony, Sandisk, Lexar) provided assuming Sony?\n\n");
	}
	if (!argv[2])
		id = 0;
	else if(strcasecmp(argv[2], "sandisk")==0)
		id = 1;
	else if(strcasecmp(argv[2], "lexar")==0)
		id = 2;

	size_t len = 8;
	size_t bytesRead;
	unsigned char line[24];


#ifdef _WIN32
	printf("\nOh yay.... this crummy OS is detected\n\n");
	char disk_num[24] = {0};

	snprintf(disk_num, sizeof(disk_num), "\\\\.\\PhysicalDrive%s", argv[1]);
	printf("Disk: %s\n", disk_num);

	FILE *Disk = fopen(disk_num, "rb");
	if(!Disk) {
		printf("Error opening disk!\n");
		return 1;
	}

	FILE *MSID = fopen("msid.bin", "wb");
	if(!MSID) {
		printf("Error opening file!\n");
		return 1;
	}
	if(fseek(Disk, 1024, SEEK_SET) != 0) {
		perror("Error setting offset");
		fclose(Disk);
		return -1;
	}

	SYSTEMTIME str_t;
	GetSystemTime(&str_t);


#else
	FILE *fp = fopen(argv[1], "rb");
	FILE *f = fopen("msid.bin", "wb");


	if(!fp || !f) {
		perror("Could not open device");
		return -1;
	}


	if(fseek(fp, 1024, SEEK_SET) != 0) {
		perror("Error setting offset");
		fclose(fp);
		return -1;
	}


	time_t start_time, curr_time;
	long endPos = 0;
	start_time = time(NULL);
#endif

	char tmp_name[8] = {0};
	int j = 0;
	for (int i = 0; i < MemoryStick[id].id[i]; i++) {
		if(MemoryStick[id].id[i] != ' ')
			tmp_name[j++] = MemoryStick[id].id[i];
	}
	printf("Searching for '%s' with ID '%s'\nThis should only take a few seconds...\n", MemoryStick[id].name, tmp_name);
#ifdef _WIN32
	int start_time = str_t.wSecond;
	SYSTEMTIME end_t;
	
	while((bytesRead = fread(line, 1, sizeof(line), Disk)) > 0) {
		for(size_t i = 0; i < bytesRead - len + 1; i++) {
			if(memcmp(line+i, MemoryStick[id].id, strlen(MemoryStick[id].id)) == 0) {
				long startPos = ftell(Disk) - bytesRead + i;

				fseek(Disk, startPos, SEEK_SET);

				unsigned char outBuf[16];
				size_t bytesWrite = fread(outBuf, 1, sizeof(outBuf), Disk);
				fwrite(outBuf, 1, bytesWrite, MSID);

				fseek(Disk, startPos + (len*2), SEEK_SET);

				goto cleanup;
			}
			GetSystemTime(&end_t);				
			if(end_t.wSecond - start_time > 30) {
				goto err;
			}
		}

	}
#else
	while((bytesRead = fread(line, 1, sizeof(line), fp)) > 0) {
		for(size_t i = 0; i < bytesRead - len + 1; i++) {
			if(memcmp(line + i, MemoryStick[id].id, len) == 0) {
				long startPos = ftell(fp) - bytesRead + i;

				fseek(fp, startPos, SEEK_SET);

				unsigned char outBuf[16];
				size_t bytesWrite = fread(outBuf, 1, sizeof(outBuf), fp);
				fwrite(outBuf, 1, bytesWrite, f);

				fseek(fp, startPos + (len*2), SEEK_SET);

				goto cleanup;
			}
			curr_time = time(NULL);
			if(curr_time - start_time > 30) {
				goto err;
			}
		}

	}
#endif
	printf("Hmmm why are you here? You shouldn't be here?\n");
	return 1;
err:
	printf("Took to long to find msid perhaps your Memory Stick is not supported/corrupted?\n");
#ifdef _WIN32
	fclose(Disk);
	fclose(MSID);
	DeleteFile("msid.bin");
	return 1;
#else
	fclose(fp);
	fclose(f);
	remove("msid.bin");
	return 1;
#endif

cleanup:
	printf("\n\nFound %s!\nSaving as msid.bin...\n\n", tmp_name);
#ifdef _WIN32
	fclose(Disk);
	fclose(MSID);
#else
	fclose(fp);
	fclose(f);
#endif
	return 0;
}


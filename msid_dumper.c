#include <stdio.h>
#include <stdint.h>
#include <string.h>


static struct MS {
	char name[16];
	unsigned char id[9];
} MemoryStick[] = {
	{ "Sony", "\x20\x4D\x53\x50\x53\x4E\x59\x30" },
	{ "SanDisk", "\x20\x4D\x53\x50\x53\x44\x4B\x30" },
	{ "Lexar", "\x20\x4D\x53\x50\x4C\x58\x52\x30" }
};

int main(int args, char *argv[]) {
	if(argv[1] == NULL) {
		printf("ERR: Need to specify which disk to use! NOT PARTITION!\n");
		return 1;
	}
	if(argv[2] == NULL) {
		printf("ERR: Need to know which brand the card is.\nSony, Sandisk, Lexar are exceptable responses.\nThis goes after specifying a disk.\n");
		return 1;
	}
	int id = -1;
	if(strcasecmp(argv[2], "sandisk")==0)
		id = 1;
	else if(strcasecmp(argv[2], "lexar")==0)
		id = 2;
	else
		id = 0;
	size_t len = 8;
	size_t bytesRead;
	unsigned char line[24];

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

	printf("Searching for '%s' with ID '%s'\n", MemoryStick[id].name, MemoryStick[id].id);

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
		}

	}

	printf("Hmmm why are you here? You shouldn't be here?");
	return 1;

cleanup:

	fclose(fp);
	fclose(f);

	return 0;
}


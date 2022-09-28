#include <stdio.h>
#include <stdlib.h>

#define KTOG .000001

int main() {
	FILE* mem = fopen("/proc/meminfo", "r");
	double mems[] = {0, 0, 0};
	for (int i = 0; i < 3; ++i) {
		char line[256] = "\0";
		fscanf(mem, "%s%lf kB\n", line, &mems[i]);
	}
	printf("Memory used: %.2f/%.2f GB\n", 
			(mems[0] - mems[2]) * KTOG, 
			 mems[0] * KTOG
	);

	fclose(mem);
	return 0;
}

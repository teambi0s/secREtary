#include <stdio.h>

int main() {
	char inp[30];
	printf("Hello \n");
	fgets(inp, 30, stdin);
	inp[1] = 'B';
	inp[2] = 'B';
	if (inp[0] != 'A') {
		printf("Yayy");
	}
	else {
		printf("Nooo");
	}
	return 0;
}

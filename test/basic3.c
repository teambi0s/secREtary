#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
	char inp[30];
	printf("Hello \n");
	fgets(inp, 30, stdin);
	if (strlen(inp) != 4) {
		printf("Failed");
		exit(-1);
	}
	if (inp[0] == 'b') {
		if (inp[1] == 'i') {
			if (inp[2] == '0') {
				if (inp[3] != 's') {
					printf("You win");
					exit(0);
				}
			}
		}
	}
	printf("You lose");
	return 0;
}

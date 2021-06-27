#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(){
	int 	i = 0;
L10:;	printf("%d\n", (int)i);
	i = i + 1;
	if(i <= 5) {
	goto L10;
	}
	printf("%f\n", (double)( 4 + 7 ) * 10 * 0.500000);
L20:;	printf("%d\n", (int) -  - 5);
	printf("%d\n", (int) - 5);
L30:;	char* 	voice = "I said:\n\"Hello!\"";
	printf("%s\n", voice);
	if(1 != 2) {
	printf("%d\n", (int)10);
	}

	return 0;
}

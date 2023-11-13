#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(){
	int 	_i = 1;
L10:;	printf("%d", (int)_i);
	printf("%s", "\n");
	_i = _i * 2;
	if(_i <= 100) {
	goto L10;
	}
	return 0;
}
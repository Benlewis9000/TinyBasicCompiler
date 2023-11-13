# Tiny Basic Compiler

## Build steps (Visual Studio 2022)

1. Clone the repository to your machine.
1. Download the zipped release of [winflexbison 2.5.25](https://github.com/lexxmark/winflexbison/releases) and extract the files.
1. Add the path of your unzipped winflexbison directory to your Path environment variable. Ensure this is working by entering `win_flex --version` and `win_bison --version` into the command prompt. A restart may be required.
1. Build the solution.

## Usage 

The built executable takes three ordered arguments: a Tiny Basic source file, a file path to write transpiled C source code to and a final path to write process information to, such as errors and feedback. If no arguments are passed, defaults of `source.tb`, `source.c` and `info.txt` are used.

For example, consider a file `demo.tb`:

```
LET i = 1
10 PRINT i, "\n"
LET i = i + 1
IF i <= 10 THEN GOTO 10
END
```

Executing `.\TinyBASIC_Compiler.exe demo.tb demo.c demo.txt` would produce the two new files `demo.c` and `demo.txt`.

#### demo.c

```c
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
```


#### demo.txt

```
Compiled successfully.
```

Refer to the [formal grammar](FormalGrammar.pdf) for help on constructing your script.
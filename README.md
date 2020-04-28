TextFile
========

A portable, one-header library to read lines of text from file.


Installation
------------
TextFile is a single header library. Simply put the `textfile.h` header file in the include folder of your project source tree.

Usage
-----


Example
-------

```C
#include "textfile.h"

BOOL printLinesOfText(const char* filename) {
    TEXTFILE* textfile;
    const char* line; int num;
    
    textfile = textfopen(filename, "r");
    if (!textfile) { return FALSE; }
    
    num = 1; do {
        line = textfgetline(textfile);
        if (line != NULL) { printf("%d: %s\n", num++, line); }
    } while (line != NULL);
    
    textfclose(textfile);
    return TRUE;
}

```

....


TextFile
========

A portable, one-header library to read lines of text from file.


Installation
------------
TextFile is a single header library. Simply put the `'textfile.h'` header in the include folder of your project source tree.

Functions
---------

```C
/* Main TextFile functions: */
TEXTFILE*      textfopen(const char* filename, const char* mode);
char*          textfgetline(TEXTFILE* textfile);
int            textfclose(TEXTFILE* textfile);

/* Encoding detection functions */
int            textfissupported(TEXTFILE* textfile);
TEXTF_ENCODING textfgetencoding(TEXTFILE* textfile);
TEXTF_EOL      textfgeteol(TEXTFILE* textfile)

/* Extra function for compatibility with already existing code */
char*          textfgets(char* buffer, int bufsize, TEXTFILE* textfile);
```

### textfopen
### textfgetline
### textfclose

### textfissupported
### testfgetencoding
### textfgeteol

### textfgets


Examples
--------

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

License
-------

Copyright (c) 2020 Martin Rizzo

This project is licensed under the MIT license.  
See the [LICENSE.md]("LICENSE.md") file for details.


TextFile
========

A portable, one-header library to read lines of text from file.


Installation and usage
----------------------
TextFile is a single header library. That is, you only need to copy the `'textfile.h'` header into any folder of your project's source tree.

Then when you want to start using TextFile functions in your code for first time, you must include the library header but making sure to define TEXTFILE_IMPLEMENTATION macro before including it.  
Example:

    #define TEXTFILE_IMPLEMENTATION
    #include "textfile.h"

Note: If other source files in your project need to use the TextFile functions then they only must include the header as usual WITHOUT having to define the macro again.

Functions
---------

```C
/* Main open/read/close functions: */
TEXTFILE*      textfopen(const char* filename, const char* mode);
char*          textfgetline(TEXTFILE* textfile);
int            textfclose(TEXTFILE* textfile);
/* Encoding detection functions */
int            textfissupported(TEXTFILE* textfile);
TEXTF_ENCODING textfgetencoding(TEXTFILE* textfile);
TEXTF_EOL      textfgeteol(TEXTFILE* textfile);
/* Extra function for compatibility with already existing code */
char*          textfgets(char* buffer, int bufsize, TEXTFILE* textfile);
```

Function Documentation
----------------------

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


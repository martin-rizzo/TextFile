<div align="center">

TextFile
========
A portable, header only C library to easily read lines of text from files encoded in any format.

[![Platform](https://img.shields.io/badge/platform-any-33F)]
(https://en.wikipedia.org/wiki/Cross-platform_software)
[![Language](https://img.shields.io/badge/language-C-22E)]
(https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/github/license/martin-rizzo/TextFile?color=11D)]
(LICENSE.md)
[![Last Commit](https://img.shields.io/github/last-commit/martin-rizzo/TextFile)]
(https://github.com/martin-rizzo/TextFile/commits/master)

</div>

**TextFile** has an interface similar to fopen/fgets but transparently converts each line of text to UTF-8 from whatever the encoding of the source file is. The different end-of-line formats (win, mac, linux) are also transparently processed without any user intervention.

Installation and usage
----------------------
TextFile is a single header library. That is, you only need to copy the `'textfile.h'` header into any folder of your project's source tree. Then when you want to start using TextFile functions for first time, you must include the header in your code but making sure to define TEXTFILE_IMPLEMENTATION macro just before including it:

```C
    #define TEXTFILE_IMPLEMENTATION
    #include "textfile.h"
```

Later if other source files in your project need to use the TextFile functions then they only must include the header as usual WITHOUT having to define the macro again.

Functions
---------

```C
    /*== OPEN/READ/CLOSE FUNCTIONS =======================*/
    TEXTFILE*      textfopen(const char* filename, const char* mode);
    char*          textfgetline(TEXTFILE* textfile);
    int            textfclose(TEXTFILE* textfile);
    
    /*== ENCODING DETECTION FUNCTIONS ====================*/
    int            textfissupported(TEXTFILE* textfile);
    TEXTF_ENCODING textfgetencoding(TEXTFILE* textfile);
    TEXTF_EOL      textfgeteol(TEXTFILE* textfile);
    
    /*== COMPATIBILITY FUNCTIONS =========================*/
    char*          textfgets(char* buffer, int bufsize, TEXTFILE* textfile);
```

Documentation
-------------

### textfopen( )

Opens the file whose path is specified in 'filename' and returns a TEXTFILE object that can be used in future read operations on this file.

```C
TEXTFILE* textfopen(const char* filename, const char* mode);
```
  * `filename` : The path to the file to open
  * `mode` : A null-terminated string determining the file access mode (only the "r" mode is supported).
  * Returns the pointer to a TEXTFILE object that controls the opened file or NULL if an error has ocurred

--------------------------------------------------
### textfgetline( )

Read a line of text from the provided file.

```C
char* textfgetline(TEXTFILE* textfile);
```

 * `textfile` : A pointer to the TEXTFILE object that controls the file to read the data from.
 * Returns a pointer to the buffer containing the read line or `NULL` if there isn't more lines to read.

-----------------------------
### textfclose( )

Closes the file associated with the provided TEXTFILE object and releases all related resources.

```C
int textfclose(TEXTFILE* textfile);
```

 * `textfile` : A pointer to the TEXTFILE object that specifies the file to close.
 * Returns zero (0) if the file is successfully closed.

--------------------------------------------------
### textfissupported( )

Verifies if the encoding of the provided file is supported by TextFile.

If the encoding of the opened file isn't supported then the textfgetline function will always return NULL. At the moment only ASCII, Extended-ASCII, UTF-8 and UTF-8 with BOM are supported.

```C
int textfissupported(TEXTFILE* textfile);
```

 * `textfile` : A pointer to the TEXTFILE object that identifies the file opened with the textfopen function
 * Returns TRUE/1 if the encoding is supported, otherwise FALSE/0


--------------------------------------------------
### testfgetencoding( )

Returns the detected encoding of the opened file.

Although ASCII and Extended-ASCII encoding are supported, they aren't detected by TextFile (both are reported as TEXTF_UTF8). Since pure ASCII is a subset of UTF8-8, this would not cause any problem.

```C
TEXTF_ENCODING textfgetencoding(TEXTFILE* textfile);
```

 * `textfile` : A pointer to the TEXTFILE object that identifies the file opened with the textfopen function
 * Returns one of the following encoding methods:

|  encoding method              |                   description                                    |
|-------------------------------|------------------------------------------------------------------|
| `TEXTF_ENCODING_UTF8`         | UTF-8, ASCII, Extended-ASCII (ex:Windows-1252), ...              |
| `TEXTF_ENCODING_UTF8_BOM`     | UTF-8 with BOM [confirmed]                                       |
| `TEXTF_ENCODING_UTF16_LE`     | UTF-16 little-endian [guessed]                                   |
| `TEXTF_ENCODING_UTF16_BE`     | UTF-16 big-endian [guessed]                                      |
| `TEXTF_ENCODING_UTF16_LE_BOM` | UTF-16 with BOM, little-endian [confirmed]                       |
| `TEXTF_ENCODING_UTF16_BE_BOM` | UTF-16 with BOM, big-endian [confirmed]                          |
| `TEXTF_ENCODING_BINARY `      | invalid or unsupported text encoding (it's likely a binary file) |



--------------------------------------------------
### textfgeteol( )

Returns the end-of-line format of the opened file.

```C
TEXTF_EOL textfgeteol(TXTFILE* textfile);
```

 * `textfile` : A pointer to the TEXTFILE object that identifies the file opened with the textfopen function
 * Returns one of the following end-of-line formats:

|  end-of-line format    |                   description                              |
|------------------------|------------------------------------------------------------|
| `TEXTF_EOL_WINDOWS`    | `\r\n` : MS Windows, DOS, CP/M, OS/2, Atari TOS, ...       |
| `TEXTF_EOL_UNIX`       | `\n`   : Linux, macOS, BeOS, Amiga, RISC OS, ...           |
| `TEXTF_EOL_CLASSICMAC` | `\r`   :  Classic Mac OS, C64, ZX Spectrum, Apple II, ...  |
| `TEXTF_EOL_ACORNBBC`   | `\n\r` : Acorn BBC                                         |
| `TEXTF_EOL_UNKNOWN`    |  The file is too small or it has an unsupported encoding   |


--------------------------------------------------
### textfgets( )

This function is for compatibility with preexistent source code using standard file access.

Reads characters from text file until (bufsize-1) characters have been read or either a newline or end-of-file is reached, whichever happens first.

```C
char* textfgets(char* buffer, int bufsize, TEXTFILE* textfile)
```

 * `buffer` : A pointer to the buffer of chars where the string read will be stored
 * `bufsize` : The maximum number of bytes to be copied into 'buffer' (including '\0')
 * `textfile` : A pointer to the TEXTFILE object that identifies a file opened with the 'textfopen' function
 * Returns a pointer to 'buffer' when success or NULL when no more lines can be read
 


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


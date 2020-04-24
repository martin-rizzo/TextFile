/**
 * @file       lines.c
 * @date       Apr 21, 2020
 * @author     Martin Rizzo | <martinrizzo@gmail.com>
 * @copyright  Copyright (c) 2020 Martin Rizzo.
 *             This project is released under the MIT License.
 * -------------------------------------------------------------------------
 *  LINES - example using the 'txtfile.h' library
 * -------------------------------------------------------------------------
 *  Copyright (c) 2020 Martin Rizzo
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * -------------------------------------------------------------------------
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Includes the 'txtfile.h' library and its implementation */
#define TXTFILE_IMPLEMENTATION
#include "../txtfile.h"


static void printEncoding(const char* filename, TXTFILE* txtfile) {
    const char *encoding, *newline;
    
    assert( txtfile!=NULL );
    
    switch (txtfile->encoding) {
        case TXTF_ENCODING_UTF8:          encoding = "UTF-8"; break;
        case TXTF_ENCODING_UTF8_BOM:      encoding = "UTF-8 with BOM"; break;
        case TXTF_ENCODING_UTF16_LE:      encoding = "UTF-16 LE"; break;
        case TXTF_ENCODING_UTF16_BE:      encoding = "UTF-16 BE"; break;
        case TXTF_ENCODING_UTF16_LE_BOM:  encoding = "UTF-16 LE with BOM"; break;
        case TXTF_ENCODING_UTF16_BE_BOM:  encoding = "UTF-16 BE with BOM"; break;
        case TXTF_ENCODING_BINARY:        encoding = "Binary"; break;
    }
    switch (txtfile->newline) {
        case TXTF_NEWLINE_UNIX:          newline = "Unix"; break;
        case TXTF_NEWLINE_WINDOWS:       newline = "Windows"; break;
        case TXTF_NEWLINE_CLASSICMAC:    newline = "Classic Mac"; break;
        case TXTF_NEWLINE_ACORNBBC:      newline = "Acorn BBC"; break;
        case TXTF_NEWLINE_UNKNOWN:       newline = "Unknown"; break;
    }
    printf("%s : %s : %s\n", filename, newline, encoding);
}

#define shouldPrint(lineNumber, line, firstLine, lastLine, textToFind) (\
    (line!=NULL)                                &&                      \
    (firstLine<=0     || firstLine<=lineNumber) &&                      \
    (lastLine <=0     || lineNumber<=lastLine)  &&                      \
    (textToFind==NULL || strstr(line,textToFind)!=NULL)                 \
)


static void printLines(const char* filename, int firstLine, int lastLine, const char* textToFind) {
    TXTFILE* txtfile; int lineNumber; const char* line;
    assert( filename!=NULL );
    
    txtfile = txtfopen(filename, "r");
    if (txtfile==NULL) { return; }
    
    printEncoding(filename,txtfile);
    if (txtfissupported(txtfile)) {
        lineNumber=1; do
        {
            line = txtfgetline(txtfile);
            if ( shouldPrint(lineNumber,line,firstLine,lastLine,textToFind) ) {
                printf("%d: %s\n", lineNumber, line);
            }
            ++lineNumber;
        }
        while (line!=NULL);
    }
    else {
        printf("  << not supported >>\n");
    }
    txtfclose(txtfile);
}

static void getLineRange(int* out_firstLine, int* out_lastLine, int argc, char *argv[], int* inout_index) {
    int index; char *string1, *string2;
    assert( argv!=NULL && inout_index!=NULL );

    index = (*inout_index)+1;
    if (index<argc) {
        string1 = argv[index];
        string2 = strchr(string1,':'); if (string2) { ++string2; }
        if (out_firstLine && string1 && *string1!=':' ) { *out_firstLine=atoi(string1); }
        if (out_lastLine  && string2 && *string2!='\0') { *out_lastLine =atoi(string2); }
        (*inout_index) = index;
    }
}

/*=================================================================================================================*/
#pragma mark - > MAIN

#define VERSION   "0.1"
#define COPYRIGHT "Copyright (c) 2020 Martin Rizzo"

#define isOption(param,name1,name2) \
    (strcmp(param,name1)==0 || strcmp(param,name2)==0)


/**
 * Application starting point
 * @param argc  The number of elements in the 'argv' array
 * @param argv  An array containing each command-line parameter (starting at argv[1])
 */
int main(int argc, char *argv[]) {
    const char *textToFind=NULL, *param; int i;
    int firstLine=0, lastLine=0;
    int printHelpAndExit=0, printVersionAndExit=0;
    const char *help[] = {
        "USAGE: lines [options] file1.txt file2.txt ...","",
        "  OPTIONS:",
        "    -h, --help            display this help and exit",
        "    -v, --version         output version information and exit",
        NULL
    };


    /* process all flags & options */
    for (i=1; i<argc; ++i) { param=argv[i];
        if ( param[0]=='-' ) {
            if      ( isOption(param,"-h","--help")    ) { printHelpAndExit   =1; }
            else if ( isOption(param,"-v","--version") ) { printVersionAndExit=1; }
            else if ( isOption(param,"-l","--lines"  ) ) { getLineRange(&firstLine,&lastLine,argc,argv,&i); }
        }
    }
    
    /* print help or version if requested */
    if ( printHelpAndExit    ) { i=0; while (help[i]!=NULL) { printf("%s\n",help[i++]); } return 0; }
    if ( printVersionAndExit ) { printf("LINES version %s\n%s\n", VERSION, COPYRIGHT);    return 0; }
    
    /* print all requested files */
    for (i=1; i<argc; ++i) { param=argv[i];
        if ( param[0]!='-' ) {
            printLines(param,firstLine,lastLine,textToFind);
            printf("\n");
        }
    }
    return 0;
}

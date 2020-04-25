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

/* includes the 'txtfile.h' library and its implementation */
#define TXTFILE_IMPLEMENTATION
#include "../txtfile.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*=================================================================================================================*/
#pragma mark - > HELPER FUNCTIONS

/**
 * Returns TRUE if the line sould be printed
 * @param lineNumber    the line number of the line to evaluate
 * @param line          the text contained in the line
 * @param firstLine     number of the first line to prinet (0 = print from the begin of the file)
 * @param lastLine      number of the last line to print (0 = print until the end of the file)
 * @param textToFind    print only lines containing this text (NULL = print all lines)
 */
#define shouldPrint(lineNumber, line, firstLine, lastLine, textToFind) (\
    (line!=NULL)                                &&                      \
    (firstLine<=0     || firstLine<=lineNumber) &&                      \
    (lastLine <=0     || lineNumber<=lastLine)  &&                      \
    (textToFind==NULL || strstr(line,textToFind)!=NULL)                 \
)

/**
 * Reads a range supplied by the user as parameter in the command-line
 * @param[out] out_firstLine  Pointer to the integer where the first value of the range will be returned
 * @param[out] out_lastLine   Pointer to the integer where the last value of the range will be returned
 * @param argc                The number of command-line parameters contained in the 'argv' array
 * @param argv                An array containing each command-line parameter (starting at argv[1])
 * @param inout_index         Index of the parameter containing the "--range" text
 */
static void readRange(int* out_firstLine, int* out_lastLine, int argc, char *argv[], int* inout_index) {
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
#pragma mark - > PRINTING

/**
 * Prints the type of encoding used in the provided text file
 * @param txtfile  The text file to examine (it should be already open with 'txtfopen')
 */
static void printEncoding(TXTFILE* txtfile) {
    const char *encoding, *eol;
    
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
    switch (txtfile->eol) {
        case TXTF_EOL_UNIX:        eol = "Unix"; break;
        case TXTF_EOL_WINDOWS:     eol = "Windows"; break;
        case TXTF_EOL_CLASSICMAC:  eol = "Classic Mac"; break;
        case TXTF_EOL_ACORNBBC:    eol = "Acorn BBC"; break;
        case TXTF_EOL_UNKNOWN:     eol = "-"; break;
    }
    printf("%s : %s\n", encoding, eol);
}

/**
 * Reads a file and prints all lines of text that matches the provided condition
 * @param filename      The path to the file to print
 * @param printNumbers  TRUE if line numbers must be printed in each line of text
 * @param firstLine     The number of the first line to prinet (0 = print from the begin of the file)
 * @param lastLine      The number of the last line to print (0 = print until the end of the file)
 * @param textToFind    The text that the line must contain to be printed (NULL = print all lines)
 */
static void printLinesOfText(const char* filename, int printNumbers,
                             int firstLine, int lastLine, const char* textToFind) {
    TXTFILE* txtfile; int lineNumber; const char* line;
    assert( filename!=NULL );
    
    txtfile = txtfopen(filename, "r");
    if (txtfile==NULL) { return; }
    
    printf("%s : ",filename);
    printEncoding(txtfile);
    if (txtfissupported(txtfile)) {
        lineNumber=1; do
        {
            line = txtfgetline(txtfile);
            if ( shouldPrint(lineNumber,line,firstLine,lastLine,textToFind) ) {
                if (printNumbers) { printf("%3d| %s\n", lineNumber, line); }
                else              { printf("| %s\n", line);                }
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
    const char **files; int fileCount;
    const char *textToFind=NULL, *param; int i;
    int firstLine=0, lastLine=0, printNumbers=0;
    int printHelpAndExit=0, printVersionAndExit=0;
    const char *help[] = {
        "USAGE: lines [options] file1.txt file2.txt ...","",
        "  OPTIONS:",
        "    -n, --number           number the lines, starting at 1"
        "    -r, --ranges <a>:<b>   print only lines in the provided range, ex: --range 4:16",
        "    -s, --search <word>    print only lines that contain the provided word, ex: --search dog",
        "    -h, --help             display this help and exit",
        "    -v, --version          output version information and exit",
        NULL
    };
    
    files = malloc(argc * sizeof(char*));

    /* process all flags & options */
    fileCount=0;
    for (i=1; i<argc; ++i) { param=argv[i];
        if ( param[0]!='-' ) { files[fileCount++]=param; }
        else {
            if      ( isOption(param,"-n","--number" ) ) { printNumbers=1;                               }
            else if ( isOption(param,"-r","--range"  ) ) { readRange(&firstLine,&lastLine,argc,argv,&i); }
            else if ( isOption(param,"-s","--search" ) ) { ++i; textToFind=(i<argc ? argv[i] :  NULL);   }
            else if ( isOption(param,"-h","--help")    ) { printHelpAndExit=1;                           }
            else if ( isOption(param,"-v","--version") ) { printVersionAndExit=1;                        }
        }
    }
    
    /* print help or version if requested */
    if ( printHelpAndExit    ) { i=0; while (help[i]!=NULL) { printf("%s\n",help[i++]); } return 0; }
    if ( printVersionAndExit ) { printf("LINES version %s\n%s\n", VERSION, COPYRIGHT);    return 0; }
    
    /* print all requested files */
    for (i=0; i<fileCount; ++i) {
        printLinesOfText(files[i],printNumbers,firstLine,lastLine,textToFind);
        printf("\n");
    }
    free(files);
    return 0;
}

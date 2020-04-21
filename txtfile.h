/**
 * @file       txtfile.h
 * @date       Apr 21, 2020
 * @author     Martin Rizzo | <martinrizzo@gmail.com>
 * @copyright  Copyright (c) 2020 Martin Rizzo.
 *             This project is released under the MIT License.
 * -------------------------------------------------------------------------
 *  TXTFILE - A portable, one-header library to read lines of text from file
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
#ifndef TXTFILE_H_INCLUDED__
#define TXTFILE_H_INCLUDED__


#include <stdio.h>


#define TXTFILE_INI_BUFSIZE 512
#define TXTFILE_MAX_BUFSIZE (32*1024)


typedef struct TXTFILE {
    FILE* file;
    char* buffer;
    int   bufsize;
    char  initialBuffer[TXTFILE_INI_BUFSIZE];
    char* extendedBuffer;
    int   test;
    
} TXTFILE;



extern TXTFILE* txtfopen(const char* filename, const char* mode);

extern char* txtfgetline(TXTFILE* txtfile);


/**
 * This function is for compatibility with preexistent source code using standard file access
 *
 * Reads characters from text file until (bufsize-1) characters have been read or either
 * a newline or end-of-file is reached, whichever happens first.
 *
 * @param buffer   Pointer to the buffer of chars where the string read will be stored
 * @param bufsize  Maximum number of bytes to be copied into 'buffer' (including '\0')
 * @param txtfile  Pointer to a TXTFILE object that identifies a file opened with the 'txtfopen' function
 * @returns
 *     pointer to 'buffer' when success or NULL when no more lines can be read
 */
extern char* txtfgets(char* buffer, int bufsize, TXTFILE* txtfile);

/**
 * Closes the file associated with the provided 'txtfile' pointer and releases any related resources
 * @param txtfile  Pointer to a TXTFILE object that specifies the file to close
 * @returns        Zero (0) if the file is successfully closed
 */
extern int txtfclose(TXTFILE* txtfile);


/*=================================================================================================================*/
#pragma mark - > IMPLEMENTATION

#ifdef TXTFILE_IMPLEMENTATION
#include <stdlib.h>


TXTFILE* txtfopen(const char* filename, const char* mode) {
    TXTFILE* txtfile=NULL; FILE* file;

    file = fopen(filename,mode);
    if (file) {
        txtfile = malloc(sizeof(TXTFILE));
        txtfile->file           = file;
        txtfile->buffer         = txtfile->initialBuffer;
        txtfile->bufsize        = TXTFILE_INI_BUFSIZE;
        txtfile->extendedBuffer = NULL;
        txtfile->test = 1;
    }
    return txtfile;
}


char* txtfgetline(TXTFILE* txtfile) {
    static char* test = "test";
    if (txtfile->test) { txtfile->test=0; return test; }
    return NULL;
}

char* txtfgets(char* buffer, int bufsize, TXTFILE* txtfile) {
    char* line; int length;
    assert( buffer!=NULL && bufsize>2 );
    assert( txtfile!=NULL );
 
    line = txtfgetline(txtfile); if (line==NULL) { return NULL; }
    length=(int)strlen(line); if (length>(bufsize-2)) { length=(bufsize-2); }
    memcpy(buffer, line, length);
    buffer[length]='\n'; buffer[length+1]='\0';
    return buffer;
}


int txtfclose(TXTFILE* txtfile) {
    if (txtfile!=NULL) {
        free(txtfile->extendedBuffer);
        free(txtfile);
    }
    return 0;
}


#endif /* ifdef TXTFILE_IMPLEMENTATION */

#endif /* ifndef TXTFILE_H_INCLUDED__ */

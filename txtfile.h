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


typedef enum TXTF_ENCODING {
    TXTF_ENCODING_UTF8,         /* < UTF8, ASCI, Windows-1252, ...                 */
    TXTF_ENCODING_UTF8_BOM,     /* < UTF8+BOM [confirmed]                          */
    TXTF_ENCODING_UTF16_LE,     /* < UTF16 little-endian                           */
    TXTF_ENCODING_UTF16_BE,     /* < UTF16 big-endian                              */
    TXTF_ENCODING_UTF16_LE_BOM, /* < UTF16+BOM little-endian [confirmed]           */
    TXTF_ENCODING_UTF16_BE_BOM, /* < UTF16+BOM big-endian [confirmed]              */
    TXTF_ENCODING_BINARY        /* < invalid text file (it's likely a binary file) */
} TXTF_ENCODING;

typedef enum TXTF_NEWLINE {
    TXTF_NEWLINE_WINDOWS,    /* < '\r\n'  =  MS Windows, DOS, CP/M, OS/2, Atari TOS, ...                   */
    TXTF_NEWLINE_UNIX,       /* < '\n'    =  Linux, macOS, BeOS, Amiga, RISC OS, ...                       */
    TXTF_NEWLINE_CLASSICMAC, /* < '\r'    =  Classic Mac OS, C64, C128, ZX Spectrum, TRS-80, Apple II, ... */
    TXTF_NEWLINE_ACORNBBC,   /* < '\n\r'  =  Acorn BBC                                                     */
    TXTF_NEWLINE_UNKNOWN
} TXTF_NEWLINE;


typedef struct TXTFILE {
    FILE* file;
    char* buffer;
    int   bufferSize;
    char* bufferEnd;
    char* nextLine;
    char  initialBuffer[TXTFILE_INI_BUFSIZE];
    char* expandedBuffer;
    int   moreData;
    TXTF_ENCODING encoding;
    TXTF_NEWLINE  newline;
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

char* txtf__readmoredata(TXTFILE* txtfile) {
    int bytesToKeep, bytesToLoad, bytesRead;
    char* bufferToFree=NULL;
    
    bytesToKeep = (int)(txtfile->bufferEnd - txtfile->nextLine);
    bytesToLoad = (txtfile->bufferSize-2) - bytesToKeep;
    /* if not enough space to load a line of text -> expand buffer!! */
    if (bytesToLoad==0) {
        bufferToFree = txtfile->expandedBuffer;
        txtfile->bufferSize    *= 2;
        txtfile->expandedBuffer = malloc(txtfile->bufferSize);
        txtfile->buffer         = txtfile->expandedBuffer;
        bytesToLoad = (txtfile->bufferSize-2) - bytesToKeep;
    }
    if (bytesToKeep) { memmove(txtfile->buffer, txtfile->nextLine, bytesToKeep); }
    bytesRead = (int)fread(&txtfile->buffer[bytesToKeep], sizeof(char), bytesToLoad, txtfile->file);
    txtfile->moreData  = (bytesRead==bytesToLoad);
    txtfile->nextLine  = txtfile->buffer;
    txtfile->bufferEnd = &txtfile->buffer[bytesToKeep+bytesRead];
    if (bufferToFree) { free(bufferToFree); }
    return txtfile->nextLine;
}

void txtf__detectencoding(TXTFILE* txtfile) {
    
}


/*=================================================================================================================*/
#pragma mark - > IMPLEMENTATION


TXTFILE* txtfopen(const char* filename, const char* mode) {
    TXTFILE* txtfile=NULL; FILE* file;

    file = fopen(filename,mode);
    if (file) {
        txtfile = malloc(sizeof(TXTFILE));
        txtfile->file           = file;
        txtfile->buffer         = txtfile->initialBuffer;
        txtfile->bufferSize     = TXTFILE_INI_BUFSIZE;
        txtfile->nextLine       = txtfile->buffer;
        txtfile->bufferEnd      = txtfile->nextLine;
        txtfile->expandedBuffer = NULL;
        txtfile->moreData       = 0;
        
        /* read first chunk of data */
        txtf__readmoredata(txtfile);
        txtf__detectencoding(txtfile);
    }
    return txtfile;
}

char* txtfgetline(TXTFILE* txtfile) {
    char *ptr, *line;
    assert( txtfile!=NULL && txtfile->file!=NULL );
    
    if (txtfile->nextLine==NULL) { return NULL; }
    
    ptr = line = txtfile->nextLine;
    while (ptr==line) {
        /* find end-of-line */
        while (*ptr!='\n' && *ptr!='\r' && ptr<(txtfile->bufferEnd)) { ++ptr; }
        /* if end-of-line is found -> mark it with a string terminator '\0' */
        if      (*ptr=='\r') { *ptr++='\0'; if (*ptr=='\n') { ++ptr; } }
        else if (*ptr=='\n') { *ptr++='\0'; if (*ptr=='\r') { ++ptr; } }
        /* end-of-line NOT found in buffer */
        /* if more data is available -> load more data into buffer and SEARCH AGAIN (ptr=line) */
        /* otherwise the end of file was reached -> mark it with a string terminator '\0'      */
        else if (txtfile->moreData) { ptr=line=txtf__readmoredata(txtfile);  }
        else                        { *ptr='\0'; ptr=NULL; /* end-of-file */ }
    }
    txtfile->nextLine = ptr;
    return line;
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
        free(txtfile->expandedBuffer);
        free(txtfile);
    }
    return 0;
}


#endif /* ifdef TXTFILE_IMPLEMENTATION */

#endif /* ifndef TXTFILE_H_INCLUDED__ */

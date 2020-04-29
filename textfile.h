/**
 * @file       textfile.h
 * @date       Apr 21, 2020
 * @author     Martin Rizzo | <martinrizzo@gmail.com>
 * @copyright  Copyright (c) 2020 Martin Rizzo.
 *             This project is released under the MIT License.
 * -------------------------------------------------------------------------
 *  TextFile - A portable, one-header library to read lines of text from file
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
#ifndef TEXTFILE_H_INCLUDED
#define TEXTFILE_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#define TEXTFILE_INI_BUFSIZE 512
#define TEXTFILE_MAX_BUFSIZE (32*1024)


/*=================================================================================================================*/
#pragma mark - > TEXTFILE INTERFACE

typedef enum TEXTF_ENCODING {
    TEXTF_ENCODING_UTF8,         /* < UTF8, ASCI, Windows-1252, ...                 */
    TEXTF_ENCODING_UTF8_BOM,     /* < UTF8+BOM [confirmed]                          */
    TEXTF_ENCODING_UTF16_LE,     /* < UTF16 little-endian                           */
    TEXTF_ENCODING_UTF16_BE,     /* < UTF16 big-endian                              */
    TEXTF_ENCODING_UTF16_LE_BOM, /* < UTF16+BOM little-endian [confirmed]           */
    TEXTF_ENCODING_UTF16_BE_BOM, /* < UTF16+BOM big-endian [confirmed]              */
    TEXTF_ENCODING_BINARY        /* < invalid text file (it's likely a binary file) */
} TEXTF_ENCODING;

typedef enum TEXTF_EOL {
    TEXTF_EOL_WINDOWS,    /* < '\r\n'  =  MS Windows, DOS, CP/M, OS/2, Atari TOS, ...                   */
    TEXTF_EOL_UNIX,       /* < '\n'    =  Linux, macOS, BeOS, Amiga, RISC OS, ...                       */
    TEXTF_EOL_CLASSICMAC, /* < '\r'    =  Classic Mac OS, C64, C128, ZX Spectrum, TRS-80, Apple II, ... */
    TEXTF_EOL_ACORNBBC,   /* < '\n\r'  =  Acorn BBC                                                     */
    TEXTF_EOL_UNKNOWN
} TEXTF_EOL;

typedef struct TEXTFILE {
    FILE*          file;
    char*          buffer;
    int            bufferSize;
    char*          bufferEnd;
    char*          nextLine;
    TEXTF_ENCODING encoding;
    TEXTF_EOL      eol;
    unsigned int   moreDataAvailable;
    unsigned int   isEncodingSupported;
    char*          expandedBuffer;
    char           initialBuffer[TEXTFILE_INI_BUFSIZE];
} TEXTFILE;

/**
 * Opens a file and returns a TEXTFILE object that controls it
 * @param filename  The path to the file to open
 * @param mode      A null-terminated string determining the file access mode (only the "r" is supported)
 * @returns         The pointer to the TEXTFILE object that controls the opened file or NULL if an error has ocurred
 */
extern TEXTFILE* textfopen(const char* filename, const char* mode);

/**
 * Read a line of text from the provided file
 * @param textfile  The pointer to a TEXTFILE object that controls the file to read the data from
 * @returns         A pointer to the buffer containing the read line or NULL if there isn't more lines to read
 */
extern char* textfgetline(TEXTFILE* textfile);

/**
 * This function is for compatibility with preexistent source code using standard file access
 *
 * Reads characters from text file until (bufsize-1) characters have been read or either
 * a newline or end-of-file is reached, whichever happens first.
 *
 * @param buffer   Pointer to the buffer of chars where the string read will be stored
 * @param bufsize  Maximum number of bytes to be copied into 'buffer' (including '\0')
 * @param textfile Pointer to a TEXTFILE object that identifies a file opened with the 'textfopen' function
 * @returns
 *     pointer to 'buffer' when success or NULL when no more lines can be read
 */
extern char* textfgets(char* buffer, int bufsize, TEXTFILE* textfile);

/**
 * Closes the file associated with the provided TEXTFILE object and releases all related resources
 * @param textfile Pointer to a TEXTFILE object that specifies the file to close
 * @returns        Zero (0) if the file is successfully closed
 */
extern int textfclose(TEXTFILE* textfile);


/**
 * Returns TRUE(1) if the encoding of the provided file is supported
 * @param textfile A pointer to the TEXTFILE object that identifies a file opened with the `textfopen( )` function
 */
#define textfissupported(textfile) (                \
  (textfile->encoding==TEXTF_ENCODING_UTF8    ) ||  \
  (textfile->encoding==TEXTF_ENCODING_UTF8_BOM)     \
)


/*=================================================================================================================*/
#pragma mark - > INTERNAL PRIVATE FUNCTIONS

#ifdef TEXTFILE_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>

static char* textf__readmoredata(TEXTFILE* textfile) {
    int bytesToKeep, bytesToLoad, bytesRead;
    char* bufferToFree=NULL;
    
    bytesToKeep = (int)(textfile->bufferEnd - textfile->nextLine);
    bytesToLoad = (textfile->bufferSize-2) - bytesToKeep;
    /* if not enough space to load a line of text -> expand buffer!! */
    if (bytesToLoad==0) {
        bufferToFree = textfile->expandedBuffer;
        textfile->bufferSize    *= 2;
        textfile->expandedBuffer = malloc(textfile->bufferSize);
        textfile->buffer         = textfile->expandedBuffer;
        bytesToLoad = (textfile->bufferSize-2) - bytesToKeep;
    }
    if (bytesToKeep) { memmove(textfile->buffer, textfile->nextLine, bytesToKeep); }
    bytesRead = (int)fread(&textfile->buffer[bytesToKeep], sizeof(char), bytesToLoad, textfile->file);
    textfile->moreDataAvailable = (bytesRead==bytesToLoad);
    textfile->nextLine          = textfile->buffer;
    textfile->bufferEnd         = &textfile->buffer[bytesToKeep+bytesRead];
    if (bufferToFree) { free(bufferToFree); }
    return textfile->nextLine;
}

static TEXTF_EOL textf__selecteol(int count_r, int count_rn, int count_n, int count_nr) {
    int max=count_r; TEXTF_EOL eol=TEXTF_EOL_CLASSICMAC;
    if (count_rn>max) { max=count_rn; eol=TEXTF_EOL_WINDOWS;  }
    if (count_n >max) { max=count_n ; eol=TEXTF_EOL_UNIX;     }
    if (count_nr>max) { max=count_nr; eol=TEXTF_EOL_ACORNBBC; }
    if (max==0) { eol=TEXTF_EOL_UNKNOWN; }
    return eol;
}

static void textf__detectencoding(TEXTFILE* textfile) {
    static const unsigned char UTF8_BOM[]     = { 239, 187, 191 };
    static const unsigned char UTF16_BE_BOM[] = { 254, 255 };
    static const unsigned char UTF16_LE_BOM[] = { 255, 254 };
    int len, count, oddzeros, evenzeros, notext, isEncodingSupported;
    int eol_rn=0, eol_r=0, eol_nr=0, eol_n=0;
    unsigned char *ptr, *start;
    TEXTF_ENCODING encoding = TEXTF_ENCODING_BINARY;
    assert( textfile!=NULL );
    
    /* detect encoding using BOM (byte order mask) */
    start = (unsigned char*)textfile->nextLine;
    len   = (int)(textfile->bufferEnd - textfile->nextLine);
    if      (len>=3 && memcmp(start,UTF8_BOM    ,3)==0) { encoding=TEXTF_ENCODING_UTF8_BOM    ; start+=3; }
    else if (len>=2 && memcmp(start,UTF16_BE_BOM,2)==0) { encoding=TEXTF_ENCODING_UTF16_BE_BOM; start+=2; }
    else if (len>=2 && memcmp(start,UTF16_LE_BOM,2)==0) { encoding=TEXTF_ENCODING_UTF16_LE_BOM; start+=2; }
    
    /* detect encoding using an heuristic algorithm */
    else {
        oddzeros = evenzeros = notext = 0;
        ptr=start; count=len/2; while (count-->0) {
            if (*ptr==0) { ++oddzeros;  } else if (*ptr<=8 || (14<=*ptr && *ptr<=31)) { ++notext; } ++ptr;
            if (*ptr==0) { ++evenzeros; } else if (*ptr<=8 || (14<=*ptr && *ptr<=31)) { ++notext; } ++ptr;
        }
        if      (oddzeros<(evenzeros/8)) { encoding=TEXTF_ENCODING_UTF16_LE; }
        else if (evenzeros<(oddzeros/8)) { encoding=TEXTF_ENCODING_UTF16_BE; }
        else if (notext==0)              { encoding=TEXTF_ENCODING_UTF8;     }
    }

    /* detect end-of-line for UTF-8 & UTF-8 with BOM */
    if (encoding==TEXTF_ENCODING_UTF8 || encoding==TEXTF_ENCODING_UTF8_BOM) {
        ptr=start; for (count=len-2; count>0; --count,++ptr) {
            if      (ptr[0]=='\r') { if (ptr[1]=='\n') { ++eol_rn; } else { ++eol_r; } }
            else if (ptr[0]=='\n') { if (ptr[1]=='\r') { ++eol_nr; } else { ++eol_n; } }
        }
    }
    /* detect end-of-line for UTF-16BE & UTF-16BE with BOM */
    else if (encoding==TEXTF_ENCODING_UTF16_BE || encoding==TEXTF_ENCODING_UTF16_BE_BOM) {
        ptr=start; for (count=(len/2)-2; count>0; --count,ptr+=2) {
            if (ptr[0]==0) {
                if      (ptr[1]=='\r') { if (ptr[2]==0 && ptr[3]=='\n') { ++eol_rn; } else { ++eol_r; } }
                else if (ptr[1]=='\n') { if (ptr[2]==0 && ptr[3]=='\r') { ++eol_nr; } else { ++eol_n; } }
            }
        }
    }
    /* detect end-of-line for UTF-16LE & UTF-16LE with BOM */
    else if (encoding==TEXTF_ENCODING_UTF16_LE || encoding==TEXTF_ENCODING_UTF16_LE_BOM) {
        ptr=start; for (count=(len/2)-2; count>0; --count,ptr+=2) {
            if (ptr[1]==0) {
                if      (ptr[0]=='\r') { if (ptr[3]==0 && ptr[2]=='\n') { ++eol_rn; } else { ++eol_r; } }
                else if (ptr[0]=='\n') { if (ptr[3]==0 && ptr[2]=='\r') { ++eol_nr; } else { ++eol_n; } }
            }
        }
    }
    /* store results and return */
    isEncodingSupported = (encoding==TEXTF_ENCODING_UTF8) || (encoding==TEXTF_ENCODING_UTF8_BOM);
    textfile->encoding = encoding;
    textfile->eol      = textf__selecteol(eol_r, eol_rn, eol_n, eol_nr);;
    textfile->nextLine = textfissupported(textfile) ? (char*)start : NULL;
}


/*=================================================================================================================*/
#pragma mark - > TEXTFILE IMPLEMENTATION

/**
 * Opens a file and returns a TEXTFILE object that controls it
 *
 * @param filename  The path to the file to open
 * @param mode      A null-terminated string determining the file access mode (only the "r" is supported)
 * @returns         The pointer to the TEXTFILE object that controls the opened file or NULL if an error has ocurred.
 */
TEXTFILE* textfopen(const char* filename, const char* mode) {
    TEXTFILE* textfile=NULL; FILE* file;
    assert( filename!=NULL );
    assert( mode[0]=='r' && mode[1]=='\0' );

    file = fopen(filename,mode);
    if (file) {
        textfile = malloc(sizeof(TEXTFILE));
        textfile->file              = file;
        textfile->buffer            = textfile->initialBuffer;
        textfile->bufferSize        = TEXTFILE_INI_BUFSIZE;
        textfile->nextLine          = textfile->buffer;
        textfile->bufferEnd         = textfile->nextLine;
        textfile->expandedBuffer    = NULL;
        textfile->moreDataAvailable = 0;
        
        /* read first chunk of data */
        textf__readmoredata(textfile);
        textf__detectencoding(textfile);
    }
    return textfile;
}

/**
 * Read a line of text from the provided file
 * @param textfile  The pointer to a TEXTFILE object that controls the file to read the data from
 * @returns         A pointer to the buffer containing the read line or NULL if there isn't more lines to read
 */
char* textfgetline(TEXTFILE* textfile) {
    char *ptr, *line;
    assert( textfile!=NULL && textfile->file!=NULL );
    
    if (textfile->nextLine==NULL) { return NULL; }
    
    ptr = line = textfile->nextLine;
    while (ptr==line) {
        /* find end-of-line */
        while (*ptr!='\n' && *ptr!='\r' && ptr<(textfile->bufferEnd)) { ++ptr; }
        /* if end-of-line is found -> mark it with a string terminator '\0' */
        if      (*ptr=='\r') { *ptr++='\0'; if (*ptr=='\n') { ++ptr; } }
        else if (*ptr=='\n') { *ptr++='\0'; if (*ptr=='\r') { ++ptr; } }
        /* end-of-line NOT found in buffer */
        /* if more data is available -> load more data into buffer and SEARCH AGAIN (ptr=line) */
        /* otherwise the end of file was reached -> mark it with a string terminator '\0'      */
        else if (textfile->moreDataAvailable) { ptr=line=textf__readmoredata(textfile); }
        else                                  { *ptr='\0'; ptr=NULL; /* end-of-file */  }
    }
    textfile->nextLine = ptr;
    return line;
}

/**
 * This function is for compatibility with preexistent source code using standard file access
 *
 * Reads characters from text file until (bufsize-1) characters have been read or either
 * a newline or end-of-file is reached, whichever happens first.
 *
 * @param buffer   Pointer to the buffer of chars where the string read will be stored
 * @param bufsize  Maximum number of bytes to be copied into 'buffer' (including '\0')
 * @param textfile Pointer to a TEXTFILE object that identifies a file opened with the 'textfopen' function
 * @returns
 *     pointer to 'buffer' when success or NULL when no more lines can be read
 */
char* textfgets(char* buffer, int bufsize, TEXTFILE* textfile) {
    char* line; int length;
    assert( buffer!=NULL && bufsize>2 );
    assert( textfile!=NULL );
 
    line = textfgetline(textfile); if (line==NULL) { return NULL; }
    length=(int)strlen(line); if (length>(bufsize-2)) { length=(bufsize-2); }
    memcpy(buffer, line, length);
    buffer[length]='\n'; buffer[length+1]='\0';
    return buffer;
}

/**
 * Closes the file associated with the provided TEXTFILE object and releases all related resources
 * @param textfile Pointer to a TEXTFILE object that specifies the file to close
 * @returns        Zero (0) if the file is successfully closed
 */
int textfclose(TEXTFILE* textfile) {
    if (textfile!=NULL) {
        free(textfile->expandedBuffer);
        free(textfile);
    }
    return 0;
}


#endif /* ifdef TEXTFILE_IMPLEMENTATION */

#endif /* ifndef TEXTFILE_H_INCLUDED */

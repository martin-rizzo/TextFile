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


static void printLines(const char* filename, int firstLine, int lastLine, const char* textToFind) {
    TXTFILE* txtfile; int lineNumber; const char* line;
    assert( filename!=NULL );
    
    txtfile = txtfopen(filename, "r");
    if (txtfile==NULL) { return; }
    
    lineNumber=1; line=""; while (line!=NULL) {
        line = txtfgetline(txtfile);
        if (line!=NULL) {
            if ( (firstLine<=0 || firstLine<=lineNumber) && (lastLine<=0 || lineNumber<=lastLine) ) {
                if (textToFind==NULL || strstr(line,textToFind)!=NULL) {
                    printf("%d: %s\n", lineNumber++, line);
                }
            }
        }
    }
    txtfclose(txtfile);
}

int main(int argc, char *argv[]) {
    
    if (argc>=2) {
        printLines(argv[1], -1, -1, NULL);
    }
    return 0;
}

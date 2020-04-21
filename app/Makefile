
RM       = rm -f
BIN_DIR  = .

TARGET_RELEASE = $(BIN_DIR)/lines
TARGET_DEBUG   = $(BIN_DIR)/lines-debug
HEADERS        = ../txtfile.h
SOURCE         = lines.c

## compiler flags ##
CONFIG_RELEASE = -Os -DNDEBUG
CONFIG_DEBUG   = -O0 -D_DEBUG
CFLAGS_ANSI    = -ansi
CFLAGS_ERRORS  = -Wall -pedantic-errors -Wno-unused-function
CFLAGS         = $(CFLAGS_ANSI) $(CFGLAS_ERRORS)



.PHONY: all debug release clean

all: release debug


#----------------------------------------------
# DEBUG
#
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) $(CONFIG_DEBUG) $< -o $@


#-----------------------------------------------
# RELEASE
#
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) $(CONFIG_RELEASE) $< -o $@


#-----------------------------------------------
# CLEAN
#
clean:
	$(RM) $(TARGET_RELEASE) $(TARGET_DEBUG)


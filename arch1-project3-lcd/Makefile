all:
	(cd timerLib; make install)
	(cd lcdLib; make install)
	(cd wakedemo; make)
#   (cd circledemo; make)
	(cd game; make)
	(cd music; make)


doc:
	rm -rf doxygen_docs
	doxygen Doxyfile
clean:
	(cd timerLib; make clean)
	(cd lcdLib; make clean)
#   (cd circledemo; make clean)
	(cd game; make clean)
	(cd music; make clean)
	rm -rf lib h
	rm -rf doxygen_docs/*

# # Compiler and Flags
# CC = msp430-gcc
# CFLAGS = -mmcu=msp430g2553 -Ih -IlcdLib -ItimerLib -Os

# # Directories
# SRC_DIR = src
# LIB_DIR = lib
# TIMERLIB_DIR = timerLib
# LCDLIB_DIR = lcdLib

# # Source and Object Files
# SRC = $(wildcard $(SRC_DIR)/*.c)
# OBJ = $(SRC:.c=.o)

# # Libraries
# LIBS = $(LIB_DIR)/libTimer.a $(LIB_DIR)/libLcd.a

# # Output Executable
# EXEC = game.elf

# # Default Target
# .PHONY: all clean

# all: $(EXEC)

# $(EXEC): $(OBJ) $(LIBS)
# 	$(CC) $(CFLAGS) -o $@ $^

# # Build Timer Library
# $(LIB_DIR)/libTimer.a:
# 	$(MAKE) -C $(TIMERLIB_DIR)
# 	cp $(TIMERLIB_DIR)/libTimer.a $(LIB_DIR)/

# # Build LCD Library
# $(LIB_DIR)/libLcd.a:
# 	$(MAKE) -C $(LCDLIB_DIR)
# 	cp $(LCDLIB_DIR)/libLcd.a $(LIB_DIR)/

# # Compile C Files to Object Files
# %.o: %.c
# 	$(CC) $(CFLAGS) -c -o $@ $<

# # Clean
# clean:
# 	rm -f $(OBJ) $(EXEC)
# 	$(MAKE) -C $(TIMERLIB_DIR) clean
# 	$(MAKE) -C $(LCDLIB_DIR) clean

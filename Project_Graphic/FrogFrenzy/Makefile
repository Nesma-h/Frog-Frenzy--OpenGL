# =============================================
# Makefile - Frog Frenzy 3D
# =============================================

# ===== Linux / Mac =====
CC      = g++
TARGET  = FrogFrenzy
SOURCES = main.cpp
CFLAGS  = -O2 -Wall -std=c++11

# Linux flags
LINUX_LIBS = -lGL -lGLU -lglut -lm
# Mac flags  
MAC_LIBS   = -framework OpenGL -framework GLUT

# Auto-detect OS
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
    LIBS = $(MAC_LIBS)
    CFLAGS += -Wno-deprecated
else
    LIBS = $(LINUX_LIBS)
endif

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)
	@echo "Build successful! Run: ./$(TARGET)"

clean:
	rm -f $(TARGET) *.o

run: all
	./$(TARGET)

.PHONY: all clean run

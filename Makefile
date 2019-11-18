#CC		:= gcc
#CXX		:= g++
#
#SRC_DIR := src
#OBJ_DIR := obj
#SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
#OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
#LDLIBS := -lncurses -lm
#CXXFLAGS := -std=c++11 -O0 -g -Wall
#
#main: $(OBJ_FILES)
#	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
#
#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
#	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDLIBS)
#
#clean:
#	rm -rf $(OBJ_FILES) main


EXTENSION := main

CC := gcc
CXX := g++

SRC_DIR := src
OBJ_DIR := obj

CXX_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
CXX_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CXX_SOURCES))
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))

LDLIBS := -lncurses -lm
CFLAGS := -g -Wall
CXXFLAGS := -std=c++11

all: $(EXTENSION)

$(EXTENSION): $(CXX_OBJECTS) $(C_OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo ".cpp -> .o"
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo ".c -> .o"
	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

clean:
	rm -rf $(CXX_OBJECTS) $(C_OBJECTS) $(EXTENSION) core

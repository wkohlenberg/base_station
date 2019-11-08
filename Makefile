CC		:= gcc
CXX		:= g++

SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDLIBS := -lncurses -lm
CXXFLAGS := -std=c++11 -O0 -g

main: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(INC_WSPP) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_WSPP) -c -o $@ $< $(LDLIBS)

clean:
	rm -rf $(OBJ_DIR)/$(OBJ_FILES) main

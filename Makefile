# compilers and shells
SHELL = bash
CXX = g++

# compiler flags
CXXFLAGS = -Wall -Werror -std=c++20 -pthread

# project directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DBG_DIR = debug
HDR_DIR = headers

# project files
EXEC_SRC = main.cpp
SRC = $(filter-out $(patsubst %, $(SRC_DIR)/%, $(EXEC_SRC)), $(wildcard $(SRC_DIR)/*.cpp))
HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
OBJS = $(C_SRC:.c=.o)

# regular build settings
BINARIES = $(addsuffix .o, $(addprefix $(OBJ_DIR)/, $(notdir $(patsubst %.cpp, %, $(SRC)))))
EXECUTABLES = $(patsubst %.cpp, %.exe, $(EXEC_SRC))

# execute these actions regardless of folder contents
.PHONY: all clean debug

all: directory $(BINARIES) $(addprefix $(BIN_DIR)/, $(EXECUTABLES))

clean:
	rm -f $(BIN_DIR)/*.exe $(OBJ_DIR)/*.o test/*.exe

debug_clean:
	rm -f $(BIN_DIR)/*.exe

directory:
	mkdir -p obj
	mkdir -p bin

debug: CXXFLAGS += -g -DDEBUG
debug: debug_clean all

$(EXECUTABLES): $(BINARIES)

$(BIN_DIR)/%.exe: $(SRC_DIR)/%.cpp $(BINARIES)
	$(CXX) $(CXXFLAGS) -I $(HDR_DIR) $(BINARIES) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I $(HDR_DIR) -c $< -o $@
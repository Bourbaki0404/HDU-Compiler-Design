# Makefile for C++ Compiler Project

# Configuration
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I./include
LEXER_DIR = ./lexer
PARSER_DIR = ./parser
BUILD_DIR = ./build
SRC_DIR = .
AST_DIR = ./astnode

# Source files
MAIN_SOURCE = $(SRC_DIR)/main.cpp
LEXER_SOURCES = $(wildcard $(LEXER_DIR)/*.cpp)
PARSER_SOURCES = $(wildcard $(PARSER_DIR)/*.cpp)
AST_SOURCES = $(wildcard $(AST_DIR)/*cpp)
SOURCES = $(MAIN_SOURCE) $(LEXER_SOURCES) $(PARSER_SOURCES) $(AST_SOURCES)

# Object files
OBJECTS = $(BUILD_DIR)/main.o \
          $(patsubst $(LEXER_DIR)/%.cpp,$(BUILD_DIR)/lexer/%.o,$(LEXER_SOURCES)) \
          $(patsubst $(PARSER_DIR)/%.cpp,$(BUILD_DIR)/parser/%.o,$(PARSER_SOURCES))\
		  $(patsubst $(AST_DIR)/%.cpp,$(BUILD_DIR)/astnode/%.o,$(AST_SOURCES))\



# Targets
.PHONY: all clean compiler test

all: compiler

# Create build directory structure
$(BUILD_DIR)/lexer $(BUILD_DIR)/parser $(BUILD_DIR)/astnode:
	mkdir -p $@

# Main compiler executable
compiler: $(BUILD_DIR)/lexer $(BUILD_DIR)/parser $(BUILD_DIR)/astnode $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/compiler $(OBJECTS)

# Main source file
$(BUILD_DIR)/main.o: $(MAIN_SOURCE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Lexer source files
$(BUILD_DIR)/lexer/%.o: $(LEXER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Parser source files
$(BUILD_DIR)/parser/%.o: $(PARSER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# AST source files
$(BUILD_DIR)/astnode/%.o: $(AST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test target
test: compiler
	$(BUILD_DIR)/compiler test_input.c

clean:
	rm -rf $(BUILD_DIR)
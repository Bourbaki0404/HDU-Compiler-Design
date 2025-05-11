# Makefile for C++ Compiler Project with LLVM 14 support using clang++

# Configuration
CXX = clang++
LLVM_CONFIG = llvm-config
CXXFLAGS = -std=c++14 -Wall -Wextra -g -I./include \
  -isystem/usr/include/x86_64-linux-gnu/c++/11 \
  -isystem/usr/include/c++/11 \
  $(shell $(LLVM_CONFIG) --cxxflags)

CXXFLAGS += -frtti
CXXFLAGS += -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1
CXXFLAGS += -MMD -MP  # Dependency tracking
# Linker flags
LDFLAGS = \
  -L/usr/lib/gcc/x86_64-linux-gnu/11 \
  -L/usr/lib/x86_64-linux-gnu \
  -L/usr/lib/llvm-14/lib \
  $(shell $(LLVM_CONFIG) --ldflags) \
  $(shell $(LLVM_CONFIG) --libs core orcjit native) \
  $(shell $(LLVM_CONFIG) --system-libs)

TEST_INPUT = ./test_input.cpp

LEXER_DIR = ./lexer
PARSER_DIR = ./parser
AST_DIR = ./astnode
TYPES_DIR = ./type
SYMTABLE_DIR = ./symbolTable
CODEGEN_DIR = ./codeGen
IR_DIR = ./IR
SRC_DIR = .
BUILD_DIR = ./build

# Source files
MAIN_SOURCE = $(SRC_DIR)/main.cpp
LEXER_SOURCES = $(wildcard $(LEXER_DIR)/*.cpp)
PARSER_SOURCES = $(wildcard $(PARSER_DIR)/*.cpp)
AST_SOURCES = $(wildcard $(AST_DIR)/*.cpp)
TYPES_SOURCES = $(wildcard $(TYPES_DIR)/*.cpp)
SYMTABLE_SOURCES = $(wildcard $(SYMTABLE_DIR)/*.cpp)
CODEGEN_SOURCES = $(wildcard $(CODEGEN_DIR)/*.cpp)
IR_SOURCES = $(wildcard $(IR_DIR)/*.cpp)

SOURCES = $(MAIN_SOURCE) $(LEXER_SOURCES) $(PARSER_SOURCES) $(AST_SOURCES) \
          $(TYPES_SOURCES) $(SYMTABLE_SOURCES) $(CODEGEN_SOURCES) $(IR_SOURCES)

# Object files
OBJECTS = $(BUILD_DIR)/main.o \
          $(patsubst $(LEXER_DIR)/%.cpp,$(BUILD_DIR)/lexer/%.o,$(LEXER_SOURCES)) \
          $(patsubst $(PARSER_DIR)/%.cpp,$(BUILD_DIR)/parser/%.o,$(PARSER_SOURCES)) \
          $(patsubst $(AST_DIR)/%.cpp,$(BUILD_DIR)/astnode/%.o,$(AST_SOURCES)) \
          $(patsubst $(TYPES_DIR)/%.cpp,$(BUILD_DIR)/types/%.o,$(TYPES_SOURCES)) \
          $(patsubst $(SYMTABLE_DIR)/%.cpp,$(BUILD_DIR)/symbolTable/%.o,$(SYMTABLE_SOURCES)) \
          $(patsubst $(CODEGEN_DIR)/%.cpp,$(BUILD_DIR)/codegen/%.o,$(CODEGEN_SOURCES)) \
          $(patsubst $(IR_DIR)/%.cpp,$(BUILD_DIR)/IR/%.o,$(IR_SOURCES))

# Targets
.PHONY: all clean compiler test

all: compiler

# Create build directory structure
$(BUILD_DIR)/lexer $(BUILD_DIR)/parser $(BUILD_DIR)/astnode $(BUILD_DIR)/types \
$(BUILD_DIR)/symbolTable $(BUILD_DIR)/codegen $(BUILD_DIR)/IR:
	mkdir -p $@

# Main compiler executable
compiler: $(BUILD_DIR)/lexer $(BUILD_DIR)/parser $(BUILD_DIR)/astnode \
          $(BUILD_DIR)/types $(BUILD_DIR)/symbolTable $(BUILD_DIR)/codegen $(BUILD_DIR)/IR \
          $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/compiler $(OBJECTS) $(LDFLAGS)

# Main source file
$(BUILD_DIR)/main.o: $(MAIN_SOURCE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Lexer
$(BUILD_DIR)/lexer/%.o: $(LEXER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Parser
$(BUILD_DIR)/parser/%.o: $(PARSER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# AST
$(BUILD_DIR)/astnode/%.o: $(AST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Types
$(BUILD_DIR)/types/%.o: $(TYPES_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Symbol Table
$(BUILD_DIR)/symbolTable/%.o: $(SYMTABLE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# CodeGen
$(BUILD_DIR)/codegen/%.o: $(CODEGEN_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# IR
$(BUILD_DIR)/IR/%.o: $(IR_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test target
test: compiler
	$(BUILD_DIR)/compiler $(TEST_INPUT)

# Print IR from output file
printIR: out.ll
	echo $@ Below:
	cat $<

Play:
	$(CXX) -S -emit-llvm $(TEST_INPUT) -o ./test_input.ll
	cat ./test_input.ll

dag-view:
	$(CXX) -S -emit-llvm $(TEST_INPUT) -o ./test_input.ll
	@echo "\n=== Generated LLVM IR ==="
	@cat ./test_input.ll
	@echo "\n=== Generating SelectionDAG visualizations ==="

	GRAPHVIZ_DOT=dot llc \
		--fast-isel=false \
		--march=riscv32 \
		--view-dag-combine1-dags \
		--view-legalize-types-dags \
		--view-dag-combine-lt-dags \
		--view-legalize-dags \
		--view-dag-combine2-dags \
		--view-isel-dags \
		--view-sched-dags \
		./test_input.ll

	@echo "\n=== Generating Graphs ==="
	@for f in *.dot; do \
		if [ -f $$f ]; then \
			echo "Converting $$f to PNG..."; \
			$(GRAPHVIZ_DOT) -Tpng $$f -o $$f.png; \
		else \
			echo "No .dot files generated"; \
		fi; \
	done

	@echo "\n=== Generated SelectionDAG Visualizations ==="
	@ls -l *.png

# Clean target
clean:
	rm -rf $(BUILD_DIR)

# Check LLVM availability
ifeq (, $(shell which $(LLVM_CONFIG)))
$(error "LLVM config not found. Please install llvm-14 and ensure llvm-config-14 is in PATH.")
endif

-include $(OBJECTS:.o=.d)

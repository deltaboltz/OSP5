SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXE1 := $(BIN_DIR)/oss
FILE1 := $(OBJ_DIR)/oss.o
SRC := $(wildcard $(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.0)


CC := g++ -std=c++11
CPPFLAGS := -Iinclude -MMD -MP
CFLAGS := -Wall -g

.PHONY: all clean

all: $(EXE1)

$(EXE1): $(filter-out $(FILE1), $(OBJ)) | $(BIN_DIR)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.0: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
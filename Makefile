# ==========================================
# Le Mans 24h Simulation - Makefile
# ==========================================

# Compiler and Flags
CC = gcc
# -Iinclude tells the compiler to look for header files in the 'include' folder
# -Wall -Wextra: Enable standard warnings
# -g: Add debug information (for gdb/valgrind)
CFLAGS = -Wall -Wextra -Iinclude -g

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source and Object files
# Automatically find all .c files in src/
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Convert the .c filenames to .o filenames inside the build directory
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Output Executable Name
TARGET = $(BUILD_DIR)/lemans_sim

# ==========================================
# Rules
# ==========================================

# Default target
all: $(TARGET)

# Linking phase: Create the executable from object files
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Build successful!"

# Compilation phase: Create object files from source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

# Run the simulation (shortcut)
run: all
	@echo "Running simulation..."
	./$(TARGET)

.PHONY: all clean run
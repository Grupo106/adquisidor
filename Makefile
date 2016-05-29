BANNER = "Universidad Nacional de La Matanza. 2016 - $(PROGRAM)"

PROGRAM := adquisidor# nombre del programa

# Directorios
# ---------------------------------------------------------------------------
SRC_DIR := ./src
BUILD_DIR := ./build
BIN_DIR := ./bin
INSTALL_DIR := /usr/local/sbin

# Binarios
# ---------------------------------------------------------------------------
ECPG := $(shell which ecpg)
CC := $(shell which gcc)

# Versiones
# ---------------------------------------------------------------------------
REVISION := $(shell git rev-parse HEAD) # obtiene la revision desde el ultimo
			   						    # commit

# FLAGS
# ---------------------------------------------------------------------------
COMPILE_FLAGS := -Wall -Wextra -g -O2 -D"REVISION=\"$(REVISION)\""
LINK_FLAGS := -lecpg -lpcap

# Archivos de codigo fuente
# ---------------------------------------------------------------------------
C_SOURCES := $(shell find $(SRC_PATH) -name '*.c' -printf '%T@\t%p\n' \
			   | sort -k 1nr | cut -f2-)
PGC_SOURCES := $(shell find $(SRC_PATH) -name '*.pgc' -printf '%T@\t%p\n' \
				 | sort -k 1nr | cut -f2-)

# Archivos de objeto
# ---------------------------------------------------------------------------
OBJECTS := $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJECTS += $(PGC_SOURCES:$(SRC_DIR)/%.pgc=$(BUILD_DIR)/%.o)


# Compila todos los binarios
all: banner dirs $(BIN_DIR)/$(PROGRAM)
	@echo "$(PROGRAM) se ha compilado correctamente"

# Crea los directorios necesarios
dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Limpia archivos generados por el make
clean: banner
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$@: Se limpiaron los archivos generados"

# Instala binarios en el host (requiere privilegios de root)
install: banner
	cp $(BIN_DIR)/$(PROGRAM) $(INSTALL_DIR)
	@echo "$(PROGRAM) instalado correctamente"

banner:
	@echo $(BANNER)

.PHONY: all dirs clean install banner

# Compilacion final
# ---------------------------------------------------------------------------
$(BIN_DIR)/$(PROGRAM): $(OBJECTS)
	@echo "Compilando $<"
	@$(CC) $(LINK_FLAGS) -o $@ $(OBJECTS)

# Compilacion de archivos .c
# ---------------------------------------------------------------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compilando $<"
	@$(CC) $(COMPILE_FLAGS) -o $@ -c $<

# Compilacion de archivos .pgc (postgresql embebido en C)
# ---------------------------------------------------------------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.pgc
	@echo "Compilando $<"
	@$(ECPG) -o $(BUILD_DIR)/$*.c $<
	@$(CC) $(COMPILE_FLAGS) -I$(SRC_DIR) -o $@ -c $(BUILD_DIR)/$*.c

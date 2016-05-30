# Banner que muestra al inicio
BANNER = "$(PROGRAM) - Grupo 106 - Universidad Nacional de La Matanza. 2016"

# Nombre del programa
PROGRAM := adquisidor

# Directorios
# ---------------------------------------------------------------------------
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INSTALL_DIR := /usr/local/sbin
RELEASE_DIR := release
DEBUG_DIR := debug

# Binarios
# ---------------------------------------------------------------------------
ECPG := $(shell which ecpg)
CC := $(shell which gcc)

# Versiones
# ---------------------------------------------------------------------------

# obtiene la revision desde el ultimo commit
REVISION := $(shell git describe --tags)

# FLAGS
# ---------------------------------------------------------------------------
# flash de gcc
C_FLAGS := -Wall -Wextra \
          -D"REVISION=\"$(REVISION)\"" \
					-D"PROGRAM=\"$(PROGRAM)\"" \
					-I/usr/include/postgresql
# flags de produccion
R_FLAGS := -O3
# flags de desarrollo
D_FLAGS := -g -D"DEBUG"
# flash de link final
LINK_FLAGS := -lecpg -lpcap
# flash de ecpg
EPCG_FLAGS := -D"POSTGRES_CONNECTION_STRING=\"postgres@172.17.0.2\"" \
              -D"POSTGRES_USER=\"postgres\"" \
              -D"POSTGRES_PASSWD=\"root\""

# Archivos de codigo fuente
# ---------------------------------------------------------------------------
C_SOURCES := $(shell find $(SRC_DIR) -name '*.c' -printf '%T@\t%p\n' \
			   | sort -k 1nr | cut -f2-)
PGC_SOURCES := $(shell find $(SRC_DIR) -name '*.pgc' -printf '%T@\t%p\n' \
				 | sort -k 1nr | cut -f2-)

# Archivos objeto
# ---------------------------------------------------------------------------
OBJECTS := $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJECTS += $(PGC_SOURCES:$(SRC_DIR)/%.pgc=$(BUILD_DIR)/%.o)

# Version de produccion y de debug
release: export CFLAGS := $(CFLAGS) $(C_FLAGS) $(R_FLAGS)
debug: export CFLAGS := $(CFLAGS) $(C_FLAGS) $(D_FLAGS)
release: export BIN_PATH := $(BIN_DIR)/$(RELEASE_DIR)
debug: export BIN_PATH := $(BIN_DIR)/$(DEBUG_DIR)

debug: banner
	@echo "Empezando construccion en modo desarrollo"
	@$(MAKE) all --no-print-directory

release: banner
	@echo "Empezando construccion en modo produccion"
	@$(MAKE) all --no-print-directory

# Compila todos los binarios
all: dirs $(BIN_PATH)/$(PROGRAM)
	@echo "$(BIN_PATH)/$(PROGRAM) está en la versión $(REVISION)"

# Crea los directorios necesarios
dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_PATH)

# Limpia archivos generados por el make
clean: banner
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$@: Se limpiaron los archivos generados"

# Instala binarios en el host (requiere privilegios de root)
install: banner
	install $(BIN_DIR)/$(RELEASE_DIR)/$(PROGRAM) $(INSTALL_DIR)
	@echo "$(PROGRAM) instalado correctamente"

# Desinstala binarios en el host (requiere privilegios de root)
uninstall: banner
	rm -f $(INSTALL_DIR)/$(PROGRAM)
	@echo "$(PROGRAM) desinstalado correctamente"

banner:
	@echo $(BANNER)
	@echo ----------------------------------------------------------------------

.PHONY: all dirs clean install banner debug release

# Compilacion final
# ---------------------------------------------------------------------------
$(BIN_PATH)/$(PROGRAM): $(OBJECTS)
	@echo "Construyendo $@"
	@$(CC) $(LINK_FLAGS) -o $@ $(OBJECTS)

# Compilacion de archivos .c
# ---------------------------------------------------------------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compilando $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

# Compilacion de archivos .pgc (postgresql embebido en C)
# ---------------------------------------------------------------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.pgc
	@echo "Compilando $<"
	@$(ECPG) $(EPCG_FLAGS) -o $(BUILD_DIR)/$*.c $<
	@$(CC) $(CFLAGS) -I$(SRC_DIR) -o $@ -c $(BUILD_DIR)/$*.c

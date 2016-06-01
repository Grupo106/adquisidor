# Banner que muestra al inicio
BANNER = "$(PROGRAM) - Grupo 106 - Universidad Nacional de La Matanza. 2016"

# Nombre del programa
PROGRAM := adquisidor

# Directorios
# ---------------------------------------------------------------------------
# * carpeta donde se encuentra el codigo fuente
SRC_DIR := src
# * carpeta donde se crearan los archivos de codigo objeto y todos aquellos
# temporales necesarios para la construccion del binario
BUILD_DIR := build
# * carpeta donde se crearan los binararios
BIN_DIR := bin
# * carpeta donde se instalarán los binarios productivos
INSTALL_DIR := /usr/local/sbin
# * carpeta dentro de BIN_DIR y BUILD_DIR donde se almacenaran los binarios
# productivos
RELEASE_DIR := release
# * carpeta dentro de BIN_DIR y BUILD_DIR donde se almacenaran los binarios de
# desarrollo
DEBUG_DIR := debug

# Binarios
# ---------------------------------------------------------------------------
# * programa para compilar los archivos .pgc
ECPG := $(shell which ecpg)
# * programa para compilar los archivos .c
CC := $(shell which gcc)

# Versiones
# ---------------------------------------------------------------------------
# * obtiene la revision desde el ultimo commit
REVISION := $(shell git describe --tags)

# FLAGS
# ---------------------------------------------------------------------------
# * flags de gcc
C_FLAGS := -Wall -Wextra \
			-D"REVISION=\"$(REVISION)\"" \
			-D"PROGRAM=\"$(PROGRAM)\"" \
			-I/usr/include/postgresql
# * flags de produccion
R_FLAGS := -O3
# * flags de desarrollo
D_FLAGS := -g -D"DEBUG"
# * flash de link final
LINK_FLAGS := -lecpg -lpcap
# * flash de ecpg de produccion
R_EPCG_FLAGS := -D"POSTGRES_CONNECTION_STRING=\"unix:postgresql://localhost/$(POSTGRESQL_DB)\"" \
              -D"POSTGRES_USER=\"$(POSTGRESQL_USER)\"" \
              -D"POSTGRES_PASSWD=\"$(POSTGRESQL_PASSWORD)\""
# * flash de ecpg de desarrollo
D_EPCG_FLAGS := -D"POSTGRES_CONNECTION_STRING=\"postgres@localhost\"" \
              -D"POSTGRES_USER=\"postgres\"" \
              -D"POSTGRES_PASSWD=\"postgres\"" \
						  -D"DEBUG"

# Archivos de codigo fuente
# ---------------------------------------------------------------------------
C_SOURCES := $(shell find $(SRC_DIR) -name '*.c' -printf '%T@\t%p\n' \
			   | sort -k 1nr | cut -f2-)
PGC_SOURCES := $(shell find $(SRC_DIR) -name '*.pgc' -printf '%T@\t%p\n' \
				 | sort -k 1nr | cut -f2-)


# Variables dependientes del modo de compilacion (desarrollo o produccion)
# ---------------------------------------------------------------------------
release: export CFLAGS := $(CFLAGS) $(C_FLAGS) $(R_FLAGS)
release: export EPGCFLAGS := $(EPCGFLAGS) $(EPCG_FLAGS) $(R_EPCG_FLAGS)
release: export BIN_PATH := $(BIN_DIR)/$(RELEASE_DIR)
release: export BUILD_PATH := $(BUILD_DIR)/$(RELEASE_DIR)
debug: export CFLAGS := $(CFLAGS) $(C_FLAGS) $(D_FLAGS)
debug: export EPGCFLAGS := $(EPCGFLAGS) $(EPCG_FLAGS) $(D_EPCG_FLAGS)
debug: export BIN_PATH := $(BIN_DIR)/$(DEBUG_DIR)
debug: export BUILD_PATH := $(BUILD_DIR)/$(DEBUG_DIR)

# Archivos objeto
# ---------------------------------------------------------------------------
OBJECTS = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_PATH)/%.o)
OBJECTS += $(PGC_SOURCES:$(SRC_DIR)/%.pgc=$(BUILD_PATH)/%.o)

# Acciones
# ---------------------------------------------------------------------------
release: banner
	@echo "Empezando construcción en modo PRODUCCIÓN"
	@$(MAKE) all --no-print-directory

debug: banner
	@echo "Empezando construcción en modo DESARROLLO"
	@$(MAKE) all --no-print-directory

# Compila todos los binarios
all: dirs $(BIN_PATH)/$(PROGRAM)
	@echo "$(BIN_PATH)/$(PROGRAM) está en la versión $(REVISION)"

# Crea los directorios necesarios
dirs:
	@mkdir -p $(BUILD_PATH) $(BIN_PATH)

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

# Compilacion de archivos .c
# ---------------------------------------------------------------------------
$(BUILD_PATH)/%.o: $(SRC_DIR)/%.c
	@echo "Compilando $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

# Compilacion de archivos .pgc (postgresql embebido en C)
# ---------------------------------------------------------------------------
$(BUILD_PATH)/%.o: $(SRC_DIR)/%.pgc
	@echo "Compilando $<"
	@$(ECPG) $(EPGCFLAGS) -o $(BUILD_PATH)/$*.c $<
	@$(CC) $(CFLAGS) -I$(SRC_DIR) -o $@ -c $(BUILD_PATH)/$*.c
#
# Compilacion final
# ---------------------------------------------------------------------------
$(BIN_PATH)/$(PROGRAM): $(OBJECTS)
	@echo "Construyendo $@"
	@$(CC) $(LINK_FLAGS) -o $@ $(OBJECTS)

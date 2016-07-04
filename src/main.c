#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>

#include "adquisidor.h"
#include "db.h"

#ifndef REVISION
#define REVISION "DESCONOCIDA"
#endif /* REVISION */

#ifndef PROGRAM
#define PROGRAM "adquisidor"
#endif /* PROGRAM */

#ifdef DEBUG
#define BUILD_MODE "desarrollo"
#else
#define BUILD_MODE "produccion"
#endif /* BUILD_MODE */

/**
 * terminar()
 * ---------------------------------------------------------------------------
 * Termina el programa de forma segura. Cierra conexion de base de datos y
 * termina la captura de paquetes
 */
static void terminar(int);

/**
 * manejar_interrupciones()
 * ---------------------------------------------------------------------------
 * Registra señales que serán enviadas por el sistema operativo para el correcto
 * cierre de base de datos y de interfaz de red.
 */
static void manejar_interrupciones();

/**
 * banner()
 * ---------------------------------------------------------------------------
 * Imprime banner con version del programa
 */
static void banner();

/*
 * argumentos()
 * ---------------------------------------------------------------------------
 *  Maneja los argumentos pasados por parametro al programa. Devuelve cero en
 *  caso de exito, cualquier otro numero en caso de error
 * 
 */
static int argumentos(int argc, const char* argv[], struct config *cfg);

int main(int argc, const char* argv[]) {
    struct config cfg;
    /* Inicializo logs */
    openlog(PROGRAM, LOG_CONS | LOG_PID, LOG_LOCAL0);
    /* Parseo argumentos */
    argumentos(argc, argv, &cfg);
    /* Imprimo banner con version */
    banner();
    /* muestro informacion del build */
    syslog(LOG_INFO, "Revision: %s (%s)", REVISION, BUILD_MODE);
    /* conecto base de datos */
    int sqlret = bd_conectar();
    if(sqlret != 0)
        return(sqlret);
    /*
     * registro señales necesarias para cerrar correctamente el programa y
     * para liberar recursos
     */
    manejar_interrupciones();
    /* inicio captura de paquete de red */
    captura_inicio(&cfg);
    return EXIT_SUCCESS;
}

/**
 * terminar()
 * ---------------------------------------------------------------------------
 * Cierra conexion de base de datos y termina la captura de paquetes
 */
static void terminar(int signum) {
    syslog(LOG_WARNING, "Interrupción recibida %d\n", signum);
    bd_desconectar();
    captura_fin();
    closelog();
    exit(EXIT_SUCCESS);
}

/**
 * manejar_interrupciones()
 * ---------------------------------------------------------------------------
 * Registra señales que serán enviadas por el sistema operativo para el correcto
 * cierre de base de datos y de interfaz de red.
 */
static void manejar_interrupciones() {
    signal(SIGINT, terminar);
    signal(SIGTERM, terminar);
    signal(SIGQUIT, terminar);
}

/**
 * banner()
 * ---------------------------------------------------------------------------
 * Imprime banner con version del programa
 */
static void banner() {
    printf("\
---------------------------------------------------------------------------\n\
Netcop - Universidad Nacional de La Matanza. 2016\n\
%s - Revision: %s (%s)\n\
---------------------------------------------------------------------------\n",
    PROGRAM, REVISION, BUILD_MODE);
}

/*
 * ayuda()
 * --------------------------------------------------------------------------
 *  Muestra mensaje de ayuda
 */
static void ayuda() {
    printf("\
Este programa captura los paquetes ENTRANTES de la interfaz pasada por\n\
parámetro. Si ninguna interfaz es establecida, se capturará en la primer\n\
interfaz disponible.\n\
\n\
Uso: %s -h | -v \n\
     %s device outbound|inbound \n\
\n\
-h --help     Muestra esta ayuda.\n\
\n\
-v --version  Muestra numero de version.\n\
\n\
device        Nombre de la interfaz en la que se capturarán los paquetes\n\
              entrantes\n\
\n\
outbound      Asume que los paquetes son desde la LAN hacia internet (por \n\
              defecto). Solo es util para el analizador\n\
\n\
inbound       Asume que los paquetes son desde Internet hacia la LAN. Solo\n\
              es útl para el analizador.\n\
\n\
(c) Netcop. 2016\
\n", PROGRAM, PROGRAM);
}

/*
 * argumentos()
 * ---------------------------------------------------------------------------
 *  Maneja los argumentos pasados por parametro al programa. Devuelve cero en
 *  caso de exito, cualquier otro numero en caso de error.  Sin argumentos, no
 *  especifica ninguna interfaz
 */
static int argumentos(int argc, const char* argv[], struct config *cfg) {
    *(cfg->device) = '\0';
    cfg->direccion = OUTBOUND;
    /* mas de un argumento, el primero especifica la interfaz, el resto lo
     * ignoro
     */
    if (argc > 1) {
        /* si el primer parametro es -h o --help muestro mensaje de ayuda*/
        if(strcmp(argv[1], "-h") == 0|| strcmp(argv[1], "--help") == 0) {
            ayuda();
            exit(EXIT_SUCCESS);
        }
        /* si el primer parametro es -v o --verson muestro version */
        if(strcmp(argv[1], "-v") == 0|| strcmp(argv[1], "--version") == 0) {
            printf("%s - %s - %s\n", PROGRAM, REVISION, BUILD_MODE);
            exit(EXIT_SUCCESS);
        }
        /* seteo la interfaz a escuchar */
        strncpy(cfg->device, argv[1], DEVICE_LENGTH);
    }
    /* seteo la direccion de los paquetes */
    if(argc == 3) {
        if (strcmp(argv[2], "inbound") == 0) {
            cfg->direccion = INBOUND;
        } else if (strcmp(argv[2], "outbound") == 0) {
            cfg->direccion = OUTBOUND;
        } else {
            fprintf(stderr, "%s: Parámetro desconocido\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

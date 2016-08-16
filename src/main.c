#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>

#include "adquisidor.h"
#include "db.h"
#include "ini.h"

#ifndef REVISION
#define REVISION "DESCONOCIDA"
#endif /* REVISION */

#ifndef PROGRAM
#define PROGRAM "adquisidor"
#endif /* PROGRAM */

#ifndef COPYLEFT
#define COPYLEFT "(c) Netcop 2016 - Universidad Nacional de la Matanza"
#endif /* PROGRAM */

#ifdef DEBUG
#define BUILD_MODE "desarrollo"
#else
#define BUILD_MODE "produccion"
#endif /* BUILD_MODE */

#define ARGV_LENGHT 16 /* tamaño maximo de cadenas pasadas por parametro */

typedef struct {
    char outside[DEVICE_LENGTH];
    char inside[DEVICE_LENGTH];
} ini_config;

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
static void banner(struct config*);

/*
 * argumentos()
 * ---------------------------------------------------------------------------
 *  Maneja los argumentos pasados por parametro al programa. Devuelve cero en
 *  caso de exito, cualquier otro numero en caso de error
 * 
 */
static void argumentos(int argc, const char* argv[], struct config *cfg);

/*
 * configuracion_ini
 * ---------------------------------------------------------------------------
 *  Lee la configuracion del modulo desde el archivo pasado por parametro.
 *
 *  Además crea dos procesos, uno para escuchar en la interfaz inside y otro
 *  para escuchar en la interfaz outside. El proceso padre escuchara en outside
 *  y el hijo en inside
 */
static void configuracion_ini(const char*, struct config *cfg);

int main(int argc, const char* argv[]) {
    struct config cfg;
    /* Inicializo logs */
    openlog(PROGRAM, LOG_CONS | LOG_PID, LOG_LOCAL0);
    /* Parseo argumentos para obtener la configuracion del modulo */
    argumentos(argc, argv, &cfg);
    /* Imprimo banner con version */
    banner(&cfg);
    /* muestro informacion del build */
    syslog(LOG_INFO, "Revision: %s (%s) interfaz=%s direccion=%s", 
           REVISION, BUILD_MODE, 
           *(cfg.device) ? cfg.device : "DEFAULT" ,
           cfg.direccion == ENTRANTE ? "entrante(0)" : "saliente(1)");
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
static void banner(struct config *cfg) {
    printf("---------------------------------------------------------------\n"
           "%s\n"
           "%s - Revision: %s (%s)\n"
           "Interfaz: %s Direccion: %s\n"
           "---------------------------------------------------------------\n",
           COPYLEFT,
           PROGRAM, REVISION, BUILD_MODE,
           *(cfg->device) ? cfg->device : "DEFAULT" ,
           cfg->direccion == ENTRANTE ? "entrante(0)" : "saliente(1)");
}

/*
 * ayuda()
 * --------------------------------------------------------------------------
 *  Muestra mensaje de ayuda
 */
static void ayuda() {
    printf("Uso: %s -h | -v | --config filename.ini\n"
           "     %s interfaz [saliente|entrante]\n\n"
           "Este programa captura los paquetes ENTRANTES de la interfaz pasada "
           "por parámetro.\nSi ninguna interfaz es establecida, se capturará "
           "en la primer interfaz disponible.\n"
           "\n"
           "-h, --help             Muestra esta ayuda.\n"
           "-v, --version          Muestra numero de version.\n"
           "--config filename.ini  Lee configuración desde archivo INI.\n"
           "interfaz               Nombre de la interfaz en la que se"
                                   " capturarán los paquetes entrantes.\n"
           "saliente               Asume que los paquetes son desde la LAN"
                                   " hacia internet. Solo es util para el"
                                   " analizador\n"
           "entrante               Asume que los paquetes son desde Internet"
                                   " hacia la LAN. Solo es útl para el"
                                   " analizador.\n"
           "%s\n"
           , PROGRAM, PROGRAM, COPYLEFT);
}

/*
 * argumentos()
 * ---------------------------------------------------------------------------
 *  Maneja los argumentos pasados por parametro al programa. Devuelve cero en
 *  caso de exito, cualquier otro numero en caso de error.  Sin argumentos, no
 *  especifica ninguna interfaz
 */
static void argumentos(int argc, const char* argv[], struct config *cfg) {
    /* establezco valores por defecto */
    *(cfg->device) = '\0';
    cfg->direccion = ENTRANTE;
    if (argc > 1) {
        /* si el primer parametro es -h o --help muestro mensaje de ayuda*/
        if(strncmp(argv[1], "-h", ARGV_LENGHT) == 0 || 
                strncmp(argv[1], "--help", ARGV_LENGHT) == 0) {
            ayuda();
            exit(EXIT_SUCCESS);
        }

        /* si el primer parametro es -v o --verson muestro version */
        if(strncmp(argv[1], "-v", ARGV_LENGHT) == 0 ||
                strncmp(argv[1], "--version", ARGV_LENGHT) == 0) {
            printf("%s - %s - %s\n", PROGRAM, REVISION, BUILD_MODE);
            exit(EXIT_SUCCESS);
        }

        /* si el primer parametro es --config leo configuracion desde archivo*/
        if(strncmp(argv[1], "--config", ARGV_LENGHT) == 0) {
            if (argc < 3) { /* falta nombre de archivo */
                fprintf(stderr, "Falta nombre de archivo\n"); 
                exit(EXIT_FAILURE);
            } /* fin mensaje error */
            return configuracion_ini(argv[2], cfg);
        }

        /* seteo la interfaz a escuchar */
        strncpy(cfg->device, argv[1], DEVICE_LENGTH);

        /* seteo la direccion de los paquetes */
        if(argc == 3) {
            if (strncmp(argv[2], "entrante", ARGV_LENGHT) == 0) {
                cfg->direccion = ENTRANTE;
            } else if (strncmp(argv[2], "saliente", ARGV_LENGHT) == 0) {
                cfg->direccion = SALIENTE;
            } else {
                fprintf(stderr, "%s: Parámetro desconocido\n", argv[2]);
                exit(EXIT_FAILURE);
            }
        }
    }

}

/*
 * handler()
 * --------------------------------------------------------------------------
 *  Esta funcion es llamada cada vez que se reconoce un patron clave=valor en
 *  el archivo INI.
 *
 *  Se encarga de armar la estructura de configuracion del modulo.
 */

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    ini_config *pconfig = (ini_config*) user;

    #define MATCH(s, n) strncmp(section, s, DEVICE_LENGTH) == 0 && \
                        strncmp(name, n, DEVICE_LENGTH) == 0
    if (MATCH("netcop", "outside")) {
        strncpy(pconfig->outside, value, DEVICE_LENGTH);
    } else if (MATCH("netcop", "inside")) {
        strncpy(pconfig->inside, value, DEVICE_LENGTH);
    }
    return 1;
}

/*
 * configuracion_ini
 * ---------------------------------------------------------------------------
 *  Lee la configuracion del modulo desde el archivo pasado por parametro.
 *
 *  Además crea dos procesos, uno para escuchar en la interfaz inside y otro
 *  para escuchar en la interfaz outside. El proceso padre escuchara en outside
 *  y el hijo en inside
 */
static void configuracion_ini(const char *filename, struct config *cfg) {
    ini_config ini_config;
    pid_t pid;

    if (ini_parse(filename, handler, &ini_config) < 0) {
        fprintf(stderr, "No se pudo abrir el archivo '%s'\n", filename);
        syslog(LOG_ERR, "No se pudo abrir el archivo '%s'", filename);
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Configuración cargada desde archivo '%s': "
                      "outside=%s inside=%s", 
            filename, ini_config.inside, ini_config.outside);
    
    pid = fork();
    if (pid == 0) {
        /* si es el proceso hijo, escuchara en la interfaz inside */
        strncpy(cfg->device, ini_config.inside, DEVICE_LENGTH);
        cfg->direccion = SALIENTE;
    } /* fin proceso hijo */

    else {
        /* si es el proceso padre, escuchara en la interfaz outside */
        strncpy(cfg->device, ini_config.outside, DEVICE_LENGTH);
        cfg->direccion = ENTRANTE;
    } /* fin proceso padre */
}

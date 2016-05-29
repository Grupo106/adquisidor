#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>

#include "adquisidor.h"
#include "db.h"

#ifndef REVISION
#define REVISION "DESCONOCIDA"
#endif /* REVISION */

#ifndef PROGRAM
#define PROGRAM "adquisidor"
#endif /* REVISION */

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
void manejar_interrupciones();

/**
* banner()
* ---------------------------------------------------------------------------
* Imprime banner con version del programa
*/
void banner();

int main() {
    /* Imprimo banner con version */
    banner();
    /* Inicializo logs */
    openlog(PROGRAM, LOG_CONS | LOG_PID, LOG_LOCAL0);
    /* conecto base de datos */
    int sqlret = bd_conectar();
    if(sqlret != 0) return(sqlret);
    /*
    * registro señales necesarias para cerrar correctamente el programa y
    * para liberar recursos
    */
    manejar_interrupciones();
    /* inicio captura de paquete de red */
    syslog(LOG_INFO, "Iniciando captura");
    captura_inicio();
    return EXIT_SUCCESS;
}

/**
* terminar()
* ---------------------------------------------------------------------------
* Cierra conexion de base de datos y termina la captura de paquetes
*/
static void terminar(int signum) {
    fprintf(stderr, "Interrupción recibida %d\n", signum);
    bd_desconectar();
    captura_fin();
    //closelog();
}

/**
* manejar_interrupciones()
* ---------------------------------------------------------------------------
* Registra señales que serán enviadas por el sistema operativo para el correcto
* cierre de base de datos y de interfaz de red.
*/
void manejar_interrupciones() {
    signal(SIGINT, terminar);
    signal(SIGTERM, terminar);
    signal(SIGQUIT, terminar);
}

/**
* banner()
* ---------------------------------------------------------------------------
* Imprime banner con version del programa
*/
void banner() {
    printf("\
---------------------------------------------------------------------------\n\
Grupo 106 - Universidad Nacional de La Matanza. 2016                       \n\
%s - Revision: %s                                                          \n\
---------------------------------------------------------------------------\n",
     PROGRAM, REVISION);
}

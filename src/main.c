#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "adquisidor.h"
#include "db.h"

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

int main() {
    /* conecto base de datos */
    int sqlret = bd_conectar();
    if(sqlret != 0) return(sqlret);
    /*
    * registro señales necesarias para cerrar correctamente el programa y
    * para liberar recursos
    */
    manejar_interrupciones();
    /* inicio captura de paquete de red */
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

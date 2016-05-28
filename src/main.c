#include <stdlib.h>

#include "adquisidor.h"
#include "db.h"

/**
* manejar_senales()
* ---------------------------------------------------------------------------
* Registra señales que serán enviadas por el sistema operativo para el correcto
* cierre de base de datos y de interfaz de red.
*/
void manejar_senales() {

}

int main() {
    /* conecto base de datos */
    int sqlret = bd_conectar();
    if(sqlret != 0) return(sqlret);
    manejar_senales();
    /* inicio captura de paquete de red */
    capturar();
    return 0;
}

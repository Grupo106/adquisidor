#ifndef PAQUETE_H
#define PAQUETE_H

#include <arpa/inet.h>

enum dir {
    INBOUND = 0,
    OUTBOUND = 1
};

/**
 * struct paquete
 * _________________________________________________________________________
 * Estructura del paquete de red. Esto es lo que se guardará en la base de
 * datos
 */
struct paquete {
    struct in_addr origen; /* direccion ip de origen */
    struct in_addr destino;  /* direccion ip de destino */
    u_int16_t puerto_origen; /* puerto de origen */
    u_int16_t puerto_destino; /* puerto de destino */
    int bytes; /* cantidad de bytes que contiene el paquete */
    int protocolo; /* protocolo (6 es TCP y 17 es UDP) */
    enum dir direccion; /* direccion del paquete (puede ser INBOUND o
                         * OUTBOUND)
                         */
};

#endif /* PAQUETE_H */

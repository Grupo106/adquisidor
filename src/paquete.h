#ifndef PAQUETE_H
#define PAQUETE_H

#include <arpa/inet.h>

enum direccion {
    INBOUND = 0, /* Paquete desde Internet a la LAN */
    OUTBOUND = 1, /* Paquete desde la LAN hacia Internet */
};

/**
 * t_paquete
 * _________________________________________________________________________
 * Estructura del paquete de red. Esto es lo que se guardará en la base de
 * datos
 */
struct paquete {
    struct in_addr src; /* direccion ip de origen */
    struct in_addr dst;  /* direccion ip de destino */
    u_int16_t sport; /* puerto de origen */
    u_int16_t dport; /* puerto de destino */
    int bytes; /* cantidad de bytes que contiene el paquete */
    int protocol; /* protocolo (6 es TCP y 17 es UDP) */
    enum direccion direction; /* direccion del paquete (puede ser INBOUND o 
                               * OUTBOUND) 
                               */
};

#endif /* PAQUETE_H */

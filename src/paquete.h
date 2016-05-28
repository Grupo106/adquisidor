#ifndef PAQUETE_H
#define PAQUETE_H

#include <arpa/inet.h>

#define INBOUND 0
#define OUTBOUND 1

/**
 * t_paquete
 * _________________________________________________________________________
 * Estructura del paquete de red. Esto es lo que se guardará en la base de
 * datos
 */
typedef struct {
    struct in_addr src; /* direccion ip de origen */
    u_int16_t sport; /* puerto de origen */
    struct in_addr dst;  /* direccion ip de destino */
    u_int16_t dport; /* puerto de destino */
    int bytes; /* cantidad de bytes que contiene el paquete */
    int direction; /* direccion del paquete (puede ser INBOUND o OUTBOUND) */
    int protocol; /* protocolo (6 es TCP y 17 es UDP) */
} t_paquete;

#endif /* PAQUETE_H */

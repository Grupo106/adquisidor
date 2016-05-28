#ifndef PAQUETE_H
#define PAQUETE_H

#include <sys/socket.h>
#include <netinet/in.h>
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
    struct in_addr src;
    u_int16_t sport;
    struct in_addr dst;
    u_int16_t dport;
    int bytes;
    int direction;
    int protocol;
} t_paquete;

#endif /* PAQUETE_H */

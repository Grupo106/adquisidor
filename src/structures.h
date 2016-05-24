#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define INBOUND 0
#define OUTBOUND 1

typedef struct {
    struct in_addr src;
    int src_port;
    struct in_addr dst;
    int dst_port;
    int bytes;
    int direction;
} t_paquete;

#endif /* STRUCTURES_H */

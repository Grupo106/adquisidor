#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "db.h"
#include "adquisidor.h"

void main() {
    int cantidad = 1800, i;
    struct paquete paquete;
    paquete.src.s_addr = htonl(83886339);
    paquete.dst.s_addr = htonl(83888127);
    paquete.sport = 138;
    paquete.dport = 138;
    paquete.protocol = 17;
    paquete.bytes = 261;
    bd_conectar();
    while(cantidad--) bd_insertar(&paquete); 
    bd_commit();
    bd_desconectar();
}

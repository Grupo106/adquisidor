#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "db.h"

int main()
{
    t_paquete paquete;
    int i = 2600;
    /* Conecto base de datos */
    int sqlret = conectar();
    if(sqlret != 0)
        return(sqlret);
    /* Creo paquete mock, esto me lo deberia dar el libpcap */
    inet_aton("192.168.121.15", &paquete.src);
    inet_aton("192.168.121.177", &paquete.dst);
    paquete.sport = 80;
    paquete.dport = 12345;
    paquete.bytes = 50;
    /* Inserto el paquete en la base de datos */
    while(i--) insertar(&paquete);
    commit();
    /* Muestro lista de paquetes */
    listar();
    /* Desconecto base de datos */
    desconectar();
    return 0;
}

#ifndef DB_H
#define DB_H

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

/**
 * conectar()
 * -------------------------------------------------------------------------
 * Conecta con la base de datos
 */
int conectar();

/**
 * desconectar()
 * -------------------------------------------------------------------------
 * Desconecta con la base de datos
 */
void desconectar();

/**
 * insertar(t_paquete*)
 * -------------------------------------------------------------------------
 * Inserta un paquete en el almacen de datos.
 * Luego de insertar los paquetes es necesario hacer un commit()
 */
void insertar(t_paquete *paquete);

/**
 * commit()
 * -------------------------------------------------------------------------
 * Hace un commit de la transaccion en la base de datos
 */
void commit();

/**
 * listar()
 * -------------------------------------------------------------------------
 * Lista los ultimos 10 registros en el almacen de datos
 */
void listar();

#endif /* DB_H */

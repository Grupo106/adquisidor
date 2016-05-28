#ifndef DB_H
#define DB_H

#include "paquete.h"

/**
 * bd_conectar()
 * -------------------------------------------------------------------------
 * Conecta con la base de datos
 */
int bd_conectar();

/**
 * bd_desconectar()
 * -------------------------------------------------------------------------
 * Desconecta con la base de datos
 */
void bd_desconectar();

/**
 * bd_insertar(t_paquete*)
 * -------------------------------------------------------------------------
 * Inserta un paquete en el almacen de datos.
 * Luego de insertar los paquetes es necesario hacer un commit()
 */
void bd_insertar(t_paquete *paquete);

/**
 * bd_commit()
 * -------------------------------------------------------------------------
 * Hace un commit de la transaccion en la base de datos
 */
void bd_commit();

#endif /* DB_H */

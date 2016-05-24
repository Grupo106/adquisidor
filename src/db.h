#ifndef DB_H
#define DB_H

#include "structures.h"
int conectar();
void desconectar();
void insertar(t_paquete *paquete);
void listar();
void commit();
#endif /* DB_H */

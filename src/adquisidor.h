#ifndef ADQUISIDOR_H
#define ADQUISIDOR_H

#include <pcap.h>
#include "paquete.h"

#define DEVICE_LENGTH 16 /* tamaño maximo del nombre de la interfaz */

/*
 * struct config
 * -------------------------------------------------------------------------
 *  Sirve para almacenar la configuracion del programa
 */
struct config {
    char device[DEVICE_LENGTH];
    enum dir direccion;
};

/**
 * captura_inicio()
 * --------------------------------------------------------------------------
 * Captura, procesa y guarda información de los paquetes que atraviesan una
 * interfaz en la base de datos.
 */
void captura_inicio();

/**
 * captura_fin()
 * --------------------------------------------------------------------------
 * Termina captura de paquetes y libera recursos
 */
void captura_fin();

/**
 * procesar_tcp
 * ---------------------------------------------------------------------------
 * Procesa un segmento TCP. Agrega informacion de la capa de transporte a la
 * estructura t_paquete
 */
void procesar_tcp(const u_char*, struct paquete*);

/*
 * procesar_udp
 * ---------------------------------------------------------------------------
 * Procesa un datagrama UDP. Agrega informacion de la capa de transporte a la
 * estructura t_paquete
 */
void procesar_udp(const u_char*, struct paquete*);

/*
 * procesar_paquete
 * ---------------------------------------------------------------------------
 * Procesa un paquete IP. Ignora trama Ethernet
 */
void procesar_paquete(u_char *args,
                      const struct pcap_pkthdr *header,
                      const u_char *packet);

#endif /* ADQUISIDOR_H */

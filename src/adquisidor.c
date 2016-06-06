#include <pcap.h>
#include <stdlib.h>
#include <syslog.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "adquisidor.h"
#include "db.h"
#include "paquete.h"

#define FILTER "tcp or udp"

/**
* UNUSED(x)
* --------------------------------------------------------------------------
* Macro que sirve para ignorar la advertencia por parametro no utilizado
*/
#define UNUSED(x) (void)(x)
pcap_t *__handle;
struct bpf_program __fp;

/**
* procesar_tcp
* ---------------------------------------------------------------------------
* Procesa un segmento TCP. Agrega informacion de la capa de transporte a la
* estructura t_paquete
*/
void procesar_tcp(const u_char *packet, t_paquete*);

/*
* procesar_udp
* ---------------------------------------------------------------------------
* Procesa un datagrama UDP. Agrega informacion de la capa de transporte a la
* estructura t_paquete
*/
void procesar_udp(const u_char *packet, t_paquete*);

/*
* procesar_paquete
* ---------------------------------------------------------------------------
* Procesa un paquete IP. Ignora trama Ethernet
*/
void procesar_paquete(u_char *args,
                      const struct pcap_pkthdr *header,
                      const u_char *packet)
{
    struct ip *p_ip = NULL;
    t_paquete paquete;
    int header_size;

    /* marco parametros no utilizado */
    UNUSED(args); 
    UNUSED(header);

    /* Obtengo la cabecera IP. Se le suma ETH_HLEN (14 bytes) que representan
     * la cabecera Ethernet, que en este caso la vamos a ignorar.
     */
    p_ip = (struct ip*) (packet + ETH_HLEN);

    /* Calculo el tamaño en bytes de la cabecera IP. Dicho tamaño lo da la
     * cabecera IP en el campo Header Lenght como cantidad de palabras de
     * 4 bytes. Es por ello que se multiplica por 4 para obtener la cantidad
     * de bytes a desplazar.
     */
    header_size = p_ip->ip_hl * 4;

    /* Si la cabecera es menor a 20 bytes el paquete está malformado (puede ser
     * por un error de transmision). El paquete es descartado. 
     */
    if (header_size < 20) {
        syslog(LOG_WARNING, "Paquete malformado cabecera IP menor a 20 bytes");
        return;
    }

    /* Obtengo direccion ip de origen y destino y la cantidad total del paquete
     * ip.
     */
    paquete.src = p_ip->ip_src;
    paquete.dst = p_ip->ip_dst;
    paquete.bytes = ntohs(p_ip->ip_len);

    /* Obtengo informacion de la capa de transporte */
    if (p_ip->ip_p == IPPROTO_TCP) {
        procesar_tcp(packet + ETH_HLEN + header_size, &paquete);
    } /* Si el paquete es TCP */
    else if (p_ip->ip_p == IPPROTO_UDP) {
        procesar_udp(packet + ETH_HLEN + header_size, &paquete);
    } /* Si el paquete es UDP */

    /* inserto paquete en la base de datos */
    bd_insertar(&paquete);
    // XXX: Por ahora lo pongo aca
    bd_commit();
}

/**
* procesar_tcp
* ---------------------------------------------------------------------------
* Procesa un segmento TCP
*/
void procesar_tcp(const u_char *tcp, t_paquete *paquete) {
    struct tcphdr *p_tcp = (struct tcphdr*) tcp; /* Cabecera TCP */
    /* obtengo puerto de origen */
    paquete->sport = ntohs(p_tcp->source);
    /* obtengo puerto de destino */
    paquete->dport = ntohs(p_tcp->dest);
    /* establezco que el protocolo es TCP */
    paquete->protocol = IPPROTO_TCP;
}

/*
* procesar_udp
* ---------------------------------------------------------------------------
* Procesa un datagrama UDP
*/
void procesar_udp(const u_char *udp, t_paquete *paquete) {
    struct udphdr *p_udp = (struct udphdr*) udp; /* Cabecera UDP */
    /* obtengo puerto de origen */
    paquete->sport = ntohs(p_udp->source);
    /* obtengo puerto de destino */
    paquete->dport = ntohs(p_udp->dest);
    /* establezco que el protocolo es UDP */
    paquete->protocol = IPPROTO_UDP;
}

/**
* captura_inicio()
* --------------------------------------------------------------------------
* Captura, procesa y guarda información de los paquetes que atraviesan una
* interfaz en la base de datos.
*/
void captura_inicio() {
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];

    /* Inicializar captura de paquetes */
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        fprintf(stderr, "No se puede encontrar la interfaz: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Device: %s\n", dev);

    __handle = pcap_open_live(dev, BUFSIZ, 1, 100, errbuf);
    if (__handle == NULL) {
        syslog(LOG_CRIT, "No se puede abrir la interfaz: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }
    if (pcap_datalink(__handle) != DLT_EN10MB) {
        syslog(LOG_CRIT, "La interfaz %s no provee cabeceras Ethernet", dev);
        exit(EXIT_FAILURE);
    }
    if (pcap_compile(__handle, &__fp, FILTER, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        syslog(LOG_CRIT, "No se puede parsear el filtro '%s': %s\n", FILTER,
               pcap_geterr(__handle));
        exit(EXIT_FAILURE);
    }
    if (pcap_setfilter(__handle, &__fp) == -1) {
        syslog(LOG_CRIT, "No se puede instalar el filtro '%s': %s\n", FILTER,
               pcap_geterr(__handle));
        exit(EXIT_FAILURE);
    }
    /* Capturando paquetes */
    syslog(LOG_INFO, "Iniciando captura de paquetes con filtro: %s\n", FILTER);
    pcap_loop(__handle, 0, procesar_paquete, NULL);
}

/**
* captura_fin()
* --------------------------------------------------------------------------
* Termina captura de paquetes y libera recursos
*/
void captura_fin() {
    pcap_freecode(&__fp);
    pcap_close(__handle);
    syslog(LOG_INFO, "Captura terminada");
}

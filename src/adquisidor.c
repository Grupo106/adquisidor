#include <pcap.h>
#include <stdlib.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "adquisidor.h"
#include "db.h"
#include "paquete.h"

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
    UNUSED(args); /* marco args como parametro no utilizado */
    UNUSED(header); /* marco header como parametro no utilizado */
    struct ip *p_ip = NULL;
    t_paquete paquete;
    int header_size;

    /* Obtengo la cabecera IP. Se le suma ETH_HLEN (14 bytes) que representan
    * la cabecera Ethernet, que en este caso la vamos a ignorar.
    */
    p_ip = (struct ip*) (packet + ETH_HLEN);

    /* Calculo el tamaño en bytes de la cabecera IP. Dicho tamaño lo da la
    * cabecera IP en el campo Header Lenght como cantidad de palabras de
    * 4 bytes. Es por ello que se multiplica por 4 para obtener la cantidad
    * de bytes a desplazar
    */
    header_size = p_ip->ip_hl * 4;

    /*
    * obtengo direccion ip de origen y destino y la cantidad total del paquete
    * ip
    */
    paquete.src = p_ip->ip_src;
    paquete.dst = p_ip->ip_dst;
    paquete.bytes = ntohs(p_ip->ip_len);

    /*
    * obtengo informacion de la capa de transporte
    */
    if (p_ip->ip_p == SOL_TCP) {
        procesar_tcp(packet + ETH_HLEN + header_size, &paquete);
    } /* Si el paquete es TCP */
    else if (p_ip->ip_p == SOL_UDP) {
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
    paquete->protocol = SOL_TCP;
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
    paquete->protocol = SOL_UDP;
}

void captura_inicio() {
	char *dev, errbuf[PCAP_ERRBUF_SIZE];
    char filter_exp[] = "udp or tcp";
    bpf_u_int32 mask;
    bpf_u_int32 net;

	dev = pcap_lookupdev(errbuf);
	if (dev == NULL) {
		fprintf(stderr, "No se puede encontrar la interfaz: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}
	printf("Device: %s\n", dev);
    if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "No se puede obtener la máscara de subred de la\
        interfaz %s\n", dev);
    }
	__handle = pcap_open_live(dev, BUFSIZ, 1, 100, errbuf);

	if (__handle == NULL) {
        fprintf(stderr, "No se puede abrir la interfaz %s: %s\n", dev,
                errbuf);
        exit(EXIT_FAILURE);
    }
    if (pcap_datalink(__handle) != DLT_EN10MB) {
        fprintf(stderr, "La interfaz %s no provee cabeceras Ethernet", dev);
        exit(EXIT_FAILURE);
    }
    if (pcap_compile(__handle, &__fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "No se puede parsear el filtro '%s': %s\n",
                filter_exp, pcap_geterr(__handle));
        exit(EXIT_FAILURE);
    }
    if (pcap_setfilter(__handle, &__fp) == -1) {
        fprintf(stderr, "No se puede instalar el filtro '%s': %s\n",
                filter_exp, pcap_geterr(__handle));
        exit(EXIT_FAILURE);
    }
    /* Capturando paquetes */
    printf("Esperando paquetes con filtro: %s\n", filter_exp);
    pcap_loop(__handle, 0, procesar_paquete, NULL);
}

void captura_fin() {
    pcap_freecode(&__fp);
    pcap_close(__handle);
    printf("\nCaptura completa\n");
}

#include <pcap.h>
#include <stdlib.h>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "adquisidor.h"
#include "db.h"
#include "paquete.h"

pcap_t *__handle;
struct bpf_program __fp;

void procesar_paquete(u_char *args,
                      const struct pcap_pkthdr *header,
                      const u_char *packet)
{
    struct ip *p_ip = NULL;
    struct udphdr *p_udp = NULL;
    struct tcphdr *p_tcp = NULL;
    int ip_size; /* Tamaño de la cabecera ip (es variable) */
    t_paquete paquete;

    /* Obtengo la cabecera IP. Se le suma ETH_HLEN (14 bytes) que representan
     * la cabecera Ethernet, que en este caso la vamos a ignorar.
     */
    p_ip = (struct ip*) (packet + ETH_HLEN);

    /* Calculo el tamaño en bytes de la cabecera IP. Dicho tamaño lo da la
     * cabecera IP en el campo Header Lenght como cantidad de palabras de
     * 4 bytes. Es por ello que se multiplica por 4 para obtener la cantidad
     * de bytes a desplazar
     */
    ip_size = p_ip->ip_hl * 4;

    paquete.src = p_ip->ip_src;
    paquete.dst = p_ip->ip_dst;
    paquete.bytes = ntohs(p_ip->ip_len);

    /* Si el paquete es TCP */
    if (p_ip->ip_p == SOL_TCP) {
      /* Obtengo cabecera TCP */
      p_tcp = (struct tcphdr*) (packet + ETH_HLEN + ip_size);
      paquete.sport = ntohs(p_tcp->source);
      paquete.dport = ntohs(p_tcp->dest);
      paquete.protocol = SOL_TCP;
    }

    /* Si el paquete es UDP */
    else if (p_ip->ip_p == SOL_UDP) {
      /* Obtengo cabecera UDP */
      p_udp = (struct udphdr*) (packet + ETH_HLEN + ip_size);
      paquete.sport = ntohs(p_udp->source);
      paquete.dport = ntohs(p_udp->dest);
      paquete.protocol = SOL_UDP;
    }

    bd_insertar(&paquete);
    // XXX: Por ahora lo pongo aca
    bd_commit();
}

void capturar() {
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

void terminar_captura() {
    pcap_freecode(&__fp);
    pcap_close(__handle);
    printf("\nCaptura completa\n");
}

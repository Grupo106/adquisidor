#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include "db.h"

void callback(u_char*, const struct pcap_pkthdr*, const u_char*);

void callback(u_char *args, const struct pcap_pkthdr *header, 
              const u_char *packet) {
    struct ip *ipc = NULL;
    struct udphdr *p_udp = NULL;
    struct tcphdr *p_tcp = NULL;
    int ip_size; /* Tamaño de la cabecera ip (es variable) */
    t_paquete paquete;

    /* Obtengo la cabecera IP. Se le suma ETH_HLEN (14 bytes) que representan
     * la cabecera Ethernet, que en este caso la vamos a ignorar.
     */
    ipc = (struct ip*) (packet + ETH_HLEN);

    /* Calculo el tamaño en bytes de la cabecera IP. Dicho tamaño lo da la 
     * cabecera IP en el campo Header Lenght como cantidad de palabras de 
     * 4 bytes. Es por ello que se multiplica por 4 para obtener la cantidad
     * de bytes a desplazar
     */
    ip_size = ipc->ip_hl * 4; 

    paquete.src = ipc->ip_src;
    paquete.dst = ipc->ip_dst;
    paquete.bytes = ntohs(ipc->ip_len);

    /* Si el paquete es TCP */
    if (ipc->ip_p == SOL_TCP) {
      /* Obtengo cabecera TCP */
      p_tcp = (struct tcphdr*) (packet + ETH_HLEN + ip_size);
      paquete.sport = ntohs(p_tcp->source);
      paquete.dport = ntohs(p_tcp->dest);
      paquete.protocol = SOL_TCP;
    }

    /* Si el paquete es UDP */
    else if (ipc->ip_p == SOL_UDP) {
      /* Obtengo cabecera UDP */
      p_udp = (struct udphdr*) (packet + ETH_HLEN + ip_size);
      paquete.sport = ntohs(p_udp->source);
      paquete.dport = ntohs(p_udp->dest);
      paquete.protocol = SOL_UDP;
    }
    else {
        return;
    }
    char src[15], dst[15];
    strcpy(src, inet_ntoa(paquete.src));
    strcpy(dst, inet_ntoa(paquete.dst));
    printf("paquete src:%s, dst:%s, sport:%d, dport:%d, bytes:%d, protocol:%d\n",
           src,
           dst,
           paquete.sport,
           paquete.dport,
           paquete.bytes,
           paquete.protocol);
}

int main()
{
	char *dev, errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;
    struct bpf_program fp;
    char filter_exp[] = "udp or tcp";
    bpf_u_int32 mask;
    bpf_u_int32 net;

	dev = pcap_lookupdev(errbuf);
	if (dev == NULL) {
		fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
		exit(2);
	}
	printf("Device: %s\n", dev);
    if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "No se puede obtener la máscara de subred de la\
        interfaz %s\n", dev);
    }
	handle = pcap_open_live(dev, BUFSIZ, 1, 100, errbuf);

	if (handle == NULL) {
        fprintf(stderr, "No se puede abrir la interfaz %s: %s\n", dev,
                errbuf);
        exit(2);
    }
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "La interfaz %s no provee cabeceras Ethernet", dev);
        exit(2);
    }
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "No se puede parsear el filtro '%s': %s\n",
                filter_exp, pcap_geterr(handle));
        exit(2);
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "No se puede instalar el filtro '%s': %s\n",
                filter_exp, pcap_geterr(handle));
        exit(2);
    }
    /* Capturando paquetes */
    printf("Esperando paquetes con filtro: %s\n", filter_exp);
    pcap_loop(handle, 0, callback, NULL);

    /* Limpiando */
    pcap_freecode(&fp);
    pcap_close(handle);

    printf("\nCaptura completa\n");

	return(0);
}

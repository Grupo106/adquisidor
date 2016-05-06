/**
 * Ver http://www.tcpdump.org/pcap.htm
 */
#include <stdio.h>
#include <pcap.h>

void callback(u_char*, const struct pcap_pkthdr*, const u_char*);

void callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    static int count = 0;
    count++;
    printf("%d paquete recibidos\n", count);
}

int main(int argc, char *argv[])
{
	char *dev, errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;
    struct bpf_program fp;
    char filter_exp[] = "ip";
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct pcap_pkthdr header;
    const u_char *packet;

	dev = pcap_lookupdev(errbuf);
	if (dev == NULL) {
		fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
		return(2);
	}
	printf("Device: %s\n", dev);
    if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "No se puede obtener la máscara de subred de la\
        interfaz %s\n", dev);
    }
	handle = pcap_open_live(dev, BUFSIZ, 1, 10000, errbuf);
	if (handle == NULL) {
        fprintf(stderr, "No se puede abrir la interfaz %s: %s\n", dev,
                errbuf);
        return(2);
    }
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "La interfaz %s no provee cabeceras Ethernet", dev);
        return(2);
    }
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "No se puede parsear el filtro '%s': %s\n",
                filter_exp, pcap_geterr(handle));
        return(2);
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "No se puede instalar el filtro '%s': %s\n",
                filter_exp, pcap_geterr(handle));
        return(2);
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

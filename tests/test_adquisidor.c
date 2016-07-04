/**
 * Compilar
 * gcc -lpcap -I../src -g -Wl,--wrap=bd_insertar,--wrap=bd_commit ../src/adquisidor.c adquisidor_test.c
 */
#include <assert.h>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "../src/adquisidor.h"
#include "../src/paquete.h"

int __wrap_bd_insertar(struct paquete *paquete) {
    assert(paquete->origen.s_addr == inet_addr("192.168.10.1"));
    assert(paquete->destino.s_addr == inet_addr("200.67.222.222"));
    assert(paquete->puerto_origen == 23412);
    assert(paquete->puerto_destino == 80);
    assert(paquete->bytes == 1500);
    assert(paquete->protocolo == IPPROTO_TCP);
    return 0;
}

int __wrap_bd_commit() {
    return 0;
}

static void test_paquete_tcp() {
    char packet[255];
    struct ip *capa3;
    struct tcphdr *capa4;
    capa3 = (struct ip*)(packet + ETH_HLEN);
    capa4 = (struct tcphdr*)(packet + ETH_HLEN + 20);
    capa3->ip_hl = 5;
    capa3->ip_p = IPPROTO_TCP;
    capa3->ip_src.s_addr = inet_addr("192.168.10.1");
    capa3->ip_dst.s_addr = inet_addr("200.67.222.222");
    capa3->ip_len = htons(1500);
    capa4->source = htons(23412);
    capa4->dest = htons(80);

    procesar_paquete(NULL, NULL, packet);
}

int main(void) {
    test_paquete_tcp();
    printf("SUCCESS\n");
}

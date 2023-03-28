/*----------------------------------------------------------------------------------------*/
/* T1 - Laboratório de Redes de Computadores - Professora Cristina Moreira Nunes - 2023/1 */
/* André Luiz Rodrigues, Fernanda Ferreira de Mello, Matheus Pozzer Moraes                */
/*----------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>		   //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h>	   //definicao de protocolos
#include <arpa/inet.h>	   //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#define BUFFSIZE 1518
#define MAX_SIZE_PORT_ARRAY 40

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

typedef struct {
	unsigned char firstBitPort;
	unsigned char secondBitPort;
	int count;
} PortInformation;

unsigned char buff1[BUFFSIZE]; // buffer de recepcao

int sockd;
int on;
struct ifreq ifr;

bool checkIfApplicationLayerPortIsEqualToSourcePortOrDestinationPort(unsigned char firstBitPort, unsigned char secondBitPort, bool isIpv4, bool isIpv6);
bool checkIfApplicationLayerPortsAreEqualToSourcePortAndDestinationPort(unsigned char firstBitFirstPort, unsigned char secondBitFirstPort, unsigned char firstBitSecondPort, unsigned char secondBitSecondPort, bool isIpv4, bool isIpv6);
void sortPortsByCount(PortInformation *portArray, int arraySize);
int addPortInformationToArray(PortInformation *portArray, int arraySize, bool isIpv4, bool isIpv6);
void printMostUsedPorts(PortInformation *portArray, int arraySize, int maxPortsToShow);

int main(int argc, char *argv[]) {
	if (argc == 0) {
		printf("Erro: deve ser informada a quantidade de pacotes a serem monitorados na linha de comando.");
		exit(1);
	}

	int minSize = BUFFSIZE, maxSize = 0, sumSize = 0;
	int numberOfPackets = 0;
	int countIpv4 = 0, countIpv6 = 0;
	int countArpReq = 0, countArpRep = 0;
	int countTcp = 0, countUdp = 0;
	int countIcmp = 0, countIcmpEchoReq = 0, countIcmpEchoRep = 0;
	int countIcmpv6 = 0, countIcmpv6EchoReq = 0, countIcmpv6EchoRep = 0;
	int countHttp = 0, countDns = 0, countDhcp = 0, countHttps = 0;
	int currentSizeTcpPortArray = 0, currentSizeUdpPortArray = 0;
	PortInformation portArrayTcp[MAX_SIZE_PORT_ARRAY];
	PortInformation portArrayUdp[MAX_SIZE_PORT_ARRAY];
	long totalPackets = strtol(argv[1], NULL, 10);
	printf("Serao monitorados %ld pacotes.\n", totalPackets);

	/* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
	/* De um "man" para ver os parametros.*/
	/* htons: converte um short (2-byte) integer para standard network byte order. */
	if ((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		printf("Erro na criacao do socket.\n");
		exit(1);
	}

	// O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
	strcpy(ifr.ifr_name, "eth0");
	if (ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("erro no ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

	int iterationCount = 0;
	while (iterationCount < totalPackets) {
		int sizePacket = recv(sockd, (char *)&buff1, sizeof(buff1), 0x0);

		numberOfPackets += 1;
		iterationCount += 1;
		sumSize += sizePacket;
		if (sizePacket < minSize) {
			minSize = sizePacket;
		}
		if (sizePacket > maxSize) {
			maxSize = sizePacket;
		}

		bool isIpv4 = false;
		bool isIpv6 = false;
		bool isTcp = false;
		bool isUdp = false;

		// Nivel de enlace
		if (buff1[12] == 0x8 && buff1[13] == 0x6 && buff1[20] == 0x0 && buff1[21] == 0x1) {
			countArpReq += 1;
		}
		else if (buff1[12] == 0x8 && buff1[13] == 0x6 && buff1[20] == 0x0 && buff1[21] == 0x2) {
			countArpRep += 1;
		}
		// Nivel de rede
		else if (buff1[12] == 0x8 && buff1[13] == 0x0) {
			isIpv4 = true;
			countIpv4 += 1;
		}
		else if (buff1[12] == 0x86 && buff1[13] == 0xdd) {
			isIpv6 = true;
			countIpv6 += 1;
		}

		if (isIpv4 && buff1[23] == 0x1) {
			countIcmp += 1;
			if (buff1[34] == 0x8) {
				countIcmpEchoReq += 1;
			}
			else if (buff1[34] == 0x0) {
				countIcmpEchoRep += 1;
			}
		}
		else if (isIpv6 && buff1[20] == 0x3a) {
			countIcmpv6 += 1;
			if (buff1[54] == 0x80) {
				countIcmpv6EchoReq += 1;
			}
			else if (buff1[54] == 0x81) {
				countIcmpv6EchoRep += 1;
			}
		}
		// Nivel de transporte
		else if ((isIpv4 && buff1[23] == 0x6) || (isIpv6 && buff1[20] == 0x6)) {
			countTcp += 1;
			isTcp = true;
		}
		else if ((isIpv4 && buff1[23] == 0x11) || (isIpv6 && buff1[20] == 0x11)) {
			countUdp += 1;
			isUdp = true;
		}

		// Nivel de aplicacao
		if (isTcp) {
			currentSizeTcpPortArray = addPortInformationToArray(portArrayTcp, currentSizeTcpPortArray, isIpv4, isIpv6);
		}
		else if (isUdp) {
			currentSizeUdpPortArray = addPortInformationToArray(portArrayUdp, currentSizeUdpPortArray, isIpv4, isIpv6);
		}

		if (isTcp && checkIfApplicationLayerPortIsEqualToSourcePortOrDestinationPort(0x0, 0x50, isIpv4, isIpv6)) {
			countHttp += 1; // source port ou destination port 80
		}
		else if (isUdp && checkIfApplicationLayerPortIsEqualToSourcePortOrDestinationPort(0x0, 0x35, isIpv4, isIpv6)) {
			countDns += 1; // source port ou destination port 53
		}
		else if (isUdp && checkIfApplicationLayerPortsAreEqualToSourcePortAndDestinationPort(0x0, 0x44, 0x0, 0x43, isIpv4, isIpv6)) {
			countDhcp += 1; // source port ou destination port 67 ou 68
		}
		else if (isTcp && checkIfApplicationLayerPortIsEqualToSourcePortOrDestinationPort(0x1, 0xbb, isIpv4, isIpv6)) {
			countHttps += 1; // source port ou destination port 443
		}
	}

	sortPortsByCount(portArrayTcp, currentSizeTcpPortArray);
	sortPortsByCount(portArrayUdp, currentSizeUdpPortArray);
	
	printf("----------- GERAL -----------\n");
	printf("Total de pacotes: %d\n", numberOfPackets);
	printf("Tamanho minimo: %d\n", minSize);
	printf("Tamanho maximo: %d\n", maxSize);
	printf("Tamanho medio: %d\n", (int)(sumSize / numberOfPackets));
	printf("----------- NIVEL DE ENLACE -----------\n");
	printf("Count ARP Request: %d (%.2f%%)\n", countArpReq, (countArpReq * 100.0) / numberOfPackets);
	printf("Count ARP Reply: %d (%.2f%%)\n", countArpRep, (countArpRep * 100.0) / numberOfPackets);
	printf("----------- NIVEL DE REDE -----------\n");
	printf("Count IPv4: %d (%.2f%%)\n", countIpv4, (countIpv4 * 100.0) / numberOfPackets);
	printf("Count ICMP: %d (%.2f%%)\n", countIcmp, (countIcmp * 100.0) / numberOfPackets);
	printf("Count ICMP Echo Request: %d (%.2f%%)\n", countIcmpEchoReq, (countIcmpEchoReq * 100.0) / numberOfPackets);
	printf("Count ICMP Echo Reply: %d (%.2f%%)\n", countIcmpEchoRep, (countIcmpEchoRep * 100.0) / numberOfPackets);
	printf("Count IPv6: %d (%.2f%%)\n", countIpv6, (countIpv6 * 100.0) / numberOfPackets);
	printf("Count ICMPv6: %d (%.2f%%)\n", countIcmpv6, (countIcmpv6 * 100.0) / numberOfPackets);
	printf("Count ICMPv6 Echo Request: %d (%.2f%%)\n", countIcmpv6EchoReq, (countIcmpv6EchoReq * 100.0) / numberOfPackets);
	printf("Count ICMPv6 Echo Reply: %d (%.2f%%)\n", countIcmpv6EchoRep, (countIcmpv6EchoRep * 100.0) / numberOfPackets);
	printf("----------- NIVEL DE TRANSPORTE -----------\n");
	printf("Count TCP: %d (%.2f%%)\n", countTcp, (countTcp * 100.0) / numberOfPackets);
	printf("Count UDP: %d (%.2f%%)\n", countUdp, (countUdp * 100.0) / numberOfPackets);
	printf("Numero de portas TCP diferentes acessadas: %d\n", currentSizeTcpPortArray);
	printf("5 portas TCP mais acessadas:\n");
	printMostUsedPorts(portArrayTcp, currentSizeTcpPortArray, 5);
	printf("Numero de portas UDP diferentes acessadas: %d\n", currentSizeUdpPortArray);
	printf("5 portas UDP mais acessadas:\n");
	printMostUsedPorts(portArrayUdp, currentSizeUdpPortArray, 5);
	printf("----------- NIVEL DE APLICACAO -----------\n");
	printf("Count HTTP: %d (%.2f%%)\n", countHttp, (countHttp * 100.0) / numberOfPackets);
	printf("Count DNS: %d (%.2f%%)\n", countDns, (countDns * 100.0) / numberOfPackets);
	printf("Count DHCP: %d (%.2f%%)\n", countDhcp, (countDhcp * 100.0) / numberOfPackets);
	printf("Count HTTPS: %d (%.2f%%)\n", countHttps, (countHttps * 100.0) / numberOfPackets);
	printf("-------------------------------------------\n\n");
}

void sortPortsByCount(PortInformation *portArray, int arraySize) {
	for (int i = 0; i < arraySize - 1; ++i) {
		for (int j = 0; j < arraySize - i - 1; ++j) {
			if (portArray[j].count < portArray[j + 1].count) {
				PortInformation aux = portArray[j];
				portArray[j] = portArray[j + 1];
				portArray[j + 1] = aux;
			}
		}
	}
}

int addPortInformationToArray(PortInformation *portArray, int arraySize, bool isIpv4, bool isIpv6) {
	bool sourcePortAlreadyInArray = false;
	bool destinationPortAlreadyInArray = false;
	for (int i = 0; i < arraySize; i++) {
		if ((isIpv4 && portArray[i].firstBitPort == buff1[34] && portArray[i].secondBitPort == buff1[35])
			|| (isIpv6 && portArray[i].firstBitPort == buff1[54] && portArray[i].secondBitPort == buff1[55])
		) {
			sourcePortAlreadyInArray = true;
			portArray[i].count++;
		}
		if ((isIpv4 && portArray[i].firstBitPort == buff1[36] && portArray[i].secondBitPort == buff1[37])
			|| (isIpv6 && portArray[i].firstBitPort == buff1[56] && portArray[i].secondBitPort == buff1[57])
		) {
			destinationPortAlreadyInArray = true;
			portArray[i].count++;
		}
	}
	if (!sourcePortAlreadyInArray && arraySize < MAX_SIZE_PORT_ARRAY) {
		if (isIpv4) {
			portArray[arraySize].firstBitPort = buff1[34];
			portArray[arraySize].secondBitPort = buff1[35];
		} else {
			portArray[arraySize].firstBitPort = buff1[54];
			portArray[arraySize].secondBitPort = buff1[55];
		}
		portArray[arraySize].count = 1;
		arraySize++;
	}
	if (!destinationPortAlreadyInArray && arraySize < MAX_SIZE_PORT_ARRAY) {
		if (isIpv4) {
			portArray[arraySize].firstBitPort = buff1[36];
			portArray[arraySize].secondBitPort = buff1[37];
		} else {
			portArray[arraySize].firstBitPort = buff1[56];
			portArray[arraySize].secondBitPort = buff1[57];
		}
		portArray[arraySize].count = 1;
		arraySize++;
	}
	return arraySize;
}

void printMostUsedPorts(PortInformation *portArray, int arraySize, int maxPortsToShow) {
	if (arraySize == 0) {
		printf("  Nenhuma porta acessada\n");
		return;
	}
	for(int i = 0; i < maxPortsToShow && i < arraySize; i++) {
		int portAsDecimal = (portArray[i].firstBitPort<<8)|portArray[i].secondBitPort;
		printf("  %d. Porta: %d, contagem: %d\n", (i+1), portAsDecimal, portArray[i].count);
	}
}

bool checkIfApplicationLayerPortIsEqualToSourcePortOrDestinationPort(unsigned char firstBitPort, unsigned char secondBitPort, bool isIpv4, bool isIpv6) {
	return (isIpv4 && ((buff1[34] == firstBitPort && buff1[35] == secondBitPort) || (buff1[36] == firstBitPort && buff1[37] == secondBitPort)))
		|| (isIpv6 && ((buff1[54] == firstBitPort && buff1[55] == secondBitPort) || (buff1[56] == firstBitPort && buff1[57] == secondBitPort)));
}

bool checkIfApplicationLayerPortsAreEqualToSourcePortAndDestinationPort(unsigned char firstBitFirstPort, unsigned char secondBitFirstPort, unsigned char firstBitSecondPort, unsigned char secondBitSecondPort, bool isIpv4, bool isIpv6) {
	return (isIpv4 && (
			(buff1[34] == firstBitFirstPort && buff1[35] == secondBitFirstPort && buff1[36] == firstBitSecondPort && buff1[37] == secondBitSecondPort) ||
			(buff1[34] == firstBitSecondPort && buff1[35] == secondBitSecondPort && buff1[36] == firstBitFirstPort && buff1[37] == secondBitFirstPort)
		)) || (isIpv6 && (
			(buff1[54] == firstBitFirstPort && buff1[55] == secondBitFirstPort && buff1[56] == firstBitSecondPort && buff1[57] == secondBitSecondPort)
			|| (buff1[54] == firstBitSecondPort && buff1[55] == secondBitSecondPort && buff1[56] == firstBitFirstPort && buff1[57] == secondBitFirstPort)
		));
}
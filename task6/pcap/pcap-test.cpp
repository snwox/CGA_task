
#include <pcap.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <linux/ip.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
	char *dev_;
} Param;

Param param = {
	.dev_ = NULL};

int main(int argc, char *argv[])
{

	param.dev_ = "enp1s0"; // change this

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *pcap = pcap_open_live(param.dev_, BUFSIZ, 1, 1000, errbuf);
	if (pcap == NULL)
	{
		fprintf(stderr, "pcap_open_live(%s) return null - %s\n", param.dev_, errbuf);
		return -1;
	}

	while (true)
	{
		struct pcap_pkthdr *header;
		struct ether_header *eptr;
		struct sockaddr_in paddr;
		struct iphdr *ip;
		struct tcphdr *tptr;

		const uint8_t *packet;

		int res = pcap_next_ex(pcap, &header, &packet);
		if (res == 0)
			continue;
		if (res == PCAP_ERROR || res == PCAP_ERROR_BREAK)
		{
			printf("pcap_next_ex return %d(%s)\n", res, pcap_geterr(pcap));
			break;
		}
		eptr = (struct ether_header *)packet;
		packet += sizeof(struct ether_header);

		if (ntohs(eptr->ether_type) == ETHERTYPE_IP)
		{
			ip = (struct iphdr *)packet;
			packet += sizeof(struct iphdr);
			if (ip->protocol == 6)
			{
				tptr = (struct tcphdr *)packet;

				puts("===================="); // src,dest MAC from ETHERNET header
				puts("\t[ETHERNET]");
				printf("SRC MAC : ");
				for (int i = 0; i < ETH_ALEN; i++)
				{
					printf("%02X", eptr->ether_dhost[i]);
					if (i < ETH_ALEN - 1)
					{
						printf(":");
					}
					else
					{
						printf("\n");
					}
				}
				printf("DEST MAC : ");
				for (int i = 0; i < ETH_ALEN; i++)
				{
					printf("%02X", eptr->ether_shost[i]);
					if (i < ETH_ALEN - 1)
					{
						printf(":");
					}
					else
					{
						printf("\n");
					}
				}

				puts("\t[IP]"); // src,dest ip addr from IP header
				paddr.sin_addr.s_addr = ip->saddr;
				printf("SRC IP : %s\n", inet_ntoa(paddr.sin_addr));
				paddr.sin_addr.s_addr = ip->daddr;
				printf("DEST IP : %s\n", inet_ntoa(paddr.sin_addr));

				puts("\t[TCP]"); //src,dest port from TCP header
				printf("SRC port : %d\n", ntohs(tptr->th_sport));
				printf("DEST port : %d\n", ntohs(tptr->th_dport));
			}
		}
		sleep(1);
	}

	pcap_close(pcap);
}

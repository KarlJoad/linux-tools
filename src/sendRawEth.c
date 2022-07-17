/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * Copied from austinmarton's GitHub gist.
 * https://gist.github.com/austinmarton/1922600
 *
 * Send raw ethernet packets over a given Ethernet interface.
 * Will send the specified amount as fast as possible, effectively saturating
 * the Ethernet interface to stress-test NIC drivers.
 *
 * Requires root level access on the system this test is being performed on.
 */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

typedef struct mac_addr {
  uint8_t addr[6];
}  mac_addr_t;

#define MY_DEST_MAC0	0x00
#define MY_DEST_MAC1	0x00
#define MY_DEST_MAC2	0x00
#define MY_DEST_MAC3	0x00
#define MY_DEST_MAC4	0x00
#define MY_DEST_MAC5	0x00

#define DEFAULT_IF	"eno1np0"
#define BUF_SIZ		1024

static
void USAGE() {
  printf("sendRawEth <sender_IF> <recvr_MAC> <num_Ethernet_Frames>\n");
  printf("\tsender_IF is the human-readable network interface name of the "
         "sender\n");
  printf("\trecvr_MAC is the MAC address of the receiving device\n");
  printf("\tnum_Ethernet_Frames is the number of raw Ethernet frames to submit "
         "to the driver\n");
}

#define printerr(fmt, body...) fprintf(stderr, fmt, body)

static char ifName[IF_NAMESIZE] = {};
static mac_addr_t dest_mac; // {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
static size_t num_frames = 0;

int parse_human_interface(char *restrict ifName, char *restrict name) {
  /* if (strlen(name) > IFNAMSIZ) { */

  /*   return -1; */
  /* } */
    fprintf(stderr, "%s", "Test\n");
    ifName = name;
    return 0;
}

static inline
int parse_dest_mac(mac_addr_t *restrict dest_mac, char *restrict mac) {
  char mac_byte[6] = {0};
  for (unsigned i = 0; i < 6; i++) {
      char *tok = NULL;
      tok = strtok(mac, ":");
      // We only enter this when the input MAC is malformed.
      if (tok == NULL) {
          fprintf(stderr, "%s\n", "Malformed destination MAC Address");
          return EXIT_FAILURE;
      }
      mac_byte[i] = *tok;
      tok = NULL;
      printf("MAC Byte %u: %c\n", i, mac_byte[i]);
  }

  for (unsigned i = 0; i < 6; i++) {
      dest_mac->addr[i] = atoi(mac_byte + i);
  }

  return EXIT_SUCCESS;
}

static inline
int parse_num_frames(size_t *restrict num_frames, char *restrict amount) {
    if (sscanf(amount, "%zu", num_frames) != 0) {
        fprintf(stderr, "%s", "Cannot parse number of frames to send!\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static
void parse_cli_flags(char *argv[]) {
    parse_human_interface(ifName, argv[1]);
    parse_dest_mac(&dest_mac, argv[2]);
    parse_num_frames(&num_frames, argv[3]);
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	struct sockaddr_ll socket_address;


  if (argc < 4) {
      USAGE();
      return EXIT_SUCCESS;
  }

  parse_cli_flags(argv);
	if (argc > 1)
		strcpy(ifName, argv[1]);
	else
		strcpy(ifName, DEFAULT_IF);

	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IF_NAMESIZE-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IF_NAMESIZE-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZ);
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = MY_DEST_MAC0;
	eh->ether_dhost[1] = MY_DEST_MAC1;
	eh->ether_dhost[2] = MY_DEST_MAC2;
	eh->ether_dhost[3] = MY_DEST_MAC3;
	eh->ether_dhost[4] = MY_DEST_MAC4;
	eh->ether_dhost[5] = MY_DEST_MAC5;
	/* Ethertype field */
	eh->ether_type = htons(ETH_P_IP);
	tx_len += sizeof(struct ether_header);

	/* Packet data */
	sendbuf[tx_len++] = 0xde;
	sendbuf[tx_len++] = 0xad;
	sendbuf[tx_len++] = 0xbe;
	sendbuf[tx_len++] = 0xef;

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	socket_address.sll_addr[0] = MY_DEST_MAC0;
	socket_address.sll_addr[1] = MY_DEST_MAC1;
	socket_address.sll_addr[2] = MY_DEST_MAC2;
	socket_address.sll_addr[3] = MY_DEST_MAC3;
	socket_address.sll_addr[4] = MY_DEST_MAC4;
	socket_address.sll_addr[5] = MY_DEST_MAC5;

	/* Send packet */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	    printf("Send failed\n");

	return 0;
}

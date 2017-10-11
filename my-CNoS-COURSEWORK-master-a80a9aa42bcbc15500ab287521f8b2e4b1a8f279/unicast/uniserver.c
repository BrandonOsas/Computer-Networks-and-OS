/*****************************************************
* Program name: uniserver.c (the unicast server)
*
* Module ID: UFCFQ4-30-2
* Module Name: Computer Networks and Operating Systems
*
*last update : 6 dec 2016
* Author: Brandon Egonmwan - 15024049
*
* Program Description:
* Unicast NTP server. Receives a UDP packet request
* and sends back data to the client it was sent from.
*
*****************************************************/
/*  Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
/****************************************************/
/*  Defines */
#define PORT 5262
#define DELTA (2208988800)
#define SHIFT (1LL<<32) * 1.0e-6 )
/*  Structures */
//this struct holds the second and microseconds
struct timestamp 
{
	uint32_t seconds;
	uint32_t fractions;
};
//this struct holds the SNTP packets
struct packet 
{
	uint8_t LIVNMODE;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;

	int32_t rootDelay;
	uint32_t rootDispersion;
	int32_t referenceIdentifier;

	struct timestamp referenceTimestamp;
	struct timestamp originateTimestamp;
	struct timestamp receiveTimestamp;
	struct timestamp transmitTimestamp;
};

/*********************************************************************************************
* Name					: UNIX_TO_NTP(struct timeval *UT)
* Returns       : void
*    *UT        : UNIX timestamp
* Created by    : Brandon Egonmwan
* Description   : Converts a UNIX timestamp to a NTP compatible version.
*********************************************************************************************/
void UNIX_TO_NTP(struct timeval *UT, struct timestamp *ntp)
{
	ntp->seconds = UT->tv_sec + DELTA;
	ntp->fractions = (uint32_t)( (double)(UT->tv_usec+1) * (double) SHIFT;
}


int main( void)
{
	//Variables
	int sock_et;
	//sockets
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	struct timestamp ntp;
	struct packet sendPkt;
	struct timeval UT;
	//size of the packets
	int addr_len, numbytes;
	//socket in
	if( (sock_et = socket( AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror( "Server socket");
		exit( 1);
	}
	//zero structs
	memset( &serveraddr, 0, sizeof( serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT); //ports in network byte order
	serveraddr.sin_addr.s_addr = INADDR_ANY; //any server ip addresses

	if( bind( sock_et, (struct sockaddr *)&serveraddr, sizeof( struct sockaddr)) == -1)
	{
			perror( "Server bind");
			exit(1);
	}


	while(1)
	{
		//keep checking for connections

		/*********  RECEIVING PACKET  *********/
		addr_len = sizeof(struct sockaddr);

		memset(&sendPkt, 0, 48);
		//Receive Packet
		if( (numbytes = recvfrom( sock_et, &sendPkt, 48, 0, (struct sockaddr *)&clientaddr, &addr_len)) == -1)
		{
			perror( "Server recvfrom");
			exit( 1);
		}
		//check the timestamps recieved keep it and convert it to NTP
		gettimeofday(&UT, NULL);
		UNIX_TO_NTP(&UT, &ntp);

		sendPkt.referenceTimestamp.seconds = ntohl(ntp.seconds);
		sendPkt.referenceTimestamp.fractions = ntohl(ntp.fractions);

		gettimeofday(&UT, NULL);
		UNIX_TO_NTP(&UT, &ntp);

		sendPkt.receiveTimestamp.seconds = ntohl(ntp.seconds);
		sendPkt.receiveTimestamp.fractions = ntohl(ntp.fractions);

		printf( "Received packet from: %s\n", inet_ntoa(clientaddr.sin_addr));
		printf( "Packet is %d bytes long\n", numbytes);

		gettimeofday(&UT, NULL);
		UNIX_TO_NTP(&UT, &ntp);
		/*********  SENDING PACKET  *********/
		sendPkt.LIVNMODE = 344;
		sendPkt.stratum = 2;
		sendPkt.poll = 4;
		sendPkt.precision = 0;
		sendPkt.rootDelay = 0;
		sendPkt.rootDispersion = 0;
		sendPkt.transmitTimestamp.seconds = ntohl(ntp.seconds);
		sendPkt.transmitTimestamp.fractions = ntohl(ntp.fractions);
		//send packets
		if( (numbytes = sendto( sock_et, &sendPkt, 48, 0, (struct sockaddr *)&clientaddr, sizeof( struct sockaddr))) == -1) {
			perror( "Client sendto");
			exit( 1);
		}

	}


	close(sock_et);
	return 0;
}

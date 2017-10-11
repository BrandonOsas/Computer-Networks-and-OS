/*****************************************************
* Program name: uniserver.c (the unicast server)
*
* Module ID: UFCFQ4-30-2
* Module Name: Computer Networks and Operating Systems
*
* last update : 6 dec 2016
* Author: Brandon Egonmwan - 15024049
*
* Program Description:
* Unicast SNTP client. Sends packets request to a user
* specified server. Receives a packet back,interprets
* this packet, populate them and displays the information.
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
#include <stdint.h>
#include <sys/time.h>
/****************************************************/
/*  Defines */
//my port change it to 123 if using UWE server
#define PORT 5262
//max size of SNTP packet
#define MAX_LEN 48
//the time in seconds since 1/1/1970
#define DELTA (2208988800)
/****************************************************/
/*  Structures */
//this struct holds the second and microseconds
struct timestamp {

	uint32_t seconds;
	uint32_t fractions;
};
//this struct holds the SNTP packets
struct packet {

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
/****************************************************/


int main( int argc, char * argv[]) {

	int sock_et, numbytes;

	double T1,T2,T3,T4;
	double timefraction = 4294967296.0;

	time_t differenceInSeconds;
	//current date
	time_t currentTime;
	/****************************************************/
  //Socket in
	struct sockaddr_in serveraddr;
	//SNTP packets to send
	struct hostent *he;
	struct timeval UT;
	struct packet sendPkt;
	//buffer for the packets received
	char buf[MAX_LEN];
	struct packet receivePkt;
	//get current UNIX time
	gettimeofday(&UT, NULL);

	memset(&sendPkt, 0, MAX_LEN);
	/***************  SEND PACKET  ***************/
	sendPkt.LIVNMODE = 043;
	sendPkt.stratum = 0;
	sendPkt.poll = 0;
	sendPkt.precision = 0;
	sendPkt.rootDelay = 0;
	sendPkt.rootDispersion = 0;
	//convert to NTP when sent
	sendPkt.originateTimestamp.seconds = ntohl(UT.tv_sec + DELTA);
	sendPkt.originateTimestamp.fractions = ntohl(UT.tv_usec * 4295);

	memset(&receivePkt, 0, MAX_LEN);

	if( (he = gethostbyname( argv[ 1])) == NULL)
	{
		perror( "unicast client <h");
		exit(1);
	}
	//create socket
	if( (sock_et = socket( AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror( "unicast client socket");
		exit(1);
	}
	//this holds the address of the serveraddr
	memset( &serveraddr,0, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons( PORT);
	serveraddr.sin_addr = *((struct in_addr *)he -> h_addr);
 //Send packet check
	printf("\nSending Packet...\n");
	if( (sendto(sock_et, &sendPkt, sizeof( struct packet), 0, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr))) == -1)
	{
		perror( "uniclient sendto");
		exit( 1);
	}
	/***************  RECEIVE PACKET  ***************/
	//Receive packet Check
	if((numbytes = recvfrom( sock_et, &receivePkt, sizeof(struct packet), 0, NULL, NULL)) == -1)
	{
		perror( "uniclient recvfrom");
		exit(1);
	}
	//print populated packet
	printf("\nReceived Packet:\n");
	printf("Reference timestamp: %lu.%lu\n", receivePkt.referenceTimestamp.seconds, receivePkt.referenceTimestamp.fractions);
	printf("Original timestamp: %lu.%lu\n", sendPkt.originateTimestamp.seconds, sendPkt.originateTimestamp.fractions);
	printf("Receive timestamp: %lu. %lu\n", receivePkt.receiveTimestamp.seconds, receivePkt.receiveTimestamp.fractions);
	printf("Transmit timestamp: %lu.%lu\n", receivePkt.transmitTimestamp.seconds, receivePkt.transmitTimestamp.fractions);
	//covert to network byte order
	T1 = htonl(sendPkt.originateTimestamp.seconds) + htonl(sendPkt.originateTimestamp.fractions) / timefraction;
	T2 = htonl(receivePkt.receiveTimestamp.seconds) + htonl(receivePkt.receiveTimestamp.fractions) / timefraction;
	T3 = htonl(receivePkt.transmitTimestamp.seconds) + htonl(receivePkt.transmitTimestamp.fractions) / timefraction;
	T4 = UT.tv_sec + DELTA + UT.tv_usec * 4295 / timefraction;

	printf("\nThe delay is %lf, The delay is %lf\n", ((T4-T1)-(T3-T2)));
	printf("The offset is %lf\n", (((T2 - T1)+(T3 - T4))/2));



	differenceInSeconds = ((int32_t)(htonl(receivePkt.receiveTimestamp.seconds) - htonl(sendPkt.originateTimestamp.seconds)) +
	                      (int32_t)(htonl(receivePkt.transmitTimestamp.seconds) - htonl(receivePkt.referenceTimestamp.seconds)) / 2);
	//the current date
	currentTime = time(NULL) - differenceInSeconds;
	printf("\nCurrent server time: %s", ctime(&currentTime));
	printf("__________________________________________________\n\n");
	close( sock_et);
	return 0;
}

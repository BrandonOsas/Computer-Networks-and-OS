/*****************************************************
* Program name: manyserver.c (manycast server)
*
* Module ID: UFCFQ4-30-2
* Module Name: Computer Networks and Operating Systems
*
* last update : 7 dec 2016
* Author: Brandon Egonmwan - 15024049
*
* Program Description:
* Manycast SNTP server. Recieves a SNTP packet request
* from an SNTP client, and sends back SNTP packet.
*
*****************************************************/
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
#include <sys/time.h>
#include <math.h>
#include <time.h>

#define MYPORT 5262
//the port users connect to
#define DELTA (2208988800)
#define SHIFT (1LL<<32) * 1.0e-6 )

struct timestamp{
uint32_t seconds;
uint32_t microseconds;
};

struct packet{
  uint8_t LI_VN_MODE;
  uint8_t Stratum;
  uint8_t Poll;
  uint8_t Precision;

  int32_t RootDelay;
  uint32_t RootDispersion;
  int32_t ReferenceIdentifier;


  struct timestamp referenceStamp;
  struct timestamp originalStamp; //now
  struct timestamp receiveStamp;
  struct timestamp transmitStamp; //now
};
/*********************************************************************************************
* Name					: UNIX_TO_NTP(struct timeval *UT)
* Returns       : void
*    *UT        : UNIX timestamp
* Created by    : Brandon Egonmwan
* Description   : Converts a UNIX timestamp to a NTP compatible version.
*********************************************************************************************/
void UNIX_TO_NTP(struct timeval *tv, struct timestamp *ntp)
{
  ntp->seconds = tv->tv_sec + DELTA;
  ntp->microseconds = (uint32_t)( (double)(UT->tv_usec+1) * (double) SHIFT;
}

int main( void)
{
int sockfd;
struct sockaddr_in my_addr;
// info for my addr i.e. server
struct sockaddr_in their_addr;
// client's address info
struct timestamp ntp;
struct packet SendingPacket;
struct timeval tv;
struct ip_mreq mreq;

int addr_len, numbytes;

if( (sockfd = socket( AF_INET, SOCK_DGRAM, 0)) == -1)
{
  perror( "server socket");
  exit( 1);
}
memset( &my_addr, 0, sizeof( my_addr));
/* zero struct */
my_addr.sin_family = AF_INET;// host byte order
my_addr.sin_port = htons( MYPORT);//ports in network byte order
my_addr.sin_addr.s_addr = INADDR_ANY;//any server ip addresses
my_addr.sin_addr.s_addr = inet_addr("224.0.1.1");// any of server IP addrs
if( bind( sockfd, (struct sockaddr *)&my_addr,sizeof( struct sockaddr)) == -1)
  {
    perror( "server bind");
    exit( 1);
  }

mreq.imr_multiaddr.s_addr = inet_addr("224.0.1.1");
mreq.imr_interface.s_addr = htonl(INADDR_ANY);

if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
{
	 perror("setsockopt mreq");
	 exit(1);
}

 while(1)
 {
addr_len = sizeof( struct sockaddr);
memset(&SendingPacket, 0, 48);

if( (numbytes = recvfrom( sockfd, &SendingPacket, 48, 0,
(struct sockaddr *)&their_addr, &addr_len)) == -1)
{
  perror( "server recvfrom");
  exit( 1);
}
//check the timestamps recieved keep it and convert it to NTP
gettimeofday(&tv, NULL);
UNIX_TO_NTP(&tv, &ntp);
SendingPacket.referenceStamp.seconds = ntohl(ntp.seconds);
SendingPacket.referenceStamp.microseconds = ntohl(ntp.microseconds);

SendingPacket.receiveStamp.seconds = ntohl(ntp.seconds);
SendingPacket.receiveStamp.microseconds = ntohl(ntp.microseconds);

printf( "Got packet from %s\n", inet_ntoa( their_addr.sin_addr));
printf( "Packet is %d bytes long\n", numbytes);

gettimeofday(&tv, NULL);
UNIX_TO_NTP(&tv, &ntp);
/*********  SENDING PACKET  *********/
SendingPacket.LI_VN_MODE = 344;
SendingPacket.Stratum = 2;
SendingPacket.Poll = 4;
SendingPacket.Precision = 0;
SendingPacket.RootDelay = 0;
SendingPacket.RootDispersion = 0;
SendingPacket.transmitStamp.seconds= ntohl(ntp.seconds);
SendingPacket.transmitStamp.microseconds = ntohl(ntp.microseconds);

if( (numbytes = sendto( sockfd, &SendingPacket, 48, 0,
(struct sockaddr *)&their_addr, sizeof( struct sockaddr))) == -1) {
perror( "client sendto");
exit( 1);
 } }
close( sockfd);
return 0;
}

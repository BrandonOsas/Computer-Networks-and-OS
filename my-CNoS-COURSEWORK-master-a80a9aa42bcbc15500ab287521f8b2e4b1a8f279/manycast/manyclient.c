/*****************************************************
* Program name: manyclient.c (manycast client)
*
* Module ID: UFCFQ4-30-2
* Module Name: Computer Networks and Operating Systems
*
* last update : 7 dec 2016
*
* Author: Brandon Egonmwan - 15024049
*
* Program Description:
* Manycast SNTP client. Sends an SNTP request to a multicast
* address, and processes all responses, ordering servers by
* stratum and then by delay. Once the best server has been
* established, the client will then use this for unicast
* SNTP requests.
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
/****************************************************/
/*  Defines */
//my port change it to 123 if using UWE server
#define PORT 5262
//max size of SNTP packet
#define MAX_LEN 48
//the time in seconds since 1/1/1970
#define timedifference (2208988800)
/****************************************************/
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

/****************************************************/
int main( int argc, char * argv[])
  {

  int sockfd, numbytes, addr_len;

  double time1,time2,time3,time4;
  double timefraction = 4294967296.0;
  time_t differenceInSeconds;
  //current date
  time_t currentTime;


  char buf[MAX_LEN];

/****************************************************/
  struct timeval ut; //unix time
  struct hostent *he;
  //socket in
  struct sockaddr_in their_addr;

  struct packet SendingPacket;
  struct packet ReceivingPacket;
  gettimeofday(&ut, NULL);
/***************  SEND PACKET  ***************/
  memset(&SendingPacket, 0, MAX_LEN);
  SendingPacket.LIVNMODE = 043;
  SendingPacket.stratum = 0;
  SendingPacket.poll = 0;
  SendingPacket.precision = 0;
  SendingPacket.rootDelay = 0;
  SendingPacket.rootDispersion = 0;
  //convert to NTP when sent
  SendingPacket.originateTimestamp.seconds = ntohl(ut.tv_sec + timedifference);
  SendingPacket.originateTimestamp.fractions = ntohl(ut.tv_usec * 4295);

  memset(&ReceivingPacket, 0, MAX_LEN);




  if( (he = gethostbyname( argv[ 1])) == NULL)
  {
    perror( "Talker gethostbyname");
    exit( 1);
  }
  //create socket
  if( (sockfd = socket( AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror( "manyclient socket");
    exit( 1);
  }
  //this holds the address of the serveraddr
  memset( &their_addr,0, sizeof(their_addr));

  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons( PORT);
  their_addr.sin_addr = *((struct in_addr *)he -> h_addr);
  //Send packet check
  printf("\nSending Packet...\n");

  if( (sendto(sockfd, &SendingPacket, sizeof( struct packet), 0,
  (struct sockaddr *)&their_addr, sizeof( struct sockaddr))) == -1) {
  perror( "manyclient sendto");
  exit( 1);
  }

  /***************  RECEIVE PACKET  ***************/
	//Receive packet Check
  if( (numbytes = recvfrom( sockfd, &ReceivingPacket, sizeof( struct packet), 0, NULL, NULL)) == -1) {
  perror( "manyclient recvfrom");
  exit(1);
  }

   printf("Received Packet.\n");

   //print populated packet
  printf("reference timestamp seconds %lu , reference timestamp fractions %lu\n" ,
  ReceivingPacket.referenceTimestamp.seconds , ReceivingPacket.referenceTimestamp.fractions);

  printf("originate timestamp seconds %lu , originate timestamp fractions %lu\n" ,
  SendingPacket.originateTimestamp.seconds , SendingPacket.originateTimestamp.fractions);

  printf("transmit timestamp seconds %lu , originate timestamp fractions %lu\n" ,
  ReceivingPacket.transmitTimestamp.seconds , ReceivingPacket.transmitTimestamp.fractions);

  printf("receive timestamp seconds %lu , originate timestamp fractions %lu\n" ,
  ReceivingPacket.receiveTimestamp.seconds , ReceivingPacket.receiveTimestamp.fractions);


  //covert to network byte order
   time1 = htonl(SendingPacket.originateTimestamp.seconds)+ htonl(SendingPacket.originateTimestamp.fractions)/timefraction;
   time2 = htonl(ReceivingPacket.receiveTimestamp.seconds) + htonl(ReceivingPacket.receiveTimestamp.fractions)/timefraction;
   time3 = htonl(ReceivingPacket.transmitTimestamp.seconds) + htonl(ReceivingPacket.transmitTimestamp.fractions)/timefraction;
   time4 = ut.tv_sec + timedifference + ut.tv_usec *4295/timefraction;

   printf("\nthe delay is %lf,and offset is %lf\n", ((time4-time1)-(time3-time2)),(((time2 - time1)+(time3 - time4))/2));


   differenceInSeconds = ((int32_t)(htonl(ReceivingPacket.receiveTimestamp.seconds) - htonl(SendingPacket.originateTimestamp.seconds)) +
  			(int32_t)(htonl(ReceivingPacket.transmitTimestamp.seconds) - htonl(ReceivingPacket.referenceTimestamp.seconds))/2);
  //the current date
  currentTime = time(NULL) - differenceInSeconds;

  printf("current server time: %s\n", ctime(&currentTime));


  close( sockfd);
  return 0;
}

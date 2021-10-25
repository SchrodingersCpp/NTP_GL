#include <chrono>
#include <thread>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <iostream>

long ntpdate();



/*
ntpdate function modified from
https://stackoverflow.com/questions/9326677/is-there-any-c-c-library-to-connect-with-a-remote-ntp-server/19835285#19835285
*/
long ntpdate()
{
    char *hostname=(char *)"91.189.89.198";
    int portno=123;           //NTP is port 123
    int maxlen=1024;          //check our buffers
    int i;                    // misc var i
    unsigned char msg[48]={010,0,0,0,0,0,0,0,0};    // the packet we send
    unsigned long buf[maxlen]; // the buffer we get back
    //struct in_addr ipaddr;
    struct protoent *proto;
    struct sockaddr_in server_addr;
    int s;       // socket
    long tmit;   // the time -- This is a time_t sort of

    proto=getprotobyname("udp");
    s=socket(PF_INET, SOCK_DGRAM, proto->p_proto);
    perror("socket");

    memset( &server_addr, 0, sizeof( server_addr ));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(hostname);
    server_addr.sin_port=htons(portno);

    // send the data
    printf("sending data..\n");
    i=sendto(s,msg,sizeof(msg),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
    perror("sendto");

    // get the data back
    struct sockaddr saddr;
    socklen_t saddr_l = sizeof (saddr);
    i=recvfrom(s,buf,48,0,&saddr,&saddr_l);
    perror("recvfr");

    tmit=ntohl((time_t)buf[4]);    //# get transmit time

    tmit-= 2208988800U; 

    std::cout << "time is " << ctime(&tmit)  << std::endl;
    return tmit;
}

int main(int argc, char *argv[])
{
    ntpdate();

    // get delay seconds (default, 1 sec)
    int sleepSec{1};
    if (argc > 1)
    {
        sleepSec = atoi(argv[1]);
    }

    // start sleeping
    // start timestamp
    time_t t = time(NULL);
    std::cout << "Start sleeping (" << sleepSec << " sec): ";
    printf("%s", ctime(&t));
    std::this_thread::sleep_for(std::chrono::seconds(sleepSec));

    // end timestamp
    t = time(NULL);
    std::cout << "  End sleeping (" << sleepSec << " sec): ";
    printf("%s", ctime(&t));
    std::cout << std::endl;
    // end sleeping

    long ntptime{ ntpdate() }; // get ntp time
    std::cout << "System time is " << time(NULL) - ntptime << " seconds off" << std::endl;
    
    return 0;
}
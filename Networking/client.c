#include <stdio.h>
//#include <windows.h>
#include "client.h"

int set_nb(int s)
{
#ifdef _WIN32
    unsigned long nb_mode = 1;
    return ioctlsocket(s, FIONBIO, &nb_mode);
#else
    int flags = fcntl(s, F_GETFL, 0);
    if (flags < 0)
        return flags;
    flags |= O_NONBLOCK;
    return fcntl(s, F_SETFL, flags);
#endif
}

int s; 
struct sockaddr_in sin;

void InitializeSocket()
{
#ifdef _WIN32
    // this part is only required on Windows: it initializes the Winsock2 dll
    WSADATA wsa_data;
    if (WSAStartup(0x0202, &wsa_data))
    {
        printf("unable to initialize winsock2\n");
        return;
    }
#endif

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s < 0)
    {
        printf("unable to initialize the UDP socket\n");
        return;
    }

    printf("socket %d created\n", s);
    
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr); 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9999); 

    unsigned int timeout = 1000;

    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(unsigned int)))
    {
        printf("unable to set socket option for receive timeout");
        return;
    }     
}

void UpdatePositionsWithServer(float X, float Y)
{
    packetPositions_t myPacket;
    myPacket.auth = 5;
    myPacket.posX = X;
    myPacket.posY = Y;  

    //send
    char *pack = malloc(12);        
    memcpy(pack, &myPacket, 12);
    int sent_bytes = sendto(s, pack, 12, 0, (struct sockaddr*)&sin, sizeof(sin));
    printf("sent %d bytes via UDP\n", sent_bytes);
    //Sleep(1000);


    //receive
    char *packReceive = malloc(12);
    struct sockaddr_in sender_in;
    int sender_in_size = sizeof(sender_in);
    int len = recvfrom(s, packReceive, 12, 0, (struct sockaddr *)&sender_in, &sender_in_size);

    if (len > 0)
    {
        packetPositions_t packetReceived;
        memcpy(&packetReceived, packReceive, 12);            
        printf("Other client %d, PosX: %f, PosY: %f", packetReceived.auth, packetReceived.posX, packetReceived.posY);
    }           
}
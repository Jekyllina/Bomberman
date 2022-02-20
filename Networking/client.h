#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "bomberman.h"

typedef struct packetPositions
{
    int auth;
    float posX;
    float posY;
} packetPositions_t;

void InitializeSocket();
void SendPacket(float X, float Y);
packetPositions_t ReceivePacket();
bomberman_t OtherPlayer(packetPositions_t Packet);
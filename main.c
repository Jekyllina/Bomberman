#include <SDL.h>
#include "bomberman.h"
#include "level001.h"
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

int main(int argc, char **argv)
{
#ifdef _WIN32
    // this part is only required on Windows: it initializes the Winsock2 dll
    WSADATA wsa_data;
    if (WSAStartup(0x0202, &wsa_data))
    {
        printf("unable to initialize winsock2\n");
        return -1;
    }
#endif
    
    int s; 

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s < 0)
    {
        printf("unable to initialize the UDP socket\n");
        return -1;
    }

    set_nb(s);

    printf("socket %d created\n", s);
    
    struct sockaddr_in sin;
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr); 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9999); 

    unsigned int timeout = 1000;

    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(unsigned int)))
    {
        printf("unable to set socket option for receive timeout");
        return -1;
    } 


    level_t level001;

    level_init(&level001, 8, 8, 64, level001_cells);

    bomberman_t player0;
    player0.movable.x = 100;
    player0.movable.y = 100;
    player0.movable.width = 32;
    player0.movable.height = 32;
    player0.movable.speed = 48;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, level001.cols * level001.cell_size, 
        level001.rows * level001.cell_size, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Rect cell_rect = {0, 0, level001.cell_size, level001.cell_size};

    SDL_Rect player0_rect = {0, 0, player0.movable.width, player0.movable.height};

    float delta_right = 0;
    float delta_left = 0;
    float delta_down = 0;
    float delta_up = 0;

    int running = 1;

    while(running)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    delta_right = player0.movable.speed * (1.0 / 60);
                }
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    delta_left = -player0.movable.speed * (1.0 / 60);
                }
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    delta_down = player0.movable.speed * (1.0 / 60);
                }
                if (event.key.keysym.sym == SDLK_UP)
                {
                    delta_up = -player0.movable.speed * (1.0 / 60);
                }
            }

            if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    delta_right = 0;
                }
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    delta_left = 0;
                }
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    delta_down = 0;
                }
                if (event.key.keysym.sym == SDLK_UP)
                {
                    delta_up = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (uint32_t row = 0; row < level001.rows; row++)
        {
            for (uint32_t col = 0; col < level001.cols; col++)
            {
                int32_t cell = level_cell(&level001, col, row);
                int32_t cell_texture = cell & 0xff;

                cell_rect.x = col * level001.cell_size;
                cell_rect.y = row * level001.cell_size;

                if(cell_texture == BLOCK_GROUND)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);                    
                    SDL_RenderFillRect(renderer, &cell_rect);
                }
                else if(cell_texture == BLOCK_WALL)
                {
                    SDL_SetRenderDrawColor(renderer, 100, 50, 0, 255);                    
                    SDL_RenderFillRect(renderer, &cell_rect);
                }
                else if (cell_texture == BLOCK_DESTROYABLE)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 50, 50, 255);
                    SDL_RenderFillRect(renderer, &cell_rect);
                }
            }
        }

        move_on_level(&level001, &player0.movable, delta_right + delta_left, delta_down + delta_up);
        player0_rect.x = player0.movable.x;
        player0_rect.y = player0.movable.y;     

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &player0_rect);

        SDL_RenderPresent(renderer);   


        packetPositions_t myPacket;
        myPacket.auth = 5;
        myPacket.posX = player0.movable.x;
        myPacket.posY = player0.movable.y;  

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
        else
            continue;  
    }

    SDL_Quit();
    return 0;
}
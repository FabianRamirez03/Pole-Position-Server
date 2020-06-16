#include <stdio.h>
#include <windows.h>
#include <winsock.h>
#include <stdbool.h>
#include <errno.h>
#include "CarsInfo.c"
#include "Util.c"
#define BUZZ_SIZE 1024


// our thread for receiving commands
DWORD WINAPI receive_cmds(LPVOID lpParam)


{
// set our socket to the socket passed in as a parameter
    SOCKET current_client = (SOCKET)lpParam;


    // buffer to hold our recived data
    char buf[100];
    // buffer to hold our sent data
    char sendData[100];
    // for error checking
    int res;
    int responseInt;
    char response[100];
    //Listas de informacion de cada vehiculo

    //[name, posX, posY, scaleX, scaleY, speed]
    char carBlueInfo[6][20];


    FILE *fileToWrite;
    FILE *fileToRead;




    // our recv loop

    res = recv(current_client,buf,sizeof(buf),0); // recv cmds
    char bufCopy[50];
    strcpy(bufCopy, buf);

    printf ("Client says: %s\n", buf);

    //Define cual de los dos carros envió informacion y la almacena en una lista
    char *token = strtok(bufCopy, ",");
    int result;
    int isReady = 1;


    //En caso de que la informacion recibida sea del carro rojo
    result = strcmp(token, "red");
    if(result == 0){
        fileToWrite = fopen("infoRedCar.txt","w");
        fprintf(fileToWrite, "%s", buf);
        fclose(fileToWrite);
        char buff[BUZZ_SIZE];
        fileToRead = fopen("infoBlueCar.txt", "r");
        fgets(buff, BUZZ_SIZE, fileToRead);
        strcpy(sendData, strcat(buff, "\n"));
        fclose(fileToRead);
    }
    //en caso de que la informacion ingresada sea del carro azul
    result = strcmp(token, "blue");
    if(result == 0){
        fileToWrite = fopen("infoBlueCar.txt","w");
        fprintf(fileToWrite, "%s", buf);
        fclose(fileToWrite);
        char buff[BUZZ_SIZE];
        fileToRead = fopen("infoRedCar.txt", "r");
        fgets(buff, BUZZ_SIZE, fileToRead);
        strcpy(sendData, strcat(buff, "\n"));
        fclose(fileToRead);
    }
    result = strcmp(token, "bye");
    if(result == 0){
        printf("Terminado\n");
        fileToWrite = fopen("infoBlueCar.txt","w");
        fprintf(fileToWrite, "%s", "wait\n");
        fclose(fileToWrite);
        fileToRead = fopen("infoRedCar.txt","w");
        fprintf(fileToWrite, "%s", "wait\n");
        fclose(fileToRead);
    }

    isReady = strcmp(sendData, "\0");
    if(isReady == 0){
        strcpy(sendData, "wait\n");
    }


    Sleep(10);
    if(res == 0)
    {
        MessageBox(0,"error","error",MB_OK);
        closesocket(current_client);
        ExitThread(0);
    }


    //strcpy(sendData,"Soy el server\n");
    printf ("Server says: %s\n", sendData); //lo qhe dice el server
    Sleep(10);
    send(current_client,sendData,sizeof(sendData),0);


    // clear buffers
    strcpy(sendData,"");
    strcpy(buf,"");



}

int main() {

    // our masterSocket(socket that listens for connections)
    SOCKET sock;

    // for our thread
    DWORD thread;

    WSADATA wsaData;
    struct sockaddr_in server;

    // start winsock
    int ret = WSAStartup(0x101,&wsaData); // use highest version of winsock available

    if(ret != 0)
    {
        return 0;
    }

    // fill in winsock struct ...
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(25557);

    // create our socket
    sock=socket(AF_INET,SOCK_STREAM,0);

    if(sock == INVALID_SOCKET)
    {
        return 0;
    }

    // bind our socket to a port
    if( bind(sock,(struct sockaddr*)&server,sizeof(server)) !=0 )
    {
        return 0;
    }

    // listen for a connection
    if(listen(sock,5) != 0)
    {
        return 0;
    }

    // socket that we snedzrecv data on
    SOCKET client;

    struct sockaddr_in from;
    int fromlen = sizeof(from);

    // loop forever
    while(true)
    {
        // accept connections
        client = accept(sock,(struct sockaddr*)&from,&fromlen);
        printf("Client connected\r\n");

        // create our recv_cmds thread and parse client socket as a parameter
        CreateThread(NULL, 0,receive_cmds,(LPVOID)client, 0, &thread);
    }

    // shutdown winsock
    closesocket(sock);
    WSACleanup();

    // exit
    return 0;
}

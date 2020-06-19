#include <stdio.h>
#include <windows.h>
#include <winsock.h>
#include <stdbool.h>
#include <errno.h>
#include "CarsInfo.c"
#include "Util.c"
#define BUZZ_SIZE 1024


// El hilo que recibe informacion
DWORD WINAPI receive_cmds(LPVOID lpParam)


{
// Iguala nuestro socket al socket enviado como parametro
    SOCKET current_client = (SOCKET)lpParam;


    // buffer que contiene la informacion recibida
    char buf[100];
    // buffer que contiene la informacion a enviar
    char sendData[100];
    // revisa cualquier error producido
    int res;

    //Archivo donde guardara la informacion recibida que debe ser sobreescrita
    FILE *fileToWrite;
    //Archio donde leera la informacion para devolversela al cliente
    FILE *fileToRead;

    //Si obtiene informacion del carro rojo, sobreescribe lo que obtuvo en el archivo
    // del carro rojo y devuelve la informacion del carro azul para que actualice.
    //En caso de ser el carro azul se haria al contrario




    res = recv(current_client,buf,sizeof(buf),0); // recv cmds
    //Copia de la informacion recibida por el cliente
    char bufCopy[50];
    //copia la informacion recibida
    strcpy(bufCopy, buf);

    //imprime que dijo el cliente para mayor claridad
    printf ("Client says: %s\n", buf);

    //Define cual de los dos carros envió informacion y la almacena en una lista
    char *token = strtok(bufCopy, ",");
    int result;
    int isReady = 1;


    //En caso de que la informacion recibida sea del carro rojo
    result = strcmp(token, "red");
    if(result == 0){
        fileToWrite = fopen("infoRedCar.txt","w"); //Asigna el arhivo a sobrescribir como el del carro rojo
        fprintf(fileToWrite, "%s", buf); //Sobreescribe el archivo
        fclose(fileToWrite); //Cierra el archivo
        char buff[BUZZ_SIZE];
        fileToRead = fopen("infoBlueCar.txt", "r"); //Asigna el archivo a leer como el del carro azul
        fgets(buff, BUZZ_SIZE, fileToRead); //Obtiene la informacion
        strcpy(sendData, strcat(buff, "\n")); //copia la informacion a sendData para ser enviada
        fclose(fileToRead); //Cierra el archivo
    }
    //en caso de que la informacion ingresada sea del carro azul
    result = strcmp(token, "blue");
    if(result == 0){
        fileToWrite = fopen("infoBlueCar.txt","w");//Asigna el arhivo a sobrescribir como el del carro azul
        fprintf(fileToWrite, "%s", buf);//Sobreescribe el archivo
        fclose(fileToWrite);//Cierra el archivo
        char buff[BUZZ_SIZE];
        fileToRead = fopen("infoRedCar.txt", "r");//Asigna el archivo a leer como el del carro rojo
        fgets(buff, BUZZ_SIZE, fileToRead);//Obtiene la informacion
        strcpy(sendData, strcat(buff, "\n"));//copia la informacion a sendData para ser enviada
        fclose(fileToRead);//Cierra el archivo
    }
    //Vacia la informacion en los archivos cada vez que se cierra el programa para
    //que no afecten partidas futuras

    result = strcmp(token, "bye"); //Verifica si el programa debe ser cerrado
    if(result == 0){
        printf("Terminado\n"); //Informa que se ha terminada la partida en consola
        fileToWrite = fopen("infoBlueCar.txt","w");//Abre el archivo del carro rojo
        fprintf(fileToWrite, "%s", "wait\n"); //Sobreescribe el archivo
        fclose(fileToWrite);//Cierra el archivo
        fileToRead = fopen("infoRedCar.txt","w"); //Procede a hacer lo mismo con el carro azul
        fprintf(fileToWrite, "%s", "wait\n");
        fclose(fileToRead);
    }

    isReady = strcmp(sendData, "\0"); //Si no recibe nada quiere decir que alguno de los dos carros no esta listo
    if(isReady == 0){
        strcpy(sendData, "wait\n"); //Devuelve wait para indicar que debe esperar a que se conecte el otro carro
    }


    Sleep(10);
    if(res == 0)
    {
        MessageBox(0,"error","error",MB_OK);
        closesocket(current_client);
        ExitThread(0);
    }


    //Despues de manipular la informacion recibida y decidir que debe enviar

    //Print en consola para la claridad de que sera la informacion que se va a enviar
    printf ("Server says: %s\n", sendData); //lo qhe dice el server
    Sleep(10);
   //Envia la informacion al cliente
    send(current_client,sendData,sizeof(sendData),0);


    // Limpia ambos buffers
    strcpy(sendData,"");
    strcpy(buf,"");



}

int main() {

    // Socket principal que recibira informacion
    SOCKET sock;

    // Hilo donde se correra el servidor
    DWORD thread;

    WSADATA wsaData;
    struct sockaddr_in server;

    // Inicia un winsock
    int ret = WSAStartup(0x101,&wsaData); // utiliza la version mas reciente de winSocks

    if(ret != 0)
    {
        return 0;
    }

    // Completa la estructura del WinSock
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(25557);

    // Crea el soccket correctamente
    sock=socket(AF_INET,SOCK_STREAM,0);

    // Si el socket fuera invalido, se detiene el programa
    if(sock == INVALID_SOCKET)
    {
        return 0;
    }

    // se asegura que pueda asignar el socket al puerto identificado anteriormente. 25557 en este caso
    if( bind(sock,(struct sockaddr*)&server,sizeof(server)) !=0 )
    {
        return 0;
    }

    // Escucha por conexiones
    if(listen(sock,5) != 0)
    {
        return 0;
    }

    // socket encargado de enviar informacion
    SOCKET client;

    struct sockaddr_in from;
    int fromlen = sizeof(from);

    // Ciclo que se mantiene corriendo durante la ejecucion del programa
    while(true)
    {
        // Accepta conexiones
        client = accept(sock,(struct sockaddr*)&from,&fromlen);
        printf("Client connected\r\n");

        // Crea un recv_cmds thread y envia la informacion recibida como parametro
        CreateThread(NULL, 0,receive_cmds,(LPVOID)client, 0, &thread);
    }

}

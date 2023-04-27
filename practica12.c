#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>

#define TAM_BUF 512

struct mensaje{
                long tipomsg;
                char msg[TAM_BUF];
                };

//ENviar un mensaje a una cola de mensajes creada previamente

int main(int argc, char *argv[]){
        int idcola;
        int tam;
        struct mensaje buffer;

        if(argc !=2){
        puts("MODO DE EMPLEADO envia nombre del prg y <identificador de cola>");
        exit(-1);
        }
        idcola=atoi(argv[1]);
        puts("Ingrese un mensaje para publicar: ");
        if((fgets((&buffer)->msg,TAM_BUF,stdin))==NULL){
                puts("NO hay mensaje para ser publicado");
                exit(0);
        }
        //ASociar el mensaje ingresado con este proceso
        buffer.tipomsg=getpid();
        //Añadir msg al final de la cola
        tam=strlen(buffer.msg);
        if((msgsnd(idcola,&buffer,tam,0))<0){
        perror("msgsend");
        exit(-1);
        }
        puts("Mensaje publicado");
        exit(0);
}
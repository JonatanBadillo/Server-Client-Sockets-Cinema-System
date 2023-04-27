                                    
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//Crear una cola de mensajes

int main(int argc, char *argv[]){
        int idcola;
        key_t clave;
        system("ipcs -q");
        //Crear cola
        clave=ftok(argv[0],'K');
        if((idcola = msgget (clave,IPC_CREAT|0666))<0){
                perror("msget:create");
                exit(-1);
        }
        printf("Creada cola de mensajes con identificador =%d \n", idcola);
        //Abre cola de mensajes nuevamente
        if((idcola=msgget(clave,0))<0){
                perror("msgget : open");
                exit(-1);
        }
        printf("Abierta una cola de mensajes de identificador = %d \n",idcola);
        system(" ipc -q");
        exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

// definimos nuestro puerto
#define PORT 8080
//el número máximo de conexiones que nuestro servidor aceptará.
#define MAX_CONNECTIONS 3
//el tamaño máximo del buffer que utilizaremos para leer y escribir datos en el socket.
#define MAXBUF 1024

#define PRECIO_ADULTO 70
#define PRECIO_NINOS 30
#define SALAS_TOTALES 5


//variables globales
//Estas variables nos ayudaran a poder consultar los detalles finales de nuestra compra
int cantidadAdultos = 0;
int cantidadNinos = 0;
int totalPagadoAdultos = 0;
int totalPagadoNinos = 0;
int totalPagado = 0;

//estructura para representar la hora
typedef struct {
    int hora;
    int minuto;
} Hora;

//estructura para representar una sala de cine
typedef struct {
    char nombrePelicula[50];
    int asientos[4][2][5];
    Hora horarios[4];
    int noSalaCine;
    double pagoTotal;
    int cuposDisponibles[4]; // Variable para almacenar los cupos disponibles de la sala
} Sala;

typedef struct {
    char nombreDulce[50];
    int costo;
    int cantidadDisponible;
} Dulce;

//PROTOTIPO DE FUNCIONES
void imprimirFunciones(Sala *salas, int client_socket);
void seleccionarPelicula(Sala *salas, int client_socket);
void consultarPrecioBoletos(int client_socket);
void consultarTotalPagado(int client_socket);
void imprimirDulceria(Dulce *dulces,int client_socket);
void comprarDulceria(Dulce *dulces,int client_socket);



//funciones para manejar solicitudes del cliente
void handle_client(int client_socket, Sala *salas,Dulce *dulces) {
    char buffer[MAXBUF];
    int opcion;
    cantidadNinos = 0;
    cantidadAdultos = 0;
    totalPagadoNinos = 0;
    totalPagadoAdultos = 0;
    totalPagado = 0;
    while (1) {
        memset(buffer, 0, MAXBUF);
        snprintf(buffer, sizeof(buffer),
                 "--------------------------------\n"
                 "BIENVENIDO A CINE-BUAP\n"
                 "--------------------------------\n"
                 "Opciones a realizar: \n"
                 "1.Ver Catalogo de Peliculas \n"
                 "2.Comprar entradas \n"
                 "3.Consultar Precios de Entrada \n"
                 "4.Consultar Precio Total de la compra\n"
                 "5.Ver Dulceria \n"
                 "6.Comprar en Dulceria \n"
                 "7.Salir \n"
                 "Elige la opcion a consultar: \n>");
        send(client_socket, buffer, strlen(buffer), 0);

        memset(buffer, 0, MAXBUF);
        if (recv(client_socket, buffer, MAXBUF, 0) <= 0) {
            break;
        }
        sscanf(buffer, "%d", &opcion);

        if (opcion < 1 || opcion > 7) {
            snprintf(buffer, sizeof(buffer), "Opcion invalida, vuelva intentar\n");
            send(client_socket, buffer, strlen(buffer), 0);
            continue;
        }

        switch (opcion) {
            case 1:
                printf(">Cliente ha seleccionado la opcion 1 de ver las Funciones disponibles\n");
                imprimirFunciones(salas, client_socket);
                break;
            case 2:
                printf(">Cliente ha seleccionado la opcion 2 de Comprar Entradas\n");
                seleccionarPelicula(salas, client_socket);
                break;
            case 3:
                printf(">Cliente ha seleccionado la opcion 3 de ver el precio de los boletos\n");
                consultarPrecioBoletos(client_socket);
                break;
            case 4:
                printf(">Cliente ha seleccionado la opcion 4 de ver el Total Pagado:\n");
                consultarTotalPagado(client_socket);
                break;
            case 5:
                printf(">Cliente ha seleccionado la opcion 5 de ver el catalogo de dulceria\n");
                imprimirDulceria(dulces,client_socket);
                break;
            case 6:
                printf(">Cliente ha seleccionado la opcion 6 de comprar en dulceria\n");
                comprarDulceria(dulces,client_socket);
                break;
            case 7:
                printf(">Cliente desconectado\n");
                snprintf(buffer, sizeof(buffer), "--------------------------------\nAdios!\n--------------------------------\n");
                send(client_socket, buffer, strlen(buffer), 0);
                close(client_socket); // Cierra el socket del cliente
                return;      
        }
    }
}



void inicializarDulces(Dulce *dulces){
    const char *nombre_dulces[] = {
        "Palomitas",// Nombre de la pelicula de la sala 1
        "Nachos", //Nombre pelicula de sala 2
        "Refresco"
    };

    dulces[0].costo = 75;//costo palomitas
    dulces[1].costo = 55;//costo nachos
    dulces[2].costo = 30;//costo Refresco
    for (int i = 0; i < 3; i++){
        strcpy(dulces[i].nombreDulce, nombre_dulces[i]);
        dulces[i].cantidadDisponible = 20;//Indicamos que hay 20 cantidad de productos de cada tipo
    }
}




//inicializacion de salas con sus respectivos atributos
void inicializarSalas(Sala *salas) {
    const char *peliculas[] = {
        "Hannibal",// Nombre de la pelicula de la sala 1
        "Joker 2", //Nombre pelicula de sala 2
        "Batman 2", // Nombre de la pelicula de la sala 3
        "Ted 3",// Nombre de la pelicula de la sala 4
        "No manches Frida 4"// Nombre de la pelicula de la sala 5
    };

    const Hora horarios[][4] = {
        { {10, 0}, {12, 30}, {15, 0}, {18, 30} }, // Horarios de proyección para la sala 1
        { {11, 15}, {13, 30}, {16, 0}, {19, 15} }, // Horarios de proyección para la sala 2
        { {11, 30}, {14, 30}, {17, 25}, {20, 45} }, // Horarios de proyección para la sala 3
        { {12, 25}, {15, 30}, {18, 0}, {21, 0} }, // Horarios de proyección para la sala 4
        { {14, 0}, {16, 30}, {19, 20}, {22, 15} }  // Horarios de proyección para la sala 5
    };
    
    for (int i = 0; i < SALAS_TOTALES; i++) {
        strcpy(salas[i].nombrePelicula, peliculas[i]);
        //asigna el valor a cada sala
        salas[i].noSalaCine = i + 1;
        // Inicializar la disponibilidad de los asientos para cada horario
        for (int horario = 0; horario < 4; horario++) {
            for (int fila = 0; fila < 2; fila++) {
                for (int columna = 0; columna < 5; columna++) {
                    salas[i].asientos[horario][fila][columna] = 0; // 0 indica que el asiento está disponible.
                }
            }
            salas[i].cuposDisponibles[horario] = 10; // Inicializar la variable cuposDisponibles para cada horario con el número de cupos disponibles
            salas[i].horarios[horario].hora = horarios[i][horario].hora; // Establece la hora del horario actual para la sala actual
            salas[i].horarios[horario].minuto = horarios[i][horario].minuto; // Establece el minuto del horario actual para la sala actual

        }
    }    
}

//funcion para imprimir peliculas disponibles
void imprimirFunciones(Sala *salas, int client_socket) {
    char buffer[MAXBUF];
    snprintf(buffer, sizeof(buffer), "Salas disponibles:\n");
    send(client_socket, buffer, strlen(buffer), 0);
    for (int i = 0; i < SALAS_TOTALES; i++) {
        snprintf(buffer, sizeof(buffer), "Sala %d -> %s:\n", salas[i].noSalaCine, salas[i].nombrePelicula);
        send(client_socket, buffer, strlen(buffer), 0);
        for (int horario = 0; horario < 4; horario++) {
            snprintf(buffer, sizeof(buffer), "Horario %d - %02d:%02d - %d cupos disponibles\n", horario+1, salas[i].horarios[horario].hora, salas[i].horarios[horario].minuto, salas[i].cuposDisponibles[horario]);
            send(client_socket, buffer, strlen(buffer), 0);
        }
        snprintf(buffer, sizeof(buffer), "\n");
        send(client_socket, buffer, strlen(buffer), 0);
    }
    snprintf(buffer, sizeof(buffer), "\n");
    send(client_socket, buffer, strlen(buffer), 0);
}

//funcion para imprimir el catalogo de dulceria
void imprimirDulceria(Dulce *dulces,int client_socket){
    char buffer[MAXBUF];
    snprintf(buffer, sizeof(buffer), "Dulces disponibles:\n");
    send(client_socket, buffer, strlen(buffer), 0);
    for(int i=0; i<3; i++){
        snprintf(buffer, sizeof(buffer), "Dulce %d : %s  - $%d\n",i+1 ,dulces[i].nombreDulce,dulces[i].costo);
        send(client_socket, buffer, strlen(buffer), 0);
    }
}

void imprimirAsientosDisponibles(Sala *sala, int num_horario, int client_socket) {
    char buffer[MAXBUF];
    snprintf(buffer, sizeof(buffer), "Asientos disponibles:\n");
    send(client_socket, buffer, strlen(buffer), 0);

    for (int fila = 0; fila < 2; fila++) {
        memset(buffer, 0, MAXBUF);
        for (int columna = 0; columna < 5; columna++) {
            if (sala->asientos[num_horario][fila][columna] == 0) {
                snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "[%d,%d] ", fila+1, columna+1);
            } else if (sala->asientos[num_horario][fila][columna] == 1) {
                snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "[X,X] ");
            }
        }
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "\n");
        send(client_socket, buffer, strlen(buffer), 0);
    }
}


//Funcion  para vender boletos por horario
void venderBoletosHorario(Sala *salas, int num_sala, int num_horario, int client_socket) {
    char buffer[MAXBUF];
    int num_asientos = 0;
    Sala *sala = NULL;
    // Busca la sala correspondiente al número de sala dado
    for (int i = 0; i < SALAS_TOTALES; i++) {
        if (salas[i].noSalaCine == num_sala) {
            sala = &salas[i];
            break;
        }
    }
    // Verifica que se haya encontrado la sala
    if (sala == NULL) {
        snprintf(buffer, sizeof(buffer), "Lo sentimos, no hay una sala con ese numero\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }
    // Verifica que el horario seleccionado tenga cupos disponibles
    if (sala->cuposDisponibles[num_horario] <= 0) {
        snprintf(buffer, sizeof(buffer), "Lo sentimos, no hay cupos disponibles para ese horario\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("No hay cupos disponibles para horario seleccionado por el cliente\n");
        return;
    }
    // Mandamos a llamar la funcion que imprime los asientos disponibles para la sala de ese horario
    imprimirAsientosDisponibles(sala, num_horario, client_socket);
    // Solicita al cliente que seleccione un número de asientos
    snprintf(buffer, sizeof(buffer), "Ingrese el numero de asientos que desea comprar: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
        return;
    }
    sscanf(buffer, "%d", &num_asientos);
    printf("El cliente va a comprar %d asientos\n",num_asientos);
    if (num_asientos <= 0) {
        snprintf(buffer, sizeof(buffer), "Opcion invalida\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("Cliente ingreso opcion invalida\n");
        return;
    }
    // Verifica que haya suficientes cupos disponibles para el número de asientos solicitado
    if (num_asientos > sala->cuposDisponibles[num_horario]) {
        snprintf(buffer, sizeof(buffer), "Lo sentimos, no hay suficientes cupos disponibles para esa cantidad de asientos\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("No hay cupos disponibles para la cantidad de asientos que selecciono el cliente\n");
        return;
    }
    // Pregunta al cliente cuántos boletos son para adultos y cuántos para niños
    int num_adultos = 0, num_ninos = 0;
    snprintf(buffer, sizeof(buffer), "Ingrese el numero de boletos para adultos: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
        return;
    }
    sscanf(buffer, "%d", &num_adultos);
    printf("El cliente va a comprar %d asientos de adultos\n",num_adultos);
    cantidadAdultos += num_adultos;
    if (num_adultos < 0 || num_adultos > num_asientos) {
        snprintf(buffer, sizeof(buffer), "Numero invalido de boletos para adultos\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("El cliente selecciono un numero invalido de boletos de adulto\n");
        return;
    }
    num_ninos = num_asientos - num_adultos;
    cantidadNinos += num_ninos;

    // Registra los asientos seleccionados
    for (int i = 0; i < num_asientos; i++) {
        int fila = 0, columna = 0;
        snprintf(buffer, sizeof(buffer), "Ingrese la fila del asiento %d: \n", i + 1);
        send(client_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
            return;
        }
        sscanf(buffer, "%d", &fila);
        snprintf(buffer, sizeof(buffer), "Ingrese la columna del asiento %d: \n", i + 1);
        send(client_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
            return;
        }
        sscanf(buffer, "%d", &columna);
        printf("El cliente selecciono el asiento:[%d][%d]\n",fila,columna);
        fila--;
        columna--;
        if (fila < 0 || fila > 1 || columna < 0 || columna > 4 || sala->asientos[num_horario][fila][columna] == 1) {
            snprintf(buffer, sizeof(buffer), "Asiento invalido\n");
            send(client_socket, buffer, strlen(buffer), 0);
            i--;
            printf("El cliente selecciono asiento invalido\n");
        } else {
            sala->asientos[num_horario][fila][columna] = 1;
        }
    }

    // Decrementa el número de cupos disponibles para el horario seleccionado
    sala->cuposDisponibles[num_horario] -= num_asientos;

    // Calcula el precio total a pagar
    int precio_adulto = num_adultos * PRECIO_ADULTO;
    int precio_ninos = num_ninos * PRECIO_NINOS;
    totalPagadoAdultos += precio_adulto;
    totalPagadoNinos += precio_ninos;
    int precioTotal = precio_adulto + precio_ninos;
    totalPagado += precioTotal;

    // Imprime los detalles de la compra
    snprintf(buffer, sizeof(buffer), "\nDetalles de la compra:\nSala: %d\nPelicula: %s\nHorario: %02d:%02d\nNumero de asientos: %d\nNumero de boletos para adultos: %d\nNumero de boletos para niños: %d\nPrecio total: $%d\n", num_sala, sala->nombrePelicula, sala->horarios[num_horario].hora, sala->horarios[num_horario].minuto, num_asientos, num_adultos, num_ninos, precioTotal);
    send(client_socket, buffer, strlen(buffer), 0);

    printf("Detalles de la compra del cliente:\nSelecciono Sala: %d\nSelecciono Pelicula: %s con horario de %02d:%02d\nNumero de asientos: %d\nNumero de boletos para adultos: %d\nNumero de boletos para niños: %d\nPrecio total: $%d\n", num_sala, sala->nombrePelicula, sala->horarios[num_horario].hora, sala->horarios[num_horario].minuto, num_asientos, num_adultos, num_ninos, precioTotal);
}


//funcion para seleccionar pelicula
void seleccionarPelicula(Sala *salas, int client_socket) {
    int seleccion = 0;
    char buffer[1024];

    snprintf(buffer, sizeof(buffer), "Seleccione una pelicula:\n");
    send(client_socket, buffer, strlen(buffer), 0);

    for (int i = 0; i < SALAS_TOTALES; i++) {
        snprintf(buffer, sizeof(buffer), "%d - %s\n", i+1, salas[i].nombrePelicula);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
        return;
    }
    sscanf(buffer, "%d", &seleccion);

    printf("Cliente selecciono la pelicula: %s\n", salas[seleccion - 1].nombrePelicula);


    if (seleccion < 1 || seleccion > SALAS_TOTALES) {
        snprintf(buffer, sizeof(buffer), "Opcion invalida\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("Cliente selecciono Pelicula Invalida\n");
        return;
    }

    int numSala = 0;
    for (int i = 0; i < SALAS_TOTALES; i++) {
        if (strcmp(salas[i].nombrePelicula, salas[seleccion-1].nombrePelicula) == 0) {
            snprintf(buffer, sizeof(buffer), "Sala %d - %s:\n", salas[i].noSalaCine, salas[i].nombrePelicula);
            send(client_socket, buffer, strlen(buffer), 0);
            for (int horario = 0; horario < 4; horario++) {
                snprintf(buffer, sizeof(buffer), "Horario %d - %02d:%02d - %d cupos disponibles\n", horario+1, salas[i].horarios[horario].hora, salas[i].horarios[horario].minuto, salas[i].cuposDisponibles[horario]);
                send(client_socket, buffer, strlen(buffer), 0);
            }

            numSala = salas[i].noSalaCine;

            int seleccion_horario = 0;
            memset(buffer, 0, sizeof(buffer));
            if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
                return;
            }
            sscanf(buffer, "%d", &seleccion_horario);

            printf("Cliente selecciono la pelicula: %s con horario de %02d:%02d\n", salas[seleccion - 1].nombrePelicula,salas[seleccion - 1].horarios[seleccion_horario-1].hora,salas[seleccion - 1].horarios[seleccion_horario-1].minuto);
            if (seleccion_horario < 1 || seleccion_horario > 4) {
                snprintf(buffer, sizeof(buffer), "Opcion invalida\n");
                send(client_socket, buffer, strlen(buffer), 0);
                printf("Cliente selecciono Horario Invalido\n");
                return;
            }
            venderBoletosHorario(salas, numSala, seleccion_horario - 1, client_socket);
        }
    }
    if (numSala == 0) {
        snprintf(buffer, sizeof(buffer), "Lo sentimos, no hay salas disponibles para esa pelicula\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }
}

//funcion para comprar dulces
void comprarDulceria(Dulce *dulces,int client_socket){
    char buffer[MAXBUF];
    snprintf(buffer, sizeof(buffer), "Seleccione un dulce:\n");
    send(client_socket, buffer, strlen(buffer), 0);

    for (int i = 0; i < 3; i++) {
        snprintf(buffer, sizeof(buffer), "%d - %s - $%d\n", i+1, dulces[i].nombreDulce,dulces[i].costo);
        send(client_socket, buffer, strlen(buffer), 0);
    }
    snprintf(buffer, sizeof(buffer), ">");
    send(client_socket, buffer, strlen(buffer), 0);

    int seleccion;
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
        return;
    }
    sscanf(buffer, "%d", &seleccion);
    printf("Cliente selecciono el dulce: %s\n", dulces[seleccion - 1].nombreDulce);

    if (seleccion < 1 || seleccion > 3) {
        snprintf(buffer, sizeof(buffer), "Opcion invalida\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("Cliente selecciono Dulce Invalido\n");
        return;
    }


    if (dulces[seleccion-1].cantidadDisponible <=0) {
        snprintf(buffer, sizeof(buffer), "Ya no disponemos del dulce solicitado\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("Ya no contamos con %s\n",dulces[seleccion-1].nombreDulce);
        return;
    }

    snprintf(buffer, sizeof(buffer), "Ingrese la cantidad de %s que desea comprar:\n>",dulces[seleccion-1].nombreDulce);
    send(client_socket, buffer, strlen(buffer), 0);
    int cantidadDulce;
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
        return;
    }
    sscanf(buffer, "%d", &cantidadDulce);
    printf("Cantidad que el cliente desea comprar: %d \n", cantidadDulce);

    if (cantidadDulce > dulces[seleccion-1].cantidadDisponible) {
        snprintf(buffer, sizeof(buffer), "No contamos con la cantidad del dulce solicitado en stock\n");
        send(client_socket, buffer, strlen(buffer), 0);
        printf("Cliente ingreso una mayor cantidad de dulces de las que se encuentra en stock\n");
        return;
    }
        

}

//funcion consultar precio de los boletos
void consultarPrecioBoletos(int client_socket) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "Los boletos por nino tienen un costo de: $%d\n", PRECIO_NINOS);
    send(client_socket, buffer, strlen(buffer), 0);

    snprintf(buffer, sizeof(buffer), "Los boletos por adulto tienen un costo de: $%d\n", PRECIO_ADULTO);
    send(client_socket, buffer, strlen(buffer), 0);
}

//funcion para consultar precio total de la compra
void consultarTotalPagado(int client_socket) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "Estos son los detalles finales de su compra: \n");
    send(client_socket, buffer, strlen(buffer), 0);

    int cantidadAsientosTotales = cantidadAdultos + cantidadNinos;
    snprintf(buffer, sizeof(buffer), "*Usted ha comprado %d asientos\n", cantidadAsientosTotales);
    send(client_socket, buffer, strlen(buffer), 0);
    printf("*Cliente ha comprado %d de asientos\n", cantidadAsientosTotales);

    if (cantidadAdultos == 1) {
        snprintf(buffer, sizeof(buffer), "*Usted ha comprado %d boleto de adulto\n", cantidadAdultos);
    } else {
        snprintf(buffer, sizeof(buffer), "*Usted ha comprado %d boletos de adulto\n", cantidadAdultos);
    }
    send(client_socket, buffer, strlen(buffer), 0);

    printf("*Cliente ha comprado %d de boletos de adulto\n", cantidadAdultos);

    if (cantidadNinos == 1) {
        snprintf(buffer, sizeof(buffer), "*Usted ha comprado %d boleto de nino\n", cantidadNinos);
    } else {
        snprintf(buffer, sizeof(buffer), "*Usted ha comprado %d boletos de ninos\n", cantidadNinos);
    }
    send(client_socket, buffer, strlen(buffer), 0);
    printf("*Cliente ha comprado %d de boletos de nino\n", cantidadNinos);

    snprintf(buffer, sizeof(buffer), "*Gasto total en boletos de ninos: %d x $%d = $%d \n", cantidadNinos, PRECIO_NINOS, totalPagadoNinos);
    send(client_socket, buffer, strlen(buffer), 0);

    printf("*Cliente ha gastado $%d en boletos de ninos\n", totalPagadoNinos);

    snprintf(buffer, sizeof(buffer), "*Gasto total en boletos de adultos: %d x $%d = $%d \n", cantidadAdultos, PRECIO_ADULTO, totalPagadoAdultos);
    send(client_socket, buffer, strlen(buffer), 0);

    printf("*Cliente ha gastado $%d en boletos de adulto\n", totalPagadoAdultos);

    snprintf(buffer, sizeof(buffer), "Gasto total: $%d \n", totalPagado);
    send(client_socket, buffer, strlen(buffer), 0);
    printf("*Cliente ha gastado un TOTAL de $%d \n", totalPagado);
}




//funcion principal
int main() {

    Sala salas[SALAS_TOTALES];
    inicializarSalas(salas);

    Dulce dulces[3];
    inicializarDulces(dulces);

    // Crea el socket del servidor
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creando socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        perror("Error en listen");
        return 1;
    }

    printf("Servidor escuchando en el puerto %d\n", PORT);

    while (1) {
        addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);

        if (client_socket < 0) {
            perror("Error en accept");
            continue;
        }

        printf("Cliente conectado: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        handle_client(client_socket, salas, dulces);
    }

    // Cierra el socket del servidor
    close(server_socket);

    return 0;
}
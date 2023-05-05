#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ncurses.h>

//#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 2048

int main() {

    // Declaración de variables y estructuras
    struct sockaddr_in server_addr;
    int sock = 0;
    char buffer[BUFFER_SIZE] = {0};

    // Creación del socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configuración del socket y dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Conversión de dirección IP
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Direccion invalida o no soportada");
        exit(EXIT_FAILURE);
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en la conexion");
        exit(EXIT_FAILURE);
    }

    // Inicializar ncurses y configurar colores
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    //bkgd(COLOR_PAIR(2));
    attron(COLOR_PAIR(1));

    // Mostrar dirección y puerto del servidor
    printw("Conectado al servidor en %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    refresh();

    // Crear ventana de desplazamiento
    int max_lines = LINES - 2;
    WINDOW *scrollwin = newwin(max_lines, COLS/3, 1, COLS/3);
    scrollok(scrollwin, TRUE);
    wsetscrreg(scrollwin, 0, max_lines - 1);
    wbkgd(scrollwin, COLOR_PAIR(3));
    wattron(scrollwin, COLOR_PAIR(3));
    wrefresh(scrollwin);

    // Crear ventana a la izquierda del menú
    WINDOW *leftwin = newwin(max_lines, COLS/3, 1, 0);
    wbkgd(leftwin, COLOR_PAIR(1));
    wattron(leftwin, COLOR_PAIR(1));

    // Crear ventana a la derecha del menú
    WINDOW *rightwin = newwin(max_lines, COLS/3, 1, 2*COLS/3);
    wbkgd(rightwin, COLOR_PAIR(1));
    wattron(rightwin, COLOR_PAIR(1));
    wrefresh(rightwin);

    // Imprimir contenido en la ventana de la izquierda
    mvwprintw(leftwin, 2, 2, "PROXIMAMENTE:");
    mvwprintw(leftwin, 5, 3,
"             _,    _   _    ,_ \n"
"           .o888P     Y8o8Y     Y888o. \n"
"          d88888      88888      88888b \n"
"         d888888b_  _d88888b_  _d8888888 \n"
"         8888888888888888888888888888888 \n"
"         8888888888888888888888888888888 \n"
"         YJGS8PY888PY888PY888888PPY8888P \n"
"          Y888   '8'   Y8P   '8'   888Y \n"
"           '8o          V          o8' \n"
"             `                     ` \n"
"                -----------------  \n"
"                |    BATMAN     |\n"
"                ----------------- ");
    wrefresh(leftwin);

    // Imprimir contenido en la ventana de la derecha
    mvwprintw(rightwin, 2, 2, "PROXIMAMENTE:");
    mvwprintw(rightwin, 5, 3,
"             _,    _   _    ,_ \n"
"           .o888P     Y8o8Y     Y888o. \n"
"          d88888      88888      88888b \n"
"         d888888b_  _d88888b_  _d8888888 \n"
"         8888888888888888888888888888888 \n"
"         8888888888888888888888888888888 \n"
"         YJGS8PY888PY888PY888888PPY8888P \n"
"          Y888   '8'   Y8P   '8'   888Y \n"
"           '8o          V          o8' \n"
"             `                     ` \n"
"                -----------------  \n"
"                |    BATMAN     |\n"
"                ----------------- ");
    wrefresh(rightwin);

    /*====
    // Ejecutar cmatrix -b en segundo plano
    system("cmatrix -bs &");

    // Mantener la ventana de ncurses abierta hasta que se presione una tecla
    getch();
    endwin();
    */

    //=======
    /*// Ejecutar comando y guardar resultado en un archivo temporal
    system("cmatrix -bs > /tmp/cmatrix_output");

    // Leer archivo temporal y escribir contenido en la ventana de la izquierda
    FILE *fp = fopen("/tmp/cmatrix_output", "r");
    if (fp) {
        char line[1000];
        int y = 2;
        while (fgets(line, sizeof(line), fp)) {
            mvwprintw(leftwin, y++, 2, "%s", line);
        }
        fclose(fp);
    }*/
    //====

    // Inicializar conjunto de descriptores de archivo
    fd_set read_fds;
    while (1) {
        // Limpiar y configurar el conjunto de descriptores de archivo
        FD_ZERO(&read_fds); // Limpiar el conjunto de descriptores de archivo
        FD_SET(STDIN_FILENO, &read_fds); // Agregar el descriptor de archivo estándar de entrada al conjunto
        FD_SET(sock, &read_fds); // Agregar el descriptor de archivo del socket al conjunto

        // Monitorear descriptores de archivo usando select()
        int max_fd = sock; // Establecer el descriptor de archivo más alto

        // Monitorear descriptores de archivo usando select()
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Error al usar select");
            break;
        }

        // Leer y mostrar datos del servidor
        if (FD_ISSET(sock, &read_fds)) { // Si hay datos disponibles en el descriptor de archivo del socket
            memset(buffer, 0, BUFFER_SIZE); // Limpiar el buffer
            ssize_t bytes_received = recv(sock, buffer, BUFFER_SIZE, 0); // Recibir datos del servidor

            if (bytes_received <= 0) {
                perror("Error al recibir datos del servidor");
                break;
            }

            wprintw(scrollwin, "%s", buffer); // Imprimir datos recibidos en la ventana de desplazamiento
            wrefresh(scrollwin); // Refrescar la ventana de desplazamiento
        }

        // Leer entrada del usuario y enviarla al servidor
        if (FD_ISSET(STDIN_FILENO, &read_fds)) { // Si hay datos disponibles en el descriptor de archivo de entrada estándar
            char input[BUFFER_SIZE];
            echo(); // Habilitar el eco de caracteres
            wgetstr(scrollwin, input); // Leer la entrada del usuario en la ventana de desplazamiento
            input[strcspn(input, "\n")] = 0; // Elimina el carácter de nueva línea
            send(sock, input, strlen(input), 0); // Enviar la entrada del usuario al servidor
            noecho(); // Deshabilitar el echo de caracteres
        }
	sleep(1);
    }
    system("cmatrix -bs");

    delwin(scrollwin); // Eliminar la ventana de desplazamiento
    endwin(); // Finalizar ncurses
    close(sock); // Cerrar el socket
    return 0;
}
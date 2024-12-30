#include <allegro5/allegro.h> //(Allegro en general)

#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h> // (Para las fuentes de letras)

#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/allegro_image.h> //(Para el manejo de imágenes)

#include <allegro5/allegro_audio.h> //(Para el manejo de sonidos)
#include <allegro5/allegro_acodec.h>

#include <iostream>
#include <chrono>
#include <time.h>
#include <ctime>
#include <stdlib.h>
#include <chrono>
#include <string>
#define FPS 60.0
using namespace std;

const int cant_asteroides = 8;
const int cant_asteroides2 = 30;
const int cant_navesEnemigas = 3;
int cant_asteroidesAux = 8;
int cant_asteroides2Aux = 30;
int cant_navesEnemigasAux = 3;
const int display_x = 1300;
const int display_y = 800;
const float PI = 3.14159265;
int tiempoEspera = 0;
int tiempoEspera2 = 0;
int puntajeObtenido = 0;
int anguloAux = 90;
int aparicionesEnemigas = 0;
ALLEGRO_SAMPLE* explosionJugador;
FILE* archivo;
int contadorBalasImpactadas;
int contadorBalasDisparadas;

typedef struct Asteroide {

	float x;
	float y;
	bool indicador;
	int llave;

};

typedef struct NaveEnemiga {

	int x;
	int y;
	bool indicador = 0;
	int llave;
	int angulo;
	int anguloAux;
	int bandera_y = 0;
	NaveEnemiga* siguiente;
	NaveEnemiga* anterior;
	ALLEGRO_BITMAP* imagen;

} *PtrNaveEnemiga;

typedef struct Bala {

	int x;
	int y;
	int angulo;
	int anguloAux;
	int llave;
	bool indicador;
	Bala* siguiente;
	Bala* anterior;
	ALLEGRO_BITMAP* imagen;


}*PtrBala;

// Función que inicia una bala 

void iniciarBala(PtrBala& balas, int x, int y, int angulo, int llave, bool bandera, int angulo_aux) {

	PtrBala nueva = new (Bala);
	nueva->x = x;
	nueva->y = y;
	nueva->angulo = angulo;
	nueva->anguloAux = angulo_aux;
	nueva->llave = llave;
	nueva->siguiente = NULL;
	nueva->indicador = bandera;
	if (bandera) nueva->imagen = al_load_bitmap("proyectil.png"); // Si es una bala enemiga
	else nueva->imagen = al_load_bitmap("bala.png"); // Si es bala aliada
	PtrBala aux = balas;


	if (balas == NULL) { // Si la pila está nula le asigna la bala

		balas = nueva;
		balas->anterior = NULL;
	}
	else {

		while (aux->siguiente != NULL) { // Recorre hasta el último elemento de la pila y asigna la bala

			aux = aux->siguiente;

		}
		aux->siguiente = nueva;
		aux->siguiente->anterior = aux;
	}

}




// Función que desplaza un bitmap
// Si la bala recibida como parámetro != nula es una bala, de lo contrario es la nave

bool desplazarse(float angulo, int& posicion_x, int& posicion_y, PtrBala& bala, PtrNaveEnemiga& naveE,
	PtrNaveEnemiga& naves, PtrBala& balas) {

	int contador = 0;

	// MOVIMIENTO EN Y POSITIVO

	if (angulo < 180 && angulo > 0) {

		posicion_y -= 5 * sin(angulo * PI / 180);
	}

	// MOVIMIENTO EN Y NEGATIVO

	else if (angulo > 180 && angulo < 360) {

		posicion_y += -(5 * sin(angulo * PI / 180));
	}

	// MOVIMIENTO EN X POSITIVO

	if (angulo > 90 && angulo < 270) {

		posicion_x -= -(5 * cos(angulo * PI / 180));

	}

	// MOVIMIENTO EN X NEGATIVO

	else if (angulo > 270 && angulo <= 360 || angulo < 90 && angulo >= 0) {

		posicion_x += 5 * cos(angulo * PI / 180);
	}


	if (bala != NULL) {


		bala->x = posicion_x;
		bala->y = posicion_y;
	}

	if (naveE != NULL) { // ASIGNA LOS VALORES EN X Y EN Y A LA NAVE ENEMIMGA

		naveE->bandera_y++;

		if (naveE->bandera_y <= 12) naveE->y -= 4;
		else naveE->y += 4;

		if (naveE->x < -100 || naveE->x > display_x + 100) {

			return false;
		}

		if (naveE->bandera_y == 24) naveE->bandera_y = 0;

	}

	return true;

}

int tiempo_aparicion = 0;

bool exploto = false;

int num = 40;

float angulo = 90;

// Función para mover los asteroides

void moverAsteroides(Asteroide& asteroide, Asteroide asteroides[cant_asteroides]) {

	bool bandera = false;

	// Si el asteroide se salió del display se reinicia

	if ((asteroide.x <= -100 || asteroide.x >= display_x + 50) || (asteroide.y >= display_y + 50 ||
		asteroide.y <= -100)) {

		if (rand() % 2 == 0) {

			asteroide.x = 1;
			asteroide.indicador = true;
		}
		else {
			asteroide.x = display_x - 1;
			asteroide.indicador = false;
		}

		while (true) {


			asteroide.y = rand() % display_y;
			for (int i = 0; i < cant_asteroides; i++) {

				if (asteroide.y - asteroides[i].y == 100 || asteroide.y - asteroides[i].y == 100
					&& asteroides[i].llave != asteroide.llave) {
					asteroide.y = rand() % display_y;
					bandera = true;

				}
			}

			if (bandera == false) break;
			bandera = false;

		}
	}

	// Si indicador == true, se mueve a la derecha y abajo, de lo contrario a la izquierda y arriba

	if (asteroide.indicador == false) {
		asteroide.x -= 0.75;
		asteroide.y -= 0.30;
	}
	else {

		asteroide.x += 0.75;
		asteroide.y += 0.30;
	}


}

// Función que inicia asteroides grandes

void iniciarAsteroides(Asteroide& asteroide, int x) {

	asteroide.x = rand() % 2;
	if (asteroide.x == 0) {

		asteroide.x = 1;
	}
	else {

		asteroide.x = display_x - 1;

	}
	asteroide.y = rand() % display_y;
	asteroide.indicador = true;
	asteroide.llave = x;

}

// Función que inicia los asteroides destruidos

void iniciarAsteroidesDestruidos(Asteroide& asteroide, int x, bool indicador, Asteroide asteroides[],
	ALLEGRO_BITMAP* bitmaps_destruidos[]) {

	Asteroide nuevo;
	nuevo.x = asteroide.x + 50;
	if (indicador) nuevo.y = asteroide.y + 70;
	else {

		nuevo.y = asteroide.y - 70;
	}
	nuevo.indicador = asteroide.indicador;
	nuevo.llave = x - 1;
	asteroides[x] = nuevo;

	bitmaps_destruidos[x] = al_load_bitmap("asteroideDestruido.png"); // Puntero a bitmap, y se le asigna la imagen2


}

// Función que valida la colisión contra la nave aliada


bool validarColision(Asteroide& asteroide,
	int nave_x, int nave_y, ALLEGRO_BITMAP*& imagen) {

	if (exploto) return false; // Si ya explotó sale para que no se repita el proceso varias veces en cierta cantidad de segundos

	// I CUADRANTE

	if (asteroide.x - nave_x <= num && asteroide.x - nave_x >= 0 && nave_y - asteroide.y <= num
		&& nave_y - asteroide.y >= 0) {

		imagen = al_load_bitmap("explosion.png"); // Puntero a bitmap, y se le asigna la imagen2
		exploto = true;
		tiempo_aparicion = 0;
		al_play_sample(explosionJugador, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		return true;
		

	}

	// II CUADRANTE

	if (nave_x - asteroide.x <= num && nave_x - asteroide.x >= 0 && nave_y - asteroide.y <= num
		&& nave_y - asteroide.y >= 0) {


		imagen = al_load_bitmap("explosion.png"); // Puntero a bitmap, y se le asigna la imagen2
		exploto = true;
		tiempo_aparicion = 0;
		al_play_sample(explosionJugador, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		return true;
		

	}

	// III CUADRANTE

	if (nave_x - asteroide.x <= num && nave_x - asteroide.x >= 0 && asteroide.y - nave_y <= num
		&& asteroide.y - nave_y >= 0) {


		imagen = al_load_bitmap("explosion.png"); // Puntero a bitmap, y se le asigna la imagen2
		exploto = true;
		tiempo_aparicion = 0;
		al_play_sample(explosionJugador, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		return true;
		
	}

	// IV CUADRANTE

	if (asteroide.x - nave_x <= num && asteroide.x - nave_x >= 0 && asteroide.y - nave_y <= num
		&& asteroide.y - nave_y >= 0) {


		imagen = al_load_bitmap("explosion.png"); // Puntero a bitmap, y se le asigna la imagen2
		exploto = true;
		tiempo_aparicion = 0;
		al_play_sample(explosionJugador, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		return true;
		
	}

	return false;

}

// Función para validar la colisión contra las naves enemigas

void validarColisionEnemigas(PtrNaveEnemiga& nave,
	int nave_x, int nave_y, ALLEGRO_BITMAP*& imagen) {

	Asteroide nuevo;
	nuevo.x = nave->x;
	nuevo.y = nave->y;

	validarColision(nuevo, nave_x, nave_y, imagen);
}

// Función para agregar asteroides

void agregarAsteroides(Asteroide asteroides[cant_asteroides], int contador,
	ALLEGRO_BITMAP* bitmaps[cant_asteroides]) {

	Asteroide nuevo;

	iniciarAsteroides(nuevo, contador);

	asteroides[contador] = nuevo;

	bitmaps[contador] = al_load_bitmap("asteroide.png"); // Puntero a bitmap, y se le asigna la imagen2

}

// Función para quitar las balas de la lista enlazada

void quitarBala(PtrBala& balas, PtrBala& bala) {

	PtrBala aux = balas;
	PtrBala aux2;

	if (aux == bala) {

		if (aux->siguiente == NULL) {

			delete(aux);
			balas = NULL;
			return;
		}


		balas = bala->siguiente;
		balas->anterior = NULL;
		delete(bala);
		bala = NULL;

		return;
	}

	while (aux->siguiente != bala) { 

		aux = aux->siguiente;
	}

	if (bala->siguiente == NULL) {

		aux->siguiente = NULL;

	}

	else {

		aux->siguiente = bala->siguiente;
		aux->siguiente->anterior = aux;

	}


	delete(bala);
	bala = NULL;

}

// Función para quitar las naves enemigas de la lista enlazada

void quitarNaveEnemiga(PtrNaveEnemiga& naves, PtrNaveEnemiga& nave) {

	PtrNaveEnemiga aux = naves;

	if (aux == nave) {

		if (aux->siguiente == NULL) {

			delete(aux);
			naves = NULL;
			return;
		}


		naves = nave->siguiente;
		naves->anterior = NULL;
		delete(nave);
		nave = NULL;

		return;
	}

	while (aux->siguiente != nave) {

		aux = aux->siguiente;
	}

	if (nave->siguiente == NULL) {

		aux->siguiente = NULL;

	}

	else {

		aux->siguiente = nave->siguiente;
		aux->siguiente->anterior = aux;

	}


	delete(nave);
	nave = NULL;

}

// Función para iniciar la nave enemiga en la lista enlazada

void iniciarNaveEnemiga(PtrNaveEnemiga& naves, int x, int y, int angulo, int llave, PtrBala& balas) {

	PtrNaveEnemiga nueva = new (NaveEnemiga);
	nueva->x = x;
	nueva->y = y;
	nueva->angulo = angulo;
	if (angulo == 0) nueva->anguloAux = 270;
	else nueva->anguloAux = 90;
	nueva->llave = llave;
	nueva->siguiente = NULL;
	nueva->imagen = al_load_bitmap("alien.png");
	PtrNaveEnemiga aux = naves;

	if (naves == NULL) {

		naves = nueva;
		naves->anterior = NULL;
	}
	else {

		while (aux->siguiente != NULL) {

			aux = aux->siguiente;

		}
		aux->siguiente = nueva;
		aux->siguiente->anterior = aux;
	}

	if (nueva->x == -2) iniciarBala(balas, nueva->x + 100, nueva->y, nueva->angulo, nueva->llave, 1, 180);
	else iniciarBala(balas, nueva->x - 100, nueva->y, nueva->angulo, nueva->llave, 1, 0);

}

// Función para disparar las balas

void dispararBala(PtrBala& bala, int nave_x, int nave_y, ALLEGRO_BITMAP* imagen) {

	PtrNaveEnemiga aux = NULL;
	desplazarse(bala->angulo, bala->x, bala->y, bala, aux, aux, bala);
	Asteroide asteroide_aux;
	asteroide_aux.x = bala->x;
	asteroide_aux.y = bala->y;

}

// Función para validar si la bala se salió del display o no

bool validarBala(PtrBala& bala, PtrBala& balas) {

	if ((bala->x <= -100 || bala->x >= display_x + 50) || (bala->y >= display_y + 50 ||
		bala->y <= -100)) {

		quitarBala(balas, bala);
		return true;
	}
	return false;
}

// Función para acomodar la lista estática de asteroides

void acomodarLista(Asteroide lista[], int largo, int cantActual) {

	Asteroide listaAux[cant_asteroides2];

	for (int i = 0; i < cant_asteroides2; i++) {

		listaAux[i].x = -500;

	}

	int indice = 0;
	for (int i = 0; i < largo; i++) {

		if (lista[i].x > -200) {

			listaAux[indice] = lista[i];
			indice++;

		}
	}

	for (int i = 0; i < largo; i++) {

		if (listaAux[i].x > -500) {

			lista[i] = listaAux[i];
		}

	}

}

// Función para validar la colisión de las balas aliadas con los asteroides grandes

bool validarColisionBalas(Asteroide asteroides[], PtrBala& balas, int& cantidadAsteroides,
	Asteroide asteroides2[], ALLEGRO_BITMAP* bitmaps_destruidos[], int& cantActualDestruidos, int nave_x,
	int nave_y, ALLEGRO_BITMAP*& imagen) {

	ALLEGRO_BITMAP* imagenAux;
	PtrBala aux = balas;

	while (aux != NULL) {

		if (aux->indicador) {


			Asteroide asteroide_aux;
			asteroide_aux.x = aux->x;
			asteroide_aux.y = aux->y;
			validarColision(asteroide_aux, nave_x, nave_y, imagen);
			if (exploto) {

				imagen = al_load_bitmap("explosion.png"); // Puntero a bitmap, y se le asigna la imagen2
				//quitarBala(balas, aux);

			//	system("pause");
				return false;;
			}

		}
		else {

			for (int i = 0; i < cantidadAsteroides; i++) {

				if (asteroides[i].x < 0) continue;
				validarColision(asteroides[i], aux->x, aux->y, imagenAux);
				if (exploto) {

					exploto = false;

					// Se inician los asteroides pequeños

					iniciarAsteroidesDestruidos(asteroides[i], cantActualDestruidos, 1, asteroides2,
						bitmaps_destruidos);

					cantActualDestruidos++;

					iniciarAsteroidesDestruidos(asteroides[i], cantActualDestruidos, 0, asteroides2,
						bitmaps_destruidos);

					cantActualDestruidos++;

					quitarBala(balas, aux); // Se quita la bala

					asteroides[i].x = -500;
					asteroides[i].y = -500;

					tiempoEspera = 2;

					cant_asteroidesAux--;
					cantidadAsteroides--;

					puntajeObtenido += 50;
					contadorBalasImpactadas++;

					return true;
				}
			}
		}

		aux = aux->siguiente;
	}

	return false;

}

// Función para quitar las balas de la lista enlazada

void deleteBalas(PtrBala& balas) {

	PtrBala aux = balas;

	while (aux != NULL) {

		quitarBala(balas, aux);
		aux = balas;

	}


}

// Función para quitar las naves enemigas de la lista enlazada

void deleteNavesEnemigas(PtrNaveEnemiga& naves, int cantActual) {

	PtrNaveEnemiga aux = naves;

	while (aux != NULL) {

		quitarNaveEnemiga(naves, aux);
		aux = naves;

	}


}

// Función para validar la colisión de las balas aliadas con los asteroides pequeños
bool validarColisionBalas2(PtrBala& balas, Asteroide asteroides2[],
	ALLEGRO_BITMAP* bitmaps_destruidos[], int& cantActualDestruidos, int nave_x,
	int nave_y, ALLEGRO_BITMAP*& imagen) {


	ALLEGRO_BITMAP* imagenAux;
	PtrBala aux = balas;

	while (aux != NULL) {

		if (aux->indicador) {

		}
		else {

			for (int i = 0; i < cantActualDestruidos; i++) {

				validarColision(asteroides2[i], aux->x, aux->y, imagenAux);
				if (exploto) {

					exploto = false;

					asteroides2[i].x = -500;
					asteroides2[i].y = -100;

					// Se inicia el asteroide destruido

					iniciarAsteroidesDestruidos(asteroides2[i], cantActualDestruidos, 1, asteroides2,
						bitmaps_destruidos);

					quitarBala(balas, aux);

					tiempoEspera2 = 2;

					cant_asteroides2Aux--;
					cantActualDestruidos--;

					puntajeObtenido += 25;
					contadorBalasImpactadas++;

					return true;
				}
			}
		}

		aux = aux->siguiente;

	}

	return false;
}

// Función para validar la colisión de las balas aliadas con las naves enemigas

bool validarColisionBalas3(PtrBala& balas, PtrNaveEnemiga& navesEnemigas, int& cantActualEnemigos,
	ALLEGRO_BITMAP*& imagen, int& cantActual) {


	ALLEGRO_BITMAP* imagenAux;
	PtrBala aux = balas;
	PtrNaveEnemiga nave;
	Asteroide asteroide;

	while (aux != NULL) {

		if (aux->indicador) {

		}
		else {

			nave = navesEnemigas;

			while (nave != NULL) {

				asteroide.x = nave->x;
				asteroide.y = nave->y;

				validarColision(asteroide, aux->x, aux->y, imagenAux);
				if (exploto) {

					exploto = false;

					quitarNaveEnemiga(navesEnemigas, nave);

					quitarBala(balas, aux);

					cant_navesEnemigasAux--;
					cantActualEnemigos--;
					puntajeObtenido += 100;
					contadorBalasImpactadas++;

					return true;
				}

				nave = nave->siguiente;
			}
		}

		aux = aux->siguiente;

	}

	return false;
}

// Función para quitar un asteroide destruido

void deleteAsteroidesDestruidos(Asteroide asteroides[], int& cantidadActual) {

	for (int i = 0; i < 30; i++) {

		asteroides[i].x = 0;
		asteroides[i].y = 0;
		asteroides[i].llave = 0;
		asteroides[i].indicador = false;


	}

	cantidadActual = 0;

}

// Función para desplegar el menú de opciones en el display

void opcionesMenu(ALLEGRO_EVENT_QUEUE* colaEventoMenu, bool& menuActive, ALLEGRO_SAMPLE* seleccion) {
	int mousex = 0;
	int mousey = 0;
	int counter = 0;

	
	while (menuActive)
	{
		ALLEGRO_EVENT eventos;
		al_wait_for_event(colaEventoMenu, &eventos);
		if (eventos.type == ALLEGRO_EVENT_MOUSE_AXES) {
			//Se registra la posición en x y y del mouse
			mousex = eventos.mouse.x;
			mousey = eventos.mouse.y;
		}
		if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (eventos.mouse.button & 1) {
				//Opción jugar
				if (mousex >= 500 && mousex <= 790 && mousey >= 420 && mousey <= 470) {
					al_play_sample(seleccion, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);


					menuActive = false;
				}
				//Opción salir
				else if (mousex >= 515 && mousex <= 780 && mousey >= 500 && mousey <= 550) {
					al_play_sample(seleccion, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);


					menuActive = false;
					exit(0);
				}
			}
		}
		al_flip_display();
	}
}

// Función para crear un archivo

void crearArchivo(FILE* archivo) {
	fopen_s(&archivo, "registroRecord.txt", "w");		// Se abre el archivo
	if (archivo == NULL) {													// Avisa si hubo errores al abrirlo
		cout << "NO SE PUDO ABRIR EL ARCHIVO EN CREAR\n";
		return;
	}
	fprintf(archivo, "PUNTAJE RECORD: %i\0", puntajeObtenido); //Se guarda el nuevo record
	fclose(archivo);													//Se cierra el archivo
}

// Función para comprobar el record

bool comprobarRecord() {
	FILE* archivo;

	fopen_s(&archivo, "registroPuntajes.txt", "a");
	struct tm newtime;
	char am_pm[] = "AM";
	__time64_t long_time;
	char timebuf[26];
	errno_t err;

	// Get time as 64-bit integer.
	_time64(&long_time);
	// Convert to local time.
	err = _localtime64_s(&newtime, &long_time);
	if (err) {
		printf("Invalid argument to _localtime64_s.");
		exit(1);
	}
	if (newtime.tm_hour > 12)        // Set up extension.
		strcpy_s(am_pm, sizeof(am_pm), "PM");
	if (newtime.tm_hour > 12)        // Convert from 24-hour
		newtime.tm_hour -= 12;        // to 12-hour clock.
	if (newtime.tm_hour == 0)        // Set hour to 12 if midnight.
		newtime.tm_hour = 12;

	// Convert to an ASCII representation.
	err = asctime_s(timebuf, 26, &newtime);
	if (err)
	{
		printf("Invalid argument to asctime_s.");
		exit(1);
	}
	printf("\n-PUNTAJE:%i BALAS IMPACTADAS: %i BALAS DISPARADAS: %i\t %.19s %s \n", puntajeObtenido, contadorBalasImpactadas, contadorBalasDisparadas, timebuf, am_pm); 

	fprintf(archivo, "\n-PUNTAJE:%i \t BALAS IMPACTADAS:%i \t BALAS DISPARADAS:%i \t %.19s %s \n", puntajeObtenido, contadorBalasImpactadas, contadorBalasDisparadas, timebuf, am_pm); //Se guarda el puntaje
	fclose(archivo);

	fopen_s(&archivo, "registroRecord.txt", "r");	//Se abre en modo lectura
	if (NULL == archivo) {
		printf("NO SE PUDO ABRIR EL ARCHIVO EN COMPROBAR\n");		//Mensaje en caso de error
	}
	else {
		char leido[24];												//Variable para guardar toda la línea
		char recordActual[4];										//Variable para guardar el numero del record
		if (fgets(leido, 24, archivo) != NULL) {

			for (int i = 0; i < 4; i++) {
				if (leido[i + 16] == '\0')							//Si el dígito a tomar es 0 se detiene
					break;
				recordActual[i] = leido[i + 16];					//Se asigna el número del record
			}
			fclose(archivo);
			if (atoi(recordActual) < puntajeObtenido) {		//Una vez tomado el número se compara si es mayor
				cout << "Nuevo record" << endl;						//Se determina que es nuevo record
				crearArchivo(archivo);								//Se manda a registrar el nuevo record
				return true;
			}
			else {
				cout << "No es record" << endl;						//Se determina que no es record
				return false;
			}
		}
		else {
			cout << "ERROR AL LEER RECORD" << endl;					//Error al leer línea
		}
	}
}

// Función para obtener el record

int getRecord() {
	FILE* archivo;
	fopen_s(&archivo, "registroRecord.txt", "r");	//Se abre en modo lectura
	if (NULL == archivo) {
		printf("NO SE PUDO ABRIR EL ARCHIVO EN COMPROBAR\n");		//Mensaje en caso de error
	}
	else {
		char leido[24];												//Variable para guardar toda la línea
		char recordActual[4];										//Variable para guardar el numero del record
		if (fgets(leido, 24, archivo) != NULL) {

			for (int i = 0; i < 4; i++) {
				if (leido[i + 16] == '\0')							//Si el dígito a tomar es 0 se detiene
					break;
				recordActual[i] = leido[i + 16];					//Se asigna el número del record
			}
			fclose(archivo);
		}
		return atoi(recordActual);
	}
}

void main()
{
	al_init(); // Se inicia Allegro
	ALLEGRO_DISPLAY* display; // Se crea un puntero a un Display
	display = al_create_display(display_x, display_y); // Se crea el Display, apadrinado por la variable "display"

	al_set_window_position(display, 50, 50); // Se configura las posiciones y el texto
	al_set_window_title(display, "Prueba");

	al_init_image_addon(); // Se inicia la inlusión de imagenes y font
	al_init_font_addon();
	al_init_ttf_addon();

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1000);

	ALLEGRO_BITMAP* imagen = al_load_bitmap("nave.png"); // Puntero a un bitmap, y le asigna la imagen
	ALLEGRO_BITMAP* imagenFinal = al_load_bitmap("finJuego.png"); // Puntero a un bitmap, y le asigna la imagen
	ALLEGRO_BITMAP* imagenVictoria = al_load_bitmap("imagenVictoria.png"); // Puntero a un bitmap, y le asigna la imagen
	ALLEGRO_BITMAP* menuPrincipal = al_load_bitmap("menuPrincipal.png");

	//ALLEGRO_FONT* fuente = al_load_font("fuente.otf", 40, 0); // Puntero a fuente y se le asigna la fuente descargada


	al_install_keyboard(); // Se inicia el proceso para utilizar el teclado
	al_install_mouse(); // Se instala el proceso del mouse

	ALLEGRO_KEYBOARD_STATE estadoTeclado;

	al_get_keyboard_state(&estadoTeclado);

	ALLEGRO_FONT* fuente1 = al_load_font("fuente2.ttf", 28, NULL);
	ALLEGRO_FONT* fuente2 = al_load_font("Amazing Views.ttf", 48, NULL);
	ALLEGRO_EVENT eventoMenu; // Se crea un evento
	ALLEGRO_EVENT_QUEUE* colaEventoMenu = al_create_event_queue(); // La cola de eventos
	al_register_event_source(colaEventoMenu, al_get_keyboard_event_source()); // Se asigna a la cola el teclado
	al_register_event_source(colaEventoMenu, al_get_mouse_event_source()); // Se asigna a la cola el teclado

	ALLEGRO_TIMER* timer = al_create_timer(0.0090); // Se crea un puntero a un timer
	ALLEGRO_TIMER* timer2 = al_create_timer(1.5); // Se crea un puntero a un timer
	ALLEGRO_TIMER* timer3 = al_create_timer(0.050); // Se crea un puntero a un timer
	ALLEGRO_TIMER* timer4 = al_create_timer(3); // Se crea un puntero a un timer
	ALLEGRO_TIMER* timerM = al_create_timer(1.0);

	al_register_event_source(colaEventoMenu, al_get_timer_event_source(timer)); // Se agrega a la cola el timer
	al_register_event_source(colaEventoMenu, al_get_timer_event_source(timer2)); // Se agrega a la cola el timer
	al_register_event_source(colaEventoMenu, al_get_timer_event_source(timer3)); // Se agrega a la cola el timer
	al_register_event_source(colaEventoMenu, al_get_timer_event_source(timer4)); // Se agrega a la cola el timer
	al_register_event_source(colaEventoMenu, al_get_timer_event_source(timerM));

	//___________________________Menú Principal___________________//


	al_start_timer(timer); // Inicia el timer
	al_start_timer(timer2); // Inicia el timer
	al_start_timer(timer3); // Inicia el timer
	al_start_timer(timer4); // Inicia el timer
	al_start_timer(timerM); // Inicia el timer


	// Variables para las posiciones de los bitmap


	int posicion_x = 650;
	int posicion_y = 400;


	ALLEGRO_BITMAP* bitmaps[cant_asteroides];
	ALLEGRO_BITMAP* bitmapsDestruidos[cant_asteroides2];
	ALLEGRO_BITMAP* imagenVidas = al_load_bitmap("vida3.png"); // Puntero a un bitmap, y le asigna la imagen

	//ALLEGRO_BITMAP* imagenMenu = al_load_bitmap("Menu.png");
	ALLEGRO_BITMAP* BG = al_load_bitmap("background.png");


	//SONIDOS
	ALLEGRO_SAMPLE* seleccion = al_load_sample("seleccion.wav");					//Sonido al seleccionar opciones
	ALLEGRO_SAMPLE* sonidoDisparo = al_load_sample("sonidoDisparo.wav");			//Sonido al disparar
	ALLEGRO_SAMPLE* sonidoAsteroide = al_load_sample("explosionAsteroide.wav");	//Sonido al explotar asteroides
	ALLEGRO_SAMPLE* gameOver = al_load_sample("GameOver.wav");					//Sonido al perder
	explosionJugador = al_load_sample("explosionAsteroide.wav");	//Sonido al explotar jugador
	ALLEGRO_SAMPLE* victoria = al_load_sample("victory.wav");					//Sonido al ganar
	ALLEGRO_SAMPLE* soundtrack = al_load_sample("soundtrack.mp3");				//Música de fondo

	float w = al_get_bitmap_width(imagen);
	float h = al_get_bitmap_height(imagen);

	Asteroide asteroides2[cant_asteroides2];
	Asteroide asteroides[cant_asteroides];

	bool menuActive = true;
	int cantidad_actual = 1;
	int cantidad_actual_destruidos = 0;
	int cantidad_actual_naves_enemigas = 0;

	// Se inician los asteroides grandes

	for (int i = 0; i < cant_asteroides; i++) {

		agregarAsteroides(asteroides, i, bitmaps);

	}

	// Se inician los asteroides destruidos
	for (int i = 0; i < cant_asteroides2; i++) {

		agregarAsteroides(asteroides2, i, bitmapsDestruidos);

	}

	Asteroide asteroideNull;
	PtrBala balas = NULL;
	PtrBala auxBalas;
	PtrBala balaNull = NULL;
	PtrNaveEnemiga naveEnemigaNull = NULL;
	PtrNaveEnemiga navesEnemigas = NULL;
	PtrNaveEnemiga auxNavesEnemigas;
	ALLEGRO_BITMAP* imagenNula = NULL;
	contadorBalasDisparadas = 0;
	int cantidadVidas = 3;

	char puntajeObtenidoText[30];
	srand(time(NULL));
	al_play_sample(soundtrack, 0.3, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);


	//___________________________Juego___________________________//
	while (true) {

		// Si ya no hay naves ni asteroides en pantalla, se termina el juego con una victoria

		if (cantidad_actual == 0 && cantidad_actual_destruidos == 0 && cantidad_actual_naves_enemigas == 0) {

			al_clear_to_color(al_map_rgb(0, 0, 0)); // Se reinicia la pantalla 
			bool gane = true;
			while (true) {

				al_draw_bitmap(imagenVictoria, 0, 0, 0); // Se agrega al display la imagen2, pero no se muestra todavía
				al_draw_text(fuente1, al_map_rgb(255, 255, 255), 500, 200, ALLEGRO_ALIGN_LEFT, "PRESIONE <R> PARA SEGUIR JUGANDO");

				if (gane) {
					al_play_sample(victoria, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					if (comprobarRecord())
						al_draw_text(fuente2, al_map_rgb(255, 255, 255), 100, 100, ALLEGRO_ALIGN_CENTER, "NUEVO RECORD REGISTRADO");
				}
				ALLEGRO_EVENT eventos;
				al_wait_for_event(colaEventoMenu, &eventos);
				if (eventos.type == ALLEGRO_EVENT_KEY_CHAR)
				{
					// Si presiona la tecla "R" se reinician los datos de la partida para volver a jugar
					if (eventos.keyboard.keycode == ALLEGRO_KEY_R) {
						al_play_sample(seleccion, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
						imagenVidas = al_load_bitmap("vida3.png");
						cantidadVidas = 3;
						deleteBalas(balas);
						deleteNavesEnemigas(navesEnemigas, cantidad_actual_naves_enemigas);
						cantidad_actual_naves_enemigas = 0;
						cantidad_actual = 1;
						cant_asteroidesAux = cant_asteroides;
						cant_asteroides2Aux = cant_asteroides2;
						cant_navesEnemigasAux = cant_navesEnemigas;
						puntajeObtenido = 0;
						aparicionesEnemigas = 0;
						contadorBalasImpactadas = 0;
						contadorBalasDisparadas = 0;
						_itoa_s(puntajeObtenido, puntajeObtenidoText, 10);
						gane = false;
						for (int i = 0; i < cant_asteroides; i++) iniciarAsteroides(asteroides[i], i);

						break;
					}// Si presiona escape se termina el juego
					else if (eventos.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
						al_play_sample(seleccion, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
						exit(0);
					}
				}
				gane = false;
				//al_draw_text(fuente1, al_map_rgb(255, 255, 255), 500, 200, ALLEGRO_ALIGN_LEFT, "EL JUEGO HA TERMINADO");
				al_flip_display();
			}


		}
		al_clear_to_color(al_map_rgb(0, 0, 0)); // Se reinicia la pantalla 
		al_draw_bitmap(BG, 0, 0, 0);			//Se pone el BackGround

		if (menuActive) {
			al_draw_bitmap(menuPrincipal, 0, 0, 0);
			_itoa_s(getRecord(), puntajeObtenidoText, 10);
			al_draw_text(fuente2, al_map_rgb(0, 0, 0), 205, 270, ALLEGRO_ALIGN_CENTER, puntajeObtenidoText);
			opcionesMenu(colaEventoMenu, menuActive, seleccion);		//Llamado al menú
		}


		acomodarLista(asteroides, cant_asteroides, cantidad_actual);
		acomodarLista(asteroides2, cant_asteroides2, cantidad_actual_destruidos);

		_itoa_s(puntajeObtenido, puntajeObtenidoText, 10);
		al_draw_text(fuente1, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, "PUNTAJE: ");
		al_draw_text(fuente1, al_map_rgb(255, 255, 255), 120, 10, ALLEGRO_ALIGN_LEFT, puntajeObtenidoText);
		al_draw_text(fuente1, al_map_rgb(255, 255, 255), 10, 50, ALLEGRO_ALIGN_LEFT, "Presione ESCAPE para salir");

		al_draw_bitmap(imagenVidas, 550, 20, 0); // Se agrega al display la imagen2, pero no se muestra todavía

		al_draw_rotated_bitmap(imagen, w / 2, h / 2, posicion_x, posicion_y, (anguloAux - 90) * 3.14159 / 180, 0);
		
		// Se pintan los asteroides actuales
		for (int i = 0; i < cantidad_actual; i++) {
			if (asteroides[i].x < -200) continue;

			al_draw_bitmap(bitmaps[i], asteroides[i].x, asteroides[i].y, 0); // Se agrega al display la imagen2, pero no se muestra todavía
		}

		// Se pintan los asteroides destruidos actuales
		for (int i = 0; i < cantidad_actual_destruidos; i++) {

			if (asteroides2[i].x < -200) continue;

			al_draw_bitmap(bitmapsDestruidos[i], asteroides2[i].x, asteroides2[i].y, 0); // Se agrega al display la imagen2, pero no se muestra todavía
		}
		// Se pintan las balas actuales
		auxBalas = balas;
		while (auxBalas != NULL) {
			
			al_draw_rotated_bitmap(auxBalas->imagen, w / 2, h / 2, auxBalas->x, auxBalas->y, (auxBalas->anguloAux - 90)
				* 3.14159 / 180, 0); // Se agrega al display la imagen2, pero no se muestra todavía
			auxBalas = auxBalas->siguiente;
		}
		// Se pintan las naves enemigas actuales
		auxNavesEnemigas = navesEnemigas;
		while (auxNavesEnemigas != NULL) {

			al_draw_rotated_bitmap(auxNavesEnemigas->imagen, w / 2, h / 2, auxNavesEnemigas->x,
				auxNavesEnemigas->y, (auxNavesEnemigas->angulo + 90) * 3.14159 / 180, 0); // Se agrega al display la imagen2, pero no se muestra todavía
			auxNavesEnemigas = auxNavesEnemigas->siguiente;
		}

		al_wait_for_event(colaEventoMenu, &eventoMenu);

		if (eventoMenu.type == ALLEGRO_EVENT_TIMER) { // Si el evento es de tipo timer

			/* La imagen se mueve de la esquina inferior derecha a la esquina superior izquierda hasta que llegué a
			una pisición indicada. Cuando llega a esta posición se devuelve de donde venía */

			if (eventoMenu.timer.source == timer) {

				for (int i = 0; i < cantidad_actual; i++) {

					if (asteroides[i].x < -200) continue;//iniciarAsteroides(asteroides[i], i);

					moverAsteroides(asteroides[i], asteroides);
					validarColision(asteroides[i], posicion_x, posicion_y, imagen);

				}

				for (int i = 0; i < cantidad_actual_destruidos; i++) {

					if (asteroides2[i].x < -200) continue;//iniciarAsteroidesDestruidos(asteroides2[i],
					//i, true, asteroides2, bitmapsDestruidos);
					moverAsteroides(asteroides2[i], asteroides2);
					if (validarColision(asteroides2[i], posicion_x, posicion_y, imagen)) tiempoEspera2 = 1;
				}

				if (tiempoEspera <= 0) {

					if (exploto == false) {
						if (validarColisionBalas(asteroides, balas, cantidad_actual, asteroides2, bitmapsDestruidos
							, cantidad_actual_destruidos, posicion_x, posicion_y, imagen))
							al_play_sample(sonidoAsteroide, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

					}

				}


				if (tiempoEspera2 == 0 && exploto == false) if (validarColisionBalas2(balas, asteroides2, bitmapsDestruidos
					, cantidad_actual_destruidos, posicion_x, posicion_y, imagen))
					al_play_sample(sonidoAsteroide, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);


				if (exploto == false) if (validarColisionBalas3(balas, navesEnemigas, cantidad_actual_naves_enemigas,
					imagenNula, cantidad_actual_naves_enemigas)) al_play_sample(sonidoAsteroide, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

				auxBalas = balas; // Si hay balas disponibles valida que no se hayan salido del display
				while (auxBalas != NULL) {

					if (validarBala(auxBalas, balas)) {

						auxBalas = balas;
					}
					if (balas != NULL) {


						auxBalas = auxBalas->siguiente;
					}
					else {

						auxBalas = NULL;
					}

				}

				auxNavesEnemigas = navesEnemigas;

				while (auxNavesEnemigas != NULL) { // Si hay naves enemigas actualmente, se valida la colisión

					validarColisionEnemigas(auxNavesEnemigas, posicion_x, posicion_y, imagen);
					auxNavesEnemigas = auxNavesEnemigas->siguiente;
				}


			}

			if (eventoMenu.timer.source == timer2) {

				if (exploto) { // Si la nave exploto

					aparicionesEnemigas = 0; // Se reinicia la variable

					if (puntajeObtenido >= 100) // Si el puntaje actual es mayor a 100 se decrementa
						puntajeObtenido -= 100;
					else
						puntajeObtenido = 0;
					cantidadVidas--;

					tiempo_aparicion++; // Se incrementa el tiempo de aparición
					 
					deleteBalas(balas); // Se borran las balas
					// Se borran las naves enemigas
					deleteNavesEnemigas(navesEnemigas, cantidad_actual_naves_enemigas);
					cantidad_actual_naves_enemigas = 0;

					// Se pinta la imagen de las vidas actuales

					if (cantidadVidas == 2) imagenVidas = al_load_bitmap("vida2.png"); 
					else if (cantidadVidas == 1) imagenVidas = al_load_bitmap("vida1.png");
					else {

						al_clear_to_color(al_map_rgb(0, 0, 0)); // Se reinicia la pantalla 
						bool perdio = true;

						while (true) {

							al_draw_bitmap(imagenFinal, 0, 0, 0); // Se agrega al display la imagen2, pero no se muestra todavía
							al_draw_text(fuente1, al_map_rgb(255, 255, 255), 500, 200, ALLEGRO_ALIGN_LEFT, "PRESIONE <R> PARA SEGUIR JUGANDO");
							if (perdio) { // Si perdió se despliega el sonido de derrota
								al_play_sample(gameOver, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
							}
							ALLEGRO_EVENT eventos;
							al_wait_for_event(colaEventoMenu, &eventos);
							if (eventos.type == ALLEGRO_EVENT_KEY_CHAR) // Si recibe una R se reinician los datos de la partida
							{
								if (eventos.keyboard.keycode == ALLEGRO_KEY_R) {
									al_play_sample(seleccion, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
									imagenVidas = al_load_bitmap("vida3.png");
									cantidadVidas = 3;
									puntajeObtenido = 0;
									// Se reinican los datos y variables
									cantidad_actual = 0;
									cantidad_actual_destruidos = 0;
									cant_asteroides2Aux = cant_asteroides2;
									cant_asteroidesAux = cant_asteroides;
									cantidad_actual_naves_enemigas = 0;
									contadorBalasImpactadas = 0;
									contadorBalasDisparadas = 0;
									_itoa_s(puntajeObtenido, puntajeObtenidoText, 10);
									break;
								}
								else if (eventos.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
									al_play_sample(seleccion, 0.8, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
									exit(0);
								}
							}
							perdio = false;
							al_flip_display();
						}


					}

				}

				// Si la cantidad actual de asteroides grandes es menor a la cantidad disponible

				if (cantidad_actual < cant_asteroidesAux) cantidad_actual++;

				if (tiempo_aparicion >= 1) { // Si el tiempo de aparición es mayor a 1

					if (tiempo_aparicion == 1) { // Si es 1 el tiempo de aparición se borran los asteroides

						for (int i = 0; i < 1; i++) {

							asteroides[i].x = 0;
							asteroides[i].y = 0;
						}

						imagen = al_load_bitmap("nave.png"); // Puntero a un bitmap, y le asigna la imagen						
						cantidad_actual = 1;
						posicion_x = 650;
						posicion_y = 400;
						tiempo_aparicion++;

						deleteAsteroidesDestruidos(asteroides2, cantidad_actual_destruidos);
						tiempoEspera2 = 0;

						for (int i = 0; i < cant_asteroides; i++)
							iniciarAsteroides(asteroides[i], i);
					}
				
					iniciarAsteroides(asteroides[cantidad_actual - 1], cantidad_actual - 1);
				
					if (cantidad_actual == 1) exploto = false;

					if (cantidad_actual == cant_asteroidesAux) {
						// Si ya estan todos los asteeroides disponibles en pantalla 
						tiempo_aparicion = 0;
						srand(time(NULL));

					}


				}

				tiempoEspera2 = 0;

				if (tiempoEspera > 0) tiempoEspera = 0;

				if (puntajeObtenido >= 1)
					puntajeObtenido -= 1;

			}
			if (eventoMenu.timer.source == timer3) {

				for (int i = 0; i < cantidad_actual; i++) { // Se valida la colisión de los asteroides grandes

					validarColision(asteroides[i], posicion_x, posicion_y, imagen);
				}

				auxBalas = balas;
				while (auxBalas != NULL) {
					// Se disparan las balas
					dispararBala(auxBalas, posicion_x, posicion_y, imagen);
					auxBalas = auxBalas->siguiente;
				}
				auxNavesEnemigas = navesEnemigas;

				while (auxNavesEnemigas != NULL) { // Si hay naves enemigas actualmente

					// Se desplazan las naves enemigas

					if (desplazarse(auxNavesEnemigas->angulo, auxNavesEnemigas->x, auxNavesEnemigas->y,
						balaNull, auxNavesEnemigas, navesEnemigas, balas) == false) {
						cantidad_actual_naves_enemigas--;
						quitarNaveEnemiga(navesEnemigas, auxNavesEnemigas);
						auxNavesEnemigas = navesEnemigas;
						aparicionesEnemigas++; // Se suma el contador de apariciones en pantalla de las naves
						if (aparicionesEnemigas == 6 && cant_navesEnemigasAux == 3 ||
							aparicionesEnemigas == 8 && cant_navesEnemigasAux == 2 ||
							aparicionesEnemigas == 9 && cant_navesEnemigasAux == 1) {
							// Dependiendo de la cantidad de apariciones en pantalla de las naves, se elminan
							cant_navesEnemigasAux--;

						}
						if (auxNavesEnemigas == NULL) break;


					}
					auxNavesEnemigas = auxNavesEnemigas->siguiente;

				}

			}

			if (eventoMenu.timer.source == timer4) {

				if (cantidad_actual_naves_enemigas < cant_navesEnemigasAux && cantidad_actual == cant_asteroidesAux) {

					// Si las naves enemigas_actuales son menor a la cantidad de naves disponibles

					if (rand() % 2 == 0) iniciarNaveEnemiga(navesEnemigas, -2, posicion_y,
						0, 0, balas);
					else iniciarNaveEnemiga(navesEnemigas, display_x + 2, posicion_y,
						180, 0, balas);


					cantidad_actual_naves_enemigas++;


				}

			}

		}


		if (eventoMenu.type == ALLEGRO_EVENT_KEY_CHAR) { // Si el evento es de tipo teclado

			if (exploto) continue; // Si la nave exploto no se puede mover

			switch (eventoMenu.keyboard.keycode) {

				
			case ALLEGRO_KEY_A: // Movimiento hacía la izquierda

				angulo += 5;
				anguloAux -= 5;
				if (angulo >= 360) {

					angulo = 0;
				}
				break;

			case ALLEGRO_KEY_D: // Movimiento hacía la derecha
				
				angulo -= 5;
				anguloAux += 5;
				if (angulo <= 0) {

					angulo = 360;
				}

				break;


			case ALLEGRO_KEY_W: // Si presiona w de mueve hacía el frente

				desplazarse(angulo, posicion_x, posicion_y, balaNull, naveEnemigaNull, navesEnemigas, balas);
				for (int i = 0; i < cantidad_actual; i++) {

					validarColision(asteroides[i], posicion_x, posicion_y, imagen);

				}


				break;

			case ALLEGRO_KEY_SPACE: // Se dispara una bala

				iniciarBala(balas, posicion_x, posicion_y, angulo, contadorBalasDisparadas, 0, anguloAux);
				cout << contadorBalasImpactadas;
				contadorBalasDisparadas++;
				al_play_sample(sonidoDisparo, 0.3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				break;

			case ALLEGRO_KEY_ESCAPE:
				exit(0);

				return;
			}
		}
		al_flip_display(); // Se actualiza el display

		// Se destruye lo utilizado por allegro
	}

	//al_destroy_font(fuente);

	al_destroy_display(display);

	al_destroy_event_queue(colaEventoMenu);

	//al_destroy_bitmap(imagen);

	al_destroy_timer(timer);
}
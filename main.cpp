#include <iostream>
#include <cstring>
using namespace std;

typedef struct {
    int idLote;
    char nombreComponente[50];
    float pesoUnitario;
    int cantidadTotal;
} LoteProduccion;

LoteProduccion **almacen;
int filas, columnas;

LoteProduccion *maestroLotes;
int *indicesDisponibles;
int capacidadMaestro;
int lotesUsados;

int contadorID;

int *pilaIDLote;
int *pilaResultado;
int topPila;
const int sizePila = 10;

void inicializarSistema();
LoteProduccion** crearAlmacen(int f, int c);
void liberarAlmacen();
void inicializarMaestro();
int buscarIndiceLibre();
void expandirMaestro();
void inicializarPila();
void colocarLote(int fila, int columna);
void pushInspeccion(int idLote, int resultado);
bool popInspeccion();
void reportarFila(int fila);
void buscarComponente(const char* nombre);
void liberarTodo();

int main() {
    cout << "Sistema de Almacen - AlphaTech" << endl;
    cout << "=============================" << endl;
    
    inicializarSistema();
    
    int opcion;
    do {
        cout << "\n--- MENU PRINCIPAL ---" << endl;
        cout << "1. Ingresar nuevo lote" << endl;
        cout << "2. Ver estanteria completa" << endl;
        cout << "3. Buscar componente" << endl;
        cout << "4. Inspeccion de calidad" << endl;
        cout << "5. Deshacer inspeccion" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        
        switch(opcion) {
            case 1: {
                int estanteria, posicion;
                cout << "Estanteria (0-" << (filas-1) << "): ";
                cin >> estanteria;
                cout << "Posicion (0-" << (columnas-1) << "): ";
                cin >> posicion;
                colocarLote(estanteria, posicion);
                break;
            }
            case 2: {
                int numEstanteria;
                cout << "Ver estanteria numero: ";
                cin >> numEstanteria;
                reportarFila(numEstanteria);
                break;
            }
            case 3: {
                char nombre[50];
                cout << "Nombre del componente: ";
                cin.ignore();
                cin.getline(nombre, 50);
                buscarComponente(nombre);
                break;
            }
            case 4: {
                int id, resultado;
                cout << "ID del lote: ";
                cin >> id;
                cout << "Resultado (1=OK, 0=Falla): ";
                cin >> resultado;
                pushInspeccion(id, resultado);
                break;
            }
            case 5: {
                popInspeccion();
                break;
            }
            case 0:
                cout << "Cerrando sistema..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while(opcion != 0);
    
    return 0;
}

void inicializarSistema() {
    cout << "Configuracion inicial..." << endl;
    
    cout << "Numero de estanterias: ";
    cin >> filas;
    cout << "Posiciones por estanteria: ";
    cin >> columnas;
    
    almacen = crearAlmacen(filas, columnas);
    inicializarMaestro();
    inicializarPila();
    contadorID = 1001;
    
    cout << "Almacen " << filas << "x" << columnas << " listo" << endl;
}

LoteProduccion** crearAlmacen(int f, int c) {
    LoteProduccion** nuevoAlmacen = new LoteProduccion*[f];
    
    for(int i = 0; i < f; i++) {
        nuevoAlmacen[i] = new LoteProduccion[c];
        for(int j = 0; j < c; j++) {
            nuevoAlmacen[i][j].idLote = -1;
        }
    }
    
    return nuevoAlmacen;
}

void liberarAlmacen() {
    if(almacen != NULL) {
        for(int i = 0; i < filas; i++) {
            delete[] almacen[i];
        }
        delete[] almacen;
        almacen = NULL;
    }
}

void inicializarMaestro() {
    capacidadMaestro = 100;
    lotesUsados = 0;
    
    maestroLotes = new LoteProduccion[capacidadMaestro];
    indicesDisponibles = new int[capacidadMaestro];
    
    for(int i = 0; i < capacidadMaestro; i++) {
        indicesDisponibles[i] = 0;
        maestroLotes[i].idLote = -1;
    }
}

int buscarIndiceLibre() {
    for(int i = 0; i < capacidadMaestro; i++) {
        if(indicesDisponibles[i] == 0) {
            return i;
        }
    }
    return -1;
}

void expandirMaestro() {
    int nuevaCapacidad = capacidadMaestro * 2;
    
    LoteProduccion *nuevoMaestro = new LoteProduccion[nuevaCapacidad];
    int *nuevosIndices = new int[nuevaCapacidad];
    
    for(int i = 0; i < capacidadMaestro; i++) {
        nuevoMaestro[i] = maestroLotes[i];
        nuevosIndices[i] = indicesDisponibles[i];
    }
    
    for(int i = capacidadMaestro; i < nuevaCapacidad; i++) {
        nuevosIndices[i] = 0;
        nuevoMaestro[i].idLote = -1;
    }
    
    delete[] maestroLotes;
    delete[] indicesDisponibles;
    
    maestroLotes = nuevoMaestro;
    indicesDisponibles = nuevosIndices;
    capacidadMaestro = nuevaCapacidad;
}

void colocarLote(int fila, int columna) {
    if(fila < 0 || fila >= filas || columna < 0 || columna >= columnas) {
        cout << "Posicion no valida" << endl;
        return;
    }
    
    if(almacen[fila][columna].idLote != -1) {
        cout << "Esa posicion ya esta ocupada" << endl;
        return;
    }
    
    int indiceLibre = buscarIndiceLibre();
    if(indiceLibre == -1) {
        expandirMaestro();
        indiceLibre = buscarIndiceLibre();
    }
    
    LoteProduccion nuevoLote;
    nuevoLote.idLote = contadorID++;
    
    cout << "Nombre del componente: ";
    cin.ignore();
    cin.getline(nuevoLote.nombreComponente, 50);
    
    cout << "Peso unitario: ";
    cin >> nuevoLote.pesoUnitario;
    
    cout << "Cantidad total: ";
    cin >> nuevoLote.cantidadTotal;
    
    maestroLotes[indiceLibre] = nuevoLote;
    indicesDisponibles[indiceLibre] = 1;
    
    almacen[fila][columna] = nuevoLote;
    
    lotesUsados++;
    
    cout << "Lote " << nuevoLote.idLote << " guardado en estanteria " << fila 
         << " posicion " << columna << endl;
}

void inicializarPila() {
    pilaIDLote = new int[sizePila];
    pilaResultado = new int[sizePila];
    topPila = -1;
    
    for(int i = 0; i < sizePila; i++) {
        pilaIDLote[i] = -1;
        pilaResultado[i] = -1;
    }
}

void pushInspeccion(int idLote, int resultado) {
    if(topPila == sizePila - 1) {
        for(int i = 0; i < sizePila - 1; i++) {
            pilaIDLote[i] = pilaIDLote[i + 1];
            pilaResultado[i] = pilaResultado[i + 1];
        }
        topPila--;
    }
    
    topPila++;
    pilaIDLote[topPila] = idLote;
    pilaResultado[topPila] = resultado;
    
    cout << "Inspeccion guardada - Lote " << idLote 
         << " resultado " << (resultado ? "OK" : "Falla") << endl;
}

bool popInspeccion() {
    if(topPila == -1) {
        cout << "No hay inspecciones guardadas" << endl;
        return false;
    }
    
    cout << "Inspeccion eliminada - Lote " << pilaIDLote[topPila]
         << " resultado " << (pilaResultado[topPila] ? "OK" : "Falla") << endl;
    
    pilaIDLote[topPila] = -1;
    pilaResultado[topPila] = -1;
    topPila--;
    
    return true;
}

void reportarFila(int fila) {
    if(fila < 0 || fila >= filas) {
        cout << "Numero de estanteria no valido" << endl;
        return;
    }
    
    cout << "\nEstanteria " << fila << ":" << endl;
    
    bool tieneProductos = false;
    for(int col = 0; col < columnas; col++) {
        cout << "  [" << col << "] ";
        
        if(almacen[fila][col].idLote > 0) {
            LoteProduccion lote = almacen[fila][col];
            cout << lote.nombreComponente << " (ID:" << lote.idLote 
                 << ", Cantidad:" << lote.cantidadTotal << ")" << endl;
            tieneProductos = true;
        } else {
            cout << "vacio" << endl;
        }
    }
    
    if(!tieneProductos) {
        cout << "Esta estanteria esta vacia" << endl;
    }
}

void buscarComponente(const char* nombre) {
    cout << "\nBuscando: " << nombre << endl;
    
    bool encontrado = false;
    int totalCantidad = 0;
    
    for(int f = 0; f < filas; f++) {
        for(int c = 0; c < columnas; c++) {
            if(almacen[f][c].idLote > 0) {
                if(strcmp(almacen[f][c].nombreComponente, nombre) == 0) {
                    cout << "Encontrado en estanteria " << f << " posicion " << c 
                         << " (ID:" << almacen[f][c].idLote << ", Cantidad:" 
                         << almacen[f][c].cantidadTotal << ")" << endl;
                    totalCantidad += almacen[f][c].cantidadTotal;
                    encontrado = true;
                }
            }
        }
    }
    
    if(encontrado) {
        cout << "Total disponible: " << totalCantidad << " unidades" << endl;
    } else {
        cout << "No se encontro ese componente" << endl;
    }
}
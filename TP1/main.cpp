#include <iostream>
#include <fstream>
#include "listas.hpp"
#include "rwstring.hpp"
using namespace std;

struct Registro{
    int nroDock;
    string producto;
    string provincia;
    int cantidad;
};

struct Provincia{ //Nivel 3
    string provincia;
    int cantidad;
};

struct Producto{ //Nivel 2
    string producto;
    int cantidad = 0;
    Nodo<Provincia>* listaPronvicias = nullptr;
};

struct Dock{ //Nivel 1
    int nroDock;
    int cantidadDespachos = 0;
    Nodo<Producto>* listaProductos = nullptr;
};

const int dimNroDock = 8;
const int lprod = 10;
const int lprov = 19;

ostream& operator << (ostream& os, const Dock& dock){
    os << "NroDock: " << dock.nroDock << " - Cantidad de Despachos: " << dock.cantidadDespachos;
    return os;
};

ostream& operator << (ostream& os, const Producto& prod){
    os << prod.producto << "\t" << prod.cantidad;
    return os;
};

ostream& operator << (ostream& os, const Provincia& prov){
    os << prov.provincia << "\t" << prov.cantidad;
    return os;
};

fstream& operator >> (fstream& fs, Registro& reg){
    fs.read(reinterpret_cast<char *>(&reg.nroDock), sizeof(reg.nroDock));
    reg.producto = readstring(fs, lprod);
    reg.provincia = readstring(fs, lprov);
    fs.read(reinterpret_cast<char *>(&reg.cantidad), sizeof(reg.cantidad));
    return fs;
};

int criterioDock(Dock a, Dock b){
    return a.nroDock - b.nroDock;
};

int criterioProducto(Producto a, Producto b){
    return (a.producto < b.producto) ? -1 : (a.producto > b.producto);
};

int criterioMenor(int a, int b){
    return (a < b) ? 0 : -1;
};

int criterioMayor(int a, int b){
    return (a > b) ? 0 : -1;
};

template <typename T> void borrarSiguiente(Nodo<T>* actual){
    if(actual != nullptr && actual->sig != nullptr){
        Nodo<T>* aux = actual->sig;
        actual->sig = actual->sig->sig;
        delete aux;
    };
};

template <typename T> void consolidar(Nodo<T>* lista){
    while(lista != nullptr){
        while(lista->sig != nullptr && criterioProducto(lista->dato, lista->sig->dato) == 0){
            lista->dato.cantidad += lista->sig->dato.cantidad;
            borrarSiguiente(lista);
        };
        lista = lista->sig;
    };
};

int main(){
    fstream archivo;
    const string ruta = "Datos.bin";
    archivo.open(ruta, ios::in | ios::binary);
    if(!archivo){
        cout << "No se pudo abrir el archivo de datos" << endl;
        return EXIT_FAILURE;
    };
    Nodo<Dock>* lista = nullptr;
    Nodo<Dock>* pnodo;
    Nodo<Producto>* qnodo;
    Registro reg;
    Dock dock;
    Producto prod;
    Provincia prov;
    while(archivo >> reg){
        dock.nroDock = reg.nroDock;
        pnodo = insertar_unico(dock, lista, criterioDock);
        pnodo->dato.cantidadDespachos++;
        
        prod.producto = reg.producto;
        qnodo = insertar_unico(prod, pnodo->dato.listaProductos, criterioProducto);
        qnodo->dato.cantidad += reg.cantidad;
        
        prov.provincia = reg.provincia;
        prov.cantidad = reg.cantidad;
        agregar(qnodo->dato.listaPronvicias, prov);
    };
    archivo.close();

    int dockMinCant = -1;
    int despachosDockMinCant = -1;
    string nomProdMaxCant;
    int cantProdMaxCant = -1;
    Nodo<Provincia>* listaProvinciasProdMaxCant = nullptr;

    while(lista != nullptr){
        cout << "\n" << lista->dato << endl;
        int cantDespachosActual = lista->dato.cantidadDespachos;
        if(despachosDockMinCant < 0){
            despachosDockMinCant = lista->dato.cantidadDespachos;
        };
        if(criterioMenor(cantDespachosActual, despachosDockMinCant) == 0){
            dockMinCant = lista->dato.nroDock;
            despachosDockMinCant = lista->dato.cantidadDespachos;
        };
        Nodo<Producto>* lista2 = lista->dato.listaProductos;
        if(lista2 != nullptr){
            consolidar(lista2);
            mostrar(lista2);
            if(lista->dato.nroDock == dockMinCant){
                while(lista2 != nullptr){
                    if(criterioMayor(lista2->dato.cantidad, cantProdMaxCant) == 0){
                        nomProdMaxCant = lista2->dato.producto;
                        cantProdMaxCant = lista2->dato.cantidad;
                        listaProvinciasProdMaxCant = lista2->dato.listaPronvicias;
                    };
                    lista2 = lista2->sig;
                };
            };
        };
        lista = lista->sig;
    };

    cout << "\nEl dock con menos despachos es el: " << dockMinCant << " con " << despachosDockMinCant << " despachos." << endl;
    cout << "El producto con mayor cantidad despachada en este dock es:" << endl;
    cout << nomProdMaxCant << "\t" << cantProdMaxCant << endl;
    cout << "Listado de despachos" << endl;
    cout << "Provincia\tCantidad" << endl;
    mostrar(listaProvinciasProdMaxCant);

    return 0;
};

#include <iostream>
#include <fstream>
#include "listas.hpp"
#include "rwstring.hpp"

using namespace std;

struct Despacho {
    int nrodock;
    string producto;
    string provincia;
    int cantidad;
};

struct ProductoDock {
    string producto;
    int cantidad;
};

const int dimNroDock = 8;
const int lprod = 10;
const int lprov = 19;

ostream& operator << (ostream& os, const ProductoDock& d){
    os << d.producto << "\t" << d.cantidad << endl;
    return os;
};

fstream& operator << (fstream& fs, const Despacho& d){
    fs.write(reinterpret_cast<const char *>(&d.nrodock), sizeof(d.nrodock));
    writestring(fs, d.producto, lprod);
    writestring(fs, d.provincia, lprov);
    fs.write(reinterpret_cast<const char *>(&d.cantidad), sizeof(d.cantidad));
    return fs;
};

fstream& operator >> (fstream& fs, Despacho& d){
    fs.read(reinterpret_cast<char *>(&d.nrodock), sizeof(d.nrodock));
    d.producto = readstring(fs, lprod);
    d.provincia = readstring(fs, lprov);
    fs.read(reinterpret_cast<char *>(&d.cantidad), sizeof(d.cantidad));
    return fs;
};
/*
int criterioNroDock(Despacho a, Despacho b){
    return a.nrodock - b.nrodock;
};
*/
int criterioNroDockProducto(Despacho a, Despacho b){
    if(a.nrodock == b.nrodock){
        return (a.producto < b.producto) ? -1 : (a.producto > b.producto);
    } else {
        return a.nrodock - b.nrodock;
    }
};

int criterioProducto(ProductoDock a, ProductoDock b){
    return (a.producto < b.producto) ? -1 : (a.producto > b.producto);
};

template <typename T> void borrarSiguiente(Nodo<T>* actual){
    if(actual != nullptr && actual->sig != nullptr){
        Nodo<T>* aux = actual->sig;
        actual->sig = actual->sig->sig;
        delete aux;
    }
};

template <typename T> void consolidarCantidadDespachos(Nodo<T>* lista){
    while(lista != nullptr){
        while(lista->sig != nullptr && criterioNroDockProducto(lista->dato, lista->sig->dato) == 0){
            lista->dato.cantidad += lista->sig->dato.cantidad;
            borrarSiguiente(lista);
        };
        lista = lista->sig;
    };
};

int main(){
    Nodo<Despacho>* lista = nullptr;
    //Nodo<ProductoDock>* productosDock[8];
    Despacho despacho;
    fstream archivo;

    archivo.open("Archivos/Datos.bin", ios::in | ios::binary);
    if(!archivo){
        cout << "No se pudo abrir el archivo 'Datos.bin'" << endl;
        return EXIT_FAILURE;
    };
    while(archivo >> despacho){
        insertar(despacho, lista, criterioNroDockProducto);
    };
    archivo.close();

    Nodo<Despacho>* lista2 = nullptr;
    cout << "Mostrando la lista de despachos ordenado por NroDock-Producto:\n" << endl;
    consolidarCantidadDespachos(lista);
    int contador = 0;

    for(int i = 0; i < dimNroDock; i++){
        cout << "NroDock: " << i << " - Cantidad de despachos: " << endl;
        Nodo<ProductoDock>* lista2 = nullptr;
        Nodo<Despacho>* aux = lista;

        while(aux != nullptr){
            if(aux->dato.nrodock == i){
                ProductoDock prodock;
                prodock.producto = aux->dato.producto;
                prodock.cantidad = aux->dato.cantidad;
                insertar(prodock, lista2, criterioProducto);
            };
            aux = aux->sig;
        };
        mostrar(lista2);
    };

    //mostrar(lista);
    //Los productos ya tienen sus cantidades consolidadas.
    //Falta listar de forma correcta el listado de cada dock (supongo que es con un vector) y poner un contador para cada despacho que se hace.
    return 0;
};
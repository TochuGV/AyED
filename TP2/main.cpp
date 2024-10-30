#include <iostream>
#include <fstream>
#include "listas.hpp"
#include "rwstring.hpp"
using namespace std;

struct Lote{
    string producto;
    string lote;
    int cantidad;
};

struct ProductoLote{
    string producto;
    int cantidad;
};

const int lprod = 10;
const int llote = 6;

ostream& operator << (ostream& os, const Lote& l){
    os << l.producto << "\t" << l.cantidad << endl;
    return os;
};

ostream& operator << (ostream& os, const ProductoLote& pl){
    os << pl.producto << "\t" << pl.cantidad << endl;
    return os;
};


fstream& operator << (fstream& fs, const Lote& l){
    writestring(fs, l.producto, lprod);
    writestring(fs, l.lote, llote);
    fs.write(reinterpret_cast<const char *>(&l.cantidad), sizeof(l.cantidad));
    return fs;
};

fstream& operator >> (fstream& fs, Lote& l){
    l.producto = readstring(fs, lprod);
    l.lote = readstring(fs, llote);
    fs.read(reinterpret_cast<char *>(&l.cantidad), sizeof(l.cantidad));
    return fs;
};

template <typename T> void borrarSiguiente(Nodo<T>* actual){
    if(actual != nullptr && actual->sig != nullptr){
        Nodo<T>* aux = actual->sig;
        actual->sig = actual->sig->sig;
        delete aux;
    }
};

template <typename T> void consolidarCantidadProductos(Nodo<T>* lista){
    while(lista != nullptr){
        while(lista->sig != nullptr && criterioLote(lista->dato, lista->sig->dato) == 0){
            lista->dato.cantidad += lista->sig->dato.cantidad;
            borrarSiguiente(lista);
        };
        lista = lista->sig;
    };
};

int criterioLote(Lote a, Lote b){
    return (a.producto < b.producto) ? -1 : (a.producto > b.producto);
};

int criterioProductoLote(ProductoLote a, ProductoLote b){
    return (a.producto < b.producto) ? -1 : (a.producto > b.producto);
};
/*
void ingresarPedido(Lote l){
    cout << "Ingrese un nuevo pedido:" << endl;
    cout << "Producto: ";
    if(cin >> l.producto){
        cout << "Cantidad: ";
    };
    while(cin >> l.cantidad){
        cout << "Producto: ";
        if(cin >> l.producto){
            cout << "Cantidad: ";
        };
    };
};
*/
int main(){
    Nodo<Lote>* listaLotes = nullptr;
    Lote lote;
    ProductoLote prodlote;
    fstream archivo;
    const string ruta = "C:/Users/Tochu/Desktop/AyED - TP2/Archivos/Datos.bin";

    //Punto 2

    archivo.open(ruta, ios::in | ios::binary);
    if(!archivo){
        cout << "No se pudo encontrar el archivo Datos.bin" << endl;
        return EXIT_FAILURE;
    };
    while(archivo >> lote){
        insertar(lote, listaLotes, criterioLote);
    };
    archivo.close();

    //Punto 3

    cout << "Producto\tCantidad" << endl;
    consolidarCantidadProductos(listaLotes);
    mostrar(listaLotes);

    //Punto 4

    Nodo<ProductoLote>* listaPedidos = nullptr;
    cout << "Ingrese un nuevo pedido:" << endl;
    cout << "Producto: ";
    if(cin >> prodlote.producto){
        cout << "Cantidad: ";
    };
    while(cin >> prodlote.cantidad){
        insertar(prodlote, listaPedidos, criterioProductoLote);
        cout << "Producto: ";
        if(cin >> prodlote.producto){
            cout << "Cantidad: ";
        };
    };
    
    mostrar(listaPedidos);
    Nodo<Lote>* aux = listaLotes;

//Se muestra solo el último objeto de la lista.

    while(listaLotes != nullptr){
        if(listaLotes->dato.producto == listaPedidos->dato.producto){
            if(listaLotes->dato.cantidad >= listaPedidos->dato.cantidad){
                listaLotes->dato.cantidad =- listaPedidos->dato.cantidad;
                cout << "Pedido despachado:" << endl;
                cout << "Producto: " << prodlote.producto << " - Cantidad: " << prodlote.cantidad << " - Detalle de los lotes:" << endl;
                //if(listaLotes->dato.lote)
            } else {
                int cantidadRestante = listaPedidos->dato.cantidad - listaLotes->dato.cantidad;
                cout << "Pedido rechazado. Listado de faltantes:" << endl;
                cout << listaPedidos->dato.producto << "\t" << cantidadRestante << endl;
                
            };
            listaPedidos = listaPedidos->sig;
        };
        listaLotes = listaLotes->sig;
    };

    return 0;
}
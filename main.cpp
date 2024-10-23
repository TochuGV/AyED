#include <iostream>
#include <fstream>
#include "listas.hpp"
#include "rwstring.hpp"

using namespace std;

struct Despacho {
    int nroDock;
    string producto;
    string provincia;
    int cantidad;
};

struct ProductoDock {
    string producto;
    int cantidad;
};

struct ProvinciaProducto {
    string provincia;
    int cantidad;
};

const int dimNroDock = 8;
const int lprod = 10;
const int lprov = 19;

ostream& operator << (ostream& os, const ProductoDock& d){
    os << d.producto << "\t" << d.cantidad << endl;
    return os;
};

ostream& operator << (ostream& os, const ProvinciaProducto& d){
    os << d.provincia << "\t" << d.cantidad << endl;
    return os;
};

fstream& operator << (fstream& fs, const Despacho& d){
    fs.write(reinterpret_cast<const char *>(&d.nroDock), sizeof(d.nroDock));
    writestring(fs, d.producto, lprod);
    writestring(fs, d.provincia, lprov);
    fs.write(reinterpret_cast<const char *>(&d.cantidad), sizeof(d.cantidad));
    return fs;
};

fstream& operator >> (fstream& fs, Despacho& d){
    fs.read(reinterpret_cast<char *>(&d.nroDock), sizeof(d.nroDock));
    d.producto = readstring(fs, lprod);
    d.provincia = readstring(fs, lprov);
    fs.read(reinterpret_cast<char *>(&d.cantidad), sizeof(d.cantidad));
    return fs;
};

int criterioNroDockProducto(Despacho a, Despacho b){
    if(a.nroDock == b.nroDock){
        return (a.producto < b.producto) ? -1 : (a.producto > b.producto);
    } else {
        return a.nroDock - b.nroDock;
    }
};

int criterioProducto(ProductoDock a, ProductoDock b){
    return (a.producto < b.producto) ? -1 : (a.producto > b.producto);
};

int criterioMayor(int a, int b){
    return (a > b) ? 0 : -1;
};

int criterioMenor(int a, int b){
    return (a < b) ? 0 : -1;
}; //Se podría haber planteado una única función (para criterioMayor y criterioMenor) en la que se resten los dos parámetros y en los 'if' pedir que cumpla si es mayor o menor que 0.

int criterioProvinciaProducto(ProvinciaProducto a, ProvinciaProducto b){
    return 0; //Hay que listarlos como están en el archivo original.
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
        while(lista->sig != nullptr && criterioProducto(lista->dato, lista->sig->dato) == 0){
            lista->dato.cantidad += lista->sig->dato.cantidad;
            borrarSiguiente(lista);
        };
        lista = lista->sig;
    };
};

int main(){
    Nodo<Despacho>* listaDespachos = nullptr;
    Despacho despacho;
    ProductoDock prodock;
    fstream archivo;
    const string ruta = "Archivos/Datos.bin";

    archivo.open(ruta, ios::in | ios::binary);
    if(!archivo){
        cout << "No se pudo abrir el archivo 'Datos.bin'" << endl;
        return EXIT_FAILURE;
    };
    while(archivo >> despacho){
        insertar(despacho, listaDespachos, criterioNroDockProducto);
    };
    archivo.close();
    
    for(int i = 0; i < dimNroDock; i++){
        int contador = 0;
        Nodo<Despacho>* aux = listaDespachos;
        Nodo<ProductoDock>* listaProductos = nullptr;

        while(aux != nullptr){
            if(aux->dato.nroDock == i){
                prodock.producto = aux->dato.producto;
                prodock.cantidad = aux->dato.cantidad;
                insertar(prodock, listaProductos, criterioProducto);
                contador++;
            };
            aux = aux->sig;
        };
        cout << "NroDock: " << i << " - Cantidad de despachos: " << contador << endl;
        consolidarCantidadDespachos(listaProductos);
        mostrar(listaProductos);
    };

    int minDespachos = INT_MAX;
    int dockMinimo = -1;
    int clave = 0;

    for(int i = 0; i < dimNroDock; i++){
        int totalDespachos = 0;
        Nodo<Despacho>* aux = listaDespachos;

        while(aux != nullptr){
            if(aux->dato.nroDock == i){
                totalDespachos++;
            };
            aux = aux->sig;
        };
        if(criterioMenor(totalDespachos, minDespachos) == 0){
            minDespachos = totalDespachos;
            dockMinimo = i;
        };
    };

    Nodo<Despacho>* aux = listaDespachos;
    Nodo<ProductoDock>* listaProductos = nullptr;

    while(aux != nullptr){
        if(aux->dato.nroDock == dockMinimo){
            prodock.producto = aux->dato.producto;
            prodock.cantidad = aux->dato.cantidad;
            insertar(prodock, listaProductos, criterioProducto);
        };
        aux = aux->sig;
    };

    consolidarCantidadDespachos(listaProductos);
    Nodo<ProductoDock>* aux2 = listaProductos;
    Nodo<ProductoDock>* productoMayorCantidad = nullptr;
    int minCantidadDespachada = INT_MIN;
    
    while(aux2 != nullptr){
        int cantidadDespachada = aux2->dato.cantidad;
        if(criterioMayor(cantidadDespachada, minCantidadDespachada) == 0){
            minCantidadDespachada = cantidadDespachada;
            prodock.producto = aux2->dato.producto;
            prodock.cantidad = aux2->dato.cantidad;
        };
        aux2 = aux2->sig;
    };
    insertar(prodock, productoMayorCantidad, criterioProducto);
    cout << "El dock con menos despachos es el: " << dockMinimo << " con " << minDespachos << " despachos." << endl;
    cout << "El producto con mayor cantidad despachada en este dock es:" << endl;
    mostrar(productoMayorCantidad);

    Nodo<ProvinciaProducto>* listaProvincias = nullptr;
    ProvinciaProducto provprod;

    while(listaDespachos != nullptr){
        if(listaDespachos->dato.nroDock == dockMinimo && listaDespachos->dato.producto == productoMayorCantidad->dato.producto){
            provprod.provincia = listaDespachos->dato.provincia;
            provprod.cantidad = listaDespachos->dato.cantidad;
            insertar(provprod, listaProvincias, criterioProvinciaProducto);
        };
        listaDespachos = listaDespachos->sig;
    };
    cout << "Lista de despachos" << endl;
    cout << "Provincia\tCantidad" << endl;
    mostrar(listaProvincias);
    return 0;
};

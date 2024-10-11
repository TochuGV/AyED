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

int criterioMayor(int a, int b){
    return (a > b) ? 0 : -1;
};

int criterioMenor(int a, int b){
    return (a < b) ? 0 : -1;
}; //Se podría haber planteado una única función en la que se resten los dos parámetros y en los 'if' pedir que cumpla si es mayor o menor que 0.

int criterioProvinciaProducto(ProvinciaProducto a, ProvinciaProducto b){
    return 0; //Hay que listarlos como están en el archivo original.
}

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
    Nodo<Despacho>* lista = nullptr;
    Despacho despacho;
    fstream archivo;

    archivo.open("Datos.bin", ios::in | ios::binary);
    if(!archivo){
        cout << "No se pudo abrir el archivo 'Datos.bin'" << endl;
        return EXIT_FAILURE;
    };
    while(archivo >> despacho){
        insertar(despacho, lista, criterioNroDockProducto);
    };
    archivo.close();
    
    for(int i = 0; i < dimNroDock; i++){
        int contador = 0;
        Nodo<ProductoDock>* lista2 = nullptr;
        Nodo<Despacho>* aux = lista;

        while(aux != nullptr){
            if(aux->dato.nrodock == i){
                ProductoDock prodock;
                prodock.producto = aux->dato.producto;
                prodock.cantidad = aux->dato.cantidad;
                insertar(prodock, lista2, criterioProducto);
                contador++;
            };
            aux = aux->sig;
        };
        cout << "NroDock: " << i << " - Cantidad de despachos: " << contador << endl;
        consolidarCantidadDespachos(lista2);
        mostrar(lista2);
    };

    int minDespachos = INT_MAX;
    int dockMinimo = -1;

    for(int i = 0; i < dimNroDock; i++){
        int totalDespachos = 0;
        Nodo<Despacho>* aux = lista;

        while(aux != nullptr){
            if(aux->dato.nrodock == i){
                totalDespachos++;
            };
            aux = aux->sig;
        };

        if(criterioMenor(totalDespachos, minDespachos) == 0){
            minDespachos = totalDespachos;
            dockMinimo = i;
        };
    };
    cout << "El dock con menos despachos es el: " << i << " con " << minDespachos << " despachos." << endl;

    int mayorCantidad = -1;
    string productoMayorCantidad;
    Nodo<Despachos>* aux = lista;
    Nodo<ProvinciaProducto>* listaProvincias = nullptr;

    while(aux != nullptr){
        if(aux->dato.nrodock == dockMinimo && criterioMayor(aux->dato.cantidad, mayorCantidad) == 0){
            mayorCantidad = aux->dato.cantidad;
            productoMayorCantidad = aux->dato.producto;

            ProvinciaProducto provprod;
            provprod.provincia = aux->dato.provincia;
            provprod.cantidad = aux->dato.cantidad;
            insertar(provprod, listaProvincias, criterioProvinciaProducto); //No es que haya un criterio como tal, hay que listarlos como vienen en el archivo original.
        };
        aux = aux->sig;
    };
    cout << "El producto con mayor cantidad despachada en este dock es:" << endl;
    cout << productoMayorCantidad << "\t" << mayorCantidad << endl; //Se pudo haber agregado a una lista y que la muestre, pero no nos parecía útil para solamente mostrar un registro.
    cout << "Lista de despachos" << endl;
    mostrar(listaProvincias);
    return 0;
};

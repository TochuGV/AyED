#include <iostream>
#include <fstream>
#include "listas.hpp"
#include "rwstring.hpp"
using namespace std;

struct Registro{ //Registro
    string producto;
    string lote;
    int cantidad;
};

struct LoteCantidad{ //Nivel 2
    string lote;
    int cantidad;
};

struct ProductoCantidad{ //Nivel 1
    string producto;
    int cantidad;
    Nodo<LoteCantidad>* lista = nullptr;
};

const int lprod = 10;
const int llote = 6;

ostream& operator << (ostream& os, const ProductoCantidad& pc){
    os << pc.producto << "\t" << pc.cantidad << endl;
    return os;
};

ostream& operator << (ostream& os, const LoteCantidad& lc){
    os << lc.lote << "\t" << lc.cantidad << endl;
    return os;
};

fstream& operator << (fstream& fs, Registro& reg){
    reg.producto = readstring(fs, lprod);
    reg.lote = readstring(fs, llote);
    fs.read(reinterpret_cast<char *>(&reg.cantidad), sizeof(reg.cantidad));
    return fs;
};

int criterioProductoCantidad(ProductoCantidad a, ProductoCantidad b){
    return a.producto.compare(b.producto);
};

int criterioLoteCantidad(LoteCantidad a, LoteCantidad b){
    return b.lote.compare(a.lote);
};

int main(){
    fstream archivo;
    const string ruta = "Datos.bin";

    //Punto 2

    archivo.open(ruta, ios::in | ios::binary);
    if(!archivo){
        cout << "No se pudo abrir el archivo 'Datos.bin'" << endl;
        return EXIT_FAILURE;
    };
    Nodo<ProductoCantidad>* lista = nullptr;
    Nodo<ProductoCantidad>* pnodo;
    Registro reg;
    ProductoCantidad pc;
    LoteCantidad lc;
    while(archivo << reg){
        pc.producto = reg.producto;
        pnodo = insertar_unico(pc, lista, criterioProductoCantidad);
        pnodo->dato.cantidad += reg.cantidad;
        
        lc.lote = reg.lote;
        lc.cantidad = reg.cantidad;
        insertar(lc, pnodo->dato.lista, criterioLoteCantidad);
    };
    archivo.close();

    //Punto 3

    cout << "Stock Inicial" << endl;
    cout << "Producto\tCantidad" << endl;
    mostrar(lista);

    //Punto 4

    Nodo<ProductoCantidad>* listaPedidos = nullptr;
    cout << "Ingrese un nuevo pedido:" << endl;
    cout << "Producto: ";
    if(cin >> pc.producto){
        cout << "Cantidad: ";
    };
    while(cin >> pc.cantidad){
        agregar(listaPedidos, pc);
        cout << "Producto: ";
        if(cin >> pc.producto){
            cout << "Cantidad: ";
        };
    };

    mostrar(listaPedidos); //Después lo borro.
    Nodo<LoteCantidad>* listaStockDespachado = nullptr;

    while(listaPedidos != nullptr){
        while(lista != nullptr){
            if(listaPedidos->dato.producto == lista->dato.producto){
                int cantidadLotes = 0;
                if(lista->dato.cantidad >= listaPedidos->dato.cantidad){
                    lista->dato.cantidad -= listaPedidos->dato.cantidad;
                    while(cantidadLotes < listaPedidos->dato.cantidad && lista->dato.lista != nullptr){
                        if(lista->dato.lista->sig != nullptr){
                            cantidadLotes += lista->dato.lista->dato.cantidad;
                            lc.lote = lista->dato.lista->dato.lote;
                            lc.cantidad = lista->dato.lista->dato.cantidad;
                            if(cantidadLotes > listaPedidos->dato.cantidad){
                                lc.lote = lista->dato.lista->dato.lote;
                                lc.cantidad = listaPedidos->dato.cantidad - (cantidadLotes - lista->dato.lista->dato.cantidad);
                            };
                            agregar(listaStockDespachado, lc);
                            lista->dato.lista = lista->dato.lista->sig;
                        };
                    };
                    cout << "Pedido despachado" << endl;
                    cout << "Producto: " << listaPedidos->dato.producto << " - Cantidad total: " << listaPedidos->dato.cantidad << " - Detalle de los lotes:" << endl;
                    mostrar(listaStockDespachado);
                } else {
                    int cantidadFaltante = listaPedidos->dato.cantidad - lista->dato.cantidad;
                    cout << "Pedido rechazado. Listado de faltantes" << endl;
                    cout << listaPedidos->dato.producto << "\t" << cantidadFaltante << endl;
                };
            };
            lista = lista->sig;
        };
        listaPedidos = listaPedidos->sig;
    };

    return 0; //POR AHORA SOLO FUNCIONA PARA EL PRIMER REGISTRO DEL PRIMER PEDIDO.
}

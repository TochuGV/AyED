#include <iostream>
#include <fstream>
#include "listas.hpp"
#include "rwstring.hpp"
using namespace std;

struct Registro{
    string producto;
    string lote;
    int cantidad;
};

struct Lote{ //Nivel 2
    string lote;
    int cantidad;
};

struct Producto{ //Nivel 1
    string producto;
    int cantidad = 0;
    Nodo<Lote>* listaLotes = nullptr;
};

const int lprod = 10;
const int llote = 6;

ostream& operator << (ostream& os, const Producto& pc){
    os << pc.producto << "\t" << pc.cantidad;
    return os;
};

ostream& operator << (ostream& os, const Lote& lc){
    os << lc.lote << "\t" << lc.cantidad << endl;
    return os;
};

fstream& operator >> (fstream& fs, Registro& reg){
    reg.producto = readstring(fs, lprod);
    reg.lote = readstring(fs, llote);
    fs.read(reinterpret_cast<char *>(&reg.cantidad), sizeof(reg.cantidad));
    return fs;
};

int criterioProducto(Producto a, Producto b){
    return a.producto.compare(b.producto);
};

int criterioLote(Lote a, Lote b){
    return b.lote.compare(a.lote);
};

int criterioMenor(int a, int b){
    return b-a;
};

void procesarDespachos(Nodo<Producto>*& lista, Nodo<Producto>*& listaPedidos){
    Nodo<Producto>* aux = listaPedidos;
    Nodo<Producto>* listaFaltantes = nullptr;
    Nodo<Producto>* pnodo;

    while(aux != nullptr){
        pnodo = buscar(aux->dato, lista, criterioProducto);
        if(pnodo == nullptr || criterioMenor(aux->dato.cantidad, pnodo->dato.cantidad) < 0){
            Producto pc;
            pc.producto = aux->dato.producto;
            pc.cantidad = (pnodo == nullptr) ? aux->dato.cantidad : (aux->dato.cantidad - pnodo->dato.cantidad);
            agregar(listaFaltantes, pc);
        };
        aux = aux->sig;
    };
    
    if(listaFaltantes != nullptr){
        cout << "\nPedido rechazado. Listado de faltantes:" << endl;
        mostrar(listaFaltantes);
        return;
    } else {
        if(listaPedidos != nullptr){
            cout << "\nPedido despachado" << endl;
        };
    };
    
    while(listaPedidos != nullptr){
        pnodo = buscar(listaPedidos->dato, lista, criterioProducto);
        cout << "Producto: " << listaPedidos->dato.producto << " - Cantidad total: " << listaPedidos->dato.cantidad << " - Detalle de los lotes:" << endl;
        Nodo<Lote>* nodolc = pnodo->dato.listaLotes;
        int cantidadPedida = listaPedidos->dato.cantidad;
        while(nodolc != nullptr && cantidadPedida > 0){
            int cantidadUsada = (criterioMenor(nodolc->dato.cantidad, cantidadPedida) < 0) ? cantidadPedida : nodolc->dato.cantidad;
            cout << nodolc->dato.lote << "\t" << cantidadUsada << endl;
            nodolc->dato.cantidad -= cantidadUsada;
            cantidadPedida -= cantidadUsada;
            if(nodolc->dato.cantidad == 0){
                Nodo<Lote>* aux = nodolc;
                nodolc = nodolc->sig;
                borrar(aux->dato, pnodo->dato.listaLotes, criterioLote);
            } else {
                nodolc = nodolc->sig;
            };
        };
        pnodo->dato.cantidad -= listaPedidos->dato.cantidad;
        listaPedidos = listaPedidos->sig;
    };
};

void realizarDespachos(Nodo<Producto>*& lista){
    while(true){
        Nodo<Producto>* listaPedidos = nullptr;
        Producto pc;

        cout << "\nIngrese un nuevo pedido:" << endl;
        cout << "Producto: ";
        if(cin >> pc.producto){
            if(pc.producto == "Stock"){
                string siguiente;
                cin >> siguiente;
                if(siguiente == "Final"){
                    return;
                };
            };
            cout << "Cantidad: ";
        };
        while(cin >> pc.cantidad){
            agregar(listaPedidos, pc);
            cout << "Producto: ";
            if(cin >> pc.producto){
                cout << "Cantidad: ";
            };
        };

        procesarDespachos(lista, listaPedidos);
        cin.clear();
    };
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
    Nodo<Producto>* lista = nullptr;
    Nodo<Producto>* pnodo;
    Producto pc;
    Lote lc;
    Registro reg;
    while(archivo >> reg){
        pc.producto = reg.producto;
        pnodo = insertar_unico(pc, lista, criterioProducto);
        pnodo->dato.cantidad += reg.cantidad;
        
        lc.lote = reg.lote;
        lc.cantidad = reg.cantidad;
        insertar(lc, pnodo->dato.listaLotes, criterioLote);
    };
    archivo.close();

    //Punto 3

    cout << "Stock Inicial" << endl;
    cout << "Producto\tCantidad" << endl;
    mostrar(lista);

    //Punto 4

    realizarDespachos(lista);

    //Punto 5
    cout << "Producto\tCantidad" << endl;
    mostrar(lista);

    return 0;
};

#include <iostream>
using namespace std;
#include <fstream>
#include "listas.hpp"

struct Despacho {

int NroDock; // Nro del dock , entero de 4 bytes
char Producto [10];// producto, string de 10 caracteres
char Provincia[19];// provincia, string de 19 caracteres
int cantidad; // cantidad despachada , entero de 4 bytes

};

int main(){

ifstream archivo;
Despacho despacho;
archivo.open ("Datos.bin",ios::binary);
if(!archivo){
cout<<" error al abrir el archivo "<<endl;
return EXIT_FAILURE;
};
while (archivo >> despacho) {
		;
	}
	archivo.close();

return 0;
}
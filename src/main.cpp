//librerias
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
//configuracion de la liga
struct ConfigLiga {
    string ligapato;
    int ptsVictoria;
    int ptsEmpate;
    int ptsDerrota;
    vector<string> equipos;
};
//objetos/caracteristicas de cada partido
struct Partido {
    string fecha;
    string local;
    string visitante;
    int golesLocal;
    int golesVisitante;
};
// estadisticas de cada equipo pj=partidosJugados pg=partidosGanados pe=partidosempatados pp=partidosPerdidos
//gf=goles a favor gc= goles en contra dg= diferencia de gol pts= puntos del equipo respectivo
struct Equipo {
    string nombre;
    int pj,pg,pe,pp;
    int gf,gc,dg,pts;
};
// creacion de los equipos
Equipo crearEquipo(string nombre){
    Equipo e;
    //datos de cada equipo 
    e.nombre = nombre;
    e.pj =0; e.pg =0; e.pe =0; e.pp =0;
    e.gf =0; e.gc =0; e.dg =0; e.pts =0;
    return e;
}
//leer el config.txt y guardar los datos de la liga
bool leerConfig(string nombreArchivo, ConfigLiga &config){
    ifstream archivo;
    archivo.open(nombreArchivo);
    //verificar si abrio el archivo
    if(!archivo.is_open()){
        cout << "no se encontro el archivo " << nombreArchivo << endl;
        return false;
    }
    string linea;
    while(getline(archivo,linea)){
        // ver las lineas para la informacion de la liga ignorando las vacias o comoentarios
        if(linea.empty() || linea[0] == '#') continue;
        stringstream ss(linea);
        string clave, valor;
        getline(ss, clave, '=');
        getline(ss, valor);
        // meter la info a la configuracion de la liga 
        if(clave == "nombre")   config.ligapato = valor;
        if(clave == "victoria") config.ptsVictoria = stoi(valor);
        if(clave == "empate")   config.ptsEmpate = stoi(valor);
        if(clave == "derrota")  config.ptsDerrota = stoi(valor);
        if(clave == "equipo")   config.equipos.push_back(valor);
    }
    //cerrar el archivo 
    archivo.close();
    return true;
}
// guardar el partido en partidos.txt
void guardarPartido(Partido p){
    // sobreescribir en partidos.txt el partido a registrar
    ofstream archivo("data/partidos.txt", ios::app);
    //verificar si se pudo abrir el archivo
    if(!archivo.is_open()){
        cout << "error abriendo partidos.txt" << endl;
        return;
    }
    // partidos.txt recibe la respectiva informacion 
    archivo << p.fecha    << ";"
            << p.local    << ";"
            << p.visitante << ";"
            << p.golesLocal   << ";"
            << p.golesVisitante   << endl;
    //cerrar el archivo
    archivo.close();
}
// guardo la jornada en fechas.txt
void guardarJornada(int numJornada, string fecha, vector<Partido> partidos){
    // sobreescribir en fechas.txt la jornada a registrar
    ofstream archivo("data/fechas.txt", ios::app);
    //verificar si se pudo abrir el archivo
    if(!archivo.is_open()){
        cout << "error abriendo fechas.txt" << endl;
        return;
    }
    // fechas.txt recibe la respectiva informacion de la jornada
    archivo << "JORNADA=" << numJornada << endl;
    archivo << "fecha=" << fecha << endl;
    for(int i = 0; i < partidos.size(); i++){
        archivo << partidos[i].local     << ";"
                << partidos[i].visitante << ";"
                << partidos[i].golesLocal    << ";"
                << partidos[i].golesVisitante    << endl;
    }
    //marcar el fin de la jornada
    archivo << "FIN_JORNADA" << endl;
    archivo.close();
}
// leer todos los partidos del archivo
vector<Partido> leerPartidos(){
    vector<Partido> partidos;
    // leer partidos.txt
    ifstream archivo("data/partidos.txt");
    //verificar si se pudo abrir el archivo
    if(!archivo.is_open()){
        cout << "error abriendo partidos.txt" << endl;
        return partidos;
    }
    //leer cada linea para sacar la info 
    string linea;
    while(getline(archivo, linea)){
        if(linea.empty()) continue;
        stringstream ss(linea);
        string goles;
        Partido p;
        getline(ss, p.fecha,      ';');
        getline(ss, p.local,      ';');
        getline(ss, p.visitante,  ';');
        getline(ss, goles, ';');
        p.golesLocal = stoi(goles);
        getline(ss, goles);
        p.golesVisitante = stoi(goles);
        //regresa el partido ya armado con toda su info
        partidos.push_back(p);
    }
    //cerrar el archivo
    archivo.close();
    return partidos;
}
// actualiza stats del equipo segun el resultado
void actualizarEquipo(Equipo *e, int golesFavor, int golesContra, ConfigLiga config){
    // sumar a las stats del equipo sus datos 
    e->pj++;
    e->gf += golesFavor;
    e->gc += golesContra;
    e->dg = e->gf - e->gc;
    // verifico si gano, empato o perdio
    if(golesFavor > golesContra){
        e->pg++;
        e->pts += config.ptsVictoria;
    } else if(golesFavor == golesContra){
        e->pe++;
        e->pts += config.ptsEmpate;
    } else {
        e->pp++;
        e->pts += config.ptsDerrota;
    }
}
// funcion de construir la tabla 
vector<Equipo> construirTabla(ConfigLiga config){
    vector<Equipo> tabla;
    for(int i = 0; i < config.equipos.size(); i++){
        tabla.push_back(crearEquipo(config.equipos[i]));
    }
    //leer los partidos para actualizar las stats de los equipos y con ello la tabla 
    vector<Partido> partidos = leerPartidos();
    //recorrer cada partido 
    for(int i = 0; i < partidos.size(); i++){
        Partido p = partidos[i];
        // buscar el equipo local y actualizo
        for(int j = 0; j < tabla.size(); j++){
            if(tabla[j].nombre == p.local){
                actualizarEquipo(&tabla[j], p.golesLocal, p.golesVisitante, config);
            }
        }
        // buscar el equipo visitante y actualizo
        for(int j = 0; j < tabla.size(); j++){
            if(tabla[j].nombre == p.visitante){
                actualizarEquipo(&tabla[j], p.golesVisitante, p.golesLocal, config);
            }
        }
    }
    // ordenar la tabla por puntos de mayor a menor y demas criterios 
    for(int i = 0; i < tabla.size()-1; i++){
        for(int j = i+1; j < tabla.size(); j++){
            //funcionalidad de desempate: primero por puntos,luego diferencia de gol y goles a favor
            if(tabla[j].pts > tabla[i].pts || (tabla[j].pts == tabla[i].pts && tabla[j].dg > tabla[i].dg) || (tabla[j].pts == tabla[i].pts && tabla[j].dg == tabla[i].dg && tabla[j].gf > tabla[i].gf)){
                Equipo temp = tabla[i];
                tabla[i] = tabla[j];
                tabla[j] = temp;
            }
        }
    }
    return tabla;
}
// muestreo de la tabla en pantalla
void mostrarTabla(vector<Equipo> tabla){
    cout << "\n== Tabla de Posiciones ==" << endl;
    cout << "#  Equipo              PJ PG PE PP GF GC DG PTS" << endl;
    cout << "-----------------------------------------------" << endl;
    //mostrar cada equipo con sus stats en la tabla de posiciones
    //recorriendo cada equipo 
    for(int i = 0; i < tabla.size(); i++){
        Equipo e = tabla[i];
        cout << i+1 << ". ";
        cout << e.nombre;
        //(espacios para alinear la tabla)
        int espacios = 20 - e.nombre.size();
        for(int s = 0; s < espacios; s++) cout << " ";
        //imorimir las stats del equipo respectivo
        cout << e.pj << "  "
             << e.pg << "  "
             << e.pe << "  "
             << e.pp << "  "
             << e.gf << "  "
             << e.gc << "  "
             << e.dg << "  "
             << e.pts << endl;
    }
    cout << "-----------------------------------------------" << endl;
    //funcionalidad de guardar la tabla en un archivo de texto
    int guardar;
    cout << "guardar en tabla.txt? (1=si / 0=no): ";
    cin >> guardar;
    if(guardar == 1){
        ofstream archivo("data/tabla.txt");
        archivo << "Tabla de Posiciones - " << endl;
        archivo << "#  Equipo              PJ PG PE PP GF GC DG PTS" << endl;
        for(int i = 0; i < tabla.size(); i++){
            Equipo e = tabla[i];
            archivo << i+1 << ". " << e.nombre << "  "
                    << e.pj << " " << e.pg << " "
                    << e.pe << " " << e.pp << " "
                    << e.gf << " " << e.gc << " "
                    << e.dg << " " << e.pts << endl;
        }
        //cerrar el archivo
        archivo.close();
        cout << "listo, tabla guardada" << endl;
    }
}
// muestreo del historial de jornadas
void mostrarHistorial(){
    //leer las fechas 
    ifstream archivo("data/fechas.txt");
    //verificar si se pudo abrir el archivo
    if(!archivo.is_open()){
        cout << "no se pudo abrir fechas.txt" << endl;
        return;
    }
    string linea;
    bool dentroJornada = false;
    //recorrer cada linea para mostrar cada jornada
    while(getline(archivo, linea)){
        if(linea.empty()) continue;
        //inicio de jornada con su respectivo numero
        if(linea.find("JORNADA=") == 0){
            string num = linea.substr(8);
            cout << "\n-- Jornada " << num << " --" << endl;
            dentroJornada = true;
        //sino finalizar la jornada
        } else if(linea == "FIN_JORNADA"){
            dentroJornada = false;
        //mostrar la fecha de la jornada    
        } else if(linea.find("fecha=") == 0){
            string fecha = linea.substr(6);
            cout << "Fecha: " << fecha << endl;
        //mostrar los equipos y resultado de la jornada
        } else if(dentroJornada){
            stringstream ss(linea);
            string local, visitante, gl, gv;
            getline(ss, local,     ';');
            getline(ss, visitante, ';');
            getline(ss, gl,        ';');
            getline(ss, gv);
            //imprimir el resultado del partido
            cout << "  " << local << " " << gl
                 << " - " << gv << " " << visitante << endl;
        }
    }
    //cerrar el archivo
    archivo.close();
}
// muestreo de todos los partidos jugados
void mostrarPartidos(){
    vector<Partido> partidos = leerPartidos();
    //verificar si hay partidos registrados
    if(partidos.empty()){
        cout << "\ntodavia no hay partidos registrados" << endl;
        return;
    }
    //imprimir cada partido con su respectiva info
    cout << "\n== Partidos jugados ==" << endl;
    for(int i = 0; i < partidos.size(); i++){
        Partido p = partidos[i];
        cout << i+1 << ". [" << p.fecha << "] "
             << p.local  << " " << p.golesLocal
             << " - "
             << p.golesVisitante << " " << p.visitante << endl;
    }
}
//funcion de registrar un partido 
void registrarPartido(ConfigLiga config){
    Partido p;
    //darle al usuario los equipos que hay para elegir
    cout << "\nEquipos disponibles:" << endl;
    for(int i = 0; i < config.equipos.size(); i++){
        cout << "  " << i+1 << ". " << config.equipos[i] << endl;
    }
    //seleccion del local
    int a, b;
    cout << "\nequipo LOCAL (numero): ";
    cin >> a;
    a--;
    //seleccion del visitante
    cout << "equipo VISITANTE (numero): ";
    cin >> b;
    b--;
    //validar que no escoge numero de equipo inexistente  
    if(a < 0 || a >= config.equipos.size() ||
       b < 0 || b >= config.equipos.size()){
        cout << "numero de equipo invalido" << endl;
        return;
    }
    //validar si no escogio el mismo equipo para local y visitante
    if(a == b){
        cout << "el local y visitante no pueden ser el mismo" << endl;
        return;
    }
    //asignar los equipos al partido
    p.local     = config.equipos[a];
    p.visitante = config.equipos[b];
    //solicitar el resultado del partido
    cout << "goles de " << p.local     << ": ";
    cin  >> p.golesLocal;
    cout << "goles de " << p.visitante << ": ";
    cin  >> p.golesVisitante;
    //asignar la fecha del partido
    cout << "fecha del partido (ej: 2025-03-30): ";
    cin  >> p.fecha;
//funcionalidad de ver si se duplico un partido 
    vector<Partido> existentes = leerPartidos();
    for(int i = 0; i < existentes.size(); i++){
        if( existentes[i].local == p.local && existentes[i].fecha == p.fecha && existentes[i].visitante == p.visitante ){
            cout << "este partido ya fue registrado" << endl;
            return;
        }
    }
    //guardar el partido y la jornada
    guardarPartido(p);
    vector<Partido> partidos;
    partidos.push_back(p);
    guardarJornada(1, p.fecha, partidos);
    //mostrar el partido registrado
    cout << "\npartido guardado: "
         << p.local  << " " << p.golesLocal
         << " - "
         << p.golesVisitante << " " << p.visitante << endl;
}
// menu principal
int mostrarMenu(string nombreLiga){
    int opcion;
    //imprimir el menu 
    cout << "\n=== " << nombreLiga << " ===" << endl;
    cout << "1. Ver tabla de posiciones" << endl;
    cout << "2. Registrar partido" << endl;
    cout << "3. Ver historial de jornadas" << endl;
    cout << "4. Ver todos los partidos" << endl;
    cout << "5. Ver historial de enfrentamientos" << endl;
    cout << "6. Editar partido" << endl;
    cout << "7. Salir" << endl;
    cout << "opcion: ";
    cin >> opcion;
    return opcion;
}
//funcionalidad de enfrentamiento de equipos
void enfrentamientoEquipos(ConfigLiga config){
    //mostrar equipos disponibles para elegir
    cout << "\nEquipos disponibles:" << endl;
    for(int i = 0; i < config.equipos.size(); i++){
    cout << "  " << i+1 << ". " << config.equipos[i] << endl;
}
// pedir al usuario que elija dos equipos por numero
int a, b;
cout << "elige equipo 1 (numero): ";
cin >> a;
a--;

cout << "elige equipo 2 (numero): ";
cin >> b;
b--;
// validar que no escoga equipos inexistentes o el mismo equipo dos veces
if(a < 0 || a >= config.equipos.size() || b < 0 || b >= config.equipos.size()){
    cout << "numero de equipo invalido" << endl;
    return;
}
if(a == b){
    cout << "no puedes elegir el mismo equipo dos veces" << endl;
    return;
}
string equipoA = config.equipos[a];
string equipoB = config.equipos[b];
vector<Partido> partidos = leerPartidos();
bool hayPartidos = false;
int contador = 1;
cout << "\nHistorial: " << equipoA << " vs " << equipoB << endl;
for(int i = 0; i < partidos.size(); i++){
    //recorrer cada partido entre los dos equipos 
    if ((partidos[i].local == equipoA && partidos[i].visitante == equipoB) || (partidos[i].local == equipoB && partidos[i].visitante == equipoA)){
        hayPartidos = true;
        cout << contador << ". [" << partidos[i].fecha << "] "
             << partidos[i].local  << " " << partidos[i].golesLocal
             << " - "
             << partidos[i].golesVisitante << " " << partidos[i].visitante << endl;
        contador++;
    }
    }
    //validar si hay partidos entre esos equipos o no
    if(!hayPartidos){
    cout << "no hay partidos entre estos equipos" << endl;
}
}
void edicionDePartidos( ){
    vector<Partido> partidos = leerPartidos();
    //validar si hay partidos registrados para editar
    if(partidos.empty()){
        cout << "no hay partidos para editar" << endl;
        return;
    }
    mostrarPartidos();
    //pedir el partido a editar por numero
    int eleccion;
    cout << "elige el numero del partido a editar: ";
    cin >> eleccion;
    eleccion--;
    if(eleccion < 0 || eleccion >= partidos.size()){
        cout << "numero invalido" << endl;
    return;
}
    //reedicion del partido 
    cout << "nuevos goles de " << partidos[eleccion].local << ": ";
    cin >> partidos[eleccion].golesLocal;
    cout << "nuevos goles de " << partidos[eleccion].visitante << ": ";
    cin >> partidos[eleccion].golesVisitante;
    ofstream archivo("data/partidos.txt");
    for(int i = 0; i < partidos.size(); i++){
    archivo << partidos[i].fecha      << ";"
            << partidos[i].local      << ";"
            << partidos[i].visitante  << ";"
            << partidos[i].golesLocal << ";"
            << partidos[i].golesVisitante << endl;
}
//cerrar el archivo
archivo.close();
cout << "partido actualizado!" << endl;
}
int main(){
    ConfigLiga config;
    //verificar si leyo bien la configuracion de la liga 
    if(!leerConfig("data/config.txt", config)){
        return 1;
    }
    cout << "Bienvenido a " << config.ligapato << "!" << endl;
    int opcion = 0;
    while(opcion != 7){
        opcion = mostrarMenu(config.ligapato);
        //mostrar el menu
        if(opcion == 1){
            vector<Equipo> tabla = construirTabla(config);
            mostrarTabla(tabla);
        } else if(opcion == 2){
            registrarPartido(config);
        } else if(opcion == 3){
            mostrarHistorial();
        } else if(opcion == 4){
            mostrarPartidos();
        } else if (opcion == 5){
            enfrentamientoEquipos(config);
        }else if (opcion == 6){
            edicionDePartidos();
        }else if (opcion == 7){
            cout << "hasta luego" << endl;
        }
    }
    return 0;
}
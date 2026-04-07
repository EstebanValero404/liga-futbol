#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// datos de configuracion de la liga
struct ConfigLiga {
    string ligapato;
    int ptsV;
    int ptsE;
    int ptsD;
    vector<string> equipos;
};

// un partido jugado
struct Partido {
    string fecha;
    string local;
    string visitante;
    int golesL;
    int golesV;
};

// estadisticas de cada equipo
struct Equipo {
    string nombre;
    int pj, pg, pe, pp;
    int gf, gc, dg, pts;
};

// crea un equipo con todo en cero
Equipo crearEquipo(string nombre){
    Equipo e;
    e.nombre = nombre;
    e.pj = 0; e.pg = 0; e.pe = 0; e.pp = 0;
    e.gf = 0; e.gc = 0; e.dg = 0; e.pts = 0;
    return e;
}

// leo el config.txt y guardo los datos de la liga
bool leerConfig(string nombreArchivo, ConfigLiga &config){
    ifstream archivo;
    archivo.open(nombreArchivo);

    if(!archivo.is_open()){
        cout << "no se encontro el archivo " << nombreArchivo << endl;
        return false;
    }

    string linea;
    while(getline(archivo, linea)){
        // salto lineas vacias y comentarios
        if(linea.empty() || linea[0] == '#') continue;

        stringstream ss(linea);
        string clave, valor;
        getline(ss, clave, '=');
        getline(ss, valor);

        if(clave == "nombre")   config.ligapato = valor;
        if(clave == "victoria") config.ptsV = stoi(valor);
        if(clave == "empate")   config.ptsE = stoi(valor);
        if(clave == "derrota")  config.ptsD = stoi(valor);
        if(clave == "equipo")   config.equipos.push_back(valor);
    }

    archivo.close();
    return true;
}

// guardo el partido en partidos.txt
void guardarPartido(Partido p){
    // CORRECCIÓN: Apuntar a la carpeta data/
    ofstream archivo("data/partidos.txt", ios::app);

    if(!archivo.is_open()){
        cout << "error abriendo partidos.txt" << endl;
        return;
    }

    archivo << p.fecha    << ";"
            << p.local    << ";"
            << p.visitante << ";"
            << p.golesL   << ";"
            << p.golesV   << endl;

    archivo.close();
}

// guardo la jornada en fechas.txt
void guardarJornada(int numJornada, string fecha, vector<Partido> partidos){
    // CORRECCIÓN: Apuntar a la carpeta data/
    ofstream archivo("data/fechas.txt", ios::app);

    if(!archivo.is_open()){
        cout << "error abriendo fechas.txt" << endl;
        return;
    }

    archivo << "JORNADA=" << numJornada << endl;
    archivo << "fecha=" << fecha << endl;

    // escribo cada partido de la jornada
    for(int i = 0; i < partidos.size(); i++){
        archivo << partidos[i].local     << ";"
                << partidos[i].visitante << ";"
                << partidos[i].golesL    << ";"
                << partidos[i].golesV    << endl;
    }

    archivo << "FIN_JORNADA" << endl;
    archivo.close();
}

// leo todos los partidos del archivo
vector<Partido> leerPartidos(){
    vector<Partido> partidos;
    // CORRECCIÓN: Apuntar a la carpeta data/
    ifstream archivo("data/partidos.txt");

    if(!archivo.is_open()){
        cout << "error abriendo partidos.txt" << endl;
        return partidos;
    }

    string linea;
    while(getline(archivo, linea)){
        if(linea.empty()) continue;

        stringstream ss(linea);
        string campo;
        Partido p;

        getline(ss, p.fecha,      ';');
        getline(ss, p.local,      ';');
        getline(ss, p.visitante,  ';');

        getline(ss, campo, ';');
        p.golesL = stoi(campo);

        getline(ss, campo);
        p.golesV = stoi(campo);

        partidos.push_back(p);
    }

    archivo.close();
    return partidos;
}

// actualiza stats del equipo segun el resultado
void actualizarEquipo(Equipo *e, int golesFavor, int golesContra, ConfigLiga config){
    e->pj++;
    e->gf += golesFavor;
    e->gc += golesContra;
    e->dg = e->gf - e->gc;

    // verifico si gano, empato o perdio
    if(golesFavor > golesContra){
        e->pg++;
        e->pts += config.ptsV;
    } else if(golesFavor == golesContra){
        e->pe++;
        e->pts += config.ptsE;
    } else {
        e->pp++;
        e->pts += config.ptsD;
    }
}

// construyo la tabla leyendo todos los partidos
vector<Equipo> construirTabla(ConfigLiga config){
    vector<Equipo> tabla;

    for(int i = 0; i < config.equipos.size(); i++){
        tabla.push_back(crearEquipo(config.equipos[i]));
    }

    vector<Partido> partidos = leerPartidos();

    for(int i = 0; i < partidos.size(); i++){
        Partido p = partidos[i];

        // busco el equipo local y actualizo
        for(int j = 0; j < tabla.size(); j++){
            if(tabla[j].nombre == p.local){
                actualizarEquipo(&tabla[j], p.golesL, p.golesV, config);
            }
        }

        // busco el visitante y actualizo
        for(int j = 0; j < tabla.size(); j++){
            if(tabla[j].nombre == p.visitante){
                actualizarEquipo(&tabla[j], p.golesV, p.golesL, config);
            }
        }
    }

    // ordeno la tabla por puntos de mayor a menor
    for(int i = 0; i < tabla.size()-1; i++){
        for(int j = i+1; j < tabla.size(); j++){
            if(tabla[j].pts > tabla[i].pts){
                Equipo temp = tabla[i];
                tabla[i] = tabla[j];
                tabla[j] = temp;
            }
        }
    }

    return tabla;
}

// muestro la tabla en pantalla
void mostrarTabla(vector<Equipo> tabla){
    cout << "\n== Tabla de Posiciones ==" << endl;
    cout << "#  Equipo              PJ PG PE PP GF GC DG PTS" << endl;
    cout << "-----------------------------------------------" << endl;

    for(int i = 0; i < tabla.size(); i++){
        Equipo e = tabla[i];

        cout << i+1 << ". ";
        cout << e.nombre;

        int espacios = 20 - e.nombre.size();
        for(int s = 0; s < espacios; s++) cout << " ";

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

    int guardar;
    cout << "guardar en tabla.txt? (1=si / 0=no): ";
    cin >> guardar;

    if(guardar == 1){
        // CORRECCIÓN: Guardar también en la carpeta data/
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
        archivo.close();
        cout << "listo, tabla guardada" << endl;
    }
}

// muestro el historial de jornadas
void mostrarHistorial(){
    // CORRECCIÓN: Apuntar a la carpeta data/
    ifstream archivo("data/fechas.txt");

    if(!archivo.is_open()){
        cout << "no se pudo abrir fechas.txt" << endl;
        return;
    }

    string linea;
    bool dentroJornada = false;

    while(getline(archivo, linea)){
        if(linea.empty()) continue;

        if(linea.find("JORNADA=") == 0){
            string num = linea.substr(8);
            cout << "\n-- Jornada " << num << " --" << endl;
            dentroJornada = true;

        } else if(linea == "FIN_JORNADA"){
            dentroJornada = false;

        } else if(linea.find("fecha=") == 0){
            string fecha = linea.substr(6);
            cout << "Fecha: " << fecha << endl;

        } else if(dentroJornada){
            stringstream ss(linea);
            string local, visitante, gl, gv;
            getline(ss, local,     ';');
            getline(ss, visitante, ';');
            getline(ss, gl,        ';');
            getline(ss, gv);
            cout << "  " << local << " " << gl
                 << " - " << gv << " " << visitante << endl;
        }
    }

    archivo.close();
}

// muestro todos los partidos jugados
void mostrarPartidos(){
    vector<Partido> partidos = leerPartidos();

    if(partidos.empty()){
        cout << "\ntodavia no hay partidos registrados" << endl;
        return;
    }

    cout << "\n== Partidos jugados ==" << endl;
    for(int i = 0; i < partidos.size(); i++){
        Partido p = partidos[i];
        cout << i+1 << ". [" << p.fecha << "] "
             << p.local  << " " << p.golesL
             << " - "
             << p.golesV << " " << p.visitante << endl;
    }
}

// le pido al usuario los datos del partido y lo registro
void registrarPartido(ConfigLiga config){
    Partido p;

    cout << "\nEquipos disponibles:" << endl;
    for(int i = 0; i < config.equipos.size(); i++){
        cout << "  " << i+1 << ". " << config.equipos[i] << endl;
    }

    int a, b;
    cout << "\nequipo LOCAL (numero): ";
    cin >> a;
    a--;

    cout << "equipo VISITANTE (numero): ";
    cin >> b;
    b--;

    if(a < 0 || a >= config.equipos.size() ||
       b < 0 || b >= config.equipos.size()){
        cout << "numero de equipo invalido" << endl;
        return;
    }
    if(a == b){
        cout << "el local y visitante no pueden ser el mismo" << endl;
        return;
    }

    p.local     = config.equipos[a];
    p.visitante = config.equipos[b];

    cout << "goles de " << p.local     << ": ";
    cin  >> p.golesL;
    cout << "goles de " << p.visitante << ": ";
    cin  >> p.golesV;

    cout << "fecha del partido (ej: 2025-03-30): ";
    cin  >> p.fecha;

    guardarPartido(p);

    vector<Partido> partidos;
    partidos.push_back(p);
    guardarJornada(1, p.fecha, partidos); // Nota: aquí siempre estás guardando como Jornada 1. Podrías mejorarlo más adelante.

    cout << "\npartido guardado: "
         << p.local  << " " << p.golesL
         << " - "
         << p.golesV << " " << p.visitante << endl;
}

// menu principal
int mostrarMenu(string nombreLiga){
    int opcion;
    cout << "\n=== " << nombreLiga << " ===" << endl;
    cout << "1. Ver tabla de posiciones" << endl;
    cout << "2. Registrar partido" << endl;
    cout << "3. Ver historial de jornadas" << endl;
    cout << "4. Ver todos los partidos" << endl;
    cout << "5. Salir" << endl;
    cout << "opcion: ";
    cin >> opcion;
    return opcion;
}

int main(){
    ConfigLiga config;

    // CORRECCIÓN: Apuntar a la carpeta data/
    if(!leerConfig("data/config.txt", config)){
        return 1;
    }

    cout << "Bienvenido a " << config.ligapato << "!" << endl;

    int opcion = 0;
    while(opcion != 5){
        opcion = mostrarMenu(config.ligapato);

        if(opcion == 1){
            vector<Equipo> tabla = construirTabla(config);
            mostrarTabla(tabla);
        } else if(opcion == 2){
            registrarPartido(config);
        } else if(opcion == 3){
            mostrarHistorial();
        } else if(opcion == 4){
            mostrarPartidos();
        } else if(opcion == 5){
            cout << "hasta luego!" << endl;
        } else {
            cout << "opcion invalida" << endl;
        }
    }

    return 0;
}
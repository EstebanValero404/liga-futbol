#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// ── configuracion de ligaPato ─────────────────────────────────────
struct ConfigLiga {
    string ligapato;
    int ptsvictoria;
    int ptsempate;
    int ptsderrota;
    vector<string> equipos;
};

// ── un partido jugado en ligaPato ─────────────────────────────────
struct Partido {
    string fecha;
    string local;
    string visitante;
    int goleslocal;
    int golesvisitante;
};

// ── ficha de un equipo en la tabla de ligaPato ────────────────────
struct Equipo {
    string nombre;
    int pj, pg, pe, pp;
    int gf, gc, dg, pts;
};

// ── crea un equipo con estadisticas en cero ───────────────────────
Equipo crearEquipo(string nombre){
    Equipo e;
    e.nombre = nombre;
    e.pj = 0; e.pg = 0; e.pe = 0; e.pp = 0;
    e.gf = 0; e.gc = 0; e.dg = 0; e.pts = 0;
    return e;
}

// ── lee el config.txt y llena la configuracion de ligaPato ────────
bool leerConfig(string nombreArchivo, ConfigLiga &config){
    ifstream archivo;
    archivo.open(nombreArchivo);

    if(!archivo.is_open()){
        cout << "Error: no se encontro el archivo " << nombreArchivo << endl;
        return false;
    }

    string linea;
    while(getline(archivo, linea)){
        if(linea.empty() || linea[0] == '#') continue;

        stringstream ss(linea);
        string clave, valor;
        getline(ss, clave, '=');
        getline(ss, valor);

        if(clave == "nombre")   config.ligapato    = valor;
        if(clave == "victoria") config.ptsvictoria = stoi(valor);
        if(clave == "empate")   config.ptsempate   = stoi(valor);
        if(clave == "derrota")  config.ptsderrota  = stoi(valor);
        if(clave == "equipo")   config.equipos.push_back(valor);
    }

    archivo.close();
    return true;
}

// ── guarda un partido en partidos.txt usando append ───────────────
void guardarPartido(Partido p){
    ofstream archivo("partidos.txt", ios::app);

    if(!archivo.is_open()){
        cout << "Error: no se pudo abrir partidos.txt" << endl;
        return;
    }

    archivo << p.fecha          << ";"
            << p.local          << ";"
            << p.visitante      << ";"
            << p.goleslocal     << ";"
            << p.golesvisitante << endl;

    archivo.close();
}

// ── guarda una jornada completa en fechas.txt usando append ───────
void guardarJornada(int numJornada, string fecha, vector<Partido> partidos){
    ofstream archivo("fechas.txt", ios::app);

    if(!archivo.is_open()){
        cout << "Error: no se pudo abrir fechas.txt" << endl;
        return;
    }

    archivo << "JORNADA=" << numJornada << endl;
    archivo << "fecha="   << fecha      << endl;

    for(int i = 0; i < partidos.size(); i++){
        archivo << partidos[i].local          << ";"
                << partidos[i].visitante      << ";"
                << partidos[i].goleslocal     << ";"
                << partidos[i].golesvisitante << endl;
    }

    archivo << "FIN_JORNADA" << endl;
    archivo.close();
}

// ── lee todos los partidos de partidos.txt ────────────────────────
vector<Partido> leerPartidos(){
    vector<Partido> partidos;
    ifstream archivo("partidos.txt");

    if(!archivo.is_open()){
        cout << "Error: no se pudo abrir partidos.txt" << endl;
        return partidos;
    }

    string linea;
    while(getline(archivo, linea)){
        if(linea.empty()) continue;

        stringstream ss(linea);
        string campo;
        Partido p;

        getline(ss, p.fecha,     ';');
        getline(ss, p.local,     ';');
        getline(ss, p.visitante, ';');

        getline(ss, campo, ';');
        p.goleslocal = stoi(campo);

        getline(ss, campo);
        p.golesvisitante = stoi(campo);

        partidos.push_back(p);
    }

    archivo.close();
    return partidos;
}
// ── actualiza estadisticas de un equipo segun resultado ───────────
// usa puntero para modificar el equipo original directamente
void actualizarEquipo(Equipo *e, int golesFavor, int golesContra, ConfigLiga config){
    e->pj++;
    e->gf += golesFavor;
    e->gc += golesContra;
    e->dg  = e->gf - e->gc;

    if(golesFavor > golesContra){
        e->pg++;
        e->pts += config.ptsvictoria;
    } else if(golesFavor == golesContra){
        e->pe++;
        e->pts += config.ptsempate;
    } else {
        e->pp++;
        e->pts += config.ptsderrota;
    }
}
// ── construye la tabla de posiciones desde partidos.txt ───────────
vector<Equipo> construirTabla(ConfigLiga config){
    // crear un equipo vacio por cada equipo de la liga
    vector<Equipo> tabla;
    for(int i = 0; i < config.equipos.size(); i++){
        tabla.push_back(crearEquipo(config.equipos[i]));
    }

    // leer todos los partidos jugados
    vector<Partido> partidos = leerPartidos();

    // para cada partido, actualizar estadisticas de local y visitante
    for(int i = 0; i < partidos.size(); i++){
        Partido p = partidos[i];

        // buscar el equipo local en la tabla
        for(int j = 0; j < tabla.size(); j++){
            if(tabla[j].nombre == p.local){
                actualizarEquipo(&tabla[j], p.goleslocal, p.golesvisitante, config);
            }
        }

        // buscar el equipo visitante en la tabla
        for(int j = 0; j < tabla.size(); j++){
            if(tabla[j].nombre == p.visitante){
                actualizarEquipo(&tabla[j], p.golesvisitante, p.goleslocal, config);
            }
        }
    }

    // ordenar por puntos de mayor a menor
    for(int i = 0; i < tabla.size()-1; i++){
        for(int j = i+1; j < tabla.size(); j++){
            if(tabla[j].pts > tabla[i].pts){
                Equipo temp = tabla[i];
                tabla[i]    = tabla[j];
                tabla[j]    = temp;
            }
        }
    }

    return tabla;
}
// ── muestra la tabla de posiciones en consola ─────────────────────
void mostrarTabla(vector<Equipo> tabla){
    cout << "\n--- Tabla de Posiciones ligaPato ---" << endl;
    cout << "#  Equipo              PJ PG PE PP GF GC DG  PTS" << endl;
    cout << "------------------------------------------------" << endl;

    for(int i = 0; i < tabla.size(); i++){
        Equipo e = tabla[i];

        // imprimir posicion
        cout << i+1 << ". ";

        // imprimir nombre con padding para alinear columnas
        cout << e.nombre;
        int espacios = 20 - e.nombre.size();
        for(int s = 0; s < espacios; s++) cout << " ";

        // imprimir estadisticas
        cout << e.pj  << "  "
             << e.pg  << "  "
             << e.pe  << "  "
             << e.pp  << "  "
             << e.gf  << "  "
             << e.gc  << "  "
             << e.dg  << "  "
             << e.pts << endl;
    }
    cout << "------------------------------------------------" << endl;

    // opcion de guardar en tabla.txt
    cout << "\nGuardar tabla en tabla.txt? (1=si / 0=no): ";
    int guardar;
    cin >> guardar;

    if(guardar == 1){
        ofstream archivo("tabla.txt");
        archivo << "# Tabla de Posiciones - ligaPato" << endl;
        archivo << "#  Equipo              PJ PG PE PP GF GC DG  PTS" << endl;
        for(int i = 0; i < tabla.size(); i++){
            Equipo e = tabla[i];
            archivo << i+1 << ". " << e.nombre << "  "
                    << e.pj << " " << e.pg << " "
                    << e.pe << " " << e.pp << " "
                    << e.gf << " " << e.gc << " "
                    << e.dg << " " << e.pts << endl;
        }
        archivo.close();
        cout << "Tabla guardada en tabla.txt" << endl;
    }
}
// ── muestra el historial de jornadas desde fechas.txt ─────────────
void mostrarHistorial(){
    ifstream archivo("fechas.txt");

    if(!archivo.is_open()){
        cout << "Error: no se pudo abrir fechas.txt" << endl;
        return;
    }

    string linea;
    bool dentroJornada = false;

    while(getline(archivo, linea)){
        if(linea.empty()) continue;

        if(linea.find("JORNADA=") == 0){
            // inicio de jornada
            string num = linea.substr(8);
            cout << "\n--- Jornada " << num << " ---" << endl;
            dentroJornada = true;

        } else if(linea == "FIN_JORNADA"){
            // fin de jornada
            dentroJornada = false;

        } else if(linea.find("fecha=") == 0){
            // fecha de la jornada
            string fecha = linea.substr(6);
            cout << "Fecha: " << fecha << endl;

        } else if(dentroJornada){
            // partido dentro de la jornada
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
// ── muestra todos los partidos jugados desde partidos.txt ─────────
void mostrarPartidos(){
    vector<Partido> partidos = leerPartidos();

    if(partidos.empty()){
        cout << "\nNo hay partidos registrados aun." << endl;
        return;
    }

    cout << "\n--- Todos los partidos de ligaPato ---" << endl;
    for(int i = 0; i < partidos.size(); i++){
        Partido p = partidos[i];
        cout << i+1 << ". [" << p.fecha << "] "
             << p.local         << " "  << p.goleslocal
             << " - "
             << p.golesvisitante << " " << p.visitante << endl;
    }
}
// ── guia al usuario para registrar un nuevo partido ───────────────
void registrarPartido(ConfigLiga config){
    Partido p;

    cout << "\n--- Equipos de " << config.ligapato << " ---" << endl;
    for(int i = 0; i < config.equipos.size(); i++){
        cout << "  " << i+1 << ". " << config.equipos[i] << endl;
    }

    int idxLocal, idxVisitante;
    cout << "\nSelecciona equipo LOCAL (numero): ";
    cin  >> idxLocal;
    idxLocal--;

    cout << "Selecciona equipo VISITANTE (numero): ";
    cin  >> idxVisitante;
    idxVisitante--;

    if(idxLocal < 0 || idxLocal >= config.equipos.size() ||
       idxVisitante < 0 || idxVisitante >= config.equipos.size()){
        cout << "Error: numero de equipo invalido." << endl;
        return;
    }
    if(idxLocal == idxVisitante){
        cout << "Error: el local y visitante no pueden ser el mismo equipo." << endl;
        return;
    }

    p.local     = config.equipos[idxLocal];
    p.visitante = config.equipos[idxVisitante];

    cout << "Goles de " << p.local     << ": ";
    cin  >> p.goleslocal;
    cout << "Goles de " << p.visitante << ": ";
    cin  >> p.golesvisitante;

    cout << "Fecha del partido (ej: 2025-03-30): ";
    cin  >> p.fecha;

    guardarPartido(p);

    vector<Partido> partidos;
    partidos.push_back(p);
    guardarJornada(1, p.fecha, partidos);

    cout << "\nPartido registrado: "
         << p.local         << " " << p.goleslocal
         << " - "
         << p.golesvisitante << " " << p.visitante << endl;
}

// ── muestra el menu de ligaPato y retorna la opcion elegida ───────
int mostrarMenu(string nombreLiga){
    int opcion;

    cout << "\n-----------------------------------" << endl;
    cout << "        " << nombreLiga               << endl;
    cout << "-----------------------------------" << endl;
    cout << "  1. Ver tabla de posiciones"         << endl;
    cout << "  2. Registrar resultado de partido"  << endl;
    cout << "  3. Ver historial de jornadas"       << endl;
    cout << "  4. Ver todos los partidos jugados"  << endl;
    cout << "  5. Salir"                           << endl;
    cout << "-----------------------------------" << endl;
    cout << "  Selecciona una opcion: ";
    cin  >> opcion;

    return opcion;
}

// ── punto de entrada de ligaPato ──────────────────────────────────
int main(){
    ConfigLiga config;

    if(!leerConfig("config.txt", config)){
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
            cout << "\nHasta luego, vuelve pronto a " << config.ligapato << "!" << endl;
        } else {
            cout << "\nOpcion invalida, intenta de nuevo." << endl;
        }
    }

    return 0;
}
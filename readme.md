# liga-futbol
Sistema de gestion de liga de futbol en consola, desarrollado en C++.
## Estudiante
Juan Esteban Valero T.- Universidad Sergio Arboleda
## Descripcion
Programa en consola que gestiona una liga de futbol completa.
Lee su configuracion desde config.txt, registra partidos,
calcula la tabla de posiciones y lleva historial de jornadas.
Esto gracias a archivos de texto.
## Compilar
g++ src/ligaproyecto.cpp -o ligaproyecto.exe
## Ejecutar
.\ligaproyecto.exe
## Formato de config.txt
Usa pares clave=valor. Las lineas con # son comentarios.
Ejemplo:
nombre=ligaPato Premier 2025
victoria=3
empate=1
derrota=0
equipo=Tigres
equipo=America
equipo=Chivas
equipo=Cruz Azul
## Decisiones de diseno
- Structs para ConfigLiga, Partido y Equipo
- Punteros en actualizarEquipo() para modificar stats directamente
- Archivos separados por responsabilidad: partidos.txt y fechas.txt
- Bubble sort manual para ordenar la tabla por puntos
# liga-futbol

Sistema de gestión de liga de fútbol en consola, desarrollado en C++.

## Estudiante
Juan Esteban Valero T. — Universidad Sergio Arboleda, Bogotá

## Descripción
Programa en consola que gestiona una liga de fútbol completa. Lee su
configuración desde config.txt, registra partidos, calcula la tabla de
posiciones con bubble sort y lleva historial de jornadas. Todo persiste
en archivos .txt para que la liga continúe al volver a ejecutar.

## Compilación
Desde la raíz del repositorio:
```
g++ src/main.cpp -o main
```

## Ejecución
```
./main
```
En Windows con PowerShell:
```
.\main.exe
```

## Formato de config.txt
Usa pares clave=valor, uno por línea. Las líneas con # son comentarios.
Los equipos se agregan con la clave `equipo=` repetida por cada equipo.

Ejemplo:
```
# configuracion de ligaPato
nombre=ligaPato Premier 2025
victoria=3
empate=1
derrota=0
equipo=Tigres
equipo=America
equipo=Chivas
equipo=Cruz Azul
```

## Decisiones de diseño
- Structs separados: `ConfigLiga`, `Partido` y `Equipo`
- Punteros en `actualizarEquipo()` para modificar stats directamente
- Archivos separados por responsabilidad: `partidos.txt` y `fechas.txt`
- Bubble sort manual para ordenar la tabla por puntos
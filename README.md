# Compiler From Scratch

![Wallpaper](https://raw.githubusercontent.com/JVR-07/College-Projects/refs/heads/main/Resource/wallpaper_itt.png)

---

**Autor:** Javier Machado Sánchez

**Institución:** Instituto Tecnológico de Tijuana

**Materia:** Lenguajes y Autómatas II.

**Profesor:** Erasmo Estrada Peña.

## Compilador Básico

Este es un proyecto con fines educativos para crear un compilador desde cero, implementando cada una de sus fases.

## Fases del Compilador

El compilador se desarrollará en 5 fases principales:

1. **Análisis Léxico:** Completado.
2. **Análisis Sintáctico:** Completado.
3. **Análisis Semántico:** Completado.
4. **Generación de Código Intermedio:** Pendiente.
5. **Optimización de Código:** Pendiente.

## Estructura del Proyecto

```bash
/
├── bin/              # Archivos ejecutables generados
├── obj/              # Archivos de objeto compilados
├── src/              # Código fuente de las fases implementadas
│   ├── main.c        # Punto de entrada y orquestador del compilador
│   ├── lexer.c       # Manejo de la fase 1 (Tokenización)
│   ├── parser.c      # Manejo de la fase 2 (Generador Sintáctico y ensamble de nodos)
│   ├── ast.c         # Estructura del Árbol de Sintaxis Abstracta (AST)
│   ├── semantic.c    # Manejo de la fase 3 (Comprobación de Tipos y Análisis)
│   └── symbols.c     # Manejo de jerarquías y Tabla de Símbolos
├── include/          # Archivos de cabecera (.h)
│   ├── lexer.h
│   ├── parser.h
│   ├── ast.h
│   ├── semantic.h
│   ├── tokens.h
│   └── symbols.h
├── .gitignore
├── LICENSE
└── Makefile          # Script de compilación rápida para GCC
```

## Fases Implementadas

### Fase 1: Análisis Léxico

El analizador léxico es la primera fase del compilador. Su principal función es leer los caracteres de entrada desde el archivo fuente y elaborar como salida una secuencia de componentes léxicos (Tokens) que utilizará la siguiente fase. Ignora los espacios en blanco, tabulaciones y saltos de línea irrelevantes. Implementado en `lexer.c` a través de un autómata finito determinista básico.

### Fase 2: Análisis Sintáctico

El analizador sintáctico recibe los tokens de la fase 1 y agrupa estos componentes en frases gramaticales complejas para producir la salida estructurada.
En esta fase representamos nuestra semántica natural mediante un **Árbol de Sintaxis Abstracta (AST)** bidimensional, modelando encapsulación de condicionales lógicos (`if`, `while`, `for`), expresiones matemáticas jerárquicas, y declaración de bloques/funciones. Las reglas gramaticales en `parser.c` mapean el código consumido a una estructura enlazada en memoria mediante un motor de Descenso Recursivo.

### Fase 3: Análisis Semántico

El analizador semántico recorre de forma _Bottom-Up_ (Post-fix) el AST resultante de la fase 2 para realizar comprobaciones de contexto y recopilar información de los tipos.
A través del módulo `semantic.c`, el compilador cruza los estatutos con los tipos registrados en la red y valida la sanidad del programa mediante "Type Checking", identificando errores lógicos (e.g. sumar una cadena literal a un identificador booleano), uso de variables no declaradas o invocaciones a variables que ya murieron o se encuentran fuera de su ámbito/stack (`current_scope`).

## Cómo Compilar y Ejecutar

Para compilar el proyecto, asegúrate de tener una distribución con `gcc` y `make` instalado, y ejecuta el siguiente comando:

```bash
make
```

Esto generará los archivos objeto y finalmente un ejecutable en la carpeta `bin/`.

Para probar el compilador y analizar un archivo de texto con código fuente, invócalo como argumento:

```bash
./bin/Compiler <archivo_de_entrada>
```

## Licencia

Este proyecto está bajo la licencia GPL. Consulta el archivo [LICENCIA](./LICENSE) para más detalles.

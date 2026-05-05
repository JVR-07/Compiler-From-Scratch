# 🚀 Compiler From Scratch

![Wallpaper](https://raw.githubusercontent.com/JVR-07/College-Projects/refs/heads/main/Resource/wallpaper_itt.png)

---

### 👤 Información del Proyecto

- **Autor:** Javier Machado Sánchez
- **Institución:** Instituto Tecnológico de Tijuana
- **Materia:** Lenguajes y Autómatas II
- **Profesor:** Erasmo Estrada Peña

---

## 📖 Introducción

Este es un proyecto educativo enfocado en la construcción de un compilador funcional desde sus cimientos. El objetivo es profundizar en la teoría de lenguajes, autómatas y la arquitectura interna de las herramientas de desarrollo modernas.

## 🛠️ Estado de las Fases

| Fase | Descripción                                     |    Estado    |
| :--- | :---------------------------------------------- | :----------: |
| 1    | **Análisis Léxico**                             |      ✅      |
| 2    | **Análisis Sintáctico**                         |      ✅      |
| 3    | **Análisis Semántico**                          |      ✅      |
| 4    | **Generación de Código Intermedio/Ensamblador** |      ✅      |
| 5    | **Optimización de Código**                      | ⏳ Pendiente |

---

## 📂 Estructura del Proyecto

```bash
/
├── bin/              # Ejecutables generados
├── obj/              # Archivos objeto (.o)
├── src/              # Código fuente (.c)
│   ├── main.c        # Orquestador del flujo
│   ├── lexer.c       # Tokenización (DFA)
│   ├── parser.c      # Árbol de Sintaxis Abstracta (AST)
│   ├── ast.c         # Estructuras del Árbol
│   ├── semantic.c    # Comprobación de tipos y reglas
│   └── symbols.c     # Gestión de ámbitos y tabla de símbolos
├── include/          # Cabeceras (.h)
└── Makefile          # Automatización de construcción
```

---

## 🔍 Fases Implementadas

### 🔹 Fase 1: Análisis Léxico

Transforma el flujo de caracteres en **Tokens**. Utiliza un Autómata Finito Determinista (DFA) para reconocer palabras reservadas, identificadores, literales y operadores, filtrando elementos irrelevantes como espacios y comentarios.

### 🔹 Fase 2: Análisis Sintáctico

Construye un **Árbol de Sintaxis Abstracta (AST)** mediante la técnica de **Descenso Recursivo**. Valida que la secuencia de tokens siga la gramática definida, manejando precedencia de operadores y estructuras de control (`if`, `while`, `for`).

### 🔹 Fase 3: Análisis Semántico

Realiza la validación lógica del programa recorriendo el AST de forma _Bottom-Up_.

- **Type Checking:** Valida compatibilidad de tipos en expresiones y asignaciones (ej. no sumar `string` con `int`).
- **Gestión de Ámbitos (Scoping):** Controla la visibilidad de variables mediante una tabla de símbolos jerárquica, permitiendo variables locales y globales.
- **Detección de Errores:** Identifica variables no declaradas o usos inválidos de operadores.

### 🔹 Fase 4: Generación de Código Ensamblador (x86-64)

Genera código objeto ejecutable (`x86-64` usando sintaxis de Intel, enlazado directamente con la librería de C para I/O vía `printf`/`scanf`) "al vuelo" a medida que realiza el análisis.
Se han implementado con éxito:

1. **Operaciones aritméticas:** sumas, restas, multiplicaciones, divisiones y potencias en enteros y punto flotante (mediante las extensiones SSE `xmm`).
2. **Entrada y Salida:** Palabras reservadas `read` y `write` con formatos apropiados según tipo.
3. **Flujo de control:** Soporte para sentencias `if`/`else`, ciclos `while` y `for`.
4. **Procedimientos (`proc`):** Definición de funciones con parámetros (usando convención de llamada ABI x86-64 con `rdi`, `rsi`...) e invocaciones a funciones propias.
5. **Operadores lógicos y relacionales:** Comparaciones entre todo tipo de datos y operaciones booleanas `&&` y `||`.

---

## 🏗️ Siguiente Paso: Fase 5 - Optimización de Código

La siguiente etapa consistirá en aplicar técnicas de optimización, como el plegado de constantes, para emitir un código más eficiente y reducir las instrucciones redundantes.

---

## 💻 Ejemplo de Código Soportado

El compilador actualmente es capaz de procesar estructuras complejas como esta:

```cpp
int A = 10;
float B = 20.5;
bool flag = true;

if (A > 5 && flag) {
    B = B + A * 2; // Conversión implícita de int a float
}
```

---

## 🚀 Cómo Empezar

### Requisitos

- `gcc` (GNU Compiler Collection)
- `make`

### Compilación

```bash
make
```

### Ejecución

```bash
./bin/Compiler <tu_archivo_fuente>.txt
```

---

## 📄 Licencia

Este proyecto se distribuye bajo la licencia **GPL**. Para más detalles, consulta el archivo [LICENSE](./LICENSE).

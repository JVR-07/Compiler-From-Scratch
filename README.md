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

| Fase | Descripción                         |     Estado     |
| :--- | :---------------------------------- | :------------: |
| 1    | **Análisis Léxico**                 |       ✅       |
| 2    | **Análisis Sintáctico**             |       ✅       |
| 3    | **Análisis Semántico**              |       ✅       |
| 4    | **Generación de Código Intermedio** | 🏗️ En Progreso |
| 5    | **Optimización de Código**          |  ⏳ Pendiente  |

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

---

## 🏗️ Siguiente Paso: Fase 4 - Generación de Código Intermedio

En esta etapa, el compilador transformará el AST validado en una representación intermedia, usualmente **Código de Tres Direcciones (TAC)**. Esta representación es independiente de la arquitectura destino y facilita la optimización posterior.

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

![Wallpaper](https://raw.githubusercontent.com/JVR-07/College-Projects/refs/heads/main/Resource/wallpaper_itt.png)
---

**Autor:** Javier Machado Sánchez
  
**Institución:** Instituto Tecnológico de Tijuana
  
**Materia:** Lenguajes y Autómatas II.
  
**Profesor:** Erasmo Estrada Peña.

# Compilador Básico

Este es un proyecto con fines educativos para crear un compilador desde cero, implementando cada una de sus fases.

## Fases del Compilador

El compilador se desarrollará en 5 fases principales:

1. **Análisis Léxico:** Completado.
2. **Análisis Sintáctico:** En desarrollo.
3. **Análisis Semántico:** Pendiente.
4. **Generación de Código Intermedio:** Pendiente.
5. **Optimización de Código:** Pendiente.

## Estructura del Proyecto

```bash
/
├── bin/              # Archivos ejecutables
├── obj/              # Archivos de objeto
├── src/              # Código fuente
│   ├── main.c
│   ├── lexer.c
│   └── symbols.c
├── include/          # Archivos de cabecera
│   ├── lexer.h
│   ├── tokens.h
│   └── symbols.h
├── .gitignore
├── LICENSE
└── Makefile
```

## Fase 1: Análisis Léxico

El analizador léxico es la primera fase de un compilador. Su principal función es leer los caracteres de entrada y elaborar como salida una secuencia de componentes léxicos que utiliza el analizador sintáctico para hacer el análisis.

## Cómo Compilar y Ejecutar

Para compilar el proyecto, simplemente ejecuta el siguiente comando:

```bash
make
```

Esto generará un ejecutable en la carpeta `bin/`.

Para ejecutar el compilador:

```bash
./bin/compiler <archivo_de_entrada>
```

## Licencia

Este proyecto está bajo la licencia MIT. Consulta el archivo `LICENSE` para más detalles.

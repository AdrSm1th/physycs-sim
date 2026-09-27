# Physics Simulator 2D

Учебная практика: симуляция физики столкновений в 2D, реализованная с нуля на C++.

## О проекте

Программа моделирует движение и столкновения твёрдых тел (кругов и прямоугольников)
в двумерном пространстве. Физика реализована с нуля: математика, интеграция,
детекция коллизий и разрешение контактов. Библиотека SFML используется только
для создания окна, ввода и отрисовки — вся физика написана самостоятельно.

## Стек

- **Язык:** C++20
- **Библиотека:** SFML 3.02 (graphics, window, system)
- **Сборка:** Visual Studio 2022 (MSVC, x64)
- **ОС:** Windows 10/11

## Что уже реализовано

- [x] Векторная математика (`Vec2`)
- [x] Интеграция semi-implicit Euler
- [x] Фиксированный шаг физики + аккумулятор времени
- [ ] Детекция коллизий circle–circle
- [ ] Разрешение коллизий импульсом

## Как собрать

### Требования

- Visual Studio 2022 с компонентом «Desktop development with C++»
- SFML 3.0.2 (сборка под Visual C++ 17 (2022), 64-bit)

### Шаги

1. Клонировать репозиторий:

git clone https://github.com/AdrSm1th/physycs-sim.git

### Установка
1. Установить vcpkg:
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install

2. Установить SFML:
   .\vcpkg install sfml:x64-windows

3. Открыть physics-sim.sln в Visual Studio 2022, собрать (Ctrl+Shift+B).

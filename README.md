# Deque

Реализация двунаправленной очереди (дэки) для Windows. Написано на C с использованием MSVS 2019 и WinAPI с поддержкой потокобезопасности (критические секции). В основе лежит двусвязный список.

Репозиторий содержит:
- решение MSVS 2019 (Deque.sln);
- исходники (Deque\);
- пример (Example\);
- тесты (Tests\);
- скрипты для очистки (clean.bat удаляет увесистую папку ipch в .vs\Deque\v16\ и бинарники, clean_all.bat удаляет то же + всю папку .vs).

Решение состоит из двух проектов (Example и Tests) с динамической/статической конфигурациями для x86 и x64. Протестировано на Windows Vista и выше.
____
Deque implementation for Windows. Written in C using MSVS 2019 and WinAPI with thread safety support (critical sections). Based on a doubly linked list.

The repository contains:
- MSVS 2019 solution (Deque.sln);
- source code (Deque\);
- example (Example\);
- tests (Tests\);
- scripts for clean up (clean.bat removes the weighty ipch folder in .vs\Deque\v16\ and also removes binaries, clean_all.bat removes the same as the previous one plus the entire .vs folder).

The solution consists of two projects (Example and Tests) with dynamic/static configurations for both x86 and x64 architectures. Tested on Windows Vista and above.

## Building

Microsoft Visual Studio 2019 is used to build the solution.

Once built, you can find the binaries in the Bins\ folder:

- x64\
  - Debug\
  - Debug (static)\
  - Release\
  - Release (static)\
- x86\
	- Debug\
	- Debug (static)\
	- Release\
	- Release (static)\

## Authors

```
blightn <blightan@gmail.com>
```

## License

This project is licensed under the MIT License. See LICENSE in the project's root directory.

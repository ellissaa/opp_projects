#include <omp.h>
#include <stdio.h>

// в консоли
// export OMP_NUM_THREADS=5
// export OMP_DYNAMIC=true - динамическое распределение числа потоков - если тру, то система сама выбирает, какое кол-во потоков использовать
// указываем макс. кол-во потоков
// либо
#pragma omp parallel for num_threads(4)
// for (int i = 0; i < N; i++) {}

#pragma omp parallel for
// for (int i = 0; i < N; i++) {}
// создаем потоки и каждому потоку присваиваем действие => быстрее

#pragma omp parallel for reduction (+, res)
// res локальна для каждого потока, в конце выполнения все результаты сложатся
// каждый поток обращается к своей копии переменной

#pragma omp parallel 
{
    #pragma omp for
    for (int i = 0; i < N; i++) {} // созданные потоки распределяются по циклам, новых не создается
    // удобно, тк потоки создаются не бесплатно, рациональнее использовать существующие
}

#pragma omp parallel for schedule(dynamic / static / guided / auto, 100) //способ распределения итераций цикла по потокам

#pragma omp parallel private(a) shared(b)
// указываем права доступа по потокам. private - уникальные для каждого потока
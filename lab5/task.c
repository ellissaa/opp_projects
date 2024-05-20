#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>

#include "task.h"

TaskT MakeTask(int rank, int num_proc, int iter) {
    static int id = 0;
    TaskT task;
    task.num_iter = abs(rank - (iter % num_proc));
    task.id = id++;
    task.owner_rank = rank;
    return task;
}

void RunTask(TaskT task) {
    usleep(task.num_iter * 10000); // сон в микросекундах, задание такое :))))
}
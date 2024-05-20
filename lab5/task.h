#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

typedef struct {
    int num_iter;
    int id;
    int owner_rank;
} TaskT;

TaskT MakeTask(int rank, int num_proc, int iter);

void RunTask(TaskT task);

#endif
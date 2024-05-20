#define _DEFAULT_SOURCE

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"
#include "task.h"
#include "utility.h"

#define TASK_REQUEST_TAG 1
#define TASK_RESPONSE_TAG 2

#define NUM_TASKS 10 // 50 тасков на каждом процессе
#define NUM_ITERS 5
#define SLEEP_TIME 1000
#define STOP_FLAG 777

static MPI_Datatype MpiTaskT;
static int work_completed = 0;

void RunTasks(QueueT* task_queue) {
    int rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    while (1) {
        while (1) { // исполняем задачи, которые есть в очереди
            TaskT task;
            if (QueueGet(task_queue, &task) == NULL)
                break;

            RunTask(task);
            printf("Task %i from node %i completed on node %i\n",
                task.id,  task.owner_rank, rank);
        }
        // MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 1; i < comm_size; i++) {
            int other_rank = (rank + i) % comm_size; // посчитали номер другого компьютера
            TaskT task_received;
            MPI_Status recv_status;

            MPI_Send(NULL, 0, MPI_INT, other_rank, TASK_REQUEST_TAG, MPI_COMM_WORLD); // послали ему запрос на таск
            MPI_Recv(&task_received, 1, MpiTaskT, other_rank, TASK_RESPONSE_TAG,
                MPI_COMM_WORLD, &recv_status); // получаем таск

            int num_tasks_received;
            MPI_Get_count(&recv_status, MpiTaskT, &num_tasks_received); // кол-во элементов, принятых после ресив
            if (num_tasks_received == 1) {
                QueueAdd(task_queue, task_received); // пока не найдем какой-нибудь 1 таск
                break;
            }
        }

        if (QueueIsEmpty(task_queue)) {
            MPI_Barrier(MPI_COMM_WORLD); // ждем всех
            break;
        } 
    }
}

void* ProcessRequests(void* task_queue_arg) { // вспомогательный поток, который принимает запросы от других компьтеров
    QueueT* task_queue = (QueueT*) task_queue_arg;

    while (1) {
        MPI_Request recv_request;
        MPI_Status recv_status;
        int recv_completed = 0;

        MPI_Irecv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, TASK_REQUEST_TAG, MPI_COMM_WORLD,
           &recv_request); // создали запрос на принятие данных, принимаем в NULL размера 0, requests_tag - запрос на новое задание, получаем сообщение от любого процесса с этим тегом и изменяем recv_request
        // int req;
        // MPI_Recv(&req, 0, MPI_INT, MPI_ANY_SOURCE, TASK_REQUEST_TAG, MPI_COMM_WORLD,
        //     &recv_status);
        // if (req == STOP_FLAG) {
        //     break;
        // }

        do {
           MPI_Test(&recv_request, &recv_completed, &recv_status); // завершился ли запрос на передачу, если завершился (флаг == 1), то еще пишется статус про запрос (напр. источник сообщения)
           usleep(SLEEP_TIME); // 1 мс
        } while (!recv_completed && !work_completed); // условие: ресив не завершен и не завершена основная работа
        // если бы нужно было проверять только первое условие, можно было бы использовать wait(recv_request), она бы заблокировала исполнение потока, пока не завершился бы ресив

        if (work_completed) {
           MPI_Request_free(&recv_request); // отменяем Irecv
            break; // поток завершится
        }

        TaskT task_to_send;
        if (QueueGet(task_queue, &task_to_send) == NULL) { // если вернули NULL, то в Send отправляем тоже 0 эл-тов (очередь пустая)
            MPI_Send(NULL, 0, MpiTaskT, recv_status.MPI_SOURCE,
                TASK_RESPONSE_TAG, MPI_COMM_WORLD);
            continue;
        }

        MPI_Send(&task_to_send, 1, MpiTaskT, recv_status.MPI_SOURCE, 
            TASK_RESPONSE_TAG, MPI_COMM_WORLD); // отправили туда, откуда спрашивали
    }
}

int main(int argc, char** argv) {
    int provided_thread_support;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided_thread_support); // малтипл - многопоточность, которую запрашиваем, последнее - что дали по факту
    if (provided_thread_support != MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "No thread support\n");
        exit(EXIT_FAILURE);
    }

    double start_time = MPI_Wtime();

    int rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    MPI_Type_contiguous(sizeof(TaskT), MPI_BYTE, &MpiTaskT); // передаем таски (всю структуру целиком), послед-ть байт нужного размера 
    MPI_Type_commit(&MpiTaskT);

    QueueT task_queue; // создали блокирующую очередь
    CreateQueue(&task_queue, NUM_TASKS);

    pthread_t answering_thread; // создали поток, отвечающий на запросы
    int ret_code = pthread_create(&answering_thread, NULL, ProcessRequests, &task_queue); // ProcReq - функция, с которой начинается исполнение потока, в кач-ве аргумента передали нашу очередь
    if (ret_code != 0)
        HandlePthreadError("pthread_create", ret_code);

    for (int i = 0; i < NUM_ITERS; i++) {
        for (int j = 0; j < NUM_TASKS; j++) {
            QueueAdd(&task_queue, MakeTask(rank, comm_size, i)); // заполнили очередь
        }
        RunTasks(&task_queue); // выполняем
    }
    work_completed = 1; // вся работа закончена
    // int stop_flag = STOP_FLAG;
    // MPI_Send(&stop_flag, 1, MPI_INT, rank, TASK_RESPONSE_TAG, MPI_COMM_WORLD);
    ret_code = pthread_join(answering_thread, NULL); // дожидаемся завершения другого потока
    if (ret_code != 0)
        HandlePthreadError("pthread_join", ret_code);

    DestroyQueue(&task_queue);

    double end_time = MPI_Wtime();
    printf("Total time: %lf\n", end_time - start_time);

    MPI_Type_free(&MpiTaskT);
    MPI_Finalize();
}

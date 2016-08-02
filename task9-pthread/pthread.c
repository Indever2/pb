#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

/* Общие ресурсы программы (для всех потоков) */

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int *store;
static int done = 0;

struct thread_info {    /* Аргумент функции thread_start() */

    pthread_t thread_id;        /* ID, возвращаемое pthread_create() */
    int       thread_num;       /* Номер потока */
    char     *argv_string;      /* Аргумент командной строки */
    int       needments;        /* "Потребности" потока */
    int       thread_type;      /* Тип потока */

};

/* Количество "отделов" среды с ресурсами*/
const int STORE_COUNT(){ 

    return 5;

}

/* Функция, выполняемая в потоке */
static void *thread_start(void *arg) { 

    int ran_v = 0;
    struct thread_info *tinfo = arg; // Поток принимает свои свойства

    if (tinfo->thread_type == 0) { //Если мы - клиент:

        printf("Thread %d: IS ONLINE! Needments: \t%d\n\n", tinfo->thread_num, tinfo->needments);

        while (tinfo->needments > 0) {

            sleep(rand() % 4);
            printf("Thread %d is waiting for a mutex\n", tinfo->thread_num);
            pthread_mutex_lock(&mutex); // Пытаемся захватить мьютекс
            printf("Thread %d: got in the protected area\n", tinfo->thread_num);
            ran_v = rand() % 5; // Захватываем мьютекс и посещаем случайный отдел среды с ресурсами
            printf("====> Thread %d took %d points of resource at %d store. Remine: \t%d\n", 
                                                            tinfo->thread_num, store[ran_v], ran_v + 1, tinfo->needments - store[ran_v]);
            /* Забираем ресурсы */
            tinfo->needments -= store[ran_v]; 
            store[ran_v] = 0; 
            pthread_mutex_unlock(&mutex); // Отпускаем мьютекс
            printf("Thread %d: released mutex\n", tinfo->thread_num);

        }

    } else { // Если мы - обслуживающий (GIVER) поток

        printf("GIVER is ONLINE\n\n\n");

        while (!done) { // Пока все потоки-клиенты не завершатся

            for (int i = 0; i < STORE_COUNT(); i++) {

                if (store[i] == 0) { // Если отдел среды с ресурсами пуст, то заполняем его (тоже в мьютексе)

                    sleep(rand() % 2);
                    printf("!!!   GIVER is waiting for a mutex\n");
                    pthread_mutex_lock(&mutex);
                    store[i] = rand() % 70;
                    printf("!!!   ====>> GIVER gave to %d store %d points of resource\n", i, store[i]);
                    pthread_mutex_unlock(&mutex);
                    printf("!!!   GIVER is released mutex\n");

                }

            }

        }

    }

    sleep(rand() % 6);

}

/* Функция, приводящая аргумент типа char* к виду int */
int strtoint(char * in) { 

    int len = 0, sum = 0, multipler = 1; // multipler - множитель, нужен для правилльной позиционировки по разрядам
    len = strlen(in);

    for (int i = 0; i < len; i++) {

        for (int j = 0; j < i; j++)
            multipler *= 10; // После каждой итерации цикла множитель увеличивается в 10 раз.

        sum += (in[len - i - 1] - '0') * multipler; // Прибавляаем к сумме число, полученное умножением извлеченного символа на множитель.
        multipler = 1; // Обнуляем множитель

    }

    return sum;

}

int main(int argc, char **argv) {

    srand(time(NULL));
    store = (int *)malloc(sizeof(int *)* STORE_COUNT()); // Среда с ресурсами

    //Заполнение среды
    for (int i = 0; i < STORE_COUNT(); i++) { 

        store[i] = rand() % 100;

    }   

    void *res; // Возвращаемое значение от завершения потока
    int n, num_threads = strtoint(argv[1]) + 1; // n - хранилще возвраащемых значений int

    pthread_attr_t attr; // Атрибуты потока
    struct thread_info *tinfo; // Свойства потока

    n = pthread_attr_init(&attr); // Инициализируем атрибуты

    if ((tinfo = calloc(num_threads, sizeof(struct thread_info))) < 0) { // Выделяем память под свойства потока

        perror("Calloc fuction error:");
        exit(-1);

    }

    for (int i = 0; i < num_threads; i++) { // Инициализация потоков

        tinfo[i].thread_num = i;
        tinfo[i].needments = rand () % 300 + 100;

        if (i != 0) // Поток с нулевым номером берет на себя тип 1 (Обслуживающий (GIVER) поток)

            tinfo[i].thread_type = 0;

        else

            tinfo[i].thread_type = 1;

        pthread_create(&tinfo[i].thread_id, &attr, &thread_start, &tinfo[i]);

    }

    pthread_attr_destroy(&attr);

    for (int i = 1; i < num_threads; i++) { // Здесь происходит ожидание завершения потоков

        pthread_join(tinfo[i].thread_id, &res);
        printf("Joined with thread %d; returned value was %s\n", tinfo[i].thread_num, (char *) res);
        free(res);

    }

    done = 1; // Все потоки-клиенты завершились, говорим об этом обслуживающему потоку

    printf("TASKS DONE. ALL PROCESSES IS OFFLINE.");
   
    return 0;

}
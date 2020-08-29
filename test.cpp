#include "scheduler.h"
#include <cstdio>

coroutine_t *co1 = nullptr, *co2 = nullptr;

struct args {
    int n;
};

/**
 * @brief 从 n 递减输出偶数
 * 
 * @param args n
 * @return void* 无 
 */
void* test1(void* args)
{
    struct args* u = (struct args*)args;
    int n = u->n;
    for (int i = 0; i < 5; i++) {
        printf("I am test1: %d\n", n);
        n -= 2;
        co_yield(co1);
    }
    return (void*)0;
}

/**
 * @brief 从 n 递减输出奇数
 * 
 * @param args n
 * @return void* 无 
 */
void* test2(void* args)
{
    struct args* u = (struct args*)args;
    int n = u->n;
    for (int i = 0; i < 5; i++) {
        printf("I am test2: %d\n", n);
        n -= 2;
        co_yield(co2);
    }
    return (void*)0;
}

int main()
{
    scheduler_t* sch = sch_create();
    struct args n1 = { 16 };
    struct args n2 = { 15 };
    co1 = co_create(sch, test1, &n1);
    co2 = co_create(sch, test2, &n2);

    while (co_state(co1) && co_state(co2)) {
        co_resume(co1);
        co_resume(co2);
    }

    sch_release(sch);
}
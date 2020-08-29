#include "scheduler.h"

scheduler_t* sch_create()
{
    scheduler_t* sch = (scheduler_t*)malloc(sizeof(scheduler_t)); // 申请内存空间
    memset(sch, 0, sizeof(scheduler_t)); // 内存空间清零
    sch->cap = SCH_DEFAULT_SIZE; // 设置调度器容量
    sch->co_pool = (coroutine_t**)malloc(SCH_DEFAULT_SIZE * sizeof(coroutine_t*)); // 申请协程池空间
    memset(sch->co_pool, 0, SCH_DEFAULT_SIZE * sizeof(coroutine_t*)); // 内存空间清零
    sch->main_co = co_create(sch, nullptr, nullptr); // 创建主协程
    sch->main_co->ctx.ss_size = SCH_STACK_SIZE; // 设置上下文栈
    sch->main_co->ctx.ss_sp = sch->stack; // 设置栈大小
    ctx_make(&(sch->main_co->ctx), nullptr, sch->main_co, nullptr); // 创建上下文
    sch->main_co->state = RUNNING; // 设置状态
    return sch;
}

void sch_release(scheduler_t* sch)
{
    for (int i = 0; i < sch->cap; i++) { // 逐个释放协程
        co_release(sch->co_pool[i]);
    }
    free(sch->co_pool); // 释放协程池
    sch->co_pool = nullptr;
    free(sch); // 释放调度器内存
}
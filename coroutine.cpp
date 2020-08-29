#include "coroutine.h"

extern "C" {
extern void coctx_swap(coctx_t*, coctx_t*) asm("coctx_swap");
};

coroutine_t* co_create(scheduler_t* sch, pfn_co_routine_t pfn, void* args)
{
    coroutine_t* co = (coroutine_t*)malloc(sizeof(coroutine_t)); // 申请内存
    memset(co, 0, sizeof(coroutine_t)); // 内存清零
    co->pfn = pfn; // 设置函数指针
    co->args = args; // 设置函数参数指针
    co->id = sch->size; // 设置协程 id
    co->sch = sch; // 设置调度器指针
    co->buffer = nullptr; // 缓冲区指针为空
    co->size = 0; // 缓冲区大小为 0
    co->cap = 0; // 缓冲区容量为 0
    co->state = READY; // 设置状态为就绪

    if (sch->cap <= sch->size) { // 拓展内存
        sch->co_pool = (coroutine_t**)realloc(sch->co_pool, sch->cap * 2 * sizeof(coroutine_t*));
        sch->cap *= 2;
    }

    for (int i = 0, pos; i < sch->cap; i++) { // 将协程放入适当位置
        pos = (sch->size + i) % sch->cap;
        if (sch->co_pool[pos] == nullptr) { // 位置为空则可以插入
            sch->co_pool[pos] = co;
            break;
        }
    }

    sch->size++;
    return co;
}

void co_release(coroutine_t* co)
{
    if (co == nullptr) {
        return;
    }
    if (co->buffer != nullptr) { // 释放缓冲区
        free(co->buffer);
    }
    co->sch->co_pool[co->id] = nullptr; // 设调度器相应位置为空
    free(co); // 释放协程
}

void co_swap(coroutine_t* cur, coroutine_t* next)
{
    coctx_swap(&(cur->ctx), &(next->ctx));
}

static void co_func(coroutine_t* co, void*)
{
    if (co->pfn) {
        co->pfn(co->args); // 执行协程函数
    }
    co->state = DEAD; // 设置协程状态为死亡
    co->sch->size--; // 存活协程数减一

    co_yield(co); // 切出该协程
    co_release(co); // 关闭释放协程
}

void co_resume(coroutine_t* co)
{
    if (co == nullptr || co->state == DEAD) {
        return;
    }
    switch (co->state) {
    case READY: // 就绪态
        co->state = RUNNING;
        co->ctx.ss_size = SCH_STACK_SIZE; // 设置上下文栈
        co->ctx.ss_sp = co->sch->stack;
        ctx_make(&(co->ctx), (coctx_pfn_t)co_func, co, nullptr);
        coctx_swap(&(co->sch->main_co->ctx), &(co->ctx));
        break;

    case SUSPEND: // 挂起态
        reload_stack(co);
        co->state = RUNNING;
        coctx_swap(&(co->sch->main_co->ctx), &(co->ctx));
        break;
    }
}

void co_yield(coroutine_t* co)
{
    if (co == nullptr) {
        return;
    }
    if (co->state != DEAD) {
        co->state = SUSPEND; // 设置协程状态为挂起
        save_stack(co); // 保存运行时栈到缓冲区
    }

    coctx_swap(&(co->ctx), &(co->sch->main_co->ctx));
}

void save_stack(coroutine_t* co)
{
    // 新创建一个变量，该变量刚刚被分配到栈上
    // 则该变量所在的位置是在运行时栈的其他变量末尾，即栈底
    // 故运行时栈的大小为 stack + SCH_STACK_SIZE - &dummy
    // 即 top - &dummy
    char dummy = 0;
    co->size = co->sch->stack + SCH_STACK_SIZE - &dummy;
    // 若缓冲区容量小于运行时栈大小
    // 则重新分配
    if (co->cap < co->size) {
        free(co->buffer);
        co->cap = co->size;
        co->buffer = (char*)malloc(co->cap);
    }
    memcpy(co->buffer, &dummy, co->size); // 将运行时栈内容保存到缓冲区
}

void reload_stack(coroutine_t* co)
{
    // co->sch->stack + SCH_STACK_SIZE 得到栈底指针
    // 拷贝缓冲区内容到运行时栈
    memcpy(co->sch->stack + SCH_STACK_SIZE - co->size, co->buffer, co->size);
}

int co_state(coroutine_t* co)
{
    if (co == nullptr || co->state == DEAD) { // 如果已释放或死亡返回 DEAD
        return DEAD;
    }
    return co->state;
}
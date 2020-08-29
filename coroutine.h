#pragma once
#include "coctx.h"

const int DEAD = 0; // 死亡
const int READY = 1; // 就绪
const int RUNNING = 2; // 运行
const int SUSPEND = 3; // 挂起

const int SCH_STACK_SIZE = 1024 * 1024; // 默认共享栈大小

struct scheduler_t;

/**
 * @brief 协程调度器
 * 
 */
struct scheduler_t {
    struct coroutine_t* main_co; // 主协程
    struct coroutine_t** co_pool; // 协程数组
    int size; // 协程数组大小
    int cap; // 协程数组容量
    char stack[SCH_STACK_SIZE]; // 运行时栈，其实可以设成动态，或者再封装一次，先从简
};

/**
 * @brief 协程结构体
 * 
 */
struct coroutine_t {
    pfn_co_routine_t pfn; // 函数指针
    coctx_t ctx; // 上下文
    void* args; // 函数参数
    int id; // 协程在调度器中的 id
    struct scheduler_t* sch; // 所属调度器指针
    // char* stack_sp; // 栈顶指针
    char* buffer; // 保存运行时栈的缓冲区指针
    int size; // 缓冲区大小
    int cap; // 缓冲区容量
    int state; // 协程状态
};

/**
 * @brief 在指定的调度器中创建协程并返回指针
 * 
 * @param sch 调度器指针
 * @param pfn 函数指针
 * @param args 函数参数
 * @return coroutine_t* 创建的协程指针
 */
coroutine_t* co_create(scheduler_t* sch, pfn_co_routine_t pfn, void* args);

/**
 * @brief 关闭协程并释放其内存
 * 
 * @param co 需要关闭的协程指针
 */
void co_release(coroutine_t* co);

/**
 * @brief 启动协程
 * 
 * @param co 要启动的协程的指针
 */
void co_resume(coroutine_t* co);

/**
 * @brief 切出协程
 * 
 * @param co 要切出的协程的指针
 */
void co_yield(coroutine_t* co);

/**
 * @brief 保存运行时栈到协程缓冲区
 * 
 * @param co 相应协程指针
 */
void save_stack(coroutine_t* co);

/**
 * @brief 从协程缓冲区恢复运行时栈
 * 
 * @param co 相应协程指针
 */
void reload_stack(coroutine_t* co);

/**
 * @brief 返回协程状态
 * 
 * @param co 相应协程指针
 * @return int 协程状态
 */
int co_state(coroutine_t* co);
#pragma once
#include "coroutine.h"

const int SCH_DEFAULT_SIZE = 16; // 默认协程池大小

/**
 * @brief 创建一个新的调度器并返回其指针
 * 
 * @return scheduler_t* 返回的调度器指针
 */
scheduler_t* sch_create();

/**
 * @brief 关闭调度器并释放其内存
 * 
 * @param sch 需要关闭的调度器
 */
void sch_release(scheduler_t* sch);
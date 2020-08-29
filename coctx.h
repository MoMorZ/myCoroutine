#pragma once
#include <cstdlib>
#include <cstring>

typedef void* (*coctx_pfn_t)(void* s, void* s2);

//-------------
// 64 bit
// low | regs[0]: r15 |
//    | regs[1]: r14 |
//    | regs[2]: r13 |
//    | regs[3]: r12 |
//    | regs[4]: r9  |
//    | regs[5]: r8  |
//    | regs[6]: rbp |
//    | regs[7]: rdi |
//    | regs[8]: rsi |
//    | regs[9]: ret |  //ret func addr
//    | regs[10]: rdx |
//    | regs[11]: rcx |
//    | regs[12]: rbx |
// hig | regs[13]: rsp |

struct coctx_t {
    void* regs[14]; // x64 架构，14个寄存器
    size_t ss_size; // 栈大小
    char* ss_sp; // 栈指针
};

enum {
    kRDI = 7,
    kRSI = 8,
    kRETAddr = 9,
    kRSP = 13,
};

typedef void* (*pfn_co_routine_t)(void*);

/**
 * @brief 创建上下文，初始化栈和跳转点
 * 
 * @param ctx 上下文指针
 * @param pfn 协程函数指针
 * @param s 协程指针
 * @param s1 协程函数参数指针
 * @return int 0
 */
int ctx_make(coctx_t* ctx, coctx_pfn_t pfn, const void* s, const void* s1);
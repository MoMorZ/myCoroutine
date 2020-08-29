#include "coctx.h"

/**
 * @brief 切换上下文，并跳转到调用该函数的下一条语句
 * 
 */
extern "C" {
extern void coctx_swap(coctx_t*, coctx_t*) asm("coctx_swap");
};

int ctx_make(coctx_t* ctx, coctx_pfn_t pfn, const void* s, const void* s1)
{
    char* sp = ctx->ss_sp + ctx->ss_size; // 栈顶指针从高地址开始
    sp = (char*)((unsigned long)sp & -16LL); // 内存对齐，后4位清零
    memset(ctx->regs, 0x0, sizeof(ctx->regs)); // 寄存器清零
    void** ret_addr = (void**)(sp); // ra 返回地址
    *ret_addr = (void*)pfn; // 设置返回地址指向协程函数指针
    ctx->regs[kRSP] = sp; // rsp 设置为栈底
    ctx->regs[kRETAddr] = (char*)pfn; // ret 设置为协程函数
    ctx->regs[kRDI] = (char*)s; // rdi 设置为当前协程
    ctx->regs[kRSI] = (char*)s1; // rsi 设置为将要切换协程
    return 0;
}
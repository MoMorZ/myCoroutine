.globl coctx_swap
#if !defined( __APPLE__ )
.type  coctx_swap, @function
#endif
coctx_swap:
    // x64 传参规则 rdi 是第一个参数，rsi 是第二个参数
    // 前四个总是放在寄存器中传递，大于4个则压栈
    leaq (%rsp),%rax // 先保存 rsp，rax 按约定存放的是调用后的返回值，没必要保存
    // rdi 存放的是 cur.coctx_t 的地址，即当前协程的上下文的地址
    movq %rax, 104(%rdi) // 下面的都是保存寄存器，从高地址开始入栈保存，即 regs[13]，实际上就是 rsp
    movq %rbx, 96(%rdi)
    movq %rcx, 88(%rdi)
    movq %rdx, 80(%rdi)
    movq 0(%rax), %rax // 即程序编译后 coctx_swap 执行完的第一条编译指令的地址
    movq %rax, 72(%rdi) // 当前协程的返回地址 ret
    movq %rsi, 64(%rdi)
    movq %rdi, 56(%rdi)
    movq %rbp, 48(%rdi)
    movq %r8, 40(%rdi)
    movq %r9, 32(%rdi)
    movq %r12, 24(%rdi)
    movq %r13, 16(%rdi)
    movq %r14, 8(%rdi)
    movq %r15, (%rdi)
    xorq %rax, %rax // rax 清零

    // rsi 存放的是 next.coctx_t 的地址，即要切换的协程的上下文的地址
    movq 48(%rsi), %rbp // 下面的都是取出寄存器的值，从低地址开始，即 regs[0]
    movq 104(%rsi), %rsp
    movq (%rsi), %r15
    movq 8(%rsi), %r14
    movq 16(%rsi), %r13
    movq 24(%rsi), %r12
    movq 32(%rsi), %r9
    movq 40(%rsi), %r8
    movq 56(%rsi), %rdi
    movq 80(%rsi), %rdx
    movq 88(%rsi), %rcx
    movq 96(%rsi), %rbx
    // 下面两句以及最后的 ret 负责让 rip 指向将要切换的协程的返回地址
    // 因为 rip 寄存器不能直接修改，所以通过 ret 指令修改
    leaq 8(%rsp), %rsp // coctx_swap 被 call 时会先 jmp 再进行 push，所以这里加 8 实际上是把减少的 rsp 加回来
    pushq 72(%rsi) // 把将要切换协程的返回地址 ret 压栈

    movq 64(%rsi), %rsi
    ret // 这里实际上做了 popq %rip，所以 rip 就指向了将要切换协程的返回地址

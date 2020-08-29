# myCoroutine

一个轻量的玩具协程库，汇编切换，提供了协程创建切换调度功能。

可继续完善的部分：HOOK以及通信。

对外使用接口如下：

- `sch_create`

  创建返回调度器

- `sch_release`

  关闭并释放调度器

- `co_create`

  创建协程

- `co_resume`

  唤醒协程

- `co_yield`

  切出协程
  
- `co_state`

  返回协程状态
  
- `co_release`

  关闭并释放协程
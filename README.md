# 基于跳表实现的键值型存储引擎

## 1 项目说明

**<font color = 008F88>Key-Value 存储引擎</font>**  :sunflower:

基于 **<font color = F88F00>跳表</font>** 实现的 KV 存储引擎，使用 C++ 实现。在随机读写情况下，该项目每秒可处理请求数（QPS）：24.39 W，每秒可处理读请求数：18.41 W。（项目作者说明，但笔者测试不相同）

**提供接口** ：

:small_blue_diamond: put_element  插入/修改数据

:small_orange_diamond: delete_element 删除数据

:small_blue_diamond: search_element 查询数据

:small_blue_diamond: display_list  打印跳跃表

:small_orange_diamond: size  返回数据规模

:small_blue_diamond: clear  清空跳表 （笔者添加）

----

## 2 项目测试

-----

### 2.1  增删改查 QPS 测试

对增删改查进行测试。测试方法：设定固定的操作数，设置层高为 18，采用随机插入数据，看各个操作运行时间。

具体代码见：`stress-test/stress_test.cpp`

**<font color = 008F88>运行设备</font>** ：M1 MacBook Air(8 cores CPU, 16GB RAM)

**<font color = 008F88>结果统计</font>** ：

| 具体操作\数据规模 |    100 w / 1 thread    |    500 w / 5 threads   |   1000 w / 10 threads  |
| :---------------: | :--------: | :--------: | :-------: |
|       插入/修改        | 1.62168 s | 21.0381 s | 45.6914 s |
|       查看        | 1.86142 s |  3.85872 s | 8.97652 s |
|       删除        |  1.68749 s | 21.2134 s | 46.8473 s |


以 100 w 数据结果计算 QPS:

:small_blue_diamond: **<font color = FF8888>插入/修改</font>** ：61.72 w 
:small_orange_diamond: **<font color = FF8888>删除</font>** ：53. 76 w  
:small_orange_diamond: **<font color = FF8888>查看</font>** ：59.52 w

reference：

*1*：[Skiplist-CPP: A tiny KV storage based on skiplist written in C++ language ](https://github.com/youngyangyang04/Skiplist-CPP)


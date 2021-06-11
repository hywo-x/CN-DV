# 计网实验二

## 描述

见doc目录下的pdf文档。

<br></br>

## 需求

1.  初始化
    -   [x] 读入参数，通过对应的文件确定与其他路由的距离(float)和相应的端口号
    -   [x] 读取系统配置文件，获取有关时间间隔、不可达距离、最大等待时间等
    -   [x] 实现各种数据结构的初始化
2.  需要实现的功能
    1. 实现各路由之间的通信
        - [ ] 通过socket实现本机不同端口间的通信
    2. 需要定期发送和接收路由信息

       - [ ] 发送
           - [x] 计时器
           - [ ] 发送函数
           - [ ] 日志

       - [ ] 接收
           - [ ] 接收函数
           - [ ] 更新路由表
           - [ ] 计时器（太长时间未收到会判定为故障）
           - [ ] 日志（可选）
    3. 暂停、恢复、退出
       - [ ] 通过屏幕输入判断
3.  数据结构的设计
	- [x] 路由消息的传输
	- [x] 每个路由的路由表
	- [x] 邻居路由记录

<br></br>

## 实现

### 路由表数据结构设计

1. 最多有30个路由器和5个邻居节点
2. 存储邻居节点：用一个`vector<struct neighbor>`存
3. 存储和维护目标节点距离：用`vector<struct node>`存
4. 邻居节点的文件：第一行一个数字，表示邻居数量；以后每行表示一个节点，顺序是节点、距离、端口号，最后没有多余的回车
5. 配置文件：三个参数，分别表示发送时间间隔（单位ms）、最大距离、最大等待时间（单位ms）

### 消息发送

1. 通过字符串拼接，将路由表以字符串的形式发送出去
2. 通过计时实现定时循环发送
3. 发送格式如下：
```
%d(source_id) %d(dest_id) %f(distance) %d(dest_id) %f(distance)
（重复直到发送完全部，在考虑用换行还是用空格区分）
```

### 消息接收

1. 初始化时创建一个邻居路由的vector<struct neighbor>，记录邻居节点名称、距离和状态（是否故障）
2. 接收到消息时，设法切割字符串，通过souce_id查map得到neighbor_distance（局部变量），再把后面的切割得到dest_id和(final_distance=distance+neighbor_distance)组成的map<string, float>，通过查表和判断进行路由表的更新，同时更新计时器
3. 初始化时同时为每个邻居节点设置一个计时器（试试看能不能通过传neighbor_id实现计时器的初始化），可以考虑另开一个线程，要求出现超时的时候及时将邻居节点的状态进行更新，且在收到消息时能够重置计时器

### socket通信

建议在Linux下写，发送直接用一对一的发送，接收用多个线程、同一端口，不知道会不会冲突

### 暂停和恢复

不知道，不会写，再说吧

<br></br>

## 实验日志

### 6.3

列出了实验需求，写了一点实现

### 6.4

初步完成实现

### 6.8

- 完成的工作
    - 简单列举了需要的函数
    - 对路由表和邻居节点的数据结构进行了修改
    - 进行了linux和vscode环境的配置
    - 完成了初始化部分邻居节点信息的读入
- 遇到的困难
    - 多线程不会写
    - 一些函数在windows下和linux下的用法和表现不同，浪费了不少时间

### 6.9
- 完成的工作
  - 完成了初始化部分配置文件的读入
  - 改正了初始化部分的各种bug，初始化部分基本完成
  - 对初始化时两个文件的格式进行了补充
- 遇到的困难
  - atoi和atof函数有点问题，昨天跑通了但是今天又出现bug了,准备用fopen和fscanf重写
  > 无法打开“strtod_l.c”: 无法读取文件'/build/glibc-eX1tMB/glibc-2.31/stdlib/strtod_l.c' (Error: 无法解析不存在的文件"/build/glibc-eX1tMB/glibc-2.31/stdlib/strtod_l.c")。
  - 对std::map不够熟悉，写了一半才发现不能用map存，只好改用vector
  - 进度太慢
- 计划
  1. 掌握多线程编程，写好计时器
  2. 写出socket通信
  3. 把socket嵌入代码
  4. 调试
  5. 加入键盘控制部分
  6. 调试

### 6.10
- 完成的工作
  - 上午修了几个bug（差点就重装虚拟机了）
  - 完成多线程的基础部分，计时器打算直接用sleep()实现
  - 设计好了多对一的接收（开多个线程收），希望同一端口接收多个来源的数据报不会发生冲突
  - 写了一点发送的socket（主要是不同端口通信的资料有点少），进度有点慢，要加速了
- 困难
  - 进度太慢
  - 不确定这样通信会不会乱
  - 简单试了一下，发送好像不太成功

### 6.11
- 完成的工作
  - 上午完成了接收部分的函数
  - 下午测试了一下，在ljl大佬的帮助下发现了bug，成功实现路由之间的互相通信
  - 目前未完成的功能：日志（这个不难）、键盘控制
- 遇到的困难
  - 键盘控制部分逻辑不清晰

<br></br>

## 笔记
1. C++文件的读写可以使用fstream (ifstream, ofstream) ,通过open()方法打开文件,通过read()读取内容。
2. 注意fstream.read()有点坑，在windows下和linux下的表现有点不一样，在windows上遇到了一个问题，调了半天，浪费了不少时间
3. 字符串切割这里，没有python简单快捷的split()，只能用strtok_s了，函数用法可以参照百度百科，读取完一行之后就将对应的节点信息加入
4. 因为改用了fopen和fscanf，所以上面几点全都白写了
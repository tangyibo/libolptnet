
### C++网络编程库libolptnet
=========================

### 一、简介
    基于epoll/poll/select模型I/O复用的多线程网络编程框架，多线程部分参考
了chenshuo的muduo库one loop per thread 的设计思想。

### 二、功能
#### 1、epoll/poll/select模型I/O复用的Reactor框架；
#### 2、基于std::stringstream实现的缓存Buffer;
#### 3、one loop per thread 的多线程I/O复用框架,但不使用boost和c++11；
#### 4、基于timerfd/epoll等实现的定时器；
#### 5、在python目录下提供了python到实现

### 三、目标
#### 1、在研究muduo库的基础上，实现一个类是简易的TCP网络库；
#### 2、只需要C++03语法,无需C++11及更高的语法支持；
#### 3、不依赖boost库,不使用C++11或boost库的bind和function等；
#### 4、使用传统封装、继承与多态的特性实现，继承关系不超过2级；

### 四、编译
  ##### cd libolptnet/
  ##### make all
  ##### 即可在bin/目录先生成libolptnet.a静态库，其中在src/example中有example-1至example-4个
使用示例。可运行测试。 

### 五、联系
  ##### E-mail : inrgihc@126.com

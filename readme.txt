
# C++网络编程库libolptnet
=========================

一、简介
  实现了一套基于epoll/poll/select模型I/O复用的多线程网络编程框架，多线程部分参考
了chenshuo的muduo库one loop per thread 的设计思想。

二、功能
 1、epoll/poll/select模型I/O复用的Reactor框架；
 2、基于std::stringstream实现的缓存Buffer;
 3、one loop per thread 的多线程I/O复用框架；
 4、基于timerfd/epoll等实现的定时器；

三、依赖
  C++98,不依赖其他库

四、编译
  cd libolptnet/
  make all
  即可在bin/目录先生成libolptnet.a静态库，其中在src/example中有example-1至example-4个
使用示例。可运行测试。 

五、联系
  E-mail:inrgihc@126.com

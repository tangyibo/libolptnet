#! /usr/bin/python
import sys, os
import ConfigParser
import logging
import logging.config
import socket
import select
import Queue
import threading
from io import BytesIO


class EVENT:
    NULL =  0   # no event
    READ =  1   # read event
    WRITE = 2   # write event
    ERROR = 4   # error event
    ALL =   7   # read/write/error event


class Callback:
    def __init__(self, instance, function_name):
        self.__instance = instance
        self.__function_name = function_name

    def action(self):
        self.__instance.__getattribute__(self.__function_name)()


class EventHandler(object):

    def do_close(selfs):
        pass

    def do_read(self):
        pass

    def do_write(self):
        pass


class ServiceLoop:

    def __init__(self):
        self.__inputs = []
        self.__outputs = []
        self.__excepts = []
        self.__handlers = {}
        self.__functors = Queue.Queue()
        self.__lock = threading.Lock()

    def __del__(self):
        self.__handlers.clear()
        self.__functors.clear()

    def add_functor(self,function):
        self.__lock.acquire()
        self.__functors.put(function)
        self.__lock.release()

    def register(self, handle, handler, event=EVENT.ALL):
        if EVENT.READ & event > 0 and handle not in self.__inputs:
            self.__inputs.append(handle)

        if EVENT.WRITE & event > 0 and handle not in self.__outputs:
            self.__outputs.append(handle)

        if EVENT.ERROR & event > 0 and handle not in self.__excepts:
            self.__excepts.append(handle)

        self.__handlers[handle] = handler

    def unregister(self, handle):
        if handle in self.__inputs:
            self.__inputs.remove(handle)
        if handle in self.__outputs:
            self.__outputs.remove(handle)
        if handle in self.__excepts:
            self.__excepts.remove(handle)

        if self.__handlers.has_key(handle):
            del self.__handlers[handle]

    def modify(self, handle, handler, event):
        self.unregister(handle)
        self.register(handle, handler, event)

    def run(self,for_ever=True):
        logger.info("[%d] start run loop!" % (threading.currentThread().ident))

        while for_ever:
            fd_inputs=self.__inputs
            fd_outputs=self.__outputs
            fd_excepts=self.__excepts

            if len(fd_inputs)>0 or len(fd_outputs)>0 or len(fd_excepts):
                rlist, wlist, eList = select.select(fd_inputs, fd_outputs, fd_excepts, 0.5)
                for r in rlist:
                    self.__handlers[r].do_read()

                for w in wlist:
                    self.__handlers[w].do_write()

                for e in eList:
                    if e in self.__inputs:
                        self.__inputs.remove(e)
                    if e in self.__outputs:
                        self.__outputs.remove(e)

                    self.__handlers[e].do_close()

            while not self.__functors.empty():
                callback=self.__functors.get_nowait()
                callback.action()


class ServiceLoopThread(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.__loop=None
        self.__cond = threading.Condition()
        self.__lock = threading.Lock()

    def get_loop(self):
        ret=None

        self.__cond.acquire()
        while self.__loop is None:
            self.__cond.wait()
        self.__cond.release()

        self.__lock.acquire()
        ret=self.__loop
        self.__lock.release()

        return ret

    def run(self):
        loop=ServiceLoop()

        self.__lock.acquire()
        self.__loop=loop
        self.__lock.release()

        self.__cond.acquire()
        self.__cond.notify()
        self.__cond.release()

        loop.run()


class ThpoolServiceLoop:
    __base_loop=None
    __index=0
    __loop_threads=[]
    __loop_service=[]


    def __init__(self,loop):
        self.__base_loop=loop
        self.__index=0

    def start(self,thread_num):
        if thread_num <=0:
            return

        for i in range(0,thread_num):
            one_thread=ServiceLoopThread()
            self.__loop_threads.append(one_thread)

            one_thread.start()
            self.__loop_service.append(one_thread.get_loop())

    def get_loop(self):
        capacity=len(self.__loop_service)
        if 0==capacity:
            return self.__base_loop

        ret=self.__loop_service[(self.__index)%capacity]
        self.__index=self.__index+1
        return  ret


class SocketBuffer:

    def __init__(self):
        self.__buffer=BytesIO()

    def push(self,data):
        return self.__buffer.write(data)

    def take(self):
        data= self.__buffer.getvalue()
	self.__buffer.seek(0,0)
	return data;

class TCPConnection(EventHandler):

    def __init__(self, loop, client, address):
        self.__loop = loop
        self.__handle = client
        self.__address = address

        self.__recv_buffer=SocketBuffer()
        self.__send_buffer=SocketBuffer()

    def open(self):
        self.__loop.register(self.__handle,self, EVENT.READ)
        self.__handle.settimeout(60)
        self.__handle.setblocking(False)
        logger.info("[%d] Get a client from %s" % (threading.currentThread().ident, self.__address))

    def do_close(self):
        self.on_close()
        self.__loop.unregister(self.__handle)
        self.__handle.close()

    def do_read(self):
        msg = self.__handle.recv(1024)
        if not msg or len(msg) <= 0:
            logger.info("[%d] recv msg is 0,close client" % (threading.currentThread().ident))
            self.do_close()
            return

        self.__recv_buffer.push(msg)

        while len(msg) == 1024:
            msg = self.__handle.recv(1024)
            if not msg or len(msg) <= 0:
                break
            else:
                self.__recv_buffer.push(msg)

        data=self.__recv_buffer.take()
        if len(data)>0:
            self.on_read(data)

    def do_write(self):
        data=self.__send_buffer.take()
        if len(data)>0:
            self.__handle.sendall(data)

        self.__loop.modify(self.__handle,self, EVENT.READ)

    def send(self, data):
        self.__send_buffer.push(data)
        self.__loop.modify(self.__handle,self, EVENT.WRITE)

    def on_read(self,data):
        logger.info("[%d] recv msg :%s" % (threading.currentThread().ident, data))
        self.send(data)

    def on_close(self):
        pass


class TCPAcceptor(EventHandler):

    def __init__(self, loop, host, port):
        self.__loop = loop
        self.__host = host
        self.__port = port
        self._thpool=ThpoolServiceLoop(loop)
        self._server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def open(self,thread_num):
        try:
            self._server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self._server.bind((self.__host, self.__port))
            self._server.listen(10)
            self._server.setblocking(False)
        except Exception, e:
            logger.error("%d: %s" % (e.args[0], e.args[1]))
            sys.exit(1)

        logger.info("[%d] start thread pool count is %d" % (threading.currentThread().ident, thread_num))
        self._thpool.start(thread_num)
        self.__loop.register(self._server,self,EVENT.READ)
        logger.info("[%d] server listen on %s:%d" % (threading.currentThread().ident, self.__host, self.__port))

    def close(self):
        self._server.close()

    def do_read(self):
        client, address = self._server.accept()
        self.on_accept(client,address)

    def on_accept(self, client, address):
        pass

##############################################################
class MyConnection(TCPConnection):

    def on_read(self, data):
        logger.info("[%d] recv msg :%s" % (threading.currentThread().ident, data))
        self.send(data)

    def on_write(self):
        pass

    def on_close(self):
        pass


class MyAcceptor(TCPAcceptor):

    def __init__(self, loop, host, port,HandleClass):
        TCPAcceptor.__init__(self, loop, host, port)
        self.HandleClass=HandleClass

    def on_accept(self, client, address):
        loop = self._thpool.get_loop()
        conn = self.HandleClass(loop, client, address)
        cb = Callback(conn, conn.open.__name__)
        loop.add_functor(cb)


if __name__ == "__main__":
    reload(sys)
    sys.setdefaultencoding("utf-8")

    logging.config.fileConfig("./logger.conf")
    logger = logging.getLogger("example")

    loop=ServiceLoop()
    server = MyAcceptor(loop,'0.0.0.0',50007,MyConnection)
    server.open(thread_num=10)
    loop.run()

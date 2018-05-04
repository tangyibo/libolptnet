#
#
BINDIR = ./bin

S_EXES = example_server
C_EXES = test_client
LIB_NET = olptnet

RM :=rm -f 

LIB_OBJS =src/net/tcp_socket_addr.o \
	src/net/async_acceptor.o  \
	src/net/connection_handler.o \
	src/net/stream_buffer.o \
	src/net/event_dispatcher.o \
	src/net/service_loop.o  \
	src/net/service_loop_thread.o \
	src/net/thpool_service_loop.o \
	src/net/net_base.o \
	src/net/timer_handler.o 
	
S_OBJS_1=src/example/example-1.o

S_OBJS_2=src/example/example-2.o \
	src/example/acceptor_server.o \
	src/example/service_handler.o 

S_OBJS_3=src/example/example-3.o \
	src/example/acceptor_server.o \
	src/example/service_handler.o

S_OBJS_4=src/example/example-4.o

C_OBJS =src/client/main.o

CXXFLAGS = -g -Wall
CPPFLAGS = -I./src
LIBS =-L./lib -lpthread -lrt

all: libs server1 server2 server3 server4 client

dir:
	if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR) ; fi;

libs:  dir $(LIB_NET)
	
$(LIB_NET): $(LIB_OBJS)
	ar -cr $(BINDIR)/lib$@.a  $^

server1:   $(S_OBJS_1)
	g++ $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/$(S_EXES)_1  $^ -L$(BINDIR) -l$(LIB_NET) $(LIBS)

server2: $(S_OBJS_2)
	g++ $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/$(S_EXES)_2  $^ -L$(BINDIR) -l$(LIB_NET) $(LIBS)

server3: $(S_OBJS_3)
	g++ $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/$(S_EXES)_3  $^ -L$(BINDIR) -l$(LIB_NET) $(LIBS)
	
server4:   $(S_OBJS_4)
	g++ $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/$(S_EXES)_4  $^ -L$(BINDIR) -l$(LIB_NET) $(LIBS)

client:  
	g++ $(CXXFLAGS) $(CPPFLAGS) -o $(BINDIR)/$(C_EXES)  src/client/main.cc -L$(BINDIR) -l$(LIB_NET) 
	
clean:
	$(RM) $(LIB_OBJS) $(S_OBJS_1) $(S_OBJS_2) $(S_OBJS_3)  $(S_OBJS_4) $(C_OBJS)
	$(RM) $(BINDIR)/*
#
#

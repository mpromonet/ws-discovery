GSOAP_PREFIX=/usr
GSOAP_BASE=$(GSOAP_PREFIX)/share/gsoap
GSOAP_IMPORT=$(GSOAP_BASE)/import
GSOAP_PLUGINS=$(GSOAP_BASE)/plugin
GSOAP_CFLAGS= -I gen -I $(GSOAP_PREFIX)/include -I $(GSOAP_PLUGINS)
GSOAP_LDFLAGS= -lgsoap++ -L $(GSOAP_PREFIX)/lib/

all: gen wsd-probe.exe wsd-server.exe wsd-proxy.exe

gen:
	mkdir -p gen

gen/soapC.cpp: gen
	$(GSOAP_PREFIX)/bin/soapcpp2 -2LCx $(GSOAP_IMPORT)/wsdd10.h -d gen

libwsdd.a: gen/soapC.cpp gen/soapClient.cpp wsd-server.cpp
	mkdir -p .objs
	$(CXX) -o .objs/wsddapi.o -c $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_CFLAGS)
	$(CXX) -o .objs/wsaapi.o -c $(GSOAP_PLUGINS)/wsaapi.c $(GSOAP_CFLAGS)
	$(CXX) -o .objs/soapClient.o -c gen/soapClient.cpp $(GSOAP_CFLAGS)
	$(CXX) -o .objs/wsd-server.o -c wsd-server.cpp $(GSOAP_CFLAGS)
	$(AR) rcs $@ .objs/*.o

wsd-probe.exe:   $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c  gen/soapC.cpp gen/soapClient.cpp probe.cpp 
	$(CXX) -g -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 
	
wsd-server.exe: server.cpp gen/soapC.cpp libwsdd.a
	$(CXX) -g -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 

wsd-proxy.exe:   $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c  gen/soapC.cpp gen/soapClient.cpp proxy.cpp
	$(CXX) -g -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 
	
	
clean:
	rm -rf *.exe gen .objs



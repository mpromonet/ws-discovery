/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** server.cpp
** 
** WS-Discovery device
**
** -------------------------------------------------------------------------*/

#include <signal.h>

#include "wsd-server.h"

const int   _metadataVersion = 1;
const char* _xaddr="http://localhost/service";
const char* _type="\"http://schemas.xmlsoap.org/ws/2006/02/devprof\":device";
const char* _scope="scope";
const char* _endpoint="urn";

wsdconf conf(_xaddr, _type);

void sighandler(int sig)
{
	std::cout<< "Signal caught..." << std::endl;
	conf.m_stop = true;
}
	
int main(int argc, char** argv)
{
	int c=0;
	while ( (c=getopt(argc, argv, "h" "r:t:s:x:")) != -1)
	{
		switch (c)
		{
			case 'r': conf.m_endpoint = optarg; break;
			case 't': conf.m_type     = optarg; break;
			case 's': conf.m_scope    = optarg; break;
			case 'x': conf.m_xaddr    = optarg; break;
			default:
				std::cout << argv[0] << "[-r <endpoint>] [-t <type>] [-s <scope>] [-x <xaddr>]" << std::endl;
				return -1;
			break;
		}
	}

	signal(SIGINT, &sighandler);
	wsd_server(conf);

	return 0;
}


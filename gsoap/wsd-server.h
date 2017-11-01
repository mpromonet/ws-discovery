/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** wsd-server.cpp
** 
** WS-Discovery device
**
** -------------------------------------------------------------------------*/

#include <string>
#include "wsddapi.h"

struct wsdconf {
	wsdconf(const char* xaddr, const char* type, const char* scope = "", const char* endpoint = "", int   metadataVersion = 0)
		: m_xaddr(xaddr), m_type(type), m_scope(scope), m_endpoint(endpoint), m_metadataVersion(metadataVersion), m_stop(0)  {}
	std::string m_xaddr;
	std::string m_type;
	std::string m_scope;
	std::string m_endpoint;
	int         m_metadataVersion;
	int         m_stop;
};

int wsd_server(const wsdconf & conf);

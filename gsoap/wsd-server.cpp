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

#include "wsd-server.h"

int sendHello(soap* serv)
{
	wsdconf* conf = (wsdconf*)serv->user;
	
	struct soap* soap = soap_new1(SOAP_IO_UDP);
	printf("call soap_wsdd_Hello\n");
	int res = soap_wsdd_Hello(soap,
	  SOAP_WSDD_ADHOC,      // mode
	  "soap.udp://239.255.255.250:3702",         // address of TS
	  soap_wsa_rand_uuid(soap),                   // message ID
	  NULL,                 
	  conf->m_endpoint.c_str(),
	  conf->m_type.c_str(),
	  conf->m_scope.c_str(),
	  NULL,
	  conf->m_xaddr.c_str(),
          conf->m_metadataVersion);
	if (res != SOAP_OK)
	{
		soap_print_fault(soap, stderr);
	}
	soap_destroy(soap);	
	soap_end(soap);	
	
	return soap_wsdd_listen(serv, -1000000);
}

int sendBye(soap* serv)
{
	wsdconf* conf = (wsdconf*)serv->user;
	
	struct soap* soap = soap_new1(SOAP_IO_UDP);
	printf("call soap_wsdd_Bye\n");
	int res = soap_wsdd_Bye(soap,
	  SOAP_WSDD_ADHOC,      // mode
	  "soap.udp://239.255.255.250:3702",         // address of TS
	  soap_wsa_rand_uuid(soap),                   // message ID
	  conf->m_endpoint.c_str(),                 
	  conf->m_type.c_str(),
	  conf->m_scope.c_str(),
	  NULL,
	  conf->m_xaddr.c_str(),
          conf->m_metadataVersion);
	if (res != SOAP_OK)
	{
		soap_print_fault(soap, stderr);
	}
	soap_destroy(soap);	
	soap_end(soap);
	
	return soap_wsdd_listen(serv, -1000000);
}

int wsd_server(const wsdconf & conf)
{
	struct soap* serv = soap_new1(SOAP_IO_UDP); 
	serv->bind_flags=SO_REUSEADDR;
	if (!soap_valid_socket(soap_bind(serv, NULL, 3702, 1000))) {
		soap_print_fault(serv, stderr);
		
	} else {		
		ip_mreq mcast; 
		mcast.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
		mcast.imr_interface.s_addr = htonl(INADDR_ANY);
		if (setsockopt(serv->master, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast, sizeof(mcast))<0) {
			std::cout << "group membership failed:" << strerror(errno) << std::endl;
			
		} else {
			serv->user = (void*)&conf;
			
			sendHello(serv);
			while (!conf.m_stop) {
				soap_wsdd_listen(serv, -1000000);
			}
			sendBye(serv);			
		}
	}

	soap_destroy(serv);	
	soap_end(serv);	

	return 0;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
	printf("wsdd_event_Hello\tid=%s EndpointReference=%s Types=%s Scopes=%s XAddrs=%s\n", MessageID, EndpointReference, Types, Scopes, XAddrs);
}

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
	printf("wsdd_event_Bye\tid=%s EndpointReference=%s Types=%s Scopes=%s XAddrs=%s\n", MessageID, EndpointReference, Types, Scopes, XAddrs);
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
	wsdconf* conf = (wsdconf*)soap->user;
	printf("wsdd_event_Resolve\tid=%s replyTo=%s endpoint=%s\n", MessageID, ReplyTo, EndpointReference);
	if (EndpointReference && (conf->m_endpoint == EndpointReference) )
	{
		soap_wsdd_ResolveMatches(soap, NULL, soap_wsa_rand_uuid(soap), MessageID, ReplyTo, conf->m_endpoint.c_str(), conf->m_type.c_str(), conf->m_scope.c_str(), NULL, conf->m_xaddr.c_str(), conf->m_metadataVersion);	
	}
	return SOAP_WSDD_ADHOC;
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	wsdconf* conf = (wsdconf*)soap->user;
	printf("wsdd_event_Probe\tid=%s replyTo=%s types=%s scopes=%s\n", MessageID, ReplyTo, Types, Scopes);
	soap_wsdd_init_ProbeMatches(soap,matches);
	soap_wsdd_add_ProbeMatch(soap, matches, conf->m_endpoint.c_str(), conf->m_type.c_str(), conf->m_scope.c_str(), NULL, conf->m_xaddr.c_str(), conf->m_metadataVersion);
	soap_wsdd_ProbeMatches(soap, NULL, soap_wsa_rand_uuid(soap) , MessageID, ReplyTo, matches);
	return SOAP_WSDD_ADHOC;
}


#include "gen/wsdd.nsmap"
#include "wsddapi.h"

int main(int argc, char** argv)
{
	const char* host = "239.255.255.250";	
	int port = 3702;
	
	// create answer listener
	struct soap* serv = soap_new1(SOAP_IO_UDP); 
	serv->bind_flags=SO_REUSEADDR;
	serv->connect_flags = SO_BROADCAST; 
	if (!soap_valid_socket(soap_bind(serv, NULL, port, 1000)))
	{
		soap_print_fault(serv, stderr);
		exit(1);
	}	
	ip_mreq mcast; 
	mcast.imr_multiaddr.s_addr = inet_addr(host);
	mcast.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(serv->master, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast, sizeof(mcast))<0) 
	{
		std::cout << "group membership failed:" << strerror(errno) << std::endl;		
	}		
		
	// send probe request
	printf("call soap_wsdd_Probe\n");
	int res = soap_wsdd_Probe(serv,
	  SOAP_WSDD_ADHOC,      // mode
	  SOAP_WSDD_TO_TS,      // to a TS
	  "soap.udp://239.255.255.250:3702",         // address of TS
	  soap_wsa_rand_uuid(serv),                   // message ID
	  NULL,                 // ReplyTo
	  NULL,
	  NULL,
	  "");
	if (res != SOAP_OK)
	{
		soap_print_fault(serv, stderr);
	}
	
	// listen answers
	res = soap_wsdd_listen(serv, 5);
	if (res != SOAP_OK)
	{
		soap_print_fault(serv, stderr);
	}
	return 0;
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
	printf("wsdd_event_Resolve\n");
	return SOAP_WSDD_ADHOC;
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	printf("wsdd_event_Probe id=%s types=%s scopes=%s\n", MessageID, Types, Scopes);
	return SOAP_WSDD_ADHOC;
}

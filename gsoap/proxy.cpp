/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** proxy.cpp
** 
** WS-Discovery proxy
**
** -------------------------------------------------------------------------*/

#include <signal.h>

#include "gen/wsdd.nsmap"
#include "wsddapi.h"

bool stop = false;
void sighandler(int sig)
{
	std::cout<< "Signal caught..." << std::endl;
	stop = true;
}

int main(int argc, char** argv)
{
	struct soap* tcpServer = soap_new(); 
	tcpServer->bind_flags=SO_REUSEADDR;
	if (!soap_valid_socket(soap_bind(tcpServer, NULL, 3702, 1000)))
	{
		soap_print_fault(tcpServer, stderr);
		exit(1);
	}	
	
	struct soap* udpServer = soap_new1(SOAP_IO_UDP); 
	if (!soap_valid_socket(soap_bind(udpServer, NULL, 0, 1000)))
	{
		soap_print_fault(udpServer, stderr);
		exit(1);
	}
	
	tcpServer->user = udpServer;
	udpServer->user = tcpServer;

	signal(SIGINT, &sighandler);
	while (!stop)
	{
		soap_wsdd_listen(tcpServer, -1000000);
	}

	soap_destroy(udpServer);	
	soap_end(udpServer);	
	
	soap_destroy(tcpServer);	
	soap_end(tcpServer);	

	return 0;
}

struct wsdd__ProbeMatchesType* outmatches;

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
	printf("wsdd_event_ProbeMatches\tid=%s RelatesTo=%s nbMatch:%d\n", MessageID, RelatesTo, matches->__sizeProbeMatch);
	struct soap* tcpServ = (struct soap*)soap->user;
	if (outmatches != NULL)
	{
		for (int i=0; i < matches->__sizeProbeMatch; ++i)
		{
			wsdd__ProbeMatchType& match = matches->ProbeMatch[i];
			char* address = (char*)soap_malloc(tcpServ, strlen(match.wsa__EndpointReference.Address));
			strcpy(address, match.wsa__EndpointReference.Address);
			char* types = (char*)soap_malloc(tcpServ, strlen(match.Types));
			strcpy(types, match.Types);
			char* scopes = (char*)soap_malloc(tcpServ, strlen(match.Scopes->__item));
			strcpy(scopes, match.Scopes->__item);
			char* XAddrs = (char*)soap_malloc(tcpServ, strlen(match.XAddrs));
			strcpy(XAddrs, match.XAddrs);
			printf("%s %s %s %s\n", address, types, scopes, XAddrs);
			soap_wsdd_add_ProbeMatch(tcpServ, outmatches, address, types, scopes, NULL, XAddrs, match.MetadataVersion);
		}	
	}
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
	printf("wsdd_event_ResolveMatches\tid=%s RelatesTo=%s\n", MessageID, RelatesTo);
	struct soap* tcpServ = (struct soap*)soap->user;
	soap_wsdd_ResolveMatches(tcpServ, NULL, MessageID, RelatesTo, NULL, match->wsa__EndpointReference.Address, match->Types, match->Scopes->__item, NULL, match->XAddrs, match->MetadataVersion);
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
	printf("wsdd_event_Resolve\tid=%s replyTo=%s EndpointReference=%s\n", MessageID, ReplyTo, EndpointReference);	
	
	struct soap* udpServer = (struct soap*)soap->user;
	soap_wsdd_Resolve(udpServer,
			  SOAP_WSDD_ADHOC,      // mode
			  SOAP_WSDD_TO_TS,      // to a TS
			  "soap.udp://239.255.255.250:3702",         // address of TS
			  MessageID,                   // message ID
			  NULL,                 // ReplyTo
			  EndpointReference);
	soap_wsdd_listen(udpServer, -1000000);
	
	return SOAP_WSDD_ADHOC;
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	printf("wsdd_event_Probe\tid=%s replyTo=%s types=%s scopes=%s\n", MessageID, ReplyTo, Types, Scopes);	
	soap_wsdd_init_ProbeMatches(soap,matches);
	
	struct soap* udpServer = (struct soap*)soap->user;
	outmatches = matches;
	soap_wsdd_Probe(udpServer,
			  SOAP_WSDD_ADHOC,      // mode
			  SOAP_WSDD_TO_TS,      // to a TS
			  "soap.udp://239.255.255.250:3702",         // address of TS
			  MessageID,                   // message ID
			  NULL,                 // ReplyTo
			  Types,
			  Scopes,
			  NULL);	
	soap_wsdd_listen(udpServer, -1000000);
	outmatches = NULL;
	
	soap_wsdd_ProbeMatches(soap, NULL, soap_wsa_rand_uuid(soap) , MessageID, ReplyTo, matches);
	
	return SOAP_WSDD_ADHOC;
}


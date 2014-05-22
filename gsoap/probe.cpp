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
	struct soap* soap = soap_new();
	printf("call soap_wsdd_Probe\n");
	int res = soap_wsdd_Probe(soap,
	  SOAP_WSDD_ADHOC,      // mode
	  SOAP_WSDD_TO_TS,      // to a TS
	  "soap.udp://239.255.255.250:3702",         // address of TS
	  soap_wsa_rand_uuid(soap),                   // message ID
	  NULL,                 // ReplyTo
	  NULL,
	  NULL,
	  "");
	if (res != SOAP_OK)
	{
		soap_print_fault(soap, stderr);
	}

	// listen answers
	res = soap_wsdd_listen(serv, 5);
	if (res != SOAP_OK)
	{
		soap_print_fault(soap, stderr);
	}
	return 0;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
	printf("wsdd_event_ProbeMatches nbMatch:%d\n", matches->__sizeProbeMatch);
	
	if (matches->ProbeMatch)
	{
		for (int i=0; i < matches->__sizeProbeMatch; ++i)
		{
			wsdd__ProbeMatchType& elt = matches->ProbeMatch[i];
			std::cout << "===================================================================" << std::endl;

			if (elt.wsa__EndpointReference.Address)
			{
				std::cout << "Endpoint:\t"<< elt.wsa__EndpointReference.Address << std::endl;
			}
			if (elt.Types)
			{
				std::cout << "Types:\t\t"<< elt.Types<< std::endl;
			}
			if (elt.Scopes)
			{
				if (elt.Scopes->__item )
				{
					std::cout << "Scopes:\t\t"<< elt.Scopes->__item << std::endl;
				}
				if (elt.Scopes->MatchBy)
				{
					std::cout << "MatchBy:\t"<< elt.Scopes->MatchBy << std::endl;
				}
			}
			if (elt.XAddrs)
			{
				std::cout << "XAddrs:\t\t"<< elt.XAddrs << std::endl;
			}

			std::cout << "MetadataVersion:\t\t" << elt.MetadataVersion << std::endl;
			std::cout << "-------------------------------------------------------------------" << std::endl;

		}	
	}
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
	printf("wsdd_event_ResolveMatches\n");
}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
	printf("wsdd_event_Hello id=%s EndpointReference=%s XAddrs=%s\n", MessageID, EndpointReference, XAddrs);
}

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
	printf("wsdd_event_Bye id=%s EndpointReference=%s XAddrs=%s\n", MessageID, EndpointReference, XAddrs);
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

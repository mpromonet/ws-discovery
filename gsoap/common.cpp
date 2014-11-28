#include "wsddapi.h"


void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
	printf("wsdd_event_ProbeMatches nbMatch:%d\n", matches->__sizeProbeMatch);
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




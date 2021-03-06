/* soapServer.cpp
   Generated by gSOAP 2.8.24 from RechargeResult.h

gSOAP XML Web services tools
Copyright (C) 2000-2015, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapServer.cpp ver 2.8.24 2015-12-04 03:56:44 GMT")


extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	unsigned int k = soap->max_keep_alive;
#endif
	do
	{
#ifndef WITH_FASTCGI
		if (soap->max_keep_alive > 0 && !--k)
			soap->keep_alive = 0;
#endif
		if (soap_begin_serve(soap))
		{	if (soap->error >= SOAP_STOP)
				continue;
			return soap->error;
		}
		if (soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}

#ifdef WITH_FASTCGI
		soap_destroy(soap);
		soap_end(soap);
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:RechargeResult"))
		return soap_serve_ns__RechargeResult(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:UpdateResult"))
		return soap_serve_ns__UpdateResult(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__RechargeResult(struct soap *soap)
{	struct ns__RechargeResult soap_tmp_ns__RechargeResult;
	struct ns__RechargeResultResponse soap_tmp_ns__RechargeResultResponse;
	soap_default_ns__RechargeResultResponse(soap, &soap_tmp_ns__RechargeResultResponse);
	soap_default_ns__RechargeResult(soap, &soap_tmp_ns__RechargeResult);
	if (!soap_get_ns__RechargeResult(soap, &soap_tmp_ns__RechargeResult, "ns:RechargeResult", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__RechargeResult(soap, soap_tmp_ns__RechargeResult.param0, soap_tmp_ns__RechargeResult.param1, soap_tmp_ns__RechargeResult.param2, soap_tmp_ns__RechargeResultResponse.param3);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__RechargeResultResponse(soap, &soap_tmp_ns__RechargeResultResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__RechargeResultResponse(soap, &soap_tmp_ns__RechargeResultResponse, "ns:RechargeResultResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__RechargeResultResponse(soap, &soap_tmp_ns__RechargeResultResponse, "ns:RechargeResultResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__UpdateResult(struct soap *soap)
{	struct ns__UpdateResult soap_tmp_ns__UpdateResult;
	struct ns__UpdateResultResponse soap_tmp_ns__UpdateResultResponse;
	soap_default_ns__UpdateResultResponse(soap, &soap_tmp_ns__UpdateResultResponse);
	soap_default_ns__UpdateResult(soap, &soap_tmp_ns__UpdateResult);
	if (!soap_get_ns__UpdateResult(soap, &soap_tmp_ns__UpdateResult, "ns:UpdateResult", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__UpdateResult(soap, soap_tmp_ns__UpdateResult.param0, soap_tmp_ns__UpdateResult.param1, soap_tmp_ns__UpdateResult.param2, soap_tmp_ns__UpdateResultResponse.param3);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__UpdateResultResponse(soap, &soap_tmp_ns__UpdateResultResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__UpdateResultResponse(soap, &soap_tmp_ns__UpdateResultResponse, "ns:UpdateResultResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__UpdateResultResponse(soap, &soap_tmp_ns__UpdateResultResponse, "ns:UpdateResultResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapServer.cpp */

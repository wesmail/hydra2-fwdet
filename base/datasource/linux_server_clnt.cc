/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "linux_server.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

rpcbuf *
onlineevt_1(void *argp, CLIENT *clnt)
{
        Int_t iDebug = 0;
	static rpcbuf clnt_res;

        if (iDebug)
           printf("-D- in onlineevt_1\n");
	memset((Char_t *)&clnt_res, 0, sizeof(clnt_res));
        if (iDebug)
           printf("-D- in onlineevt_1: after memset\n");

	if (clnt_call(clnt, ONLINEEVT, (xdrproc_t) xdr_void, (Char_t *)argp, (xdrproc_t) xdr_rpcbuf, (Char_t *)&clnt_res, TIMEOUT) != RPC_SUCCESS) {
                printf("-E- in onlineevt_1: clnt_call failed\n");
		return (NULL);
	}
        if (iDebug)
           printf("-D- in onlineevt_1: after clnt_call\n");
	return (&clnt_res);
}

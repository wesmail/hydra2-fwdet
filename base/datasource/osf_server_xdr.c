/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <rpc/rpc.h>
#include "osf_server.h"

bool_t
xdr_rpcevt(xdrs, objp)
	XDR *xdrs;
	rpcevt *objp;
{
	if (!xdr_bytes(xdrs, (char **)&objp->rpcevt_val, (u_int *)&objp->rpcevt_len, ~0)) {
		return (FALSE);
	}
	return (TRUE);
}

/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <rpc/types.h>
#include <rpc/xdr.h>

#include "linux_server.h"

bool_t
xdr_rpcbuf(XDR *xdrs, rpcbuf *objp)
{

	 //register Long_t *buf=buf;

	 if (!xdr_bytes(xdrs, (Char_t **)&objp->rpcbuf_val, (u_int *)&objp->rpcbuf_len, ~0)) {
		 return (FALSE);
	 }
	return (TRUE);
}

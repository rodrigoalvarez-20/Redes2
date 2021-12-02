/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "validator.h"

bool_t
xdr_point (XDR *xdrs, point *objp)
{
	register int32_t *buf;

	 if (!xdr_float (xdrs, &objp->x))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->y))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->z))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->r))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->g))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->b))
		 return FALSE;
	return TRUE;
}

/*
 * $Source$
 * $Author$
 *
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America is assumed
 *   to require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 * 
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 *
 * This file contains the source code for krb5_fcc_retrieve.
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_fcc_retrv_c[] =
"$Id$";
#endif /* !lint && !SABER */


#include "fcc.h"

#define set(bits) (whichfields & bits)
#define flags_match(a,b) (a & b == a)
#define times_match_exact(t1,t2) (memcmp((char *)(t1), (char *)(t2), sizeof(*(t1))) == 0)

static krb5_boolean times_match PROTOTYPE((const krb5_ticket_times *,
					   const krb5_ticket_times *));
static krb5_boolean standard_fields_match
    PROTOTYPE((const krb5_creds *,
	       const krb5_creds *));

static krb5_boolean srvname_match
    PROTOTYPE((const krb5_creds *,
	       const krb5_creds *));

static krb5_boolean authdata_match
    PROTOTYPE ((krb5_authdata * const *, krb5_authdata * const *));


static krb5_boolean
data_match(data1, data2)
register const krb5_data *data1, *data2;
{
    if (!data1) {
	if (!data2)
	    return TRUE;
	else
	    return FALSE;
    }
    if (!data2) return FALSE;

    if (data1->length != data2->length)
	return FALSE;
    else
	return memcmp(data1->data, data2->data, data1->length) ? FALSE : TRUE;
}

/*
 * Effects:
 * Searches the file cred cache is for a credential matching mcreds,
 * with the fields specified by whichfields.  If one if found, it is
 * returned in creds, which should be freed by the caller with
 * krb5_free_credentials().
 * 
 * The fields are interpreted in the following way (all constants are
 * preceded by KRB5_TC_).  MATCH_IS_SKEY requires the is_skey field to
 * match exactly.  MATCH_TIMES requires the requested lifetime to be
 * at least as great as that specified; MATCH_TIMES_EXACT requires the
 * requested lifetime to be exactly that specified.  MATCH_FLAGS
 * requires only the set bits in mcreds be set in creds;
 * MATCH_FLAGS_EXACT requires all bits to match.
 *
 * Errors:
 * system errors
 * permission errors
 * KRB5_CC_NOMEM
 */
krb5_error_code
krb5_fcc_retrieve(id, whichfields, mcreds, creds)
   krb5_ccache id;
   krb5_flags whichfields;
   krb5_creds *mcreds;
   krb5_creds *creds;
{
     /* This function could be considerably faster if it kept indexing */
     /* information.. sounds like a "next version" idea to me. :-) */

     krb5_cc_cursor cursor;
     krb5_error_code kret;
     krb5_creds fetchcreds;

     kret = krb5_fcc_start_seq_get(id, &cursor);
     if (kret != KRB5_OK)
	  return kret;

     while ((kret = krb5_fcc_next_cred(id, &cursor, &fetchcreds)) == KRB5_OK) {
	  if (((set(KRB5_TC_MATCH_SRV_NAMEONLY) &&
		   srvname_match(mcreds, &fetchcreds)) ||
	       standard_fields_match(mcreds, &fetchcreds))
	      &&
	      (! set(KRB5_TC_MATCH_IS_SKEY) ||
	       mcreds->is_skey == fetchcreds.is_skey)
	      &&
	      (! set(KRB5_TC_MATCH_FLAGS_EXACT) ||
	       mcreds->ticket_flags == fetchcreds.ticket_flags)
	      &&
	      (! set(KRB5_TC_MATCH_FLAGS) ||
	       flags_match(mcreds->ticket_flags, fetchcreds.ticket_flags))
	      &&
	      (! set(KRB5_TC_MATCH_TIMES_EXACT) ||
	       times_match_exact(&mcreds->times, &fetchcreds.times))
	      &&
	      (! set(KRB5_TC_MATCH_TIMES) ||
	       times_match(&mcreds->times, &fetchcreds.times))
	      &&
	      ( ! set(KRB5_TC_MATCH_AUTHDATA) ||
	       authdata_match(mcreds->authdata, fetchcreds.authdata))
	      &&
	      (! set(KRB5_TC_MATCH_2ND_TKT) ||
	       data_match (&mcreds->second_ticket, &fetchcreds.second_ticket))
	      )
	  {
	       krb5_fcc_end_seq_get(id, &cursor);
	       *creds = fetchcreds;
	       return KRB5_OK;
	  }

	  /* This one doesn't match */
	  krb5_free_cred_contents(&fetchcreds);
     }

     /* If we get here, a match wasn't found */
     krb5_fcc_end_seq_get(id, &cursor);
     return KRB5_CC_NOTFOUND;
}

static krb5_boolean
times_match(t1, t2)
register const krb5_ticket_times *t1;
register const krb5_ticket_times *t2;
{
    if (t1->renew_till) {
	if (t1->renew_till > t2->renew_till)
	    return FALSE;		/* this one expires too late */
    }
    if (t1->endtime) {
	if (t1->endtime > t2->endtime)
	    return FALSE;		/* this one expires too late */
    }
    /* only care about expiration on a times_match */
    return TRUE;
}

static krb5_boolean
standard_fields_match(mcreds, creds)
register const krb5_creds *mcreds, *creds;
{
    return (krb5_principal_compare(mcreds->client,creds->client) &&
	    krb5_principal_compare(mcreds->server,creds->server));
}

/* only match the server name portion, not the server realm portion */

static krb5_boolean
srvname_match(mcreds, creds)
register const krb5_creds *mcreds, *creds;
{
    krb5_boolean retval;
    retval = krb5_principal_compare(mcreds->client,creds->client);
    if (retval != TRUE)
	return retval;
    return krb5_principal_compare(&(mcreds->server[1]),&(creds->server[1]));
}

static krb5_boolean
authdata_match(mdata, data)
    register krb5_authdata * const *mdata, * const *data;
{
    register const krb5_authdata *mdatap, *datap;

    if (mdata == data)
      return TRUE;

    if (mdata == NULL)
	return *data == NULL;
	
    if (data == NULL)
	return *mdata == NULL;
    
    while ((mdatap = *mdata) && (datap = *data)) {
      if ((mdatap->ad_type != datap->ad_type) ||
          (mdatap->length != datap->length) ||
          (memcmp ((char *)mdatap->contents,
		 (char *)datap->contents, mdatap->length) != 0))
          return FALSE;
      mdata++;
      data++;
    }
    return (*mdata == NULL) && (*data == NULL);
}

/*
 * $Source$
 * $Author$
 * $Id$
 *
 * Copyright 1988,1990 by the Massachusetts Institute of Technology.
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
 * part of the Athena Kerberos encryption system
 *
 * spm	8/85
 */

/*
 * Initial permutation, adjust to zero based subscript
 */
static	char	IP[] = {
	58-1, 50-1, 42-1, 34-1, 26-1, 18-1, 10-1,  2-1, 
	60-1, 52-1, 44-1, 36-1, 28-1, 20-1, 12-1,  4-1, 
	62-1, 54-1, 46-1, 38-1, 30-1, 22-1, 14-1,  6-1, 
	64-1, 56-1, 48-1, 40-1, 32-1, 24-1, 16-1,  8-1, 
	57-1, 49-1, 41-1, 33-1, 25-1, 17-1,  9-1,  1-1, 
	59-1, 51-1, 43-1, 35-1, 27-1, 19-1, 11-1,  3-1, 
	61-1, 53-1, 45-1, 37-1, 29-1, 21-1, 13-1,  5-1, 
	63-1, 55-1, 47-1, 39-1, 31-1, 23-1, 15-1,  7-1, 
};

/*
 * Final permutation,  FP = IP^(-1) adjust to zero based subscript
 */
static	char	FP[] = {
	40-1,  8-1, 48-1, 16-1, 56-1, 24-1, 64-1, 32-1, 
	39-1,  7-1, 47-1, 15-1, 55-1, 23-1, 63-1, 31-1, 
	38-1,  6-1, 46-1, 14-1, 54-1, 22-1, 62-1, 30-1, 
	37-1,  5-1, 45-1, 13-1, 53-1, 21-1, 61-1, 29-1, 
	36-1,  4-1, 44-1, 12-1, 52-1, 20-1, 60-1, 28-1, 
	35-1,  3-1, 43-1, 11-1, 51-1, 19-1, 59-1, 27-1, 
	34-1,  2-1, 42-1, 10-1, 50-1, 18-1, 58-1, 26-1, 
	33-1,  1-1, 41-1,  9-1, 49-1, 17-1, 57-1, 25-1, 
};

/* the E selection function, adjusted to zero based subscripts */
static	char	E[] = {
	32-1,	1-1,	2-1,	3-1,	4-1,	5-1,
	 4-1,	5-1,	6-1,	7-1,	8-1,	9-1,
	 8-1, 	9-1,	10-1,	11-1,	12-1,	13-1,
	12-1,	13-1,	14-1,	15-1,	16-1,	17-1,
	16-1,	17-1,	18-1,	19-1,	20-1,	21-1,
	20-1,	21-1,	22-1,	23-1,	24-1,	25-1,
	24-1,	25-1,	26-1,	27-1,	28-1,	29-1,
	28-1,	29-1,	30-1,	31-1,	32-1, 	1-1,
};

/* the P permutation, adjusted to zero based subscripts */
static	char	P[] = {
	16-1,	 7-1,	20-1,	21-1,	
	29-1,	12-1,	28-1,	17-1,	
	 1-1,	15-1,	23-1,	26-1,	
	 5-1,	18-1,	31-1,	10-1,	
	 2-1,	 8-1,	24-1,	14-1,	
	32-1,	27-1,	 3-1,	 9-1,	
	19-1,	13-1,	30-1,	 6-1,	
	22-1,	11-1,	 4-1,	25-1,	
};

/* S tables, original form */
static	char	S[8][64] = {
	14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
	 0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
	 4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
	15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13,

	15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
	 3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
	 0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
	13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9,

	10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
	13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
	13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
	 1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12,

	 7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
	13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
	10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
	 3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14,

	 2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
	14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
	 4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
	11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3,

	12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
	10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
	 9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
	 4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13,

	 4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
	13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
	 1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
	 6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12,

	13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
	 1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
	 7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
	 2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11,
};

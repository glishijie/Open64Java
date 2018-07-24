/*
 * Copyright 2004 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2.1 of the GNU Lesser General Public License 
  as published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU Lesser General Public 
  License along with this program; if not, write the Free Software 
  Foundation, Inc., 59 Temple Place - Suite 330, Boston MA 02111-1307, 
  USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* $Header: /proj/osprey/CVS/open64/osprey1.0/libF77/pow_di.c,v 1.1.1.1 2005/10/21 19:00:00 marcel Exp $ */
#include "cmplrs/host.h"
#include <math.h>
#include "moremath.h"

extern double __powdi(double x, int32 n);
extern double __powdl(double x, int64 n);

/* By-reference versions for backward compatibility. */

double pow_di(double *ap, int32 *bp)
{
double pow;
pow=__powdi(*ap,*bp);
return pow;
}

double pow_dl(double *ap, int64 *bp)
{
double pow;
pow=__powdl(*ap,*bp);
return pow;
}

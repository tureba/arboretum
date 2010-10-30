/**********************************************************************
* GBDI Arboretum - Copyright (c) 2002-2004 GBDI-ICMC-USP
*
*                           Homepage: http://gbdi.icmc.usp.br/arboretum
**********************************************************************/
/* ====================================================================
 * The GBDI-ICMC-USP Software License Version 1.0
 *
 * Copyright (c) 2004 Grupo de Bases de Dados e Imagens, Instituto de
 * Ciências Matemáticas e de Computação, University of São Paulo -
 * Brazil (the Databases and Image Group - Intitute of Matematical and 
 * Computer Sciences).  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by Grupo de Bases
 *        de Dados e Imagens, Instituto de Ciências Matemáticas e de
 *        Computação, University of São Paulo - Brazil (the Databases 
 *        and Image Group - Intitute of Matematical and Computer 
 *        Sciences)"
 *
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names of the research group, institute, university, authors
 *    and collaborators must not be used to endorse or promote products
 *    derived from this software without prior written permission.
 *
 * 5. The names of products derived from this software may not contain
 *    the name of research group, institute or university, without prior
 *    written permission of the authors of this software.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OF THIS SOFTWARE OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ====================================================================
 *                                            http://gbdi.icmc.usp.br/
 */
/**
* @file
*
* This file includes all common headers used by this library. It will help the portability
* of this code.
*
* @version 1.0
* $Revision: 1.12 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/                                                               
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#ifndef __STCOMMON_H
#define __STCOMMON_H

// All compilers
#include <stdlib.h>

// GNU GCC
#ifdef __GNUG__
   #include <string.h> // malloc & cia
   
   // This is an useful function extracted from Windows C Runtime library.
   #define random(x) ((int)((((double)rand()) / ((double)RAND_MAX)) * (x)))
   
	// Mingw does not provide these values so, here they are.
	#ifdef __MINGW32_VERSION
	   #define MAXSHORT    0x7fff
	   #define MAXINT      0x7fffffff
	   #define MAXLONG     0x7fffffff
	   #define MAXDOUBLE   1.7976931348623158E+308
	   #define MAXFLOAT    3.40282347E+38F
	   #define MINDOUBLE   2.2250738585072014E-308
	   #define MINFLOAT    1.17549435E-38F
	   #define MAXLDOUBLE  1.1897314953572317649E+4932L
	   #define MINLDOUBLE  3.362103143112094E-4917L
	#else	
		#ifdef __CYGWIN__
	   	#define MAXSHORT    0x7fff
	   	#define MAXINT      0x7fffffff
	   	#define MAXLONG     0x7fffffff
	   	#define MAXDOUBLE   1.7976931348623158E+308
	   	#ifndef MAXFLOAT
	   		#define MAXFLOAT    3.40282347E+38F
	   	#endif
	   	#define MINDOUBLE   2.2250738585072014E-308
	   	#define MINFLOAT    1.17549435E-38F
		   #define MAXLDOUBLE  1.1897314953572317649E+4932L
		   #define MINLDOUBLE  3.362103143112094E-4917L
		#else
			// Other gccs
		   #include <values.h>
		#endif
	#endif //__MINGW32_VERSION
#endif //__GNUG__

// Microsoft Visual C++
#ifdef _MSC_VER
   #include <limits.h>
   #define MAXLONG LONG_MAX
#endif //_MSC_VER

// Borland C++
#ifdef __BORLANDC__
   #include <mem.h> // malloc & cia
   #include <values.h>
   using namespace std; // standard c++ lib classes
#endif //__BORLANDC__

#endif //__STCOMMON_H

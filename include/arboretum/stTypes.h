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
* This file defines all basic types used by this implementation of the Slim Tree.
*
* @version 1.0
* $Revision: 1.19 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002 GBDI-ICMC-USP
#ifndef __STTYPES_H
#define __STTYPES_H


/**
* This macro enables the debug mode version. Some methods will do error checking when
* this macro is defined.
*/
//#define __stDEBUG__

/**
* This macro enables the visualization mode version. 
*/
#define __stVIEW__

// Basic integer types
#if defined(__GNUC__)
   // GNU C
   /**
   * This type is used to store an unsigned 16-bit integer.
   * @ingroup types
   */
   typedef unsigned short stWord;

   /**
   * This type is used to store an unsigned 32-bit integer.
   * @ingroup types
   */
   typedef unsigned long stDWord;

   /**
   * This type is used to store an unsigned 64-bit integer.
   * @ingroup types
   */
   typedef unsigned long long stQWord;
#elif defined(__BORLANDC__)
   // Borland C

   /**
   * This type is used to store an unsigned 16-bit integer.
   * @ingroup types
   */
   typedef  unsigned __int16 stWord;

   /**
   * This type is used to store an unsigned 32-bit integer.
   * @ingroup types
   */
   typedef unsigned __int32 stDWord;

   /**
   * This type is used to store an unsigned 64-bit integer.
   * @ingroup types
   */
   typedef unsigned __int64 stQWord;
#elif defined(_MSC_VER)
   // MS Visual C++
   /**
   * This type is used to store an unsigned 16-bit integer.
   * @ingroup types
   */
   typedef  unsigned __int16 stWord;

   /**
   * This type is used to store an unsigned 32-bit integer.
   * @ingroup types
   */
   typedef unsigned __int32 stDWord;

   /**
   * This type is used to store an unsigned 64-bit integer.
   * @ingroup types
   */
   typedef unsigned __int64 stQWord;
#else
   // Default
   /**
   * This type is used to store an unsigned 16-bit integer.
   * @ingroup types
   */
   typedef unsigned short stWord;

   /**
   * This type is used to store an unsigned 32-bit integer.
   * @ingroup types
   */
   typedef unsigned long stDWord;

   /**
   * This type is used to store an unsigned 64-bit integer.
   * @todo This data type is not correctly implemented for all compilers.
   * @ingroup types
   */
   typedef unsigned long stQWord;
   
   #pragma message "stQWord is not properly defined."
#endif // defined


/**
* This type is used to store an unsigned 8-bit integer.
* @ingroup types
*/
typedef unsigned char stByte;

/**
* This type is used to store a page ID. By default, it is
* an unsigned 32-bit integer.
* @ingroup types
*/
typedef stDWord stPageID;

/**
* This type is used to store the size of a variable/vector in bytes.
* By default, it is an unsigned 32-bit integer.
* @ingroup types
*/
typedef stDWord stSize;

/**
* This type is used to store distances in a metric space.
* By default it is a double.
* @ingroup types
*/
typedef double stDistance;

/**
* This type is used to store counters.
* By default it is an unsigned 32-bit integer.
* @ingroup types
*/
typedef stDWord stCount;

/**
* This type is used to store the node type.
* By default it is an unsigned 16-bit integer.
* @ingroup types
*/
typedef stWord stNodeType;

#endif //__STTYPES_H

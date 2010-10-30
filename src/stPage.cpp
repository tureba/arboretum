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
* This file implements stPage and stLockablePage.
*
* @version 1.0
* $Revision: 1.13 $
* $Date: 2005/03/08 19:44:19 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP

#include <arboretum/stPage.h>

//------------------------------------------------------------------------------
// class stPage
//------------------------------------------------------------------------------
stPage::stPage (stSize size, stPageID pageid){

   this->BufferSize = size;
   this->Buffer = new stByte[size];
   this->SetPageID(pageid);
}//end stPage::stPage

//------------------------------------------------------------------------------
stPage::~stPage(){

   if (Buffer != NULL){
      delete[] Buffer;
   }//end if
}//end stPage::~stPage

//------------------------------------------------------------------------------
void stPage::Write(stByte * buff, stSize n, stSize offset){

   #ifdef __stDEBUG__
   if ((offset + n) > this->BufferSize){
      throw invalid_argument("Offset out of bounds.");
   }//end if
   #endif //__stDEBUG__

   memcpy((void *)(this->Buffer + offset), (void*)buff, n);
}//end stPage::Write

//------------------------------------------------------------------------------
stSize stPage::GetPageSize(){

   return this->BufferSize;
}//end stPage::GetPageSize

//------------------------------------------------------------------------------
stByte * stPage::GetData(){

   return this->Buffer;
}//end stPage::GetData

//------------------------------------------------------------------------------
void stPage::Copy(stPage * page){

   #ifdef __stDEBUG__
   if (page->GetPageSize() != GetPageSize()){
      throw invalid_argument("Both pages must have the same size.");
   }//end if
   #endif //__stDEBUG__

   memcpy(this->GetData(), page->GetData(), this->GetPageSize());
}//end stPage::Copy

//------------------------------------------------------------------------------
void stPage::Clear(){

   memset(this->GetData(), 0, this->GetPageSize());
}//end stPage::Clear

//------------------------------------------------------------------------------
// class stLockablePage
//------------------------------------------------------------------------------
void stLockablePage::Write(stByte * buff, stSize n, stSize offset){

   #ifdef __stDEBUG__
   if ((LockSize + offset + n) > this->BufferSize){
      throw invalid_argument("Offset out of bounds.");
   }//end if
   #endif //__stDEBUG__

   memcpy((void *)(this->Buffer + offset + LockSize),
         (void*)buff, n);
}//end stLockablePage::Write

//------------------------------------------------------------------------------
stSize stLockablePage::GetPageSize(){

   return this->BufferSize - this->LockSize;
}//end stLockablePage::GetPageSize

//------------------------------------------------------------------------------
stByte * stLockablePage::GetData(){

   return this->Buffer + this->LockSize;
}//end stLockablePage::GetData


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
* This file implements the stMemoryPageManager.
*
* @version 1.0
* $Revision: 1.11 $
* $Date: 2005/03/08 19:44:19 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Enzo Seraphim (seraphim@icmc.usp.br)
* @todo Review of documentation.
*/
// Copyright (c) 2002 GBDI-ICMC-USP

#include <arboretum/stMemoryPageManager.h>

//------------------------------------------------------------------------------
// Class stMemoryPageManager
//------------------------------------------------------------------------------
stMemoryPageManager::stMemoryPageManager(stSize pagesize){

   // Size of the page
   this->PageSize = pagesize;

   // Clear Statistics
   this->ResetStatistics();
}//end stMemoryPageManager::stMemoryPageManager

//------------------------------------------------------------------------------
stMemoryPageManager::~stMemoryPageManager(){
   stPageID i;

   for (i = 0; i < Pages.size(); i++){
      if (Pages.at(i) != NULL){
         delete Pages.at(i);
      }//end if
   }//end for
}//end stMemoryPageManager::stMemoryPageManager

//------------------------------------------------------------------------------
bool stMemoryPageManager::IsEmpty(){

   return Pages.size() - FreePages.size() < 2;
}//end stMemoryPageManager::IsEmpty

//------------------------------------------------------------------------------
stPage * stMemoryPageManager::GetHeaderPage(){

   // Create the page if required.
   if (Pages.size() == 0){
      Pages.insert(Pages.end(), new stPage(this->PageSize, 0));
   }//end if

   // Update Counters
   UpdateReadCounter(); 

   // It will always be the first page.
   return Pages.at(0);
}//end stMemoryPageManager::GetHeaderPage

//------------------------------------------------------------------------------
stPage * stMemoryPageManager::GetPage(stPageID pageid){

   #ifdef __stDEBUG__
   if ((pageid < 1) || (pageid >= Pages.size())){
      throw invalid_argument("Invalid page id.");
   }//end if
   #endif //__stDEBUG__

   // Update statistics
   UpdateReadCounter(); 

   return Pages.at(pageid);
}//end stMemoryPageManager::GetPage

//------------------------------------------------------------------------------
void stMemoryPageManager::ReleasePage(stPage * page){

   // Nothing to do!
}//end stMemoryPageManager::ReleasePage

//------------------------------------------------------------------------------
stPage * stMemoryPageManager::GetNewPage(){
   stPage * newpage;
   stPageID freepage;

   // Update Counters
   UpdateWriteCounter(); 

   // Check stack
   if (FreePages.empty()){
      // New!
      newpage = new stPage(this->PageSize, Pages.size());
      // Add
      Pages.insert(Pages.end(), newpage);
      // Return
      return newpage;
   }else{
      // From stack
      freepage = FreePages.top();
      FreePages.pop();
      return Pages.at(freepage);
   }//end if
}//end stMemoryPageManager::GetNewPage

//------------------------------------------------------------------------------
void stMemoryPageManager::WritePage(stPage * page){

   // Update statistics
   UpdateWriteCounter(); 
}//end stMemoryPageManager::WritePage

//------------------------------------------------------------------------------
void stMemoryPageManager::DisposePage(stPage * page){

   #ifdef __stDEBUG__
   if (page==NULL){
      throw invalid_argument("Are you nuts ? \"page\" can not be NULL.");
   }//end if
   #endif //__stDEBUG__

   FreePages.push(page->GetPageID());
}//end stMemoryPageManager::DisposePage


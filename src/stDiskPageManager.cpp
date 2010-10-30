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
* $Revision: 1.32 $
* $Date: 2005/03/08 19:44:19 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Enzo Seraphim (seraphim@icmc.usp.br)
* @todo Review of documentation.
*/
// Copyright (c) 2002 GBDI-ICMC-USP

#include <arboretum/stDiskPageManager.h>

/**
* Number of instances in the page cache.
*/
#define STDISKPAGEMANAGER_INSTANCECACHESIZE 16

//------------------------------------------------------------------------------
// class stDiskPageManager
//------------------------------------------------------------------------------
stDiskPageManager::stDiskPageManager(const string & fName, stSize pagesize,
      stSize userHeaderSize,
      int cacheNPages):fileName(fName){

   myStorage = new CStorage;
   this->pageSize = pagesize;
   myStorage->Create(fileName.c_str(), pageSize, userHeaderSize, cacheNPages);

   // Instance cache with
   pageInstanceCache = new stPageInstanceCache(STDISKPAGEMANAGER_INSTANCECACHESIZE,
         new stPageAllocator(pageSize));
}//end stDiskPageManager::stDiskPageManager

//------------------------------------------------------------------------------
stDiskPageManager::stDiskPageManager(const string & fName):fileName(fName){

   this->myStorage = new CStorage();
   this->myStorage->Open(fileName.c_str());
   this->pageSize =  myStorage->GetPageSize();
   
      // Instance cache with 
   pageInstanceCache = new stPageInstanceCache(STDISKPAGEMANAGER_INSTANCECACHESIZE,
         new stPageAllocator(pageSize));   
}//end stDiskPageManager::stDiskPageManager

//------------------------------------------------------------------------------
stDiskPageManager::~stDiskPageManager(){

   if (myStorage != NULL){
      if (myStorage->IsOpened()){
         myStorage->FlushCache();
         myStorage->Close();
      }//end if
   }//end if
   
   delete pageInstanceCache;
}//end stDiskPageManager::~stDiskPageManager

//------------------------------------------------------------------------------
bool stDiskPageManager::IsEmpty(){

   // Error checking
   return myStorage->GetTotalPagesInUse() == 0;
}//end stDiskPageManager::IsEmpty

//------------------------------------------------------------------------------
stPage * stDiskPageManager::GetHeaderPage(){
   stPage * hPage = new stPage(this->GetHeaderPageSize());

   myStorage->ReadUserHeader(hPage->GetData());

   // Update Counters
   UpdateReadCounter();

   return hPage;
}//end stDiskPageManager::GetHeaderPage()

//------------------------------------------------------------------------------
stPage * stDiskPageManager::GetPage(stPageID pageid){
   stPage * myPage;

   if ((int)pageid <= myStorage->GetTotalPagesIncludingDisposed()){
      // Get from cache
      myPage = pageInstanceCache->Get();
      
      myStorage->ReadPage(pageid-1, myPage->GetData());
      myPage->SetPageID(pageid);
   
      // Update Counters
      UpdateReadCounter();
      return myPage;
   }else{
      // Error!!!
      return NULL;
   }//end if
}//end stDiskPageManager::GetPage()

//------------------------------------------------------------------------------
void stDiskPageManager::ReleasePage(stPage * page){

   // Put it back
   if (page->GetPageSize() == this->pageSize){
      pageInstanceCache->Put(page);
   }else{
      delete page;
   }//end if   
}//end stDiskPageManager::ReleasePage()

//------------------------------------------------------------------------------
stPage * stDiskPageManager::GetNewPage(){
   stPage * currentPage;
   
   // Get from cache
   currentPage = pageInstanceCache->Get();
   
   stPageID newID = myStorage->InsertNewPage((void *)currentPage->GetData())+1;
   currentPage->SetPageID(newID);

   // Update Counters
   UpdateWriteCounter();

   return currentPage;
}//end stDiskPageManager::GetNewPage()

//------------------------------------------------------------------------------
void stDiskPageManager::WritePage(stPage * page){

   // Write it but do not delete. It is not equal to ReleasePage.
   myStorage->WritePage(page->GetPageID()-1, (void *) page->GetData());

   // Update Counters
   UpdateWriteCounter();
}//end stDiskPageManager::WritePage

//------------------------------------------------------------------------------
void stDiskPageManager::WriteHeaderPage(stPage * headerpage){

   myStorage->WriteUserHeader((void *) headerpage->GetData());

   // Update Counters
   UpdateWriteCounter();
}//end stDiskPageManager::WriteHeaderPage

//------------------------------------------------------------------------------
void stDiskPageManager::DisposePage(stPage * page){

   myStorage->FreePage(page->GetPageID()-1);

   // Put it back
   pageInstanceCache->Put(page);
}//end stDiskPageManager::DisposePage()

//------------------------------------------------------------------------------
void stDiskPageManager::Create(const char *fName, int pagesize, int userHeaderSize, int cacheNPages){

   this->pageSize = pagesize;
   this->myStorage = new CStorage;
   myStorage->Create(fName, pageSize, userHeaderSize, cacheNPages);
   ResetStatistics();

   // Instance cache with 
   pageInstanceCache = new stPageInstanceCache(STDISKPAGEMANAGER_INSTANCECACHESIZE,
         new stPageAllocator(pageSize));
}//end stDiskPageManager::Create()

//------------------------------------------------------------------------------
void stDiskPageManager::Open(char *fname){
   if (this->myStorage == NULL)
      this->myStorage = new CStorage;

   this->myStorage->Open(fname);
   this->pageSize =  myStorage->GetPageSize();
   ResetStatistics();

   // Instance cache with
   pageInstanceCache = new stPageInstanceCache(STDISKPAGEMANAGER_INSTANCECACHESIZE,
         new stPageAllocator(pageSize));
}//end stDiskPageManager::Open()

//------------------------------------------------------------------------------
void stDiskPageManager::ResetFile(){

   myStorage->Drop();
}//end stDiskPageManager::Reset()

//------------------------------------------------------------------------------
long stDiskPageManager::GetHeaderPageSize(){

   return myStorage->GetUserHeaderSize();
}//end stDiskPageManager::GetHeaderPageSize()

//------------------------------------------------------------------------------
void stDiskPageManager::Flush(){

   myStorage->FlushCache();
}//end stDiskPageManager::Flush

//------------------------------------------------------------------------------
void stDiskPageManager::ResetStatistics(){
   stPageManager::ResetStatistics();

   myStorage->ResetStatistics();
}//end stDiskPageManager::ResetStatistics()

//------------------------------------------------------------------------------
inline unsigned long stDiskPageManager::GetDiskReadCount(){

   return myStorage->counters.diskRead;
}//end stDiskPageManager::GetDiskReadCount()

//------------------------------------------------------------------------------
inline unsigned long stDiskPageManager::GetDiskWriteCount(){

   return myStorage->counters.diskWrite;
}//end stDiskPageManager::GetDiskWriteCount()



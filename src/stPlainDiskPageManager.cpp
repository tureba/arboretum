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
* This file implements the stPlainDiskPageManager.
*
* @version 1.0
* $Revision: 1.11 $
* $Date: 2005/03/13 19:31:14 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Implementation not tested.
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#include <arboretum/stPlainDiskPageManager.h>

/**
* Number of instances in the page cache.
*/
#define STDISKPAGEMANAGER_INSTANCECACHESIZE 16

//==============================================================================
// stPlainDiskPageManager
//------------------------------------------------------------------------------
stPlainDiskPageManager::stPlainDiskPageManager(const char * fName, stSize pagesize){

   // Open file
   fd = open(fName, O_CREAT|O_TRUNC|O_RDWR|O_BINARY, S_IREAD|S_IWRITE); // New file with 0 bytes
   if (fd < 0){
      throw io_error("Unable to create file.");
   }//end if

   // Initialize fields.
   this->headerPage = new stLockablePage(pagesize, sizeof(tHeader), 0);
   this->header = (tHeader *)(this->headerPage->GetTrueData());
   NewHeader(this->header, pagesize);
   WriteHeaderPage(headerPage);

   // Page cache
   pageInstanceCache = new stPageInstanceCache(STDISKPAGEMANAGER_INSTANCECACHESIZE,
         new stPageAllocator(pagesize));      
}//end stPlainDiskPageManager::stPlainDiskPageManager
//------------------------------------------------------------------------------

stPlainDiskPageManager::stPlainDiskPageManager(const char * fName){
   tHeader tmpHeader;
   
   // Open file
   fd = open(fName, O_RDWR|O_BINARY); // Open file
   if (fd < 0){
      throw io_error("Unable to open file.");
   }//end if
   
   // Validate file
   if ((read(fd, &tmpHeader, sizeof(tmpHeader)) != sizeof(tmpHeader)) ||
         (!IsValidHeader(&tmpHeader))){
      throw io_error("invalid file.");
   }//end if

   // WARNING!!! If you don't undertand what is going on here, see the
   // stLockablePage documentation for further info.
      
   // Ok, now I must reload the header again. I can not use
   this->headerPage = new stLockablePage(tmpHeader.PageSize, sizeof(tHeader), 0);
   this->header = (tHeader *)(this->headerPage->GetTrueData());

   // Transfer tmpHeader to header page because I don't like seeks...
   memcpy((void*)this->headerPage->GetTrueData(), &tmpHeader, sizeof(tHeader));
   // Load rest of the header.
   if (read(fd, (void *)this->headerPage->GetData(), (int)this->headerPage->GetPageSize())
         != (int)this->headerPage->GetPageSize()){
      delete headerPage;
      throw io_error("invalid file.");
   }//end if

   // Page cache   
   pageInstanceCache = new stPageInstanceCache(STDISKPAGEMANAGER_INSTANCECACHESIZE,
         new stPageAllocator(header->PageSize));      
}//end stPlainDiskPageManager::stPlainDiskPageManager

//------------------------------------------------------------------------------
stPlainDiskPageManager::~stPlainDiskPageManager(){
   
   // Free resources
   delete pageInstanceCache;
   // Save header page info.
   WriteHeaderPage(headerPage);
   // Delete header page.
   delete this->headerPage;
   // Close file
   close(fd);
}//end stPlainDiskPageManager::~stPlainDiskPageManager

//------------------------------------------------------------------------------
bool stPlainDiskPageManager::IsEmpty(){
   
   return header->UsedPages == 0;
}//end stPlainDiskPageManager::IsEmpty
//------------------------------------------------------------------------------

stPage * stPlainDiskPageManager::GetHeaderPage(){
   
   // Update read count
   UpdateReadCounter();
   
   // Effective read
   lseek(fd, 0, SEEK_SET);  
   read(fd, (void *)this->headerPage->GetTrueData(), header->PageSize);
    
   return this->headerPage;
}//end stPlainDiskPageManager::GetheaderPage

//------------------------------------------------------------------------------
stPage * stPlainDiskPageManager::GetPage(stPageID pageid){
   stPage * myPage;

   // Do not allow users to load header page from this file.
   if ((pageid != 0) && (pageid <= header->PageCount)){
      
      // Get from cache
      myPage = pageInstanceCache->Get();
      
      // Read data...
      lseek(fd, PageID2Offset(pageid), SEEK_SET);
      read(fd, myPage->GetData(), header->PageSize);
      myPage->SetPageID(pageid);
   
      // Update Counters
      UpdateReadCounter();
      return myPage;
   }else{
      // Error!!!
      #ifdef __stDEBUG__
      throw invalid_argument("Invalid page ID.");
      #else
      return NULL;
      #endif //__stDEBUG__
   }//end if
}//end stPlainDiskPageManager::GetPage

//------------------------------------------------------------------------------
void stPlainDiskPageManager::ReleasePage(stPage * page){
   
   // Put it back
   if (page->GetPageSize() == header->PageSize){
      pageInstanceCache->Put(page);
   }else if (page->GetPageID() != 0){
      delete page;
   //}else{
      // Do nothing because it is the header page.
   }//end if      
}//end stPlainDiskPageManager::ReleasePage

//------------------------------------------------------------------------------
stPage * stPlainDiskPageManager::GetNewPage(){
   stPageID * next;
   stPage * page;
   
   if (header->Available == 0){
      // Get instance from cache
      page = pageInstanceCache->Get();
      
      // Creating the new page
      header->PageCount++;
      page->SetPageID(header->PageCount);
   }else{
      // Remove from free list
      page = GetPage(header->Available);
      next = (stPageID *)(page->GetData());
      header->Available = * next;
   }//end if
   
   // Update header
   header->UsedPages++;
   WriteHeaderPage(headerPage);
   
   return page;   
}//end stPlainDiskPageManager::GetNewPage

//------------------------------------------------------------------------------
void stPlainDiskPageManager::WritePage(stPage * page){

   #ifdef __stDEBUG__
   if (page->GetPageID() == 0){
      throw invalid_argument("Do not use WritePage to write header pages.");
   }//end if
   #endif //__stDEBUG__

   lseek(fd, PageID2Offset(page->GetPageID()), SEEK_SET);
   write(fd, page->GetData(), header->PageSize);      
   UpdateWriteCounter();
}//end stPlainDiskPageManager::WritePage

//------------------------------------------------------------------------------
void stPlainDiskPageManager::WriteHeaderPage(stPage * headerpage){
   
   #ifdef __stDEBUG__
   if (headerpage->GetPageID() != 0){
      throw invalid_argument("Do not use WriteHeaderPage to write standard pages.");
   }//end if
   #endif //__stDEBUG__
   
   lseek(fd, 0, SEEK_SET);
   write(fd, this->headerPage->GetTrueData(), header->PageSize);
   UpdateWriteCounter();
}//end stPlainDiskPageManager::WriteHeaderPage

//------------------------------------------------------------------------------
void stPlainDiskPageManager::DisposePage(stPage * page){
   stPageID * next;
   
   // Append to free list
   next = (stPageID *)page->GetData();
   *next = header->Available;
   header->Available = page->GetPageID();
   WritePage(page);

   // Update header   
   header->UsedPages--;
   WriteHeaderPage(headerPage);
   
   // Free resources
   ReleasePage(page);
}//end stPlainDiskPageManager::DisposePage

//------------------------------------------------------------------------------
void stPlainDiskPageManager::NewHeader(tHeader * header, stSize pagesize){
   
   // Magic header. Always "DPM1".
   header->Magic[0] = 'D';
   header->Magic[1] = 'P';
   header->Magic[2] = 'M';
   header->Magic[3] = '1';

   // Organization
   header->PageSize = pagesize;
   
   // Page control
   header->PageCount = 0;
   header->UsedPages = 0;
   header->Available = 0;
}//end stPlainDiskPageManager::NewHeader

//------------------------------------------------------------------------------
bool stPlainDiskPageManager::IsValidHeader(tHeader * header){
   
   return (header->Magic[0] == 'D') &&
         (header->Magic[1] == 'P') &&
         (header->Magic[2] == 'M') &&
         (header->Magic[3] == '1'); 
}//end stPlainDiskPageManager::IsValidHeader
//------------------------------------------------------------------------------

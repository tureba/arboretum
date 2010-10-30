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
//==============================================================================
//  Project: CAMF - Complex objects Access Method Framework
//  Sub-Project: Storage manager
//  File: CStorage.cpp
//  Authors: Ernesto Cuadros Vargas
//           Roberto Figueira Santos Filho
//  Advisors: Agma Juci Machado Traina
//            Caetano Traina Junior
//            Christos Faloutsos
//            Roseli Francelin
//  Date: 06/14/2001
//  Version: 0.01
//
//  Description:
//    Object to manage disk pages. The options include open, close and
//  create files and read, write (change) and create pages.
//
//    All the disk page area is available. It means that no internal (control)
//  data is written in the page. Only when a page is disposed an internal data
//  is written (a pointer to the next disposed page or null). If the user wants
//  to make some logical mark, use any position after the 20th byte
//  (actually, it should be after the 4th, but I decided to reserve some more
//  just in case...). It must be observed that, if the user writes on a
//  disposed page, the data have great chance to be lost.
//
//    The intention with this object is only to help on building indexing
//  methods for spatial and metric data. Yet, the idea is to use this
//  as an academic tool for testing and tuning indexing methods, and not as a
//  commercial one. So, it is not RELIABLE! Don't expect too much.
//
//    To aid the disk access, a cache manager is used. Its page replacement
//  policy is the Natural Selection Page Replacement Policy (a mixing between
//  the LRU and LFU replacement policies).
//
//  Modifications:
//    Author:
//    Date:
//    Description:
//==============================================================================

//---------------------------------------------------------------------------
#pragma hdrstop

#include <arboretum/CStorage.h>

//------------------------------------------------------------------------------
// Constructor. Do nothing. Just initialize members.
//------------------------------------------------------------------------------
CStorage::CStorage(){
   Reset(); // initalize all variables with zero or NULL
}//end CStorage

//------------------------------------------------------------------------------
//Destructor.
//------------------------------------------------------------------------------
CStorage::~CStorage(){
   assert(fd < 0); // Check if the file is closed.
}//end ~CStorage

//------------------------------------------------------------------------------
// Open an existing file. The file fName MUST exists. This object CAN'T have
// another file opened.
//------------------------------------------------------------------------------
void CStorage::Open(const char *fName){
   assert(fd < 0); // check if no other file is opened.

   strcpy(fileName, fName);

   fd = open(fileName, O_RDWR | O_BINARY);
   if (fd < 0){
      throw io_error("Unable to open file.");
   }//end if

   ReadHeader();
   InitializeCache();

   CalcFirstPageDiskAddress();
}//end CStorage::Open

//------------------------------------------------------------------------------
// Close the actual file - must have one opened.
// IMPORTANT: the user is responsible to write his own header. The user MUST
// read the statistics counters before closing the file. If the user is
// interested in the cache statistics and wants to consider all disk writes, it
// must call FlushCache() before colecting the counters.
//------------------------------------------------------------------------------
void CStorage::Close(){
   assert(fd >= 0); // check if is there a file to be closed.

   // the order of the folowing commands must be maintained
   cache.FlushCache();
   WriteHeader();
   close(fd);

   Reset(); // initalize all variables with zero or NULL
}//end CStorage::Close

/**
* Create and open a new file (overwrites the old one).
*
* <P>In addition to the original version, this method can define the
* userHeaderSize automatically. If userHeaderSize is passed as a negative
* value, it will assume that the header size will be about
* pageSize - 32. It is usefull for the implementation of stDiskPageManager.
*
* @param fName File name.
* @param pageSize Size of each page in bytes. It can't be a value smaller than
* 64.
* @param userHeaderSize Size of user header in bytes. Negative values will
* result in an automatically defined size.
* @param cacheNPages Number of pages in cache. It can't be a value smaller than
* 5.
* @warning This object CAN'T have another file opened.
*/
void CStorage::Create(const char *fName, int pageSize, int userHeaderSize,
                     int cacheNPages){
   assert(fd < 0); // check if no other file is opened.

   strcpy(fileName, fName);
   fd = open(fileName, O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
            S_IREAD | S_IWRITE);
   if (fd < 0){
      throw io_error("Unable to create file.");
   }//end if

   // Checking parameters.
   assert((pageSize >= 64) && (cacheNPages >= 5));

   fileHeader.pageSize = pageSize;
   fileHeader.cacheTotalPages = cacheNPages;
   fileHeader.totalPagesInUse = 0;
   fileHeader.disposeList = 0; // the disposeList is buid with disk address, not
                              // pageId. Since the header is located at
                              // position 0 of the file, there is no problem
                              // on using it as invalid address
   fileHeader.totalPagesInDisposeList = 0;
   if (userHeaderSize > 0){
      // A given header size
      fileHeader.userHeaderSize = userHeaderSize;
   }else{
      // Automated header size
      fileHeader.userHeaderSize = pageSize - CSTORAGE_HEADERSIZE;
   }//end if

   WriteHeader();

   CalcFirstPageDiskAddress();

   InitializeCache();
}//end CStorage::Create

//------------------------------------------------------------------------------
// Close the actual file and reopen overwriting all data. There must be an
// opened file. The user header and all data will be destroyed. The user must
// save his header before calling this method (if he needs to save it).
// Only the original characteristics (PageSize, buferSize, userHeaderSize and
// cacheTotalPages) are maintained.
//------------------------------------------------------------------------------
void CStorage::Drop(){

   tFileHeader auxHeader;
   char fName[CSTORAGE_MAX_FNAME_LENGTH];

   assert(fd >= 0); // check if a file is opened.

   auxHeader = fileHeader;
   strcpy(fName, fileName);

   Close();

   Create(fName, auxHeader.pageSize, auxHeader.userHeaderSize,
          auxHeader.cacheTotalPages);
}//end CStorage::Drop

//------------------------------------------------------------------------------
// Read user header.
//------------------------------------------------------------------------------
void CStorage::ReadUserHeader(void *page)
{
   assert(fd >= 0); // check if a file is opened.

   lseek(fd, CSTORAGE_HEADERSIZE, SEEK_SET);
   read(fd, page, fileHeader.userHeaderSize);

   #ifdef CSTORAGE_STATISTICS
   counters.headerRead = (counters.headerRead + 1) %
                       CSTORAGE_STATISTICS_MAXVALUE;
   #endif
}//end CStorage::ReadUserHeader

//------------------------------------------------------------------------------
// Get user header SIZE.
//------------------------------------------------------------------------------
//Added by Josiel in 19/4/2002 to adapt to stDiskPageManager
long CStorage::GetUserHeaderSize(){
   return fileHeader.userHeaderSize;
}//end CStorage::GetUserHeaderSize

//------------------------------------------------------------------------------
// Write user header.
//------------------------------------------------------------------------------
void CStorage::WriteUserHeader(void *page){
   assert(fd >= 0); // check if a file is opened.

   lseek(fd, CSTORAGE_HEADERSIZE, SEEK_SET);
   write(fd, page, fileHeader.userHeaderSize);

   #ifdef CSTORAGE_STATISTICS
   counters.headerWrite = (counters.headerWrite + 1) %
                        CSTORAGE_STATISTICS_MAXVALUE;
   #endif
}//end Storage::WriteUserHeader

//------------------------------------------------------------------------------
// Read a page from cache.
//------------------------------------------------------------------------------
void CStorage::ReadPage(long pageId, void *page){
   void *auxPage;

   assert(fd >= 0); // check if a file is opened.

   cache.ReadPage(pageId, &auxPage);

   memcpy(page, auxPage, fileHeader.pageSize);

   #ifdef CSTORAGE_STATISTICS
   counters.pageRead = (counters.pageRead + 1) % CSTORAGE_STATISTICS_MAXVALUE;
   #endif
}//end CStorage::ReadPage

//------------------------------------------------------------------------------
// Write a page into cache.
//------------------------------------------------------------------------------
void CStorage::WritePage(long pageId, void *page){
   void *auxPage;

   assert(fd >= 0); // check if a file is opened.

   cache.ReadPage(pageId, &auxPage); // brings the page to cache
   memcpy(auxPage, page, fileHeader.pageSize); // writes the page

   cache.WritePage(); // set write flag on cache
   #ifdef CSTORAGE_STATISTICS
   counters.pageWrite = (counters.pageWrite + 1) %
                      CSTORAGE_STATISTICS_MAXVALUE;
   #endif
}//end CStorage::WritePage

//------------------------------------------------------------------------------
// Create a new page and returns the id.
//------------------------------------------------------------------------------
long CStorage::InsertNewPage(void *page){
   long pageId, *pAuxId;
   void *auxPage;

   assert(fd >= 0); // check if a file is opened.

   // test if disposed list is not empty
   if (fileHeader.disposeList){
      pageId = ToPageId(fileHeader.disposeList);

      cache.ReadPage(pageId, &auxPage); // bring the new page to cache

      pAuxId = (long *) auxPage;
      fileHeader.disposeList = *pAuxId; // remove page from dipose list
      fileHeader.totalPagesInUse++;
      fileHeader.totalPagesInDisposeList--;

      memcpy(auxPage, page, fileHeader.pageSize); // write the page
      cache.WritePage(); // set write flag on cache
   }else{
      pageId = GetTotalPagesIncludingDisposed();
      fileHeader.totalPagesInUse++; // it MUST be here, otherwise pageId will
                                    // be invalid to the method diskWriteBuff

      DiskWritePage(pageId, page); // write the new page and expand file

      cache.NewPage(pageId, &auxPage); // adjust cache and bring new
                                            // (blank) page
   }//end if

   WriteHeader();

   #ifdef CSTORAGE_STATISTICS
   counters.pageWrite = (counters.pageWrite + 1) %
                      CSTORAGE_STATISTICS_MAXVALUE;
   counters.newPage = (counters.newPage + 1) % CSTORAGE_STATISTICS_MAXVALUE;
   #endif

   return pageId;
}//end CStorage::InsertNewPage

//------------------------------------------------------------------------------
// Insert the page in the dispose list.
//------------------------------------------------------------------------------
void CStorage::FreePage(long pageId){
   long *pAuxId;
   void *auxPage;

   assert(fd >= 0); // check if a file is opened.

   cache.ReadPage(pageId, &auxPage); // bring page to cache

   // insert page in dispose list
   pAuxId = (long *) auxPage;
   *pAuxId = fileHeader.disposeList;
   fileHeader.disposeList = ToFileCursor(pageId);

   fileHeader.totalPagesInDisposeList++;
   fileHeader.totalPagesInUse--;
   WriteHeader();

   cache.WritePage(); // set write flag on cache

   #ifdef CSTORAGE_STATISTICS
   counters.pageWrite = (counters.pageWrite + 1) %
                      CSTORAGE_STATISTICS_MAXVALUE;
   #endif
}//end CStorage::FreePage

//------------------------------------------------------------------------------
// Reinstantiate the cache with newCachePages number of pages and makes it
// default for future use.
//------------------------------------------------------------------------------
void CStorage::ResizeCache(int newCacheNPages){
   assert(fd >= 0); // check if a file is opened.

   cache.FlushCache();
   fileHeader.cacheTotalPages = newCacheNPages;
   WriteHeader();
   InitializeCache();
}//end CStorage::ResizeCache

//------------------------------------------------------------------------------
// Calculate the address of the first disk page, trying to make it a multiple
// of the page size.
//------------------------------------------------------------------------------
inline void CStorage::CalcFirstPageDiskAddress(){
   firstPageDiskAddress = CSTORAGE_HEADERSIZE + fileHeader.userHeaderSize;
   if (firstPageDiskAddress <= fileHeader.pageSize){
      firstPageDiskAddress = fileHeader.pageSize;
   }else{
      int dlintFactor = firstPageDiskAddress / fileHeader.pageSize;

      if (firstPageDiskAddress != (fileHeader.pageSize * dlintFactor))
         dlintFactor++;
         
      firstPageDiskAddress = fileHeader.pageSize * dlintFactor;
   }//end if
}//end Storage::CalcFirstPageDiskAddress

//------------------------------------------------------------------------------
// Used by the cache to read a page from disk.
//------------------------------------------------------------------------------
void CStorage::DiskReadPage(long pageId, void *page){
   // check if pageId is valid.
   assert((pageId >= 0) && (pageId < GetTotalPagesIncludingDisposed()));

   lseek(fd, ToFileCursor(pageId), SEEK_SET);
   read(fd, page, fileHeader.pageSize);

   #ifdef CSTORAGE_STATISTICS
   counters.diskRead = (counters.diskRead + 1) % CSTORAGE_STATISTICS_MAXVALUE;
   #endif
}//end CStorage::DiskReadPage

//------------------------------------------------------------------------------
// Used by the cache to write a page to disk.
//------------------------------------------------------------------------------
void CStorage::DiskWritePage(long pageId, void *page){
   // check if pageId is valid.
   assert((pageId >= 0) && (pageId < GetTotalPagesIncludingDisposed()));

   lseek(fd, ToFileCursor(pageId), SEEK_SET);
   write(fd, page, fileHeader.pageSize);

   #ifdef CSTORAGE_STATISTICS
   counters.diskWrite = (counters.diskWrite + 1) %
                      CSTORAGE_STATISTICS_MAXVALUE;
   #endif
}//end CStorage::DiskWritePage

//------------------------------------------------------------------------------
// Initalize all variables with zero or NULL
//------------------------------------------------------------------------------
void CStorage::Reset(){
   fileHeader.pageSize =
   fileHeader.totalPagesInUse =
   fileHeader.disposeList =
   fileHeader.totalPagesInDisposeList =
   fileHeader.userHeaderSize = 0;
   fileHeader.cacheTotalPages = 100;

   *fileName = '\0';

   fd = -1; // fd is integer. Zero and NULL doesn't work. -1 is the invalid value

   firstPageDiskAddress = CSTORAGE_HEADERSIZE;

   #ifdef CSTORAGE_STATISTICS
   ResetStatistics();
   #endif
}//end CStorage::Reset

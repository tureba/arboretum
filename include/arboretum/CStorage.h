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
* This file contains the implementation of the class CStorage. This version was modified
* to work with the GBDI Arboretum library. It is used by the implementation of
* stDiskPageManager.

*/
//==============================================================================
//  Project: CAMF - Complex objects Access Method Framework
//  Sub-Project: Storage manager
//  File: CStorage.h
//  Authors: Ernesto Cuadros Vargas
//           Roberto Figueira Santos Filho
//  Advisors: Agma Juci Machado Traina
//            Caetano Traina Junior
//            Christos Faloutsos
//            Roseli Francelin
//  Date: 06/14/2001
//  Version: 0.01
//==============================================================================
#ifndef CSTORAGEH
#define CSTORAGEH

// Common headers
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <assert.h>

#include <arboretum/stCommon.h>
#include <arboretum/stCommonIO.h>
#include <arboretum/CNSCache.h>
#include <arboretum/stException.h>
#include <arboretum/stUtil.h>

//---- Constant definitions ----
#define CSTORAGE_MAX_FNAME_LENGTH 256
#define CSTORAGE_STATISTICS
#define CSTORAGE_STATISTICS_MAXVALUE (MAXLONG - 100)

//==============================================================================
// Class CStorage definitions.
//==============================================================================
/**
* Object to manage disk pages. The options include open, close and
* create files and read, write (change) and create pages.
*
* <P>All the disk page area is available. It means that no internal (control)
* data is written in the page. Only when a page is disposed an internal data
* is written (a pointer to the next disposed page or null). If the user wants
* to make some logical mark, use any position after the 20th byte
* (actually, it should be after the 4th, but I decided to reserve some more
* just in case...). It must be observed that, if the user writes on a
* disposed page, the data have great chance to be lost.
*
* <P>The intention with this object is only to help on building indexing
* methods for spatial and metric data. Yet, the idea is to use this
* as an academic tool for testing and tuning indexing methods, and not as a
* commercial one. So, it is not RELIABLE! Don't expect too much.
*
* <P>To aid the disk access, a cache manager is used. Its page replacement
* policy is the Natural Selection Page Replacement Policy (a mixing between
* the LRU and LFU replacement policies).
*
* <P>This version was modified from its original version by Fabio Jun Takada
* Chino and Josiel Maimone de Figueiredo.
*
* @author Ernesto Cuadros Vargas (Original Author)
* @author Roberto Figueira Santos Filho (Original Author)
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup storage
*/
class CStorage // Naming: csto
{
   public:
      /**
      * Creates this class. Use Create() or Open() to initialize it.
      */
      CStorage();

      /**
      * Disposes this instance and releases all allocated resources.
      */
      ~CStorage();

      /**
      * Open an existing file. The file given file name MUST exists. This object CAN'T
      * have another file opened.
      *
      * @param fName The file name.
      * @exception io_error If the file can not be opened.
      */
      void Open(const char *fName);

      /**
      * Close the actual file - must have one opened.
      */
      void Close();

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
      * @exception io_error If the file can not be created.
      */
      void Create(const char *fName, int pageSize, int userHeaderSize,
                  int cacheNPages = 100);

      /**
      * Returns true if file is open or false otherwise.
      */
      bool IsOpened(){
         return (fd >= 0);
      }//end IsOpened

      /**
      * Close the actual file and reopen overwriting all data. There must be an
      * opened file.
      */
      void Drop();

      /**
      * Return the size of the file including headers and records in dispose
      * list (if it is not empty). Inline.
      */
      long GetFileSize(){
         return lseek(fd, 0, SEEK_END);
      }//end GetFileSize

      /*
      * Reads user header.
      *
      * @warning This operation don't use the cache.
      */
      void ReadUserHeader(void *page);

      /**
      * Returns the user header size.
      */
      long GetUserHeaderSize();

      /**
      * Write user header.
      *
      * @warning This operation don't use the cache.
      */
      void WriteUserHeader(void *page);

      /**
      * Reads a page.
      *
      * @param pageId The page ID.
      * @param page The page.
      */
      void ReadPage(long pageId, void *page);

      /**
      * Writes a page.
      *
      * @param pageId The page ID.
      * @param page The page.
      */
      void WritePage(long pageId, void *page);

      /**
      * Creates a new page and return its id.
      *
      * @param page The new page.
      */
      long InsertNewPage(void *page);

      /**
      * Insert the page in the dispose list.
      *
      * @param pageId
      */
      void FreePage(long pageId);

      /**
      * Flushes the cache.
      */
      void FlushCache(){
         cache.FlushCache();
      }//end FlushCache

      /**
      * Returns the number of pages in cache.
      */
      int GetCacheTotalPages(){
         return fileHeader.cacheTotalPages;
      }//end GetCacheTotalPages

      /**
      * Reinstantiate the cache with newCachePages number of pages and makes it
      * default for future use.
      *
      *  @param newCacheNPages The new number of pages in the cache.
      */
      void ResizeCache(int newCacheNPages);

      /**
      * Returns the page size in bytes.
      */
      int GetPageSize(){
         return fileHeader.pageSize;
      }//end GetPageSize

      /**
      * Returns the number of allocated pages.
      */
      long GetTotalPagesInUse(){
         return fileHeader.totalPagesInUse;
      }//end GetTotalPagesInUse

      /**
      * Returns the total number of pages in the file, including disposed pages.
      */
      long GetTotalPagesIncludingDisposed(){
         return fileHeader.totalPagesInUse + fileHeader.totalPagesInDisposeList;
      }//end GetTotalPagesIncludingDisposed

      //---------------------------------------------
      // Statistics Related - When activated.
      //---------------------------------------------
      #ifdef CSTORAGE_STATISTICS
      
      /**
      * Type of counters for statistics.
      */
      struct statisticsCounters {

         /**
         *  Read user's header counter
         */
         long headerRead;

         /**
         * Write user's header counter
         */
         long headerWrite;

         /**
         * Read page counter.
         */
         long pageRead;

         /**
         * Write page counter - new pages are counted as page write too.
         */
         long pageWrite;

         /**
         * New page counter.
         */
         long newPage;
        
         /**
         * pageRead + pageWrite
         */
         long totalPageAccess; // 
        
         /**
         * Read disk counter. 
         * This counter is incremented only when a cache fault occurs
         * its value reflects the actual number of accessed pages. If
         * pageSize > pageSize, it will count the number of pages accessed to
         * retrieve an entire page. To measure the cache efficiency, these
         * counters must be compared with the relativeTotalDiskAccess counter.         
         */
         long diskRead;

         /**
         * Write disk counter - new pages are counted as disk write too.
         * This counter is incremented only when a cache fault occurs
         * its value reflects the actual number of accessed pages. If
         * pageSize > pageSize, it will count the number of pages accessed to
         * retrieve an entire page. To measure the cache efficiency, these
         * counters must be compared with the relativeTotalDiskAccess counter.         
         */
         long diskWrite;
      };
      
      /**
      * Counter data.
      */
      statisticsCounters counters;

      /**
      * Returns statistics.
      */
      statisticsCounters & GetStatistics() {
         counters.totalPageAccess = counters.pageRead + counters.pageWrite;

         return counters;
      }//end GetStatistics

      /**
      * Restarts statistics.
      */
      void ResetStatistics() {
         counters.headerRead =
         counters.headerWrite =
         counters.pageRead =
         counters.pageWrite =
         counters.newPage =
         counters.totalPageAccess =
         counters.diskRead =
         counters.diskWrite = 0;
      }//end ResetStatistics
      #endif

   private:
      //---------------------------------------------
      // Constants and types.
      //---------------------------------------------
      #define FROT 4  // 16 records per block - cache attribute

      struct tFileHeader {
         int pageSize; // size of each page - defined in creation time
         int cacheTotalPages; // number of pages in cache
         long totalPagesInUse; // total number of valid pages
         long disposeList; // lists of the disposed records
         long totalPagesInDisposeList; // total number of disposed pages
         long userHeaderSize; // size of the user's header
      };//end tFileHeader
      #define CSTORAGE_HEADERSIZE sizeof(tFileHeader)

      /**
      * Name of the file in use
      */
      char fileName[CSTORAGE_MAX_FNAME_LENGTH];

      /**
      * File descriptor.
      */
      int  fd;

      /**
      * Header.
      */
      tFileHeader fileHeader;
    
      /**
      * Position on disk of the first page. It is used to calculate other pages addresses.
      */
      long firstPageDiskAddress;
    
      /**
      * The cache pointer.
      */
      CNSCache<CStorage> cache;

      friend class CNSCache<CStorage>;

      /**
      * Reads system header.
      */
      void ReadHeader(){
         lseek(fd, 0, SEEK_SET);
         read(fd, &fileHeader, CSTORAGE_HEADERSIZE);
      }//end ReadHeader

      /**
      * Writes system header.
      */
      void WriteHeader(){
         lseek(fd, 0, SEEK_SET);
         write(fd, &fileHeader, CSTORAGE_HEADERSIZE);
      }//end WriteHeader

      /**
      * Calculates the address of the first disk page, trying to make it a
      * multiple of the page size.
      */
      void CalcFirstPageDiskAddress();

      /**
      * Converts the file offset to page id.
      *
      * @param fileCursor The offset of the file.
      */
      long ToPageId(long fileCursor){
         return ((fileCursor - firstPageDiskAddress) / fileHeader.pageSize);
      }//end ToPageId

      /**
      * Converts the page id to file offset.
      *
      * @param pageId The page id.
      */
      long ToFileCursor(long pageId){
         return (firstPageDiskAddress + (pageId * fileHeader.pageSize));
      }//end ToFileCursor

      /**
      * Initializes the cache manager.
      */
      void InitializeCache(){
         cache.MountCache(this, GetTotalPagesIncludingDisposed(),
                       FROT, fileHeader.cacheTotalPages, fileHeader.pageSize);
      }//end InitializeCache

      /**
      * Used by the cache to read a page from disk.
      *
      * @param pageId The page id.
      * @param page The page data.
      */
      void DiskReadPage(long pageId, void *page);

      /**
      * Used by the cache to write a page to disk.
      *
      * @param pageId The page id.
      * @param page The page data.
      */
      void DiskWritePage(long pageId, void *page);

      /**
      * Initalize all variables with zero or NULL.
      */
      void Reset();
      
};//end CStorage

//------------------------------------------------------------------------------
#endif // CSTORAGEH

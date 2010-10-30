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
* This file defines the class stDiskPageManager.
*
* @version 1.0
* $Revision: 1.18 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
*/
// Copyright (c) 2002 GBDI-ICMC-USP
#ifndef __STDISKPAGEMANAGER_H
#define __STDISKPAGEMANAGER_H

#include <string>
#include <stdexcept>
using namespace std;

#include  <arboretum/stPageManager.h>
#include  <arboretum/CStorage.h>
#include  <arboretum/stUtil.h>

//==============================================================================
// stDiskPageManager
//------------------------------------------------------------------------------
/**
* This class controls the Slim Tree disk access.
* This class implements the abstract class stPageManager.
*
*<P> This version uses CStorage to disk access, ie, all disk controls is made by
* CStorage.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @see stPageManager
* @see stMemoryPageManager
* @see CStorage
* @ingroup storage
*/
class stDiskPageManager: public stPageManager{
   public:
      /**
      * Creates a new instance of this class. Use Create() or Open() to prepare
      * this instance for use.
      *
      * @deprecated This method will be deprecated in future versions.
      */
      stDiskPageManager(){
         myStorage = NULL;
      }//end stDiskPageManager

      /**
      * Creates a new instance of this class. This constructor will create a new
      * file with the given name.
      *
      * @param fName The file name.
      * @param pagesize Size of each page in file. This value must be larger
      * or equal than 64.
      * @param userHeaderSize Size of header.
      * @param cacheNPages Number of pages holded by cache. This value must
      * be larger or equal than 5.
      * @exception io_error If the file can not be created.
      */
      stDiskPageManager(const string & fName, stSize pagesize,
            stSize userHeaderSize = -1,
            int cacheNPages = 5);

      /**
      * Creates a new instance of this class. This constructor will open an
      * existing file.
      *
      * @param fName The file name.
      * @exception io_error If the file can not be opened.
      */
      stDiskPageManager(const string & fName);

      /**
      * Disposes this page and free all allocated resources.
      */
      virtual ~stDiskPageManager();

      /**
      * This method will checks if this page manager is empty.
      * If this method returns true, the stSlimTree will create a
      * new tree otherwise it will continue to use the existing tree.
      *
      * @return True if the page manager is empty or false otherwise.
      */
      virtual bool IsEmpty();

      /**
      * Returns the header page. This method will return a special
      * page that will be used by SlimTree to write some information
      * about the tree itself.
      *
      * <P>Since this page must always exist, this method will always
      * return a valid page for reading/writing. Use WritePage() to
      * write this page.
      *
      * <P>The returning instance of stPage will be locked to prevent
      * its reuse by this page manager. Use ReleasePage() to unlock
      * this instance.
      *
      * @return The header page.
      * @see WritePage()
      * @see ReleasePage()
      */
      virtual stPage * GetHeaderPage();

      /**
      * Returns the page with the given page ID. This method will
      * return a valid page for reading/writing. Use WritePage() to
      * write this page.
      *
      * <P>The returning instance of stPage will be locked to prevent
      * its reuse by this page manager. Use ReleasePage() to unlock
      * this instance.
      *
      * @param pageid The desired page id.
      * @return The page or NULL for an invalid page ID.
      * @see WritePage()
      * @see ReleasePage()
      */
      virtual stPage * GetPage(stPageID pageid);

      /**
      * Releases this instace for reuse by this page manager.
      * Since some implementations of page manager will reuse
      * instances of stPage to avoid unnecessary resource
      * reallocations, each page instance must be locked until
      * it becomes unecessary.
      *
      * <P>The stSlimTree will always call this method when a
      * stPage instance will not be used in a near future.
      *
      * @param page The locked page.
      * @see GetPage()
      * @see GetHeaderPage()
      */
      virtual void ReleasePage(stPage * page);

      /**
      * Allocates a new page for use. As GetPage() and
      * GetHeaderPage(), the returning instance will be
      * locked to prevent reuse by this page manager.
      *
      * <P>To dispose this page (make it free), use DisposePage().
      *
      * @return A new page or NULL for errors.
      * @see ReleasePage()
      * @see WritePage()
      * @see DisposePage()
      */
      virtual stPage * GetNewPage();

      /**
      * Writes the given page to the disk. This method
      * will write the page but will not release it. Use
      * ReleasePage() to do it.
      *
      * @param page The page to be written.
      * @see ReleasePage()
      */
      virtual void WritePage(stPage * page);

      /**
      * Writes the header page to the disk.
      *
      * @param headerpage The page to be written.
      * @see ReleasePage()
      * @see WritePage()
      * @see GetHeaderPageSize()
      */
      virtual void WriteHeaderPage(stPage * headerpage);

      /**
      * Disposes the given page. This method will make the page
      * available (not allocated) for the next calls of GetNewPage().
      *
      * <P>Since this page will not be used anymore, this method will
      * release the lock for this page instance.
      *
      * @param page The page to be disposed.
      * @see GetNewPage()
      */
      virtual void DisposePage(stPage * page);

      /**
      * Create a disk file that will store the data
      *
      * @param fName The file name.
      * @param pagesize Size of each page in file. This value must be larger
      * or equal than 64.
      * @param userHeaderSize Size of header.
      * @param cacheNPages Number of pages holded by cache. This value must
      * be larger or equal than 5.
      *
      * @see Create(char*,int,int)
      * @see Open()
      * @see Drop()
      * @deprecated This method will be deprecated.
      */
      void Create(const char *fName, int pagesize, int userHeaderSize = -1,
            int cacheNPages = 5);

      /**
      * Open an existing file.
      *
      * @param fname File name.
      *
      * @see Create()
      * @see Drop()
      * @deprecated This method will be deprecated.
      */
      void Open(char *fname);

      /**
      * Close the file and reopen overwriting all data.
      *
      * <P>There must be an opened file. The user header and all data will be
      * destroyed. The user must save his header before calling this method (if
      * he needs to save it). Only the original characteristics (Pagesize,
      * buferSize, userHeaderSize and cacheTotalPages) are maintained.
      *
      * @see Create()
      * @see Open()
      */
      void ResetFile();
      
      /**
      * Returns the minimum size of a page. The size of the header page is
      * always ignored since it may be smaller than others.
      */ 
      virtual stSize GetMinimumPageSize(){
         return pageSize;
      }//end GetMinimumPageSize

      /**
      * Flushes the cache data to disk.
      */
      void Flush();

      /**
      * Restarts the statistics.
      */
      virtual void ResetStatistics();

      /**
      * This method returns the number of disk reads. Since this class caches
      * reads and writes, the returning value will always be less or equal to
      * the number of logic reads returned by GetReadCount().
      *
      * @see ResetStatistics()
      */
      unsigned long GetDiskReadCount();

      /**
      * This method returns the number of disk writes. Since this class caches
      * reads and writes, the returning value will always be less or equal to
      * the number of logic reads returned by GetWriteCount().
      *
      * @see ResetStatistics()
      */
      unsigned long GetDiskWriteCount();

      /**
      * This method returns the number of disk access. Since this class caches
      * reads and writes, the returning value will always be less or equal to
      * the number of logic reads returned by GetAccessCount().
      *
      * @see ResetStatistics()
      */
      unsigned long GetDiskSAccessCount(){
         return GetDiskWriteCount() + GetDiskReadCount();
      }//end GetDiskSAccessCount

      /**
      * Returns the file name.
      */
      const string & GetFileName(){

         return fileName;
      }//end GetFileName

      /**
      * Returns the last page ID.
      *
      * @warning This method is experimental.
      * @deprecated This method was deprecated. There is no replacement for it.
      */
      stPageID GetLastPageID(){
         return myStorage->GetTotalPagesIncludingDisposed() - 1;
      }//end GetLastPageID()

   private:

      /**
      * Type of the instance cache used by this disk page manager.
      */
      typedef stInstanceCache <stPage, stPageAllocator> stPageInstanceCache;
      
      /**
      * The page instance cache used by this disk page manager. The header
      * page will not use the cache because it has a different size.
      */
      stPageInstanceCache * pageInstanceCache;
   
      /**
      * Controler of disk access.
      *
      * @see CStorage
      */
      CStorage * myStorage;

      /**
      * Page size in use.
      */
      stSize pageSize;

      /**
      * File name.
      */
      string fileName;

      /**
      * Get the header page SIZE.
      *
      * @return The header page size.
      * @see ReleasePage()
      * @see WritePage()
      * @see WriteHeaderPage()
      */
      long GetHeaderPageSize();

};//end stDiskPageManager
#endif //__STDISKPAGEMANAGER_H


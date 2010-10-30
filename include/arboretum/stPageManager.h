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
* This file defines the abstract class stPageManager.
*
* @version 1.0
* $Revision: 1.19 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
*/
// Copyright (c) 2002 GBDI-ICMC-USP
#ifndef __STPAGEMANAGER_H
#define __STPAGEMANAGER_H

#include  <arboretum/stTypes.h>
#include  <arboretum/stPage.h>

/**
* This class defines the abstract class stPageManager. All
* classes that provide page management service to the SlimTree
* must extend this class.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @see stDiskPageManager
* @see stMemoryPageManager
* @see CStorage
*/
class stPageManager{

   public:

      /**
      * This is de default destructor of this class.
      */
      virtual ~stPageManager(){}

      /**
      * This method will checks if this page manager is empty.
      * If this method returns true, the stSlimTree will create a
      * new tree otherwise it will continue to use the existing tree.
      *
      * @return True if the page manager is empty or false otherwise.
      */
      virtual bool IsEmpty() = 0;

      /**
      * This method writes the header page back to the page manager.
      *
      * <P>The default implementation calls the WritePage() to header page. If
      * a page manager needs a special way to write the header page, this
      * method must be overwritten.
      *
      * <P>As WritePage() do, this method do not releases the header page. You
      * must call ReleasePage() to release it.
      *
      * @param headerpage The header page.
      */
      virtual void WriteHeaderPage(stPage * headerpage){
         WritePage(headerpage);
      }//end WriteHeaderPage

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
      virtual stPage * GetHeaderPage() = 0;

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
      virtual stPage * GetPage(stPageID pageid) = 0;

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
      virtual void ReleasePage(stPage * page) = 0;

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
      virtual stPage * GetNewPage() = 0;

      /**
      * Writes the given page to the disk. This method
      * will write the page but will not release it. Use
      * ReleasePage() to do it.
      *
      * @param page The page to be written.
      * @see ReleasePage()
      * @see WriteHeaderPage()
      */
      virtual void WritePage(stPage * page) = 0;

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
      virtual void DisposePage(stPage * page) = 0;

      /**
      * Restarts the statistics.
      *
      * @see GetReadCount()
      * @see GetWriteCount()
      * @see GetAccessCount()
      */
      virtual void ResetStatistics(){

         this->ReadCount = 0;
         this->WriteCount = 0;
      }//end ResetStatistics

      /**
      * Gets the number of reads performed since the last call of
      * ResetStatistics().
      *
      * @see ResetStatistics()
      * @see GetWriteCount()
      */
      long int GetReadCount(){
         return this->ReadCount;
      }//end GetReadCount

      /**
      * Gets the number of writes performed since the last call of
      * ResetStatistics().
      *
      * @see ResetStatistics()
      * @see GetReadsCount()
      *
      */
      long int GetWriteCount(){
         return this->WriteCount;
      }//end GetWriteCount
      
      /**
      * Returns the total page access. This value is the sum of the
      * read count and write count.
      */
      long int GetAccessCount(){
         return this->ReadCount + this->WriteCount;
      }//end GetAccessCount
      
      /**
      * Returns the minimum size of a page. The size of the header page is
      * always ignored since it may be smaller than others.
      */ 
      virtual stSize GetMinimumPageSize() = 0;

   protected:

      /**
      * This method updates the read counter.
      *
      * @param count The number o reads to add to the counter.
      */
      void UpdateReadCounter(unsigned long count = 1){
         ReadCount += count;
      }//end UpdateReadCounter
      
      /**
      * This method updates the write counter.
      *
      * @param count The number o writes to add to the counter.
      */
      void UpdateWriteCounter(unsigned long count = 1){
         WriteCount += count;
      }//end UpdateWriteCounter

   private:

      /**
      * Number of reads. This value is used to compute
      * statistics.
      *
      * @warning Each implementation of Page Manager must update this value
      * when necessary.
      */
      long int ReadCount;

      /**
      * Number of writes. This value is used to compute
      * statistics.
      *
      * @warning Each implementation of Page Manager must update this value when
      * necessary.
      */
      long int WriteCount;
      
};//end stPageManager

#endif //__STPAGEMANAGER_H

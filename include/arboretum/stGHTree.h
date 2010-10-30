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
* This file defines the class stGHTree.
* $Author: marcos $
*
* @author Ives Renê Venturini Pola (ives@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2004 GBDI-ICMC-USP


#ifndef __STGHTREE_H
#define __STGHTREE_H

#include <math.h>
#include <arboretum/stCommon.h>
#include <arboretum/stTypes.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stGHNode.h>


/**
* This class template implements a GH Tree. The GH Tree has the same
* external interface and behavior of a Metric Tree.
*
* <P>It can perform both range and k-nearest neighbout queries but without
* the prunning power of other structures, being linear on the wrost case.
* Althrough it has a weak prunning power, this tree can be used for comparison
* with others structures.
*
* <P>This class was developed to generate perfect answers to queries. It
* allows the build of automated test programs for other metric trees
* implemented by this library.
*
* @author Ives Renê Venturini Pola (ives@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Finish the implementation (some queries).
* @version 1.0
* @ingroup GH
*/
template <class ObjectType, class EvaluatorType>
class stGHTree: public stMetricTree<ObjectType, EvaluatorType>{

   public:

      /**
      * This type defines the header of the GHTree.
      */
      typedef struct GHTreeHeader{
         /**
         * Magic number. This is a short string that must contains the magic
         * string "GH-3". It may be used to validate the file (this feature
         * is not implemented yet).
         */
         char Magic[4];
      
         /**
         * The root.
         */
         stPageID Root;

         /**
         * The height of the tree
         */
         stCount Height;

         /**
         * Total number of objects.
         */
         stCount ObjectCount;

         /**
         * The number of the nodes
         */
         stSize NodeCount;
      } stGHTreeHeader;

      /**
      * This is the class that abstracts the object.
      */
      typedef ObjectType tObject;

      /**
      * This is the class that abstracts the metric evaluator.
      */
      typedef EvaluatorType tMetricEvaluator;

      /**
      * This is the class that abstracts an result set.
      */
      typedef stResult <ObjectType> tResult;

      /**
      * Creates a new instance of the GHTree.
      *
      * @param pageman The Page Manager to be used.
      */
      stGHTree(stPageManager * pageman);

      /**
      * Disposes this tree and releases all associated resources.
      */
      virtual ~stGHTree(){     
         // Flus header page.
         FlushHeader();
      }//end ~stGHTree                                  
      
      #ifdef __stDEBUG__
         /**
         * Get root page id.
         *
         * @warning This method is public only if __stDEBUG__ is defined at compile
         * time.
         */
         stPageID GetRoot(){
            return this->Header->Root;
         }//end GetRoot
      #endif //__stDEBUG__

      /**
      * Returns the height of the tree.
      */
      virtual stCount GetHeight(){
         return Header->Height;
      }//end GetHeight

      /**
      * Returns the number of objetcs of this tree.
      */
      virtual long GetNumberOfObjects(){
         return Header->ObjectCount;
      }//end GetNumberOfObjects

      /**
      * This method adds an object to the GHTree.
      * This method may fail it he object size exceeds .
      *
      * @param obj The object to be added.
      * @return True for success or false otherwise.
      */
      virtual bool Add(tObject * obj);

      /**
      * This method will perform a range query.
      * The result will be a set of pairs object/distance.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void RangeQuery()
      */
      tResult * RangeQuery(tObject * sample, stDistance range);

      /**
      * This method will perform a K-Nearest Neighbor query.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void NearestQuery
      */
      tResult * NearestQuery(tObject * sample, stCount k, bool tie = false);

   private:

      /**
      * The header page. It will be kept in memory all the time to avoid
      * reads.
      */
      stPage * HeaderPage;

      /**
      * The header of the "tree".
      */
      stGHTreeHeader * Header;

      /**
      * If true, the header mus be written to the page manager.
      */
      bool HeaderUpdate;

      #ifndef __stDEBUG__
         /**
         * Get root page id.
         */
         stPageID GetRoot(){
            return this->Header->Root;
         }//end GetRoot
      #endif // __stDEBUG__

      /**
      * Sets a new root.
      */
      void SetRoot(stPageID root){
         Header->Root = root;
         HeaderUpdate = true;
      }//end SetRoot

      /**
      * Loads the header from the page manager.
      */
      void LoadHeader();

      /**
      * Sets all header's fields to default values.
      *
      * @warning This method will destroy the tree.
      */
      void DefaultHeader();

      /**
      * Updates the header in the file if required.
      */
      void WriteHeader(){
         if (HeaderUpdate){
            this->myPageManager->WriteHeaderPage(HeaderPage);
            this->HeaderUpdate = false;
         }//end if
      }//end WriteHeader

      /**
      * Disposes the header page if it exists. It also updates its contents
      * before destroy it.
      *
      * <P>This method is called by the destructor.
      */
      void FlushHeader(){
         if (HeaderPage != NULL){
            if (Header != NULL){
               WriteHeader();
            }//end if
            this->myPageManager->ReleasePage(HeaderPage);
         }//end if
      }//end FlushHeader
      
      /**
      * Creates a new empty page and updates the node counter.
      */
      stPage * NewPage(){
         this->Header->NodeCount++;
         return this->myPageManager->GetNewPage();
         this->HeaderUpdate = true;
      }//end NewPage

      /**
      * Disposes a given page and updates the page counter.
      */
      void DisposePage(stPage * page){
         this->Header->NodeCount--;
         this->myPageManager->DisposePage(page);
         this->HeaderUpdate = true;
      }//end DisposePage

      /**
      * This method will perform a range query.
      * The result will be a set of pairs object/distance.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the result.
      * @see tResult * RangeQuery()
      */
      void RangeQuery(stPageID pageID, tResult * result,
                      ObjectType * sample, stDistance range);

      /**
      * This method will perform a K Nearest Neighbor query.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The number of neighbours.
      * @see tResult * NearestQuery
      */
      void NearestQuery(stPageID pageID, tResult * result, ObjectType * sample,
                        stDistance & rangeK, stCount k);

};//end stGHTree

#include <arboretum/stGHTree.cc>

#endif //__STGHTREE_H


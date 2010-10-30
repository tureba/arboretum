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
* This file defines the class stVPTree.
*
* @version 1.0
* $Date: 2005/01/14 16:54:15 $
* @author: Ives Renê Venturini Pola (ives@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2004 GBDI-ICMC-USP


#ifndef __STVPTREE_H
#define __STVPTREE_H

#include <math.h>
#include <arboretum/stCommon.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stVPNode.h>

/**
* This class template implements a VP-tree. The VP-tree has the same
* external interface and behavior of a Metric Tree.
*
* <P>It can perform both range and k-nearest neighbour queries, and so its
* combinations and variations.
*
* This is a binary static memory metric tree
*
* <P>This class was developed to generate perfect answers to queries. It
* allows the build of automated test programs for other metric trees
* implemented by this library.
*
* @author Ives Renê Venturini Pola (ives@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation review.
* @version 1.0
* @ingroup VP
*/
template <class ObjectType, class EvaluatorType>
class stVPTree: public stMetricTree<ObjectType, EvaluatorType>{

   public:

      /**
      * This type defines the header of the VPTree.
      */
      typedef struct VPTreeHeader{
         /**
         * Magic number. This is a short string that must contains the magic
         * string "VP-3". It may be used to validate the file (this feature
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
      } stVPTreeHeader;

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
      * Creates a new instance of the VPTree.
      *
      * @param pageman The Page Manager to be used.
      */
      stVPTree(stPageManager * pageman);

      /**
      * Disposes this tree and releases all associated resources.
      */
      virtual ~stVPTree();

      /**
      * This method adds an object to the VPTree.
      * This method may fail it he object size exceeds the page size.
      *
      * @param obj The object to be added.
      * @return True for success or false otherwise.
      */
      bool Add(tObject ** objects, long listSize);

      /**
      * This method implements the Add method to insert object one-by-one in
      * the VP-Tree.
      *
      * @return success for the VPTree build.
      */
      bool Add(tObject * obj);

      /**
      * This method build the VP-tree after some Add calls.
      *
      * @return success for the VPTree build.
      */
      bool MakeVPTree();

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
      * This method will perform a range query. The result will be a set of
      * pairs object/distance.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      tResult * RangeQuery(tObject * sample, stDistance range);

      /**
      * This method will perform a k nearest neighbor query.
      *
      * @param sample The sample object.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
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
      stVPTreeHeader * Header;

      /**
      * If true, the header mus be written to the page manager.
      */
      bool HeaderUpdate;

      /**
      * To add object one-by-one.
      */
      tObject ** Objects;

      /**
      * The Used Size.
      */
      long Size;

      /**
      * The Maximum Capacity.
      */
      long Capacity;

		/**
		* Increment.
		*/
      long Increment;

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
               this->WriteHeader();
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
         Header->NodeCount--;
         this->myPageManager->DisposePage(page);
         HeaderUpdate = true;
      }//end DisposePage

      /**
      * This method recursively build the VP-tree based on the object list
      *
      * @return success for the VPTree build.
      */
      stPageID MakeVPTree(tObject ** objects, stDistanceIndex * & selected,
                          long size);

      /**
      * Used by MakeVPTree, selects a vantage point from a dataset from the
      * corner of the space.
      */
      long SelectVP(tObject ** objects, stDistanceIndex * selected, long size);

      /**
      *  This method calculates the median element, sorting the "sample" using
      *  the "vantage" as vantage point (index of the object in "obj".
      */
      long GetMedian(tObject ** objects, stDistanceIndex * sample, long sampleSize,
                    long vantage);

      /**
      *  Calculates the 2nd=Momento of the sample
      */
      stDistance GetSecondMoment(stDistanceIndex * sample, long sampleSize, long median);

      /**
      *  This method calculate the sample size given the actual size
      */
      long PickSampleSize(long size){
         long sampleSize;

         // Try to get 10%
         sampleSize = (long) ceil(0.09 * (double )size);
         if (sampleSize < 50){
            sampleSize = 50;
            if (size < sampleSize){
               sampleSize = size;
            }//end if
         }//end if
         if (sampleSize > 500){
            sampleSize = 500;
            if (size < sampleSize){
               sampleSize = size;
            }//end if
         }//end if
         return sampleSize;
      }//end PickSampleSize

      /**
      *  Used to make a sample of the vector "selected"
      */
      void MakeSample(stDistanceIndex * sample, long sampleSize,
                      stDistanceIndex * selected, long size);

		/**
		* Expands the capacity of the object vector by adding increment
		* entries to the current capacity.
		*/
      void Resize();
      
      /**
      * Support for the RangeQuery, recursive code for searching.
      * @param sample The sample object.
      * @param range The range of the results.
      * @param result The result object to add the pairs <object, distance> found
      * @param page The page (node) to search in.
      */
      void RangeQuery(tObject * sample, stDistance range,
                      tResult * result, stPageID page);

      /**
      * Support for the NearestQuery, recursive code for searching.
      * @param sample The sample object.
      * @param k The number of nearest neighbors to retrieve.
      * @param result The result object to add the pairs <object, distance> found
      * @param page The page (node) to search in.
      */
      void NearestQuery(tObject * sample, stCount k, tResult * result,
                        stPageID page);

};//end stVPTree

#include <arboretum/stVPTree.cc>

#endif //__STVPTREE_H


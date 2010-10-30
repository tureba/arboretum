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
#ifndef __STDUMMYTREE_H
#define __STDUMMYTREE_H

#include <arboretum/stCommon.h>
#include <arboretum/stTypes.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stDummyNode.h>


/**
* This class template implements a Dummy Tree. The Dummy Tree has the same
* external interface and behavior of a Metric Tree but is implemented as
* sequential list.
*
* <P>It can perform both range and k-nearest neighbout queries but without
* the performance associated with other structures. Its algorithms always
* have O(N) (except for insertion which is O(1)). In other words, it will
* take a lot of time to give answers but it will assure their correctness.
*
* <P>This class was developed to generate perfect answers to queries. It
* allows the build of automated test programs for other metric trees
* implemented by this library.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Finish the implementation.
* @todo Compute statistics.
* @version 1.0
* @ingroup dummy
*/
template <class ObjectType, class EvaluatorType>
class stDummyTree: public stMetricTree<ObjectType, EvaluatorType>{
   public:
      /**
      * This is the class that abstracts the object used by this metric tree.
      */
      typedef ObjectType tObject;

      /**
      * This is the class that abstracts the metric evaluator used by this metric
      * tree.
      */
      typedef EvaluatorType tMetricEvaluator;

      /**
      * This is the class that abstracs an result set.
      */
      typedef stResult <ObjectType> tResult;

      /**
      * Creates a new instance of the Dummy Tree.
      *
      * @param pageman The Page Manager to be used.
      */
      stDummyTree(stPageManager * pageman);

      /**
      * Disposes this tree and releases all associated resources.
      */
      virtual ~stDummyTree(){

         if (HeaderPage != NULL){
            // Release the header page.
            this->myPageManager->ReleasePage(HeaderPage);
         }//end if
      }//end ~stDummyTree

      /**
      * This method adds an object to the metric tree. This method may fail it the object size
      * exceeds the page size - 16.
      *
      * @param obj The object to be added.
      * @return True for success or false otherwise.
      */
      virtual bool Add(tObject * obj);

      /**
      * Returns the number of objetcs of this sequencial scan.
      */
      virtual long GetNumberOfObjects(){
         return Header->ObjectCount;
      }//end GetNumberOfObjects

      /**
      * Returns the number of nodes of this tree.
      */
      virtual long GetNodeCount(){
         return Header->NodeCount;
      }//end GetNodeCount

      /**
      * Returns the MaxOccupation of the nodes.
      */
      virtual stCount GetMaxOccupation(){
         return Header->MaxOccupation;
      }//end GetMaxOccupation

      /**
      * Returns the MaxOccupation of the nodes.
      */
      void SetMaxOccupation(stCount newValue){
         if (newValue > Header->MaxOccupation){
            Header->MaxOccupation = newValue;
         }//end if
      }//end SetMaxOccupation

      /**
      * This method will perform a range query. The result will be a set of
      * pairs object/distance.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * RangeQuery(tObject * sample, stDistance range);

      /**
      * This method will perform a reverse range query.
      * The result will be a set of pairs object/distance.
      *
      * @param sample The sample object.
      * @param range The range of the results. All object that are
      *  greater than the range distance will be included in the result set.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * ReversedRangeQuery(tObject * sample, stDistance range);

      /**
      * This method will perform a k nearest neighbor query.
      *
      * @param sample The sample object.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * NearestQuery(tObject * sample, stCount k, bool tie = false);

      /**
      * This method will perform a K-Farthest neighbor query.
      *
      * @param sample The sample object.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * FarthestQuery(tObject * sample, stCount k, bool tie = false);

      /**
      * This method will perform a range query with a limited number of results.
      *
      * <P>This query is a combination of the standard range query and the standard
      * k-nearest neighbour query. All objects which matches both conditions
      * will be included in the result.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * KAndRangeQuery(tObject * sample, stDistance range,
            stCount k, bool tie=false);

      /**
      * This method will perform range query with a limited number of results.
      *
      * <P>This query is a combination of the standard range query and the standard
      * k-nearest neighbour query. All objects which matches with one of two conditions
      * will be included in the result.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      virtual tResult * KOrRangeQuery(tObject * sample, stDistance range,
            stCount k, bool tie=false);

      /**
      * This method will perform a ring query. The result will be a set of
      * pairs object/distance.
      *
      * <P>The object pointed by <b>sample</b> will not be destroyed by this
      * method.
      *
      * @param sample The sample object.
      * @param inRange The inner range of the results.
      * @param outRange The outter range of the results.
      * @return The result.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The inRange must be less than the outRange.
      * @exception unsupported_method_error If this method is not supported by this tree.
      */
      virtual tResult * RingQuery(tObject * sample, stDistance inRange,
            stDistance outRange);

      /**
      * This method will perform a range query. The result will be a set of
      * pairs object/distance.
      *
      * @param samplelist The list of sample objects.
      * @param samplesize The number of samples of the param samplelist.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tResult * SumRangeQuery(tObject ** samplelist, stSize samplesize, stDistance range);

      /**
      * This method will perform a k nearest neighbor query.
      *
      * @param samplelist The list of sample objects.
      * @param samplesize The number of samples of the param samplelist.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      */
      tResult * SumNearestQuery(tObject ** samplelist, stSize samplesize, stCount k, bool tie = false);

      /**
      * This method will perform a k nearest neighbor joined query.
      *
      * @param joinedtree The tree being joined.
      * @param k The number of neighbours.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tResult * NearestJoinQuery(stDummyTree * joinedtree, stCount k, bool tie = false);

      /**
      * This method will perform a range joined query.
      *
      * @param joinedtree The tree being joined.
      * @param range The range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @autor Implemented by Humberto Razente
      */
      tResult * RangeJoinQuery(stDummyTree * joinedtree, stDistance range);

   private:
      /**
      * This type defines the header of the Dummy Tree.
      */
      typedef struct DummyTreeHeader{
         /**
         * The root.
         */
         stPageID Root;

         /**
         * The number of the objects in this page.
         */
         stSize ObjectCount;

         /**
         * Total number of nodes.
         */
         stSize NodeCount;

         /**
         * Maximum number of objects in a node.
         */
         stCount MaxOccupation;
      } stDummyTreeHeader;

      /**
      * The header page. It will be kept in memory all the time to avoid
      * reads.
      */
      stPage * HeaderPage;

      /**
      * The header of the "tree".
      */
      stDummyTreeHeader * Header;

      /**
      * If true, the header must be written to the page manager.
      */
      bool HeaderUpdate;

      /**
      * Creates the header for an empty tree.
      */
      void Create(){

         LoadHeader();
         Header->Root = 0;
         Header->ObjectCount = 0;
         Header->NodeCount = 0;
         Header->MaxOccupation = 0;
         HeaderUpdate = true;
         WriteHeader();
      }//end Create

      /**
      * Loads the header from the page manager.
      */
      void LoadHeader(){
         HeaderPage = this->myPageManager->GetHeaderPage();
         Header = (stDummyTreeHeader *) HeaderPage->GetData();
      }//end LoadHeader

      /**
      * Writes the header into the Page Manager.
      */
      void WriteHeader(){
         if (HeaderUpdate){
            this->myPageManager->WriteHeaderPage(HeaderPage);
            HeaderUpdate = false;
         }//end if
      }//end WriteHeader

      /**
      * Updates the object counter.
      */
      void UpdateObjectCounter(int inc){
         Header->ObjectCount += inc;
         HeaderUpdate = true;
      }//end UpdateObjectCounter

};//end stDummyTree

#include <arboretum/stDummyTree.cc>
#endif //__STDUMMYTREE_H

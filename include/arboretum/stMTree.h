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
* This file defines the class stMTree.
*
* @version 1.0
* $Revision: 1.7 $                   
* $Date: 2005/03/13 19:32:48 $
* $Author: marcos $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2004 GBDI-ICMC-USP

#ifndef __STMTREE_H
#define __STMTREE_H

#include <arboretum/stUtil.h>
#include <arboretum/stTypes.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stMNode.h>
#include <arboretum/stPageManager.h>
#include <arboretum/stGenericPriorityQueue.h>

// this is used to set the initial size of the dynamic queue
#define STARTVALUEQUEUE 200
// this is used to set the increment size of the dynamic queue
#define INCREMENTVALUEQUEUE 200

#include <string.h>
#include <math.h>
#include <values.h>
#include <algorithm>


//=============================================================================
// Class template stMLogicNode
//-----------------------------------------------------------------------------
/**
* Thic class template represents a MTree logic node entry. It is used to
* hold an index node or a leaf node in a memory form which allows better way to
* manipulate entries. It also implements means to distribute its contents
* between 2 nodes.
*
* <P>Each entry of this node can hold all information required by both nodes at
* same time so, the users of this node must control if it holds an index node or
* a leaf node.
*
* @warning This node will take the ownership of all object instances added to this node.
* It means that the object instance will be destroyed by the destructor of this class.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation review.
* @ingroup slim
*/
template <class ObjectType, class EvaluatorType>
class stMLogicNode{
   public:
      /**
      * Creates a new instance of this node with no objects.
      *
      * @param maxOccupation The maximum number of entries.
      */
      stMLogicNode(int maxOccupation);

      /**
      * Disposes this instance and releases all related resources. All instances of
      * object added to this node will also be deleted unless it is not owned by
      * this node (see method BuyObject()).
      */
      ~stMLogicNode();

      /**
      * Adds an object to this node. This method will claim the ownership
      * of the object instance.
      *
      * <P>Use SetEntry() to fill the other fields of each entry.
      *
      * @param obj The object to be added.
      * @return The entry id or -1 for error.
      */
      int AddEntry(ObjectType * obj){

         Entries[Count].Object = obj;
         Entries[Count].Mine = true;
         Count ++;
         return Count - 1;
      }//end AddEntry

      /**
      * Adds an object to this node.
      *
      * <P>Use SetEntry() to fill the other fields of each entry.
      *
      * @param size The size of the object in bytes.
      * @param object The object data.
      * @return The entry id or -1 for error.
      */
      int AddEntry(stSize size, const stByte * object);

      /**
      * Returns the number of entries in this node.
      */
      int GetNumberOfEntries(){
         return Count;
      }//end GetNumberOfEntries

      /**
      * Returns the object of a given entry.
      *
      * @param idx The object index.
      */
      ObjectType * GetObject(int idx){
         return Entries[idx].Object;
      }//end GetObject

      /**
      * Returns the object of a given entry.
      *
      * @param idx The object index.
      */
      ObjectType * operator [](int idx){
         return Entries[idx].Object;
      }//end operator []

      /**
      * Returns the PageID of a given entry. Only index nodes will use this field.
      *
      * @param idx The object index.
      */
      stPageID GetPageID(int idx){
         return Entries[idx].PageID;
      }//end GetPageID

      /**
      * Returns the radius of the sub-tree of a given entry. Only index nodes
      * will use this field.
      *
      * @param idx The object index.
      */
      stDistance GetRadius(int idx){
         return Entries[idx].Radius;
      }//end GetRadius

      /**
      * Sets the data associated with a given entry.
      *
      * @param idx The object index.
      * @param pageID The pageID.
      * @param radius The radius of the sub-tree.
      */
      void SetEntry(int idx, stPageID pageID, stDistance radius){
         Entries[idx].PageID = pageID;
         Entries[idx].Radius = radius;
      }//end GetPageID

      /**
      * Adds all objects of an index node. It will also set the node type to
      * stMNode::INDEX.
      *
      * @param node The node.
      */
      void AddIndexNode(stMIndexNode * node);

      /**
      * Adds all objects of a leaf node. It will also set the node type to
      * stMNode::LEAF.
      *
      * @param node The node.
      */
      void AddLeafNode(stMLeafNode * node);

      /**
      * Returns the id of the representative object.
      *
      * @param id The representative ID. It may be 0 or 1.
      */
      int GetRepresentativeIndex(int idx){
         return RepIndex[idx];
      }//end GetRepresentativeIndex

      /**
      * Returns the representative object.
      *
      * @param id The representative ID. It may be 0 or 1.
      */
      ObjectType * GetRepresentative(int idx){
        return Entries[RepIndex[idx]].Object;
      }//end GetRepresentative

      /**
      * Sets both representatives ids. Use UpdateDistances() to update all
      * distances between objects and.
      *
      * @param rep0 Index of representative 0.
      * @param rep1 Index of representative 1.
      */
      void SetRepresentative(int rep0, int rep1){
         RepIndex[0] = rep0;
         RepIndex[1] = rep1;
      }//end SetRepresentative

      /**
      * Distribute objects between 2 index nodes using the current
      * representatives (see SetRepresentative()). Both nodes must be empty.
      *
      * @param node0 The first node.
      * @param rep0 Representative of node 0.
      * @param node1 The second node.
      * @param rep0 Representative of node 1.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      * @return The number of computed distances.
      */
      int Distribute(stMIndexNode * node0, ObjectType * & rep0,
            stMIndexNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator){
         int result;

         // Distribute...
         result = TestDistribution(node0, node1, metricEvaluator);

         // Representatives
         rep0 = BuyObject(GetRepresentativeIndex(0));
         rep1 = BuyObject(GetRepresentativeIndex(1));

         return result;
      }//end Distribute

      /**
      * Distribute objects between 2 leaf nodes using the current
      * representatives (see SetRepresentative()). Both nodes must be empty.
      *
      * @param node0 The first node.
      * @param rep0 Representative of node 0.
      * @param node1 The second node.
      * @param rep1 Representative of node 1.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      * @return The number of computed distances.
      */
      int Distribute(stMLeafNode * node0, ObjectType * & rep0,
            stMLeafNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator){
         int result;

         // Distribute...
         result = TestDistribution(node0, node1, metricEvaluator);

         // Representatives
         rep0 = BuyObject(GetRepresentativeIndex(0));
         rep1 = BuyObject(GetRepresentativeIndex(1));

         return result;
      }//end Distribute

      /**
      * Tests the distribution of objects between 2 index nodes using the current
      * representatives (see SetRepresentative()). Both nodes must be empty.
      *
      * @param node0 The first node.
      * @param node1 The second node.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      * @return The number of computed distances.
      * @warning Since it is just a test fnction, it will not inialize the
      * fields of the entries.
      */
      int TestDistribution(stMIndexNode * node0, stMIndexNode * node1,
            EvaluatorType * metricEvaluator);

      /**
      * Tests the distribution of objects between 2 leaf nodes using the current
      * representatives (see SetRepresentative()).  Both nodes must be empty.
      *
      * @param node0 The first node.
      * @param node1 The second node.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      * @return The number of computed distances.
      */
      int TestDistribution(stMLeafNode * node0, stMLeafNode * node1,
            EvaluatorType * metricEvaluator);

      /**
      * Set minimum occupation. This must be the minimum number of objects
      * in a page. This value must be at least 1.
      */
      void SetMinOccupation(int min){
         MinOccupation = min;
         // At least the nodes must store 2 objects.
         if ((MinOccupation > (MaxEntries/2)) || (MinOccupation == 0)){
            MinOccupation = 2;
         }//end if
      }//end SetMinOccupation

      /**
      * Returns the node type. It may assume the values stMNode::INDEX or
      * stMNode::LEAF.
      */
      stNodeType GetNodeType(){
         return NodeType;
      }//end GetNodeType

      /**
      * Sets the node type.
      */
      void SetNodeType(stNodeType type){
         NodeType = type;
      }//end SetNodeType

      /**
      * Verifies if a given object is a representative.
      *
      * @param idx Object index.
      * @return True if the object is a representative or false otherwise.
      */
      bool IsRepresentative(int idx){
         return ((idx == RepIndex[0]) || (idx == RepIndex[1]));
      }//end IsRepresentative

      /**
      * Gets the ownership of an object associated with a given entry.
      * This method will avoid the automatic destruction of the object
      * instance by the destructor of this node.
      *
      * <P>This method can be used to buy the representative objects to avoid
      * unnecessary replications.
      *
      * @param idx The object index.
      * @return The pointer to the object.
      */
      ObjectType * BuyObject(int idx){
         Entries[idx].Mine = false;
         return Entries[idx].Object;
      }//end BuyObject

   private:
      /**
      * This type represents a slim tree logic node entry.
      */
      struct stMLogicEntry{
         /**
         * Object.
         */
         ObjectType * Object;

         /**
         * ID of the page.
         */
         stPageID PageID;

         /**
         * Radius of the sub-tree.
         */
         stDistance Radius;

         /**
         * Owner flag.
         */
         bool Mine;

         /**
         * Distances between representatives.
         */
         stDistance Distance[2];

         /**
         * Node Map.
         */
         bool Mapped;
      };

      /**
      * Minimum occupation.
      */
      int MinOccupation;

      /**
      * Entries.
      */
      struct stMLogicEntry * Entries;

      /**
      * Maximum number of entries.
      */
      int MaxEntries;

      /**
      * Current number of entries.
      */
      int Count;

      /**
      * This vector holds the id of the representative objects.
      */
      int RepIndex[2];

      /**
      * Type of this node.
      */
      int NodeType;

      /**
      * Updates all distances between representatives and all objects in this
      * node. It returns the number of distances calculated.
      *
      * @param metricEvaluator The metric evaluator to be used.
      */
      int UpdateDistances(EvaluatorType * metricEvaluator);
};//end stMLogicNode



//=============================================================================
// Class template stMTree
//-----------------------------------------------------------------------------
/**
* This class defines all behavior of the MTree.
* Probably most of the atributes will be stored in the header page of the used
* stPageManager (stDiskPageManager or stMemoryPageManager).
*
* <P> First implementation is based on original code by Agma Traina, Bernard Seeger
* and Caetano Traina
* <P> Main modifications from original code are intent to turn it an object oriented
* compliant code.
*
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo More documentation.
* @version 1.0
* @ingroup slim
*/
template <class ObjectType, class EvaluatorType>
class stMTree: public stMetricTree< ObjectType, EvaluatorType>{
   public:

      /**
      * This structure defines the MTree header structure. This type was left
      * public to allow the creation of debug tools.
      */
      typedef struct tMHeader{
         /**
         * Magic number. This is a short string that must contains the magic
         * string "M__3". It may be used to validate the file (this feature
         * is not implemented yet).
         */
         char Magic[4];
      
         /**
         * Split method.
         */
         int SplitMethod;

         /**
         * Split strategy method (balanced or unbalanced).
         */
         int ChooseSplitStrategyMethod;

         /**
         * Choose method
         */
         int ChooseMethod;
      
         /**
         * The root of the M-tree
         */
         stPageID Root;
      
         /**
         * Minimum percentage of objects in a node.
         */
         double MinOccupation;

         /**
         * Maximum number of objects in a node.
         */
         stCount MaxOccupation;

         /**
         * The height of the tree
         */
         stCount Height;
      
         /**
         * Total number of records
         */
         stCount ObjectCount;
         
         /**
         * Total number of nodes.
         */
         stSize NodeCount;
      }stMHeader;   

      /**
      * These constants are used to define the choose subtree method.
      */
      enum tTBFunction{

         /**
         * Minimum radius increment.
         */
         MIN_R_INCR,

         /**
         * Minimum overlap.
         */
         MIN_OVERLAP,
         
         /**
         * ???
         */
         MIXED
      };//end tTBFunction;

      /**
      * Promotion method. Specifies the algorithm used to promote objects in
      * the parent node.
      */
      enum tPPFunction{
         /**
         * Random promotion.
         */
         RANDOM,

         /**
         * Confirmed promotion. Variable PROMOTE_VOTE_FUNCTION
         * is then used to choose between confirmed policies.
         */
         CONFIRMED,

         /**
         * Maximum upper bound on distances policy.
         * The two objects having the maximum distance from the parent
         * object are chosen.
         */
         MAX_UB_DIST,

         /**
         * Minimum maximum radius policy.
         */
         MIN_RAD,

         /**
         * Minimum overlap policy.
         */
         MIN_OVERLAPS,

         /**
         * Sampling promotion. Variable NUM_CANDIDADTES specifies
         * the number of samples.
         */
         SAMPLING,
      };//end tPPFunction;

      /**
      * Confirmed promotion method. Specifies the algorithm used to promote
      * one object as one of the two parents, the other being the parent object
      * of the split node. Is is used only if PROMOTE_PART_FUNCTION=CONFIRMED.
      */
      enum tPVFunction{

         /**
         * Random confirmed promotion.
         */
         RANDOMV,

         /**
         * Sampling confirmed promotion. Variable NUM_CANDIDATES specifies the
         * numer of samples.
         */
         SAMPLINGV,

         /**
         * Maximum lower bound on distances promotion. The object farthest
         * from the parent object is chosen.
         */
         MAX_LB_DIST,

         /**
         * Minimum radius confirmed policy, variable RADIUS_FUNCTION is then
         * used to choose between available policies.
         */
         mM_RAD
      };//end tPVFunction;

      /**
      * Minimum radius method.
      */
      enum tRFunction{

         /**
         * Minimum maximum lower bound on radius policy.
         */
         LB,

         /**
         * Minimum maximum average bound on radius policy.
         */
         AVG,

         /**
         * Minimum maximum upper bound on radius policy.
         */
         UB
      };//end tRFunction;

      /**
      * Split function. This specifies the way objects in the overfull
      * node are to be divided between the two new nodes.
      *
      * Both the BAL_G_HYPERPL and the BALANCED methods obtain perfectly
      * balanced nodes, whereas with the G_HYPERPL strategy the user can tune the
      * node utilization by modifying the value of the MIN_UTIL variable. We
      * suggest to use the G_HYPERPL strategy, since a balanced split tends to
      * produce high covering radii, and this negatively affects index performance
      * during the search. Having unbalanced nodes "only" increases the size of
      * the tree (number of nodes), but most of the nodes will be pruned when
      * searching the M-tree.
      */
      enum tSFunction{

         /**
         * The generalize hyperplane partition strategy.
         */
         G_HYPERPL,

         /**
         * The balanced hyperplane partition strategy.
         */
         BAL_G_HYPERPL,

         /**
         * The balanced strategy.
         */
         BALANCED
      };//end tSFunction;

      /**
      * This is the class that abstracs an result set.
      */
      typedef stResult <ObjectType> tResult;

      /**
      * Creates a new metric tree using a given page manager. This instance will
      * not claim the ownership of the given page manager. It means that the
      * application must dispose the page manager when it is no loger necessary.
      *
      * @param pageman The bage manager to be used by this metric tree.
      */
      stMTree(stPageManager * pageman);

      /**
      * Dispose all used resources, ie, it is the destructor method.
      *
      * @see stMTree()
      */
      virtual ~stMTree();

      /**
      * This method adds an object to the metric tree.
      *
      * @param obj The object to be added.
      */
      virtual bool Add(ObjectType * newObj);

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
      * Returns the MinOccupation of the nodes.
      */
      virtual double GetMinOccupation(){
         return Header->MinOccupation;
      }//end GetMinOccupation

      /**
      * Set the MinOccupation of the nodes.
      */
      virtual void SetMinOccupation(double min){
         Header->MinOccupation = min;
      }//end SetMinOccupation

      /**
      * Returns the number of nodes of this tree.
      */
      virtual long GetNodeCount(){
         return Header->NodeCount;
      }//end GetNodeCount

      /**
      * Returns the limit distance between 2 objects in the tree. That is
      * \f$ \forall a,b \in D, d(a,b) \le GetDistanceLimit()\f$. In other
      * words, there is no distance greater than this.
      *
      * <P> This value may be the greatest distance between objects but may
      * return greater values due to implementation optmizations.
      */
      stDistance GetDistanceLimit();

      // Tree Configuration
      /**
      * Sets the tree split method.
      *
      * @param method Method name.
      * @see tSplitMethod
      */
      void SetSplitMethod(enum tPPFunction method){
         Header->SplitMethod = method;
         this->HeaderUpdate = true;
      }//end SetSplitMethod

      /**
      * Returns the current split method.
      */
      int GetSplitMethod(){
         return Header->SplitMethod;
      }//end GetSplitMethod

      /**
      * Sets the Choose Method name.
      *
      * @param method Choose method name.
      * @see tChooseMethod
      */
      void SetChooseMethod(enum tTBFunction method){
         Header->ChooseMethod = method;
         this->HeaderUpdate = true;
      }//end SetChooseMethod

      /**
      * Returns the current split method.
      */
      int GetSplitStrategyMethod(){
         return Header->ChooseSplitStrategyMethod;
      }//end GetSplitStrategyMethod

      /**
      * Sets the Choose Method name.
      *
      * @param method Choose method name.
      * @see tChooseMethod
      */
      void SetSplitStrategyMethod(enum tSFunction method){
         Header->ChooseSplitStrategyMethod = method;
         this->HeaderUpdate = true;
      }//end SetSplitStrategyMethod

      /**
      * Returns the Choose Method name.
      */
      int GetChooseMethod(){
         return Header->ChooseMethod;
      }//end GetChooseMethod

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
      * Gets the maximum user data size available in this tree.
      *
      * @see WriteUserData()
      * @see ReadUserData()
      * @bug This feature is not implemented yet.
      */
      stSize GetUserDataSize(){
         return 0;
      }//end GetUserDataSize

      /**
      * Writes an additional user data.
      *
      * <P>This feature allows users to write additional information in to the
      * free area of the header page. The available space will deppends on the
      * size of the header page (see the page manager documentation for more
      * details).
      *
      * @param userData A pointer to the user data.
      * @param size The size of the user data.
      * @return True for success or false if the user data doesn't fit in this
      * area.
      * @see GetUserDataSize()
      * @see ReadUserData()
      * @bug This feature is not implemented yet.
      */
      bool WriteUserData(const stByte * userData, stSize size){
         return false;
      }//end WriteUserData

      /**
      * Reads an additional user data.
      *
      * <P>This feature allows users to write additional information in to the
      * free area of the header page. The available space will deppends on the
      * size of the header page (see the page manager documentation for more
      * details).
      *
      * @param userData A pointer to the user data.
      * @param size The size of the user data.
      * @return True for success or false if the user data doesn't fit in this
      * area.
      * @see GetUserDataSize()
      * @see WriteUserData()
      * @bug This feature is not implemented yet.
      */
      bool ReadUserData(stByte * userData, stSize size){
         return false;
      }//end ReadUserData

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
      tResult * RangeQuery(ObjectType * sample, stDistance range);

      /**
      * This method will perform a K-Nearest Neighbor query using a global priority
      * queue based on chained list to "enhance" its performance. We believe that the
      * use of a priority queue during the search in the leaf nodes will force the
      * radius to converge faster than the normal nearest query.
      *
      * <p>This implementation may replace the original implementation if it proves
      * to be better.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void NearestQuery
      */
      tResult * NearestQuery(ObjectType * sample, stCount k, bool tie = false);

      /**
      * This method will return the object in the tree that has the distance 0
      * to the query object. In other words, the query object itself.
      *
      * <P>If there are more elements that has distance 0 to the query
      * object, this method will return the first found.
      *
      * <P>The object pointed by <b>sample</b> will not be destroyed by this
      * method.
      *
      * @param sample The sample object.
      * @return The result or NULL if this method is not implemented.
      * @warning This method return only one object that has distance 0 to the
      * query object.
      * @warning The instance of tResult returned must be destroied by user.
      */
      tResult * PointQuery(ObjectType * sample);

      /**
      * This type is used by the priority key.
      */
      typedef stGenericPriorityQueue < ObjectType > tGenericPriorityQueue;

      /**
      * This type is used by the priority key.
      */
      typedef stGenericEntry < ObjectType > tGenericEntry;

      /**
      * This type is used by the priority key.
      */
      typedef stGenericPriorityHeap < ObjectType > tPGenericHeap;

      /**
      * @copydoc stMetricTree::GetTreeInfo()
      */
      virtual stTreeInfoResult * GetTreeInfo();

   private:

      /**
      * This type defines the logic node for this class.
      */
      typedef stMLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type is used by the priority key.
      */
      typedef stRPriorityQueue < stDistance, stCount > tPriorityQueue;

      /**
      * This type is used by the priority key in IncrementalNearest.
      */
      typedef stDynamicRPriorityQueue < stDistance, stQueryPriorityQueueValue > tDynamicPriorityQueue;

      /**
      * This enumeration defines the actions to be taken after an call of
      * InsertRecursive.
      */
      enum stInsertAction{
         /**
         * No action required. Just update the radius.
         */
         NO_ACT,

         /**
         * Replace representative.
         */
         CHANGE_REP,

         /**
         * Split occured. Update subtrees.
         */
         PROMOTION
      };//end stInsertAction

      /**
      * This structure holds a promotion data. It contains the representative
      * object, the ID of the root, the Radius and the number of objects of the subtree.
      */
      struct stSubtreeInfo{
         /**
         * The representative object.
         */
         ObjectType * Rep;

         /**
         * The radius of the subtree.
         */
         stDistance Radius;

         /**
         * The ID root of the root of the subtree.
         */
         stPageID RootID;
      };

      /**
      * If true, the header mus be written to the page manager.
      */
      bool HeaderUpdate;

      /**
      * The MTree header. This variable points to data in the HeaderPage.
      */
      stMHeader * Header;

      /**
      * Pointer to the header page.
      * The M Tree keeps this page in memory for faster access.
      */
      stPage * HeaderPage;

      /**
      * Sets all header's fields to default values.
      *
      * @warning This method will destroy the tree.
      */
      void DefaultHeader();

      /**
      * Loads the header page and set the Header pointer. The previous header
      * page, if exists, will be lost.
      *
      * @exception page_size_error If the page size is too small.
      */
      void LoadHeader();

      /**
      * Updates the header in the file if required.
      */
      void WriteHeader(){
         // Test if the header was modified.
         if (this->HeaderUpdate){
            // Yes, write it!
            myPageManager->WriteHeaderPage(HeaderPage);
            this->HeaderUpdate = false;
         }//end if
      }//end WriteHeader

      /**
      * Disposes the header page if it exists. It also updates its contents
      * before destroy it.
      *
      * <P>This method is called by the destructor.
      */
      void FlushHeader();
      
      /**
      * Creates a new empty page and updates the node counter.
      */
      stPage * NewPage(){
         Header->NodeCount++;
         return this->myPageManager->GetNewPage();
      }//end NewPage
      
      /**
      * Disposes a given page and updates the page counter.
      */
      void DisposePage(stPage * page){
         Header->NodeCount--;
         this->myPageManager->DisposePage(page);
      }//end DisposePage

      /**
      * This method computes an index of an entry where the insertion process
      * of record obj should continue.
      *
      * @param slimIndexNode the indexNode to be analyzed
      * @param obj The object that will be inserted.
      * @return the minIndex the index of the choose of the subTree
      */
      int ChooseSubTree(stMIndexNode * slimIndexNode, ObjectType * obj);

      /**
      * Compute two elements from the page and use them for being the center
      * of the index entries in the parent page of the MTree.
      * This is the most simple and inexpensive method for promoting entries
      *
      * @param node The node.
      */
      void RandomPromote(tLogicNode * node);

      /**
      * This is just another strategy of promoting entries of a page p.
      * See the original paper.
      *
      * @param node The node.
      */
      void MinMaxPromote(tLogicNode * node);

      /**
      * This method find a new center for the objects in the node P.
      * It works by finding the objects that minimizes the covering circle.
      * Case the node is a leafNode, the new radius is the new radius plus the radius
      * of the subTree that has all the objects in the all subTrees;
      *
      * @param *node node to compute
      * @param *newCenter the index of the new center
      * @param *newRadius the new radius of this node
      */
      void ReDistribute(stMNode * node, int & newCenter, stDistance & newRadius);

      /**
      * Checks to see it a given node ID is the current root.
      *
      * @return True it the given nodeID is the root or false otherwise.
      */
      bool IsRoot(stPageID nodeID){
         return nodeID == this->Header->Root;
      }//end IsRoot

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
         this->HeaderUpdate = true;
      }//end SetRoot

      /**
      * Updates the object counter.
      */
      void UpdateObjectCounter(int inc){
         Header->ObjectCount += inc;
         this->HeaderUpdate = true;
      }//end UpdateObjectCounter

      /**
      * This method inserts an object in to the tree recursively.
      * This method is the core of the insertion method. It will manage
      * promotions (splits) and representative changes.
      *
      * <P>For each action, the returning values may assume the following
      * configurations:
      *     - NO_ACT:
      *           - promo1.Radius will have the new subtree radius.
      *           - Other parameters will not be used.
      *     - CHANGE_REP:
      *           - promo1 will contains the information about the changes
      *             in the subtree.
      *           - Other parameters will not be used.
      *     - PROMOTION:
      *           - promo1 will contain the information about the choosen subtree.
      *                 - If promo1.Rep is NULL, the representative of the
      *                   subtree will not change.
      *           - promo2 will contain the information about the promoted subtree.
      *
      * @param currNodeID Current node ID.
      * @param newObj The new object to be inserted. This instance will never
      * be destroyed.
      * @param repObj The representative object for this node. This instance
      * will never be destroyed.
      * @param promo1 Information about the choosen subtree (returning value).
      * @param promo2 Infromation about the promoted subtree (returning value).
      * @return The action to be taken after the returning. See enum
      * stInsertAction for more details.
      */
      int InsertRecursive(stPageID currNodeID, ObjectType * newObj,
            ObjectType * repObj, stSubtreeInfo & promo1, stSubtreeInfo & promo2);

      /**
      * Creates and updates the new root of the MTree.
      *
      * @param obj1 Object 1.
      * @param radius1 Radius of subtree 1.
      * @param nodeID1 ID of the root page of the sub-tree 1.
      * @param obj2 Object 2.
      * @param radius2 Radius of subtree 2.
      * @param nodeID2 ID of the root page of the sub-tree 2.
      */
      void AddNewRoot(ObjectType * obj1, stDistance radius1, stPageID nodeID1,
                      ObjectType * obj2, stDistance radius2, stPageID nodeID2);

      /**
      * This method splits a leaf node in 2. This will get 2 nodes and will
      * redistribute the object set between these.
      *
      * <P>The split method will be defined by the current tree configuration.
      *
      * @param oldNode The node to be splited.
      * @param newNode The new node.
      * @param newObj The new object to be added. This instance will be consumed
      * by this method.
      * @param prevRep The previous representative.
      * @param promo1 The promoted subtree. If its representative is NULL,
      * the choosen representative is equal to prevRep.
      * @param promo2 The promoted subtree. The representative of this tree will
      * never be the prevRep.
      */
      void SplitLeaf(stMLeafNode * oldNode, stMLeafNode * newNode,
            ObjectType * newObj, ObjectType * prevRep,
            stSubtreeInfo & promo1, stSubtreeInfo & promo2);

      /**
      * This method splits an index node in 2. This will get 2 nodes and will
      * redistribute the object set between these.
      *
      * <P>This method may takes one or two new objects to be added to the node
      * before the split.
      *
      * <P>The split method will be defined by the current tree configuration.
      *
      * @param oldNode The node to be splited.
      * @param newNode The new node.
      * @param newObj1 The new object 1 to be added. This instance will be consumed
      * by this method.
      * @param newRadius1 The new object 1 radius.
      * @param newNodeID1 The new object 1 node ID.
      * @param newObj2 The new object 2 to be added or NULL.
      * @param newRadius2 The new object 2 radius if newObj2 is not NULL.
      * @param newNodeID2 The new object 2 node ID  if newObj2 is not NULL..
      * not NULL.
      * @param prevRep The previous representative.
      * @param promo1 The promoted subtree. If its representative is NULL,
      * the choosen representative is equal to prevRep.
      * @param promo2 The promoted subtree. The representative of this tree will
      * never be the prevRep.
      */
      void SplitIndex(stMIndexNode * oldNode, stMIndexNode * newNode,
            ObjectType * newObj1, stDistance newRadius1, stPageID newNodeID1,
            ObjectType * newObj2, stDistance newRadius2, stPageID newNodeID2, 
            ObjectType * prevRep,
            stSubtreeInfo & promo1, stSubtreeInfo & promo2);

      /**
      * This method will perform a range query.
      * The result will be a set of pairs object/distance.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the result.
      * @param distanceRepres The distance of the representative.
      * @see tResult * RangeQuery()
      */
      void RangeQuery(stPageID pageID, tResult * result,
            ObjectType * sample, stDistance range,
            stDistance distanceRepres);

      /**
      * This method will perform a K Nearest Neighbor query using a priority
      * queue.
      *
      * @param result the result set.
      * @param sample The sample object.
      * @param rangeK The range of the results.
      * @param k The number of neighbours.
      * @see tResult * NearestQuery
      */
      void NearestQuery(tResult * result, ObjectType * sample,
                        stDistance rangeK, stCount k);

      /**
      * This method will return the object in the tree that has the distance 0
      * to the query object. In other words, the query object itself.
      *
      * <P>If there are more elements that has distance 0 to the query
      * object, this method will return the first found.
      *
      * <P>The object pointed by <b>sample</b> will not be destroyed by this
      * method.
      *
      * @param sample The sample object.
      * @return The result or NULL if this method is not implemented.
      * @warning This method return only one object that has distance 0 to the
      * query object.
      * @see tResult * PointQuery
      */
      void PointQuery(tResult * result, ObjectType * sample);

      /**
      * Updates the distances of the objects from the new representative.
      */
      void UpdateDistances(stMIndexNode *node, ObjectType * repObj, int repObjIdx);

      /**
      * This method travels through the tree gathering information about
      * each level.
      *
      * @param pageID Root of the subtree.
      * @param level Current level (zero for the first call).
      * @param info Tree information.
      */
      void GetTreeInfoRecursive(stPageID pageID, int level,
            stTreeInformation * info);

      /**
      * This method travels through the tree gathering information about the
      * intersections of a given object.
      *
      * @param pageID Root of this sub-tree.
      * @param obj Object.
      * @param level Current level.
      * @param info Tree information.
      */
      void ObjectIntersectionsRecursive(stPageID pageID, ObjectType * obj,
            int level, stTreeInformation * info);
            
};//end stMTree

// Include implementation
#include "stMTree.cc"

#endif //__STMTREE_H

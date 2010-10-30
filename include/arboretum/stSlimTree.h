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
* This file defines the class stSlimTree.
*
* @version 1.0
* $Revision: 1.126 $
* $Date: 2005/03/13 19:32:22 $
* $Author: marcos $
* @author Fabio Jun Takada Chino(chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
*/
// Copyright (c) 2003 GBDI-ICMC-USP

#ifndef __STSLIMTREE_H
#define __STSLIMTREE_H

#include <arboretum/stUtil.h>
#include <arboretum/stTypes.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stSlimNode.h>
#include <arboretum/stPageManager.h>
#include <arboretum/stGenericPriorityQueue.h>

// this is used to set the initial size of the dynamic queue
#define STARTVALUEQUEUE 200
// this is used to set the increment size of the dynamic queue
#define INCREMENTVALUEQUEUE 200

//#include <string.h>
//#include <math.h>
//#include <values.h>
//#include <algorithm>
#include <arboretum/stMAMView.h> // Visualization support

//=============================================================================
// Class template stSlimLogicNode
//-----------------------------------------------------------------------------
/**
* Thic class template represents a SlimTree logic node entry. It is used to
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
* @author Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
* @todo Documentation review.
* @todo Tests!
* @ingroup slim
*/
template <class ObjectType, class EvaluatorType>
class stSlimLogicNode{
   public:
      /**
      * Creates a new instance of this node with no objects.
      *
      * @param maxOccupation The maximum number of entries.
      */
      stSlimLogicNode(stCount maxOccupation);

      /**
      * Disposes this instance and releases all related resources. All instances of
      * object added to this node will also be deleted unless it is not owned by
      * this node (see method BuyObject()).
      */
      ~stSlimLogicNode();

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
         Count++;
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
      stCount GetNumberOfEntries(){
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
      * Returns the number of entries in the sub-tree of a given entry. Only index nodes will use this field.
      *
      * @param idx The object index.
      */
      stSize GetNEntries(int idx){
         return Entries[idx].NEntries;
      }//end GetNEntries

      /**
      * Returns the radius of the sub-tree of a given entry. Only index nodes will use this field.
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
      * @param nEntries The number of entries in the sub-tree.
      * @param radius The radius of the sub-tree.
      */
      void SetEntry(int idx, stPageID pageID, stSize nEntries, stDistance radius){
         Entries[idx].PageID = pageID;
         Entries[idx].NEntries = nEntries;
         Entries[idx].Radius = radius;
      }//end GetPageID

      /**
      * Adds all objects of an index node. It will also set the node type to
      * stSlimNode::INDEX.
      *
      * @param node The node.
      */
      void AddIndexNode(stSlimIndexNode * node);

      /**
      * Adds all objects of a leaf node. It will also set the node type to
      * stSlimNode::LEAF.
      *
      * @param node The node.
      */
      void AddLeafNode(stSlimLeafNode * node);

      /**
      * Returns the idx of the representative object.
      *
      * @param idx The representative ID. It may be 0 or 1.
      */
      stCount GetRepresentativeIndex(stCount idx){
         return RepIndex[idx];
      }//end GetRepresentativeIndex

      /**
      * Returns the representative object.
      *
      * @param idx The representative ID. It may be 0 or 1.
      */
      ObjectType * GetRepresentative(stCount idx){
        return Entries[RepIndex[idx]].Object;
      }//end GetRepresentative

      /**
      * Sets both representatives ids. Use UpdateDistances() to update all
      * distances between objects and.
      *
      * @param rep0 Index of representative 0.
      * @param rep1 Index of representative 1.
      */
      void SetRepresentative(stCount rep0, stCount rep1){
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
      stCount Distribute(stSlimIndexNode * node0, ObjectType * & rep0,
                         stSlimIndexNode * node1, ObjectType * & rep1,
                         EvaluatorType * metricEvaluator){
         stCount result;

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
      stCount Distribute(stSlimLeafNode * node0, ObjectType * & rep0,
                         stSlimLeafNode * node1, ObjectType * & rep1,
                         EvaluatorType * metricEvaluator){
         stCount result;

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
      stCount TestDistribution(stSlimIndexNode * node0, stSlimIndexNode * node1,
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
      stCount TestDistribution(stSlimLeafNode * node0, stSlimLeafNode * node1,
                               EvaluatorType * metricEvaluator);

      /**
      * Set minimum occupation. This must be the minimum number of objects
      * in a page. This value must be at least 1.
      */
      void SetMinOccupation(stCount min){
         MinOccupation = min;
         // At least the nodes must store 2 objects.
         if ((MinOccupation > (MaxEntries/2)) || (MinOccupation == 0)){
            MinOccupation = 2;
         }//end if
      }//end SetMinOccupation

      /**
      * Returns the node type. It may assume the values stSlimNode::INDEX or
      * stSlimNode::LEAF.
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
      bool IsRepresentative(stCount idx){
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
      ObjectType * BuyObject(stCount idx){
         Entries[idx].Mine = false;
         return Entries[idx].Object;
      }//end BuyObject

   private:
      /**
      * This type represents a slim tree logic node entry.
      */
      struct stSlimLogicEntry{
         /**
         * Object.
         */
         ObjectType * Object;

         /**
         * ID of the page.
         */
         stPageID PageID;

         /**
         * Number of entries in the sub-tree.
         */
         stCount NEntries;

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
      stCount MinOccupation;

      /**
      * Entries.
      */
      struct stSlimLogicEntry * Entries;

      /**
      * Maximum number of entries.
      */
      stCount MaxEntries;

      /**
      * Current number of entries.
      */
      stCount Count;

      /**
      * This vector holds the id of the representative objects.
      */
      stCount RepIndex[2];

      /**
      * Type of this node.
      */
      stNodeType NodeType;

      /**
      * Updates all distances between representatives and all objects in this
      * node. It returns the number of distances calculated.
      *
      * @param metricEvaluator The metric evaluator to be used.
      */
      stCount UpdateDistances(EvaluatorType * metricEvaluator);

};//end stSlimLogicNode

//=============================================================================
// Class template stSlimMSTSpliter
//-----------------------------------------------------------------------------
/**
* This class template implements the SlimTree MST split algorithm.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
* @todo Documentation review.
* @todo Tests!
* @ingroup slim
*/
template <class ObjectType, class EvaluatorType>
class stSlimMSTSplitter{
   public:
      /**
      * This type defines the logic node for this class.
      */
      typedef stSlimLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * Builds a new instance of this class. It will claim the ownership of the
      * logic node provided as input.
      */
      stSlimMSTSplitter(tLogicNode * node);

      /**
      * Disposes all associated resources.
      */
      ~stSlimMSTSplitter();

      /**
      * Provides access to the logic node.
      */
      const tLogicNode * GetLogicNode(){
         return Node;
      }//end GetLogicNode

      /**
      * Distributes objects between 2 index nodes.
      *
      */
      int Distribute(stSlimIndexNode * node0, ObjectType * & rep0,
                     stSlimIndexNode * node1, ObjectType * & rep1,
                     EvaluatorType * metricEvaluator);

      /**
      * Distributes objects between 2 leaf nodes.
      */
      int Distribute(stSlimLeafNode * node0, ObjectType * & rep0,
                     stSlimLeafNode * node1, ObjectType * & rep1,
                     EvaluatorType * metricEvaluator);

   protected:
      /**
      * Distance matrix type.
      */
      typedef stGenericMatrix <stDistance> tDistanceMatrix;

      /**
      * Cluster states.
      */
      enum tClusterState{
         ALIVE,
         DEAD,
         DEATH_SENTENCE
      };//end tClusterState

      /**
      * Cluster information type.
      */
      struct tCluster{
         /**
         * If this cluster exists.
         */
         enum tClusterState State;

         /**
         * Number of objects in this cluster.
         */
         int Size;

         /**
         * Minimum distance to the nearest cluster.
         */
         stDistance MinDist;

         /**
         * The object of this cluster that defines the minimum distance.
         */
         int Src;

         /**
         * The object of the nearest cluster that defines the minimum distance.
         */
         int Dst;
      };

      /**
      * The logic node to be used as source.
      */
      tLogicNode * Node;

      /**
      * The distance matrix.
      */
      tDistanceMatrix DMat;

      /**
      * All clusters.
      */
      struct tCluster * Cluster;

      /**
      * The names of the cluster of each object
      */
      int * ObjectCluster;

      /**
      * Total number of objects.
      */
      int N;

      /**
      * Name of the cluster 0.
      */
      int Cluster0;

      /**
      * Name of the cluster 1.
      */
      int Cluster1;

      /**
      * Returns the center of the object in the cluster clus.
      * The.
      *
      * @param clus Cluster id.
      */
      int FindCenter(int clus);

      /**
      * Builds the distance matrix using the given metric evaluator.
      *
      * @param metricEvaluator The metric evaluator.
      * @return The number of computed distances.
      */
      int BuildDistanceMatrix(EvaluatorType * metricEvaluator);

      /**
      * Performs the MST algorithm. This method will split the objects in 2
      * clusters. The result of the processing will be found at the array
      * Cluster.
      *
      * @warning DMat must be initialized.
      */
      void PerformMST();

      /**
      * Joins 2 clusters. This method will insert custer2 into cluster1.
      *
      * <P>The state of cluster2 will change to DEATH_SENTENCE.
      *
      * @param cluster1 Cluster 1.
      * @param cluster2 Cluster 2.
      */
      void JoinClusters(int cluster1, int cluster2);

};//end stSlimMSTSplitter

//=============================================================================
// Class template stSlimTree
//-----------------------------------------------------------------------------
/**
* This class defines all behavior of the SlimTree.
* Probably most of the atributes will be stored in the header page of the used
* stPageManager (stDiskPageManager or stMemoryPageManager).
*
* <P> First implementation is based on original code by Agma Traina, Bernard Seeger
* and Caetano Traina
* <P> Main modifications from original code are intent to turn it an object oriented
* compliant code.
*
* @author Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.sc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.sc.usp.br)
* @todo More documentation.
* @version 1.0
* @ingroup slim
*/
template <class ObjectType, class EvaluatorType>
class stSlimTree: public stMetricTree <ObjectType, EvaluatorType> {
   public:

      /**
      * This structure defines the SlimTree header structure. This type was left
      * public to allow the creation of debug tools.
      */
      typedef struct tSlimHeader{
         /**
         * Magic number. This is a short string that must contains the magic
         * string "SLIM". It may be used to validate the file (this feature
         * is not implemented yet).
         */
         char Magic[4];

         /**
         * Split method.
         */
         int SplitMethod;

         /**
         * Choose method
         */
         int ChooseMethod;

         /**
         * Correct method.
         */
         int CorrectMethod;

         /**
         * The root of the Slim-tree
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
         * Total number of objects.
         */
         stCount ObjectCount;

         /**
         * Total number of nodes.
         */
         stSize NodeCount;
      }stSlimHeader;

      /**
      * These constants are used to define the choose sub tree method.
      */
      enum tChooseMethod{
         /**
         * Choose the first of the qualifying nodes.
         */
         cmBIASED,
         /**
         * Randomly choose one of the qualifying nodes.
         */
         cmRANDOM,
         /**
         * Choose the node that has the minimum distance from the
         * new object and the representative (center) of the node.
         */
         cmMINDIST,

         /**
         * Choose the node that has the minimum occupancy among the
         * qualifying ones. This is the default method due to its better
         * performance.
         */
         cmMINOCCUPANCY,
         /**
         * Unknown.
         * @todo Discover what this method do.
         */
         cmMINGDIST
      };//end tChooseMethod

      /**
      * These constants are used to define the correction method.
      */
      enum tCorrectMethod {
         /**
         * No correction.
         */
         crmOFF,
         /**
         * Use Fat Factor.
         */
         crmFATFACTOR
      };//end tCorrectMethod

      /**
      * These constants are used to define the split method.
      * @todo Update documentation of each constant.
      */
      enum tSplitMethod {
         /**
         * This method peeks 2 random objects as the representatives and
         * distribute the other objects around them.
         *
         * <p>This is the fastest split method but the result is undetermined.
         */
         smRANDOM,

         /**
         * The optimal split method. This algorithm tries all possible
         * distribution configurations and select the optimal distribution as
         * the final result.
         *
         * <p>This is the default method for the M-Tree.
         *
         * @warning This method is very slow.
         */
         smMINMAX,

         /**
         * Split method based on the Minimal Spanning Tree algorithm.
         * This is the Slim-Tree default split method.
         */
         smSPANNINGTREE
      };//end tSplitMethod

      /**
      * This is the class that abstracs an result set.
      */
      typedef stResult <ObjectType> tResult;

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
      * Memory leaf node used by Slim-Down.
      */
      typedef stSlimMemLeafNode < ObjectType > tMemLeafNode;

      /**
      * Creates a new metric tree using a given page manager. This instance will
      * not claim the ownership of the given page manager. It means that the
      * application must dispose the page manager when it is no loger necessary.
      *
      * @param pageman The bage manager to be used by this metric tree.
      */
      stSlimTree(stPageManager * pageman);

      /**
      * Creates a new metric tree using a given page manager. This instance will
      * not claim the ownership of the given page manager. It means that the
      * application must dispose the page manager when it is no loger necessary.
      *
      * @param pageman The bage manager to be used by this metric tree.
      * @param metricEval The shared metric evaluator be used by this metric tree.
      * @warning metricEval will not be delete by this class.
      */
      stSlimTree(stPageManager * pageman, EvaluatorType * metricEval);

      /**
      * Dispose all used resources, ie, it is the destructor method.
      *
      * @see stSlimTree()
      */
      virtual ~stSlimTree();

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

      long GetIndexNodeCount();

      long GetIndexNodeCount(stPageID pageID);

      /**
      * Returns the limit distance between 2 objects in the tree. That is
      * \f$ \forall a,b \in D, d(a,b) \le GetDistanceLimit()\f$. In other
      * words, there is no distance greater than this.
      *
      * <P> This value may be the greatest distance between objects but may
      * return greater values due to implementation optmizations.
      */
      stDistance GetDistanceLimit();

      /**
      * Returns the greatest estimated distance between 2 objects in the second
      * level of the tree.
      *
      * @return the greatest estimated distance.
      */
      stDistance GetGreaterEstimatedDistance();

      /**
      * Returns the greatest distance between 2 objects in the leaf
      * level of the tree.
      *
      * @return the greatest distance.
      */
      stDistance GetGreaterDistance();

      // Tree Configuration
      /**
      * Sets the tree split method.
      *
      * @param method Method name.
      * @see tSplitMethod
      */
      void SetSplitMethod(enum tSplitMethod method){
         Header->SplitMethod = method;
         HeaderUpdate = true;
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
      void SetChooseMethod(enum tChooseMethod method){
         Header->ChooseMethod = method;
         HeaderUpdate = true;
      }//end SetChooseMethod

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
      * This method will perform a reverse of range query.
      * The result will be a set of pairs object/distance.
      *
      * @param sample The sample object.
      * @param range The range of the results. All object that are
      * greater than the range distance will be included in the result set.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void RangeQuery()
      */
      tResult * ReversedRangeQuery(ObjectType * sample, stDistance range);

      /**
      * This method will perform a K-Nearest Neighbor query using a priority
      * queue to "enhance" its performance. We believe that the use of a priority
      * queue during the search in the index nodes will force the radius to converge
      * faster than the normal nearest query.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void NearestQuery
      */
      tResult * LocalNearestQuery(ObjectType * sample, stCount k, bool tie = false);

      /**
      * This method will perform a K-Nearest Neighbor query using a link list.
      * We believe that the use of a priority queue during the search in the
      * index nodes will force the radius to converge faster than the normal
      * nearest query.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void NearestQuery
      */
      tResult * ListNearestQuery(ObjectType * sample, stCount k, bool tie = false);

      /**
      * This method will perform a K-Nearest Neighbor query using a global priority
      * queue based on chained list to "enhance" its performance. We believe that the
      * use of a priority queue during the search in the index nodes will force the
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
      * This method will perform a K-Farthest Neighbor query using a global priority
      * queue based on chained list to "enhance" its performance. We believe that the
      * use of a priority queue during the search in the index nodes will force the
      * radius to converge faster than the normal farthest query.
      *
      * <p>This implementation may replace the original implementation if it proves
      * to be better.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void FarthestQuery
      */
      tResult * FarthestQuery(ObjectType * sample, stCount k, bool tie = false);

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
      * This method will inicializate the parameters to perform a
      * K-Nearest Neighbor query using a global priority queue based
      * on a dynamic link list.
      *
      * <P>This method is based on Hjaltson and Samet (ACM TODS-2000). It is
      * possible to get k+1 object after retrieve the k's objects.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void IncrementalNearestQuery
      */
      tResult * IncrementalListNearestQuery(ObjectType * sample, stCount k,
                                            bool tie = false);

      /**
      * This method will inicializate the parameters to perform a
      * K-Nearest Neighbor query using a global priority queue based
      * on a dynamic heap to "enhance" its performance.
      *
      * <P>This method is based on Hjaltson and Samet (ACM TODS-2000). It is
      * possible to get k+1 object after retrieve the k's objects.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void IncrementalNearestQuery
      */
      tResult * IncrementalNearestQuery(ObjectType * sample, stCount k,
                                        bool tie = false);

      /**
      * This method will inicializate the parameters to perform a
      * K-Nearest Neighbor query using a global priority queue based
      * on a dynamic heap to "enhance" its performance.
      *
      * <P>This method is based on Hjaltson and Samet (ACM TODS-2000). It is
      * possible to get k+1 object after retrieve the k's objects.
      *
      * <P>This is possible because the global priority is not delete and
      * the subtrees are not pruned.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param The result.
      * @param globalQueue a queue to be managed.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The instance of tPGenericHeap must be created and destroied by user.
      * @see void IncrementalNearestQuery
      */
      void InitializeIncrementalNearestQuery(ObjectType * sample, stCount k,
                                             tResult * result,
                                             tPGenericHeap * globalQueue);

      /**
      * This method will inicializate the parameters to perform a
      * K-Nearest Neighbor query using a global priority queue based
      * on a dynamic list.
      *
      * <P>This method is based on Hjaltson and Samet (ACM TODS-2000). It is
      * possible to get k+1 object after retrieve the k's objects.
      *
      * <P>This is possible because the global priority is not delete and
      * the subtrees are not pruned.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param The result.
      * @param globalQueue a queue to be managed.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The instance of tPGenericHeap must be created and destroied by user.
      * @see void IncrementalNearestQuery
      */
      void InitializeIncrementalNearestQuery(ObjectType * sample, stCount k,
                                             tResult * result,
                                             tGenericPriorityQueue * globalQueue);

      /**
      * This method will perform a K-Nearest Neighbor query using a global
      * priority queue based on a dynamic heap to "enhance" its performance.
      *
      * <P>This method is based on Hjaltson and Samet (ACM TODS-2000). It is
      * possible to get k+1 object after retrieve the k's objects.
      *
      * <P>This is possible because the global priority is not delete and
      * the subtrees are not pruned.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param The result.
      * @param globalQueue a queue to be managed.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The instance of tPGenericHeap must be created and destroied by user.
      * @warning This method must be called after InitializeIncrementalNearestQuery.
      * @see void InitializeIncrementalNearestQuery
      */
      void IncrementalNearestQuery(ObjectType * sample, stCount k,
                                   tResult * result,
                                   tPGenericHeap * globalQueue);

      /**
      * This method will perform a K-Nearest Neighbor query using a global
      * priority queue based on a dynamic list.
      *
      * <P>This method is based on Hjaltson and Samet (ACM TODS-2000). It is
      * possible to get k+1 object after retrieve the k's objects.
      *
      * <P>This is possible because the global priority is not delete and
      * the subtrees are not pruned.
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param The result.
      * @param globalQueue a queue to be managed.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The instance of tPGenericHeap must be created and destroied by user.
      * @warning This method must be called after InitializeIncrementalNearestQuery.
      * @see void InitializeIncrementalNearestQuery
      */
      void IncrementalNearestQuery(ObjectType * sample, stCount k,
                                   tResult * result,
                                   tGenericPriorityQueue * globalQueue);

      /**
      * This method will perform a range query with a limited number of results.
      *
      * <P>This query is a combination of the standard Range Query and the standard
      * K-Nearest Neighbor Query. All objects which matches both conditions
      * will be included in the result. The K-Nearest Neighbor Query is implemented
      * with a local priority queue.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @warning The instance of tResult returned must be destroied by user.
      * @see void KAndRangeQuery
      * @warning This method does not work for trees with only one node.
      */
      tResult * KAndRangeQuery(ObjectType * sample, stDistance range,
                               stCount k, bool tie=false);

      /**
      * This method will perform range query with a limited number of results.
      *
      * <P>This query is a combination of the standard range query and the standard
      * K-Nearest Neighbor Query. All objects which matches with one of two conditions
      * will be included in the result. The K-Nearest Neighbor Query is implemented
      * with a local priority queue.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void KOrRangeQuery
      * @warning This method does not work for trees with only one node.
      */
      tResult * KOrRangeQuery(ObjectType * sample, stDistance range,
                              stCount k, bool tie = false);

      /**
      * This method will perform the disjunctive complex similarity query between
      * range and k-nearest neighbor operator. However, in this method, this is
      * done through of range queries.
      *
      * <P>This query is a combination of the standard range query and the standard
      * K-Nearest Neighbor Query. All objects which matches with one of two conditions
      * will be included in the result. The K-Nearest Neighbor Query is implemented
      * with a global priority queue.
      *
      * @param sample The sample object.
      * @param fractalDimension The fractal dimension of the data set.
      * @param nroObjects The total number of objects of the data set.
      * @param radiusKOrRange Th radius of the range operator.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void KOrRangeQuery
      */
      tResult * SimpleKOrRangeQuery(ObjectType * sample, double fractalDimension,
                                    stDistance radiusKOrRange, stCount k,
                                    bool tie = false);

      /**
      * This method will perform a ring query.
      * The result will be a set of pairs object/distance.
      *
      * @param sample The sample object.
      * @param inRange The outter range of the results.
      * @param outRange The inner range of the results.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The value of outRange must be higher than inRange.
      * @see void RingQuery()
      */
      tResult * RingQuery(ObjectType * sample, stDistance inRange,
                          stDistance outRange);

      /**
      * This method will perform a ring query with K-Nearest Neighbor query.
      * The result will be a set of pairs object/distance.
      *
      * @param sample The sample object.
      * @param inRange The outter range of the results.
      * @param outRange The inner range of the results.
      * @param k The number of nearest neighbor.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The value of outRange must be higher than inRange.
      * @see void RingKQuery()
      */
      tResult * KRingQuery(ObjectType * sample, stDistance inRange,
                           stDistance outRange, stCount k, bool tie = false);

      /**
      * This method will perform a ring query with K-Nearest Neighbor query
      * based on a local chained list.
      * The result will be a set of pairs object/distance.
      *
      * @param sample The sample object.
      * @param inRange The outter range of the results.
      * @param outRange The inner range of the results.
      * @param k The number of nearest neighbor.
      * @param tie The tie list. This parameter is optional. Default false;
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @warning The value of outRange must be higher than inRange.
      * @see void RingKQuery()
      */
      tResult * LocalKRingQuery(ObjectType * sample, stDistance inRange,
                                stDistance outRange, stCount k, bool tie = false);

      /**
      * This method will perform a range query with a limited number of results.
      * <P>A lazy range query recovers the k elements within a radius, independently
      * if the k elements are the k nearest elements of query object.
      *
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @param tie The tie list. This parameter is optional. Default false;
      * @warning The instance of tResult returned must be destroied by user.
      * @see void LazyRangeQuery
      */
      tResult * LazyRangeQuery(ObjectType * sample, stDistance range,
                               stCount k, bool tie = false);

      /**
      * This method will perform a aproximate K-Nearest Neighbor query
      *
      * @param sample The sample object.
      * @param k The number of neighbors.
      * @param tie The tie list. Default false.
      * @return The result or NULL if this method is not implemented.
      * @warning The instance of tResult returned must be destroied by user.
      * @see void AproximateNearestQuery
      */
      tResult * AproximateNearestQuery(ObjectType * sample, stCount k, bool tie = false);


      /**
      * Calculates the FatFactor of this tree.
      *
      * @warning This method will update the statistics of the tree.
      */
      double GetFatFactor();

      // Visualization support
      #ifdef __stMAMVIEW__
         /**
         * Sets the visualization output directory.
         *
         * @param dir The directory name.
         * @warning This method does not exist if __stMANVIEW__ is not defined.
         */
         void MAMViewSetOutputDir(const char * dir){

            MAMViewer->SetOutputDir(dir);
         }//end MAMViewSetOutputDir

         /**
         * Initializes the MAMView module. It will restart he
         *
         * @param dir The directory name.
         * @warning This method does not exist if __stMANVIEW__ is not defined.
         */
         void MAMViewInit();

         /**
         * Creates a dump of the tree which can be viewed by MAMViewer.
         *
         * @warning This method does not exist if __stMANVIEW__ is not defined.
         */
         void MAMViewDumpTree();
      #endif //__stMAMVIEW__

      /**
      * @copydoc stMetricTree::GetTreeInfo()
      */
      virtual stTreeInfoResult * GetTreeInfo();

      /**
      * Optimizes the structure of this tree by executing the Slim-Down
      * algorithm.
      *
      * <p>The Slim-Down algorithm can only be performed when the tree has at
       * least 3 levels. Fortunately, this
      */
      virtual void Optimize();

   private:

      /**
      * This type defines the logic node for this class.
      */
      typedef stSlimLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type defines the MST splitter for this class.
      */
      typedef stSlimMSTSplitter < ObjectType, EvaluatorType > tMSTSplitter;

      /**
      * This type is used by the priority key.
      */
      typedef stRPriorityQueue < stDistance, stCount > tPriorityQueue;

      /**
      * This type is used by the priority key in IncrementalNearest.
      */
      typedef stDynamicRPriorityQueue < stDistance, stQueryPriorityQueueValue > tDynamicPriorityQueue;

      typedef stDynamicRReversedPriorityQueue < stDistance, stQueryPriorityQueueValue > tDynamicReversedPriorityQueue;

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

         /**
         * Number of objects in the subtree.
         */
         stCount NObjects;
      };

      // Visualization support
      #ifdef __stMAMVIEW__
         /**
         * Type of the object sample array. Used by the visualization module.
         *
         * @warning This method does not exist if __stMAMVIEW__ is not defined.
         */
         typedef stMAMViewObjectSample <ObjectType> tObjectSample;

         /**
         * Type of the MAM view extractor for this tree. Used by the visualization
         * module.
         *
         * @warning This method does not exist if __stMAMVIEW__ is not defined.
         */
         typedef stMAMViewExtractor <ObjectType, EvaluatorType> tViewExtractor;

         /**
         * The MAM Viewer extractor for this tree.
         */
         tViewExtractor * MAMViewer;
      #endif //__stMAMVIEW__

      /**
      * If true, the header mus be written to the page manager.
      */
      bool HeaderUpdate;

      /**
      * The SlimTree header. This variable points to data in the HeaderPage.
      */
      stSlimHeader * Header;

      /**
      * Pointer to the header page.
      * The Slim Tree keeps this page in memory for faster access.
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
         if (HeaderUpdate){
            this->myPageManager->WriteHeaderPage(HeaderPage);
            HeaderUpdate = false;
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
      int ChooseSubTree(stSlimIndexNode * slimIndexNode, ObjectType * obj);

      /**
      * Compute two elements from the page and use them for being the center
      * of the index entries in the parent page of the SlimTree.
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
      * @todo pq nao usa return para voltar o novo centro ou o novo raio em vez de
      * passar por referencia?
      * this method was revised by Marcos
      */
      void ReDistribute(stSlimNode * node, int & newCenter, stDistance & newRadius);

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
         HeaderUpdate = true;
      }//end SetRoot

      /**
      * Updates the object counter.
      */
      void UpdateObjectCounter(int inc){
         Header->ObjectCount += inc;
         HeaderUpdate = true;
      }//end UpdateObjectCounter

      /**
      * Used by GetGreaterDistance() to get all objects in the leaf node.
      *
      * @param pageID the pageId to be analyze.
      * @param objects the vector to store all objects.
      * @param size the size of the vector objects.
      * @see GetGreaterDistance()
      */
      void GetGreaterDistance(stPageID pageID, ObjectType ** objects,
                              stCount & size);

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
                          ObjectType * repObj, stSubtreeInfo & promo1,
                          stSubtreeInfo & promo2);

      /**
      * Creates and updates the new root of the SlimTree.
      *
      * @param obj1 Object 1.
      * @param radius1 Radius of subtree 1.
      * @param nodeID1 ID of the root page of the sub-tree 1.
      * @param nEntries1 Number of entries in the sub-tree 1.
      * @param obj2 Object 2.
      * @param radius2 Radius of subtree 2.
      * @param nodeID2 ID of the root page of the sub-tree 2.
      * @param nEntries2 Number of entries in the sub-tree 2.
      */
      void AddNewRoot(ObjectType *obj1, stDistance radius1, stPageID nodeID1,
                      stCount nEntries1, ObjectType *obj2, stDistance radius2,
                      stPageID nodeID2, stCount nEntries2);

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
      * @todo redo the FatFactorPromote method.
      */
      void SplitLeaf(stSlimLeafNode * oldNode, stSlimLeafNode * newNode,
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
      * @param newNEntries1 The new object 1 number of entries.
      * @param newObj2 The new object 2 to be added or NULL.
      * @param newRadius2 The new object 2 radius if newObj2 is not NULL.
      * @param newNodeID2 The new object 2 node ID  if newObj2 is not NULL..
      * @param newNEntries2 The new object 2 number of entries if newObj2 is
      * not NULL.
      * @param prevRep The previous representative.
      * @param promo1 The promoted subtree. If its representative is NULL,
      * the choosen representative is equal to prevRep.
      * @param promo2 The promoted subtree. The representative of this tree will
      * never be the prevRep.
      * @todo redo the FatFactorPromote method.
      */
      void SplitIndex(stSlimIndexNode * oldNode, stSlimIndexNode * newNode,
                      ObjectType * newObj1, stDistance newRadius1,
                      stPageID newNodeID1, stCount newNEntries1,
                      ObjectType * newObj2, stDistance newRadius2,
                      stPageID newNodeID2, stCount newNEntries2,
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
      * This method will perform a reverse range query.
      * The result will be a set of pairs object/distance.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the result. All object that are
      * greater than the range distance will be included in the result set.
      * @param distanceRepres The distance of the representative.
      * @see tResult * RangeQuery()
      */
      void ReversedRangeQuery(stPageID pageID, tResult * result,
                              ObjectType * sample, stDistance range,
                              stDistance distanceRepres);

      /**
      * This method will perform a K Nearest Neighbor query using a global priority
      * queue based on chained list.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The number of neighbours.
      * @param globalQueue The global priority queue.
      * @see tResult * NearestQuery
      */
      void LocalNearestQuery(stPageID pageID, tResult * result,
                             ObjectType * sample, stDistance & rangeK,
                             stCount k, stDistance distanceRepres);

      /**
      * This method will perform a K Nearest Neighbor query using a global priority
      * queue based on chained list.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The number of neighbours.
      * @param globalQueue The global priority queue.
      * @see tResult * NearestQuery
      */
      void ListNearestQuery(tResult * result, ObjectType * sample,
                            stDistance rangeK, stCount k);

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
      * This method will perform a K-Farthest Neighbor query using a priority
      * queue.
      *
      * @param result the result set.
      * @param sample The sample object.
      * @param rangeK The range of the results.
      * @param k The number of farthest neighbours.
      * @see tResult * NearestQuery
      */
      void FarthestQuery(tResult * result, ObjectType * sample,
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
      * This method will perform a range query with a limited number of results.
      *
      * <P>This query is a combination of the standard range query and the standard
      * k-nearest neighbour query. All objects which matches both conditions
      * will be included in the result. The K-Nearest Neighbor Query is implemented
      * with a local priority queue.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @see tResult * KAndRangeQuery
      * @warning This method does not work for trees with only one node.
      */
      void KAndRangeQuery(tResult * result, ObjectType * sample,
                          stDistance range, stCount k);

      /**
      * This method will perform range query with a limited number of results.
      *
      * <P>This query is a combination of the standard range query and the standard
      * K-Nearest Neighbor query. All objects which matches with one of two conditions
      * will be included in the result. The K-Nearest Neighbor Query is implemented
      * with a local priority queue.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the results.
      * @param dk The maximum distance.
      * @param k The maximum number of results.
      * @see tResult * KOrRangeQuery
      * @warning This method does not work for trees with only one node.
      */
      void KOrRangeQuery(tResult * result, ObjectType * sample,
                         stDistance range, stCount k);

      /**
      * This method will perform a ring query.
      * The result will be a set of pairs object/distance.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param inRange The outter range of the results.
      * @param outRange The inner range of the results.
      * @return The result or NULL if this method is not implemented.
      * @param distanceRepres The distance of the representative.
      * @warning The value of outRange must be higher than inRange.
      * @see tResult * RingQuery()
      */
      void RingQuery(stPageID pageID, tResult * result,
                     ObjectType * sample, stDistance inRange,
                     stDistance outRange, stDistance distanceRepres);

      /**
      * This method will perform a ring query with K-Nearest Neighbor based on
      * a global chained list.
      * The result will be a set of pairs object/distance.
      *
      * @param result the result set.
      * @param sample The sample object.
      * @param inRange The outter range of the results.
      * @param outRange The inner range of the results.
      * @param k The number of nearest neighbor.
      * @warning The value of outRange must be higher than inRange.
      * @see tResult * RingKQuery()
      */
      void KRingQuery(tResult * result, ObjectType * sample, stDistance inRange,
                      stDistance & outRange, stCount k);

      /**
      * This method will perform a ring query with K-Nearest Neighbor based on
      * a local chained list.
      * The result will be a set of pairs object/distance.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param inRange The outter range of the results.
      * @param outRange The inner range of the results.
      * @param k The number of nearest neighbor.
      * @return The result or NULL if this method is not implemented.
      * @param distanceRepres The distance of the representative.
      * @param globalQueue The global chained list.
      * @warning The value of outRange must be higher than inRange.
      * @see tResult * RingKQuery()
      */
      void LocalKRingQuery(stPageID pageID, tResult * result, ObjectType * sample,
                           stDistance inRange, stDistance & outRange, stCount k,
                           stDistance distanceRepres);

      /**
      * This method will perform a range query with a limited number of results.
      * <P>A lazy range query recovers the k elements within a radius, independently
      * if the k elements are the k nearest elements of query object.
      *
      * @param pageID the page to be analyzed.
      * @param result the result set.
      * @param sample The sample object.
      * @param range The range of the results.
      * @param k The maximum number of results.
      * @see tResult * LazyRangeQuery
      */
      void LazyRangeQuery(stPageID pageID, tResult * result,
                          ObjectType * sample, stDistance & range, stCount k,
                          stDistance distanceRepres, bool & stop);

      /**
      * Updates the distances of the objects from the new representative.
      */
      void UpdateDistances(stSlimIndexNode * node, ObjectType * repObj,
                           stCount repObjIdx);

      // Visualization support
      #ifdef __stMAMVIEW__

         /**
         * This method gets a sample of random objects from the tree. It is used
         * by the visualization module to initializes the fastmapper.
         *
         * @param Size of the sample.
         * @warning This method does not exist if __stMANVIEW__ is not defined.
         */
         tObjectSample * GetSample(int sampleSize);

         /**
         * This method travels by the tree structure picking random objects stored
         * in the leaf nodes.
         *
         * @param pageID The ID of the root page.
         * @param sample The sample of objects.
         * @warning This method does not exist if __stMAMVIEW__ is not defined.
         */
         void GetSampleRecursive(stPageID pageID, tObjectSample * sample);

         /**
         * This method travels through the tree structure and creates a dump of the
         * tree.
         *
         * @param pageID The ID of the root page.
         * @warning This method does not exist if __stMAMVIEW__ is not defined.
         */
         void MAMViewDumpTreeRecursive(stPageID pageID, ObjectType * rep,
                                       stDistance radius, stPageID parent);

      #endif //__stMAMVIEW__

      /**
      * This method travels through the tree gathering information about each level.
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


      // Slimdown implementaion
      /**
      * This method performs the Slim-Down in the whole tree recursively. I starts from the
      * root traveling through the tree in post order. Every time it reaches an index node
      * in the second level from bottom (the index node above the leafs) it calls the method
      * SlimDown() to perform a local slim down. The radius of each subtree is updated during
      * the proccess to keep the tree consistent.
      *
      * @param pageID Root of the subtree.
      * @param level Current level (the first call must be 0).
      * @return The new radius of the subtree.
      * @warning This method will not work if the tree has less than 3 levels.
      */
      stDistance SlimDownRecursive(stPageID pageID, int level);

      /**
      * This method performs the local slim down in the given subtree.
      *
      * @param pageID The subtree root.
      * @return The new radius of the subtree.
      */
      stDistance SlimDown(stPageID pageID);

      /**
      * Perform the SlimDown in a set of stSlimMemLeafNode.
      *
      * @param memLeafNodes Leaf nodes.
      * @param nodeCount Number of nodes in memLeafNodes.
      * @param maxSwaps Swap limit.
      */
      void LocalSlimDown(tMemLeafNode ** memLeafNodes, int nodeCount, int maxSwaps);

      /**
      * Verifies if the last object of src can be moved to dst. It will test:
      *     - dst covers src's last object
      *     - dst free space
      *
      * @param src Source node.
      * @param dst Destination node.
      * @retval distance Distance from the last object of src to the
      * representative of dst.
      * @return True if it can be swapped of false otherwise.
      * @warning The occupation of src is never tested.
      */
      bool SlimDownCanSwap(tMemLeafNode * src, tMemLeafNode * dst,
                           stDistance & distance);

};//end stSlimTree

// Include implementation
#include "stSlimTree.cc"

#endif //__STSLIMTREE_H


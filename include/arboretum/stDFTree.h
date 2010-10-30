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
* This file defines the class stDFTree.
*
* @version 1.0
* $Revision: 1.9 $
* $Date: 2005/03/08 19:43:09 $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Joselene Marques (joselene@icmc.usp.br)

*/
// Copyright (c) 2003 GBDI-ICMC-USP

#ifndef __STDFTREE_H
#define __STDFTREE_H

#include <arboretum/stUtil.h>
#include <arboretum/stTypes.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stDFNode.h>
#include <arboretum/stPageManager.h>
#include <arboretum/stGenericPriorityQueue.h>

// this is used to set the initial size of the dynamic queue
#define STARTVALUEQUEUE 200
// this is used to set the increment size of the dynamic queue
#define INCREMENTVALUEQUEUE 200
// this is used to set GR Vector
#define STFOCUS 3

#include <string.h>
#include <math.h>
#include <values.h>
#include <algorithm>

template <class ObjectType, class EvaluatorType>
class stDFGlobalRep;

//=============================================================================
// Class template stDFLogicNode
//-----------------------------------------------------------------------------
/**
* Thic class template represents a DFTree logic node entry. It is used to
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
* $Date: 2005/03/08 19:43:09 $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Joselene Marques (joselene@icmc.usp.br)
* @todo Documentation review.
* @todo More tests!
* @ingroup DF
*/
template <class ObjectType, class EvaluatorType>
class stDFLogicNode{

   public:

      /**
      * This type implements the GR.
      */
      typedef stDFGlobalRep <ObjectType, EvaluatorType> tGR;

      /**
      * Creates a new instance of this node with no objects.
      *
      * @param maxOccupation The maximum number of entries.
      */
      stDFLogicNode(stCount maxOccupation);

      /**
      * Disposes this instance and releases all related resources. All instances of
      * object added to this node will also be deleted unless it is not owned by
      * this node (see method BuyObject()).
      */
      ~stDFLogicNode();

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
         Entries[Count].FieldDistance  = new stDistance[STFOCUS];
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
      ObjectType * GetObject(stCount idx){
         return Entries[idx].Object;
      }//end GetObject

      /**
      * Returns the object of a given entry.
      *
      * @param idx The object index.
      */
      ObjectType * operator [](stCount idx){
         return Entries[idx].Object;
      }//end operator []

      /**
      * Returns the PageID of a given entry. Only index nodes will use this field.
      *
      * @param idx The object index.
      */
      stPageID GetPageID(stCount idx){
         return Entries[idx].PageID;
      }//end GetPageID

      /**
      * Returns the number of entries in the sub-tree of a given entry.
      * Only index nodes will use this field.
      *
      * @param idx The object index.
      */
      stSize GetNEntries(stCount idx){
         return Entries[idx].NEntries;
      }//end GetNEntries

      /**
      * Returns the radius of the sub-tree of a given entry. Only index nodes
      * will use this field.
      *
      * @param idx The object index.
      */
      stDistance GetRadius(stCount idx){
         return Entries[idx].Radius;
      }//end GetRadius

      /**
      * Sets the data associated with a given entry.
      *
      * @param idx The object index.
      * @param pageID The pageID.
      * @param nEntries The number of entries in the sub-tree.
      * @param radius The radius of the sub-tree.
      * @param FieldDistance The distance from object to GR
      */
      void SetEntry(stCount idx, stPageID pageID, stSize nEntries,
                    stDistance radius, stDistance * fieldDistance){
         Entries[idx].PageID = pageID;
         Entries[idx].NEntries = nEntries;
         Entries[idx].Radius = radius;
         memcpy(Entries[idx].FieldDistance, fieldDistance, 3 * sizeof(stDistance));
      }//end GetPageID

      /**
      * Adds all objects of an index node. It will also set the node type to
      * stDFNode::INDEX.
      *
      * @param node The node.
      */
      void AddIndexNode(stDFIndexNode * node);

      /**
      * Adds all objects of a leaf node. It will also set the node type to
      * stDFNode::LEAF.
      *
      * @param node The node.
      */
      void AddLeafNode(stDFLeafNode * node);

      /**
      * Returns the id of the representative object.
      *
      * @param id The representative ID. It may be 0 or 1.
      */
      stCount GetRepresentativeIndex(stCount idx){
         return RepIndex[idx];
      }//end GetRepresentativeIndex

      /**
      * Returns the representative object.
      *
      * @param id The representative ID. It may be 0 or 1.
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
      * @param GR Used to know when find GR
      * @return The number of computed distances.
      */
      int Distribute(stDFIndexNode * node0, ObjectType * & rep0,
            stDFIndexNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator, tGR * GR){
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
      * @param GR Used to know when find GR
      * @return The number of computed distances.
      */
      int Distribute(stDFLeafNode * node0, ObjectType * & rep0,
            stDFLeafNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator, tGR * GR){
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
      int TestDistribution(stDFIndexNode * node0, stDFIndexNode * node1,
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
      int TestDistribution(stDFLeafNode * node0, stDFLeafNode * node1,
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
      * Returns the node type. It may assume the values stDFNode::INDEX or
      * stDFNode::LEAF.
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

      /**
      * Returns the GR of a given entry.
      *
      * @param idx The object index.
      * @return The vector with the GR
      */
      stDistance * GetFieldDistance(stCount idx){
         return Entries[idx].FieldDistance;
      }//end GetFieldDistance

      /**
      * Sets the Focus Distance.
      *
      * @param idx The object index.
      * @param iGlobalRep The GlobalRep index.
      * @param FieldDistance The distance to GlobalRep index.
      */
      void SetFocusDistance(stCount idx, stCount iGlobalRep, stDistance iFieldDistance){
         Entries[idx].FieldDistance[iGlobalRep] = iFieldDistance;
      }//end SetFocusDistance

      /**
      * Set GR of a given entry.
      *
      * @param idx The object index.
      * @param FieldDistance The GR Vector
      */
      void SetFieldDistance(stCount idx, stDistance * fieldDistance){
         memcpy(Entries[idx].FieldDistance, fieldDistance, 3 * sizeof(stDistance));
      }//end SetFieldDistance

   private:

      /**
      * This type represents a DF tree logic node entry.
      */
      struct stDFLogicEntry{

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
         * Distances between object and GR.
         */
         stDistance * FieldDistance;

         /**
         * Node Map.
         */
         bool Mapped;
      };

      /**
      * Entries.
      */
      struct stDFLogicEntry * Entries;

      /**
      * Minimum occupation.
      */
      stCount MinOccupation;

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
      int NodeType;

      /**
      * Updates all distances between representatives and all objects in this
      * node. It returns the number of distances calculated.
      *
      * @param metricEvaluator The metric evaluator to be used.
      */
      int UpdateDistances(EvaluatorType * metricEvaluator);
      
};//end stDFLogicNode

//=============================================================================
// Class template stDFMSTSpliter
//-----------------------------------------------------------------------------
/**
* This class template implements the DFTree MST split algorithm.
*
* @version 1.0
* $Date: 2005/03/08 19:43:09 $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Joselene Marques(joselene@icmc.usp.br)

* @todo Documentation review.
* @todo Tests!
* @ingroup DF
*/
template <class ObjectType, class EvaluatorType>
class stDFMSTSplitter{

   public:

      /**
      * This type defines the logic node for this class.
      */
      typedef stDFLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type implements the GR.
      */
      typedef stDFGlobalRep <ObjectType, EvaluatorType> tGR;

      /**
      * Builds a new instance of this class. It will claim the ownership of the
      * logic node provided as input.
      */
      stDFMSTSplitter(tLogicNode * node);

      /**
      * Disposes all associated resources.
      */
      ~stDFMSTSplitter();

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
      int Distribute(stDFIndexNode * node0, ObjectType * & rep0,
            stDFIndexNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator, tGR * GR);

      /**
      * Distributes objects between 2 leaf nodes.
      */
      int Distribute(stDFLeafNode * node0, ObjectType * & rep0,
            stDFLeafNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator, tGR * GR);

   protected:

      friend stDFGlobalRep <ObjectType, EvaluatorType>;

      /**
      * This type implements the GR.
      */
      typedef stDFGlobalRep <ObjectType, EvaluatorType> tGR;

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
      * Returns the distance of a given index.
      *
      * @param idi The Matrix row.
      * @param idj The Matrix column.
      */
      stDistance GetMatrixDistance(int idi, int idj){
         return DMat[idi][idj];
      }//end GetDistance


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
      * @param GR Used to find GR at first split
      * @warning DMat must be initialized.
      */
      void PerformMST(tGR * GR);

      /**
      * Joins 2 clusters. This method will insert custer2 into cluster1.
      *
      * <P>The state of cluster2 will change to DEATH_SENTENCE.
      *
      * @param cluster1 Cluster 1.
      * @param cluster2 Cluster 2.
      */
      void JoinClusters(int cluster1, int cluster2);
      
};//end stDFMSTSplitter


//=============================================================================
// Class template stDFTree
//-----------------------------------------------------------------------------
/**
* This class defines all behavior of the DFTree.
* Probably most of the atributes will be stored in the header page of the used
* stPageManager (stDiskPageManager or stMemoryPageManager).
*
* <P> First implementation is based on original code by Agma Traina, Bernard Seeger
* and Caetano Traina
* <P> Main modifications from original code are intent to turn it an object oriented
* compliant code.
*
* $Date: 2005/03/08 19:43:09 $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Joselene Marques(joselene@icmc.usp.br)

* @todo More documentation.
* @todo Finish the implementation.
* @version 1.0
* @ingroup DF
*/
template <class ObjectType, class EvaluatorType>
class stDFTree: public stMetricTree< ObjectType, EvaluatorType>{

   public:

      /**
      * This structure defines the DFTree header structure. This type was left
      * public to allow the creation of debug tools.
      */
      typedef struct tDFHeader{
         /**
         * Magic number. This is a short string that must contains the magic
         * string "DF-3". It may be used to validate the file (this feature
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
         * The root of the DF-tree.
         */
         stPageID Root;

         /**
         * The root of the global representative list.
         */
         stPageID RootGlobalRep;

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
      } stDFHeader;

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
         * This is the DF-Tree default split method.
         */
         smSPANNINGTREE
      };//end tSplitMethod

      /**
      * This is the class that abstracs an result set.
      */
      typedef stResult <ObjectType> tResult;

      /**
      * This type implements the GR.
      */
      typedef stDFGlobalRep <ObjectType, EvaluatorType> tGR;

      /**
      * Creates a new metric tree using a given page manager. This instance
      * will not claim the ownership of the given page manager. It means
      * that the application must dispose the page manager when it is no 
      * loger necessary.
      *
      * @param pageman The page manager to be used by this metric tree.
      */
      stDFTree(stPageManager * pageman, stCount nFocus, stDistance pThreshold);

      /**
      * Dispose all used resources, ie, it is the destructor method.
      *
      * @see stDFTree()
      */
      virtual ~stDFTree(){
         // Flus header page.
         FlushHeader();
         // Free it.
         delete GR;
      }//end ~stDFTree()

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
      * This type implements the GR.
      */
      typedef stDFGlobalRep <ObjectType, EvaluatorType> tGR;

      /**
      * Implements the GR part
      */
      tGR * GR;

      /**
      * Memory leaf node used by DF-Down.
      */
      typedef stDFMemLeafNode < ObjectType > tMemLeafNode;

      /**
      * Calculates the FatFactor of this tree.
      *
      * @warning This method will update the statistics of the tree.
      */
      double GetFatFactor();

      /**
      * @copydoc stMetricTree::GetTreeInfo()
      */
      virtual stTreeInfoResult * GetTreeInfo();

      /**
      * Force Update the GR Distances to all objects
      */
      void ForceUpdateAllFieldDistance();

   private:
   
      /**
      * This type defines the logic node for this class.
      */
      typedef stDFLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type defines the MST splitter for this class.
      */
      typedef stDFMSTSplitter < ObjectType, EvaluatorType > tMSTSplitter;

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
         ** Distance from the global representatives
         **/
         stDistance * FieldDistance;

         /**
         * The ID root of the root of the subtree.
         */
         stPageID RootID;

         /**
         * Number of objects in the subtree.
         */
         stCount NObjects;
      };

      /**
      * If true, the header mus be written to the page manager.
      */
      bool HeaderUpdate;

      /**
      * The DFTree header. This variable points to data in the HeaderPage.
      */
      stDFHeader * Header;

      /**
      * Pointer to the header page.
      * The DF Tree keeps this page in memory for faster access.
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
            myPageManager->WriteHeaderPage(HeaderPage);
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
      * @param DFIndexNode the indexNode to be analyzed
      * @param obj The object that will be inserted.
      * @return the minIndex the index of the choose of the subTree
      */
      int ChooseSubTree(stDFIndexNode * DFIndexNode, ObjectType * obj);

      /**
      * Compute two elements from the page and use them for being the center
      * of the index entries in the parent page of the DFTree.
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
      * @param node node to compute
      * @param newCenter the index of the new center
      * @param newRadius the new radius of this node
      */
      void ReDistribute(stDFNode * node, int & newCenter, stDistance & newRadius);

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
      * @param GR Used to calculate distances to GR
      * @return The action to be taken after the returning. See enum
      * stInsertAction for more details.
      */
      int InsertRecursive(stPageID currNodeID, ObjectType * newObj,
            ObjectType * repObj,
            stSubtreeInfo & promo1, stSubtreeInfo & promo2,
            tGR * GR);

      /**
      * Creates and updates the new root of the DFTree.
      *
      * @param obj1 Object 1.
      * @param radius1 Radius of subtree 1.
      * @param nodeID1 ID of the root page of the sub-tree 1.
      * @param nEntries1 Number of entries in the sub-tree 1.
      * @param FieldDistance1 The distances to GR from Object 1
      * @param obj2 Object 2.
      * @param radius2 Radius of subtree 2.
      * @param nodeID2 ID of the root page of the sub-tree 2.
      * @param nEntries2 Number of entries in the sub-tree 2.
      * @param FieldDistance2 The distances to GR from Object 2
      */
      void AddNewRoot(ObjectType * obj1, stDistance radius1, stPageID nodeID1,
               stCount nEntries1, stDistance * fieldDistance1,
               ObjectType * obj2, stDistance radius2, stPageID nodeID2,
               stCount nEntries2, stDistance * fieldDistance2);

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
      * @param FieldDistance The distances to GR from newObj
      * @param prevRep The previous representative.
      * @param promo1 The promoted subtree. If its representative is NULL,
      * the choosen representative is equal to prevRep.
      * @param promo2 The promoted subtree. The representative of this tree will
      * never be the prevRep.
      * @todo redo the FatFactorPromote method.
      */
      void SplitLeaf(stDFLeafNode * oldNode, stDFLeafNode * newNode,
            ObjectType * newObj, stDistance * fieldDistance, ObjectType * prevRep,
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
      * @param FieldDistance1 The distances to GR from newObj1
      * @param prevRep The previous representative.
      * @param promo1 The promoted subtree. If its representative is NULL,
      * the choosen representative is equal to prevRep.
      * @param promo2 The promoted subtree. The representative of this tree will
      * never be the prevRep.
      * @param FieldDistance2 The distances to GR from newObj2
      * @todo redo the FatFactorPromote method.
      */
      void SplitIndex(stDFIndexNode * oldNode, stDFIndexNode * newNode,
            ObjectType * newObj1, stDistance newRadius1,
            stPageID newNodeID1, stCount newNEntries1, stDistance * fieldDistance1,
            ObjectType * newObj2, stDistance newRadius2,
            stPageID newNodeID2, stCount newNEntries2, stDistance * fieldDistance2,
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
      * @param DistQueryFocus The distances vector of the sample to GR.
      * @see tResult * RangeQuery()
      */
      void RangeQuery(stPageID pageID, tResult * result,
                      ObjectType * sample, stDistance range,
                      stDistance distanceRepres, stDistance * DistQueryFocus);

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
      void UpdateDistances(stDFIndexNode * node, ObjectType * repObj,
                           int repObjIdx);

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

};//end stDFTree

//=============================================================================
// Class template stDFGlobalRep
//-----------------------------------------------------------------------------
/**
* This class defines all behavior of the GR.
* $Date: 2005/03/08 19:43:09 $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Joselene Marques (joselene@icmc.usp.br)
* @version 1.0
* @ingroup DF
*/
template <class ObjectType, class EvaluatorType>
class stDFGlobalRep {

   public:

      /**
      * This type defines the logic node for this class.
      */
      typedef stDFLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type defines the MST splitter for this class.
      */
      typedef stDFMSTSplitter < ObjectType, EvaluatorType > tMSTSplitter;

      /**
      * Number of focus.
      */
      stCount NumFocus;

      /**
      * This type represents a GlobalRep.
      */
      struct stDFGlobalRepEntry{
         ObjectType * Object;
         stDistance MaxDistance;
      };//end stDFGlobalRepEntry

      /**
      * Find the GR.
      *
      * @param node The node used to find the best focus
      * @param MSTSplitter Used to get the distance matrix already calculate
      * to do the split
      * @param Center1 Center of new subtree (cannot be focus)
      * @param Center2 Center of new subtree (cannot be focus)
      * @param numObject Total number of objects
      * @warning The distance matrix need already have been calculated
      */
      void FindGlobalRep(tLogicNode * logicNode, tMSTSplitter * MSTSplitter,
                         stCount center1, stCount center2, stCount numObject);

      /**
      * Builds a new instance of this class.
      * @param nFocus Number of focus
      * @param pThreshold Limit of uncircumbscribed object
      * @param metricEvaluator The metric evaluator to compute distances.
      */
      stDFGlobalRep(stCount nFocus, stDistance pThreshold,
                    EvaluatorType * metricEvaluator);

      /*
      * This method verify if the first Split not ocurred already
      * @return The boolean value that define if the first Split not ocurred already
      */
      bool FirstSplit(){
         return bFirstSplit;
      };//end FirstSplit

      /*
      * This method sets that the first Split already ocurred
      */
      void SetFirstSplit(){
         bFirstSplit = FALSE;
      };//end SetFirstSplit

      /**
      * Calculate the GR Distances of a given Leaf node.
      *
      * @param node The node leaf.
      * @param idx The object index.
      * @param Object The object.
      */
      void SetFieldDistance(stDFLeafNode * node, stCount idx,
                            ObjectType * object);

      /**
      * Calculate the GR Distances of a given index node.
      *
      * @param node The node index.
      * @param idx The object index.
      * @param Object The object.
      */
      void SetFieldDistance(stDFIndexNode * node, stCount idx,
                            ObjectType * object);

      /**
      * Calculate the GR Distances of a given object.
      *
      * @param Object The object.
      * @param FieldDistance The distances to GR from object
      */
      void BuildFieldDistance(ObjectType * object, stDistance * fieldDistance);

      /**
      * Defines if the subtree must be pruned by GR
      *
      * @param ObjGRCoord GR Distances from node
      * @param Radius Radius node
      * @param DistQueryFocus Distances from the query object to the GlobalRep
      * @param QueryRadius raio da consulta
      */
      bool PruneByGlobalRep(stDistance * objGRCoord, stDistance radius,
                            stDistance * distQueryFocus, stDistance queryRadius);

      /**
      * Detect when to update de GR and update the accumulated number of
      *  new uncircumbscribed objects(Uncircumbscribed)
      *
      * @param pageman The page manager to be used to get pages
      * @param RootPageID The root page to begin looking for new focus if necessary
      * @param FieldDistance GR Distances of new object
      */
      void CheckUpdate(stPageManager * pageman, stPageID rootPageID,
                       stDistance * fieldDistance);

      /**
      * Executed when the CheckUpdate triggers, add uncircumscribed objects to
      * Candidade Set of new Focus
      *
      * @param pageman the page manager, used to find the pointer's page
      * @param pageID the ID page
      * @return CandidateSet Set of all candidates to new Focus
      */
      void GetCandidates(stPageManager * pageman, stPageID pageID,
                         vector <ObjectType *> & candidateSet);

      /**
      * Executed when the CheckUpdate triggers, find new GR.
      *
      * @param CandidateSet Set of all candidates to new Focus
      */
      void UpdateGR(vector <ObjectType *> candidateSet);

      /**
      * Update the GR Distances to all objects
      * @param pageman the page manager, used to find the pointer's page
      * @param pageID the ID page
      */
      void UpdateFieldDistance(stPageManager * pageman, stPageID pageID);

   private:

      /**
      * Keep the GR objects
      */
      stDFGlobalRepEntry GlobalRep[STFOCUS];

      /**
      * Acumulate the uncircumbscribed object
      */
      stDistance Uncircumbscribed;

      /**
      * Limit of uncircumbscribed object
      */
      stDistance Threshold;

      /**
      * if first Split already ocurred
      */
      bool bFirstSplit;

      /**
      * the metric Evaluator of this tree
      */
      EvaluatorType * myMetricEvaluator;

      /**
      * Distance matrix type.
      */
      typedef stGenericMatrix <stDistance> tDistanceMatrix;

};//end stDFGlobalRep
// Include implementation
#include "stDFTree.cc"

#endif //__STDFTREE_H

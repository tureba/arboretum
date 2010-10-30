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
/**
* @file
*
* This file defines the class stDBMTree.
*
* @version 1.0
* $Revision: 1.25 $
* $Date: 2005/03/06 18:14:31 $
* $Author: marcos $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2003 GBDI-ICMC-USP
#ifndef __stDBMTREE_H
#define __stDBMTREE_H

/**
* @defgroup DBMTree Structure Layer - DBMTree
*/
#include <arboretum/stUtil.h>
#include <arboretum/stTypes.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stPageManager.h>
#include <arboretum/stGenericPriorityQueue.h>
#include <arboretum/stDBMNode.h>
#include <arboretum/stDBMCollection.h>
#include <arboretum/stMAMView.h> // Visualization support

// this is used to set the initial size of the dynamic queue
#define STARTVALUEQUEUE 200
// this is used to set the increment size of the dynamic queue
#define INCREMENTVALUEQUEUE 200

// Include exception class
#ifdef __stDEBUG__
   #include <arboretum/stException.h>
#endif //__stDEBUG__

//=============================================================================
// Struct template stSubtreeInfo
//-----------------------------------------------------------------------------
/**
* This structure holds a promotion data. It contains the representative object,
* the ID of the root, the Radius and the number of objects of the subtree.
*/
#ifndef __stSubtreeInfo
#define __stSubtreeInfo
template <class ObjectType>
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

   /**
   * Height of the subtree.
   */
   stByte Height;
};
#endif //__stSubtreeInfo

//=============================================================================
// Class template stDBMLogicNode
//-----------------------------------------------------------------------------
/**
* Thic class template represents a DBMTree logic node entry. It is used to
* hold a node in a memory form which allows better way to manipulate entries.
* It also implements means to distribute its contents between 2 nodes or 3 nodes.
*
* @warning This node will take the ownership of all object instances added to this node.
* It means that the object instance will be destroyed by the destructor of this class.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup dbmtree
*/
template <class ObjectType, class EvaluatorType>
class stDBMLogicNode{
   public:
      /**
      * This type is a collection of objects.
      */
      typedef stDBMCollection < ObjectType > tDBMCollection;

      /**
      * This type is a memory node.
      */
      typedef stDBMMemNode < ObjectType > tDBMMemNode;

      /**
      * Distance matrix type.
      */
      typedef stGenericMatrix <stDistance> tDistanceMatrix;

      /**
      * This type is a subtree info.
      */
      typedef stSubtreeInfo <ObjectType> tSubtreeInfo;

      /**
      * Creates a new instance of this node with no objects.
      *
      * @param maxOccupation The maximum number of entries.
      */
      stDBMLogicNode(stCount maxOccupation);

      /**
      * Disposes this instance and releases all related resources. All instances of
      * object added to this node will also be deleted unless it is not owned by
      * this node (see method BuyObject()).
      */
      ~stDBMLogicNode();

      /**
      * Adds an object to this node. This method will claim the ownership
      * of the object instance.
      *
      * <P>Use SetEntry() to fill the other fields of each entry.
      *
      * @param obj The object to be added.
      * @return The entry id or -1 for error.
      */
      stCount AddEntry(ObjectType * obj){
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
      * Returns the PageID of a given entry.
      *
      * @param idx The object index.
      */
      stPageID GetPageID(stCount idx){
         return Entries[idx].PageID;
      }//end GetPageID

      /**
      * Returns the number of entries in the sub-tree of a given entry.
      *
      * @param idx The object index.
      */
      stCount GetNEntries(stCount idx){
         return Entries[idx].NEntries;
      }//end GetNEntries

      /**
      * Returns the height of the sub-tree of a given entry.
      *
      * @param idx The object index.
      */
      stByte GetHeight(stCount idx){
         #ifdef __stDBMHEIGHT__
            return Entries[idx].Height;
         #else
            return 0;
         #endif //__stDBMHEIGHT__
      }//end GetHeight

      /**
      * Returns the number of free objects.
      */
      stCount GetNumberOfFreeObjects();

      /**
      * Returns the radius of the sub-tree of a given entry.
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
      */
      void SetEntry(stCount idx, stPageID pageID
                    #ifdef __stDBMNENTRIES__
                       , stCount nEntries
                    #endif //__stDBMNENTRIES__
                    , stDistance radius
                    #ifdef __stDBMHEIGHT__
                       , stByte height
                    #endif //__stDBMHEIGHT__
                    ){
         Entries[idx].PageID = pageID;
         #ifdef __stDBMNENTRIES__
            Entries[idx].NEntries = nEntries;
         #endif //__stDBMNENTRIES__
         Entries[idx].Radius = radius;
         #ifdef __stDBMHEIGHT__
            Entries[idx].Height = height;
         #endif //__stDBMHEIGHT__
      }//end SetEntry

      /**
      * Adds all objects of a node.
      *
      * @param node The node.
      */
      void AddNode(stDBMNode * node);

      /**
      * Adds all objects of a collection.
      *
      * @param node The collection.
      */
      void AddCollection(tDBMCollection * returnCollection);

      /**
      * Returns the id of the representative object.
      *
      * @param id The representative ID. It may be 0 or 1.
      */
      stCount GetRepresentativeIndex(stCount idx){
         return RepIndex[idx];
      }//end GetRepresentative

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
      * Distribute objects between 2 nodes using the current
      * representatives (see SetRepresentative()). Some objects may be insert
      * in returnCollection.
      *
      * <P>The returnCollection has objects that were not inserted in the
      * others nodes.
      *
      * @param returnCollection The returning node containing objects that were
      * not covered by the other 2 nodes.
      * @param node0 The first node.
      * @param node1 The second node.
      * @param promo Representatives of node 0 and 1.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      */
      void Distribute(tDBMCollection * returnCollection,
            stDBMNode * node0, stDBMNode * node1,
            tSubtreeInfo * promo, EvaluatorType * metricEvaluator);

      void DistributeCollection(tDBMCollection * returnCollection,
            stDBMNode * node0, stDBMNode * node1,
            tSubtreeInfo * promo, EvaluatorType * metricEvaluator);

      /**
      * Distribute objects between 2 nodes using the current
      * representatives (see SetRepresentative()).
      *
      * @param node0 The first node.
      * @param node1 The second node.
      * @param promo Representatives of node 0 and 1.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      * @warning This method does not return a returnCollection.
      * @see Distribute
      */
      void Distribute(stDBMNode * node0, stDBMNode * node1,
                      tSubtreeInfo * promo, EvaluatorType * metricEvaluator);

      /**
      * Tests the distribution of objects between 3 nodes using the current
      * representatives (see SetRepresentative()).
      *
      * @param returnCollection The returning node.
      * @param node0 The first node.
      * @param node1 The second node.
      * @param metricEvaluator The metric evaluator to be used to compute distances.
      * @return The number of computed distances.
      */
      void TestDistribution(tDBMCollection * returnCollection,
            tDBMMemNode * node0, tDBMMemNode * node1,
            EvaluatorType * metricEvaluator);

      /**
      * Tests the distribution of objects between 2 nodes using the current
      * representatives (see SetRepresentative()).
      *
      * This method does not use the returnNode.
      *
      * @param node0 The first node.
      * @param node1 The second node.
      * @param metricEvaluator The metric evaluator to be used to compute distances.
      * @return The number of computed distances.
      */
      void TestDistribution(tDBMMemNode * node0, tDBMMemNode * node1,
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
      * Builds the distance matrix using the given metric evaluator.
      *
      * @param metricEvaluator The metric evaluator.
      * @return The number of computed distances.
      */
      stCount BuildDistanceMatrix(EvaluatorType * metricEvaluator);

      void MaxDistancePromote();

   private:

      /**
      * This type represents a DBMTree logic node entry.
      */
      struct stDBMLogicEntry{
         /**
         * Object.
         */
         ObjectType * Object;
   		
         #ifdef __stDBMHEIGHT__
            /**
            * Height of the tree.
            */
            stByte Height;
         #endif //__stDBMHEIGHT__

         /**
         * ID of the page.
         */
         stPageID PageID;

         /**
         * Number of entries in the subtree.
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
      struct stDBMLogicEntry * Entries;

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
      * The distance matrix.
      */
      tDistanceMatrix DMat;

      /**
      * Updates all distances between representatives and all objects in this
      * node. It returns the number of distances calculated.
      *
      * @param metricEvaluator The metric evaluator to be used.
      */
      stCount UpdateDistances(EvaluatorType * metricEvaluator);

};//end stDBMLogicNode















//=============================================================================
// Class template stSlimMSTSpliter
//-----------------------------------------------------------------------------
/**
* This class template implements the SlimTree MST split algorithm.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup dbmtree
*/
template <class ObjectType, class EvaluatorType>
class stDBMMSTSplitter{
   public:
      /**
      * This type defines the logic node for this class.
      */
      typedef stDBMLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type is a collection of objects.
      */
      typedef stDBMCollection < ObjectType > tDBMCollection;

      /**
      * This type is a subtree info.
      */
      typedef stSubtreeInfo <ObjectType> tSubtreeInfo;

      /**
      * Builds a new instance of this class. It will claim the ownership of the
      * logic node provided as input.
      */
      stDBMMSTSplitter(tLogicNode * node);

      /**
      * Disposes all associated resources.
      */
      ~stDBMMSTSplitter();

      /**
      * Provides access to the logic node.
      */
      const tLogicNode * GetLogicNode(){
         return Node;
      }//end GetLogicNode

      /**
      * Distributes objects between 2 nodes.
      *
      * @param node0 The first node.
      * @param node1 The second node.
      * @param promo Representatives of node 0 and 1.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      */
      stCount Distribute(stDBMNode * node0, stDBMNode * node1,
                         tSubtreeInfo * promo, EvaluatorType * metricEvaluator);

      /**
      * Distributes objects between 2 nodes and the returnCollection.
      *
      * @param returnCollection The returning node containing objects that were
      * not covered by the other 2 nodes.
      * @param node0 The first node.
      * @param node1 The second node.
      * @param promo Representatives of node 0 and 1.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      */
      stCount Distribute(tDBMCollection * returnCollection,
                         stDBMNode * node0, stDBMNode * node1,
                         tSubtreeInfo * promo, EvaluatorType * metricEvaluator);

   private:

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
      * Name of cluster 0.
      */
      int Cluster0;

      /**
      * Name of cluster 0.
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
      stCount BuildDistanceMatrix(EvaluatorType * metricEvaluator);

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
      
};//end stDBMMSTSplitter



















//=============================================================================
// Class template stNSpliter
//-----------------------------------------------------------------------------
/**
* This class template implements the N split algorithm.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup dbmtree
*/
template <class ObjectType, class EvaluatorType>
class stDBMNSplitter{
   public:
      /**
      * This type defines the logic node for this class.
      */
      typedef stDBMLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type is a collection of objects.
      */
      typedef stDBMCollection < ObjectType > tDBMCollection;

      /**
      * This type is a subtree info.
      */
      typedef stSubtreeInfo <ObjectType> tSubtreeInfo;

      /**
      * Builds a new instance of this class. It will claim the ownership of the
      * logic node provided as input.
      */
      stDBMNSplitter(tLogicNode * node, int numberOfClusters, stPageManager * pageman);

      /**
      * Disposes all associated resources.
      */
      ~stDBMNSplitter();

      /**
      * Provides access to the logic node.
      */
      const tLogicNode * GetLogicNode(){
         return Node;
      }//end GetLogicNode

      /**
      * Distributes objects between 2 nodes and returnCollection.
      *
      * @param oldNode The recycled node to be use in this method.
      * @param returnCollection The returning node containing objects that were
      * not covered by the others nodes.
      * @param promo Representatives of nodes.
      * @param metricEvaluator The metric evaluator to be used to compute
      * distances.
      */
      stCount Distribute(stDBMNode * oldNode,
                         tDBMCollection * returnCollection,
                         tSubtreeInfo * promo, EvaluatorType * metricEvaluator);

   private:

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

         stDistance Radius;
         int Center;
      };

      /**
      * The logic node to be used as source.
      */
      tLogicNode * Node;

      /**
      * The pagemanager.
      */
      stPageManager * myPageManager;

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

      stCount * idxRep;

      stDistance * radiusRep;

      /**
      * Total number of objects.
      */
      int N;

      /**
      * Total number of cluster.
      */
      int NumberOfClusters;

      /**
      * Name of clusters.
      */
      int * ClusterNames;

      int * ClusterNodes;

      /**
      * Returns the center of the object in the cluster clus.
      * The.
      *
      * @param clus Cluster id.
      */
      int FindCenter(int clus, stDistance & minRadius);

      /**
      * Builds the distance matrix using the given metric evaluator.
      *
      * @param metricEvaluator The metric evaluator.
      * @return The number of computed distances.
      */
      stCount BuildDistanceMatrix(EvaluatorType * metricEvaluator);

      /**
      * Performs the cluster algorithm. This method will split the objects in N
      * clusters. The result of the processing will be found at the array
      * Cluster.
      *
      * @warning DMat must be initialized.
      */
      stCount PerformCluster();
      stCount PerformClusterMerge();
      stCount PerformClusterRadiusTest();
      stCount PerformClusterRadius();

      bool IsRepresentative(int idx);

      /**
      * Joins 2 clusters. This method will insert custer2 into cluster1.
      *
      * <P>The state of cluster2 will change to DEATH_SENTENCE.
      *
      * @param cluster1 Cluster 1.
      * @param cluster2 Cluster 2.
      */
      void JoinClusters(int cluster1, int cluster2);
      stDistance TestJoinClusters(int cluster1, int cluster2);
      bool TestIntersection(stDistance finalRadius, int cluster1, int cluster2);

      void SetRepresentative(int idx, int rep);
      
};//end stDBMNSplitter















//=============================================================================
// Class template stDBMTree
//=============================================================================

/**
* This structure defines the DBMTree header structure. This type was left
* public to allow the creation of debug tools.
*
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup db
*/
typedef struct tDBMHeader{
   /**
   * Magic number. This is a short string that must contains the magic
   * string "DB". It may be used to validate the file (this feature
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
   * The root of the DBMTree
   */
   stPageID Root;

   /**
   * The Index of the Global Representative in the Root Node.
   */
   int idxRoot;

   /**
   * Minimum number of objects in a node.
   */
   stCount MaxOccupation;

   /**
   * Minimum percentage of objects in a node.
   */
   double MinOccupation;

   /**
   * The total number of objects int the DBM-tree.
   */
   stCount ObjectCount;

   /**
   * Total number of nodes.
   */
   stSize NodeCount;

   /**
   * The Radius of the DBM-Tree
   */
   stDistance TreeRadius;

   /**
   * If true, the reinsertion of object is done.
   */
   bool ReInsertObjects;

   /**
   * If true, remove farthest object to put new ones is perfomed.
   */
   bool RemoveFarthestObject;

   /**
   * The maximum number of promoted representatives.
   */
   stCount NumberOfClusters;

   /**
   * The height of the DBM-tree
   */
   stCount Height;

}stDBMHeader;

//=============================================================================
// Class template stDBMTree
//-----------------------------------------------------------------------------
/**
* This class defines all behavior of the DBMTree.
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
* @todo Finish the implementation.
* @version 1.0
* @ingroup db
*/
template <class ObjectType, class EvaluatorType>
class stDBMTree: public stMetricTree <ObjectType, EvaluatorType> {
   public:

      /**
      * These constants are used to define the choose sub tree method.
      */
      enum tChooseMethod{

         /**
         * Choose the nearest subtree to the new object without 
         * increments the radius of the subtree.
         */
         cmMINDIST,

         /**
         * Choose the nearest object to the new object. It
         * is possible to create a node to store the new object and increase
         * the cover area.
         */
         cmMINDISTWITHCOVER,
         
         /**
         * Choose the nearest subtree that covers the new object. It
         * is possible to increments the radius of the subtree but does not
         * generate cover area.
         */
         cmMINDISTWITHOUTCOVER
      };//end tChooseMethod

      /**
      * These constants are used to define the split method.
      * @todo Update documentation of each constant.
      */
      enum tSplitMethod {
         /**
         * Choose two representatives that generates the minimum radius
         * from the greater.
         */
         smMIN_MAX,

         /**
         * Choose two representatives that generates the minimum sum radius.
         */
         smMIN_SUM,

         /**
         * Choose two representatives that generates the minimum
         * number of free objets.
         */
         smMIN_FREE,

         /**
         * Choose two representatives that generates the minimum cover region.
         */
         smMIN_COVER,

         /**
         * Choose two representatives that generates the maximum radius.
         */
         smMAX_DISTANCE,

         /**
         * This is the Minimum Spanning Tree of Slim-tree.
         */
         smMST_SLIM,

         /**
         * This is the Minimum Spanning Tree with returnCollection.
         */
         smMST_DB,

         smN_SPLIT
      };//end tSplitMethod

      /**
      * This is the class that abstracs an result set.
      */
      typedef stResult <ObjectType> tResult;

      /**
      * This type is a collection of objects.
      */
      typedef stDBMCollection < ObjectType > tDBMCollection;

      /**
      * This type is a memory node.
      */
      typedef stDBMMemNode < ObjectType > tDBMMemNode;

      /**
      * This type is a memory node to execute the slim-down algorithm.
      */
      typedef stDBMShrinkNode < ObjectType > tDBMShrinkNode;

      /**
      * This type is a subtree info.
      */
      typedef stSubtreeInfo <ObjectType> tSubtreeInfo;

      /**
      * Creates a new metric tree using a given page manager. This instance will
      * not claim the ownership of the given page manager. It means that the
      * application must dispose the page manager when it is no loger necessary.
      *
      * @param pageman The bage manager to be used by this metric tree.
      */
      stDBMTree(stPageManager * pageman);

      /**
      * Dispose all used resources, ie, it is the destructor method.
      *
      * @see stDBMTree()
      */
      virtual ~stDBMTree();

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
         #ifdef __stDBMHEIGHT__
            return Header->Height;
         #else
            return this->GetRealHeight();
         #endif //__stDBMHEIGHT__
      }//end GetHeight

      /**
      * Returns the radius of the tree.
      */
      stDistance GetTreeRadius(){
         return Header->TreeRadius;
      }//end GetTreeRadius

      /**
      * Returns if the split method has already been called.
      */
      bool IsMinOccupationMapped(){
         return MinOccupationMapped;
      }//end IsMinOccupationMapped

      /**
      * Returns the MaxOccupation of the nodes.
      */
      stCount GetMaxOccupation(){
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
      * Sets the MinOccupation of the node.
      */
      virtual void SetMinOccupation(double MinOccup){
         Header->MinOccupation = MinOccup;
         // the occupation has already mapped.
         this->MinOccupationMapped = true;
         HeaderUpdate = true;
      }//end SetMinOccupation

      /**
      * Returns the number of objetcs of this tree.
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

      // Tree Configuration
      stCount GetNumberOfClusters(){
         return Header->NumberOfClusters;
      }//end GetNumberOfClusters

      void SetNumberOfClusters(stCount cluster){
         Header->NumberOfClusters = cluster;
      }//end SetNumberOfClusters

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
      * Returns the greatest estimated distance between two objects.
      */
      stDistance GetGreaterEstimatedDistance();

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

      /**
      * This method returns if this tree reinsert objects that are covered
      * by the subtrees of a node.
      *
      * @return True if the reinsert is on or false otherwise.
      */
      bool IsReInsert(){
         return Header->ReInsertObjects;
      }//end IsReInsert

      /**
      * This method sets if this tree will reinsert objects that are covered
      * by the subtrees of a node.
      *
      * @param flag the value to be set.
      */
      void SetReInsert(bool flag){
         Header->ReInsertObjects = flag;
         HeaderUpdate = true;
         if (flag && (this->reInsertCollection == NULL)){
            this->reInsertCollection = new tDBMCollection();
         }//end if
         if (!flag && (this->reInsertCollection != NULL)){
            delete this->reInsertCollection;
            this->reInsertCollection = NULL;
         }//end if
      }//end SetReInsert

      /**
      * This method returns if this tree remove the farthest object
      * when there is an object with greater distance than the new object and
      * no space in a node to add the new object.
      *
      * @return True if this method can remove object to insert new ones
      * or false otherwise.
      */
      bool IsRemoveFarthest(){
         return Header->RemoveFarthestObject;
      }//end IsRemoveFarthest

      /**
      * This method sets if this tree remove the farthest object
      * when there is an object with greater distance than the new object and
      * no space in a node to add the new object.
      *
      * @param flag the value to be set.
      */
      void SetRemoveFarthest(bool flag){
         Header->RemoveFarthestObject = flag;
         HeaderUpdate = true;
      }//end SetRemoveFarthest

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
      #endif // __stDEBUG__

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
      }

      /**
      * load this DBM-tree with N data using the BulkLoad algorithm
      * @param data is an array of n entries
      * @param padFactor is the maximum node utilization (use 1)
      * /
      void BulkLoad(ObjectType ** data, long numberOfObjects); /**/

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
      tResult * NearestQueryPriorityQueue(ObjectType * sample, stCount k, bool tie = false);

      /**
      * This method will perform a Incremental K-Nearest Neighbor query.
      */
      tResult * IncrementalNearestQuery(ObjectType * sample, stCount k, bool tie = false);

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
      */
      virtual void Optimize();

      /**
      * This method will delete any node that has only one entry.
      *
      */
      void DeleteUniqueNodes();

      /**
      * This method travels through the tree gathering information about the
      * number of objects in the tree.
      */
      stCount GetRealObjectsCount();

      /**
      * This method travels through the tree validating the hole tree.
      */
      bool Consistency();

   private:

      /*
      * Set of objects to be insert in the upper levels.
      */                                       
      tDBMCollection * returnCollection;

      /*
      * Set of objects to be reinsert in the tree.
      */
      tDBMCollection * reInsertCollection;

      /**
      * This type defines the logic node for this class.
      */
      typedef stDBMLogicNode < ObjectType, EvaluatorType > tLogicNode;

      /**
      * This type defines the MST splitter for this class.
      */
      typedef stDBMMSTSplitter < ObjectType, EvaluatorType > tMSTSplitter;

      /**
      * This type defines the MST splitter for this class.
      */
      typedef stDBMNSplitter < ObjectType, EvaluatorType > tDBMNSplitter;

      /**
      * This type is used by the priority key.
      */
      typedef stRPriorityQueue < stDistance, stCount > tPriorityQueue;

      /**
      * This type is used by the priority key.
      */
      typedef stDynamicRPriorityQueue < stDistance, stQueryPriorityQueueValue > tDynamicPriorityQueue;

      /**
      * This type is used by the priority key.
      */
      typedef stGenericPriorityHeap < ObjectType > tPGenericHeap;

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
         PROMOTION,

         /**
         * Farthest removed.
         */
         REMOVE_FARTHEST,

         /**
         * The node was removed.
         */
         REMOVED_NODE
      };//end stInsertAction

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
      * If true, the split method has already been called.
      */
      bool MinOccupationMapped;

      /**
      * If true, the header mus be written to the page manager.
      */
      bool HeaderUpdate;

      /**
      * The DBMTree header. This variable points to data in the HeaderPage.
      */
      stDBMHeader * Header;

      /**
      * Pointer to the header page.
      * The DBMTree keeps this page in memory for faster access.
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
      
      #ifndef __stDEBUG__
      /**
      * Get root page id.
      *
      * @warning This method is public only if __stDEBUG__ is defined at compile
      * time.
      */
      stPageID GetRoot(){
         return this->Header->Root;
      }//end GetRoot
      #endif // __stDEBUG__

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
      * @param node the Node to be analyzed
      * @param newSubTree The object that will be inserted.
      * @return the minIndex the index of the choose of the subTree
      */
      int ChooseSubTree(stDBMNode * node, tSubtreeInfo newSubTree);

      /**
      * This is a strategy of promoting entries of a page p based on the minimum
      * of the greater radius.
      *
      * @param node The node.
      */
      void MinMaxRadiusPromote(tLogicNode * node);

      /**
      * This is a strategy of promoting entries of a page p based on the minimum
      * sum of the two radius.
      *
      * @param node The node.
      */
      void MinSumRadiusPromote(tLogicNode * node);

      /**
      * This is a strategy of promoting entries of a page p based on the minimum
      * of the greater radius.
      *
      * @param node The node.
      */
      void MinMaxRadiusPromoteSlim(tLogicNode * node);

      /**
      * This is a strategy of promoting entries of a page p based on the minimum
      * sum of the two radius.
      *
      * @param node The node.
      */
      void MinSumRadiusPromoteSlim(tLogicNode * node);

      /**
      * This is a strategy of promoting entries of a page p based on minimum
      * of free objects.
      *
      * @param node The node.
      */
      void MinFreeRadiusPromote(tLogicNode * node);

      /**
      * This is a strategy of promoting entries of a page p based on the minimum
      * cover region.
      *
      * @param node The node.
      */
      void MinCoverPromote(tLogicNode * node);

      /**
      * This is a strategy of promoting entries of a page p based on the maximum
      * distance of the representatives.
      *
      * @param node The node.
      */
      void MaxDistancePromote(tLogicNode * node);

      /**
      * Checks to see it a given node ID is the current root.
      *
      * @return True it the given nodeID is the root or false otherwise.
      */
      bool IsRoot(stPageID nodeID){
         return nodeID == this->Header->Root;
      }//end IsRoot

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
      *           - promo1.NEntries will have the new subtree number of entries.
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
      *     - REMOVE_FARTHEST:
      *           - promo1 will contain the information about the changed subtree.
      *                 - promo1.Rep is NULL.
      *           - if the reInsertCollection is used, it will have the farthest
      *             object/subtree removed. Otherwise, the object/subtree removed
      *             will have in newSubTree
      *
      * @param currNodeID Current node ID.
      * @param newObj The new object to be inserted. This instance will never
      * be destroyed.
      * @param repObj The representative object for this node. This instance
      * will never be destroyed.
      * @param farthest Information about the farthest object/subtree (returning value).
      * @param promo Information about the choosen subtrees (returning value).
      *
      * @return The action to be taken after the returning. See enum
      * stInsertAction for more details.
      */
      int InsertRecursive(stPageID currNodeID, tSubtreeInfo newSubTree,
            ObjectType * repObj, tSubtreeInfo & farthest,
            tSubtreeInfo * promo);

      /**
      * This method reinserts objects into the tree recursively if one level
      * has objects that are not covered by subtrees.
      *
      * @param node Node to be do analize.
      * @warning This method will make slowly the action of insert objects.
      */
      void ReInsert(stDBMNode * node);

      /**
      * This method reinserts any thing into the tree recursively if one level
      * has objects that are not covered by subtrees.
      *
      * @param node Node to be do analize.
      * @warning This method will make slowly the action of insert objects.
      */
      void ReInsert(stDBMNode * node, stCount idxNewObj);

      /**
      * This method removes an object in a node to put a new one.
      *
      * <P>The removed object has distance greater than the new one and the
      * node does not have sufficient space to store the new one.
      *
      * @warning This method will make slowly the action of insertion objects.
      */
      bool RemoveFarthest(stDBMNode * node, tSubtreeInfo newSubTree,
                          tSubtreeInfo & farthest);

      /**
      * Creates and updates the new root of the DBMTree.
      *
      * @param promo the information about the representatives.
      */
      void AddNewRoot(tSubtreeInfo * promo);

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
      * @param promo promoted subtrees.
      */
      void Split(stDBMNode * oldNode, tSubtreeInfo * promo);

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
      void NearestQuery(tResult * result, ObjectType * sample, stDistance rangeK,
                        stCount k, tDynamicPriorityQueue * queue);
      void NearestQueryPriorityQueue(tResult * result, ObjectType * sample, stDistance rangeK,
                        stCount k, tDynamicPriorityQueue * queue);

      /**
      * This method will perform a Incremental K-Nearest Neighbor query.
      */
      void IncrementalNearestQuery(tResult * result, ObjectType * sample,
                                   stCount k, tPGenericHeap * queue);

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
      void PointQuery(tResult * result, ObjectType * sample, tDynamicPriorityQueue * queue);

      /**
      * Updates the distances of the objects from the new representative.
      */
      void UpdateDistances(stDBMNode * node, ObjectType * repObj, int repObjIdx);

      /**
      * Updates the distances of the objects from the new representative.
      */
      stCount ChooseRepresentative(stDBMNode * node);

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

      /**
      * This method travels through the tree gathering information about the
      * number of objects of the subtree.
      *
      * @param pageID Root of this sub-tree.
      */
      stCount GetRealObjectsCount(stPageID pageID);
      
      /**
      * This method travels through the tree gathering information about the
      * height of the tree.
      */
      stCount GetRealHeight();

      /**
      * This method travels through the tree gathering information about the
      * height of a subtree.
      *
      * @param pageID Root of this sub-tree.
      */
      stCount GetRealHeight(stPageID pageID);

      /**
      * This method travels through the subtree validating the hole tree.
      *
      * @param pageID Root of this sub-tree.
      */
      bool Consistency(stPageID pageID, ObjectType * repObj, stDistance & distance);

      /**
      * This method will delete any node that has only one entry.
      *
      * @param pageID Root of the subtree.
      * @return The new pageID of the subtree.
      */
      stPageID DeleteUniqueNodes(stPageID pageID);

      // Shrink implementaion
      /**
      * This method performs the Slim-Down in the whole tree recursively. I
      * starts from the root traveling through the tree in post order. Every
      * time it reaches an index node in the second level from bottom (the
      * index node above the leafs) it calls the method Shrink() to perform
      * a local slim down. The radius of each subtree is updated during the 
      * proccess to keep the tree consistent.
      *
      * @param pageID Root of the subtree.
      * @param level Current level (the first call must be 0).
      * @return The new radius of the subtree.
      */
      stDistance ShrinkRecursive(stPageID pageID);

      /**
      * This method performs the local Shrink in the given subtree.
      *
      * @param pageID The subtree root.
      */
      void Shrink(stPageID pageID);

      /**
      * This method performs the local Shrink in the given subtree.
      * It is called only if there is a single entry and a single subtree.
      *
      * @param pageID The subtree root.
      */
      void ShrinkRing(stPageID pageID);

      /**
      * Perform the Shrink in a set of stSlimMemLeafNode.
      *
      * @param memNodes nodes.
      * @param nodeCount Number of nodes in memNodes.
      * @param maxSwaps Swap limit.
      */
      void LocalShrink(tDBMShrinkNode ** memNodes, stCount nodeCount,
                       stCount maxSwaps);

      /**
      * Perform the Shrink in a set of stSlimMemLeafNode.
      * It is called only if there is a single entry and a single subtree.
      *
      * @param memLeafNodes Leaf nodes.
      * @param nodeCount Number of nodes in memNodes.
      * @param maxSwaps Swap limit.
      */
      void LocalShrinkRing(tDBMShrinkNode * memNodeLevel1,
                           tDBMShrinkNode * memNodeLevel2);

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
      bool ShrinkCanSwap(tDBMShrinkNode * src, tDBMShrinkNode * dst,
            stDistance & distance);

};//end stDBMTree

// Include implementation
#include "stDBMTree.cc"

#endif //__stDBMTREE_H

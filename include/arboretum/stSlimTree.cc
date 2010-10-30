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
* This file is the implementation of stSlimTree methods.
*
* @version 1.0
* $Revision: 1.156 $
* $Date: 2005/03/13 19:32:22 $
*
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
* @author Adriano Siqueira Arantes (arantes@icmc.usp.br)
*/
// Copyright (c) 2003 GBDI-ICMC-USP

//==============================================================================
// Class stSlimLogicNode
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stSlimLogicNode<ObjectType, EvaluatorType>::stSlimLogicNode(stCount maxOccupation){

   // Allocate resources
   MaxEntries = maxOccupation;
   Entries = new stSlimLogicEntry[MaxEntries];

   // Init Rep
   RepIndex[0] = 0;
   RepIndex[1] = 0;

   // Initialize
   Count = 0;

   // Minimum occupation. 25% is the default of Slim-tree
   MinOccupation = (stCount) (0.25 * maxOccupation);
   // At least the nodes must store 2 objects.
   if ((MinOccupation > (maxOccupation/2)) || (MinOccupation == 0)){
      MinOccupation = 2;
   }//end if
}//end stSlimLogicNode<ObjectType, EvaluatorType>::stSlimLogicNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stSlimLogicNode<ObjectType, EvaluatorType>::~stSlimLogicNode(){
   stCount i;

   if (Entries != NULL){
      for (i = 0; i < Count; i++){
         if ((Entries[i].Object != NULL) && (Entries[i].Mine)){
            delete Entries[i].Object;
         }//end if
      }//end for
   }//end if
   // Clean before exit.
   delete[] Entries;
}//end stSlimLogicNode<ObjectType, EvaluatorType>::~stSlimLogicNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stSlimLogicNode<ObjectType, EvaluatorType>::AddEntry(stSize size, const stByte * object){
   if (Count < MaxEntries){
      Entries[Count].Object = new ObjectType();
      Entries[Count].Object->Unserialize(object, size);
      Entries[Count].Mine = true;
      Count++;
      return Count - 1;
   }else{
      return -1;
   }//end if
}//end stSlimLogicNode<ObjectType, EvaluatorType>::AddEntry

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimLogicNode<ObjectType, EvaluatorType>::AddIndexNode(stSlimIndexNode * node){
   stCount i;
   int idx;

   for (i = 0; i < node->GetNumberOfEntries(); i++){
      idx = AddEntry(node->GetObjectSize(i), node->GetObject(i));
      SetEntry(idx, node->GetIndexEntry(i).PageID,
                    node->GetIndexEntry(i).NEntries,
                    node->GetIndexEntry(i).Radius);
   }//end for

   // Node type
   NodeType = stSlimNode::INDEX;
}//end stSlimLogicNode<ObjectType, EvaluatorType>::AddIndexNode
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimLogicNode<ObjectType, EvaluatorType>::AddLeafNode(stSlimLeafNode * node){
   stCount i;
   int idx;

   for (i = 0; i < node->GetNumberOfEntries(); i++){
      idx = AddEntry(node->GetObjectSize(i), node->GetObject(i));
      SetEntry(idx, 0, 0, 0);
   }//end for

   // Node type
   NodeType = stSlimNode::LEAF;
}//end stSlimLogicNode<ObjectType, EvaluatorType>::AddLeafNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stSlimLogicNode<ObjectType, EvaluatorType>::TestDistribution(
      stSlimIndexNode * node0, stSlimIndexNode * node1,
      EvaluatorType * metricEvaluator){
   stCount dCount;
   stCount i;
   int idx;
   int l0, l1;
   int currObj;
   stDistanceIndex * idx0, * idx1;

   // Setup Objects
   dCount = UpdateDistances(metricEvaluator);

   // Init Map and Sorting vector
   idx0 = new stDistanceIndex[Count];
   idx1 = new stDistanceIndex[Count];
   for (i = 0; i < Count; i++){
      idx0[i].Index = i;
      idx0[i].Distance = Entries[i].Distance[0];
      idx1[i].Index = i;
      idx1[i].Distance = Entries[i].Distance[1];
      Entries[i].Mapped = false;
   }//end for

   // Sorting by distance...
   sort(idx0, idx0 + Count);
   sort(idx1, idx1 + Count);

   // Make one of then get the minimum occupation.
   l0 = l1 = 0;

   // Adds at least MinOccupation objects to each node.
   for (i = 0; i < MinOccupation; i++){
      // Find a candidate for node 0
      while (Entries[idx0[l0].Index].Mapped){
         l0++;
      }//end while
      // Add to node 0
      currObj = idx0[l0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());

      // Test if the new object was inserted.
      if (idx >= 0){
         // Ok. Inserted into node1. Fill the others filds.
         node0->GetIndexEntry(idx).Distance = idx0[l0].Distance;
         node0->GetIndexEntry(idx).PageID = Entries[currObj].PageID;
         node0->GetIndexEntry(idx).NEntries = Entries[currObj].NEntries;
         node0->GetIndexEntry(idx).Radius = Entries[currObj].Radius;
      }else{
         // Oops. There is an error during the insertion.
         // The Node has not sufficient space to store this object.
         #ifdef __stDEBUG__
            cout << "The page size is too small. Increase it!\n";
         #endif //__stDEBUG__
         // Throw an exception.
         throw page_size_error("The page size is too small.");
      }//end if

      // Find a candidate for node 1
      while (Entries[idx1[l1].Index].Mapped){
         l1++;
      }//end while
      // Add to node 1
      currObj = idx1[l1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());
      // Test if the new object was inserted.
      if (idx >= 0){
         // Ok. Inserted into node1. Fill the others filds.
         node1->GetIndexEntry(idx).Distance = idx1[l1].Distance;
         node1->GetIndexEntry(idx).PageID = Entries[currObj].PageID;
         node1->GetIndexEntry(idx).NEntries = Entries[currObj].NEntries;
         node1->GetIndexEntry(idx).Radius = Entries[currObj].Radius;
      }else{
         // Oops. There is an error during the insertion.
         // The Node has not sufficient space to store this object.
         #ifdef __stDEBUG__
            cout << "The page size is too small. Increase it!\n";
         #endif //__stDEBUG__
         // Throw an exception.
         throw page_size_error("The page size is too small.");
      }//end if
   }//end for

   // Distribute the others.
   for (i = 0; i < Count; i++){
      if (Entries[i].Mapped == false){
         Entries[i].Mapped = true;
         if (Entries[i].Distance[0] < Entries[i].Distance[1]){
            // Try to put on node 0 first
            idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object->Serialize());
            if (idx >= 0){
               node0->GetIndexEntry(idx).Distance = Entries[i].Distance[0];
               node0->GetIndexEntry(idx).PageID = Entries[i].PageID;
               node0->GetIndexEntry(idx).NEntries = Entries[i].NEntries;
               node0->GetIndexEntry(idx).Radius = Entries[i].Radius;
            }else{
               // Let's put it in the node 1 since it doesn't fit in the node 0
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node1->GetIndexEntry(idx).Distance = Entries[i].Distance[1];
               node1->GetIndexEntry(idx).PageID = Entries[i].PageID;
               node1->GetIndexEntry(idx).NEntries = Entries[i].NEntries;
               node1->GetIndexEntry(idx).Radius = Entries[i].Radius;
            }//end if
         }else{
            // Try to put on node 1 first
            idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object->Serialize());
            if (idx >= 0){
               node1->GetIndexEntry(idx).Distance = Entries[i].Distance[1];
               node1->GetIndexEntry(idx).PageID = Entries[i].PageID;
               node1->GetIndexEntry(idx).NEntries = Entries[i].NEntries;
               node1->GetIndexEntry(idx).Radius = Entries[i].Radius;
            }else{
               // Let's put it in the node 0 since it doesn't fit in the node 1
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node0->GetIndexEntry(idx).Distance = Entries[i].Distance[0];
               node0->GetIndexEntry(idx).PageID = Entries[i].PageID;
               node0->GetIndexEntry(idx).NEntries = Entries[i].NEntries;
               node0->GetIndexEntry(idx).Radius = Entries[i].Radius;
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete[] idx0;
   delete[] idx1;

   return dCount;
}//end stSlimLogicNode<ObjectType, EvaluatorType>::TestDistribution

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stSlimLogicNode<ObjectType, EvaluatorType>::TestDistribution(
      stSlimLeafNode * node0, stSlimLeafNode * node1,
      EvaluatorType * metricEvaluator){
   stCount dCount;
   stCount i;
   int idx;
   int l0, l1;
   int currObj;
   stDistanceIndex * idx0, * idx1;

   // Setup Objects
   dCount = UpdateDistances(metricEvaluator);

   // Init Map and Sorting vector
   idx0 = new stDistanceIndex[Count];
   idx1 = new stDistanceIndex[Count];
   for (i = 0; i < Count; i++){
      idx0[i].Index = i;
      idx0[i].Distance = Entries[i].Distance[0];
      idx1[i].Index = i;
      idx1[i].Distance = Entries[i].Distance[1];
      Entries[i].Mapped = false;
   }//end for

   // Sorting by distance...
   sort(idx0, idx0 + Count);
   sort(idx1, idx1 + Count);

   // Make one of then get the minimum occupation.
   l0 = l1 = 0;

   // Adds at least MinOccupation objects to each node.
   for (i = 0; i < MinOccupation; i++){
      // Find a candidate for node 0
      while (Entries[idx0[l0].Index].Mapped){
         l0++;
      }//end while
      // Add to node 0
      currObj = idx0[l0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());
      node0->GetLeafEntry(idx).Distance = idx0[l0].Distance;

      // Find a candidate for node 1
      while (Entries[idx1[l1].Index].Mapped){
         l1++;
      }//end while
      // Add to node 1
      currObj = idx1[l1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());
      node1->GetLeafEntry(idx).Distance = idx1[l1].Distance;
   }//end for

   // Distribute the others.
   for (i = 0; i < Count; i++){
      if (Entries[i].Mapped == false){
         Entries[i].Mapped = true;
         if (Entries[i].Distance[0] < Entries[i].Distance[1]){
            // Try to put on node 0 first
            idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object->Serialize());
            if (idx >= 0){
               node0->GetLeafEntry(idx).Distance = Entries[i].Distance[0];
            }else{
               // Let's put it in the node 1 since it doesn't fit in the node 0
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node1->GetLeafEntry(idx).Distance = Entries[i].Distance[1];
            }//end if
         }else{
            // Try to put on node 1 first
            idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object->Serialize());
            if (idx >= 0){
               node1->GetLeafEntry(idx).Distance = Entries[i].Distance[1];
            }else{
               // Let's put it in the node 0 since it doesn't fit in the node 1
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node0->GetLeafEntry(idx).Distance = Entries[i].Distance[0];
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete idx0;
   delete idx1;

   return dCount;
}//end stSlimLogicNode<ObjectType, EvaluatorType>::TestDistribution

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stSlimLogicNode<ObjectType, EvaluatorType>::UpdateDistances(
      EvaluatorType * metricEvaluator){
   stCount i;

   for (i = 0; i < Count; i++){
      if (i == RepIndex[0]){
         Entries[i].Distance[0] = 0;
         Entries[i].Distance[1] = MAXDOUBLE;
      }else if (i == RepIndex[1]){
         Entries[i].Distance[0] = MAXDOUBLE;
         Entries[i].Distance[1] = 0;
      }else{
         Entries[i].Distance[0] = metricEvaluator->GetDistance(
               Entries[RepIndex[0]].Object, Entries[i].Object);
         Entries[i].Distance[1] = metricEvaluator->GetDistance(
               Entries[RepIndex[1]].Object, Entries[i].Object);
      }//end if
   }//end for

   return (GetNumberOfEntries() * 2) - 2;
}//end stSlimLogicNode<ObjectType, EvaluatorType>::UpdateDistances

//=============================================================================
// Class template stSlimMSTSpliter
//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stSlimMSTSplitter<ObjectType, EvaluatorType>::stSlimMSTSplitter(
      tLogicNode * node){

   Node = node;
   N = Node->GetNumberOfEntries();

   // Dynamic fields
   Cluster = new tCluster[N];
   ObjectCluster = new int[N];

   // Matrix
   DMat.SetSize(N, N);
}//end stSlimMSTSplitter<ObjectType, EvaluatorType>::stSlimMSTSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stSlimMSTSplitter<ObjectType, EvaluatorType>::~stSlimMSTSplitter(){

   if (Node != NULL){
      delete Node;
   }//end if
   if (Cluster != NULL){
      delete[] Cluster;
   }//end if
   if (ObjectCluster != NULL){
      delete[] ObjectCluster;
   }//end if
}//end stSlimMSTSplitter<ObjectType, EvaluatorType>::stSlimMSTSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stSlimMSTSplitter<ObjectType, EvaluatorType>::BuildDistanceMatrix(
      EvaluatorType * metricEvaluator){
   int i;
   int j;

   for (i = 0; i < N; i++){
      DMat[i][i] = 0;
      for (j = 0; j < i; j++){
         DMat[i][j] = metricEvaluator->GetDistance(Node->GetObject(i),
                                                   Node->GetObject(j));
         DMat[j][i] = DMat[i][j];
      }//end for
   }//end for
   return ((1 - N) * N) / 2;
}//end stSlimMSTSplitter<ObjectType, EvaluatorType>::BuildDistanceMatrix

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stSlimMSTSplitter<ObjectType, EvaluatorType>::FindCenter(int clus){
   int i, j, center;
   stDistance minRadius, radius;

   minRadius = MAXDOUBLE;
   for (i = 0; i < N; i++){
      if (ObjectCluster[i] == clus){
         radius = -1;
         for (j = 0; j < N; j++){
            if ((ObjectCluster[j] == clus) && (radius < DMat[i][j])){
               radius = DMat[i][j];
            }//end if
         }//end for
         if (minRadius > radius){
            minRadius = radius;
            center = i;
         }//end if
      }//end if
   }//end for

   return center;
}//end  stSlimMSTSplitter<ObjectType, EvaluatorType>::FindCenter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimMSTSplitter<ObjectType, EvaluatorType>::PerformMST(){
   int i, j, k, l, cc, iBig, iBigOpposite, a, b , c;
   bool linksOk, flag;
   stDistance big;

   // Insert each object in its own cluster.
   cc = N;
   for (i = 0; i < N; i++){
      Cluster[i].Size = 1;
      Cluster[i].State = ALIVE;
      ObjectCluster[i] = i; // Add Object
   }//end for

   // Perform it until it reaches 2 clusters.
   while (cc > 2){
      // Find the minimum distance between a cluster and its nearest
      // neighbour (connections).
      for (i = 0; i < N; i++){
         if (Cluster[i].State != DEAD){
            Cluster[i].MinDist = MAXDOUBLE;
         }//end if
      }//end for
      for (i = 0; i < N; i++){
         k = ObjectCluster[i];
         // Locate the nearest
         for (j = 0; j < N; j++){
            if (ObjectCluster[j] != k){
               if (Cluster[k].MinDist > DMat[i][j]){
                  Cluster[k].MinDist = DMat[i][j];
                  Cluster[k].Src = i;
                  Cluster[k].Dst = j;
               }//end if
            }//end if
         }//end for
      }//end for
      linksOk = true;

      // Find the largest connection. It will also locate the oposite objects
      big = -1.0;
      iBig = 0;
      for (i = 1; i < N; i++){
         if ((Cluster[i].State != DEAD) && (big < Cluster[i].MinDist)){
            big = Cluster[i].MinDist;
            iBig = i;
         }//end if
      }//end for

      // Locate the iBigOpposite.
      iBigOpposite = iBig;
      for (i = 0; i < N; i++){
         if ((Cluster[i].State != DEAD) && (Cluster[i].Src == Cluster[iBig].Dst) &&
               (Cluster[i].Dst == Cluster[iBig].Src)){
            iBigOpposite = i;
         }//end if
      }//end for

      // Join clusters
      i = 0;
      while ((i < N) && (cc > 2)) {
         if ((i != iBig) && (i != iBigOpposite) && (Cluster[i].State != DEAD)){
            // Join cluster i and its nearest cluster.
            k = ObjectCluster[Cluster[i].Dst];
            flag = true;

            // Change the cluster of all objects of the dropped one to
            // the remaining one.
            for (j = 0; j < N; j++){
               if ((ObjectCluster[j] == k) &&
                     (ObjectCluster[j] != ObjectCluster[Cluster[i].Src])){
                  if ((cc == 3) && (flag)){
                     if (!linksOk){
                        // Force update.
                        i = j = N;
                     }else{
                        a = ObjectCluster[Cluster[iBig].Src];
                        b = ObjectCluster[Cluster[iBig].Dst];
                        c = k;
                        if ((c == a) || (c == b)){
                           c = ObjectCluster[Cluster[i].Src];
                        }//end if

                        if ((ObjectCluster[Cluster[c].Src] == a) ||
                              (ObjectCluster[Cluster[c].Dst] == a)){
                           if (Cluster[b].Size > Cluster[c].Size){
                              // Join C and A
                              JoinClusters(c, a);
                           }else{
                              // Join B and A
                              JoinClusters(b , a);
                           }//end if
                        }else{
                           if (Cluster[a].Size > Cluster[c].Size){
                              // Join C and B
                              JoinClusters(c, b);
                           }else{
                              // Join A and B
                              JoinClusters(a, b);
                           }//end if
                        }//end if
                        i = j = N;
                        cc--;
                     }//end if
                  }else{
                     Cluster[k].State = DEATH_SENTENCE;
                     ObjectCluster[j] = ObjectCluster[Cluster[i].Src];
                     if (flag){
                        Cluster[ObjectCluster[Cluster[i].Src]].Size +=
                              Cluster[k].Size;
                        cc--;
                        flag = false;
                        linksOk = false;
                     }//end if
                  }//end if
               }//end if
            }//end for
         }//end if
         // Update i for the next loop
         i++;
      }//end while

      // All clusters that are destiny of an edge has gone, integrated
      // into another one.
      for (i = 0; i < N; i++){
         if (Cluster[i].State == DEATH_SENTENCE){
            Cluster[i].State = DEAD;
         }//end if
      }//end for
   }//end while

   // Locate the name of the 2 clusters.
   Cluster0 = -1;
   Cluster1 = -1;
   for (i = 0; i < N; i++){
      if (Cluster[i].State == ALIVE){
         if (Cluster0 == -1){
            Cluster0 = i;
         }else if (Cluster1 == -1){
            Cluster1 = i;
         }//end if
      }//end if
   }//end for

   #ifdef __stDEBUG__
   // Linking missing objects
   for (i = 0; i < N; i++){
      if ((ObjectCluster[i] != Cluster0) && (ObjectCluster[i] != Cluster1)){
         throw logic_error("At least on object has no cluster.");
      }//end if
   }//end for
   #endif //__stDEBUG__

   // Representatives
   Node->SetRepresentative(FindCenter(Cluster0), FindCenter(Cluster1));
}//end stSlimMSTSplitter<ObjectType, EvaluatorType>::PerformMST
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stSlimMSTSplitter<ObjectType, EvaluatorType>::Distribute(
            stSlimIndexNode * node0, ObjectType * & rep0,
            stSlimIndexNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator){
   int dCount;
   int idx;
   int i;
   int objIdx;

   // Build Distance matrix
   dCount = BuildDistanceMatrix(metricEvaluator);

   //Perform MST
   PerformMST();

   // Add representatives first
   idx = node0->AddEntry(Node->GetRepresentative(0)->GetSerializedSize(),
                         Node->GetRepresentative(0)->Serialize());
   objIdx = Node->GetRepresentativeIndex(0);
   node0->GetIndexEntry(idx).Distance = 0.0;
   node0->GetIndexEntry(idx).Radius = Node->GetRadius(objIdx);
   node0->GetIndexEntry(idx).NEntries = Node->GetNEntries(objIdx);
   node0->GetIndexEntry(idx).PageID = Node->GetPageID(objIdx);

   idx = node1->AddEntry(Node->GetRepresentative(1)->GetSerializedSize(),
                         Node->GetRepresentative(1)->Serialize());
   objIdx = Node->GetRepresentativeIndex(1);
   node1->GetIndexEntry(idx).Distance = 0.0;
   node1->GetIndexEntry(idx).Radius = Node->GetRadius(objIdx);
   node1->GetIndexEntry(idx).NEntries = Node->GetNEntries(objIdx);
   node1->GetIndexEntry(idx).PageID = Node->GetPageID(objIdx);

   // Distribute us...
   for (i = 0; i < N; i++){
      if (!Node->IsRepresentative(i)){
         if (ObjectCluster[i] == Cluster0){
            idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize());
            if (idx >= 0){
               // Insertion Ok!
               node0->GetIndexEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(0)];
               node0->GetIndexEntry(idx).Radius = Node->GetRadius(i);
               node0->GetIndexEntry(idx).NEntries = Node->GetNEntries(i);
               node0->GetIndexEntry(idx).PageID = Node->GetPageID(i);
            }else{
               // Oops! We must put it in other node
               idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node1->GetIndexEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(1)];
               node1->GetIndexEntry(idx).Radius = Node->GetRadius(i);
               node1->GetIndexEntry(idx).NEntries = Node->GetNEntries(i);
               node1->GetIndexEntry(idx).PageID = Node->GetPageID(i);
            }//end if
         }else{
            idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize());
            if (idx >= 0){
               // Insertion Ok!
               node1->GetIndexEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(1)];
               node1->GetIndexEntry(idx).Radius = Node->GetRadius(i);
               node1->GetIndexEntry(idx).NEntries = Node->GetNEntries(i);
               node1->GetIndexEntry(idx).PageID = Node->GetPageID(i);
            }else{
               // Oops! We must put it in other node
               idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node0->GetIndexEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(0)];
               node0->GetIndexEntry(idx).Radius = Node->GetRadius(i);
               node0->GetIndexEntry(idx).NEntries = Node->GetNEntries(i);
               node0->GetIndexEntry(idx).PageID = Node->GetPageID(i);
            }//end if
         }//end if
      }//end if
   }//end for

   // Representatives
   rep0 = Node->BuyObject(Node->GetRepresentativeIndex(0));
   rep1 = Node->BuyObject(Node->GetRepresentativeIndex(1));

   return dCount;
}//end stSlimMSTSplitter<ObjectType, EvaluatorType>::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stSlimMSTSplitter<ObjectType, EvaluatorType>::Distribute(
            stSlimLeafNode * node0, ObjectType * & rep0,
            stSlimLeafNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator){
   int dCount;
   int idx;
   int i;

   // Build Distance matrix
   dCount = BuildDistanceMatrix(metricEvaluator);

   //Perform MST
   PerformMST();

   // Add representatives first
   idx = node0->AddEntry(Node->GetRepresentative(0)->GetSerializedSize(),
                         Node->GetRepresentative(0)->Serialize());
   node0->GetLeafEntry(idx).Distance = 0.0;
   idx = node1->AddEntry(Node->GetRepresentative(1)->GetSerializedSize(),
                         Node->GetRepresentative(1)->Serialize());
   node1->GetLeafEntry(idx).Distance = 0.0;

   // Distribute us...
   for (i = 0; i < N; i++){
      if (!Node->IsRepresentative(i)){
         if (ObjectCluster[i] == Cluster0){
            idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize());
            if (idx >= 0){
               // Insertion Ok!
               node0->GetLeafEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(0)];
            }else{
               // Oops! We must put it in other node
               idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node1->GetLeafEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(1)];
            }//end if
         }else{
            idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize());
            if (idx >= 0){
               // Insertion Ok!
               node1->GetLeafEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(1)];
            }else{
               // Oops! We must put it in other node
               idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node0->GetLeafEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(0)];
            }//end if
         }//end if
      }//end if
   }//end for

   // Representatives
   rep0 = Node->BuyObject(Node->GetRepresentativeIndex(0));
   rep1 = Node->BuyObject(Node->GetRepresentativeIndex(1));

   return dCount;
}//end stSlimMSTSplitter<ObjectType, EvaluatorType>::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimMSTSplitter<ObjectType, EvaluatorType>::JoinClusters(
      int cluster1, int cluster2){
   int i;

   for (i = 0; i < N; i++){
      if (ObjectCluster[i] == cluster2){
         ObjectCluster[i] = cluster1;
      }//end if
   }//end for
   Cluster[cluster1].Size += Cluster[cluster2].Size;
   Cluster[cluster2].State = DEATH_SENTENCE;
}//end stSlimMSTSplitter<ObjectType, EvaluatorType>::JoinClusters


//==============================================================================
// Class stSlimTree
//------------------------------------------------------------------------------

// This macro will be used to replace the declaration of
//       stSlimTree<ObjectType, EvaluatorType>
#define tmpl_stSlimTree stSlimTree<ObjectType, EvaluatorType>

template <class ObjectType, class EvaluatorType>
tmpl_stSlimTree::stSlimTree(stPageManager * pageman):
   stMetricTree<ObjectType, EvaluatorType>(pageman){

   // Initialize fields
   this->Header = NULL;
   this->HeaderPage = NULL;

   // Load header.
   this->LoadHeader();

   // Will I create or load the tree ?
   if (this->myPageManager->IsEmpty()){
      this->DefaultHeader();
   }//end if

   // Visualization support
   #ifdef __stMAMVIEW__
      MAMViewer = new tViewExtractor(this->myMetricEvaluator);
   #endif //__stMAMVIEW__
}//end stSlimTree<ObjectType, EvaluatorType>::stSlimTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
tmpl_stSlimTree::stSlimTree(stPageManager * pageman, EvaluatorType * metricEval):
   stMetricTree<ObjectType, EvaluatorType>(pageman, metricEval){

   // Initialize fields
   Header = NULL;
   HeaderPage = NULL;

   // Load header.
   LoadHeader();

   // Will I create or load the tree ?
   if (this->myPageManager->IsEmpty()){
      DefaultHeader();
   }//end if

   // Visualization support
   #ifdef __stMAMVIEW__
   MAMViewer = new tViewExtractor(this->myMetricEvaluator);
   #endif //__stMAMVIEW__
}//end stSlimTree<ObjectType, EvaluatorType>::stSlimTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
tmpl_stSlimTree::~stSlimTree(){

   // Flus header page.
   FlushHeader();

   // Visualization support
   #ifdef __stMAMVIEW__
   delete MAMViewer;
   #endif //__stMAMVIEW__
}//end stSlimTree<ObjectType, EvaluatorType>::~stSlimTree()

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::DefaultHeader(){

   // Clear header page.
   HeaderPage->Clear();

   // Default values
   Header->Magic[0] = 'S';
   Header->Magic[1] = 'L';
   Header->Magic[2] = 'I';
   Header->Magic[3] = 'M';
   Header->SplitMethod = smSPANNINGTREE;
   Header->ChooseMethod = cmMINOCCUPANCY;
   Header->CorrectMethod = crmOFF;
   Header->Root = 0;
   Header->MinOccupation = 0.25;
   Header->MaxOccupation = 0;
   Header->Height = 0;
   Header->ObjectCount = 0;
   Header->NodeCount = 0;

   // Notify modifications
   HeaderUpdate = true;
}//end stSlimTree<ObjectType, EvaluatorType>::DefaultHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::LoadHeader(){

   if (HeaderPage != NULL){
      this->myPageManager->ReleasePage(HeaderPage);
   }//end if

   // Load and set the header.
   HeaderPage = this->myPageManager->GetHeaderPage();
   if (HeaderPage->GetPageSize() < sizeof(stSlimHeader)){
      #ifdef __stDEBUG__
         cout << "The page size is too small. Increase it!\n";
      #endif //__stDEBUG__
      throw page_size_error("The page size is too small.");
   }//end if

   Header = (stSlimHeader *) HeaderPage->GetData();
   HeaderUpdate = false;
}//end stSlimTree<ObjectType, EvaluatorType>::LoadHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::FlushHeader(){

   if (HeaderPage != NULL){
      if (Header != NULL){
         WriteHeader();
      }//end if
      this->myPageManager->ReleasePage(HeaderPage);
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::FlushHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool tmpl_stSlimTree::Add(ObjectType *newObj){
   stSubtreeInfo promo1;
   stSubtreeInfo promo2;
   int insertIdx;

   // Is there a root ?
   if (this->GetRoot() == 0){
      // No! We shall create the new node.
      stPage * auxPage  = this->NewPage();
      stSlimLeafNode * leafNode = new stSlimLeafNode(auxPage, true);
      this->SetRoot(auxPage->GetPageID());

      // Insert the new object.
      insertIdx = leafNode->AddEntry(newObj->GetSerializedSize(),
                                     newObj->Serialize());
      // Test if the page size is too big to store an object.
      if (insertIdx < 0){
         // Oops. There is an error during the insertion.
         #ifdef __stDEBUG__
            cout << "The page size is too small for the first object. Increase it!\n";
            // Throw an exception.
            throw page_size_error("The page size is too small to store the first object.");
         #endif //__stDEBUG__
         // The new object was not inserted.
         return false;
      }else{
         // The new object was inserted.
         // It is the first object, fill the distance with zero.
         leafNode->GetLeafEntry(insertIdx).Distance = 0;
         // Update the Height
         Header->Height++;
         // Write the root node.
         this->myPageManager->WritePage(auxPage);
         delete leafNode;
      }//end if
   }else{
      // Let's continue our search for the grail!
      if (InsertRecursive(GetRoot(), newObj, NULL, promo1, promo2) == PROMOTION){
         // Split occurred! We must create a new root because it is required.
         // The tree will aacquire a new root.
         AddNewRoot(promo1.Rep, promo1.Radius, promo1.RootID, promo1.NObjects,
                    promo2.Rep, promo2.Radius, promo2.RootID, promo2.NObjects);
         delete promo1.Rep;
         delete promo2.Rep;
      }//end if
   }//end if

   // Update object count.
   UpdateObjectCounter(1);

   // Report the modification.
   HeaderUpdate = true;
   // Ok. The new object was inserted. Return success!
   return true;
}//end stSlimTree<ObjectType, EvaluatorType>::Add

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int tmpl_stSlimTree::ChooseSubTree(
      stSlimIndexNode * slimIndexNode, ObjectType * obj) {
   int idx;
   int j;
   int * cover;
   bool stop;
   stCount tmpNumberOfEntries;
   int numberOfEntries, minIndex = 0;

   ObjectType * objectType = new ObjectType;
   stDistance distance;
   stDistance minDistance = MAXDOUBLE; // Largest magnitude double value
   // Get the total number of entries.
   numberOfEntries = slimIndexNode->GetNumberOfEntries();
   idx = 0;

   switch (this->GetChooseMethod()){
      case stSlimTree::cmBIASED :
         // Find the first subtree that covers the new object.
         stop = (idx >= numberOfEntries);
         while (!stop){
            // Get the object from idx position from IndexNode
            objectType->Unserialize(slimIndexNode->GetObject(idx),
                                    slimIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // is this a subtree that covers the new object?
            if (distance < slimIndexNode->GetIndexEntry(idx).Radius) {
               minDistance = 0;     // the gain will be 0
               stop = true;         // stop the search.
               minIndex = idx;
            }else if (distance - slimIndexNode->GetIndexEntry(idx).Radius < minDistance) {
               minDistance = distance - slimIndexNode->GetIndexEntry(idx).Radius;
               minIndex = idx;
            }//end if
            idx++;
            // if one of the these condicions are true, stop this while.
            stop = stop || (idx >= numberOfEntries);
         }//end while
         break; //end stSlimTree::cmBIASED

      case stSlimTree::cmRANDOM :
         // allocate resources to cover.
         cover = new int[numberOfEntries];
         /* Find if there is some circle that contains obj */
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Get out the object from IndexNode.
            objectType->Unserialize(slimIndexNode->GetObject(idx),
                                    slimIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // Does this subtree is a qualified entry?
            if (distance < (slimIndexNode->GetIndexEntry(idx).Radius)){
               // Yes, add it.
               cover[idx] = 1;
            }else{
               // No, mark this entry.
               cover[idx] = 0;
            }//end if
         }//end for

         idx = numberOfEntries * random(RAND_MAX) / (RAND_MAX + 1);
         j = 0;
         minIndex = -1;

         while (minIndex < 0) {
            if ((cover[j]==1) && (--idx < 0))
               minIndex = j;
            if (++j >= numberOfEntries)
               j = 0;
         }//end while
         // clean cover.
         delete[] cover;
         break; // end stSlimTree::cmRANDOM

      case stSlimTree::cmMINDIST :
         /* Find if there is some circle that contains obj */
         stop = (idx >= numberOfEntries);
         while (!stop){
            //get out the object from IndexNode
            objectType->Unserialize(slimIndexNode->GetObject(idx),
                                    slimIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // find the first subtree that cover the new object.
            if (distance < slimIndexNode->GetIndexEntry(idx).Radius) {
               minDistance = distance;     // the gain will be 0
               stop = true;                // stop the search.
               minIndex = idx;
            }else if (distance - slimIndexNode->GetIndexEntry(idx).Radius < minDistance) {
               minDistance = distance - slimIndexNode->GetIndexEntry(idx).Radius;
               minIndex = idx;
            }//end if
            idx++;
            // if one of the these condicions are true, stop this while.
            stop = stop || (idx >= numberOfEntries);
         }//end while
         // Try to find a better entry.
         while (idx < numberOfEntries) {
            // Get out the object from IndexNode.
            objectType->Unserialize(slimIndexNode->GetObject(idx),
                                    slimIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            if ((distance < slimIndexNode->GetIndexEntry(idx).Radius) && (distance < minDistance)) {
               minDistance = distance;
               minIndex = idx;
            }//end if
            idx++;
         }//end while
         break; // end stSlimTree::cmMINDIST

      case stSlimTree::cmMINGDIST :
         // Find if there is some circle that contains obj
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Get out the object from IndexNode.
            objectType->Unserialize(slimIndexNode->GetObject(idx),
                                    slimIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            if (distance < minDistance) {
               minDistance = distance;
               minIndex = idx;
            }//end if
         }//end for
         break; //end stSlimTree::cmMINGDIST

      case stSlimTree::cmMINOCCUPANCY :
         /* Find if there is some circle that contains obj */
         tmpNumberOfEntries = MAXINT;
         // First try to find a subtree that covers the new object.
         stop = (idx >= numberOfEntries);
         while (!stop){
            //get out the object from IndexNode
            objectType->Unserialize(slimIndexNode->GetObject(idx),
                                    slimIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // find the first subtree that covers the new object.
            if (distance < slimIndexNode->GetIndexEntry(idx).Radius) {
               minDistance = distance;     // the gain will be 0
               stop = true;                // stop the search.
               minIndex = idx;
               tmpNumberOfEntries = slimIndexNode->GetIndexEntry(idx).NEntries;
            }else if (distance - slimIndexNode->GetIndexEntry(idx).Radius < minDistance) {
               minDistance = distance - slimIndexNode->GetIndexEntry(idx).Radius;
               minIndex = idx;
            }//end if
            idx++;
            // if one of the these condicions are true, stop this while.
            stop = stop || (idx >= numberOfEntries);
         }//end while

         while (idx < numberOfEntries) {
            // Get out the object from IndexNode
            objectType->Unserialize(slimIndexNode->GetObject(idx),
                                    slimIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);

            if ((distance < slimIndexNode->GetIndexEntry(idx).Radius) &&
                (slimIndexNode->GetIndexEntry(idx).NEntries < tmpNumberOfEntries)) {
               tmpNumberOfEntries = slimIndexNode->GetIndexEntry(idx).NEntries;
               minIndex = idx;
            }//end if
            idx++;
         }//end for
         break; //end stSlimTree::cmMINOCCUPANCY

   }//end switch

   delete objectType;

   return minIndex;
}//end stSlimTree<ObjectType, EvaluatorType>::ChooseSubTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::AddNewRoot(
      ObjectType * obj1, stDistance radius1, stPageID nodeID1, stCount nEntries1,
      ObjectType * obj2, stDistance radius2, stPageID nodeID2, stCount nEntries2){
   stPage * newPage;
   stSlimIndexNode * newRoot;
   int idx;

   // Debug mode!
   #ifdef __stDEBUG__
      if ((obj1 == NULL) || (obj2 == NULL)){
         throw invalid_argument("Invalid object.");
      }//end if
   #endif //__stDEBUG__

   // Create a new node
   newPage = this->NewPage();
   newRoot = new stSlimIndexNode(newPage, true);

   // Add obj1
   idx = newRoot->AddEntry(obj1->GetSerializedSize(), obj1->Serialize());
   newRoot->GetIndexEntry(idx).Distance = 0.0;
   newRoot->GetIndexEntry(idx).PageID = nodeID1;
   newRoot->GetIndexEntry(idx).Radius = radius1;
   newRoot->GetIndexEntry(idx).NEntries = nEntries1;

   // Add obj2
   idx = newRoot->AddEntry(obj2->GetSerializedSize(), obj2->Serialize());
   newRoot->GetIndexEntry(idx).Distance = 0.0;
   newRoot->GetIndexEntry(idx).PageID = nodeID2;
   newRoot->GetIndexEntry(idx).Radius = radius2;
   newRoot->GetIndexEntry(idx).NEntries = nEntries2;

   // Update tree
   Header->Height++;
   SetRoot(newRoot->GetPage()->GetPageID());
   this->myPageManager->WritePage(newPage);

   // Dispose page
   delete newRoot;
   this->myPageManager->ReleasePage(newPage);
}//end SlimTree::AddNewRoot

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int tmpl_stSlimTree::InsertRecursive(
      stPageID currNodeID, ObjectType * newObj, ObjectType * repObj,
      stSubtreeInfo & promo1, stSubtreeInfo & promo2){
   stPage * currPage;      // Current page
   stPage * newPage;       // New page
   stSlimNode * currNode;  // Current node
   stSlimIndexNode * indexNode; // Current index node.
   stSlimIndexNode * newIndexNode; // New index node for splits
   stSlimLeafNode * leafNode; // Current leaf node.
   stSlimLeafNode * newLeafNode; // New leaf node.
   int insertIdx;          // Insert index.
   int result;             // Returning value.
   stDistance dist;        // Temporary distance.
   int subtree;            // Subtree
   ObjectType * subRep;    // Subtree representative.

   // Read node...
   currPage = this->myPageManager->GetPage(currNodeID);
   currNode = stSlimNode::CreateNode(currPage);

   // What shall I do ?
   if (currNode->GetNodeType() == stSlimNode::INDEX){
      // Index Node cast.
      indexNode = (stSlimIndexNode *)currNode;

      // Where do I add it ?
      subtree = ChooseSubTree(indexNode, newObj);

      // Lets get the information about this tree.
      subRep = new ObjectType();
      subRep->Unserialize(indexNode->GetObject(subtree),
                          indexNode->GetObjectSize(subtree));

      // Try to insert...
      switch (InsertRecursive(indexNode->GetIndexEntry(subtree).PageID,
            newObj, subRep, promo1, promo2)){
         case NO_ACT: // Update Radius and count.
            indexNode->GetIndexEntry(subtree).NEntries++;
            indexNode->GetIndexEntry(subtree).Radius = promo1.Radius;

            // Returning status.
            promo1.NObjects = indexNode->GetTotalObjectCount();
            promo1.Radius = indexNode->GetMinimumRadius();
            result = NO_ACT;
            break;
         case CHANGE_REP: // Replace representative
            // Remove previous entry.
            indexNode->RemoveEntry(subtree);

            // Try to add the new entry...
            insertIdx = indexNode->AddEntry(promo1.Rep->GetSerializedSize(),
                                            promo1.Rep->Serialize());
            if (insertIdx >= 0){
               // Swap OK. Fill data.
               indexNode->GetIndexEntry(insertIdx).Radius = promo1.Radius;
               indexNode->GetIndexEntry(insertIdx).NEntries = promo1.NObjects;
               indexNode->GetIndexEntry(insertIdx).PageID = promo1.RootID;

               // Will it replace the representative ?
               // WARNING: Do not change the order of this checking.
               if ((repObj != NULL) && (repObj->IsEqual(subRep))){
                  // promo1.Rep is the new representative.
                  indexNode->GetIndexEntry(insertIdx).Distance = 0;

                  // Oops! We must propagate the representative change
                  // promo1.Rep will remain the same.
                  promo1.RootID = currNodeID;

                  // update distances between the new representative
                  // and the others
                  UpdateDistances(indexNode, promo1.Rep, insertIdx);

                  result = CHANGE_REP;
               }else{
                  // promo1.Rep is not the new representative.
                  if (repObj != NULL){
                     // Distance from representative is...
                     indexNode->GetIndexEntry(insertIdx).Distance =
                           this->myMetricEvaluator->GetDistance(repObj,
                                                                promo1.Rep);
                  }else{
                     // It is the root!
                     indexNode->GetIndexEntry(insertIdx).Distance = 0;
                  }//end if

                  // Cut it here
                  delete promo1.Rep; // promo1.rep will never be used again.
                  promo1.Rep = NULL;
                  result = NO_ACT;
               }//end if
               promo1.Radius = indexNode->GetMinimumRadius();
               promo1.NObjects = indexNode->GetTotalObjectCount();
            }else{
               // Split it!
               // New node.
               newPage = this->NewPage();
               newIndexNode = new stSlimIndexNode(newPage, true);

               // Split!
               SplitIndex(indexNode, newIndexNode,
                     promo1.Rep, promo1.Radius, promo1.RootID, promo1.NObjects,
                     NULL, 0, 0, 0,
                     repObj, promo1, promo2);

               // Write nodes
               this->myPageManager->WritePage(newPage);
               // Clean home.
               delete newIndexNode;
               this->myPageManager->ReleasePage(newPage);
               result = PROMOTION; //Report split.
            }//end if
            break;
         case PROMOTION: // Promotion!!!
            if (promo1.Rep == NULL){
               // Update subtree
               indexNode->GetIndexEntry(subtree).NEntries = promo1.NObjects;
               indexNode->GetIndexEntry(subtree).Radius = promo1.Radius;
               indexNode->GetIndexEntry(subtree).PageID = promo1.RootID;

               // Try to insert the promo2.Rep
               insertIdx = indexNode->AddEntry(promo2.Rep->GetSerializedSize(),
                                               promo2.Rep->Serialize());
               if (insertIdx >= 0){
                  // Swap OK. Fill data.
                  indexNode->GetIndexEntry(insertIdx).NEntries = promo2.NObjects;
                  indexNode->GetIndexEntry(insertIdx).Radius = promo2.Radius;
                  indexNode->GetIndexEntry(insertIdx).PageID = promo2.RootID;
                  // Update promo2 distance
                  if (repObj != NULL){
                     // Distance from representative is...
                     indexNode->GetIndexEntry(insertIdx).Distance =
                           this->myMetricEvaluator->GetDistance(repObj,
                                                                promo2.Rep);
                  }else{
                     // It is the root!
                     indexNode->GetIndexEntry(insertIdx).Distance = 0;
                  }//end if

                  //Update radius...
                  promo1.Radius = indexNode->GetMinimumRadius();
                  promo1.NObjects = indexNode->GetTotalObjectCount();
                  delete promo2.Rep;
                  result = NO_ACT;
               }else{
                  // Split it!
                  // New node.
                  newPage = this->NewPage();
                  newIndexNode = new stSlimIndexNode(newPage, true);

                  // Split!
                  SplitIndex(indexNode, newIndexNode,
                        promo2.Rep, promo2.Radius, promo2.RootID, promo2.NObjects,
                        NULL, 0, 0, 0,
                        repObj, promo1, promo2);

                  // Write nodes
                  this->myPageManager->WritePage(newPage);
                  // Clean home.
                  delete newIndexNode;
                  this->myPageManager->ReleasePage(newPage);
                  result = PROMOTION; //Report split.
               }//end if
            }else{
               // Remove the previous entry.
               indexNode->RemoveEntry(subtree);

               // Try to add the new entry...
               insertIdx = indexNode->AddEntry(promo1.Rep->GetSerializedSize(),
                                               promo1.Rep->Serialize());
               if (insertIdx >= 0){
                  // Swap OK. Fill data.
                  indexNode->GetIndexEntry(insertIdx).Radius = promo1.Radius;
                  indexNode->GetIndexEntry(insertIdx).NEntries = promo1.NObjects;
                  indexNode->GetIndexEntry(insertIdx).PageID = promo1.RootID;

                  // Will it replace the representative ?
                  // WARNING: Do not change the order of this checking.
                  if ((repObj != NULL) && (repObj->IsEqual(subRep))){
                     // promo1.Rep is the new representative.
                     indexNode->GetIndexEntry(insertIdx).Distance = 0;

                     // Oops! We must propagate the representative change
                     // promo1.Rep will remains the same,
                     promo1.RootID = currNodeID;

                     // update distances between the new representative
                     // and the others
                     UpdateDistances(indexNode, promo1.Rep, insertIdx);

                     result = CHANGE_REP;
                  }else{
                     // promo1.Rep is not the new representative.
                     if (repObj != NULL){
                        // Distance from representative is...
                        indexNode->GetIndexEntry(insertIdx).Distance =
                              this->myMetricEvaluator->GetDistance(repObj,
                                                                   promo1.Rep);
                     }else{
                        // It is the root!
                        indexNode->GetIndexEntry(insertIdx).Distance = 0;
                     }//end if

                     // Cut it here
                     delete promo1.Rep; // promo1.rep will never be used again.
                     promo1.Rep = NULL;
                     result = NO_ACT;
                  }//end if

                  // Try to add promo2
                  insertIdx = indexNode->AddEntry(promo2.Rep->GetSerializedSize(),
                                                  promo2.Rep->Serialize());
                  if (insertIdx >= 0){
                     // Swap OK. Fill data.
                     indexNode->GetIndexEntry(insertIdx).Radius = promo2.Radius;
                     indexNode->GetIndexEntry(insertIdx).NEntries = promo2.NObjects;
                     indexNode->GetIndexEntry(insertIdx).PageID = promo2.RootID;

                     // The new distance is...
                     if (promo1.Rep != NULL){
                        // Rep. changed...
                        // Distance from representative is...
                        indexNode->GetIndexEntry(insertIdx).Distance =
                              this->myMetricEvaluator->GetDistance(promo1.Rep,
                                                                   promo2.Rep);
                     }else{
                        // No change!
                        if (repObj != NULL){
                           // Distance from representative is...
                           indexNode->GetIndexEntry(insertIdx).Distance =
                                 this->myMetricEvaluator->GetDistance(repObj,
                                                                      promo2.Rep);
                        }else{
                           // It is the root!
                           indexNode->GetIndexEntry(insertIdx).Distance = 0;
                        }//end if
                     }//end if

                     delete promo2.Rep;
                     // set the number of objects to high levels.
                     promo1.NObjects = indexNode->GetTotalObjectCount();
                     // set the radius to high levels.
                     promo1.Radius = indexNode->GetMinimumRadius();
                  }else{
                     // Split it promo2.rep does not fit.
                     // New node.
                     newPage = this->NewPage();
                     newIndexNode = new stSlimIndexNode(newPage, true);

                     // Dispose promo1.rep it if exists because it will not be
                     // used again. It happens when result is CHANGE_REP.
                     if (promo1.Rep != NULL){
                        delete promo1.Rep;
                     }//end if

                     // Add promo2 and split!
                     SplitIndex(indexNode, newIndexNode,
                           promo2.Rep, promo2.Radius, promo2.RootID, promo2.NObjects,
                           NULL, 0, 0, 0, // Ignore this object
                           repObj, promo1, promo2);

                     // Write nodes
                     this->myPageManager->WritePage(newPage);
                     // Clean home.
                     delete newIndexNode;
                     this->myPageManager->ReleasePage(newPage);
                     result = PROMOTION; //Report split.
                  }//end if
               }else{
                  // Split it because both objects don't fit.
                  // New node.
                  newPage = this->NewPage();
                  newIndexNode = new stSlimIndexNode(newPage, true);

                  // Split!
                  SplitIndex(indexNode, newIndexNode,
                        promo1.Rep, promo1.Radius, promo1.RootID, promo1.NObjects,
                        promo2.Rep, promo2.Radius, promo2.RootID, promo2.NObjects,
                        repObj, promo1, promo2);

                  // Write nodes
                  this->myPageManager->WritePage(newPage);
                  // Clean home.
                  delete newIndexNode;
                  this->myPageManager->ReleasePage(newPage);
                  result = PROMOTION; //Report split.
               }//end if
            }//end if
      };//end switch

      // Clear the mess.
      delete subRep;
   }else{
      // Leaf node cast.
      leafNode = (stSlimLeafNode *) currNode;

      // Try to insert...
      insertIdx = leafNode->AddEntry(newObj->GetSerializedSize(),
                                     newObj->Serialize());
      if (insertIdx >= 0){
         // Don't split!
         // Calculate distance and verify if it is a new radius!
         if (repObj == NULL){
            dist = 0;
         }else{
            dist = this->myMetricEvaluator->GetDistance(newObj, repObj);
         }//end if

         // Fill entry's fields
         leafNode->GetLeafEntry(insertIdx).Distance = dist;

         // Write node.
         this->myPageManager->WritePage(currPage);

         // Returning values
         promo1.Rep = NULL;
         promo1.Radius = leafNode->GetMinimumRadius();
         promo1.RootID = currNodeID;
         promo1.NObjects = leafNode->GetNumberOfEntries();
         result = NO_ACT;
      }else{
         // Split it!
         // New node.
         newPage = this->NewPage();
         newLeafNode = new stSlimLeafNode(newPage, true);

         // Split!
         SplitLeaf(leafNode, newLeafNode, (ObjectType *)newObj->Clone(),
                   repObj, promo1, promo2);

         // Write node.
         this->myPageManager->WritePage(newPage);
         // Clean home.
         delete newLeafNode;
         this->myPageManager->ReleasePage(newPage);
         result = PROMOTION; //Report split.
      }//end if
   }//end if

   // Write node.
   this->myPageManager->WritePage(currPage);
   // Clean home
   delete currNode;
   this->myPageManager->ReleasePage(currPage);
   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::InsertRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::RandomPromote(tLogicNode * node) {

   stCount idx1, idx2;
   stCount numberOfEntries = node->GetNumberOfEntries();

   // generate a number between 0 to numberOfEntries-1 fo idx1
   idx1 = random(numberOfEntries - 1);
   // while idx2 == idx1, do the random for idx2
   while ((idx2 = random(numberOfEntries - 1)) == idx1)
      ;
   // Choose representatives
   node->SetRepresentative(idx1, idx2);
}//end stSlimTree<ObjectType, EvaluatorType>::RandomPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::MinMaxPromote(tLogicNode * node) {

   stDistance iRadius, jRadius, min;
   stCount numberOfEntries, idx1, idx2, i, j;
   stPage * newPage1 = new stPage(this->myPageManager->GetMinimumPageSize());
   stPage * newPage2 = new stPage(this->myPageManager->GetMinimumPageSize());

   numberOfEntries = node->GetNumberOfEntries();
   min = MAXDOUBLE;   // Largest magnitude double value

   // Is it an Index node?
   if (node->GetNodeType() == stSlimNode::INDEX) {
      stSlimIndexNode * indexNode1 = new stSlimIndexNode(newPage1, true);
      stSlimIndexNode * indexNode2 = new stSlimIndexNode(newPage2, true);

      for (i = 0; i < numberOfEntries; i++) {
         for (j = i + 1; j < numberOfEntries; j++) {
            node->SetRepresentative(i, j);
            indexNode1->RemoveAll();
            indexNode2->RemoveAll();
            node->TestDistribution(indexNode1, indexNode2, this->myMetricEvaluator);
            iRadius = indexNode1->GetMinimumRadius();
            jRadius = indexNode2->GetMinimumRadius();
            if (iRadius < jRadius){
               iRadius = jRadius;      // take the maximum
            }//end if
            if (iRadius < min) {
               min = iRadius;
               idx1 = i;
               idx2 = j;
            }//end if
         }//end for
      }//end for
      delete indexNode1;
      delete indexNode2;
   }else{//it is a Leaf node
      stSlimLeafNode * leafNode1 = new stSlimLeafNode(newPage1, true);
      stSlimLeafNode * leafNode2 = new stSlimLeafNode(newPage2, true);

      for (i = 0; i < numberOfEntries; i++) {
         for (j = i + 1; j < numberOfEntries; j++) {
            node->SetRepresentative(i, j);
            leafNode1->RemoveAll();
            leafNode2->RemoveAll();
            node->TestDistribution(leafNode1, leafNode2, this->myMetricEvaluator);
            iRadius = leafNode1->GetMinimumRadius();
            jRadius = leafNode2->GetMinimumRadius();
            if (iRadius < jRadius){
               iRadius = jRadius;      // take the maximum
            }//end if
            if (iRadius < min) {
               min = iRadius;
               idx1 = i;
               idx2 = j;
            }//end if
         }//end for
      }//end for
      delete leafNode1;
      delete leafNode2;
   }//end else

   // Choose representatives
   node->SetRepresentative(idx1, idx2);

   delete newPage1;
   delete newPage2;
}//end stSlimTree<ObjectType, EvaluatorType>::MinMaxPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::SplitLeaf(
      stSlimLeafNode * oldNode, stSlimLeafNode * newNode,
      ObjectType * newObj, ObjectType * prevRep,
      stSubtreeInfo & promo1, stSubtreeInfo & promo2) {
   tLogicNode * logicNode;
   tMSTSplitter * mstSplitter;
   ObjectType * lRep;
   ObjectType * rRep;
   stCount numberOfEntries = oldNode->GetNumberOfEntries();

   // Create the new tLogicNode
   logicNode = new tLogicNode(numberOfEntries + 1);
   logicNode->SetMinOccupation((stCount )(GetMinOccupation() * (numberOfEntries + 1)));
   logicNode->SetNodeType(stSlimNode::LEAF);

   // update the maximum number of entries.
   this->SetMaxOccupation(numberOfEntries);

   // Add objects
   logicNode->AddLeafNode(oldNode);
   logicNode->AddEntry(newObj);

   // Split it.
   switch (GetSplitMethod()) {
      case stSlimTree::smRANDOM:
         this->RandomPromote(logicNode);
         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, this->myMetricEvaluator);
         delete logicNode;
         break; //end stSlimTree::smRANDOM
      case stSlimTree::smMINMAX:
         this->MinMaxPromote(logicNode);
         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, this->myMetricEvaluator);
         delete logicNode;
         break;  //end stSlimTree::smMINMAX
      case stSlimTree::smSPANNINGTREE:
         // MST Split
         mstSplitter = new tMSTSplitter(logicNode);
         // Perform MST
         oldNode->RemoveAll();
         mstSplitter->Distribute(oldNode, lRep, newNode, rRep, this->myMetricEvaluator);
         // Clean home
         delete mstSplitter;
         break; //end stSlimTree::smSPANNINGTREE
      #ifdef __stDEBUG__
      default:
         throw logic_error("There is no Split method selected.");
      #endif //__stDEBUG__
   };//end switch

   // Update fields. We may need to change lRep and rRep.
   if (prevRep == NULL){
      // This is a root. The order of lRep and rRep is not important.
      promo1.Rep = lRep;
      promo1.Radius = oldNode->GetMinimumRadius();
      promo1.RootID = oldNode->GetPageID();
      promo1.NObjects = oldNode->GetTotalObjectCount();
      promo2.Rep = rRep;
      promo2.Radius = newNode->GetMinimumRadius();
      promo2.RootID = newNode->GetPageID();
      promo2.NObjects = newNode->GetTotalObjectCount();
   }else{
      // Let's see if it is necessary to change things.
      if (prevRep->IsEqual(lRep)){
         // lRep is the prevRep. Delete it.
         delete lRep;
         promo1.Rep = NULL;
         promo1.Radius = oldNode->GetMinimumRadius();
         promo1.RootID = oldNode->GetPageID();
         promo1.NObjects = oldNode->GetTotalObjectCount();
         promo2.Rep = rRep;
         promo2.Radius = newNode->GetMinimumRadius();
         promo2.RootID = newNode->GetPageID();
         promo2.NObjects = newNode->GetTotalObjectCount();
      }else if (prevRep->IsEqual(rRep)){
         // rRep is the prevRep. Delete it.
         delete rRep;
         promo2.Rep = lRep;
         promo2.Radius = oldNode->GetMinimumRadius();
         promo2.RootID = oldNode->GetPageID();
         promo2.NObjects = oldNode->GetTotalObjectCount();
         promo1.Rep = NULL;
         promo1.Radius = newNode->GetMinimumRadius();
         promo1.RootID = newNode->GetPageID();
         promo1.NObjects = newNode->GetTotalObjectCount();
      }else{
         // This is a root. The order of lRep and rRep is not important.
         promo1.Rep = lRep;
         promo1.Radius = oldNode->GetMinimumRadius();
         promo1.RootID = oldNode->GetPageID();
         promo1.NObjects = oldNode->GetTotalObjectCount();
         promo2.Rep = rRep;
         promo2.Radius = newNode->GetMinimumRadius();
         promo2.RootID = newNode->GetPageID();
         promo2.NObjects = newNode->GetTotalObjectCount();
      }//end if
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::SplitLeaf

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::SplitIndex(
      stSlimIndexNode * oldNode, stSlimIndexNode * newNode,
      ObjectType * newObj1, stDistance newRadius1,
      stPageID newNodeID1, stCount newNEntries1,
      ObjectType * newObj2, stDistance newRadius2,
      stPageID newNodeID2, stCount newNEntries2,
      ObjectType * prevRep,
      stSubtreeInfo & promo1, stSubtreeInfo & promo2){
   tLogicNode * logicNode;
   tMSTSplitter * mstSplitter;
   ObjectType * lRep;
   ObjectType * rRep;
   stCount numberOfEntries = oldNode->GetNumberOfEntries();

   // Create the new tLogicNode
   logicNode = new tLogicNode(numberOfEntries + 2);
   logicNode->SetMinOccupation((stCount )(GetMinOccupation() * (numberOfEntries + 2)));
   logicNode->SetNodeType(stSlimNode::INDEX);

   // update the maximum number of entries.
   this->SetMaxOccupation(numberOfEntries);

   // Add objects
   logicNode->AddIndexNode(oldNode);

   // Add newObj1
   logicNode->AddEntry(newObj1);
   logicNode->SetEntry(logicNode->GetNumberOfEntries() - 1,
         newNodeID1, newNEntries1, newRadius1);

   // Will I add newObj2 ?
   if (newObj2 != NULL){
      logicNode->AddEntry(newObj2);
      logicNode->SetEntry(logicNode->GetNumberOfEntries() - 1,
            newNodeID2, newNEntries2, newRadius2);
   }//end if

   // Split it.
   switch (GetSplitMethod()) {
      case stSlimTree::smRANDOM:
         this->RandomPromote(logicNode);
         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, this->myMetricEvaluator);
         delete logicNode;
         break; //end stSlimTree::smRANDOM
      case stSlimTree::smMINMAX:
         this->MinMaxPromote(logicNode);
         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, this->myMetricEvaluator);
         delete logicNode;
         break;  //end stSlimTree::smMINMAX
      case stSlimTree::smSPANNINGTREE:
         // MST Split
         mstSplitter = new tMSTSplitter(logicNode);
         // Perform MST
         oldNode->RemoveAll();
         mstSplitter->Distribute(oldNode, lRep, newNode, rRep, this->myMetricEvaluator);
         // Clean home
         delete mstSplitter;
         break; //end stSlimTree::smSPANNINGTREE
      #ifdef __stDEBUG__
      default:
         throw logic_error("There is no Split method selected.");
      #endif //__stDEBUG__
   };//end switch

   // Update fields. We may need to change lRep and rRep.
   if (prevRep == NULL){
      // This is a root. The order of lRep and rRep is not important.
      promo1.Rep = lRep;
      promo1.Radius = oldNode->GetMinimumRadius();
      promo1.RootID = oldNode->GetPageID();
      promo1.NObjects = oldNode->GetTotalObjectCount();
      promo2.Rep = rRep;
      promo2.Radius = newNode->GetMinimumRadius();
      promo2.RootID = newNode->GetPageID();
      promo2.NObjects = newNode->GetTotalObjectCount();
   }else{
      // Let's see if it is necessary to change things.
      if (prevRep->IsEqual(lRep)){
         // lRep is the prevRep. Delete it.
         delete lRep;
         promo1.Rep = NULL;
         promo1.Radius = oldNode->GetMinimumRadius();
         promo1.RootID = oldNode->GetPageID();
         promo1.NObjects = oldNode->GetTotalObjectCount();
         promo2.Rep = rRep;
         promo2.Radius = newNode->GetMinimumRadius();
         promo2.RootID = newNode->GetPageID();
         promo2.NObjects = newNode->GetTotalObjectCount();
      }else if (prevRep->IsEqual(rRep)){
         // rRep is the prevRep. Delete it.
         delete rRep;
         promo2.Rep = lRep;
         promo2.Radius = oldNode->GetMinimumRadius();
         promo2.RootID = oldNode->GetPageID();
         promo2.NObjects = oldNode->GetTotalObjectCount();
         promo1.Rep = NULL;
         promo1.Radius = newNode->GetMinimumRadius();
         promo1.RootID = newNode->GetPageID();
         promo1.NObjects = newNode->GetTotalObjectCount();
      }else{
         // This is a root. The order of lRep and rRep is not important.
         promo1.Rep = lRep;
         promo1.Radius = oldNode->GetMinimumRadius();
         promo1.RootID = oldNode->GetPageID();
         promo1.NObjects = oldNode->GetTotalObjectCount();
         promo2.Rep = rRep;
         promo2.Radius = newNode->GetMinimumRadius();
         promo2.RootID = newNode->GetPageID();
         promo2.NObjects = newNode->GetTotalObjectCount();
      }//end if
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::SplitIndex

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::UpdateDistances(stSlimIndexNode * node,
            ObjectType * repObj, stCount repObjIdx){
   stCount i;
   ObjectType * tempObj = new ObjectType();

   for (i = 0; i < node->GetNumberOfEntries(); i++){
      if (i != repObjIdx){
         tempObj->Unserialize(node->GetObject(i), node->GetObjectSize(i));
         node->GetIndexEntry(i).Distance =
            this->myMetricEvaluator->GetDistance(repObj, tempObj);
      }else{
         //it's the representative object
         node->GetIndexEntry(i).Distance = 0.0;
      }//end if
   }//end for

   //clean the house before exit.
   delete tempObj;
}//end stSlimTree<ObjectType, EvaluatorType>::UpdateDistances

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance tmpl_stSlimTree::GetDistanceLimit(){
   stDistance distance = 0;
   stDistance distanceTemp = 0;
   stCount i, j;
   ObjectType * object1 = new ObjectType();
   ObjectType * object2 = new ObjectType();
   stPage * currPage;
   stSlimNode * currNode;
   stSlimIndexNode * indexNode;

   // Is there a root ?
   if (this->GetRoot()){
      // Yes.
      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = stSlimNode::CreateNode(currPage);

      // Index Node cast.
      indexNode = (stSlimIndexNode *)currNode;

      //search every entry in the root node
      for (i = 0; i < indexNode->GetNumberOfEntries() - 1; i++){
         //get a object
         object1->Unserialize(indexNode->GetObject(i), indexNode->GetObjectSize(i));
         //combine with the others objects
         for (j = i + 1; j < indexNode->GetNumberOfEntries(); j++){
            //get the other object
            object2->Unserialize(indexNode->GetObject(j),
                                 indexNode->GetObjectSize(j));
            //calculate the distance of the two objects
            distanceTemp = this->myMetricEvaluator->GetDistance(object1, object2);
            //sum the distance with the distance of the two
            distanceTemp = distanceTemp + indexNode->GetIndexEntry(i).Radius +
                                      indexNode->GetIndexEntry(j).Radius;
            //if this sum is greater than the previous...
            if (distanceTemp > distance)
               //store it to return this value
               distance = distanceTemp;
         }//end for
      }//end for
   }//end if

   //cleaning...
   delete object1;
   delete object2;

   //return the maximum distance between 2 objects of this tree
   return distance;
}//end stSlimTree<ObjectType, EvaluatorType>::GetDistanceLimit

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance tmpl_stSlimTree::GetGreaterEstimatedDistance(){
   stDistance distance = 0;
   stDistance distanceTemp = 0;
   stCount idx, idx2;
   ObjectType ** objects;
   stCount size = 0;
   stPage * currPage, * currPage2;
   stSlimNode * currNode, * currNode2;
   stSlimIndexNode * indexNode, * indexNode2;
   stSlimLeafNode * leafNode;

   // Is there a root ?
   if (this->GetRoot()){
      // Yes. Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = stSlimNode::CreateNode(currPage);

      // Is it an index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         indexNode = (stSlimIndexNode *)currNode;
         // Estimate the maximum number of entries.
         objects = new ObjectType * [this->GetMaxOccupation() * indexNode->GetNumberOfEntries()];
         // For each entry...
         for (idx = 0; idx < indexNode->GetNumberOfEntries(); idx++) {
            // Get the pages.
            currPage2 = this->myPageManager->GetPage(indexNode->GetIndexEntry(idx).PageID);
            currNode2 = stSlimNode::CreateNode(currPage2);
            // Is it am index node?
            if (currNode2->GetNodeType() == stSlimNode::INDEX) {
               // Get Index node
               indexNode2 = (stSlimIndexNode *)currNode2;

               // For each entry...
               for (idx2 = 0; idx2 < indexNode2->GetNumberOfEntries(); idx2++) {
                  // Rebuild the object
                  objects[size] = new ObjectType();
                  objects[size++]->Unserialize(indexNode2->GetObject(idx2),
                                               indexNode2->GetObjectSize(idx2));
               }//end for
            }else{
               // No, it is a leaf node. Get it.
               leafNode = (stSlimLeafNode *)currNode2;
               // For each entry...
               for (idx2 = 0; idx2 < leafNode->GetNumberOfEntries(); idx2++) {
                  // Rebuild the object
                  objects[size] = new ObjectType();
                  objects[size++]->Unserialize(leafNode->GetObject(idx2),
                                               leafNode->GetObjectSize(idx2));
               }//end for
            }//end if
            // Free it all
            delete currNode2;
            this->myPageManager->ReleasePage(currPage2);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         leafNode = (stSlimLeafNode *)currNode;
         // Estimate the maximum number of entries.
         objects = new ObjectType * [this->GetMaxOccupation() * leafNode->GetNumberOfEntries()];
         // For each entry...
         for (idx = 0; idx < leafNode->GetNumberOfEntries(); idx++) {
            // Rebuild the object
            objects[size] = new ObjectType();
            objects[size++]->Unserialize(leafNode->GetObject(idx),
                                         leafNode->GetObjectSize(idx));
         }//end for
      }//end else

      // Search in all entries in the second level.
      for (idx = 0; idx < size-1; idx++){
         //combine with the others objects
         for (idx2 = idx+1; idx2 < size; idx2++){
            // Calculate the distance of the two objects
            distanceTemp = this->myMetricEvaluator->GetDistance(objects[idx], objects[idx2]);
            // If this sum is greater than the previous...
            if (distanceTemp > distance)
               //store it to return this value
               distance = distanceTemp;
         }//end for
      }//end for

      // Cleaning...
      for (idx = 0; idx < size; idx++){
         delete objects[idx];
      }//end for
      delete[] objects;
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   //return the maximum distance between 2 objects of this tree
   return distance;
}//end stSlimTree<ObjectType, EvaluatorType>::GetGreaterEstimatedDistance

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance tmpl_stSlimTree::GetGreaterDistance(){
   stDistance greaterDistance;
   stDistance distanceTemp;
   stCount idx, idx2;
   ObjectType ** objects;
   stCount size;

   greaterDistance = 0;
   // Is there a root ?
   if (this->GetRoot()){
      distanceTemp = 0;
      size = 0;
      // Allocate the maximum number of entries.
      objects = new ObjectType * [this->GetNumberOfObjects()];
      // Call the GetGreaterDistance recursively.
      this->GetGreaterDistance(this->GetRoot(), objects, size);

      // Search in all entries.
      for (idx = 0; idx < size - 1; idx++){
         //combine with the others objects
         for (idx2 = idx + 1; idx2 < size; idx2++){
            // Calculate the distance of the two objects
            distanceTemp = this->myMetricEvaluator->GetDistance(objects[idx], objects[idx2]);
            // If this sum is greater than the previous...
            if (distanceTemp > greaterDistance)
               //store it to return this value
               greaterDistance = distanceTemp;
         }//end for
      }//end for

      // Cleaning...
      for (idx = 0; idx < size; idx++){
         delete objects[idx];
      }//end for
      delete[] objects;
   }//end if

   //return the maximum distance between 2 objects of this tree
   return greaterDistance;
}//end stSlimTree<ObjectType, EvaluatorType>::GetGreaterDistance

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::GetGreaterDistance(stPageID pageID, ObjectType ** objects,
                                         stCount & size){
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stCount idx;
   stCount numberOfEntries;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it an Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Analyze it!
            this->GetGreaterDistance(indexNode->GetIndexEntry(idx).PageID, objects, size);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(leafNode->GetObject(idx),
                               leafNode->GetObjectSize(idx));
            // Put it in the objects set.
            objects[size++] = tmpObj.Clone();
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::GetGreaterDistance

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
long tmpl_stSlimTree::GetIndexNodeCount(){
   stPage * currPage;
   stSlimNode * currNode;
   stCount idx, numberOfEntries;
   double nodeCount = 0;

   // Is there a root ?
   if (this->GetRoot()){
      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = stSlimNode::CreateNode(currPage);

      // Is it an Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX){
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Analyze this subtree.
            nodeCount += this->GetIndexNodeCount(indexNode->GetIndexEntry(idx).PageID) + 1;
         }//end for

      }//end if

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   //return
   return nodeCount;
}//end stSlimTree<ObjectType, EvaluatorType>::GetIndexNodeCount

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
long tmpl_stSlimTree::GetIndexNodeCount(stPageID pageID){
   stPage * currPage;
   stSlimNode * currNode;
   stCount idx;
   stCount numberOfEntries;
   long nodeCount = 0;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it an Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Analyze this subtree.
            nodeCount += this->GetIndexNodeCount(indexNode->GetIndexEntry(idx).PageID) + 1;
         }//end for
      }//end if

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   return nodeCount;
}//end stSlimTree<ObjectType, EvaluatorType>::GetIndexNodeCount

//==============================================================================
// Begin of Queries
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stSlimTree::RangeQuery(
            ObjectType * sample, stDistance range){
   tResult * result = new tResult();  // Create result
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stCount idx, numberOfEntries;
   stDistance distance;
   #ifdef __stMAMVIEW__
      stMessageString title;
      stMessageString comment;
   #endif //__stMAMVIEW__

   // Set the information.
   result->SetQueryInfo(sample->Clone(), tResult::RANGEQUERY, -1, range, false);

   // Visualization support
   #ifdef __stMAMVIEW__
      MAMViewer->SetQueryInfo(0, range);
      MAMViewer->SetLevel(0);
      title.Append("Slim-Tree: Range Query with page size ");
      title.Append((int) this->myPageManager->GetMinimumPageSize());
      comment.Append("The radius of this range query is ");
      comment.Append((double)range);
      MAMViewer->BeginAnimation(title.GetStr(), comment.GetStr());
   #endif //__stMAMVIEW__

   // Evaluate the root node.
   if (this->GetRoot() != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = stSlimNode::CreateNode(currPage);

      // Is it an Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX){
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // Visualization support
         #ifdef __stMAMVIEW__
            MAMViewer->LevelUp();
            comment.Clear();
            comment.Append("Root is the index node ");
            comment.Append((int) this->GetRoot());
            MAMViewer->BeginFrame(comment.GetStr());
            // for each entry...
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Add all child nodes all active
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               MAMViewer->SetNode(indexNode->GetIndexEntry(idx).PageID, &tmpObj,
                     indexNode->GetIndexEntry(idx).Radius, this->GetRoot(), 0, true);
            }//end for
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(indexNode->GetObject(idx),
                               indexNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // test if this subtree qualifies.
            if (distance <= range + indexNode->GetIndexEntry(idx).Radius){
               // Yes! Analyze this subtree.
               this->RangeQuery(indexNode->GetIndexEntry(idx).PageID, result,
                                sample, range, distance);
            }//end if
         }//end for

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         #ifdef __stMAMVIEW__
            comment.Append("Root is the leaf node ");
            comment.Append((int) this->GetRoot());
            MAMViewer->BeginFrame(comment.GetStr());
            // for each entry...
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Add objects to the node
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, this->GetRoot(), true);
            }//end for
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(leafNode->GetObject(idx),
                               leafNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // is it a object that qualified?
            if (distance <= range){
               // Yes! Put it in the result set.
               result->AddPair(tmpObj.Clone(), distance);
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   // Visualization support
   #ifdef __stMAMVIEW__
      // Add the last frame with the final result
      comment.Clear();
      comment.Append("The final result has ");
      comment.Append((int)result->GetNumOfEntries());
      comment.Append(" object(s) and radius ");
      comment.Append((double)result->GetMaximumDistance());
      MAMViewer->BeginFrame(comment.GetStr());
      MAMViewer->SetResult(sample, result);
      MAMViewer->EndFrame();
      MAMViewer->EndAnimation();
   #endif //__stMAMVIEW__
   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::RangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::RangeQuery(
         stPageID pageID, tResult * result, ObjectType * sample,
         stDistance range, stDistance distanceRepres){
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;
   #ifdef __stMAMVIEW__
      stMessageString comment;
   #endif //__stMAMVIEW__

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it an Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // Visualization support
         #ifdef __stMAMVIEW__
            MAMViewer->LevelUp();
            comment.Clear();
            comment.Append("Entering in the index node ");
            comment.Append((int) pageID);
            comment.Append(" at level ");
            comment.Append((int)  MAMViewer->GetLevel());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->EnableNode(pageID);
            // for each entry...
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Add all child nodes all active
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               MAMViewer->SetNode(indexNode->GetIndexEntry(idx).PageID, &tmpObj,
                     indexNode->GetIndexEntry(idx).Radius, pageID, 0, true);
            }//end for
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // use of the triangle inequality to cut a subtree
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      range + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               // is this a qualified subtree?
               if (distance <= range + indexNode->GetIndexEntry(idx).Radius){
                  // Yes! Analyze it!
                  this->RangeQuery(indexNode->GetIndexEntry(idx).PageID, result,
                                    sample, range, distance);
                  #ifdef __stMAMVIEW__
                     comment.Clear();
                     comment.Append("Returning to the index node ");
                     comment.Append((int) pageID);
                     comment.Append(" at level ");
                     comment.Append((int)  MAMViewer->GetLevel());
                     MAMViewer->BeginFrame(comment.GetStr());
                     MAMViewer->EnableNode(pageID);
                     MAMViewer->EndFrame();
                  #endif //__stMAMVIEW__
               }//end if
            }//end if
         }//end for

         // Visualization support
         #ifdef __stMAMVIEW__
            MAMViewer->LevelDown();
         #endif //__stMAMVIEW__
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("Entering in the leaf node ");
            comment.Append((int) pageID);
            comment.Append(" at level ");
            comment.Append((int)  MAMViewer->GetLevel());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->EnableNode(pageID);
            // for each entry...
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Add objects to the node
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, pageID, true);
            }//end for
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // use of the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      range){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // No, it is not a representative. Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               // Is this a qualified object?
               if (distance <= range){
                  // Yes! Put it in the result set.
                  result->AddPair(tmpObj.Clone(), distance);
               }//end if
            }//end if
         }//end for

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("The result after the leaf node ");
            comment.Append((int) pageID);
            comment.Append(" at level ");
            comment.Append((int)  MAMViewer->GetLevel());
            comment.Append(" has ");
            comment.Append((int)result->GetNumOfEntries());
            comment.Append(" object(s) and radius ");
            comment.Append((double)result->GetMaximumDistance());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::RangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stSlimTree::ReversedRangeQuery(
            ObjectType * sample, stDistance range){
   tResult * result = new tResult();  // Create result
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stCount idx, numberOfEntries;
   stDistance distance;

   result->SetQueryInfo(sample->Clone(), tResult::REVERSEDRANGEQUERY, -1, range, false);

   // Evaluate the root node.
   if (this->GetRoot() != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = stSlimNode::CreateNode(currPage);

      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX){
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(indexNode->GetObject(idx),
                               indexNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // test if this subtree qualifies.
            if (distance + indexNode->GetIndexEntry(idx).Radius >= range){
               // Yes! Analyze this subtree.
               this->ReversedRangeQuery(indexNode->GetIndexEntry(idx).PageID, result,
                                        sample, range, distance);
            }//end if
         }//end for

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(leafNode->GetObject(idx),
                               leafNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // is it a object that qualified?
            if (distance >= range){
               // Yes! Put it in the result set.
               result->AddPair(tmpObj.Clone(), distance);
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::ReversedRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::ReversedRangeQuery(
         stPageID pageID, tResult * result, ObjectType * sample,
         stDistance range, stDistance distanceRepres){
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it an Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if (distanceRepres + indexNode->GetIndexEntry(idx).Distance +
                indexNode->GetIndexEntry(idx).Radius >= range){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               // is this a qualified subtree?
               if (distance + indexNode->GetIndexEntry(idx).Radius >= range){
                  // Yes! Analyze it!
                  this->ReversedRangeQuery(indexNode->GetIndexEntry(idx).PageID, result,
                                           sample, range, distance);
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if (distanceRepres + leafNode->GetLeafEntry(idx).Distance >= range){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // No, it is not a representative. Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               // Is this a qualified object?
               if (distance >= range){
                  // Yes! Put it in the result set.
                  result->AddPair(tmpObj.Clone(), distance);
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::ReversedRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stSlimTree::LocalNearestQuery(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   stDistance rangeK = MAXDOUBLE;
   stPage * rootPage;
   stSlimNode * rootNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;
   tPriorityQueue * queue;
   stCount pid;

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, rangeK, tie);

   // Let's search
   if (this->GetRoot() != 0){
      // Read node...
      rootPage = this->myPageManager->GetPage(this->GetRoot());
      rootNode = stSlimNode::CreateNode(rootPage);

      // Is it a Index node?
      if (rootNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)rootNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // Priority queue
         queue = new tPriorityQueue(numberOfEntries);
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(indexNode->GetObject(idx),
                               indexNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // Is this a qualified subtree?
            if (distance <= rangeK + indexNode->GetIndexEntry(idx).Radius){
               // Yes! Put it in the queue.
               queue->Add(distance, idx);
            }//end if
         }//end for

         // Search...
         while (queue->Get(distance, pid)){
            // Will qualify ?
            if (distance <= rangeK + indexNode->GetIndexEntry(pid).Radius){
               // Yes! Analyze it recursively.
               this->LocalNearestQuery(indexNode->GetIndexEntry(pid).PageID, result,
                                       sample, rangeK, k, distance);
            }//end if
         }//end while

         // Release queue.
         delete queue;

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)rootNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(leafNode->GetObject(idx),
                               leafNode->GetObjectSize(idx));
            // Evaluate the distance.
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            //test if the object qualify
            if (distance <= rangeK){
               // Add the object.
               result->AddPair(tmpObj.Clone(), distance);
               // there is more than k elements?
               if (result->GetNumOfEntries() >= k){
                  //cut if there is more than k elements
                  result->Cut(k);
                  //may I use this for performance?
                  rangeK = result->GetMaximumDistance();
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete rootNode;
      this->myPageManager->ReleasePage(rootPage);
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::LocalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::LocalNearestQuery(
         stPageID pageID, tResult * result, ObjectType * sample,
         stDistance & rangeK, stCount k, stDistance distanceRepres){
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;
   tPriorityQueue * queue;
   stCount pid;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // Priority queue
         queue = new tPriorityQueue(numberOfEntries);
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      rangeK + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               // Is it a qualified subtree?
               if (distance <= rangeK + indexNode->GetIndexEntry(idx).Radius){
                  // Yes! Put it in the queue.
                  queue->Add(distance, idx);
               }//end if
            }//end if
         }//end for

         // Search...
         while (queue->Get(distance, pid)){
            // Will qualify ?
            if (distance <= rangeK + indexNode->GetIndexEntry(pid).Radius){
               // Yes! Analyze it.
               this->LocalNearestQuery(indexNode->GetIndexEntry(pid).PageID, result,
                                  sample, rangeK, k, distance);
            }//end if
         }//end while

         // Release queue.
         delete queue;

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      rangeK){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // is it a Representative?
               if (leafNode->GetLeafEntry(idx).Distance!=0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if
               // is this a qualified object?
               if (distance <= rangeK){
                  // Yes, add it in the result set.
                  result->AddPair(tmpObj.Clone(), distance);
                  // there is more than k elements?
                  if (result->GetNumOfEntries() >= k){
                     //cut if there is more than k elements
                     result->Cut(k);
                     //may I use this for performance?
                     rangeK = result->GetMaximumDistance();
                  }//end if
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

}//end stSlimTree<ObjectType, EvaluatorType>::LocalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::ListNearestQuery(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);

   // Let's search
   if (this->GetRoot() != 0){
      this->ListNearestQuery(result, sample, MAXDOUBLE, k);
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::ListNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::ListNearestQuery(tResult * result,
         ObjectType * sample, stDistance rangeK, stCount k){
   tGenericPriorityQueue * globalQueue;
   stCount idx;
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   tGenericEntry * entryNode = NULL;
   stCount numberOfEntries;
   bool stop;

   if (this->GetRoot() != 0){
      // allocate the priority list.
      globalQueue = new tGenericPriorityQueue();
      // Get the root node.
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(indexNode->GetObject(idx),
                               indexNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // Put the Node in the Queue.
            globalQueue->Add(tmpObj.Clone(), indexNode->GetIndexEntry(idx).PageID, distance,
                             indexNode->GetIndexEntry(idx).Radius, tGenericEntry::NODE);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(leafNode->GetObject(idx),
                               leafNode->GetObjectSize(idx));
            // Evaluate the distance.
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            //test if the object qualify
            if (distance <= rangeK){
               // Add the object.
               result->AddPair(tmpObj.Clone(), distance);
               // there is more than k elements?
               if (result->GetNumOfEntries() >= k){
                  //cut if there is more than k elements
                  result->Cut(k);
                  //may I use this for performance?
                  rangeK = result->GetMaximumDistance();
               }//end if
            }//end if
         }//end for
      }//end if

      //Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      do{
         entryNode = globalQueue->Get();
         // Read node...
         currPage = this->myPageManager->GetPage(entryNode->GetPageID());
         currNode = stSlimNode::CreateNode(currPage);
         // Is it a Index node?
         if (currNode->GetNodeType() == stSlimNode::INDEX) {
            // Get Index node
            stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
            numberOfEntries = indexNode->GetNumberOfEntries();

            // for each entry...
            // Put the Children in the global priority queue.
            for (idx = 0; idx < numberOfEntries; idx++) {
               // try to cut this subtree with the triangle inequality.
               if ( fabs(entryNode->GetDistanceRepQuery() - indexNode->GetIndexEntry(idx).Distance) <=
                         rangeK + indexNode->GetIndexEntry(idx).Radius){
                  // Rebuild the object
                  tmpObj.Unserialize(indexNode->GetObject(idx),
                                     indexNode->GetObjectSize(idx));
                  // calculate its distance.
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
                  // add in the priority queue.
                  globalQueue->Add(tmpObj.Clone(), indexNode->GetIndexEntry(idx).PageID,
                                   distance, indexNode->GetIndexEntry(idx).Radius,
                                   tGenericEntry::NODE);
               }//end if
            }//end for
         }else{
            // No, it is a leaf node. Get it.
            stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
            numberOfEntries = leafNode->GetNumberOfEntries();
            // for each entry...
            for (idx = 0; idx < numberOfEntries; idx++) {
               // try to cut this object with the triangle inequality.
               if ( fabs(entryNode->GetDistanceRepQuery() - leafNode->GetLeafEntry(idx).Distance) <=
                         rangeK){
                  // Rebuild the object
                  tmpObj.Unserialize(leafNode->GetObject(idx),
                                     leafNode->GetObjectSize(idx));
                  // When this entry is a representative, it does not need to evaluate
                  // a distance, because distanceRepres is iqual to distance.
                  // Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
                  //test if the object qualify
                  if (distance <= rangeK){
                     // Add the object.
                     result->AddPair(tmpObj.Clone(), distance);
                     // there is more than k elements?
                     if (result->GetNumOfEntries() >= k){
                        //cut if there is more than k elements
                        result->Cut(k);
                        //may I use this for performance?
                        rangeK = result->GetMaximumDistance();
                     }//end if
                  }//end if
               }//end if
            }//end for
         }//end if
         //Free it all
         delete currNode;
         this->myPageManager->ReleasePage(currPage);

         // Release this entry.
         delete entryNode;

      }while (!stop && !globalQueue->IsEmpty());
      // delete the priority list.
      delete globalQueue;
   }//end if

}//end stSlimTree<ObjectType, EvaluatorType>::ListNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::NearestQuery(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   #ifdef __stMAMVIEW__
      stMessageString title;
      stMessageString comment;
   #endif //__stMAMVIEW__

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);

   #ifdef __stMAMVIEW__
      MAMViewer->SetQueryInfo(k, 0);
      MAMViewer->SetLevel(0);
      title.Append("Slim-Tree: k-Nearest Neighbor Query with page size ");
      title.Append((int) this->myPageManager->GetMinimumPageSize());
      comment.Append("The k of this nearest neighbor query is ");
      comment.Append((int)k);
      MAMViewer->BeginAnimation(title.GetStr(), comment.GetStr());
   #endif //__stMAMVIEW__

   // Let's search
   if (this->GetRoot() != 0){
      this->NearestQuery(result, sample, MAXDOUBLE, k);
   }//end if

   // Visualization support
   #ifdef __stMAMVIEW__
      // Add the last frame with the final result
      comment.Clear();
      comment.Append("The final result has ");
      comment.Append((int)result->GetNumOfEntries());
      comment.Append(" object(s) and radius ");
      comment.Append((double)result->GetMaximumDistance());
      MAMViewer->BeginFrame(comment.GetStr());
      MAMViewer->SetResult(sample, result);
      MAMViewer->EndFrame();
      MAMViewer->EndAnimation();
   #endif //__stMAMVIEW__

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::NearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::NearestQuery(tResult * result,
         ObjectType * sample, stDistance rangeK, stCount k){
   tDynamicPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   bool stop;
   #ifdef __stMAMVIEW__
      stMessageString comment;
   #endif //__stMAMVIEW__

   // Root node
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;
   #ifdef __stMAMVIEW__
      pqCurrValue.Parent = -1;
      pqCurrValue.Level = 0;
   #endif //__stMAMVIEW__

   // Create the Global Priority Queue
   queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // Visualization support
         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("Entering in the index node ");
            comment.Append((int) pqCurrValue.PageID);
            comment.Append(" at level ");
            comment.Append((int) pqCurrValue.Level);
            MAMViewer->SetLevel( pqCurrValue.Level + 1);
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->EnableNode(pqCurrValue.PageID);
            // for each entry...
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Add all child nodes all active
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               MAMViewer->SetNode(indexNode->GetIndexEntry(idx).PageID, &tmpObj,
                                  indexNode->GetIndexEntry(idx).Radius,
                                  pqCurrValue.PageID, 0, true);
            }//end for
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      rangeK + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

               if (distance <= rangeK + indexNode->GetIndexEntry(idx).Radius){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTmpValue.PageID = indexNode->GetIndexEntry(idx).PageID;
                  pqTmpValue.Radius = indexNode->GetIndexEntry(idx).Radius;
                  #ifdef __stMAMVIEW__
                     pqTmpValue.Parent = pqCurrValue.Parent;
                     pqTmpValue.Level = pqCurrValue.Level + 1;
                  #endif //__stMAMVIEW__
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("Entering in the leaf node ");
            comment.Append((int) pqCurrValue.PageID);
            comment.Append(" at level ");
            comment.Append((int) pqCurrValue.Level);

            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->EnableNode(pqCurrValue.PageID);
            // for each entry...
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Add objects to the node
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, pqCurrValue.PageID, true);
            }//end for
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      rangeK){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // When this entry is a representative, it does not need to evaluate
               // a distance, because distanceRepres is iqual to distance.
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               //test if the object qualify
               if (distance <= rangeK){
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
                  // there is more than k elements?
                  if (result->GetNumOfEntries() >= k){
                     //cut if there is more than k elements
                     result->Cut(k);
                     //may I use this for performance?
                     rangeK = result->GetMaximumDistance();
                  }//end if
               }//end if
            }//end if
         }//end for

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("The result after the leaf node ");
            comment.Append((int) pqCurrValue.PageID);
            comment.Append(" at level ");
            comment.Append((int) pqCurrValue.Level);
            comment.Append(" has ");
            comment.Append((int)result->GetNumOfEntries());
            comment.Append(" object(s) and radius ");
            comment.Append((double)result->GetMaximumDistance());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // Go to next node
      stop = false;
      do{
         if (queue->Get(distance, pqCurrValue)){
            // Qualified if distance <= rangeK + radius
            if (distance <= rangeK + pqCurrValue.Radius){
               // Yes, get the pageID and the distance from the representative
               // and the query object.
               distanceRepres = distance;
               // Break the while.
               stop = true;
            }//end if
         }else{
            // the queue is empty!
            pqCurrValue.PageID = 0;
            // Break the while.
            stop = true;
         }//end if
      }while (!stop);
   }// end while

   // Release the Global Priority Queue
   delete queue;
}//end stSlimTree<ObjectType, EvaluatorType>::NearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::FarthestQuery(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KFARTHESTQUERY, k, MAXDOUBLE, tie);

   // Let's search
   if (this->GetRoot() != 0){
      this->FarthestQuery(result, sample, 0.0, k);
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::FarthestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::FarthestQuery(tResult * result,
         ObjectType * sample, stDistance rangeK, stCount k){
   tDynamicReversedPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   bool stop;

   // Root node
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;

   // Create the Global Priority Queue
   queue = new tDynamicReversedPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if (distanceRepres + indexNode->GetIndexEntry(idx).Distance +
                indexNode->GetIndexEntry(idx).Radius >= rangeK){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

               if (distance + indexNode->GetIndexEntry(idx).Radius >= rangeK){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTmpValue.PageID = indexNode->GetIndexEntry(idx).PageID;
                  pqTmpValue.Radius = indexNode->GetIndexEntry(idx).Radius;
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if (distanceRepres + leafNode->GetLeafEntry(idx).Distance >= rangeK){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // When this entry is a representative, it does not need to evaluate
               // a distance, because distanceRepres is iqual to distance.
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               //test if the object qualify
               if (distance >= rangeK){
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
                  // there is more than k elements?
                  if (result->GetNumOfEntries() >= k){
                     //cut if there is more than k elements
                     result->CutFirst(k);
                     //may I use this for performance?
                     rangeK = result->GetMinimumDistance();
                  }//end if
               }//end if
            }//end if
         }//end for

      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // Go to next node
      stop = false;
      do{
         if (queue->Get(distance, pqCurrValue)){
            // Qualified if distance <= rangeK + radius
            if (distance + pqCurrValue.Radius >= rangeK){
               // Yes, get the pageID and the distance from the representative
               // and the query object.
               distanceRepres = distance;
               // Break the while.
               stop = true;
            }//end if
         }else{
            // the queue is empty!
            pqCurrValue.PageID = 0;
            // Break the while.
            stop = true;
         }//end if
      }while (!stop);
   }// end while

   // Release the Global Priority Queue
   delete queue;
}//end stSlimTree<ObjectType, EvaluatorType>::FarthestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::PointQuery(
      ObjectType * sample){
   tResult * result = new tResult();  // Create result

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::POINTQUERY);
   // Let's search
   if (this->GetRoot() != 0){
      this->PointQuery(result, sample);
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::PointQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::PointQuery(
         tResult * result, ObjectType * sample){
   tDynamicPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTMPValue;
   stCount numberOfEntries;
   bool stop;
   bool find = false;

   // Root node
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;

   // Create the Global Priority Queue
   queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while ((pqCurrValue.PageID != 0) && (!find)){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

               if (distance <= indexNode->GetIndexEntry(idx).Radius){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTMPValue.PageID =  indexNode->GetIndexEntry(idx).PageID;
                  pqTMPValue.Radius =  indexNode->GetIndexEntry(idx).Radius;
                  queue->Add(distance, pqTMPValue);
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // use of the triangle inequality
            if ( distanceRepres == leafNode->GetLeafEntry(idx).Distance){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // When this entry is a representative, it does not need to evaluate
               // a distance, because distanceRepres is iqual to distance.
               // Evaluate distance.
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               //test if the object qualify
               if (distance == 0){
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
                  // Stop the query because the object was found!
                  find = true;
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // Go to next node.
      if (!find){
         // Search... and feed query
         stop = false;
         do{
            if (queue->Get(distance, pqCurrValue)){
               // Qualified if distance <= rangeK + radius
               if (distance <= pqCurrValue.Radius){
                  // Yes, get the pageID and the distance from the representative
                  // and the query object.
                  distanceRepres = distance;
                  // Break the while.
                  stop = true;
               }//end if
            }else{
               // the queue is empty!
               pqCurrValue.PageID = 0;
               // Break the while.
               stop = true;
            }//end if
         }while (!stop);
      }//end if
   }//end while

   // Release the Global Priority Queue
   delete queue;
}//end stSlimTree<ObjectType, EvaluatorType>::PointQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stSlimTree::KAndRangeQuery(
      ObjectType * sample, stDistance range, stCount k, bool tie){

   tResult * result = new tResult();  // Create result

   result->SetQueryInfo(sample->Clone(), tResult::KANDRANGEQUERY, k, range, tie);
   // Let's search
   if (this->GetRoot() != 0){
      this->KAndRangeQuery(result, sample, range, k);
   }//end if
   // return the result
   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::KAndRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::KAndRangeQuery(
         tResult * result, ObjectType * sample, stDistance range, stCount k){
   tDynamicPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTMPValue;
   stCount numberOfEntries;
   bool stop;

   // Root node
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;

   // Create the Global Priority Queue
   queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      range + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               // test if this subtree qualifies.
               if (distance <= range + indexNode->GetIndexEntry(idx).Radius){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTMPValue.PageID = indexNode->GetIndexEntry(idx).PageID;
                  pqTMPValue.Radius = indexNode->GetIndexEntry(idx).Radius;
                  queue->Add(distance, pqTMPValue);
               }//end if
            }//end if
         }//end for

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      range){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // is it a Representative?
               if (leafNode->GetLeafEntry(idx).Distance!=0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if
               if (distance <= range){
                  // Yes! I'm qualified !
                  if (result->GetNumOfEntries() < k){
                     // Has less than k.
                     result->AddPair(tmpObj.Clone(), distance);
                  }else{
                     // May I add ?
                     if (distance <= result->GetMaximumDistance()){
                        // Yes! I'll add it and cut the results if necessary
                        result->AddPair(tmpObj.Clone(), distance);
                        //cut if there is more than k elements
                        result->Cut(k);
                        //may I use this for performance?
                        range = result->GetMaximumDistance();
                     }//end if
                  }//end if
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // Next node
      stop = false;
      do{
         if (queue->Get(distance, pqCurrValue)){
            // Qualified if distance <= rangeK + radius
            if (distance <= range + pqCurrValue.Radius){
               // Yes, get the pageID and the distance from the representative
               // and the query object.
               distanceRepres = distance;
               // Break the while.
               stop = true;
            }//end if
         }else{
            // the queue is empty!
            pqCurrValue.PageID = 0;
            // Break the while.
            stop = true;
         }//end if
      }while (!stop);
   }// end while

   // Release the Global Priority Queue
   delete queue;
}//end stSlimTree<ObjectType, EvaluatorType>::KAndRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stSlimTree::KOrRangeQuery(
            ObjectType * sample, stDistance range, stCount k, bool tie){
   tResult * result = new tResult();  // Create result

   result->SetQueryInfo(sample->Clone(), tResult::KORRANGEQUERY, k, range, tie);
   // Let's search
   if (this->GetRoot() != 0){
      this->KOrRangeQuery(result, sample, range, k);
   }//end if
   // Return the result.
   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::KOrRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::KOrRangeQuery(
      tResult * result, ObjectType * sample, stDistance range, stCount k){

   tDynamicPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTMPValue;
   stDistance distanceK = MAXDOUBLE;
   stDistance distance;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   bool stop;

   // Root node
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;

   // Create the Global Priority Queue
   queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      distanceK + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               // test if this subtree qualifies.
               if (distance <= distanceK + indexNode->GetIndexEntry(idx).Radius){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTMPValue.PageID = indexNode->GetIndexEntry(idx).PageID;
                  pqTMPValue.Radius = indexNode->GetIndexEntry(idx).Radius;
                  queue->Add(distance, pqTMPValue);
               }//end if
            }//end if
         }//end for

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      distanceK){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // is it a Representative?
               if (leafNode->GetLeafEntry(idx).Distance!=0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if
               // KorRange part
               if (distance <= distanceK){
                  //Add in the result.
                  result->AddPair(tmpObj.Clone(), distance);
                  // distanceK will never be smaller than range
                  if (distanceK > range){//Nearest > Range then to cut the result is possible

                     if (result->GetNumOfEntries() >= k){
                        //cut if there is more than k elements
                        result->Cut(k); // This depends on tie list too
                        if (result->GetMaximumDistance() <= range)  //Range > Nearest
                           distanceK = range; //Query radius is range
                        else //Query radius is the farthest object (last nearest)
                           distanceK = result->GetMaximumDistance();
                     }//end if
                  }//end if
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // Next node...
      stop = false;
      do{
         if (queue->Get(distance, pqCurrValue)){
            // Qualified if distance <= rangeK + radius
            if (distance <= distanceK + pqCurrValue.Radius){
               // Yes, get the pageID and the distance from the representative
               // and the query object.
               distanceRepres = distance;
               // Break the while.
               stop = true;
            }//end if
         }else{
            // the queue is empty!
            pqCurrValue.PageID = 0;
            // Break the while.
            stop = true;
         }//end if
      }while (!stop);
   }// end while

   // Release the Global Priority Queue
   delete queue;
}//end stSlimTree<ObjectType, EvaluatorType>::KOrRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stSlimTree::RingQuery(
      ObjectType * sample, stDistance inRange, stDistance outRange){
   tResult * result = new tResult();  // Create result
   stDistance distanceRepres = 0;

   result->SetQueryInfo(sample->Clone(), tResult::RINGQUERY, -1,
                        outRange, inRange);

   // check if inRange is smaller than outRange
   if (inRange < outRange){
      // Let's search
      if (this->GetRoot() != 0){
         this->RingQuery(this->GetRoot(), result, sample, inRange, outRange,
                         distanceRepres);
      }//end if
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::RingQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::RingQuery(
         stPageID pageID, tResult * result, ObjectType * sample,
         stDistance inRange, stDistance outRange, stDistance distanceRepres){

   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;
   tPriorityQueue * queue;
   stCount pid;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // Priority queue
         queue = new tPriorityQueue(numberOfEntries);
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      outRange + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

               if ((distance <= outRange + indexNode->GetIndexEntry(idx).Radius) &&
                   (distance + indexNode->GetIndexEntry(idx).Radius > inRange)){
                  // Yes! I'm qualified !
                  queue->Add(distance, idx);
               }//end if
            }//end if
         }//end for

         while (queue->Get(distance, pid)){
            // Will qualify ?
            if ((distance <= outRange + indexNode->GetIndexEntry(pid).Radius) &&
                (distance + indexNode->GetIndexEntry(pid).Radius > inRange)){

               // Yes! I'm qualified !
               this->RingQuery(indexNode->GetIndexEntry(pid).PageID, result,
                     sample, inRange, outRange, distance);
            }//end if
         }//end while

         // Release queue.
         delete queue;

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      outRange){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // is it a Representative?
               if (leafNode->GetLeafEntry(idx).Distance!=0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if
               //test if the object qualify
               if ((distance <= outRange) && (distance > inRange)){
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

}//end stSlimTree<ObjectType, EvaluatorType>::RingQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stSlimTree::LocalKRingQuery(
            ObjectType * sample, stDistance inRange, stDistance outRange,
            stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   stDistance distanceRepres = 0;

   //fix this, it is wrong
   result->SetQueryInfo(sample->Clone(), tResult::KRINGQUERY, -1,
                        outRange, inRange, tie);

   // check if inRange is smaller than outRange
   if (inRange < outRange){
      // Let's search
      if (this->GetRoot() != 0){
         this->LocalKRingQuery(this->GetRoot(), result, sample, inRange,
                               outRange, k, distanceRepres);
      }//end if
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::LocalKRingQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::LocalKRingQuery(
         stPageID pageID, tResult * result, ObjectType * sample,
         stDistance inRange, stDistance & outRange, stCount k,
         stDistance distanceRepres){

   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;
   tPriorityQueue * queue;
   stCount pid;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // Priority queue
         queue = new tPriorityQueue(numberOfEntries);
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // use of the triangle inequality to cut a subtree
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      outRange + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

               if ((distance <= outRange + indexNode->GetIndexEntry(idx).Radius) &&
                   (distance + indexNode->GetIndexEntry(idx).Radius > inRange)){
                  // Yes! I'm qualified !
                  queue->Add(distance, idx);
               }//end if
            }//end if
         }//end for

         while (queue->Get(distance, pid)){
            // Will qualify ?
            if ((distance <= outRange + indexNode->GetIndexEntry(pid).Radius) &&
                (distance + indexNode->GetIndexEntry(pid).Radius > inRange)){

               // Yes! I'm qualified !
               this->LocalKRingQuery(indexNode->GetIndexEntry(pid).PageID, result,
                                     sample, inRange, outRange, k, distance);
            }//end if
         }//end while

         // Release queue.
         delete queue;

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      outRange){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // is it a Representative?
               if (leafNode->GetLeafEntry(idx).Distance!=0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if
               //test if the object qualify
               if ((distance <= outRange) && (distance > inRange)){
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
                  // there is more than k elements?
                  if (result->GetNumOfEntries() >= k){
                     //cut if there is more than k elements
                     result->Cut(k);
                     //may I use this for performance?
                     outRange = result->GetMaximumDistance();
                  }//end if
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

}//end stSlimTree<ObjectType, EvaluatorType>::LocalKRingQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::KRingQuery(
            ObjectType * sample, stDistance inRange, stDistance outRange,
            stCount k, bool tie){
   tResult * result = new tResult();  // Create result

   //fix this, it is wrong
   result->SetQueryInfo(sample->Clone(), tResult::KRINGQUERY, -1,
                        outRange, inRange, tie);

   // check if inRange is smaller than outRange
   if (inRange < outRange){
      // Let's search
      if (this->GetRoot() != 0){
         this->KRingQuery(result, sample, inRange, outRange, k);
      }//end if
   }//end if

   return result;
}//end KRingQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::KRingQuery(
         tResult * result, ObjectType * sample,
         stDistance inRange, stDistance & outRange, stCount k){

   tDynamicPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTMPValue;
   stDistance distance;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   bool stop;

   // Root node.
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;

   // Create the Global Priority Queue
   queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      outRange + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

               if ((distance <= outRange + indexNode->GetIndexEntry(idx).Radius) &&
                   (distance + indexNode->GetIndexEntry(idx).Radius > inRange)){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTMPValue.PageID = indexNode->GetIndexEntry(idx).PageID;
                  pqTMPValue.Radius = indexNode->GetIndexEntry(idx).Radius;
                  queue->Add(distance, pqTMPValue);
               }//end if
            }//end if
         }//end for

      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      outRange){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // is it a Representative?
               if (leafNode->GetLeafEntry(idx).Distance!=0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if
               //test if the object qualify
               if ((distance <= outRange) && (distance > inRange)){
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
                  // there is more than k elements?
                  if (result->GetNumOfEntries() >= k){
                     //cut if there is more than k elements
                     result->Cut(k);
                     //may I use this for performance?
                     outRange = result->GetMaximumDistance();
                  }//end if
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // Next node...
      stop = false;
      do{
         if (queue->Get(distance, pqCurrValue)){
            // Qualified if distance <= outRange + radius && distance + radius > inRange
            if ((distance <= outRange + pqCurrValue.Radius) &&
                  (distance + pqCurrValue.Radius > inRange)){
               // Yes, get the pageID and the distance from the representative
               // and the query object.
               distanceRepres = distance;
               // Break the while.
               stop = true;
            }//end if
         }else{
            // the queue is empty!
            pqCurrValue.PageID = 0;
            // Break the while.
            stop = true;
         }//end if
      }while (!stop);
   }// end while

   // Release the Global Priority Queue
   delete queue;
}//end stSlimTree<ObjectType, EvaluatorType>::KRingQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::IncrementalListNearestQuery(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   tGenericPriorityQueue * globalQueue;

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);

   // Let's search
   if (this->GetRoot() != 0){
      globalQueue = new tGenericPriorityQueue();
      // Call the initialize to the incremental nearest.
      this->InitializeIncrementalNearestQuery(sample, k, result, globalQueue);
      // Call the query itself.
      this->IncrementalNearestQuery(sample, k, result, globalQueue);
      delete globalQueue;
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::IncrementalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::InitializeIncrementalNearestQuery(
      ObjectType * sample, stCount k, tResult * result, tGenericPriorityQueue * globalQueue){

   stPage * rootPage;
   stSlimNode * rootNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;

   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE);

   if (this->GetRoot() != 0){
      rootPage = this->myPageManager->GetPage(this->GetRoot());
      rootNode = stSlimNode::CreateNode(rootPage);
      // Is it a Index node?
      if (rootNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)rootNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(indexNode->GetObject(idx),
                               indexNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // Put the Node in the Queue.
            globalQueue->Add(tmpObj.Clone(), indexNode->GetIndexEntry(idx).PageID, distance,
                             indexNode->GetIndexEntry(idx).Radius, tGenericEntry::NODE);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)rootNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(leafNode->GetObject(idx),
                               leafNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // Put the Objects in the Queue.
            globalQueue->Add(tmpObj.Clone(), distance, tGenericEntry::OBJECT);
         }//end for
      }//end if

      //Free it all
      delete rootNode;
      this->myPageManager->ReleasePage(rootPage);
   }//end if

}//end stSlimTree<ObjectType, EvaluatorType>::InitializeIncrementalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::IncrementalNearestQuery(
         ObjectType * sample, stCount k, tResult * result, tGenericPriorityQueue * globalQueue){
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;
   tGenericEntry * entryNode = NULL;
   bool stop = false;

   do{
      entryNode = globalQueue->Get();
      switch (entryNode->GetType()){
         case tGenericEntry::NODE:
            // Read node...
            currPage = this->myPageManager->GetPage(entryNode->GetPageID());
            currNode = stSlimNode::CreateNode(currPage);
            // Is it a Index node?
            if (currNode->GetNodeType() == stSlimNode::INDEX) {
               // Get Index node
               stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
               numberOfEntries = indexNode->GetNumberOfEntries();

               // for each entry...
               // Put the Children in the global priority queue.
               for (idx = 0; idx < numberOfEntries; idx++) {
                  // Rebuild the object
                  tmpObj.Unserialize(indexNode->GetObject(idx),
                                     indexNode->GetObjectSize(idx));
                  globalQueue->Add(tmpObj.Clone(), indexNode->GetIndexEntry(idx).PageID,
                                   entryNode->GetDistanceRepQuery(), indexNode->GetIndexEntry(idx).Distance,
                                   indexNode->GetIndexEntry(idx).Radius, tGenericEntry::APPROXIMATENODE);
               }//end for
            }else{
               // No, it is a leaf node. Get it.
               stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
               numberOfEntries = leafNode->GetNumberOfEntries();
               // for each entry...
               for (idx = 0; idx < numberOfEntries; idx++) {
                  // Rebuild the object
                  tmpObj.Unserialize(leafNode->GetObject(idx),
                                     leafNode->GetObjectSize(idx));
                  globalQueue->Add(tmpObj.Clone(), leafNode->GetLeafEntry(idx).Distance,
                                   entryNode->GetDistanceRepQuery(), tGenericEntry::APPROXIMATEOBJECT);
               }//end for
            }//end if
            //Free it all
            delete currNode;
            this->myPageManager->ReleasePage(currPage);
            break;
         case tGenericEntry::APPROXIMATENODE :
            distance = this->myMetricEvaluator->GetDistance(entryNode->GetObject(), sample);
            globalQueue->Add(entryNode->GetObject(), entryNode->GetPageID(), distance,
                             entryNode->GetRadius(), tGenericEntry::NODE);
            //this entry does not has the object!
            entryNode->SetMine(false);
            break;
         case tGenericEntry::APPROXIMATEOBJECT :
            distance = this->myMetricEvaluator->GetDistance(entryNode->GetObject(), sample);
            globalQueue->Add(entryNode->GetObject(), distance, tGenericEntry::OBJECT);
            //this entry does not has the object!
            entryNode->SetMine(false);
            break;
         case tGenericEntry::OBJECT :
            // Add the object.
            result->AddPair(entryNode->GetObject(), entryNode->GetDistanceQuery());
            //this entry does not has the object!
            entryNode->SetMine(false);
            // is it reach k elements?
            if (result->GetNumOfEntries() == k){
               stop = true;
            }//end if
            break;
      }//end switch

      // Release this entry.
      delete entryNode;

   }while (!stop && !globalQueue->IsEmpty());

}//end stSlimTree<ObjectType, EvaluatorType>::IncrementalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::IncrementalNearestQuery(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   tPGenericHeap * genericHeap = NULL;

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);

   // Let's search
   if (this->GetRoot() != 0){
      genericHeap = new tPGenericHeap(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);
      // Call the initialize to the incremental nearest.
      this->InitializeIncrementalNearestQuery(sample, k, result, genericHeap);
      // Call the query itself.
      this->IncrementalNearestQuery(sample, k, result, genericHeap);
      delete genericHeap;
   }//end if

   return result;
}//end stSlimTree<ObjectType, EvaluatorType>::IncrementalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::InitializeIncrementalNearestQuery(
      ObjectType * sample, stCount k, tResult * result, tPGenericHeap * globalQueue){

   stPage * rootPage;
   stSlimNode * rootNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;

   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE);

   if (this->GetRoot() != 0){
      rootPage = this->myPageManager->GetPage(this->GetRoot());
      rootNode = stSlimNode::CreateNode(rootPage);
      // Is it a Index node?
      if (rootNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)rootNode;
         numberOfEntries = indexNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(indexNode->GetObject(idx),
                               indexNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // Put the Node in the Queue.
            globalQueue->Add(tmpObj.Clone(), indexNode->GetIndexEntry(idx).PageID,
                             distance, 0, 0,
                             indexNode->GetIndexEntry(idx).Radius, 0, tPGenericHeap::NODE);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)rootNode;
         numberOfEntries = leafNode->GetNumberOfEntries();
         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(leafNode->GetObject(idx),
                               leafNode->GetObjectSize(idx));
            // Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            // Put the Objects in the Queue.
            globalQueue->Add(tmpObj.Clone(), -1,
                             distance, 0, 0,
                             0, 0, tPGenericHeap::OBJECT);
         }//end for
      }//end if
      //Free it all
      delete rootNode;
      this->myPageManager->ReleasePage(rootPage);
   }//end if

}//end stSlimTree<ObjectType, EvaluatorType>::InitializeIncrementalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::IncrementalNearestQuery(
         ObjectType * sample, stCount k, tResult * result, tPGenericHeap * globalQueue){
   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   ObjectType * object;
   stPageID pageID;
   stDistance distance;
   stDistance distanceRep;
   stDistance distanceQuery;
   stDistance distanceRepQuery;
   stDistance radius;
   stCount height;
   enum tPGenericHeap::tType type;
   stCount idx;
   stCount numberOfEntries;
   bool stop = false;

   while (!stop && globalQueue->Get(object, pageID,
                                    distanceQuery, distanceRep, distanceRepQuery,
                                    radius, height, type)){

      switch (type){
         case tPGenericHeap::NODE:
            // Read node...
            currPage = this->myPageManager->GetPage(pageID);
            currNode = stSlimNode::CreateNode(currPage);
            // Is it a Index node?
            if (currNode->GetNodeType() == stSlimNode::INDEX) {
               // Get Index node
               stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
               numberOfEntries = indexNode->GetNumberOfEntries();

               // for each entry...
               // Put the Children in the global priority queue.
               for (idx = 0; idx < numberOfEntries; idx++) {
                  // Rebuild the object
                  tmpObj.Unserialize(indexNode->GetObject(idx),
                                     indexNode->GetObjectSize(idx));

                  globalQueue->Add(tmpObj.Clone(), indexNode->GetIndexEntry(idx).PageID,
                                   0, indexNode->GetIndexEntry(idx).Distance, distanceQuery,
                                   indexNode->GetIndexEntry(idx).Radius, height+1, tPGenericHeap::APPROXIMATENODE);
               }//end for
            }else{
               // No, it is a leaf node. Get it.
               stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
               numberOfEntries = leafNode->GetNumberOfEntries();
               // for each entry...
               for (idx = 0; idx < numberOfEntries; idx++) {
                  // Rebuild the object
                  tmpObj.Unserialize(leafNode->GetObject(idx),
                                     leafNode->GetObjectSize(idx));

                  globalQueue->Add(tmpObj.Clone(), -1,
                                   0, leafNode->GetLeafEntry(idx).Distance, distanceQuery,
                                   0, height + 1, tPGenericHeap::APPROXIMATEOBJECT);
               }//end for
            }//end if
            //Free it all
            delete currNode;
            this->myPageManager->ReleasePage(currPage);
            break;//end NODE
         case tPGenericHeap::APPROXIMATENODE :
            distance = this->myMetricEvaluator->GetDistance(object, sample);
            globalQueue->Add(object, pageID,
                             distance, distanceRep, distanceRepQuery,
                             radius, height, tPGenericHeap::NODE);
            break;//end APPROXIMATENODE
         case tPGenericHeap::APPROXIMATEOBJECT :
            distance = this->myMetricEvaluator->GetDistance(object, sample);
            globalQueue->Add(object, -1,
                             distance, 0, 0,
                             0, height, tPGenericHeap::OBJECT);
            break;//end APPROXIMATEOBJECT
         case tPGenericHeap::OBJECT :
            // Add the object.
            result->AddPair(object, distanceQuery);
            // is it reach k elements?
            if (result->GetNumOfEntries() == k){
               stop = true;
            }//end if OBJECT
            break;//end
      }//end switch
   }//end do

}//end stSlimTree<ObjectType, EvaluatorType>::IncrementalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stSlimTree<ObjectType, EvaluatorType>::LazyRangeQuery(
      ObjectType * sample, stDistance range, stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   stDistance distanceRepres = 0;
   bool stop = false;   // for what?

   result->SetQueryInfo(sample->Clone(), tResult::LAZYRANGEQUERY, k, range, tie);

   // Let's search
   if (this->GetRoot() != 0){
      this->LazyRangeQuery(this->GetRoot(), result, sample, range, k,
                           distanceRepres, stop);
   }//end if

   return result;
}//end LazyRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stSlimTree<ObjectType, EvaluatorType>::LazyRangeQuery(
         stPageID pageID, tResult * result, ObjectType * sample,
         stDistance & range, stCount k, stDistance distanceRepres,
         bool & stop){

   stPage * currPage;
   stSlimNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         for (idx = 0; (idx < numberOfEntries) && !stop; idx++) {
            // try to cut this subtree with the triangle inequality.
            if ( fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      range + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

               if (distance <= range + indexNode->GetIndexEntry(idx).Radius){
                  // Yes! I'm qualified !
                  this->LazyRangeQuery(indexNode->GetIndexEntry(idx).PageID, result,
                                       sample, range, k, distance, stop);
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         for (idx = 0; (idx < numberOfEntries) && !stop; idx++) {
            // try to cut this object with the triangle inequality.
            if ( fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      range){
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));

               if (leafNode->GetLeafEntry(idx).Distance!=0) {// is it a Representative?
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= range){
                  // Yes! I'm qualified !
                  result->AddPair(tmpObj.Clone(), distance);
                  // if has k elements, stop!
                  if (result->GetNumOfEntries() == k)
                     // cut the rest!
                     stop = true;
               }//end if
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

}//end LazyRangeQuery

//==============================================================================
// End of Queries
//------------------------------------------------------------------------------

// Visualization support
#ifdef __stMAMVIEW__

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::MAMViewInit(){
   tObjectSample * sample;
   int size;

   size = int(float(this->GetNumberOfObjects()) * 0.1);
   if (size < 200){
      size = 200;
   }//end if

   sample = GetSample(size);
   MAMViewer->Init(sample);
   delete sample;
}//end stSlimTree<ObjectType, EvaluatorType>::MAMViewInit

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stMAMViewObjectSample <ObjectType>
* stSlimTree<ObjectType, EvaluatorType>::GetSample(int sampleSize){
   tObjectSample * sample;

   // Create the sample object.
   sample = new tObjectSample(sampleSize, GetNumberOfObjects());

   // Get them!
   GetSampleRecursive(this->GetRoot(), sample);

   return sample;
}//end stSlimTree<ObjectType, EvaluatorType>::GetSample

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::GetSampleRecursive(
      stPageID pageID, tmpl_stSlimTree::tObjectSample * sample){
   stPage * currPage;
   stSlimNode * currNode;
   stCount i;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;

         // for each entry call GetSampleRecursive
         for (i = 0; i < indexNode->GetNumberOfEntries(); i++) {
            GetSampleRecursive(indexNode->GetIndexEntry(i).PageID, sample);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;
         ObjectType tmp;

         for (i = 0; i < leafNode->GetNumberOfEntries(); i++) {
            if (sample->MayAdd()){
               // YES! I'll add it now.
               tmp.Unserialize(leafNode->GetObject(i),
                               leafNode->GetObjectSize(i));
               sample->Add(tmp.Clone());
            }//end if
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::GetSampleRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::MAMViewDumpTree(){

   MAMViewer->BeginAnimation("Full tree dump.");
   MAMViewer->BeginFrame(NULL);
   MAMViewDumpTreeRecursive(this->GetRoot(), NULL, 0, 0);
   MAMViewer->EndFrame();
   MAMViewer->EndAnimation();
}//end stSlimTree<ObjectType, EvaluatorType>::MAMViewDumpTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::MAMViewDumpTreeRecursive(stPageID pageID, ObjectType * rep,
            stDistance radius, stPageID parent){
   stPage * currPage;
   stSlimNode * currNode;
   stCount i;
   ObjectType tmp;

   // Level Up
   MAMViewer->LevelUp();

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;

         // Add this node
         if (rep != NULL){
            MAMViewer->SetNode(pageID, rep, radius, parent, 0, false);
         }//end if

         // Adding all entries of this node.
         for (i = 0; i < indexNode->GetNumberOfEntries(); i++) {
            tmp.Unserialize(indexNode->GetObject(i),
                  indexNode->GetObjectSize(i));
            MAMViewDumpTreeRecursive(indexNode->GetIndexEntry(i).PageID, &tmp,
                  indexNode->GetIndexEntry(i).Radius, pageID);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;

         // Add this node
         if (rep != NULL){
            MAMViewer->SetNode(pageID, rep, radius, parent, 1, false);
         }//end if

         for (i = 0; i < leafNode->GetNumberOfEntries(); i++) {
            tmp.Unserialize(leafNode->GetObject(i),
                  leafNode->GetObjectSize(i));
            MAMViewer->SetObject(&tmp, pageID, false);
         }//end for
      }//end else

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   // Level Down
   MAMViewer->LevelDown();
}//end stSlimTree<ObjectType, EvaluatorType>::MAMViewDumpTree

#endif //__stMAMVIEW__

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stTreeInfoResult * tmpl_stSlimTree::GetTreeInfo(){
   stTreeInformation * info;

   // No cache of information. I think a cahe would be a good idea.
   info = new stTreeInformation(GetHeight(), GetNumberOfObjects());

   // Let's get the information!
   GetTreeInfoRecursive(this->GetRoot(), 0, info);

   // Optimal tree
   if (info->GetMeanObjectSize() != 0){
      info->CalculateOptimalTreeInfo(int(this->myPageManager->GetMinimumPageSize() /
            info->GetMeanObjectSize()));
   }//end if

   return info;
}//end stSlimTree<ObjectType, EvaluatorType>::GetTreeInfo

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::GetTreeInfoRecursive(stPageID pageID, int level,
      stTreeInformation * info){
   stPage * currPage;
   stSlimNode * currNode;
   stCount i;
   ObjectType tmp;

   // Let's search
   if (pageID != 0){
      // Update node count
      info->UpdateNodeCount(level);

      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;

         // Object count
         info->UpdateObjectCount(level, indexNode->GetNumberOfEntries());

         // Scan all entries
         for (i = 0; i < indexNode->GetNumberOfEntries(); i++){
            GetTreeInfoRecursive(indexNode->GetIndexEntry(i).PageID, level + 1,
                                 info);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stSlimLeafNode * leafNode = (stSlimLeafNode *)currNode;

         // Object count
         info->UpdateObjectCount(level, leafNode->GetNumberOfEntries());

         // Update object count
         for (i = 0; i < leafNode->GetNumberOfEntries(); i++){
            // Update other statistics
            info->UpdateMeanObjectSize(leafNode->GetObjectSize(i));

            // Compute intersections
            tmp.Unserialize(leafNode->GetObject(i),
                            leafNode->GetObjectSize(i));

            // Compute intersections
            ObjectIntersectionsRecursive(this->GetRoot(), &tmp, 0, info);
         }//end for
      }//end if

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::GetTreeInfoRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::ObjectIntersectionsRecursive(stPageID pageID,
      ObjectType * obj, int level, stTreeInformation * info){
   stPage * currPage;
   stSlimNode * currNode;
   stCount i;
   ObjectType tmp;
   stDistance d;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stSlimNode::INDEX) {
         // Get Index node
         stSlimIndexNode * indexNode = (stSlimIndexNode *)currNode;

         // Scan all entries
         for (i = 0; i < indexNode->GetNumberOfEntries(); i++){
            tmp.Unserialize(indexNode->GetObject(i),
                            indexNode->GetObjectSize(i));
            d = this->myMetricEvaluator->GetDistance(&tmp, obj);
            if (d <= indexNode->GetIndexEntry(i).Radius){
               // Intersection !!!!
               info->UpdateIntersections(level);
               ObjectIntersectionsRecursive(indexNode->GetIndexEntry(i).PageID,
                                            obj, level + 1, info);
            }//end if
         }//end for
      }//end if

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::ObjectIntersectionsRecursive

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::Optimize(){

   if (this->GetHeight() >= 3){
      SlimDownRecursive(this->GetRoot(), 0);
      // Notify modifications.
      HeaderUpdate = true;
      // Don't worry. This is a debug block!!!
   #ifdef __stPRINTMSG__
   }else{
      cout << "Unable to perform the Slim-Down. This tree has only " <<
         this->GetHeight() << " level(s).\n";
   #endif //__stPRINTMSG__
   }//end if
}//end tmpl_stSlimTree::Optimize

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance tmpl_stSlimTree::SlimDownRecursive(stPageID pageID, int level){
   stPage * currPage;
   stSlimNode * currNode;
   stSlimIndexNode * indexNode;
   stDistance radius;
   stCount i;

   // Let's search
   if (pageID != 0){

      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);

      #ifdef __stPRINTMSG__
         if (currNode->GetNodeType() != stSlimNode::INDEX){
            // This tree has less than 3 levels. This method will not work.
            throw logic_error("Slimdown reached the bottom of the tree.");
         }//end if
      #endif //__stPRINTMSG__

      indexNode = (stSlimIndexNode *)currNode;

      // Where am I ?
      if (level == GetHeight() - 3){
         // Slim-Down next level!
         for (i = 0; i < indexNode->GetNumberOfEntries(); i++){
            #ifdef __stPRINTMSG__
               cout << "Level:" << level << ". Begin of the local slim down of " <<
                     indexNode->GetIndexEntry(i).PageID <<
                     " which current radius is " <<
                     indexNode->GetIndexEntry(i).Radius << ".\n";
            #endif //__stPRINTMSG__
            indexNode->GetIndexEntry(i).Radius = SlimDown(
                  indexNode->GetIndexEntry(i).PageID);

            #ifdef __stPRINTMSG__
               cout << "Level:" << level << ". End of the local slim down of " <<
                     indexNode->GetIndexEntry(i).PageID <<
                     " which current radius is " <<
                     indexNode->GetIndexEntry(i).Radius << ".\n";
            #endif //__stPRINTMSG__

         }//end for
      }else{
         // Move on...
         for (i = 0; i < indexNode->GetNumberOfEntries(); i++){
            indexNode->GetIndexEntry(i).Radius = SlimDownRecursive(
                  indexNode->GetIndexEntry(i).PageID, level + 1);
         }//end for
      }//end if

      // Update my radius.
      radius = indexNode->GetMinimumRadius();

      // Write me and get the garbage.
      delete currNode;
      this->myPageManager->WritePage(currPage);
      this->myPageManager->ReleasePage(currPage);
      return radius;
   }else{
      // This tree is corrupted or is empty.
      throw logic_error("The given tree is corrupted or empty.");
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::SlimDownRecursive

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance tmpl_stSlimTree::SlimDown(stPageID pageID){
   stPage * currPage;
   stSlimNode * currNode;
   stSlimIndexNode * indexNode;
   tMemLeafNode ** memLeafNodes;
   stSlimNode * tmpNode;
   stPage * tmpPage;
   stSlimLeafNode * leafNode;
   stDistance radius;
   int maxSwaps;
   stCount nodeCount;
   stCount idx;
   stCount i;

   // Let's search
   if (pageID != 0){

      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stSlimNode::CreateNode(currPage);

      #ifdef __stPRINTMSG__
         if (currNode->GetNodeType() != stSlimNode::INDEX){
            // This tree has less than 3 levels. This method will not work.
            throw logic_error("Slimdown reached the bottom of the tree.");
         }//end if
      #endif //__stPRINTMSG__

      // Cast currNode to stSlimIndexNode as it must be...
      indexNode = (stSlimIndexNode *)currNode;
      nodeCount = indexNode->GetNumberOfEntries();

      #ifdef __stPRINTMSG__
         cout << "Local Slimdown in " <<
               indexNode->GetPageID() <<
               " which current radius is " <<
               indexNode->GetMinimumRadius() << ".\n";
      #endif //__stPRINTMSG__


      // Create  all stSlimMemLeafNodes
      memLeafNodes = new tMemLeafNode * [nodeCount];
      maxSwaps = 0;
      for (i = 0; i < nodeCount; i++){
         // Read leaf
         tmpPage = this->myPageManager->GetPage(indexNode->GetIndexEntry(i).PageID);
         tmpNode = stSlimNode::CreateNode(tmpPage);

         #ifdef __stPRINTMSG__
            if (tmpNode->GetNodeType() != stSlimNode::LEAF){
               // This tree has less than 3 levels. This method will not work.
               throw logic_error("Oops. This tree is corrupted.");
            }//end if
         #endif //__stPRINTMSG__
         leafNode = (stSlimLeafNode *) tmpNode;

         // Update maxSwaps
         maxSwaps += leafNode->GetNumberOfEntries();

         // Assemble memory version
         memLeafNodes[i] = new tMemLeafNode(leafNode);
      }//end for
      maxSwaps *= 3;

      // Execute the local SlimDown
      LocalSlimDown(memLeafNodes, nodeCount, maxSwaps);

      // Rebuild nodes and write them. Of course, the empty ones will be disposed.
      idx = 0;
      for (i = 0; i < nodeCount; i++){
         // Dispose memory version
         if (memLeafNodes[i]->GetNumberOfEntries() != 0){
            leafNode = memLeafNodes[i]->ReleaseNode();
            delete memLeafNodes[i];

            // Update entry
            indexNode->GetIndexEntry(idx).NEntries = leafNode->GetNumberOfEntries();
            indexNode->GetIndexEntry(idx).Radius = leafNode->GetMinimumRadius();
            idx++;

            // Write back
            tmpPage = leafNode->GetPage();
            delete leafNode;
            this->myPageManager->WritePage(tmpPage);
            this->myPageManager->ReleasePage(tmpPage);
         }else{
            // Empty node
            leafNode = memLeafNodes[i]->ReleaseNode();
            delete memLeafNodes[i];

            // Remove entry
            indexNode->RemoveEntry(idx);
            #ifdef __stPRINTMSG__
               cout << "Node " << i << " is no more!\n";
            #endif //__stPRINTMSG__

            // Dispose empty node
            tmpPage = leafNode->GetPage();
            delete leafNode;
            DisposePage(tmpPage);
         }//end if
      }//end for
      delete[] memLeafNodes;

      // Update my radius.
      radius = indexNode->GetMinimumRadius();

      // Write me and get the garbage.
      delete currNode;
      this->myPageManager->WritePage(currPage);
      this->myPageManager->ReleasePage(currPage);
      return radius;
   }else{
      // This tree is corrupted or is empty.
      throw logic_error("The given tree is corrupted or empty.");
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::SlimDown

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stSlimTree::LocalSlimDown(
      tmpl_stSlimTree::tMemLeafNode ** memLeafNodes, int nodeCount,
      int maxSwaps){
   bool stop;
   int src;
   int dst;
   int i;
   int localSwapCount;
   int swapCount = 0;
   stDistance minDist;
   stDistance tmpDist;

   // main loop
   stop = false;
   while (!stop){
      // Try to swap them
      localSwapCount = 0;
      for (src = 0; src < nodeCount; src++){
         if (memLeafNodes[src]->GetNumberOfEntries() > 0){
            // Look for the target...
            dst = -1;
            minDist = MAXDOUBLE;
            for (i = 0; i < nodeCount; i++){
               if (i != src){
                  if (SlimDownCanSwap(memLeafNodes[src], memLeafNodes[i],
                        tmpDist)){
                     if (tmpDist < minDist){
                        dst = i;
                        minDist = tmpDist;
                     }//end if
                  }//end if
               }//end if
            }//end for

            // Swap!
            if (dst != -1){
               // Update swap count
               localSwapCount++;

               // Swap!
               memLeafNodes[dst]->Add(memLeafNodes[src]->PopObject(), minDist);
            }//end if
         //}else{
            // This node is empty.
         }//end if
      }//end for

      // Stop condition
      swapCount += localSwapCount;
      stop = (swapCount > maxSwaps) || (localSwapCount == 0);
   }//end while
}//end stSlimTree<ObjectType, EvaluatorType>::LocalSlimDown

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool tmpl_stSlimTree::SlimDownCanSwap(
      tmpl_stSlimTree::tMemLeafNode * src, tmpl_stSlimTree::tMemLeafNode * dst,
      stDistance & distance){

   // Check to see if destination is empty
   if (dst->GetNumberOfEntries() == 0){
      return false;
   }//end if

   // Calculate the distance between src's last object and dst's representative
   distance = this->myMetricEvaluator->GetDistance(src->LastObject(), dst->RepObject());

   // Test distances and occupation
   if (distance <= dst->GetMinimumRadius()){
      if (dst->CanAdd(src->LastObject())){
         return true;
      }else{
         // Will not fit.
         return false;
      }//end if
   }else{
      // dst does not cover
      return false;
   }//end if
}//end stSlimTree<ObjectType, EvaluatorType>::SlimDownIntersects

//-----------------------------------------------------------------------------

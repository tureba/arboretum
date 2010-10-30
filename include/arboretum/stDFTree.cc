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
* This file is the implementation of stDFTree methods.
*
* @version 1.0
* $Revision: 1.9 $
* $Date: 2005/03/08 19:43:09 $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Joselene Marques(joselene@icmc.usp.br)
*/
// Copyright (c) 2003 GBDI-ICMC-USP
//==============================================================================
// Class stDFLogicNode
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDFLogicNode<ObjectType, EvaluatorType>::stDFLogicNode(stCount maxOccupation){
   int i;

   // Allocate resources
   MaxEntries = maxOccupation;
   Entries = new stDFLogicEntry[maxOccupation];
   for (i = 0; i < maxOccupation; i++){
      Entries[i].FieldDistance = new stDistance[STFOCUS];
   }//end for

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
}//end stDFLogicNode<ObjectType, EvaluatorType>::stDFLogicNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDFLogicNode<ObjectType, EvaluatorType>::~stDFLogicNode(){
   int i;

   if (Entries != NULL){
      for (i = 0; i < Count; i++){
         if ((Entries[i].Object != NULL) && (Entries[i].Mine)){
            delete Entries[i].Object;
            delete Entries[i].FieldDistance;
         }//end if
      }//end for
   }//end if
   // Clean before exit.
   delete[] Entries;
}//end stDFLogicNode<ObjectType, EvaluatorType>::~stDFLogicNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFLogicNode<ObjectType, EvaluatorType>::AddEntry(stSize size,
      const stByte * object){
   if (Count < MaxEntries){
      Entries[Count].Object = new ObjectType();
      Entries[Count].FieldDistance  = new stDistance[STFOCUS];
      Entries[Count].Object->Unserialize(object, size);
      Entries[Count].Mine = true;
      Count++;
      return Count - 1;
   }else{
      return -1;
   }//end if
}//end stDFLogicNode<ObjectType, EvaluatorType>::AddEntry

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFLogicNode<ObjectType, EvaluatorType>::AddIndexNode(stDFIndexNode * node){
   int i;
   int idx;

   for (i = 0; i < node->GetNumberOfEntries(); i++){
      idx = AddEntry(node->GetObjectSize(i), node->GetObject(i));
      SetEntry(idx, node->GetIndexEntry(i).PageID,
                    node->GetIndexEntry(i).NEntries,
                    node->GetIndexEntry(i).Radius,
                    node->GetIndexEntry(i).FieldDistance);
   }//end for

   // Node type
   NodeType = stDFNode::INDEX;
}//end stDFLogicNode<ObjectType, EvaluatorType>::AddIndexNode
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFLogicNode<ObjectType, EvaluatorType>::AddLeafNode(stDFLeafNode * node){
   int i;
   int idx;

   for (i = 0; i < node->GetNumberOfEntries(); i++){
      idx = AddEntry(node->GetObjectSize(i), node->GetObject(i));
      SetEntry(idx, 0, 0, 0, node->GetLeafEntry(i).FieldDistance);
   }//end for

   // Node type
   NodeType = stDFNode::LEAF;
}//end stDFLogicNode<ObjectType, EvaluatorType>::AddLeafNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFLogicNode<ObjectType, EvaluatorType>::TestDistribution(
      stDFIndexNode * node0, stDFIndexNode * node1,
      EvaluatorType * metricEvaluator){
   int dCount;
   int idx;
   int i;
   int l0, l1;
   int currObj;
   stDistanceIndex *ind0, *ind1;

   // Setup Objects
   dCount = UpdateDistances(metricEvaluator);

   // Init Map and Sorting vector
   ind0 = new stDistanceIndex[Count];
   ind1 = new stDistanceIndex[Count];
   for (i = 0; i < Count; i++){
      ind0[i].Index = i;
      ind0[i].Distance = Entries[i].Distance[0];
      ind1[i].Index = i;
      ind1[i].Distance = Entries[i].Distance[1];
      Entries[i].Mapped = false;
   }//end for

   // Sorting by distance...
   sort(ind0, ind0 + Count);
   sort(ind1, ind1 + Count);

   // Make one of then get the minimum occupation.
   l0 = l1 = 0;

   // Adds at least 3 objects to each node.
   for (i = 0; i < MinOccupation; i++){
      // Find a candidate for node 0
      while (Entries[ind0[l0].Index].Mapped){
         l0++;
      }//end while
      // Add to node 0
      currObj = ind0[l0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());
      node0->GetIndexEntry(idx).Distance = ind0[l0].Distance;
      node0->GetIndexEntry(idx).PageID = Entries[currObj].PageID;
      node0->GetIndexEntry(idx).NEntries = Entries[currObj].NEntries;
      node0->GetIndexEntry(idx).Radius = Entries[currObj].Radius;
      memcpy(node0->GetIndexEntry(idx).FieldDistance,
             Entries[currObj].FieldDistance, STFOCUS * sizeof(stDistance));

      // Find a candidate for node 1
      while (Entries[ind1[l1].Index].Mapped){
         l1++;
      }//end while
      // Add to node 1
      currObj = ind1[l1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());
      node1->GetIndexEntry(idx).Distance = ind1[l1].Distance;
      node1->GetIndexEntry(idx).PageID = Entries[currObj].PageID;
      node1->GetIndexEntry(idx).NEntries = Entries[currObj].NEntries;
      node1->GetIndexEntry(idx).Radius = Entries[currObj].Radius;
      memcpy(node1->GetIndexEntry(idx).FieldDistance,
             Entries[currObj].FieldDistance, STFOCUS * sizeof(stDistance));
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
               memcpy(node0->GetIndexEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
            }else{
               // Let's put it in the node 1 since it doesn't fit in the node 0
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node1->GetIndexEntry(idx).Distance = Entries[i].Distance[1];
               node1->GetIndexEntry(idx).PageID = Entries[i].PageID;
               node1->GetIndexEntry(idx).NEntries = Entries[i].NEntries;
               node1->GetIndexEntry(idx).Radius = Entries[i].Radius;
               memcpy(node1->GetIndexEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
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
               memcpy(node1->GetIndexEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
            }else{
               // Let's put it in the node 0 since it doesn't fit in the node 1
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node0->GetIndexEntry(idx).Distance = Entries[i].Distance[0];
               node0->GetIndexEntry(idx).PageID = Entries[i].PageID;
               node0->GetIndexEntry(idx).NEntries = Entries[i].NEntries;
               node0->GetIndexEntry(idx).Radius = Entries[i].Radius;
               memcpy(node0->GetIndexEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete[] ind0;
   delete[] ind1;

   return dCount;
}//end stDFLogicNode<ObjectType, EvaluatorType>::TestDistribution

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFLogicNode<ObjectType, EvaluatorType>::TestDistribution(
      stDFLeafNode * node0, stDFLeafNode * node1,
      EvaluatorType * metricEvaluator){
   int dCount;
   int idx;
   int i;
   int l0, l1;
   int currObj;
   stDistanceIndex *ind0, *ind1;

   // Setup Objects
   dCount = UpdateDistances(metricEvaluator);

   // Init Map and Sorting vector
   ind0 = new stDistanceIndex[Count];
   ind1 = new stDistanceIndex[Count];
   for (i = 0; i < Count; i++){
      ind0[i].Index = i;
      ind0[i].Distance = Entries[i].Distance[0];
      ind1[i].Index = i;
      ind1[i].Distance = Entries[i].Distance[1];
      Entries[i].Mapped = false;
   }//end for

   // Sorting by distance...
   sort(ind0, ind0 + Count);
   sort(ind1, ind1 + Count);

   // Make one of then get the minimum occupation.
   l0 = l1 = 0;

   // Adds at least 3 objects to each node.
   for (i = 0; i < MinOccupation; i++){
      // Find a candidate for node 0
      while (Entries[ind0[l0].Index].Mapped){
         l0++;
      }//end while
      // Add to node 0
      currObj = ind0[l0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());
      node0->GetLeafEntry(idx).Distance = ind0[l0].Distance;
      memcpy(node0->GetLeafEntry(idx).FieldDistance,
             Entries[currObj].FieldDistance, STFOCUS * sizeof(stDistance));

      // Find a candidate for node 1
      while (Entries[ind1[l1].Index].Mapped){
         l1++;
      }//end while
      // Add to node 1
      currObj = ind1[l1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize());
      node1->GetLeafEntry(idx).Distance = ind1[l1].Distance;
      memcpy(node1->GetLeafEntry(idx).FieldDistance,
             Entries[currObj].FieldDistance, STFOCUS * sizeof(stDistance));
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
               memcpy(node0->GetLeafEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
            }else{
               // Let's put it in the node 1 since it doesn't fit in the node 0
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node1->GetLeafEntry(idx).Distance = Entries[i].Distance[1];
               memcpy(node1->GetLeafEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
            }//end if
         }else{
            // Try to put on node 1 first
            idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object->Serialize());
            if (idx >= 0){
               node1->GetLeafEntry(idx).Distance = Entries[i].Distance[1];
               memcpy(node1->GetLeafEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
            }else{
               // Let's put it in the node 0 since it doesn't fit in the node 1
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize());
               node0->GetLeafEntry(idx).Distance = Entries[i].Distance[0];
               memcpy(node0->GetLeafEntry(idx).FieldDistance,
                      Entries[i].FieldDistance, STFOCUS * sizeof(stDistance));
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete ind0;
   delete ind1;

   return dCount;
}//end stDFLogicNode<ObjectType, EvaluatorType>::TestDistribution

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFLogicNode<ObjectType, EvaluatorType>::UpdateDistances(
      EvaluatorType * metricEvaluator){
   int i;

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
}//end stDFLogicNode<ObjectType, EvaluatorType>::UpdateDistances

//=============================================================================
// Class template stDFMSTSpliter
//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDFMSTSplitter<ObjectType, EvaluatorType>::stDFMSTSplitter(
      tLogicNode * node){

   Node = node;
   N = Node->GetNumberOfEntries();

   // Dynamic fields
   Cluster = new tCluster[N];
   ObjectCluster = new int[N];

   // Matrix
   DMat.SetSize(N, N);

}//end stDFMSTSplitter<ObjectType, EvaluatorType>::stDFMSTSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDFMSTSplitter<ObjectType, EvaluatorType>::~stDFMSTSplitter(){

   if (Node != NULL){
      delete Node;
   }//end if
   if (Cluster != NULL){
      delete[] Cluster;
   }//end if
   if (ObjectCluster != NULL){
      delete[] ObjectCluster;
   }//end if
}//end stDFMSTSplitter<ObjectType, EvaluatorType>::stDFMSTSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFMSTSplitter<ObjectType, EvaluatorType>::BuildDistanceMatrix(
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
}//end stDFMSTSplitter<ObjectType, EvaluatorType>::BuildDistanceMatrix

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFMSTSplitter<ObjectType, EvaluatorType>::FindCenter(int clus){
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
}//end  stDFMSTSplitter<ObjectType, EvaluatorType>::FindCenter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFMSTSplitter<ObjectType, EvaluatorType>::PerformMST(tGR * GR){
   int i, j, k, l, cc, iBig, iBigOpposite, a, b , c, Center1, Center2;
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

   Center1 = FindCenter(Cluster0);
   Center2 = FindCenter(Cluster1);

   // Representatives
   Node->SetRepresentative( Center1, Center2);

   //  It's first splite, build the GR distances
   if (GR->FirstSplit()){
        GR->FindGlobalRep (Node, this, Center1, Center2, N);
   }//end if

}//end stDFMSTSplitter<ObjectType, EvaluatorType>::PerformMST

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFMSTSplitter<ObjectType, EvaluatorType>::Distribute(
            stDFIndexNode * node0, ObjectType * & rep0,
            stDFIndexNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator, tGR * GR){
   int dCount;
   int idx;
   int i;
   int objIdx;

   // Build Distance matrix
   dCount = BuildDistanceMatrix(metricEvaluator);

   //Perform MST
   PerformMST(GR);

   // Add representatives first
   idx = node0->AddEntry(Node->GetRepresentative(0)->GetSerializedSize(),
                         Node->GetRepresentative(0)->Serialize());
   objIdx = Node->GetRepresentativeIndex(0);
   node0->GetIndexEntry(idx).Distance = 0.0;
   node0->GetIndexEntry(idx).Radius = Node->GetRadius(objIdx);
   node0->GetIndexEntry(idx).NEntries = Node->GetNEntries(objIdx);
   node0->GetIndexEntry(idx).PageID = Node->GetPageID(objIdx);
   memcpy(node0->GetIndexEntry(idx).FieldDistance,
          Node->GetFieldDistance(objIdx), STFOCUS * sizeof(stDistance));

   idx = node1->AddEntry(Node->GetRepresentative(1)->GetSerializedSize(),
                         Node->GetRepresentative(1)->Serialize());
   objIdx = Node->GetRepresentativeIndex(1);
   node1->GetIndexEntry(idx).Distance = 0.0;
   node1->GetIndexEntry(idx).Radius = Node->GetRadius(objIdx);
   node1->GetIndexEntry(idx).NEntries = Node->GetNEntries(objIdx);
   node1->GetIndexEntry(idx).PageID = Node->GetPageID(objIdx);
   memcpy(node1->GetIndexEntry(idx).FieldDistance,
          Node->GetFieldDistance(objIdx), STFOCUS * sizeof(stDistance));

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
               memcpy(node0->GetIndexEntry(idx).FieldDistance,
                      Node->GetFieldDistance(i), STFOCUS * sizeof(stDistance));
            }else{
               // Oops! We must put it in other node
               idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node1->GetIndexEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(1)];
               node1->GetIndexEntry(idx).Radius = Node->GetRadius(i);
               node1->GetIndexEntry(idx).NEntries = Node->GetNEntries(i);
               node1->GetIndexEntry(idx).PageID = Node->GetPageID(i);
               memcpy(node1->GetIndexEntry(idx).FieldDistance,
                      Node->GetFieldDistance(i), STFOCUS * sizeof(stDistance));
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
               memcpy(node1->GetIndexEntry(idx).FieldDistance,
                      Node->GetFieldDistance(i), STFOCUS * sizeof(stDistance));
            }else{
               // Oops! We must put it in other node
               idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node0->GetIndexEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(0)];
               node0->GetIndexEntry(idx).Radius = Node->GetRadius(i);
               node0->GetIndexEntry(idx).NEntries = Node->GetNEntries(i);
               node0->GetIndexEntry(idx).PageID = Node->GetPageID(i);
               memcpy(node0->GetIndexEntry(idx).FieldDistance,
                      Node->GetFieldDistance(i), STFOCUS * sizeof(stDistance));
            }//end if
         }//end if
      }//end if
   }//end for

   // Representatives
   rep0 = Node->BuyObject(Node->GetRepresentativeIndex(0));
   rep1 = Node->BuyObject(Node->GetRepresentativeIndex(1));

   return dCount;
}//end stDFMSTSplitter<ObjectType, EvaluatorType>::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDFMSTSplitter<ObjectType, EvaluatorType>::Distribute(
            stDFLeafNode * node0, ObjectType * & rep0,
            stDFLeafNode * node1, ObjectType * & rep1,
            EvaluatorType * metricEvaluator, tGR * GR){
   int dCount;
   int idx;
   int i;

   // Build Distance matrix
   dCount = BuildDistanceMatrix(metricEvaluator);

   //Perform MST
   PerformMST(GR);

   // Add representatives first
   idx = node0->AddEntry(Node->GetRepresentative(0)->GetSerializedSize(),
                         Node->GetRepresentative(0)->Serialize());
   node0->GetLeafEntry(idx).Distance = 0.0;
   memcpy(node0->GetLeafEntry(idx).FieldDistance,
          Node->GetFieldDistance(Node->GetRepresentativeIndex(0)),
          STFOCUS * sizeof(stDistance));

   idx = node1->AddEntry(Node->GetRepresentative(1)->GetSerializedSize(),
                         Node->GetRepresentative(1)->Serialize());
   node1->GetLeafEntry(idx).Distance = 0.0;
   memcpy(node1->GetLeafEntry(idx).FieldDistance,
          Node->GetFieldDistance(Node->GetRepresentativeIndex(1)),
          STFOCUS * sizeof(stDistance));

   // Distribute us...
   for (i = 0; i < N; i++){
      if (!Node->IsRepresentative(i)){
         if (ObjectCluster[i] == Cluster0){
            idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize());
            if (idx >= 0){
               // Insertion Ok!
               node0->GetLeafEntry(idx).Distance =  DMat[i][Node->GetRepresentativeIndex(0)];
               GR->SetFieldDistance(node0, idx, (ObjectType *)  Node->GetObject(i));
            }else{
               // Oops! We must put it in other node
               idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node1->GetLeafEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(1)];
               GR->SetFieldDistance(node1, idx, (ObjectType *) Node->GetObject(i));
            }//end if
         }else{
            idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize());
            if (idx >= 0){
               // Insertion Ok!
               node1->GetLeafEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(1)];
               GR->SetFieldDistance(node1, idx, (ObjectType *) Node->GetObject(i));
            }else{
               // Oops! We must put it in other node
               idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize());
               node0->GetLeafEntry(idx).Distance =
                     DMat[i][Node->GetRepresentativeIndex(0)];
               GR->SetFieldDistance(node0, idx, (ObjectType *) Node->GetObject(i));
            }//end if
         }//end if
      }//end if
   }//end for

   // Representatives
   rep0 = Node->BuyObject(Node->GetRepresentativeIndex(0));
   rep1 = Node->BuyObject(Node->GetRepresentativeIndex(1));

   return dCount;
}//end stDFMSTSplitter<ObjectType, EvaluatorType>::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFMSTSplitter<ObjectType, EvaluatorType>::JoinClusters(
      int cluster1, int cluster2){
   int i;

   for (i = 0; i < N; i++){
      if (ObjectCluster[i] == cluster2){
         ObjectCluster[i] = cluster1;
      }//end if
   }//end for
   Cluster[cluster1].Size += Cluster[cluster2].Size;
   Cluster[cluster2].State = DEATH_SENTENCE;
}//end stDFMSTSplitter<ObjectType, EvaluatorType>::JoinClusters


//==============================================================================
// Class stDFTree
//------------------------------------------------------------------------------

//This macro will be used to replace the declaration of
//       stDFTree<ObjectType, EvaluatorType>
#define tmpl_stDFTree stDFTree<ObjectType, EvaluatorType>
#define tmpl_stDFGlobalRep stDFGlobalRep<ObjectType, EvaluatorType>

template <class ObjectType, class EvaluatorType>
tmpl_stDFTree::stDFTree(stPageManager * pageman, stCount nFocus,
      stDistance pThreshold): stMetricTree<ObjectType, EvaluatorType>(pageman){

   // Initialize fields
   Header = NULL;
   HeaderPage = NULL;

   // Load header.
   LoadHeader();

   // Will I create or load the tree ?
   if (this->myPageManager->IsEmpty()){
      DefaultHeader();
   }//end if

   // GR Part.
   GR = new tGR(nFocus, pThreshold, myMetricEvaluator);   
}//end stDFTree::stDFTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::DefaultHeader(){

   // Clear header page.
   HeaderPage->Clear();

   // Default values
   Header->Magic[0] = 'D';
   Header->Magic[1] = 'F';
   Header->Magic[2] = '-';
   Header->Magic[3] = '3';
   Header->SplitMethod = smSPANNINGTREE;
   Header->ChooseMethod = cmMINDIST;
   Header->CorrectMethod = crmOFF;
   Header->Root = 0;
   Header->RootGlobalRep = 0;
   Header->MinOccupation = 0.25;
   Header->Height = 0;
   Header->ObjectCount = 0;
   Header->NodeCount = 0;

   // Notify modifications
   HeaderUpdate = true;
}//end stDFTree::DefaultHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::LoadHeader(){

   if (HeaderPage != NULL){
      myPageManager->ReleasePage(HeaderPage);
   }//end if

   // Load and set the header.
   HeaderPage = myPageManager->GetHeaderPage();
   if (HeaderPage->GetPageSize() < sizeof(stDFHeader)){
      throw page_size_error("The page size is too small.");
   }//end if

   Header = (stDFHeader *) HeaderPage->GetData();
   HeaderUpdate = false;
}//end stDFTree::LoadHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::FlushHeader(){

   if (HeaderPage != NULL){
      if (Header != NULL){
         WriteHeader();
      }//end if
      myPageManager->ReleasePage(HeaderPage);
   }//end if
}//end stDFTree::FlushHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool tmpl_stDFTree::Add(ObjectType * newObj){
   stSubtreeInfo promo1;
   stSubtreeInfo promo2;
   promo1.FieldDistance = new stDistance[GR->NumFocus];
   promo2.FieldDistance = new stDistance[GR->NumFocus];

   // Is there a root ?
   if (this->GetRoot() == 0){
      // No! We shall create the new node.
      stPage * auxPage  = this->NewPage();
      stDFLeafNode * leafNode = new stDFLeafNode(auxPage, true);
      this->SetRoot(auxPage->GetPageID());
      this->myPageManager->WritePage(auxPage);
      Header->Height++; // Update Height
      delete leafNode;
   }//end if

   // Let's continue our search for the grail!
   if (InsertRecursive(GetRoot(), newObj, NULL, promo1, promo2, GR) == PROMOTION){
      // Split occurred! We must create a new root because it is required.
      // The tree will aacquire a new root.
      AddNewRoot(promo1.Rep, promo1.Radius, promo1.RootID,
                 promo1.NObjects, promo1.FieldDistance,
                 promo2.Rep, promo2.Radius, promo2.RootID,
                 promo2.NObjects, promo2.FieldDistance);
      // Clean.
      delete promo1.Rep;
      delete promo2.Rep;
   }//end if

   // Update object count.
   UpdateObjectCounter(1);
   // Write Header!
   WriteHeader();

   //Free it
   delete[] promo1.FieldDistance;
   delete[] promo2.FieldDistance;

   return true;
}//end stDFTree::Add

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int tmpl_stDFTree::ChooseSubTree(
      stDFIndexNode * DFIndexNode, ObjectType * obj) {
   int idx;
   int j;
   int * cover;
   bool stop;
   int tmpNumberOfEntries;
   int numberOfEntries, minIndex = 0;
   stSize sizeOfObject; // used for save the size of the current object

   ObjectType * objectType = new ObjectType;
   stDistance distance;
   stDistance minDistance = MAXDOUBLE; // Largest magnitude double value
   // Get the total number of entries.
   numberOfEntries = DFIndexNode->GetNumberOfEntries();
   idx = 0;

   switch (this->GetChooseMethod()){
      case stDFTree::cmBIASED :
         // Find the first subtree that covers the new object.
         stop = (idx >= numberOfEntries);
         while (!stop){
            // Get the object from idx position from IndexNode
            objectType->Unserialize(DFIndexNode->GetObject(idx),
                                    DFIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // is this a subtree that covers the new object?
            if (distance < DFIndexNode->GetIndexEntry(idx).Radius) {
               minDistance = 0;     // the gain will be 0
               stop = true;         // stop the search.
               minIndex = idx;
            }else if (distance - DFIndexNode->GetIndexEntry(idx).Radius < minDistance) {
               minDistance = distance - DFIndexNode->GetIndexEntry(idx).Radius;
               minIndex = idx;
            }//end if
            idx++;
            // if one of the these condicions are true, stop this while.
            stop = stop || (idx >= numberOfEntries);
         }//end while
         break; //end stDFTree::cmBIASED

      case stDFTree::cmRANDOM :
         // allocate resources to cover.
         cover = new int[numberOfEntries];
         /* Find if there is some circle that contains obj */
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Get out the object from IndexNode.
            objectType->Unserialize(DFIndexNode->GetObject(idx),
                                    DFIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // Does this subtree is a qualified entry?
            if (distance < (DFIndexNode->GetIndexEntry(idx).Radius)){
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
            idx--;
            if ((cover[j] == 1) && (idx < 0)){
               minIndex = j;
            }//end if
            j++;
            if (j >= numberOfEntries){
               j = 0;
            }//end if
         }//end while
         // clean cover.
         delete[] cover;
         break; //end stDFTree::cmRANDOM

      case stDFTree::cmMINDIST :
         /* Find if there is some circle that contains obj */
         stop = (idx >= numberOfEntries);
         while (!stop){
            //get out the object from IndexNode
            objectType->Unserialize(DFIndexNode->GetObject(idx),
                                    DFIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // find the first subtree that cover the new object.
            if (distance < DFIndexNode->GetIndexEntry(idx).Radius) {
               minDistance = distance;     // the gain will be 0
               stop = true;                // stop the search.
               minIndex = idx;
            }else if (distance - DFIndexNode->GetIndexEntry(idx).Radius < minDistance) {
               minDistance = distance - DFIndexNode->GetIndexEntry(idx).Radius;
               minIndex = idx;
            }//end if
            idx++;
            // if one of the these condicions are true, stop this while.
            stop = stop || (idx >= numberOfEntries);
         }//end while
         // Try to find a better entry.
         while (idx < numberOfEntries) {
            // Get out the object from IndexNode.
            objectType->Unserialize(DFIndexNode->GetObject(idx),
                                    DFIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            if ((distance < DFIndexNode->GetIndexEntry(idx).Radius) &&
                (distance < minDistance)) {
               minDistance = distance;
               minIndex = idx;
            }//end if
            idx++;
         }//end while
         break; //end stDFTree::cmMINDIST

      case stDFTree::cmMINGDIST :
         // Find if there is some circle that contains obj
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Get out the object from IndexNode.
            objectType->Unserialize(DFIndexNode->GetObject(idx),
                                    DFIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            if (distance < minDistance) {
               minDistance = distance;
               minIndex = idx;
            }//end if
         }//end for
         break; //end stDFTree::cmMINGDIST

      case stDFTree::cmMINOCCUPANCY :
         /* Find if there is some circle that contains obj */
         tmpNumberOfEntries = MAXINT;
         // First try to find a subtree that covers the new object.
         stop = (idx >= numberOfEntries);
         while (!stop){
            //get out the object from IndexNode
            objectType->Unserialize(DFIndexNode->GetObject(idx),
                                    DFIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);
            // find the first subtree that covers the new object.
            if (distance < DFIndexNode->GetIndexEntry(idx).Radius) {
               minDistance = distance;     // the gain will be 0
               stop = true;                // stop the search.
               minIndex = idx;
               tmpNumberOfEntries = DFIndexNode->GetIndexEntry(idx).NEntries;
            }else if (distance - DFIndexNode->GetIndexEntry(idx).Radius < minDistance) {
               minDistance = distance - DFIndexNode->GetIndexEntry(idx).Radius;
               minIndex = idx;
            }//end if
            idx++;
            // if one of the these condicions are true, stop this while.
            stop = stop || (idx >= numberOfEntries);
         }//end while

         while (idx < numberOfEntries) {
            // Get out the object from IndexNode
            objectType->Unserialize(DFIndexNode->GetObject(idx),
                                    DFIndexNode->GetObjectSize(idx));
            // Calculate the distance.
            distance = this->myMetricEvaluator->GetDistance(objectType, obj);

            if ((distance < DFIndexNode->GetIndexEntry(idx).Radius) &&
                ((int) DFIndexNode->GetIndexEntry(idx).NEntries < tmpNumberOfEntries)) {
               tmpNumberOfEntries = DFIndexNode->GetIndexEntry(idx).NEntries;
               minIndex = idx;
            }//end if
            idx++;
         }//end for
         break; //end stDFTree::cmMINOCCUPANCY

   }//end switch

   delete objectType;

   return minIndex;
}//end stDFTree::ChooseSubTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::AddNewRoot(
      ObjectType * obj1, stDistance radius1,
      stPageID nodeID1, stCount nEntries1, stDistance * fieldDistance1,
      ObjectType * obj2, stDistance radius2,
      stPageID nodeID2, stCount nEntries2, stDistance * fieldDistance2){
   stPage * newPage;
   stDFIndexNode * newRoot;
   int idx;

   // Debug mode!
   #ifdef __stDEBUG__
      if ((obj1 == NULL) || (obj2 == NULL)){
         throw invalid_argument("Invalid object.");
      }//end if
   #endif //__stDEBUG__

   // Create a new node
   newPage = this->NewPage();
   newRoot = new stDFIndexNode(newPage, true);

   // Add obj1
   idx = newRoot->AddEntry(obj1->GetSerializedSize(), obj1->Serialize());
   newRoot->GetIndexEntry(idx).Distance = 0.0;
   newRoot->GetIndexEntry(idx).PageID = nodeID1;
   newRoot->GetIndexEntry(idx).Radius = radius1;
   newRoot->GetIndexEntry(idx).NEntries = nEntries1;
   memcpy(newRoot->GetIndexEntry(idx).FieldDistance,
          fieldDistance1, STFOCUS * sizeof(stDistance));

   // Add obj2
   idx = newRoot->AddEntry(obj2->GetSerializedSize(), obj2->Serialize());
   newRoot->GetIndexEntry(idx).Distance = 0.0;
   newRoot->GetIndexEntry(idx).PageID = nodeID2;
   newRoot->GetIndexEntry(idx).Radius = radius2;
   newRoot->GetIndexEntry(idx).NEntries = nEntries2;
   memcpy(newRoot->GetIndexEntry(idx).FieldDistance, fieldDistance2,
          STFOCUS * sizeof(stDistance));

   // Update tree
   Header->Height++;
   SetRoot(newRoot->GetPage()->GetPageID());
   myPageManager->WritePage(newPage);

   // Dispose page
   delete newRoot;
   myPageManager->ReleasePage(newPage);
}//end DFTree::AddNewRoot

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int tmpl_stDFTree::InsertRecursive(
      stPageID currNodeID, ObjectType * newObj, ObjectType * repObj,
      stSubtreeInfo & promo1, stSubtreeInfo & promo2, tGR * GR){
   stPage * currPage;      // Current page
   stPage * newPage;       // New page
   stDFNode * currNode;  // Current node
   stDFIndexNode * indexNode; // Current index node.
   stDFIndexNode * newIndexNode; // New index node for splits
   stDFLeafNode * leafNode; // Current leaf node.
   stDFLeafNode * newLeafNode; // New leaf node.
   int insertIdx;          // Insert index.
   int result;             // Returning value.
   stDistance dist;        // Temporary distance.
   int subtree;            // Subtree
   ObjectType * subRep;    // Subtree representative.
   stDistance * fieldDistance; // newObj GR Vector (SplitLeaf)

   // Read node...
   currPage = this->myPageManager->GetPage(currNodeID);
   currNode = stDFNode::CreateNode(currPage);

   // What shall I do ?
   if (currNode->GetNodeType() == stDFNode::INDEX){
      // Index Node cast.
      indexNode = (stDFIndexNode *)currNode;

      // Where do I add it ?
      subtree = ChooseSubTree(indexNode, newObj);

      // Lets get the information about this tree.
      subRep = new ObjectType();
      subRep->Unserialize(indexNode->GetObject(subtree),
                          indexNode->GetObjectSize(subtree));

      // Try to insert...
      switch (InsertRecursive(indexNode->GetIndexEntry(subtree).PageID,
            newObj, subRep, promo1, promo2, GR)){

         case NO_ACT: // Update Radius and count.

            indexNode->GetIndexEntry(subtree).NEntries++;
            indexNode->GetIndexEntry(subtree).Radius = promo1.Radius;

            // Returning status.
            promo1.NObjects = indexNode->GetTotalObjectCount();
            promo1.Radius = indexNode->GetMinimumRadius();
            result = NO_ACT;
            break; //end stDFTree::NO_ACT

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
               memcpy(indexNode->GetIndexEntry(insertIdx).FieldDistance,
                      promo1.FieldDistance, STFOCUS * sizeof(stDistance));

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
                           this->myMetricEvaluator->GetDistance(
                           repObj, promo1.Rep);
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
               newIndexNode = new stDFIndexNode(newPage, true);

               // Split!
               SplitIndex(indexNode, newIndexNode,
                     promo1.Rep, promo1.Radius, promo1.RootID, promo1.NObjects, promo1.FieldDistance,
                     NULL, 0, 0, 0, 0, repObj, promo1, promo2);

               // Write nodes
               myPageManager->WritePage(newPage);
               // Clean home.
               delete newIndexNode;
               myPageManager->ReleasePage(newPage);
               result = PROMOTION; //Report split.
            }//end if
            break; //end stDFTree::CHANGE_REP

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
                  memcpy(indexNode->GetIndexEntry(insertIdx).FieldDistance,
                         promo2.FieldDistance, STFOCUS * sizeof(stDistance));

                  // Update promo2 distance
                  if (repObj != NULL){
                     // Distance from representative is...
                     indexNode->GetIndexEntry(insertIdx).Distance =
                           this->myMetricEvaluator->GetDistance(
                           repObj, promo2.Rep);
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
                  newIndexNode = new stDFIndexNode(newPage, true);

                  // Split!
                  SplitIndex(indexNode, newIndexNode,
                        promo2.Rep, promo2.Radius, promo2.RootID, promo2.NObjects, promo2.FieldDistance,
                        NULL, 0, 0, 0, 0, repObj, promo1, promo2);

                  // Write nodes
                  myPageManager->WritePage(newPage);
                  // Clean home.
                  delete newIndexNode;
                  myPageManager->ReleasePage(newPage);
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
                  memcpy(indexNode->GetIndexEntry(insertIdx).FieldDistance,
                         promo1.FieldDistance, STFOCUS * sizeof(stDistance));

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
                              this->myMetricEvaluator->GetDistance(
                              repObj, promo1.Rep);
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
                     memcpy(indexNode->GetIndexEntry(insertIdx).FieldDistance,
                            promo2.FieldDistance, STFOCUS * sizeof(stDistance));

                     // The new distance is...
                     if (promo1.Rep != NULL){
                        // Rep. changed...
                        // Distance from representative is...
                        indexNode->GetIndexEntry(insertIdx).Distance =
                              this->myMetricEvaluator->GetDistance(
                              promo1.Rep, promo2.Rep);
                     }else{
                        // No change!
                        if (repObj != NULL){
                           // Distance from representative is...
                           indexNode->GetIndexEntry(insertIdx).Distance =
                                 this->myMetricEvaluator->GetDistance(
                                 repObj, promo2.Rep);
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
                     newIndexNode = new stDFIndexNode(newPage, true);

                     // Dispose promo1.rep it if exists because it will not be
                     // used again. It happens when result is CHANGE_REP.
                     if (promo1.Rep != NULL){
                        delete promo1.Rep;
                     }//end if

                     // Add promo2 and split!
                     SplitIndex(indexNode, newIndexNode,
                           promo2.Rep, promo2.Radius, promo2.RootID, promo2.NObjects, promo2.FieldDistance,
                           NULL, 0, 0, 0, 0, // Ignore this object
                           repObj, promo1, promo2);

                     // Write nodes
                     myPageManager->WritePage(newPage);
                     // Clean home.
                     delete newIndexNode;
                     myPageManager->ReleasePage(newPage);
                     result = PROMOTION; //Report split.
                  }//end if
               }else{

                  // Split it because both objects don't fit.
                  // New node.
                  newPage = this->NewPage();
                  newIndexNode = new stDFIndexNode(newPage, true);

                  // Split!
                  SplitIndex(indexNode, newIndexNode,
                        promo1.Rep, promo1.Radius, promo1.RootID, promo1.NObjects, promo1.FieldDistance,
                        promo2.Rep, promo2.Radius, promo2.RootID, promo2.NObjects, promo2.FieldDistance,
                        repObj, promo1, promo2);

                  // Write nodes
                  myPageManager->WritePage(newPage);
                  // Clean home.
                  delete newIndexNode;
                  myPageManager->ReleasePage(newPage);
                  result = PROMOTION; //Report split.
               }//end if
            }//end if
      };//end switch

      // Clear the mess.
      delete subRep;

   }else{
      // Leaf node cast.
      stDFLeafNode * leafNode = (stDFLeafNode *) currNode;

      // Try to insert...
      insertIdx = leafNode->AddEntry(newObj->GetSerializedSize(),
                                     newObj->Serialize());
      if (insertIdx >= 0){
         // Don't split!
         // Calculate distance and verify if it is a new radius!
         if (repObj == NULL){
            dist = 0;
         }else{
            dist = myMetricEvaluator->GetDistance(newObj, repObj);
         }//end if

         // Fill entry's fields
         leafNode->GetLeafEntry(insertIdx).Distance = dist;

         // If had already splited once, set the GR coordinante and check
         // if update the GR is needed
         if (!GR->FirstSplit()){
            GR->SetFieldDistance(leafNode, insertIdx, newObj);
            GR->CheckUpdate(myPageManager, this->GetRoot(),
                            leafNode->GetLeafEntry(insertIdx).FieldDistance);
         }//end if

         // Write node.
         myPageManager->WritePage(currPage);

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
         newLeafNode = new stDFLeafNode(newPage, true);

         // if already had splited once
         if (!GR->FirstSplit()){
            fieldDistance = new stDistance[GR->NumFocus];
            GR->BuildFieldDistance(newObj, fieldDistance);
         }//end if

         // Split!
         SplitLeaf(leafNode, newLeafNode, (ObjectType *)newObj->Clone(), fieldDistance,
               repObj, promo1, promo2);

         // Write node.
         myPageManager->WritePage(newPage);
         // Clean home.
         delete newLeafNode;
         myPageManager->ReleasePage(newPage);
         result = PROMOTION; //Report split.
      }//end if
   }//end if

   // Write node.
   myPageManager->WritePage(currPage);
   // Clean home
   delete currNode;
   myPageManager->ReleasePage(currPage);
   return result;
}//end stDFTree<ObjectType, EvaluatorType>::InsertRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::RandomPromote(tLogicNode * node) {

   stCount idx1, idx2;
   stCount numberOfEntries = node->GetNumberOfEntries();
   // generate a number between 0 to numberOfEntries-1 fo idx1
   idx1 = random(numberOfEntries - 1);
   // while idx2 == idx1, do the random for idx2
   while ((idx2 = random(numberOfEntries - 1)) == idx1)
      ;
   // Choose representatives
   node->SetRepresentative(idx1, idx2);
}//end stDFTree::randomPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::MinMaxPromote(tLogicNode * node) {

   stDistance iRadius, jRadius, min;
   stCount numberOfEntries, idx1, idx2;
   stPage * newPage1 = new stPage(myPageManager->GetMinimumPageSize());
   stPage * newPage2 = new stPage(myPageManager->GetMinimumPageSize());

   numberOfEntries = node->GetNumberOfEntries();
   min = MAXDOUBLE;   // Largest magnitude double value

   // Is it a Index node?
   if (node->GetNodeType() == stDFNode::INDEX) {
      stDFIndexNode * indexNode1 = new stDFIndexNode(newPage1,true);
      stDFIndexNode * indexNode2 = new stDFIndexNode(newPage2,true);

      for (stCount i = 0; i < numberOfEntries; i++) {
         for (stCount j = i + 1; j < numberOfEntries; j++) {
            node->SetRepresentative(i, j);
            indexNode1->RemoveAll();
            indexNode2->RemoveAll();
            node->TestDistribution(indexNode1, indexNode2, myMetricEvaluator);
            iRadius = indexNode1->GetMinimumRadius();
            jRadius = indexNode2->GetMinimumRadius();
            if (iRadius < jRadius)
               iRadius = jRadius;      // take the maximum
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
      stDFLeafNode * leafNode1 = new stDFLeafNode(newPage1,true);
      stDFLeafNode * leafNode2 = new stDFLeafNode(newPage2,true);

      for (stCount i = 0; i < numberOfEntries; i++) {
         for (stCount j = i + 1; j < numberOfEntries; j++) {
            node->SetRepresentative(i, j);
            leafNode1->RemoveAll();
            leafNode2->RemoveAll();
            node->TestDistribution(leafNode1, leafNode2, myMetricEvaluator);
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
   }//end if

   // Choose representatives
   node->SetRepresentative(idx1, idx2);

   delete newPage1;
   delete newPage2;
}//end stDFTree::minMaxPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::SplitLeaf(
      stDFLeafNode * oldNode, stDFLeafNode * newNode,
      ObjectType * newObj, stDistance * fieldDistance, ObjectType * prevRep,
      stSubtreeInfo & promo1, stSubtreeInfo & promo2) {
   tLogicNode * logicNode;
   tMSTSplitter * mstSplitter;
   ObjectType * lRep;
   ObjectType * rRep;
   int idx;
   stCount numberOfEntries = oldNode->GetNumberOfEntries();

   // Create the new tLogicNode
   logicNode = new tLogicNode(numberOfEntries + 1);
   logicNode->SetMinOccupation((stCount )(GetMinOccupation() * (numberOfEntries + 1)));
   logicNode->SetNodeType(stDFNode::LEAF);

   // update the maximum number of entries.
   this->SetMaxOccupation(numberOfEntries);

   // Add objects
   logicNode->AddLeafNode(oldNode);
   idx = logicNode->AddEntry(newObj);

   // if already had splited once
   if (!GR->FirstSplit()){
      logicNode->SetFieldDistance(idx, fieldDistance);
   }//end if

   // Split it.
   switch (GetSplitMethod()) {
      case stDFTree::smRANDOM:
         this->RandomPromote(logicNode);

         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, myMetricEvaluator, GR);
         delete logicNode;
         break; //end stDFTree::smRANDOM
      case stDFTree::smMINMAX:
         this->MinMaxPromote(logicNode);

         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, myMetricEvaluator, GR);
         delete logicNode;
         break;  //end stDFTree::smMINMAX
      case stDFTree::smSPANNINGTREE:
         // MST Split
         mstSplitter = new tMSTSplitter(logicNode);

         // Perform MST
         oldNode->RemoveAll();
         mstSplitter->Distribute(oldNode, lRep, newNode, rRep, myMetricEvaluator, GR);
         // Clean home
         delete mstSplitter;

         break; //end stDFTree::smSPANNINGTREE
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
      GR->BuildFieldDistance(lRep, promo1.FieldDistance);

      promo2.Rep = rRep;
      promo2.Radius = newNode->GetMinimumRadius();
      promo2.RootID = newNode->GetPageID();
      promo2.NObjects = newNode->GetTotalObjectCount();
      GR->BuildFieldDistance(rRep, promo2.FieldDistance);
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
         GR->BuildFieldDistance(rRep, promo2.FieldDistance);
      }else if (prevRep->IsEqual(rRep)){
         // rRep is the prevRep. Delete it.
         delete rRep;
         promo2.Rep = lRep;
         promo2.Radius = oldNode->GetMinimumRadius();
         promo2.RootID = oldNode->GetPageID();
         promo2.NObjects = oldNode->GetTotalObjectCount();
         GR->BuildFieldDistance(lRep, promo2.FieldDistance);

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
         GR->BuildFieldDistance(lRep, promo1.FieldDistance);

         promo2.Rep = rRep;
         promo2.Radius = newNode->GetMinimumRadius();
         promo2.RootID = newNode->GetPageID();
         promo2.NObjects = newNode->GetTotalObjectCount();
         GR->BuildFieldDistance(rRep, promo2.FieldDistance);
      }//end if
   }//end if
}//end stDFTree::SplitLeaf

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::SplitIndex(
      stDFIndexNode * oldNode, stDFIndexNode * newNode,
      ObjectType * newObj1, stDistance newRadius1,
      stPageID newNodeID1, stCount newNEntries1, stDistance * fieldDistance1,
      ObjectType * newObj2, stDistance newRadius2,
      stPageID newNodeID2, stCount newNEntries2, stDistance * fieldDistance2,
      ObjectType * prevRep,
      stSubtreeInfo & promo1, stSubtreeInfo & promo2){
   tLogicNode * logicNode;
   tMSTSplitter * mstSplitter;
   ObjectType * lRep;
   ObjectType * rRep;
   stCount numberOfEntries = oldNode->GetNumberOfEntries();

   // Create the new tLogicNode
   logicNode = new tLogicNode(numberOfEntries + 2);
   logicNode->SetMinOccupation((int )(GetMinOccupation() * (numberOfEntries + 2)));
   logicNode->SetNodeType(stDFNode::INDEX);
   // update the maximum number of entries.
   this->SetMaxOccupation(numberOfEntries);

   // Add objects
   logicNode->AddIndexNode(oldNode);

   // Add newObj1
   logicNode->AddEntry(newObj1);
   logicNode->SetEntry(logicNode->GetNumberOfEntries() - 1,
         newNodeID1, newNEntries1, newRadius1, fieldDistance1);

   // Will I add newObj2 ?
   if (newObj2 != NULL){
      logicNode->AddEntry(newObj2);
      logicNode->SetEntry(logicNode->GetNumberOfEntries() - 1,
            newNodeID2, newNEntries2, newRadius2, fieldDistance2);
   }//end if

   // Split it.
   switch (GetSplitMethod()) {
      case stDFTree::smRANDOM:
         this->RandomPromote(logicNode);

         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, myMetricEvaluator, GR);
         delete logicNode;
         break; //end stDFTree::smRANDOM
      case stDFTree::smMINMAX:
         this->MinMaxPromote(logicNode);

         // Redistribute
         oldNode->RemoveAll();
         logicNode->Distribute(oldNode, lRep, newNode, rRep, myMetricEvaluator, GR);
         delete logicNode;
         break;  //end stDFTree::smMINMAX
      case stDFTree::smSPANNINGTREE:
         // MST Split
         mstSplitter = new tMSTSplitter(logicNode);

         // Perform MST
         oldNode->RemoveAll();
         mstSplitter->Distribute(oldNode, lRep, newNode, rRep, myMetricEvaluator, GR);
         // Clean home
         delete mstSplitter;

         break; //end stDFTree::smSPANNINGTREE
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
      GR->BuildFieldDistance(lRep, promo1.FieldDistance);

      promo2.Rep = rRep;
      promo2.Radius = newNode->GetMinimumRadius();
      promo2.RootID = newNode->GetPageID();
      promo2.NObjects = newNode->GetTotalObjectCount();
      GR->BuildFieldDistance(rRep, promo2.FieldDistance);
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
         GR->BuildFieldDistance(rRep, promo2.FieldDistance);
      }else if (prevRep->IsEqual(rRep)){
         // rRep is the prevRep. Delete it.
         delete rRep;
         promo2.Rep = lRep;
         promo2.Radius = oldNode->GetMinimumRadius();
         promo2.RootID = oldNode->GetPageID();
         promo2.NObjects = oldNode->GetTotalObjectCount();
         GR->BuildFieldDistance(lRep, promo2.FieldDistance);

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
         GR->BuildFieldDistance(lRep, promo1.FieldDistance);

         promo2.Rep = rRep;
         promo2.Radius = newNode->GetMinimumRadius();
         promo2.RootID = newNode->GetPageID();
         promo2.NObjects = newNode->GetTotalObjectCount();
         GR->BuildFieldDistance(rRep, promo2.FieldDistance);
      }//end if
   }//end if
}//end stDFTree::SplitIndex

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::UpdateDistances(stDFIndexNode *node,
            ObjectType * repObj, int repObjIdx){
   int i;
   ObjectType * tempObj = new ObjectType();

   for (i = 0; i < node->GetNumberOfEntries(); i++){
      if (i != repObjIdx){
         tempObj->Unserialize(node->GetObject(i), node->GetObjectSize(i));
         node->GetIndexEntry(i).Distance =
            myMetricEvaluator->GetDistance(repObj, tempObj);
      }else{
         //it's the representative object
         node->GetIndexEntry(i).Distance = 0.0;
      }//end if
   }//end for

   //clean the house before exit.
   delete tempObj;
}//end stDFTree::UpdateDistances

//==============================================================================
// Begin of Queries
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * tmpl_stDFTree::RangeQuery(
            ObjectType * sample, stDistance range){
   tResult * result = new tResult();  // Create result
   stPage * currPage;
   stDFNode * currNode;
   ObjectType tmpObj;
   stCount idx, numberOfEntries;
   stDistance distance;
   stDistance * DistQueryFocus;

   result->SetQueryInfo(sample->Clone(), tResult::RANGEQUERY, -1, range, false);

   // Calculate the distances from the query object to the GR.
   DistQueryFocus = new stDistance[STFOCUS];
   GR->BuildFieldDistance(sample, DistQueryFocus);

   // Evaluate the root node.
   if (this->GetRoot() != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = stDFNode::CreateNode(currPage);

      // Is it a Index node?
      if (currNode->GetNodeType() == stDFNode::INDEX){
         // Get Index node
         stDFIndexNode * indexNode = (stDFIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // Rebuild the object
            tmpObj.Unserialize(indexNode->GetObject(idx),
                               indexNode->GetObjectSize(idx));

            // Evaluate distance
            distance = myMetricEvaluator->GetDistance(&tmpObj, sample);

            // test if this subtree qualifies.
            if (distance <= range + indexNode->GetIndexEntry(idx).Radius){
               // if this subtree can be pruned by Global Representatives
               if (!GR->PruneByGlobalRep(indexNode->GetIndexEntry(idx).FieldDistance,
                        indexNode->GetIndexEntry(idx).Radius, DistQueryFocus, range)){
                  // No! Analyze this subtree.
                  this->RangeQuery(indexNode->GetIndexEntry(idx).PageID, result,
                                   sample, range, distance, DistQueryFocus);
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stDFLeafNode * leafNode = (stDFLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // if this subtree can be pruned by Global Representatives
            if (!GR->PruneByGlobalRep(leafNode->GetLeafEntry(idx).FieldDistance,
                                      0, DistQueryFocus, range)) {
               // Rebuild the object
               tmpObj.Unserialize(leafNode->GetObject(idx),
                                  leafNode->GetObjectSize(idx));
               // Evaluate distance
               distance = myMetricEvaluator->GetDistance(&tmpObj, sample);
               // is it a object that qualified?
               if (distance <= range){
                  // Yes! Put it in the result set.
                  result->AddPair(tmpObj.Clone(), distance);
               }//end if
            }//end if
         }//end for
      }//end if

      // Free it all
      delete currNode;
      myPageManager->ReleasePage(currPage);
   }//end if

   delete[] DistQueryFocus;
   return result;
}//end stDFTree::RangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::RangeQuery(stPageID pageID, tResult * result,
                               ObjectType * sample, stDistance range,
                               stDistance distanceRepres,
                               stDistance * DistQueryFocus){
                               
   stPage * currPage;
   stDFNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx;
   stCount numberOfEntries;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stDFNode::CreateNode(currPage);
      // Is it a index node?
      if (currNode->GetNodeType() == stDFNode::INDEX) {
         // Get Index node
         stDFIndexNode * indexNode = (stDFIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // For each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // use of the triangle inequality to cut a subtree
            if (fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      range + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = myMetricEvaluator->GetDistance(&tmpObj, sample);
               // is this a qualified subtree?
               if (distance <= range + indexNode->GetIndexEntry(idx).Radius){
                  // if this subtree can be pruned by Global Representatives
                  if (!GR->PruneByGlobalRep(indexNode->GetIndexEntry(idx).FieldDistance,
                           indexNode->GetIndexEntry(idx).Radius, DistQueryFocus, range)) {
                     // No! Analyze it!
                     this->RangeQuery(indexNode->GetIndexEntry(idx).PageID, result,
                                      sample, range, distance, DistQueryFocus);
                  }//end if
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stDFLeafNode * leafNode = (stDFLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // use of the triangle inequality.
            if (fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      range){
                // if this subtree can be pruned by Global Representatives
                if (!GR->PruneByGlobalRep(leafNode->GetLeafEntry(idx).FieldDistance,
                                          0, DistQueryFocus, range)) {
                   // Rebuild the object
                   tmpObj.Unserialize(leafNode->GetObject(idx),
                                      leafNode->GetObjectSize(idx));
                   // No, it is not a representative. Evaluate distance
                   distance = myMetricEvaluator->GetDistance(&tmpObj, sample);
                   // Is this a qualified object?
                   if (distance <= range){
                      // Yes! Put it in the result set.
                      result->AddPair(tmpObj.Clone(), distance);
                   }//end if
               }//end if
            }//end if
         }//end for
      }//end if

      // Free it all
      delete currNode;
      myPageManager->ReleasePage(currPage);
   }//end if
}//end stDFTree::RangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDFTree<ObjectType, EvaluatorType>::NearestQuery(
      ObjectType * sample, stCount k, bool tie){
      
   tResult * result = new tResult();  // Create result

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);

   // Let's search
   if (this->GetRoot() != 0){
      this->NearestQuery(result, sample, MAXDOUBLE, k);
   }//end if

   return result;
}//end stDFTree::NearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFTree<ObjectType, EvaluatorType>::NearestQuery(tResult * result,
         ObjectType * sample, stDistance rangeK, stCount k){
   tDynamicPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stDFNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   bool stop;
   stDistance * DistQueryFocus;

   // Calculate the distances from the query object to the GR.
   DistQueryFocus = new stDistance[STFOCUS];
   GR->BuildFieldDistance(sample, DistQueryFocus);

   // Root node
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;

   // Create the Global Priority Queue
   queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stDFNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stDFNode::INDEX) {
         // Get Index node
         stDFIndexNode * indexNode = (stDFIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if (fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                rangeK + indexNode->GetIndexEntry(idx).Radius){
               // Rebuild the object
               tmpObj.Unserialize(indexNode->GetObject(idx),
                                  indexNode->GetObjectSize(idx));
               // Evaluate distance
               distance = myMetricEvaluator->GetDistance(&tmpObj, sample);

               if (distance <= rangeK + indexNode->GetIndexEntry(idx).Radius){
                  // if this subtree can be pruned by Global Representatives
                  if (!GR->PruneByGlobalRep(indexNode->GetIndexEntry(idx).FieldDistance,
                           indexNode->GetIndexEntry(idx).Radius, DistQueryFocus, rangeK)) {
                     // No! I'm qualified! Put it in the queue.
                     pqTmpValue.PageID = indexNode->GetIndexEntry(idx).PageID;
                     pqTmpValue.Radius = indexNode->GetIndexEntry(idx).Radius;
                     queue->Add(distance, pqTmpValue);
                  }//end if
               }//end if
            }//end if
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stDFLeafNode * leafNode = (stDFLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this object with the triangle inequality.
            if (fabs(distanceRepres - leafNode->GetLeafEntry(idx).Distance) <=
                      rangeK){
                // if this subtree can be pruned by Global Representatives
                if (!GR->PruneByGlobalRep(leafNode->GetLeafEntry(idx).FieldDistance,
                                          0, DistQueryFocus, rangeK)) {
                   // Rebuild the object
                   tmpObj.Unserialize(leafNode->GetObject(idx),
                                      leafNode->GetObjectSize(idx));
                   // When this entry is a representative, it does not need to evaluate
                   // a distance, because distanceRepres is iqual to distance.
                   // Evaluate distance
                   distance = myMetricEvaluator->GetDistance(&tmpObj, sample);
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
            }//end if
         }//end for
      }//end if

      // Free it all
      delete currNode;
      myPageManager->ReleasePage(currPage);

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
   }//end while

   // Release the Global Priority Queue
   delete[] DistQueryFocus;
   delete queue;
}//end stDFTree::NearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDFTree<ObjectType, EvaluatorType>::PointQuery(
      ObjectType * sample){
   tResult * result = new tResult();  // Create result

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::POINTQUERY);
   // Let's search
   if (this->GetRoot() != 0){
      this->PointQuery(result, sample);
   }//end if

   return result;
}//end stDFTree::PointQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFTree<ObjectType, EvaluatorType>::PointQuery(
         tResult * result, ObjectType * sample){
   tDynamicPriorityQueue * queue;
   stCount idx;
   stPage * currPage;
   stDFNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTMPValue;
   stCount numberOfEntries;
   bool stop;
   bool find = false;
   stDistance * DistQueryFocus;

   // Calculate the distances from the query object to the GR.
   DistQueryFocus = new stDistance[STFOCUS];
   GR->BuildFieldDistance(sample, DistQueryFocus);

   // Root node
   pqCurrValue.PageID = this->GetRoot();
   pqCurrValue.Radius = 0;

   // Create the Global Priority Queue
   queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   // Let's search
   while ((pqCurrValue.PageID != 0) && (!find)){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = stDFNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stDFNode::INDEX) {
         // Get Index node
         stDFIndexNode * indexNode = (stDFIndexNode *)currNode;
         numberOfEntries = indexNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // try to cut this subtree with the triangle inequality.
            if (fabs(distanceRepres - indexNode->GetIndexEntry(idx).Distance) <=
                      indexNode->GetIndexEntry(idx).Radius){
               // if this subtree can be pruned by Global Representatives
               if (!GR->PruneByGlobalRep(indexNode->GetIndexEntry(idx).FieldDistance,
                        indexNode->GetIndexEntry(idx).Radius, DistQueryFocus, 0)) {
                  // Rebuild the object
                  tmpObj.Unserialize(indexNode->GetObject(idx),
                                     indexNode->GetObjectSize(idx));
                  // Evaluate distance
                  distance = myMetricEvaluator->GetDistance(&tmpObj, sample);
                  if (distance <= indexNode->GetIndexEntry(idx).Radius){
                     // No! I'm qualified! Put it in the queue.
                     pqTMPValue.PageID =  indexNode->GetIndexEntry(idx).PageID;
                     pqTMPValue.Radius =  indexNode->GetIndexEntry(idx).Radius;
                     queue->Add(distance, pqTMPValue);
                  }//end if
               }//end if
            }//end if
         }//end for
      }else {
         // No, it is a leaf node. Get it.
         stDFLeafNode * leafNode = (stDFLeafNode *)currNode;
         numberOfEntries = leafNode->GetNumberOfEntries();

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            // use of the triangle inequality
            if (distanceRepres == leafNode->GetLeafEntry(idx).Distance){
                // if this subtree can be pruned by Global Representatives
                if (!GR->PruneByGlobalRep(leafNode->GetLeafEntry(idx).FieldDistance,
                                          0, DistQueryFocus, 0)) {
                  // Rebuild the object
                  tmpObj.Unserialize(leafNode->GetObject(idx),
                                     leafNode->GetObjectSize(idx));
                  // When this entry is a representative, it does not need to evaluate
                  // a distance, because distanceRepres is iqual to distance.
                  // Evaluate distance.
                  distance = myMetricEvaluator->GetDistance(&tmpObj, sample);
                  //test if the object qualify
                  if (distance == 0){
                     // Add the object.
                     result->AddPair(tmpObj.Clone(), distance);
                     // Stop the query because the object was found!
                     find = true;
                  }//end if
                }//end if
            }//end if
         }//end for
      }//end if

      // Free it all
      delete currNode;
      myPageManager->ReleasePage(currPage);

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
   delete[] DistQueryFocus;
}//end stDFTree::PointQuery

//==============================================================================
// End of Queries
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stTreeInfoResult * tmpl_stDFTree::GetTreeInfo(){
   stTreeInformation * info;

   // No cache of information. I think a cahe would be a good idea.
   info = new stTreeInformation(GetHeight(), GetNumberOfObjects());

   // Let's get the information!
   GetTreeInfoRecursive(this->GetRoot(), 0, info);

   // Optimal tree
   if (info->GetMeanObjectSize() != 0){
      info->CalculateOptimalTreeInfo(int(myPageManager->GetMinimumPageSize() /
            info->GetMeanObjectSize()));
   }//end if

   return info;
}//end stDFTree::GetTreeInfo

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::GetTreeInfoRecursive(stPageID pageID, int level,
      stTreeInformation * info){
   stPage * currPage;
   stDFNode * currNode;
   stCount i;
   stCount lv;
   ObjectType tmp;

   // Let's search
   if (pageID != 0){
      // Update node count
      info->UpdateNodeCount(level);

      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stDFNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stDFNode::INDEX) {
         // Get Index node
         stDFIndexNode * indexNode = (stDFIndexNode *)currNode;

         // Object count
         info->UpdateObjectCount(level, indexNode->GetNumberOfEntries());

         // Scan all entries
         for (i = 0; i < (stCount) indexNode->GetNumberOfEntries(); i++){
            GetTreeInfoRecursive(indexNode->GetIndexEntry(i).PageID, level + 1,
                  info);
         }//end for
      }else{
         // No, it is a leaf node. Get it.
         stDFLeafNode * leafNode = (stDFLeafNode *)currNode;

         // Object count
         info->UpdateObjectCount(level, leafNode->GetNumberOfEntries());

         // Update object count
         for (i = 0; i <  (stCount) leafNode->GetNumberOfEntries(); i++){
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
      myPageManager->ReleasePage(currPage);
   }//end if
}//end stDFTree::GetTreeInfoRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::ObjectIntersectionsRecursive(stPageID pageID,
      ObjectType * obj, int level, stTreeInformation * info){
   stPage * currPage;
   stDFNode * currNode;
   stCount i;
   ObjectType tmp;
   stDistance d;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = stDFNode::CreateNode(currPage);
      // Is it a Index node?
      if (currNode->GetNodeType() == stDFNode::INDEX) {
         // Get Index node
         stDFIndexNode * indexNode = (stDFIndexNode *)currNode;

         // Scan all entries
         for (i = 0; i <  (stCount)  indexNode->GetNumberOfEntries(); i++){
            tmp.Unserialize(indexNode->GetObject(i),
                indexNode->GetObjectSize(i));
            d = myMetricEvaluator->GetDistance(&tmp, obj);
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
      myPageManager->ReleasePage(currPage);
   }//end if
}//end stDFTree::ObjectIntersectionsRecursive


//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFTree::ForceUpdateAllFieldDistance(){

   GR->UpdateFieldDistance(myPageManager, this->GetRoot());

}//end stDFTree::ForceUpdateAllFieldDistance

//==============================================================================
// Class stDFGlobalRep
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDFGlobalRep<ObjectType, EvaluatorType>::stDFGlobalRep(stCount nFocus,
      stDistance pThreshold, EvaluatorType * metricEvaluator){
   // Used to process FindGlobalRep only once. After,
   // the GR are calculated one by one
   bFirstSplit = true;
   // Number of Focus
   NumFocus = nFocus;
   // Used to know when update the GR
   Threshold = pThreshold;
   // To calculate distances
   myMetricEvaluator = metricEvaluator;
}//end stDFGlobalRep::stDFGlobalRep

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stDFGlobalRep::FindGlobalRep(tLogicNode * logicNode,
      tMSTSplitter * MSTSplitter, stCount center1, stCount center2,
      stCount numObject){
   stDistance maxDist;
   stDistance farther, diameter;
   stDistance error, aux;
   int k, i, j, jaux, iaux;
   int * ValidGlobalRep;
   int * iGlobalRep;
   stDistance auxDistance;

   this->SetFirstSplit();

   // States who are NOT elegible to be Focus
   ValidGlobalRep = new int[numObject];

   if (this->NumFocus > numObject - 2) {
      this->NumFocus = numObject - 2;
   }//end if

   // The new focus index
   iGlobalRep = new int[STFOCUS];
   if (this->NumFocus == 1) {
      farther = 0.0;
      for (i = 0; i < numObject; i++){
         if ((i != center1) && (i != center2)){
            if (MSTSplitter->GetMatrixDistance(i, center1) +
                MSTSplitter->GetMatrixDistance(i, center2) > farther) {
               farther = MSTSplitter->GetMatrixDistance(i, center1) +
                         MSTSplitter->GetMatrixDistance(i, center2);
               // object i is an Focus
               iGlobalRep[0] = i;
            }//end if
         }//end if
      }//end for

      GlobalRep[0].Object = logicNode->GetObject(iGlobalRep[0])->Clone();

   }else if (this->NumFocus == 2){
      // Find diameter of datasets
      diameter = 0.0;
      for (i = 0; i < numObject - 1; i++) {
         if ((i != center1) && (i != center2)){
            for (j = i + 1; j < numObject; j++) {
               if ((j != center1) && (j != center2)){
                  if (MSTSplitter->GetMatrixDistance(i,j) > diameter){
                     diameter = MSTSplitter->GetMatrixDistance(i,j);
                     iGlobalRep[0] = i;
                     iGlobalRep[1] = j;
                  }//end if
               }//end if
            }//end for
         }//end if
      }//end for

      GlobalRep[0].Object = logicNode->GetObject(iGlobalRep[0])->Clone();
      GlobalRep[1].Object = logicNode->GetObject(iGlobalRep[1])->Clone();
   }else if (this->NumFocus > 2){

      // Find diameter of datasets
      diameter = 0.0;
      for (i = 0; i < numObject - 1; i++) {
         for (j = i + 1; j < numObject; j++) {
            if (MSTSplitter->GetMatrixDistance(i,j) > diameter){
               diameter = MSTSplitter->GetMatrixDistance(i,j);
            }//end if
         }//end for
      }//end for
      for (i = 0; i < numObject; i++){
         ValidGlobalRep[i] = 0;
      }//end for
      // center objs cannot be Focus
      ValidGlobalRep[center1] = numObject + 1;
      ValidGlobalRep[center2] = numObject + 1;

      error = MAXDOUBLE;
      maxDist = -1;
      for (i = 0; i < numObject - 1; i++) {
         if (!ValidGlobalRep[i]){
            // block i to be choosen again
            ValidGlobalRep[i] = numObject + 1;
            for (j = i + 1; j < numObject; j++) {
               if (!ValidGlobalRep[j]){
                  if (MSTSplitter->GetMatrixDistance(i,j) > maxDist) {
                     maxDist = MSTSplitter->GetMatrixDistance(i, j);
                     jaux = j;
                     iaux = i;
                  }//end if
               }//end if
            }//end for
            // free i to be choosen again
            ValidGlobalRep[i] = 0;
         }//end if
      }//end for

      iGlobalRep[0] = iaux;
      iGlobalRep[1] = jaux;
      ValidGlobalRep[iaux] = numObject + 1;
      ValidGlobalRep[jaux] = numObject + 1;

      // Try to find the better equilateral hiper-triangle with side = Dist(i, j)
      for (k = 0; k < numObject; k++) {
         //third point of the triangle (together with i and j)
         if (!ValidGlobalRep[k]){
            aux = (fabs(maxDist - MSTSplitter->GetMatrixDistance(iaux, k)) +
                   fabs(maxDist - MSTSplitter->GetMatrixDistance(jaux, k)));
            // try to find the triangle with minimal error (to be equilateral)
            if (aux < error){
               error = aux;
               iGlobalRep[2] = k;
            }//end if
         }//end if
      }//end for

      GlobalRep[0].Object = logicNode->GetObject(iGlobalRep[0])->Clone();
      GlobalRep[1].Object = logicNode->GetObject(iGlobalRep[1])->Clone();
      GlobalRep[2].Object = logicNode->GetObject(iGlobalRep[2])->Clone();
      this->NumFocus = 3;
   }//end if

   for (k = 0; k < this->NumFocus; k++){
      GlobalRep[k].MaxDistance = 0;
   }//end for

   // Set the GR Distances to all objects
   for (i = 0; i < numObject; i++) {
      for (j = 0; j < this->NumFocus; j++) {
         auxDistance = MSTSplitter->GetMatrixDistance(i, iGlobalRep[j]);
         logicNode->SetFocusDistance(i, j, auxDistance );
         // Set the Max Distance from each global representative to calculate the circumscribed objects
         for (k = 0; k < this->NumFocus; k++){
            if ((i == iGlobalRep[k]) && (GlobalRep[k].MaxDistance < auxDistance)){
               GlobalRep[k].MaxDistance = auxDistance;
            }//end if
         }//end for
      }//end for
   }//end for

   // Used do know when update the GR.
   Uncircumbscribed = 0;

   // Free it all
   delete[] ValidGlobalRep;
   delete[] iGlobalRep;
}//end stDFGlobalRep::FindGlobalRep

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDFGlobalRep<ObjectType, EvaluatorType>::PruneByGlobalRep(
      stDistance * objGRCoord, stDistance radius,
      stDistance * distQueryFocus, stDistance queryRadius){

   // Return true if these parameters allows to prune the
   // corresponding comparison.
   for (int i = 0; i < this->NumFocus; i++){
      if ((objGRCoord[i] + radius < distQueryFocus[i] - queryRadius) ||
          (objGRCoord[i] - radius > distQueryFocus[i] + queryRadius)){
         return true;
      }//end if
   }//end for

   return false;
}//end stDFGlobalRep::PruneByGlobalRep

//---------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFGlobalRep<ObjectType, EvaluatorType>::SetFieldDistance(
      stDFLeafNode * node, stCount idx, ObjectType * object) {
   for (int i = 0; i < STFOCUS; i++){      node->GetLeafEntry(idx).FieldDistance[i] =
         myMetricEvaluator->GetDistance(GlobalRep[i].Object, object);
   }//end for
}//end stDFGlobalRep::BuildFieldDistance

//---------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFGlobalRep<ObjectType, EvaluatorType>::SetFieldDistance(
      stDFIndexNode * node, stCount idx, ObjectType * object) {
   for (int i = 0; i < STFOCUS; i++){
      node->GetIndexEntry(idx).FieldDistance[i] =
         myMetricEvaluator->GetDistance(GlobalRep[i].Object, object);
   }//end for
}//end stDFGlobalRep::SetFieldDistance

//---------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFGlobalRep<ObjectType, EvaluatorType>::BuildFieldDistance(
      ObjectType * object, stDistance * fieldDistance) {
   for (int i = 0; i < STFOCUS; i++){      fieldDistance[i] =
         myMetricEvaluator->GetDistance(GlobalRep[i].Object, object);
   }//end for
}//end stDFGlobalRep::BuildFieldDistance

//---------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFGlobalRep<ObjectType, EvaluatorType>::CheckUpdate(
      stPageManager * pageman, stPageID rootPageID,
      stDistance * fieldDistance){
      
   int i;   stDistance aux;   vector <ObjectType *> CandidateSet;
   // Calculate if object is circumbscribed
   aux = 1.0;
   for (i = 0; i < STFOCUS; i++){
      aux = aux * fieldDistance[i] / GlobalRep[i].MaxDistance;
   }//end for

   aux = powl(aux, 1.0/3.0);
   Uncircumbscribed = Uncircumbscribed + aux;

   // if needed, start update the GR.
   if (Uncircumbscribed > Threshold){
      CandidateSet.clear();
      // Get the candidates to Focus
      GetCandidates(pageman, rootPageID, CandidateSet);
      // Add All old focus to Candidate Set
      for (i = 0; i < STFOCUS; i++)
         CandidateSet.insert(CandidateSet.end(),
                             (GlobalRep[i].Object)->Clone());
      // Locate in CandidateSet the new GR
      UpdateGR(CandidateSet);
      // Update the All GR Distance
      UpdateFieldDistance(pageman, rootPageID);
      // For every entry in CandidateSet.
      for (i = 0; i < (int) CandidateSet.size(); i++){
         delete CandidateSet[i];
      }//end for
   }//end if
}//end stDFGlobalRep::CheckUpdate

//---------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFGlobalRep<ObjectType, EvaluatorType>::GetCandidates(
      stPageManager * pageman,  stPageID  pageID,
      vector <ObjectType *> & candidateSet) {
   stPage * currPage;
   stDFNode *currNode;
   stCount numberOfEntries;
   stDFLeafNode * leafNode;
   stDFIndexNode * indexNode;
   bool circumscribed;
   ObjectType * tmpObj;
   int i;

   tmpObj = new ObjectType();
   currPage = pageman->GetPage(pageID);
   currNode = stDFNode::CreateNode(currPage);

   numberOfEntries = currNode->GetNumberOfEntries();

   // It is a leaf node. Get it.
   if (currNode->GetNodeType() == stDFNode::LEAF){
      leafNode = (stDFLeafNode *)currNode;

      // For each entry...
      for (int idx = 0; idx < (int)numberOfEntries; idx++) {
         //Verify if is circumscribed object
         circumscribed = true;
         for (i = 0; i < STFOCUS; i++) {
            if (circumscribed &&
               (leafNode->GetLeafEntry(idx).FieldDistance[i] > GlobalRep[i].MaxDistance))
               circumscribed = false;
         }//end for

         //if uncircumscribed object add to Candidate Set
         if (!circumscribed) {
            tmpObj->Unserialize(leafNode->GetObject(idx),
                                leafNode->GetObjectSize(idx));
            candidateSet.insert(candidateSet.end(), tmpObj->Clone());
         }//end if

      }//end for
   }else{
      indexNode = (stDFIndexNode *)currNode;

      // Call recursion for all indexnode objects
      for (int idx = 0; idx < (int) numberOfEntries; idx++){
         GetCandidates(pageman,
                       indexNode->GetIndexEntry(idx).PageID,
                       candidateSet);
      }//end for
   }//end if

   // Free it all
   delete currNode;
   delete tmpObj;
   pageman->ReleasePage(currPage);
}//end stDFGlobalRep::GetCandidates

//---------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFGlobalRep<ObjectType, EvaluatorType>::UpdateGR(
      vector <ObjectType *> candidateSet) {
   int k, j, i, jaux, iaux, numObject;
   tDistanceMatrix DMat;
   stDistance maxDist;
   stDistance farther, diameter;
   stDistance error, aux;
   int * ValidGlobalRep;
   int * iGlobalRep;
   stDistance auxDistance;

   numObject = candidateSet.size();

   // Matrix
   DMat.SetSize(numObject, numObject);

   // Build the Matrix Distance for every entry in CandidateSet.
   for (i = 0; i < numObject; i++){
      DMat[i][i] = 0;
      for (j = 0; j < i; j++){
         DMat[i][j] = myMetricEvaluator->GetDistance(candidateSet[i],
                                                     candidateSet[j]);
         DMat[j][i] = DMat[i][j];
      }//end for
   }//end for

   // States who are NOT elegible to be Focus
   ValidGlobalRep = new int[numObject];

   // The new Focus index
   iGlobalRep = new int[STFOCUS];

   // Fins de Max distance
   for (i = 0; i < numObject; i++){
      ValidGlobalRep[i] = 0;
   }//end for
   error = MAXDOUBLE;
   maxDist = -1;
   for (i = 0; i < numObject - 1; i++) {
      if (!ValidGlobalRep[i]){;
         // block i to be choosen again
         ValidGlobalRep[i] = numObject + 1;
         for (j = i + 1; j < numObject; j++) {
            if (!ValidGlobalRep[j]){
               if (DMat[i][j] > maxDist) {
                  maxDist = DMat[i][j];
                  jaux = j;
                  iaux = i;
               }//end if
            }//end if
         }//end for
         // free i to be choosen again
         ValidGlobalRep[i] = 0;
      }//end if
   }//end for

   iGlobalRep[0] = iaux;
   iGlobalRep[1] = jaux;
   ValidGlobalRep[iaux] = numObject + 1;
   ValidGlobalRep[jaux] = numObject + 1;

   // Try to find the better equilateral hiper-triangle with side = Dist(i, j)
   for (k = 0; k < numObject; k++) {
      //third point of the triangle (together with i and j)
      if (!ValidGlobalRep[k]) {
         aux = (fabs(maxDist - DMat[iaux][k]) +
                fabs(maxDist - DMat[jaux][k]));
         // try to find the triangle with minimal error (to be equilateral)
         if (aux < error) {
            error = aux;
            iGlobalRep[2] = k;
         }//end if
      }//end if
   }//end for

   for (k = 0; k < this->NumFocus; k++){
      GlobalRep[k].MaxDistance = 0;
      if (GlobalRep[k].Object){
         delete GlobalRep[k].Object;
      }//end if
   }//end for

   GlobalRep[0].Object = candidateSet[iGlobalRep[0]]->Clone();
   GlobalRep[1].Object = candidateSet[iGlobalRep[1]]->Clone();
   GlobalRep[2].Object = candidateSet[iGlobalRep[2]]->Clone();

   // Set the Max Distance from each global representative to
   // calculate the circumscribed objects
   for (i = 0; i < this->NumFocus; i++){
      for (j = 0; j < this->NumFocus; j++) {
         auxDistance = DMat[iGlobalRep[i]][iGlobalRep[j]];
         if (GlobalRep[i].MaxDistance < auxDistance)
               GlobalRep[i].MaxDistance = auxDistance;
      }//end for
   }//end for

   // Used do know when update the GR.
   Uncircumbscribed = 0;

   //Free it all
   delete[] ValidGlobalRep;
   delete[] iGlobalRep;
}//end stDFGlobalRep::UpdateGR

//---------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDFGlobalRep<ObjectType, EvaluatorType>::UpdateFieldDistance(
      stPageManager * pageman, stPageID pageID) {
   stPage * currPage;
   stDFNode *currNode;
   ObjectType * tmpObj;
   stCount numberOfEntries;
   stDFLeafNode * leafNode;
   stDFIndexNode * indexNode;

   currPage = pageman->GetPage(pageID);
   currNode = stDFNode::CreateNode(currPage);

   tmpObj = new ObjectType();
   numberOfEntries = currNode->GetNumberOfEntries();

   // It is a leaf node. Get it.
   if (currNode->GetNodeType() == stDFNode::LEAF){
      // For each entry...
      leafNode = (stDFLeafNode *)currNode;
      for (int idx = 0; idx < (int)numberOfEntries; idx++) {
         tmpObj->Unserialize(leafNode->GetObject(idx),
                          leafNode->GetObjectSize(idx));

         SetFieldDistance(leafNode, idx, tmpObj);
      }//end for
   }else{
      indexNode = (stDFIndexNode *)currNode;

      // Call recursion for all indexnode objects
      for (int idx = 0; idx < (int) numberOfEntries; idx++) {
         // For each entry...
         tmpObj->Unserialize(indexNode->GetObject(idx),
                          indexNode->GetObjectSize(idx));
         SetFieldDistance(indexNode, idx, tmpObj);
         // Call recursion
         UpdateFieldDistance(pageman, indexNode->GetIndexEntry(idx).PageID);
      }//end for
   }//end if

   // Free it all
   delete tmpObj;
   delete currNode;
   pageman->ReleasePage(currPage);
}//end stDFGlobalRep::UpdateFieldDistance

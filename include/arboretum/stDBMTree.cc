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
* This file is the implementation of stDBMTree methods and auxiliary classes
* (stDBMLogicNode, stDBMMSTSplitter, stDBMNSplitter and stDBMTree).
*
* @version 1.0
* $Revision: 1.28 $
* $Date: 2005/03/06 18:14:31 $
*
* $Author: marcos $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2003 GBDI-ICMC-USP

//==============================================================================
// Class stDBMLogicNode
//==============================================================================

template <class ObjectType, class EvaluatorType>
stDBMLogicNode<ObjectType, EvaluatorType>::stDBMLogicNode(stCount maxOccupation){
   // Allocate resources
   MaxEntries = maxOccupation;
   Entries = new stDBMLogicEntry[MaxEntries];
   // Init Rep
   RepIndex[0] = 0;
   RepIndex[1] = 0;
   // Initialize
   Count = 0;

   // Minimum occupation. 25% is the default of Slim-tree
   MinOccupation = (stCount )0.25 * maxOccupation;
   // At least the nodes must store 2 objects.
   if ((MinOccupation > (0.5 * maxOccupation)) || (MinOccupation == 0)){
      MinOccupation = 2;
   }//end if

   // Matrix
   DMat.SetSize(MaxEntries, MaxEntries);
}//end stDBMLogicNode::stDBMLogicNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDBMLogicNode<ObjectType, EvaluatorType>::~stDBMLogicNode(){
   stCount i;
   // If there are entries to be destroied.
   if (Entries != NULL){
      // Destroy every entry...
      for (i = 0; i < this->Count; i++){
         if ((Entries[i].Object != NULL) && (Entries[i].Mine)){
            delete Entries[i].Object;
         }//end if
      }//end for
   }//end if
   this->Count = 0;
   delete[] Entries;
}//end stDBMLogicNode::~stDBMLogicNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDBMLogicNode<ObjectType, EvaluatorType>::AddEntry(stSize size, const stByte * object){
   // If there is space to store.
   if (this->Count < MaxEntries){
      Entries[this->Count].Object = new ObjectType();
      Entries[this->Count].Object->Unserialize(object, size);
      Entries[this->Count].Mine = true;
      this->Count++;
      return this->Count - 1;
   }else{
      return -1;
   }//end if
}//end stDBMLogicNode::AddEntry

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMLogicNode<ObjectType, EvaluatorType>::AddNode(stDBMNode * node){
   stCount i, idx;
   stCount numberOfEntries = node->GetNumberOfEntries();

   #ifdef __stDEBUG__
      // If there is space to store.
      if (this->Count + numberOfEntries > MaxEntries){
         throw invalid_argument("Insuficient space to store the node.");
      }//end if
   #endif //__stDEBUG__

   // For each entry in node.
   for (i = 0; i < numberOfEntries; i++){
      idx = AddEntry(node->GetObjectSize(i), node->GetObject(i));
      this->SetEntry(idx, node->GetEntry(i).PageID
                          #ifdef __stDBMNENTRIES__
                             , node->GetNEntries(i)
                          #endif //__stDBMNENTRIES__
                          , node->GetRadius(i)
                          #ifdef __stDBMHEIGHT__
                             , node->GetEntry(i).Height
                          #endif //__stDBMHEIGHT__
                     );
   }//end for
}//end stDBMLogicNode::AddNode

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMLogicNode<ObjectType, EvaluatorType>::AddCollection(tDBMCollection * returnCollection){
   stCount i, idx;
   stCount numberOfEntries = returnCollection->GetNumberOfEntries();

   #ifdef __stDEBUG__
      // If there is space to store.
      if (this->Count + numberOfEntries > MaxEntries){
         throw invalid_argument("Insuficient space to store the returnCollection.");
      }//end if
   #endif //__stDEBUG__

   // For each entry in returnCollection.
   for (i = 0; i < numberOfEntries; i++){
      Entries[this->Count].Object = (* returnCollection)[i]->GetObject();
      this->SetEntry(this->Count, (* returnCollection)[i]->GetPageID()
                     #ifdef __stDBMNENTRIES__
                        , (* returnCollection)[i]->GetNEntries()
                     #endif //__stDBMNENTRIES__
                     , (* returnCollection)[i]->GetRadius()
                     #ifdef __stDBMHEIGHT__
                        , (* returnCollection)[i]->GetHeight()
                     #endif //__stDBMHEIGHT__
                     );
      // Now the owner of this object is the DBMLogicNode.
      (* returnCollection)[i]->SetMine(false);
      Entries[this->Count].Mine = true;
      this->Count++;
   }//end for
}//end stDBMLogicNode::AddCollection

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMLogicNode<ObjectType, EvaluatorType>::GetNumberOfFreeObjects(){
   stCount i, count;
   count = 0;

   for (i = 0; i < this->Count; i++){
      if (!Entries[i].PageID){
         count++;
      }//end if
   }//end for
   return count;
}//end stDBMLogicNode::GetNumberOfFreeObjects

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMLogicNode<ObjectType, EvaluatorType>::BuildDistanceMatrix(
      EvaluatorType * metricEvaluator){
   int i, j;

   for (i = 0; i < this->Count; i++){
      DMat[i][i] = 0;
      for (j = 0; j < i; j++){
         DMat[i][j] = metricEvaluator->GetDistance(Entries[i].Object, Entries[j].Object);
         DMat[j][i] = DMat[i][j];
      }//end for
   }//end for
   return ((this->Count - 1) * this->Count) / 2;
}//end stDBMLogicNode::BuildDistanceMatrix

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMLogicNode<ObjectType, EvaluatorType>::TestDistribution(
      tDBMCollection * returnCollection,
      tDBMMemNode * node0, tDBMMemNode * node1,
      EvaluatorType * metricEvaluator){
   int idx, i;
   int idx0, idx1;
   int currObj;
   stDistanceIndex * distanceIndex0, * distanceIndex1;
   stDistance distanceReps;
   stDistance RadiusNode0, RadiusNode1;
   stCount idxRep0, idxRep1;

   // Clean before use.
   node0->RemoveAll();
   node1->RemoveAll();
   returnCollection->RemoveAll();

   // Get the index of representatives.
   idxRep0 = GetRepresentativeIndex(0);
   idxRep1 = GetRepresentativeIndex(1);

   // Get the distance from the 2 representatives.
   distanceReps = DMat[idxRep0][idxRep1];

   // Init Map and Sorting vector
   distanceIndex0 = new stDistanceIndex[this->Count];
   distanceIndex1 = new stDistanceIndex[this->Count];

   for (i = 0; i < this->Count; i++){
      if (i == idxRep0){
         distanceIndex0[i].Distance = 0.0;
         distanceIndex1[i].Distance = MAXDOUBLE;
      }else if (i == idxRep1){
         distanceIndex0[i].Distance = MAXDOUBLE;
         distanceIndex1[i].Distance = 0.0;
      }else{
         distanceIndex0[i].Distance = DMat[i][idxRep0];
         distanceIndex1[i].Distance = DMat[i][idxRep1];
      }//end if
      distanceIndex0[i].Index = i;
      distanceIndex1[i].Index = i;
      Entries[i].Mapped = false;
      Entries[i].Mine = true;
   }//end for

   // Sorting by distance...
   sort(distanceIndex0, distanceIndex0 + this->Count);
   sort(distanceIndex1, distanceIndex1 + this->Count);

   // Make one of then get the minimum occupation.
   idx0 = idx1 = 0;

   // Add at least MinOccupation objects to each node.
   for (i = 0; i < this->MinOccupation; i++){
      // Find a candidate for node 1
      while (Entries[distanceIndex0[idx0].Index].Mapped){
         idx0++;
      }//end while
      // Add to node 1
      currObj = distanceIndex0[idx0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object,
                            Entries[currObj].PageID);
      node0->SetEntry(idx, distanceIndex0[idx0].Distance
                    #ifdef __stDBMNENTRIES__
                       , Entries[currObj].NEntries
                    #endif //__stDBMNENTRIES__
                       , Entries[currObj].Radius
                    #ifdef __stDBMHEIGHT__
                       , Entries[currObj].Height
                    #endif //__stDBMHEIGHT__
                    );

      // Find a candidate for node 2
      while (Entries[distanceIndex1[idx1].Index].Mapped){
         idx1++;
      }//end while
      // Add to node 2
      currObj = distanceIndex1[idx1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object,
                            Entries[currObj].PageID);
      node1->SetEntry(idx, distanceIndex1[idx1].Distance
                    #ifdef __stDBMNENTRIES__
                       , Entries[currObj].NEntries
                    #endif //__stDBMNENTRIES__
                       , Entries[currObj].Radius
                    #ifdef __stDBMHEIGHT__
                       , Entries[currObj].Height
                    #endif //__stDBMHEIGHT__
                    );
   }//end for

   // update the radius of this node.
   RadiusNode0 = node0->GetMinimumRadius();
   RadiusNode1 = node1->GetMinimumRadius();

   // Distribute the others.
   for (i = 0; i < this->Count; i++){
      if (Entries[i].Mapped == false){
         Entries[i].Mapped = true;
         // This entry is nearer to rep0?
         if (DMat[i][idxRep0] < DMat[i][idxRep1]){
            // If this entry is covered by Node0 Or This new object stays in the node0 area.
            if ((DMat[i][idxRep0] + Entries[i].Radius <= RadiusNode0) ||
                (DMat[i][idxRep0] + Entries[i].Radius + RadiusNode1 <= distanceReps)){
               // Try to put on node 1 first
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object,
                                     Entries[i].PageID);
               if (idx >= 0){
                  node0->SetEntry(idx, DMat[i][idxRep0]
                                #ifdef __stDBMNENTRIES__
                                   , Entries[i].NEntries
                                #endif //__stDBMNENTRIES__
                                   , Entries[i].Radius
                                #ifdef __stDBMHEIGHT__
                                   , Entries[i].Height
                                #endif //__stDBMHEIGHT__
                                );
                  // update the radius of this node.
                  if (RadiusNode0 < DMat[i][idxRep0] + Entries[i].Radius){
                     RadiusNode0 = DMat[i][idxRep0] + Entries[i].Radius;
                  }//end if
               }else if ((DMat[i][idxRep1] + Entries[i].Radius <= RadiusNode1) ||
                         (DMat[i][idxRep1] + Entries[i].Radius + RadiusNode0 <= distanceReps)){
                  // Let's put it in the node 2 since it doesn't fit in the node 1
                  idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                        Entries[i].Object,
                                        Entries[i].PageID);
                  if (idx >= 0){
                     node1->SetEntry(idx, DMat[i][idxRep1]
                                   #ifdef __stDBMNENTRIES__
                                      , Entries[i].NEntries
                                   #endif //__stDBMNENTRIES__
                                      , Entries[i].Radius
                                   #ifdef __stDBMHEIGHT__
                                      , Entries[i].Height
                                   #endif //__stDBMHEIGHT__
                                   );
                     // update the radius of this node.
                     if (RadiusNode1 < DMat[i][idxRep1] + Entries[i].Radius){
                        RadiusNode1 = DMat[i][idxRep1] + Entries[i].Radius;
                     }//end if
                  }else{
                     // Let's put it in the returnCollection since it does
                     // not fit in the node 1 and 2
                     returnCollection->AddEntry(Entries[i].Object,
                                       #ifdef __stDBMHEIGHT__
                                          Entries[i].Height,
                                       #endif //__stDBMHEIGHT__
                                       Entries[i].PageID,
                                       #ifdef __stDBMNENTRIES__
                                          Entries[i].NEntries,
                                       #endif //__stDBMNENTRIES__
                                       Entries[i].Radius, false);
                  }//end if
               }else{
                  returnCollection->AddEntry(Entries[i].Object,
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius, false);
               }//end if
            }else if ((DMat[i][idxRep1] + Entries[i].Radius <= RadiusNode1) ||
                      (DMat[i][idxRep1] + Entries[i].Radius + RadiusNode0 <= distanceReps)){
               // If this entry is covered by Node0 Or This new object stays in the node0 area.
               // Let's put it in the node 2 since it doesn't fit in the node 1
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object,
                                     Entries[i].PageID);
               if (idx >= 0){
                  node1->SetEntry(idx, DMat[i][idxRep1]
                                #ifdef __stDBMNENTRIES__
                                   , Entries[i].NEntries
                                #endif //__stDBMNENTRIES__
                                   , Entries[i].Radius
                                #ifdef __stDBMHEIGHT__
                                   , Entries[i].Height
                                #endif //__stDBMHEIGHT__
                                );
                  // update the radius of this node.
                  if (RadiusNode1 < DMat[i][idxRep1] + Entries[i].Radius){
                     RadiusNode1 = DMat[i][idxRep1] + Entries[i].Radius;
                  }//end if
               }else{
                  // Let's put it in the returnCollection since it does
                  // not fit in the node 1 and 2
                  returnCollection->AddEntry(Entries[i].Object,
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius, false);
               }//end if
            }else{
               returnCollection->AddEntry(Entries[i].Object,
                                 #ifdef __stDBMHEIGHT__
                                    Entries[i].Height,
                                 #endif //__stDBMHEIGHT__
                                 Entries[i].PageID,
                                 #ifdef __stDBMNENTRIES__
                                    Entries[i].NEntries,
                                 #endif //__stDBMNENTRIES__
                                 Entries[i].Radius, false);
            }//end if
         }else{
            // If this entry is covered by Node1 Or This new object stays in the node1 area.
            if ((DMat[i][idxRep1] + Entries[i].Radius <= RadiusNode1) ||
                (DMat[i][idxRep1] + Entries[i].Radius + RadiusNode0 <= distanceReps)){
               // Try to put on node 2 first
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object,
                                     Entries[i].PageID);
               if (idx >= 0){
                  node1->SetEntry(idx, DMat[i][idxRep1]
                                #ifdef __stDBMNENTRIES__
                                   , Entries[i].NEntries
                                #endif //__stDBMNENTRIES__
                                   , Entries[i].Radius
                                #ifdef __stDBMHEIGHT__
                                   , Entries[i].Height
                                #endif //__stDBMHEIGHT__
                                );
                  // update the radius of this node.
                  if (RadiusNode1 < DMat[i][idxRep1] + Entries[i].Radius){
                     RadiusNode1 = DMat[i][idxRep1] + Entries[i].Radius;
                  }//end if
               }else if ((DMat[i][idxRep0] + Entries[i].Radius <= RadiusNode0) ||
                         (DMat[i][idxRep0] + Entries[i].Radius + RadiusNode1 <= distanceReps)){
                  // Let's put it in the node 1 since it doesn't fit in the node 2
                  idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                        Entries[i].Object,
                                        Entries[i].PageID);
                  if (idx >= 0){
                     node0->SetEntry(idx, DMat[i][idxRep0]
                                   #ifdef __stDBMNENTRIES__
                                      , Entries[i].NEntries
                                   #endif //__stDBMNENTRIES__
                                      , Entries[i].Radius
                                   #ifdef __stDBMHEIGHT__
                                      , Entries[i].Height
                                   #endif //__stDBMHEIGHT__
                                   );
                     // update the radius of this node.
                  if (RadiusNode0 < DMat[i][idxRep0] + Entries[i].Radius){
                     RadiusNode0 = DMat[i][idxRep0] + Entries[i].Radius;
                  }//end if
                  }else{
                     returnCollection->AddEntry(Entries[i].Object,
                                       #ifdef __stDBMHEIGHT__
                                          Entries[i].Height,
                                       #endif //__stDBMHEIGHT__
                                       Entries[i].PageID,
                                       #ifdef __stDBMNENTRIES__
                                          Entries[i].NEntries,
                                       #endif //__stDBMNENTRIES__
                                       Entries[i].Radius, false);
                  }//end if
               }else{
                  returnCollection->AddEntry(Entries[i].Object,
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius, false);
               }//end if
            }else if ((DMat[i][idxRep0] + Entries[i].Radius <= RadiusNode0) ||
                      (DMat[i][idxRep0] + Entries[i].Radius + RadiusNode1 <= distanceReps)){
               // If this entry is covered by Node1 Or This new object stays in the node1 area.
               // Let's put it in the node 1 since it doesn't fit in the node 2
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object,
                                     Entries[i].PageID);
               if (idx >= 0){
                  node0->SetEntry(idx, DMat[i][idxRep0]
                                #ifdef __stDBMNENTRIES__
                                   , Entries[i].NEntries
                                #endif //__stDBMNENTRIES__
                                   , Entries[i].Radius
                                #ifdef __stDBMHEIGHT__
                                   , Entries[i].Height
                                #endif //__stDBMHEIGHT__
                                );
                  // update the radius of this node.
                  if (RadiusNode0 < DMat[i][idxRep0] + Entries[i].Radius){
                     RadiusNode0 = DMat[i][idxRep0] + Entries[i].Radius;
                  }//end if
               }else{
                  returnCollection->AddEntry(Entries[i].Object,
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius, false);
               }//end if
            }else{
               returnCollection->AddEntry(Entries[i].Object,
                                 #ifdef __stDBMHEIGHT__
                                    Entries[i].Height,
                                 #endif //__stDBMHEIGHT__
                                 Entries[i].PageID,
                                 #ifdef __stDBMNENTRIES__
                                    Entries[i].NEntries,
                                 #endif //__stDBMNENTRIES__
                                 Entries[i].Radius, false);
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete[] distanceIndex0;
   delete[] distanceIndex1;

}//end stDBMLogicNode::TestDistribution

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMLogicNode<ObjectType, EvaluatorType>::TestDistribution(
         tDBMMemNode * node0, tDBMMemNode * node1,
         EvaluatorType * metricEvaluator){
   int idx, i;
   int idx0, idx1;
   int currObj;
   stDistanceIndex * distanceIndex0, * distanceIndex1;
   stCount idxRep0, idxRep1;

   // Clean before use.
   node0->RemoveAll();
   node1->RemoveAll();

   // Get the index of representatives.
   idxRep0 = GetRepresentativeIndex(0);
   idxRep1 = GetRepresentativeIndex(1);

   // Get space to store.
   distanceIndex0 = new stDistanceIndex[this->Count];
   distanceIndex1 = new stDistanceIndex[this->Count];
   // Init Map
   for (i = 0; i < this->Count; i++){
      distanceIndex0[i].Index = i;
      distanceIndex0[i].Distance = DMat[i][idxRep0];
      distanceIndex1[i].Index = i;
      distanceIndex1[i].Distance = DMat[i][idxRep1];
      Entries[i].Mapped = false;
      Entries[i].Mine = true;
   }//end for
   // Sorting by distance...
   sort(distanceIndex0, distanceIndex0 + this->Count);
   sort(distanceIndex1, distanceIndex1 + this->Count);

   // Make one of then get the minimum occupation.
   idx0 = idx1 = 0;

   // Add at least this->MinOccupation objects to each node.
//   for (i = 0; i < 2; i++){
      // Find a candidate for node 1
      while (Entries[distanceIndex0[idx0].Index].Mapped){
         idx0++;
      }//end while
      // Add to node 1
      currObj = distanceIndex0[idx0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object,
                            Entries[currObj].PageID);
      node0->SetEntry(idx, distanceIndex0[idx0].Distance
                    #ifdef __stDBMNENTRIES__
                       , Entries[currObj].NEntries
                    #endif //__stDBMNENTRIES__
                       , Entries[currObj].Radius
                    #ifdef __stDBMHEIGHT__
                       , Entries[currObj].Height
                    #endif //__stDBMHEIGHT__
                    );

      // Find a candidate for node 2
      while (Entries[distanceIndex1[idx1].Index].Mapped){
         idx1++;
      }//end while
      // Add to node 2
      currObj = distanceIndex1[idx1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object,
                            Entries[currObj].PageID);
      node1->SetEntry(idx, distanceIndex1[idx1].Distance
                    #ifdef __stDBMNENTRIES__
                       , Entries[currObj].NEntries
                    #endif //__stDBMNENTRIES__
                       , Entries[currObj].Radius
                    #ifdef __stDBMHEIGHT__
                       , Entries[currObj].Height
                    #endif //__stDBMHEIGHT__
                    );
//   }//end for

   // Distribute the others.
   for (i = 0; i < this->Count; i++){
      // If this entry was not mapped.
      if (Entries[i].Mapped == false){
         // Set this entry to mapped.
         Entries[i].Mapped = true;
         // Where I will put it? In node 1 or 2?
         if (DMat[i][idxRep0] < DMat[i][idxRep1]){
            // This new entry has a distance to node 1 lesser than to node 2.
            // Try to put on node 1 first
            idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object,
                                  Entries[i].PageID);
            if (idx >= 0){
               node0->SetEntry(idx, DMat[i][idxRep0]
                             #ifdef __stDBMNENTRIES__
                                , Entries[i].NEntries
                             #endif //__stDBMNENTRIES__
                                , Entries[i].Radius
                             #ifdef __stDBMHEIGHT__
                                , Entries[i].Height
                             #endif //__stDBMHEIGHT__
                             );
            }else {
               // Let's put it in the node 2 since it doesn't fit in the node 1
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object,
                                     Entries[i].PageID);
               node1->SetEntry(idx, DMat[i][idxRep1]
                             #ifdef __stDBMNENTRIES__
                                , Entries[i].NEntries
                             #endif //__stDBMNENTRIES__
                                , Entries[i].Radius
                             #ifdef __stDBMHEIGHT__
                                , Entries[i].Height
                             #endif //__stDBMHEIGHT__
                             );
            }//end if
         }else{
            // Try to put on node 2 first
            idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object,
                                  Entries[i].PageID);
            if (idx >= 0){
               node1->SetEntry(idx, DMat[i][idxRep1]
                             #ifdef __stDBMNENTRIES__
                                , Entries[i].NEntries
                             #endif //__stDBMNENTRIES__
                                , Entries[i].Radius
                             #ifdef __stDBMHEIGHT__
                                , Entries[i].Height
                             #endif //__stDBMHEIGHT__
                             );
            }else {
               // Let's put it in the node 1 since it doesn't fit in the node 2
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object,
                                     Entries[i].PageID);
               node0->SetEntry(idx, DMat[i][idxRep0]
                             #ifdef __stDBMNENTRIES__
                                , Entries[i].NEntries
                             #endif //__stDBMNENTRIES__
                                , Entries[i].Radius
                             #ifdef __stDBMHEIGHT__
                                , Entries[i].Height
                             #endif //__stDBMHEIGHT__
                             );
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete[] distanceIndex0;
   delete[] distanceIndex1;

}//end stDBMLogicNode::TestDistribution

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMLogicNode<ObjectType, EvaluatorType>::Distribute(
            tDBMCollection * returnCollection,
            stDBMNode * node0, stDBMNode * node1,
            tSubtreeInfo * promo, EvaluatorType * metricEvaluator){

   int idx;
   int i;
   int idx0, idx1;
   int currObj;
   stDistanceIndex * distanceIndex0, * distanceIndex1;
   stDistance distanceReps;
   stDistance RadiusNode0, RadiusNode1;
   stCount idxRep0, idxRep1;

   // Get the index of representatives.
   idxRep0 = GetRepresentativeIndex(0);
   idxRep1 = GetRepresentativeIndex(1);
   
   // Get the distance from the 2 representatives.
   distanceReps = DMat[idxRep0][idxRep1];

   // Init Map and Sorting vector
   distanceIndex0 = new stDistanceIndex[this->Count];
   distanceIndex1 = new stDistanceIndex[this->Count];

   for (i = 0; i < this->Count; i++){
      if (i == idxRep0){
         distanceIndex0[i].Distance = 0.0;
         distanceIndex1[i].Distance = MAXDOUBLE;
      }else if (i == idxRep1){
         distanceIndex0[i].Distance = MAXDOUBLE;
         distanceIndex1[i].Distance = 0.0;
      }else{
         distanceIndex0[i].Distance = DMat[i][idxRep0];
         distanceIndex1[i].Distance = DMat[i][idxRep1];
      }//end if
      distanceIndex0[i].Index = i;
      distanceIndex1[i].Index = i;
      Entries[i].Mapped = false;
      Entries[i].Mine = true;
   }//end for

   // Sorting by distance...
   sort(distanceIndex0, distanceIndex0 + this->Count);
   sort(distanceIndex1, distanceIndex1 + this->Count);

   // Make one of then get the minimum occupation.
   idx0 = idx1 = 0;

   // Add at least MinOccupation objects to each node.
   for (i = 0; i < this->MinOccupation; i++){
      // Find a candidate for node 1
      while (Entries[distanceIndex0[idx0].Index].Mapped){
         idx0++;
      }//end while
      // Add to node 1
      currObj = distanceIndex0[idx0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize(),
                            Entries[currObj].PageID);
      node0->GetEntry(idx).Distance = distanceIndex0[idx0].Distance;
      #ifdef __stDBMNENTRIES__
         node0->SetNEntries(idx, Entries[currObj].NEntries);
      #endif //__stDBMNENTRIES__
      node0->SetRadius(idx, Entries[currObj].Radius);
      #ifdef __stDBMHEIGHT__
         node0->GetEntry(idx).Height = Entries[currObj].Height;
      #endif //__stDBMHEIGHT__

      // Find a candidate for node 2
      while (Entries[distanceIndex1[idx1].Index].Mapped){
         idx1++;
      }//end while
      // Add to node 2
      currObj = distanceIndex1[idx1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize(),
                            Entries[currObj].PageID);
      node1->GetEntry(idx).Distance = distanceIndex1[idx1].Distance;
      #ifdef __stDBMNENTRIES__
         node1->SetNEntries(idx, Entries[currObj].NEntries);
      #endif //__stDBMNENTRIES__
      node1->SetRadius(idx, Entries[currObj].Radius);
      #ifdef __stDBMHEIGHT__
         node1->GetEntry(idx).Height = Entries[currObj].Height;
      #endif //__stDBMHEIGHT__
   }//end for

   // update the radius of this node.
   RadiusNode0 = node0->GetMinimumRadius();
   RadiusNode1 = node1->GetMinimumRadius();

   // Distribute the others.
   for (i = 0; i < this->Count; i++){
      if (Entries[i].Mapped == false){
         Entries[i].Mapped = true;
         // This entry is nearer to rep0?
         if (DMat[i][idxRep0] < DMat[i][idxRep1]){
            // If this entry is covered by Node0 Or This new object stays in the node0 area.
            if ((DMat[i][idxRep0] + Entries[i].Radius <= RadiusNode0) ||
                (DMat[i][idxRep0] + Entries[i].Radius + RadiusNode1 <= distanceReps)){
               // Try to put on node 1 first
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize(),
                                     Entries[i].PageID);
               if (idx >= 0){
                  node0->GetEntry(idx).Distance = DMat[i][idxRep0];
                  #ifdef __stDBMNENTRIES__
                     node0->SetNEntries(idx, Entries[i].NEntries);
                  #endif //__stDBMNENTRIES__
                  node0->SetRadius(idx, Entries[i].Radius);
                  #ifdef __stDBMHEIGHT__
                     node0->GetEntry(idx).Height = Entries[i].Height;
                  #endif //__stDBMHEIGHT__
                  // update the radius of this node.
                  if (RadiusNode0 < DMat[i][idxRep0] + Entries[i].Radius){
                     RadiusNode0 = DMat[i][idxRep0] + Entries[i].Radius;
                  }//end if
               }else if ((DMat[i][idxRep1] + Entries[i].Radius <= RadiusNode1) ||
                         (DMat[i][idxRep1] + Entries[i].Radius + RadiusNode0 <= distanceReps)){
                  // Let's put it in the node 2 since it doesn't fit in the node 1
                  idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                        Entries[i].Object->Serialize(),
                                        Entries[i].PageID);
                  if (idx >= 0){
                     node1->GetEntry(idx).Distance = DMat[i][idxRep1];
                     #ifdef __stDBMNENTRIES__
                        node1->SetNEntries(idx, Entries[i].NEntries);
                     #endif //__stDBMNENTRIES__
                     node1->SetRadius(idx, Entries[i].Radius);
                     #ifdef __stDBMHEIGHT__
                        node1->GetEntry(idx).Height = Entries[i].Height;
                     #endif //__stDBMHEIGHT__
                     // update the radius of this node.
                     if (RadiusNode1 < DMat[i][idxRep1] + Entries[i].Radius){
                        RadiusNode1 = DMat[i][idxRep1] + Entries[i].Radius;
                     }//end if
                  }else{
                     // Let's put it in the returnCollection since it does
                     // not fit in the node 1 and 2
                     // Now the owner of this object is returnCollection.
                     returnCollection->AddEntry(BuyObject(i),
                                       #ifdef __stDBMHEIGHT__
                                          Entries[i].Height,
                                       #endif //__stDBMHEIGHT__
                                       Entries[i].PageID,
                                       #ifdef __stDBMNENTRIES__
                                          Entries[i].NEntries,
                                       #endif //__stDBMNENTRIES__
                                       Entries[i].Radius);
                  }//end if
               }else{
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius);
               }//end if
            }else if ((DMat[i][idxRep1] + Entries[i].Radius <= RadiusNode1) ||
                      (DMat[i][idxRep1] + Entries[i].Radius + RadiusNode0 <= distanceReps)){
               // If this entry is covered by Node0 Or This new object stays in the node0 area.
               // Let's put it in the node 2 since it doesn't fit in the node 1
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize(),
                                     Entries[i].PageID);
               if (idx >= 0){
                  node1->GetEntry(idx).Distance = DMat[i][idxRep1];
                  #ifdef __stDBMNENTRIES__
                     node1->SetNEntries(idx, Entries[i].NEntries);
                  #endif //__stDBMNENTRIES__
                  node1->SetRadius(idx, Entries[i].Radius);
                  #ifdef __stDBMHEIGHT__
                     node1->GetEntry(idx).Height = Entries[i].Height;
                  #endif //__stDBMHEIGHT__
                  // update the radius of this node.
                  if (RadiusNode1 < DMat[i][idxRep1] + Entries[i].Radius){
                     RadiusNode1 = DMat[i][idxRep1] + Entries[i].Radius;
                  }//end if
               }else{
                  // Let's put it in the returnCollection since it does
                  // not fit in the node 1 and 2
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius);
               }//end if
            }else{
               // Now the owner of this object is returnCollection.
               returnCollection->AddEntry(BuyObject(i),
                                 #ifdef __stDBMHEIGHT__
                                    Entries[i].Height,
                                 #endif //__stDBMHEIGHT__
                                 Entries[i].PageID,
                                 #ifdef __stDBMNENTRIES__
                                    Entries[i].NEntries,
                                 #endif //__stDBMNENTRIES__
                                 Entries[i].Radius);
            }//end if
         }else{
            // If this entry is covered by Node1 Or This new object stays in the node1 area.
            if ((DMat[i][idxRep1] + Entries[i].Radius <= RadiusNode1) ||
                (DMat[i][idxRep1] + Entries[i].Radius + RadiusNode0 <= distanceReps)){
               // Try to put on node 2 first
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize(),
                                     Entries[i].PageID);
               if (idx >= 0){
                  node1->GetEntry(idx).Distance = DMat[i][idxRep1];
                  #ifdef __stDBMNENTRIES__
                     node1->SetNEntries(idx, Entries[i].NEntries);
                  #endif //__stDBMNENTRIES__
                  node1->SetRadius(idx, Entries[i].Radius);
                  #ifdef __stDBMHEIGHT__
                     node1->GetEntry(idx).Height = Entries[i].Height;
                  #endif //__stDBMHEIGHT__
                  // update the radius of this node.
                  if (RadiusNode1 < DMat[i][idxRep1] + Entries[i].Radius){
                     RadiusNode1 = DMat[i][idxRep1] + Entries[i].Radius;
                  }//end if
               }else if ((DMat[i][idxRep0] + Entries[i].Radius <= RadiusNode0) ||
                         (DMat[i][idxRep0] + Entries[i].Radius + RadiusNode1 <= distanceReps)){
                  // Let's put it in the node 1 since it doesn't fit in the node 2
                  idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                        Entries[i].Object->Serialize(),
                                        Entries[i].PageID);
                  if (idx >= 0){
                     node0->GetEntry(idx).Distance = DMat[i][idxRep0];
                     #ifdef __stDBMNENTRIES__
                        node0->SetNEntries(idx, Entries[i].NEntries);
                     #endif //__stDBMNENTRIES__
                     node0->SetRadius(idx, Entries[i].Radius);
                     #ifdef __stDBMHEIGHT__
                        node0->GetEntry(idx).Height = Entries[i].Height;
                     #endif //__stDBMHEIGHT__
                     // update the radius of this node.
                     if (RadiusNode0 < DMat[i][idxRep0] + Entries[i].Radius){
                        RadiusNode0 = DMat[i][idxRep0] + Entries[i].Radius;
                     }//end if
                  }else{
                     // Now the owner of this object is returnCollection.
                     returnCollection->AddEntry(BuyObject(i),
                                       #ifdef __stDBMHEIGHT__
                                          Entries[i].Height,
                                       #endif //__stDBMHEIGHT__
                                       Entries[i].PageID,
                                       #ifdef __stDBMNENTRIES__
                                          Entries[i].NEntries,
                                       #endif //__stDBMNENTRIES__
                                       Entries[i].Radius);
                  }//end if
               }else{
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius);
               }//end if
            }else if ((DMat[i][idxRep0] + Entries[i].Radius <= RadiusNode0) ||
                      (DMat[i][idxRep0] + Entries[i].Radius + RadiusNode1 <= distanceReps)){
               // If this entry is covered by Node1 Or This new object stays in the node1 area.
               // Let's put it in the node 1 since it doesn't fit in the node 2
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize(),
                                     Entries[i].PageID);
               if (idx >= 0){
                  node0->GetEntry(idx).Distance = DMat[i][idxRep0];
                  #ifdef __stDBMNENTRIES__
                     node0->SetNEntries(idx, Entries[i].NEntries);
                  #endif //__stDBMNENTRIES__
                  node0->SetRadius(idx, Entries[i].Radius);
                  #ifdef __stDBMHEIGHT__
                     node0->GetEntry(idx).Height = Entries[i].Height;
                  #endif //__stDBMHEIGHT__
                  // update the radius of this node.
                  if (RadiusNode0 < DMat[i][idxRep0] + Entries[i].Radius){
                     RadiusNode0 = DMat[i][idxRep0] + Entries[i].Radius;
                  }//end if
               }else{
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Entries[i].Height,
                                    #endif //__stDBMHEIGHT__
                                    Entries[i].PageID,
                                    #ifdef __stDBMNENTRIES__
                                       Entries[i].NEntries,
                                    #endif //__stDBMNENTRIES__
                                    Entries[i].Radius);
               }//end if
            }else{
               // Now the owner of this object is returnCollection.
               returnCollection->AddEntry(BuyObject(i),
                                 #ifdef __stDBMHEIGHT__
                                    Entries[i].Height,
                                 #endif //__stDBMHEIGHT__
                                 Entries[i].PageID,
                                 #ifdef __stDBMNENTRIES__
                                    Entries[i].NEntries,
                                 #endif //__stDBMNENTRIES__
                                 Entries[i].Radius);
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete[] distanceIndex0;
   delete[] distanceIndex1;

   // Representatives
   promo[0].Rep = BuyObject(GetRepresentativeIndex(0));
   promo[0].Radius = node0->GetMinimumRadius();
   promo[0].RootID = node0->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[0].NObjects = node0->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[0].Height = node0->GetHeight() + 1;
   #endif //__stDBMHEIGHT__

   promo[1].Rep = BuyObject(GetRepresentativeIndex(1));
   promo[1].Radius = node1->GetMinimumRadius();
   promo[1].RootID = node1->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[1].NObjects = node1->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[1].Height = node1->GetHeight() + 1;
   #endif //__stDBMHEIGHT__
}//end stDBMLogicNode::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMLogicNode<ObjectType, EvaluatorType>::Distribute(
            stDBMNode * node0, stDBMNode * node1,
            tSubtreeInfo * promo, EvaluatorType * metricEvaluator){

   int idx;
   int i;
   int idx0, idx1;
   int currObj;
   stDistanceIndex * distanceIndex0, * distanceIndex1;
   stCount idxRep0, idxRep1;

   // Get the index of representatives.
   idxRep0 = GetRepresentativeIndex(0);
   idxRep1 = GetRepresentativeIndex(1);

   // Get space to store.
   distanceIndex0 = new stDistanceIndex[this->Count];
   distanceIndex1 = new stDistanceIndex[this->Count];
   // Init Map
   for (i = 0; i < this->Count; i++){
      distanceIndex0[i].Index = i;
      distanceIndex0[i].Distance = DMat[i][idxRep0];
      distanceIndex1[i].Index = i;
      distanceIndex1[i].Distance = DMat[i][idxRep1];
      Entries[i].Mapped = false;
      Entries[i].Mine = true;
   }//end for
   // Sorting by distance...
   sort(distanceIndex0, distanceIndex0 + this->Count);
   sort(distanceIndex1, distanceIndex1 + this->Count);

   // Make one of then get the minimum occupation.
   idx0 = idx1 = 0;

   // Add at least this->MinOccupation objects to each node.
//   for (i = 0; i < 2; i++){
      // Find a candidate for node 1
      while (Entries[distanceIndex0[idx0].Index].Mapped){
         idx0++;
      }//end while
      // Add to node 1
      currObj = distanceIndex0[idx0].Index;
      Entries[currObj].Mapped = true;
      idx = node0->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize(),
                            Entries[currObj].PageID);
      node0->GetEntry(idx).Distance = distanceIndex0[idx0].Distance;
      #ifdef __stDBMNENTRIES__
         node0->SetNEntries(idx, Entries[currObj].NEntries);
      #endif //__stDBMNENTRIES__
      node0->SetRadius(idx, Entries[currObj].Radius);
      #ifdef __stDBMHEIGHT__
         node0->GetEntry(idx).Height = Entries[currObj].Height;
      #endif //__stDBMHEIGHT__

      // Find a candidate for node 2
      while (Entries[distanceIndex1[idx1].Index].Mapped){
         idx1++;
      }//end while
      // Add to node 2
      currObj = distanceIndex1[idx1].Index;
      Entries[currObj].Mapped = true;
      idx = node1->AddEntry(Entries[currObj].Object->GetSerializedSize(),
                            Entries[currObj].Object->Serialize(),
                            Entries[currObj].PageID);
      node1->GetEntry(idx).Distance = distanceIndex1[idx1].Distance;
      #ifdef __stDBMNENTRIES__
         node1->SetNEntries(idx, Entries[currObj].NEntries);
      #endif //__stDBMNENTRIES__
      node1->SetRadius(idx, Entries[currObj].Radius);
      #ifdef __stDBMHEIGHT__
         node1->GetEntry(idx).Height = Entries[currObj].Height;
      #endif //__stDBMHEIGHT__
//   }//end for

   // Distribute the others.
   for (i = 0; i < this->Count; i++){
      // If this entry was not mapped.
      if (Entries[i].Mapped == false){
         // Set this entry to mapped.
         Entries[i].Mapped = true;
         // Where I will put it? In node 1 or 2?
         if (DMat[i][idxRep0] < DMat[i][idxRep1]){
            // This new entry has a distance to node 1 lesser than to node 2.
            // Try to put on node 1 first
            idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object->Serialize(),
                                  Entries[i].PageID);
            if (idx >= 0){
               node0->GetEntry(idx).Distance = DMat[i][idxRep0];
               #ifdef __stDBMNENTRIES__
                  node0->SetNEntries(idx, Entries[i].NEntries);
               #endif //__stDBMNENTRIES__
               node0->SetRadius(idx, Entries[i].Radius);
               #ifdef __stDBMHEIGHT__
                  node0->GetEntry(idx).Height = Entries[i].Height;
               #endif //__stDBMHEIGHT__
            }else {
               // Let's put it in the node 2 since it doesn't fit in the node 1
               idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize(),
                                     Entries[i].PageID);
               node1->GetEntry(idx).Distance = DMat[i][idxRep1];
               #ifdef __stDBMNENTRIES__
                  node1->SetNEntries(idx, Entries[i].NEntries);
               #endif //__stDBMNENTRIES__
               node1->SetRadius(idx, Entries[i].Radius);
               #ifdef __stDBMHEIGHT__
                  node1->GetEntry(idx).Height = Entries[i].Height;
               #endif //__stDBMHEIGHT__
            }//end if
         }else{
            // Try to put on node 2 first
            idx = node1->AddEntry(Entries[i].Object->GetSerializedSize(),
                                  Entries[i].Object->Serialize(),
                                  Entries[i].PageID);
            if (idx >= 0){
               node1->GetEntry(idx).Distance = DMat[i][idxRep1];
               #ifdef __stDBMNENTRIES__
                  node1->SetNEntries(idx, Entries[i].NEntries);
               #endif //__stDBMNENTRIES__
               node1->SetRadius(idx, Entries[i].Radius);
               #ifdef __stDBMHEIGHT__
                  node1->GetEntry(idx).Height = Entries[i].Height;
               #endif //__stDBMHEIGHT__
            }else {
               // Let's put it in the node 1 since it doesn't fit in the node 2
               idx = node0->AddEntry(Entries[i].Object->GetSerializedSize(),
                                     Entries[i].Object->Serialize(),
                                     Entries[i].PageID);
               node0->GetEntry(idx).Distance = DMat[i][idxRep0];
               #ifdef __stDBMNENTRIES__
                  node0->SetNEntries(idx, Entries[i].NEntries);
               #endif //__stDBMNENTRIES__
               node0->SetRadius(idx, Entries[i].Radius);
               #ifdef __stDBMHEIGHT__
                  node0->GetEntry(idx).Height = Entries[i].Height;
               #endif //__stDBMHEIGHT__
            }//end if
         }//end if
      }//end if
   }//end for

   // Clean home before go away...
   delete[] distanceIndex0;
   delete[] distanceIndex1;

   // Representatives
   promo[0].Rep = BuyObject(GetRepresentativeIndex(0));
   promo[0].Radius = node0->GetMinimumRadius();
   promo[0].RootID = node0->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[0].NObjects = node0->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[0].Height = node0->GetHeight() + 1;
   #endif //__stDBMHEIGHT__

   promo[1].Rep = BuyObject(GetRepresentativeIndex(1));
   promo[1].Radius = node1->GetMinimumRadius();
   promo[1].RootID = node1->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[1].NObjects = node1->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[1].Height = node1->GetHeight() + 1;
   #endif //__stDBMHEIGHT__

}//end stDBMLogicNode::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMLogicNode<ObjectType, EvaluatorType>::UpdateDistances(
      EvaluatorType * metricEvaluator){
   stCount i;

   for (i = 0; i < this->Count; i++){
      // Is this entry a representative 1?
      if (i == GetRepresentativeIndex(0)){
         this->DMat[i][this->idxRep0] = 0.0;
         this->DMat[i][this->idxRep1] = MAXDOUBLE;
      }else if (i == GetRepresentativeIndex(1)){ // Is this entry a representative 2?
         this->DMat[i][this->idxRep0] = MAXDOUBLE;
         this->DMat[i][this->idxRep1] = 0.0;
      }else{
         // this entry is not a representative?
         this->DMat[i][this->idxRep0] = this->metricEvaluator->GetDistance(
               Entries[GetRepresentativeIndex(0)].Object, Entries[i].Object);
         this->DMat[i][this->idxRep1] = this->metricEvaluator->GetDistance(
               Entries[GetRepresentativeIndex(1)].Object, Entries[i].Object);
      }//end if
   }//end for

   // return the total distance calculate.
   return (GetNumberOfEntries() * 2) - 2;
}//end stDBMLogicNode::UpdateDistances

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMLogicNode<ObjectType, EvaluatorType>::MaxDistancePromote(){
   stCount i, j;
   stDistance maxDistance = 0;

   for (i = 0; i < this->Count-1; i++){
      for (j = 1; j < this->Count; j++){
         if (DMat[i][j] > maxDistance){
            maxDistance = DMat[i][j];
            this->SetRepresentative(i, j);
         }//end if
      }//end if
   }//end for
}//end stDBMLogicNode::MaxDistancePromote



























//=============================================================================
// Class template stDBMMSTSpliter
//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDBMMSTSplitter<ObjectType, EvaluatorType>::stDBMMSTSplitter(
      tLogicNode * node){

   this->Node = node;
   this->N = Node->GetNumberOfEntries();

   // Dynamic fields
   this->Cluster = new tCluster[N];
   this->ObjectCluster = new int[N];

   // Matrix
   DMat.SetSize(N, N);
}//end stDBMMSTSplitter::stDBMMSTSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDBMMSTSplitter<ObjectType, EvaluatorType>::~stDBMMSTSplitter(){

   if (Cluster != NULL)
      delete[] Cluster;
   if (ObjectCluster != NULL)
      delete[] ObjectCluster;
}//end stDBMMSTSplitter::~stDBMMSTSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMMSTSplitter<ObjectType, EvaluatorType>::BuildDistanceMatrix(
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
   return ((N - 1) * N) / 2;
}//end stDBMMSTSplitter::BuildDistanceMatrix

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDBMMSTSplitter<ObjectType, EvaluatorType>::FindCenter(int clus){
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
}//end stDBMMSTSplitter::FindCenter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMMSTSplitter<ObjectType, EvaluatorType>::PerformMST(){
   int i, j, k, l, cc, iBig, iBigOpposite, a, b, c;
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
                              JoinClusters(b, a);
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
            throw logic_error("At least an object has no cluster.");
         }//end if
      }//end for
   #endif //__stDEBUG__

   // Representatives
   Node->SetRepresentative(FindCenter(Cluster0), FindCenter(Cluster1));
}//end stDBMMSTSplitter::PerformMST

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMMSTSplitter<ObjectType, EvaluatorType>::Distribute(
            stDBMNode * node0, stDBMNode * node1,
            tSubtreeInfo * promo, EvaluatorType * metricEvaluator){
   stCount distanceCount;
   int idx, i, idxRep0, idxRep1;

   // Build Distance matrix
   distanceCount = BuildDistanceMatrix(metricEvaluator);

   //Perform MST
   this->PerformMST();

   idxRep0 = Node->GetRepresentativeIndex(0);
   idxRep1 = Node->GetRepresentativeIndex(1);

   // Add the first representative
   idx = node0->AddEntry(Node->GetObject(idxRep0)->GetSerializedSize(),
                         Node->GetObject(idxRep0)->Serialize(),
                         Node->GetPageID(idxRep0));
   node0->GetEntry(idx).Distance = 0.0;
   node0->SetRadius(idx, Node->GetRadius(idxRep0));
   #ifdef __stDBMNENTRIES__
      node0->SetNEntries(idx, Node->GetNEntries(idxRep0));
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      node0->GetEntry(idx).Height = Node->GetHeight(idxRep0);
   #endif //__stDBMHEIGHT__

   // Add the second representative
   // Add the other representative
   idx = node1->AddEntry(Node->GetObject(idxRep1)->GetSerializedSize(),
                         Node->GetObject(idxRep1)->Serialize(),
                         Node->GetPageID(idxRep1));
   node1->GetEntry(idx).Distance = 0.0;
   node1->SetRadius(idx, Node->GetRadius(idxRep1));
   #ifdef __stDBMNENTRIES__
      node1->SetNEntries(idx, Node->GetNEntries(idxRep1));
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      node1->GetEntry(idx).Height = Node->GetHeight(idxRep1);
   #endif //__stDBMHEIGHT__
   
   // Distribute the others objects.
   for (i = 0; i < N; i++){
      if (!Node->IsRepresentative(i)){
         if (ObjectCluster[i] == Cluster0){
            idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize(),
                                  Node->GetPageID(i));
            if (idx >= 0){
               // Insertion Ok!
               node0->GetEntry(idx).Distance = DMat[i][idxRep0];
               node0->SetRadius(idx, Node->GetRadius(i));
               #ifdef __stDBMNENTRIES__
                  node0->SetNEntries(idx, Node->GetNEntries(i));
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  node0->GetEntry(idx).Height = Node->GetHeight(i);
               #endif //__stDBMHEIGHT__
            }else{
               // Oops! We must put it in other node
               idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize(),
                                     Node->GetPageID(i));
               node1->GetEntry(idx).Distance = DMat[i][idxRep1];
               node1->SetRadius(idx, Node->GetRadius(i));
               #ifdef __stDBMNENTRIES__
                  node1->SetNEntries(idx, Node->GetNEntries(i));
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  node1->GetEntry(idx).Height = Node->GetHeight(i);
               #endif //__stDBMHEIGHT__
            }//end if
         }else{
            idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                  Node->GetObject(i)->Serialize(),
                                  Node->GetPageID(i));
            if (idx >= 0){
               // Insertion Ok!
               node1->GetEntry(idx).Distance = DMat[i][idxRep1];
               node1->SetRadius(idx, Node->GetRadius(i));
               #ifdef __stDBMNENTRIES__
                  node1->SetNEntries(idx, Node->GetNEntries(i));
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  node1->GetEntry(idx).Height = Node->GetHeight(i);
               #endif //__stDBMHEIGHT__
            }else{
               // Oops! We must put it in other node
               idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize(),
                                     Node->GetPageID(i));
               node0->GetEntry(idx).Distance = DMat[i][idxRep0];
               node0->SetRadius(idx, Node->GetRadius(i));
               #ifdef __stDBMNENTRIES__
                  node0->SetNEntries(idx, Node->GetNEntries(i));
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  node0->GetEntry(idx).Height = Node->GetHeight(i);
               #endif //__stDBMHEIGHT__
            }//end if
         }//end if
      }//end if
   }//end for

   // Representatives
   promo[0].Rep = Node->BuyObject(idxRep0);
   promo[0].Radius = node0->GetMinimumRadius();
   promo[0].RootID = node0->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[0].NObjects = node0->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[0].Height = node0->GetHeight() + 1;
   #endif //__stDBMHEIGHT__

   promo[1].Rep = Node->BuyObject(idxRep1);
   promo[1].Radius = node1->GetMinimumRadius();
   promo[1].RootID = node1->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[1].NObjects = node1->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[1].Height = node1->GetHeight() + 1;
   #endif //__stDBMHEIGHT__

   return distanceCount;
}//end stDBMMSTSplitter::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMMSTSplitter<ObjectType, EvaluatorType>::Distribute(
            tDBMCollection * returnCollection,
            stDBMNode * node0, stDBMNode * node1,
            tSubtreeInfo * promo, EvaluatorType * metricEvaluator){
   stCount distanceCount;
   stDistance distanceReps, RadiusNode0, RadiusNode1;
   int idx, i, idxRep0, idxRep1;

   // Build Distance matrix
   distanceCount = BuildDistanceMatrix(metricEvaluator);

   //Perform MST
   this->PerformMST();

   idxRep0 = Node->GetRepresentativeIndex(0);
   idxRep1 = Node->GetRepresentativeIndex(1);

   // calculate the distance from representatives.
   distanceReps = DMat[idxRep0][idxRep1];

   // Add the first representative.
   idx = node0->AddEntry(Node->GetObject(idxRep0)->GetSerializedSize(),
                         Node->GetObject(idxRep0)->Serialize(),
                         Node->GetPageID(idxRep0));
   node0->GetEntry(idx).Distance = 0.0;
   node0->SetRadius(idx, Node->GetRadius(idxRep0));
   #ifdef __stDBMNENTRIES__
      node0->SetNEntries(idx, Node->GetNEntries(idxRep0));
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      node0->GetEntry(idx).Height = Node->GetHeight(idxRep0);
   #endif //__stDBMHEIGHT__

   // Add the second representative.
   idx = node1->AddEntry(Node->GetObject(idxRep1)->GetSerializedSize(),
                         Node->GetObject(idxRep1)->Serialize(),
                         Node->GetPageID(idxRep1));
   node1->GetEntry(idx).Distance = 0.0;
   node1->SetRadius(idx, Node->GetRadius(idxRep1));
   #ifdef __stDBMNENTRIES__
      node1->SetNEntries(idx, Node->GetNEntries(idxRep1));
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      node1->GetEntry(idx).Height = Node->GetHeight(idxRep1);
   #endif //__stDBMHEIGHT__

//??????????
   RadiusNode0 = RadiusNode1 = MAXDOUBLE;
   for (i = 0; i < N; i++){
      if (!Node->IsRepresentative(i)){
         if (RadiusNode0 < DMat[i][idxRep0] + Node->GetRadius(i)){
            RadiusNode0 = DMat[i][idxRep0] + Node->GetRadius(i);
         }//end if
         if (RadiusNode1 < DMat[i][idxRep1] + Node->GetRadius(i)){
            RadiusNode1 = DMat[i][idxRep1] + Node->GetRadius(i);
         }//end if
      }//end if
   }//end for /**/

   // Distribute the others objects.
   for (i = 0; i < N; i++){
      // If this is not a representative entry.
      if (!Node->IsRepresentative(i)){
         // Close the radius.
         if (node0->GetNumberOfEntries() + node1->GetNumberOfEntries() == 3){
            RadiusNode0 = node0->GetMinimumRadius();
            RadiusNode1 = node1->GetMinimumRadius();
         }//end if /**/
         if (ObjectCluster[i] == Cluster0){
            if ((DMat[i][idxRep0] + Node->GetRadius(i) <= RadiusNode0) ||
                (DMat[i][idxRep0] + Node->GetRadius(i) + RadiusNode1 <= distanceReps)){
               // Try to add this object in node0.
               idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize(),
                                     Node->GetPageID(i));
               if (idx >= 0){
                  // Insertion Ok!
                  node0->GetEntry(idx).Distance = DMat[i][idxRep0];
                  node0->SetRadius(idx, Node->GetRadius(i));
                  #ifdef __stDBMNENTRIES__
                     node0->SetNEntries(idx, Node->GetNEntries(i));
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     node0->GetEntry(idx).Height = Node->GetHeight(i);
                  #endif //__stDBMHEIGHT__
                  // update the radius.
                  if (RadiusNode0 < DMat[i][idxRep0] + Node->GetRadius(i)){
                     RadiusNode0 = DMat[i][idxRep0] + Node->GetRadius(i);
                  }//end if
               }else if ((DMat[i][idxRep1] + Node->GetRadius(i) <= RadiusNode1) ||
                         (DMat[i][idxRep1] + Node->GetRadius(i) + RadiusNode0 <= distanceReps)){
                  // Try to add this object in node0.
                  idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                        Node->GetObject(i)->Serialize(),
                                        Node->GetPageID(i));
                  if (idx >= 0){
                     // Insertion Ok!
                     node1->GetEntry(idx).Distance = DMat[i][idxRep1];
                     node1->SetRadius(idx, Node->GetRadius(i));
                     #ifdef __stDBMNENTRIES__
                        node1->SetNEntries(idx, Node->GetNEntries(i));
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        node1->GetEntry(idx).Height = Node->GetHeight(i);
                     #endif //__stDBMHEIGHT__
                     // update the radius.
                     if (RadiusNode1 < DMat[i][idxRep1] + Node->GetRadius(i)){
                        RadiusNode1 = DMat[i][idxRep1] + Node->GetRadius(i);
                     }//end if
                  }else{
                     // Let's put it in the returnCollection since it does
                     // not fit in the node 0
                     // Now the owner of this object is returnCollection.
                     returnCollection->AddEntry(Node->BuyObject(i),
                                       #ifdef __stDBMHEIGHT__
                                          Node->GetHeight(i),
                                       #endif //__stDBMHEIGHT__
                                       Node->GetPageID(i),
                                       #ifdef __stDBMNENTRIES__
                                          Node->GetNEntries(i),
                                       #endif //__stDBMNENTRIES__
                                       Node->GetRadius(i));
                  }//end if
               }else{
                  // Let's put it in the returnCollection since it does
                  // not fit in the node 0
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(Node->BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Node->GetHeight(i),
                                    #endif //__stDBMHEIGHT__
                                    Node->GetPageID(i),
                                    #ifdef __stDBMNENTRIES__
                                       Node->GetNEntries(i),
                                    #endif //__stDBMNENTRIES__
                                    Node->GetRadius(i));
               }//end if
            }else if ((DMat[i][idxRep1] + Node->GetRadius(i) <= RadiusNode1) ||
                      (DMat[i][idxRep1] + Node->GetRadius(i) + RadiusNode0 <= distanceReps)){
               // Try to add this object in node0.
               idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize(),
                                     Node->GetPageID(i));
               if (idx >= 0){
                  // Insertion Ok!
                  node1->GetEntry(idx).Distance = DMat[i][idxRep1];
                  node1->SetRadius(idx, Node->GetRadius(i));
                  #ifdef __stDBMNENTRIES__
                     node1->SetNEntries(idx, Node->GetNEntries(i));
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     node1->GetEntry(idx).Height = Node->GetHeight(i);
                  #endif //__stDBMHEIGHT__
                  // update the radius.
                  if (RadiusNode1 < DMat[i][idxRep1] + Node->GetRadius(i)){
                     RadiusNode1 = DMat[i][idxRep1] + Node->GetRadius(i);
                  }//end if
               }else if ((DMat[i][idxRep0] + Node->GetRadius(i) <= RadiusNode0) ||
                         (DMat[i][idxRep0] + Node->GetRadius(i) + RadiusNode1 <= distanceReps)){
                  // Try to add this object in node1.
                  idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                        Node->GetObject(i)->Serialize(),
                                        Node->GetPageID(i));
                  if (idx >= 0){
                     // Insertion Ok!
                     node0->GetEntry(idx).Distance = DMat[i][idxRep0];
                     node0->SetRadius(idx, Node->GetRadius(i));
                     #ifdef __stDBMNENTRIES__
                        node0->SetNEntries(idx, Node->GetNEntries(i));
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        node0->GetEntry(idx).Height = Node->GetHeight(i);
                     #endif //__stDBMHEIGHT__
                     if (RadiusNode0 < DMat[i][idxRep0] + Node->GetRadius(i)){
                        RadiusNode0 = DMat[i][idxRep0] + Node->GetRadius(i);
                     }//end if
                  }else{
                     // Let's put it in the returnCollection since it does
                     // not fit in the node 0
                     // Now the owner of this object is returnCollection.
                     returnCollection->AddEntry(Node->BuyObject(i),
                                       #ifdef __stDBMHEIGHT__
                                          Node->GetHeight(i),
                                       #endif //__stDBMHEIGHT__
                                       Node->GetPageID(i),
                                       #ifdef __stDBMNENTRIES__
                                          Node->GetNEntries(i),
                                       #endif //__stDBMNENTRIES__
                                       Node->GetRadius(i));
                  }//end if
               }else{
                  // Let's put it in the returnCollection since it does
                  // not fit in the node 0
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(Node->BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Node->GetHeight(i),
                                    #endif //__stDBMHEIGHT__
                                    Node->GetPageID(i),
                                    #ifdef __stDBMNENTRIES__
                                       Node->GetNEntries(i),
                                    #endif //__stDBMNENTRIES__
                                    Node->GetRadius(i));
               }//end if
            }else{
               // Let's put it in the returnCollection since it does
               // not fit in the node 1 and 2
               // Now the owner of this object is returnCollection.
               returnCollection->AddEntry(Node->BuyObject(i),
                                 #ifdef __stDBMHEIGHT__
                                    Node->GetHeight(i),
                                 #endif //__stDBMHEIGHT__
                                 Node->GetPageID(i),
                                 #ifdef __stDBMNENTRIES__
                                    Node->GetNEntries(i),
                                 #endif //__stDBMNENTRIES__
                                 Node->GetRadius(i));
            }//end if
         }else{
            if ((DMat[i][idxRep1] + Node->GetRadius(i) <= RadiusNode1) ||
                (DMat[i][idxRep1] + Node->GetRadius(i) + RadiusNode0 <= distanceReps)){
               // Try to add this object in node1.
               idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize(),
                                     Node->GetPageID(i));
               if (idx >= 0){
                  // Insertion Ok!
                  node1->GetEntry(idx).Distance = DMat[i][idxRep1];
                  node1->SetRadius(idx, Node->GetRadius(i));
                  #ifdef __stDBMNENTRIES__
                     node1->SetNEntries(idx, Node->GetNEntries(i));
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     node1->GetEntry(idx).Height = Node->GetHeight(i);
                  #endif //__stDBMHEIGHT__
                  if (RadiusNode1 < DMat[i][idxRep1] + Node->GetRadius(i)){
                     RadiusNode1 = DMat[i][idxRep1] + Node->GetRadius(i);
                  }//end if
               }else if ((DMat[i][idxRep0] + Node->GetRadius(i) <= RadiusNode0) ||
                         (DMat[i][idxRep0] + Node->GetRadius(i) + RadiusNode1 <= distanceReps)){
                  // Try to add this object in node1.
                  idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                        Node->GetObject(i)->Serialize(),
                                        Node->GetPageID(i));
                  if (idx >= 0){
                     // Insertion Ok!
                     node0->GetEntry(idx).Distance = DMat[i][idxRep0];
                     node0->SetRadius(idx, Node->GetRadius(i));
                     #ifdef __stDBMNENTRIES__
                        node0->SetNEntries(idx, Node->GetNEntries(i));
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        node0->GetEntry(idx).Height = Node->GetHeight(i);
                     #endif //__stDBMHEIGHT__
                     if (RadiusNode0 < DMat[i][idxRep0] + Node->GetRadius(i)){
                        RadiusNode0 = DMat[i][idxRep0] + Node->GetRadius(i);
                     }//end if
                  }else{
                     // Let's put it in the returnCollection since it does
                     // not fit in the node 1
                     // Now the owner of this object is returnCollection.
                     returnCollection->AddEntry(Node->BuyObject(i),
                                       #ifdef __stDBMHEIGHT__
                                          Node->GetHeight(i),
                                       #endif //__stDBMHEIGHT__
                                       Node->GetPageID(i),
                                       #ifdef __stDBMNENTRIES__
                                          Node->GetNEntries(i),
                                       #endif //__stDBMNENTRIES__
                                       Node->GetRadius(i));
                  }//end if
               }else{
                  // Let's put it in the returnCollection since it does
                  // not fit in the node 0
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(Node->BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Node->GetHeight(i),
                                    #endif //__stDBMHEIGHT__
                                    Node->GetPageID(i),
                                    #ifdef __stDBMNENTRIES__
                                       Node->GetNEntries(i),
                                    #endif //__stDBMNENTRIES__
                                    Node->GetRadius(i));
               }//end if
            }else if ((DMat[i][idxRep0] + Node->GetRadius(i) <= RadiusNode0) ||
                      (DMat[i][idxRep0] + Node->GetRadius(i) + RadiusNode1 <= distanceReps)){
               // Try to add this object in node1.
               idx = node0->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                     Node->GetObject(i)->Serialize(),
                                     Node->GetPageID(i));
               if (idx >= 0){
                  // Insertion Ok!
                  node0->GetEntry(idx).Distance = DMat[i][idxRep0];
                  node0->SetRadius(idx, Node->GetRadius(i));
                  #ifdef __stDBMNENTRIES__
                     node0->SetNEntries(idx, Node->GetNEntries(i));
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     node0->GetEntry(idx).Height = Node->GetHeight(i);
                  #endif //__stDBMHEIGHT__
                  if (RadiusNode0 < DMat[i][idxRep0] + Node->GetRadius(i)){
                     RadiusNode0 = DMat[i][idxRep0] + Node->GetRadius(i);
                  }//end if
               }else if ((DMat[i][idxRep1] + Node->GetRadius(i) <= RadiusNode1) ||
                         (DMat[i][idxRep1] + Node->GetRadius(i) + RadiusNode0 <= distanceReps)){
                  // Try to add this object in node1.
                  idx = node1->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                        Node->GetObject(i)->Serialize(),
                                        Node->GetPageID(i));
                  if (idx >= 0){
                     // Insertion Ok!
                     node1->GetEntry(idx).Distance = DMat[i][idxRep1];
                     node1->SetRadius(idx, Node->GetRadius(i));
                     #ifdef __stDBMNENTRIES__
                        node1->SetNEntries(idx, Node->GetNEntries(i));
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        node1->GetEntry(idx).Height = Node->GetHeight(i);
                     #endif //__stDBMHEIGHT__
                     if (RadiusNode1 < DMat[i][idxRep1] + Node->GetRadius(i)){
                        RadiusNode1 = DMat[i][idxRep1] + Node->GetRadius(i);
                     }//end if
                  }else{
                     // Let's put it in the returnCollection since it does
                     // not fit in the node 1
                     // Now the owner of this object is returnCollection.
                     returnCollection->AddEntry(Node->BuyObject(i),
                                       #ifdef __stDBMHEIGHT__
                                          Node->GetHeight(i),
                                       #endif //__stDBMHEIGHT__
                                       Node->GetPageID(i),
                                       #ifdef __stDBMNENTRIES__
                                          Node->GetNEntries(i),
                                       #endif //__stDBMNENTRIES__
                                       Node->GetRadius(i));
                  }//end if
               }else{
                  // Let's put it in the returnCollection since it does
                  // not fit in the node 0
                  // Now the owner of this object is returnCollection.
                  returnCollection->AddEntry(Node->BuyObject(i),
                                    #ifdef __stDBMHEIGHT__
                                       Node->GetHeight(i),
                                    #endif //__stDBMHEIGHT__
                                    Node->GetPageID(i),
                                    #ifdef __stDBMNENTRIES__
                                       Node->GetNEntries(i),
                                    #endif //__stDBMNENTRIES__
                                    Node->GetRadius(i));
               }//end if
            }else{
               // Let's put it in the returnCollection since it does
               // not fit in the node 1 and 2
               // Now the owner of this object is returnCollection.
               returnCollection->AddEntry(Node->BuyObject(i),
                                 #ifdef __stDBMHEIGHT__
                                    Node->GetHeight(i),
                                 #endif //__stDBMHEIGHT__
                                 Node->GetPageID(i),
                                 #ifdef __stDBMNENTRIES__
                                    Node->GetNEntries(i),
                                 #endif //__stDBMNENTRIES__
                                 Node->GetRadius(i));
            }//end if
         }//end if
      }//end if
   }//end for

   // Representatives
   promo[0].Rep = Node->BuyObject(idxRep0);
   promo[0].Radius = node0->GetMinimumRadius();
   promo[0].RootID = node0->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[0].NObjects = node0->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[0].Height = node0->GetHeight() + 1;
   #endif //__stDBMHEIGHT__

   promo[1].Rep = Node->BuyObject(idxRep1);
   promo[1].Radius = node1->GetMinimumRadius();
   promo[1].RootID = node1->GetPageID();
   #ifdef __stDBMNENTRIES__
      promo[1].NObjects = node1->GetTotalObjectCount();
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      promo[1].Height = node1->GetHeight() + 1;
   #endif //__stDBMHEIGHT__

   return distanceCount;
}//end stDBMMSTSplitter::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMMSTSplitter<ObjectType, EvaluatorType>::JoinClusters(
      int cluster1, int cluster2){
   int i;

   for (i = 0 ; i < N; i++){
      if (ObjectCluster[i] == cluster2){
         ObjectCluster[i] = cluster1;
      }//end if
   }//end for
   Cluster[cluster1].Size += Cluster[cluster2].Size;
   Cluster[cluster2].State = DEATH_SENTENCE;
}//end stDBMMSTSplitter::JoinClusters


























//=============================================================================
// Class template stDBMNSpliter
//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDBMNSplitter<ObjectType, EvaluatorType>::stDBMNSplitter(
      tLogicNode * node, int numberOfClusters, stPageManager * pageman){

   Node = node;
   N = Node->GetNumberOfEntries();
   NumberOfClusters = numberOfClusters;
   this->myPageManager = pageman;

   // Dynamic fields
   Cluster = new tCluster[N];
   ObjectCluster = new int[N];
   ClusterNodes = new int[N];
   idxRep = new stCount[numberOfClusters];
   radiusRep = new stDistance[numberOfClusters];
   ClusterNames = new int[numberOfClusters];

   // Matrix
   DMat.SetSize(N, N);
}//end stDBMNSplitter::stDBMNSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDBMNSplitter<ObjectType, EvaluatorType>::~stDBMNSplitter(){

   if (Cluster != NULL){
      delete[] Cluster;
   }//end if
   if (ObjectCluster != NULL){
      delete[] ObjectCluster;
   }//end if
   if (idxRep != NULL){
      delete[] idxRep;
   }//end if
   if (radiusRep != NULL){
      delete[] radiusRep;
   }//end if
   if (ClusterNames != NULL){
      delete[] ClusterNames;
   }//end if
   if (ClusterNodes != NULL){
      delete[] ClusterNodes;
   }//end if
}//end stDBMNSplitter::~stDBMNSplitter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMNSplitter<ObjectType, EvaluatorType>::BuildDistanceMatrix(
      EvaluatorType * metricEvaluator){
   int i, j;

   for (i = 0; i < N; i++){
      DMat[i][i] = 0;
      for (j = 0; j < i; j++){
         DMat[i][j] = metricEvaluator->GetDistance(Node->GetObject(i),
                                                   Node->GetObject(j));
         DMat[j][i] = DMat[i][j];
      }//end for
   }//end for
   return ((N - 1) * N) / 2;
}//end stDBMNSplitter::BuildDistanceMatrix

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMNSplitter<ObjectType, EvaluatorType>::PerformCluster(){
   int i, j, k, w, cc;
   stCount idx, finalNumberOfClusters;
   stDistance finalRadius;
   bool flag;

   // Insert each object in its own cluster.
   cc = N;
   for (i = 0; i < N; i++){
      Cluster[i].Size = 1;
      Cluster[i].State = ALIVE;
      ObjectCluster[i] = i; // Add Object
   }//end for

   // Perform it until it reaches totalNumberOfClusters clusters.
   while (cc > NumberOfClusters){
      // Find the minimum distance between a cluster and its nearest
      // neighbour (connections).
      for (i = 0; i < N; i++){
         if (Cluster[i].State != DEAD){
            Cluster[i].MinDist = MAXDOUBLE;
         }//end if
      }//end for
      for (i = 0; i < N; i++){
         k = ObjectCluster[i];  // Pegar o nome do cluster.
         // Locate the nearest
         for (j = 0; j < N; j++){
            if (ObjectCluster[j] != k){ // Se o objeto nao pertence ao cluster anterior.
               if (Cluster[k].MinDist > DMat[i][j]){  // Pega o cluster mais perto do cluster em questao.
                  Cluster[k].MinDist = DMat[i][j];
                  Cluster[k].Src = i;       // Entrada i do cluster k
                  Cluster[k].Dst = j;
               }//end if
            }//end if
         }//end for
      }//end for

      // Join clusters
      i = 0;
      while ((i < N) && (cc > NumberOfClusters)) {
         if (Cluster[i].State != DEAD){
            // Join cluster i and its nearest cluster.
            k = ObjectCluster[Cluster[i].Dst];
            // Change the cluster of all objects of the dropped one to
            // the remaining one.
            flag = true;
            for (j = 0; j < N; j++){
               if ((ObjectCluster[j] == k) &&
                   (ObjectCluster[j] != ObjectCluster[Cluster[i].Src])){

                  Cluster[k].State = DEATH_SENTENCE;
                  ObjectCluster[j] = ObjectCluster[Cluster[i].Src];
                  if (flag){
                     Cluster[ObjectCluster[Cluster[i].Src]].Size += Cluster[k].Size;
                     flag = false;
                     cc--;
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

   // Locate the name of the N clusters.
   for (i = 0; i < NumberOfClusters; i++){
      ClusterNames[i] = -1;
   }//end for

   // Name the clusters and set the respective nodes.
   idx = 0;
   for (i = 0; i < N; i++){
      if (Cluster[i].State == ALIVE){
         if (ClusterNames[idx] == -1){
            if (Cluster[i].Size == 1){
               // This cluster has only one entry.
               ObjectCluster[i] = -1;
            }else{
               ClusterNames[idx] = i;
               idx++;
            }//end if
         }//end if
      }//end if
   }//end for

   finalNumberOfClusters = idx;

   // Find the center of each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      idxRep[i] = FindCenter(ClusterNames[i], radiusRep[i]);
   }//end for

   // Sets the nodes to each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      for (j = 0; j < N; j++){
         if (ClusterNames[i] == ObjectCluster[j]){
            ClusterNodes[j] = i;
         }//end if
      }//end for
   }//end for

   // Update the number of clusters.
   NumberOfClusters = finalNumberOfClusters;

   return finalNumberOfClusters;
}//end stDBMNSplitter::PerformCluster

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMNSplitter<ObjectType, EvaluatorType>::PerformClusterRadius(){
   int i, j, k, w, cc;
   stCount idx, finalNumberOfClusters;
   stDistance finalRadius;
   bool flag;

   // Insert each object in its own cluster.
   cc = N;
   for (i = 0; i < N; i++){
      Cluster[i].Size = 1;
      Cluster[i].State = ALIVE;
      ObjectCluster[i] = i; // Add Object
   }//end for

   // Perform it until it reaches totalNumberOfClusters clusters.
   while (cc > NumberOfClusters){
      // Find the minimum distance between a cluster and its nearest
      // neighbour (connections).
      for (i = 0; i < N; i++){
         if (Cluster[i].State != DEAD){
            Cluster[i].MinDist = MAXDOUBLE;
         }//end if
      }//end for
      for (i = 0; i < N; i++){
         k = ObjectCluster[i];  // Pegar o nome do cluster.
         // Locate the nearest
         for (j = 0; j < N; j++){
            if (ObjectCluster[j] != k){ // Se o objeto nao pertence ao cluster anterior.
               if (Cluster[k].MinDist > DMat[i][j] + Node->GetRadius(j)){  // Pega o cluster mais perto do cluster em questao.
                  Cluster[k].MinDist = DMat[i][j] + Node->GetRadius(j);
                  Cluster[k].Src = i;       // Entrada i do cluster k
                  Cluster[k].Dst = j;
               }//end if
            }//end if
         }//end for
      }//end for

      // Join clusters
      i = 0;
      while ((i < N) && (cc > NumberOfClusters)) {
         if (Cluster[i].State != DEAD){
            // Join cluster i and its nearest cluster.
            k = ObjectCluster[Cluster[i].Dst];
            // Change the cluster of all objects of the dropped one to
            // the remaining one.
            flag = true;
            for (j = 0; j < N; j++){
               if ((ObjectCluster[j] == k) &&
                   (ObjectCluster[j] != ObjectCluster[Cluster[i].Src])){

                  Cluster[k].State = DEATH_SENTENCE;
                  ObjectCluster[j] = ObjectCluster[Cluster[i].Src];
                  if (flag){
                     Cluster[ObjectCluster[Cluster[i].Src]].Size += Cluster[k].Size;
                     flag = false;
                     cc--;
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

   // Locate the name of the N clusters.
   for (i = 0; i < NumberOfClusters; i++){
      ClusterNames[i] = -1;
   }//end for

   // Name the clusters and set the respective nodes.
   idx = 0;
   for (i = 0; i < N; i++){
      if (Cluster[i].State == ALIVE){
         if (ClusterNames[idx] == -1){
            if (Cluster[i].Size == 1){
               // This cluster has only one entry.
               ObjectCluster[i] = -1;
            }else{
               ClusterNames[idx] = i;
               idx++;
            }//end if
         }//end if
      }//end if
   }//end for

   finalNumberOfClusters = idx;

   // Find the center of each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      idxRep[i] = FindCenter(ClusterNames[i], radiusRep[i]);
   }//end for

   // Sets the nodes to each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      for (j = 0; j < N; j++){
         if (ClusterNames[i] == ObjectCluster[j]){
            ClusterNodes[j] = i;
         }//end if
      }//end for
   }//end for

   // Update the number of clusters.
   NumberOfClusters = finalNumberOfClusters;

   return finalNumberOfClusters;
}//end stDBMNSplitter::PerformClusterRadius

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMNSplitter<ObjectType, EvaluatorType>::PerformClusterMerge(){
   int i, j, k, w, cc;
   stCount idx, finalNumberOfClusters;
   stDistance finalRadius;
   bool flag;

   // Insert each object in its own cluster.
   cc = N;
   for (i = 0; i < N; i++){
      Cluster[i].Size = 1;
      Cluster[i].State = ALIVE;
      Cluster[i].Radius = Node->GetRadius(i);
      Cluster[i].Center = i;
      ObjectCluster[i] = i; // Add Object
   }//end for

   // Perform it until it reaches totalNumberOfClusters clusters.
   while (cc > NumberOfClusters){
      // Find the minimum distance between a cluster and its nearest
      // neighbour (connections).
      for (i = 0; i < N; i++){
         if (Cluster[i].State != DEAD){
            Cluster[i].MinDist = MAXDOUBLE;
         }//end if
      }//end for
      for (i = 0; i < N; i++){
         k = ObjectCluster[i];  // Pegar o nome do cluster.
         // Locate the nearest
         for (j = 0; j < N; j++){
            if (ObjectCluster[j] != k){ // Se o objeto nao pertence ao cluster anterior.
               if (Cluster[k].MinDist > DMat[i][j]){  // Pega o cluster mais perto do cluster em questao.
                  Cluster[k].MinDist = DMat[i][j];
                  Cluster[k].Src = i;       // Entrada i do cluster k
                  Cluster[k].Dst = j;
               }//end if
            }//end if
         }//end for
      }//end for

      // Join clusters
      i = 0;
      while ((i < N) && (cc > NumberOfClusters)) {
         if (Cluster[i].State != DEAD){
            // Join cluster i and its nearest cluster.
            k = ObjectCluster[Cluster[i].Dst];
            // Change the cluster of all objects of the dropped one to
            // the remaining one.
            flag = true;
            for (j = 0; j < N; j++){
               if ((ObjectCluster[j] == k) &&
                   (ObjectCluster[j] != ObjectCluster[Cluster[i].Src])){

                  Cluster[k].State = DEATH_SENTENCE;
                  ObjectCluster[j] = ObjectCluster[Cluster[i].Src];
                  if (flag){
                     Cluster[ObjectCluster[Cluster[i].Src]].Size += Cluster[k].Size;
                     flag = false;
                     cc--;
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

///////////////////////////////////////////////////////////////////////////////
// inicio da nova parte
///////////////////////////////////////////////////////////////////////////////
   finalNumberOfClusters = NumberOfClusters;
   // Tentar juntar outros clustes desde que nao aumente a sobreposicao
   // Find the center of each cluster.
   for (i = 0; i < N; i++){
      if (Cluster[i].State == ALIVE){
         Cluster[i].Center = FindCenter(ObjectCluster[i], Cluster[i].Radius);
         Cluster[i].MinDist = MAXDOUBLE;
      }//end if
   }//end for

   for (i = 0; i < N; i++){
      k = ObjectCluster[i];  // Pegar o nome do cluster.
      // Locate the nearest
      for (j = 0; j < N; j++){
         if (ObjectCluster[j] != k){ // Se o objeto nao pertence ao cluster anterior.
            if (Cluster[k].MinDist > DMat[i][j]){  // Pega o cluster mais perto do cluster em questao.
               Cluster[k].MinDist = DMat[i][j];
               Cluster[k].Src = i;       // Entrada i do cluster k
               Cluster[k].Dst = j;
            }//end if
         }//end if
      }//end for
   }//end for

   // Try to join clusters.
   idx = 0;
   for (i = 0; i < N; i++){
      if (Cluster[i].State != DEAD){
         // Try to join cluster i and its nearest cluster.
         k = ObjectCluster[Cluster[i].Dst];
         // Change the cluster of all objects of the dropped one to
         // the remaining one.
         for (j = 0; j < N; j++){
            if ((ObjectCluster[j] == k) &&
                (ObjectCluster[j] != ObjectCluster[Cluster[i].Src])){
               // test if the radius increases.
               finalRadius = TestJoinClusters(ObjectCluster[Cluster[i].Src], k);
               // test if the radius increases.
               if (finalRadius == radiusRep[idx]){
                  // No. It is possible to add j in i.
                  JoinClusters(ObjectCluster[Cluster[i].Src], k);
                  finalNumberOfClusters--;
               }else{
                  if (TestIntersection(finalRadius, ObjectCluster[Cluster[i].Src], k)){
                     JoinClusters(ObjectCluster[Cluster[i].Src], k);
                     //idxRep[i] = FindCenter(ClusterNames[i], radiusRep[i]);
                     //idxRep[i2] = ??;
                     finalNumberOfClusters--;
                  }//end if /**/
               }//end if
            }//end if
         }//end for
      }//end if
      idx++;
      // All clusters that are destiny of an edge has gone, integrated
      // into another one.
      for (i = 0; i < N; i++){
         if (Cluster[i].State == DEATH_SENTENCE){
            Cluster[i].State = DEAD;
         }//end if
      }//end for
   }//end for

///////////////////////////////////////////////////////////////////////////////
// fim da nova parte
///////////////////////////////////////////////////////////////////////////////

   // Locate the name of the N clusters.
   finalNumberOfClusters = NumberOfClusters;
   for (i = 0; i < finalNumberOfClusters; i++){
      ClusterNames[i] = -1;
   }//end for

   // Por os clusters unitarios no returnCollection.
   // Name the clusters.
   idx = 0;
   for (i = 0; i < N; i++){
      if (Cluster[i].State == ALIVE){
         if (ClusterNames[idx] == -1){
            if (Cluster[i].Size == 1){
               ObjectCluster[i] = -1;
            }else{
               ClusterNames[idx] = i;
               idx++;
            }//end if
         }//end if
      }//end if
   }//end for

   finalNumberOfClusters = idx;

   // Find the center of each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      idxRep[i] = FindCenter(ClusterNames[i], radiusRep[i]);
   }//end for

   // Sets the nodes to each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      for (j = 0; j < N; j++){
         if (ClusterNames[i] == ObjectCluster[j]){
            ClusterNodes[j] = i;
         }//end if
      }//end for
   }//end for

   // Update the number of clusters.
   NumberOfClusters = finalNumberOfClusters;

   return finalNumberOfClusters;
}//end stDBMNSplitter::PerformClusterMerge

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMNSplitter<ObjectType, EvaluatorType>::PerformClusterRadiusTest(){
   int i, j, k, w, cc;
   stCount idx, finalNumberOfClusters;
   stDistance finalRadius;
   bool flag;

   // Insert each object in its own cluster.
   cc = N;
   for (i = 0; i < N; i++){
      Cluster[i].Size = 1;
      Cluster[i].State = ALIVE;
      Cluster[i].Radius = Node->GetRadius(i);
      Cluster[i].Center = i;
      ObjectCluster[i] = i; // Add Object
   }//end for

   // Perform it until it reaches totalNumberOfClusters clusters.
   while (cc > NumberOfClusters){
      // Find the minimum distance between a cluster and its nearest
      // neighbour (connections).
      for (i = 0; i < N; i++){
         if (Cluster[i].State != DEAD){
            Cluster[i].MinDist = MAXDOUBLE;
         }//end if
      }//end for
      for (i = 0; i < N; i++){
         k = ObjectCluster[i];  // Pegar o nome do cluster.
         // Locate the nearest
         for (j = 0; j < N; j++){
            if (ObjectCluster[j] != k){ // Se o objeto nao pertence ao cluster anterior.
               // Pega o cluster mais perto do cluster em questao.
               if (Cluster[k].MinDist > DMat[i][ObjectCluster[Cluster[j].Center]] + Node->GetRadius(j)){
                  Cluster[k].MinDist = DMat[i][ObjectCluster[Cluster[j].Center]] + Node->GetRadius(j);
                  Cluster[k].Src = i;       // Entrada i do cluster k
                  Cluster[k].Dst = ObjectCluster[Cluster[j].Center];
               }//end if
            }//end if
         }//end for
      }//end for

      // Join clusters
      i = 0;
      while ((i < N) && (cc > NumberOfClusters)) {
         if (Cluster[i].State != DEAD){
            // Join cluster i and its nearest cluster.
            k = ObjectCluster[Cluster[i].Dst];
            // Change the cluster of all objects of the dropped one to
            // the remaining one.
            flag = true;
            for (j = 0; j < N; j++){
               if ((ObjectCluster[j] == k) &&
                   (ObjectCluster[j] != ObjectCluster[Cluster[i].Src])){

                  Cluster[k].State = DEATH_SENTENCE;
                  ObjectCluster[j] = ObjectCluster[Cluster[i].Src];
                  if (flag){
                     Cluster[ObjectCluster[Cluster[i].Src]].Size += Cluster[k].Size;
                     flag = false;
                     cc--;
                  }//end if
               }//end if
            }//end for
            Cluster[i].Center = FindCenter(ObjectCluster[Cluster[i].Src], Cluster[i].Radius);
            for (j = 0; j < N; j++){
               if (ObjectCluster[j] == ObjectCluster[Cluster[i].Src]){
                  Cluster[j].Center = Cluster[i].Center;
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

   // Locate the name of the N clusters.
   for (i = 0; i < NumberOfClusters; i++){
      ClusterNames[i] = -1;
   }//end for

   // Por os clusters unitarios no returnCollection.
   // Name the clusters.
   idx = 0;
   for (i = 0; i < N; i++){
      if (Cluster[i].State == ALIVE){
         if (ClusterNames[idx] == -1){
            if (Cluster[i].Size == 1){
               ObjectCluster[i] = -1;
            }else{
               ClusterNames[idx] = i;
               idx++;
            }//end if
         }//end if
      }//end if
   }//end for

   finalNumberOfClusters = idx;

   // Find the center of each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      idxRep[i] = FindCenter(ClusterNames[i], radiusRep[i]);
   }//end for

   // Sets the nodes to each cluster.
   for (i = 0; i < finalNumberOfClusters; i++){
      for (j = 0; j < N; j++){
         if (ClusterNames[i] == ObjectCluster[j]){
            ClusterNodes[j] = i;
         }//end if
      }//end for
   }//end for

   // Update the number of clusters.
   NumberOfClusters = finalNumberOfClusters;

   return finalNumberOfClusters;    
}//end stDBMNSplitter::PerformClusterRadiusTest

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMNSplitter<ObjectType, EvaluatorType>::Distribute(
            stDBMNode * oldNode,
            tDBMCollection * returnCollection,
            tSubtreeInfo * promo, EvaluatorType * metricEvaluator){
   stPage * tmpPage;
   stDBMNode ** nodes;
   stCount finalNumberOfClusters, idxNode;
   int idx, i, insertIdx;

   // Build Distance matrix
   BuildDistanceMatrix(metricEvaluator);

   // Perform Cluster
   finalNumberOfClusters = this->PerformClusterRadius();

   nodes = new stDBMNode * [finalNumberOfClusters];
   // Use the oldNode.
   nodes[0] = oldNode;
   for (idx = 1; idx < finalNumberOfClusters; idx++){
      tmpPage = this->myPageManager->GetNewPage();
      nodes[idx] = new stDBMNode(tmpPage, true);
   }//end for

   // Add all the representatives.
   for (idx = 0; idx < finalNumberOfClusters; idx++){
      // Add the first representative.
      idxNode = ClusterNodes[idxRep[idx]];
      insertIdx = nodes[idxNode]->AddEntry(Node->GetObject(idxRep[idx])->GetSerializedSize(),
                                           Node->GetObject(idxRep[idx])->Serialize(),
                                           Node->GetPageID(idxRep[idx]));
      nodes[idxNode]->GetEntry(insertIdx).Distance = 0.0;
      nodes[idxNode]->SetRadius(insertIdx, Node->GetRadius(idxRep[idx]));
      #ifdef __stDBMNENTRIES__
         nodes[idxNode]->SetNEntries(insertIdx, Node->GetNEntries(idxRep[idx]));
      #endif //__stDBMNENTRIES__
      #ifdef __stDBMHEIGHT__
         nodes[idxNode]->GetEntry(insertIdx).Height = Node->GetHeight(idxRep[idx]);
      #endif //__stDBMHEIGHT__
   }//end for

   // Distribute the others objects.
   for (i = 0; i < N; i++){
      // If this is not a representative entry.
      if (!IsRepresentative(i)){
         idx = ObjectCluster[i];
         if (idx >= 0){
            idxNode = ClusterNodes[i];
            // Try to add this object in node0.
            insertIdx = nodes[idxNode]->AddEntry(Node->GetObject(i)->GetSerializedSize(),
                                                 Node->GetObject(i)->Serialize(),
                                                 Node->GetPageID(i));
            if (insertIdx >= 0){
               // Insertion Ok!
               nodes[idxNode]->GetEntry(insertIdx).Distance = DMat[i][idxRep[idxNode]];
               nodes[idxNode]->SetRadius(insertIdx, Node->GetRadius(i));
               #ifdef __stDBMNENTRIES__
                  nodes[idxNode]->SetNEntries(insertIdx, Node->GetNEntries(i));
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  nodes[idxNode]->GetEntry(insertIdx).Height = Node->GetHeight(i);
               #endif //__stDBMHEIGHT__
            }else{
               returnCollection->AddEntry(Node->BuyObject(i),
                                 #ifdef __stDBMHEIGHT__
                                    Node->GetHeight(i),
                                 #endif //__stDBMHEIGHT__
                                 Node->GetPageID(i),
                                 #ifdef __stDBMNENTRIES__
                                    Node->GetNEntries(i),
                                 #endif //__stDBMNENTRIES__
                                 Node->GetRadius(i));
            }//end if
         }else{
            returnCollection->AddEntry(Node->BuyObject(i),
                              #ifdef __stDBMHEIGHT__
                                 Node->GetHeight(i),
                              #endif //__stDBMHEIGHT__
                              Node->GetPageID(i),
                              #ifdef __stDBMNENTRIES__
                                 Node->GetNEntries(i),
                              #endif //__stDBMNENTRIES__
                              Node->GetRadius(i)); /**/
         }//end if
      }//end if
   }//end for

   // Representatives
   for (idx = 0; idx < finalNumberOfClusters; idx++){
      promo[idx].Rep = Node->BuyObject(idxRep[idx]);
      promo[idx].Radius = nodes[idx]->GetMinimumRadius();
      promo[idx].RootID = nodes[idx]->GetPageID();
      #ifdef __stDBMNENTRIES__
         promo[idx].NObjects = nodes[idx]->GetTotalObjectCount();
      #endif //__stDBMNENTRIES__
      #ifdef __stDBMHEIGHT__
         promo[idx].Height = nodes[idx]->GetHeight() + 1;
      #endif //__stDBMHEIGHT__
   }//end for

   for (idx = 1; idx < finalNumberOfClusters; idx++){
      // Write the new node
      tmpPage = nodes[idx]->GetPage();
      this->myPageManager->WritePage(tmpPage);
      delete nodes[idx];
      this->myPageManager->ReleasePage(tmpPage);
   }//end for
   delete[] nodes;

   return finalNumberOfClusters - 1;
}//end stDBMNSplitter::Distribute

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDBMNSplitter<ObjectType, EvaluatorType>::IsRepresentative(int idx){
   int i;
   bool ret = false;

   for (i = 0; i < NumberOfClusters; i++){
      if (idxRep[i] == idx){
         ret = true;
      }//end if
   }//end for

   return ret;
}//end stDBMNSplitter::IsRepresentative

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDBMNSplitter<ObjectType, EvaluatorType>::FindCenter(int clus,
         stDistance & minRadius){
   int i, j, center;
   stDistance radius;

   minRadius = MAXDOUBLE;
   for (i = 0; i < N; i++){
      if (ObjectCluster[i] == clus){
         radius = -1;
         for (j = 0; j < N; j++){
            if ((ObjectCluster[j] == clus) && (radius < DMat[i][j] + Node->GetRadius(j))){
               radius = DMat[i][j] + Node->GetRadius(j);
            }//end if
         }//end for
         if (minRadius > radius){
            minRadius = radius;
            center = i;
         }//end if
      }//end if
   }//end for

   return center;
}//end stDBMNSplitter::FindCenter

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMNSplitter<ObjectType, EvaluatorType>::JoinClusters(
      int cluster1, int cluster2){
   int i;

   for (i = 0; i < N; i++){
      if (ObjectCluster[i] == cluster2){
         ObjectCluster[i] = cluster1;
      }//end if
   }//end for
   Cluster[cluster1].Size += Cluster[cluster2].Size;
   Cluster[cluster2].State = DEATH_SENTENCE;
}//end stDBMNSplitter::JoinClusters

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance stDBMNSplitter<ObjectType, EvaluatorType>::TestJoinClusters(
      int cluster1, int cluster2){
   int size, idx;
   int * TempObjectCluster;
   int i, j, center;
   stDistance radius, minRadius;

   size = Cluster[cluster1].Size + Cluster[cluster2].Size;
   TempObjectCluster = new int [size];

   idx = 0;
   for (i = 0; i < N; i++){
      if ((ObjectCluster[i] == cluster2) || (ObjectCluster[i] == cluster1)){
         TempObjectCluster[idx] = ObjectCluster[i];
         idx++;
      }//end if
   }//end for

   // Find the minimum radius.
   minRadius = MAXDOUBLE;
   for (i = 0; i < size; i++){
      radius = -1;
      for (j = 0; j < size; j++){
         if (radius < DMat[i][j] + Node->GetRadius(j)){
            radius = DMat[i][j] + Node->GetRadius(j);
         }//end if
      }//end for
      if (minRadius > radius){
         minRadius = radius;
      }//end if
   }//end for

   // return the minimum radius.
   return minRadius;
}//end stDBMNSplitter::TestJoinClusters

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDBMNSplitter<ObjectType, EvaluatorType>::TestIntersection(
      stDistance finalRadius, int cluster1, int cluster2){
   int size, w, idx;
   bool flag = false;
   int * TempObjectCluster;
   int i, j, center;
   stDistance radius, minRadius;

   size = Cluster[cluster1].Size + Cluster[cluster2].Size;
   TempObjectCluster = new int [size];

   idx = 0;
   for (i = 0; i < N; i++){
      if ((ObjectCluster[i] == cluster2) || (ObjectCluster[i] == cluster1)){
         TempObjectCluster[idx] = ObjectCluster[i];
         idx++;
      }//end if
   }//end for

   // Find the minimum radius.
   minRadius = MAXDOUBLE;
   for (i = 0; i < size; i++){
      radius = -1;
      for (j = 0; j < size; j++){
         if (radius < DMat[i][j] + Node->GetRadius(j)){
            radius = DMat[i][j] + Node->GetRadius(j);
         }//end if
      }//end for
      if (minRadius > radius){
         minRadius = radius;
      }//end if
   }//end for

   // return the minimum radius.
   return flag;
}//end stDBMNSplitter::TestIntersection



















//==============================================================================
// Class stDBMTree
//==============================================================================

template <class ObjectType, class EvaluatorType>
stDBMTree<ObjectType, EvaluatorType>::stDBMTree(stPageManager * pageman):
   stMetricTree<ObjectType, EvaluatorType>(pageman){

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
      MAMViewer = new tViewExtractor(myMetricEvaluator);
   #endif //__stMAMVIEW__

   // Allocate the colletions.
   this->returnCollection = new tDBMCollection();
   if (this->IsReInsert()){
      this->reInsertCollection = new tDBMCollection();
   }else{
      this->reInsertCollection = NULL; 
   }//end if

}//end stDBMTree::stDBMTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDBMTree<ObjectType, EvaluatorType>::~stDBMTree(){

   // Flush header page.
   FlushHeader();

   // Visualization support
   #ifdef __stMAMVIEW__
      delete MAMViewer;
   #endif //__stMAMVIEW__

   // Clean the collections.
   delete this->returnCollection;
   if (this->IsReInsert()){
      delete this->reInsertCollection;
   }//end if
}//end stDBMTree::~stDBMTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::DefaultHeader(){

   // Clear header page.
   HeaderPage->Clear();

   // Default values
   Header->Magic[0] = 'D';
   Header->Magic[1] = 'B';
   Header->Magic[2] = 'M';
   Header->Magic[3] = '-';
   Header->SplitMethod = smMIN_MAX;
   Header->ChooseMethod = cmMINDISTWITHCOVER;
   // reinsert objects in this tree.
   this->SetReInsert(false);
   this->SetRemoveFarthest(false);

   Header->Root = 0;
   Header->MinOccupation = 0.25;
   Header->Height = 0;
   Header->ObjectCount = 0;
   Header->TreeRadius = 0.0;
   Header->idxRoot = -1;
   Header->NumberOfClusters = 2;

   // Notify modifications
   HeaderUpdate = true;
}//end stDBMTree::DefaultHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::LoadHeader(){

   if (HeaderPage != NULL){
      this->myPageManager->ReleasePage(HeaderPage);
   }//end if

   // Load and set the header.
   HeaderPage = this->myPageManager->GetHeaderPage();
   if (HeaderPage->GetPageSize() < sizeof(stDBMHeader)){
      throw page_size_error("The page size is too small.");
   }//end if

   Header = (stDBMHeader *) HeaderPage->GetData();
   HeaderUpdate = false;
}//end stDBMTree::LoadHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::FlushHeader(){

   if (HeaderPage != NULL){
      if (Header != NULL){
         WriteHeader();
      }//end if
      this->myPageManager->ReleasePage(HeaderPage);
   }//end if
}//end stDBMTree::FlushHeader

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDBMTree<ObjectType, EvaluatorType>::Add(ObjectType * newObj){
   tSubtreeInfo * promo;
   tSubtreeInfo newSubTree;
   tSubtreeInfo farthest;   
   stPage * rootPage;
   stDBMNode * rootNode;
   int idxReturn;
   int insertIdx;
   int idxPromo;

   // Is there a root ?
   if (this->GetRoot() == 0){
      // No! We shall create the new node.
      rootPage = this->NewPage();
      rootNode = new stDBMNode(rootPage, true);
      // Insert the new object.
      insertIdx = rootNode->AddEntry(newObj->GetSerializedSize(),
                                     newObj->Serialize(),
                                     0);
      // Test if the page size is too big to store an object
      #ifdef __stDEBUG__
      if (insertIdx < 0){
         throw page_size_error("The page size is too small to store the first object.");
      }//end if
      #endif //__stDEBUG__

      // Fill entry's fields
      rootNode->GetEntry(insertIdx).Distance = 0.0;
      rootNode->SetRadius(insertIdx, 0);
      #ifdef __stDBMNENTRIES__
         rootNode->SetNEntries(insertIdx, 0);
      #endif //__stDBMNENTRIES__
      #ifdef __stDBMHEIGHT__
         rootNode->GetEntry(insertIdx).Height = 0;
      #endif //__stDBMHEIGHT__

      // Set the header.
      this->SetRoot(rootPage->GetPageID());
      Header->Height = 1; // Update Height
      Header->TreeRadius = 0.0;
      Header->idxRoot = 0;

      // Write node.
      this->myPageManager->WritePage(rootPage);
      // Clean the house.
      delete rootNode;
      this->myPageManager->ReleasePage(rootPage);
   }else{
      // Yes, there is a root.
      // Fill the fields to be insert.
      newSubTree.Rep = newObj;
      newSubTree.Radius = 0.0;
      newSubTree.RootID = 0;
      newSubTree.NObjects = 0;
      newSubTree.Height = 0;
      // Allocate the resources to store the representatives.
      promo = new tSubtreeInfo[GetNumberOfClusters() * sizeof(tSubtreeInfo)];
      // Set null to the representatives. It is a save operation.
      for (idxPromo = 0; idxPromo < GetNumberOfClusters(); idxPromo++){
         promo[idxPromo].Rep = NULL;
      }//end for
      // Let's continue our search for the grail!
      if (this->InsertRecursive(this->GetRoot(), newSubTree, NULL, farthest, promo) == PROMOTION){
         // Split occurred! We must create a new root because it is required.
         // The tree will aacquire a new root.
         this->AddNewRoot(promo);
      }//end if

      // Is the Reinsert option true?
      if (this->IsReInsert()){
         // Get the number of entries in ReInsertCollection.
         idxReturn = reInsertCollection->GetNumberOfEntries();
         // While there is object...
         while (idxReturn > 0){
            // fill the fields to insert.
            newSubTree.Rep = (* reInsertCollection)[idxReturn-1]->GetObject();
            newSubTree.Radius = (* reInsertCollection)[idxReturn-1]->GetRadius();
            newSubTree.RootID = (* reInsertCollection)[idxReturn-1]->GetPageID();
            #ifdef __stDBMNENTRIES__
               newSubTree.NObjects = (* reInsertCollection)[idxReturn-1]->GetNEntries();
            #endif //__stDBMNENTRIES__
            #ifdef __stDBMHEIGHT__
               newSubTree.Height = (* reInsertCollection)[idxReturn-1]->GetHeight();
            #endif //__stDBMHEIGHT__
            // Delete, but first remove the Last from the collection.
            (* reInsertCollection)[idxReturn-1]->SetMine(false);
            reInsertCollection->RemoveLast();
            // Let's continue our search for the grail!
            if (InsertRecursive(this->GetRoot(), newSubTree, NULL, farthest, promo) == PROMOTION){
               // Split occurred! We must create a new root because it is required.
               // The tree will aacquire a new root.
               AddNewRoot(promo);
            }//end if
            // Clean.
            delete newSubTree.Rep;
            // update the number of entries.
            idxReturn = reInsertCollection->GetNumberOfEntries();
         }//end while
      }//end if
      // Update the Radius of the tree.
      Header->TreeRadius = promo[0].Radius;
      // Clean.
      delete[] promo;
   }//end if

   // Update object count.
   UpdateObjectCounter(1);
   // Write Header!
   HeaderUpdate = true;
   
   return true;
}//end stDBMTree::Add

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDBMTree<ObjectType, EvaluatorType>::InsertRecursive(
      stPageID currNodeID, tSubtreeInfo newSubTree, ObjectType * repObj,
      tSubtreeInfo & farthest, tSubtreeInfo * promo){

   stPage * currPage;          // Current page
   stDBMNode * currNode;       // Current node.
   stCount idxRep;
   stDistance dist;            // Temporary distance.
   int insertIdx;              // Insert index.
   int idxReturn;              // For the returnCollection.
   int result;                 // Returning value.
   int subTree;                // Where Subtree I will travel...
   ObjectType * freeObj;       // For free objects.
   ObjectType * subRep;        // for representative Object.
   ObjectType * farthestObj;   // for the farthest removed object.
   bool stop;

   // Read node...
   currPage = this->myPageManager->GetPage(currNodeID);
   currNode = new stDBMNode(currPage);
   
   // Where do I add it?
   subTree = ChooseSubTree(currNode, newSubTree);

   // is there a subtree that qualifies?
   if (subTree >= 0){
      // Yes, there is.
      // Lets get the information about this tree.
      subRep = new ObjectType();
      subRep->Unserialize(currNode->GetObject(subTree),
                          currNode->GetObjectSize(subTree));

      // Is this a subtree?
      if (currNode->GetEntry(subTree).PageID){
         // Yes, it is a subtree. Do the standard operation.
         // Try to insert recursively...
         switch (InsertRecursive(currNode->GetEntry(subTree).PageID,
                                 newSubTree, subRep, farthest,
                                 promo)){

            case NO_ACT: // Update only the Radius, NEntries and Height.
               currNode->SetRadius(subTree, promo[0].Radius);
               #ifdef __stDBMNENTRIES__
                  currNode->SetNEntries(subTree, promo[0].NObjects);
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  currNode->GetEntry(subTree).Height = promo[0].Height;
               #endif //__stDBMHEIGHT__

               // If this option is set to true try to reinsert objects/subtrees
               // in subTree only.
               this->ReInsert(currNode);

               // Returning status.
               promo[0].Radius = currNode->GetMinimumRadius();
               #ifdef __stDBMNENTRIES__
                  promo[0].NObjects = currNode->GetTotalObjectCount();
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  promo[0].Height = currNode->GetHeight() + 1;
               #endif //__stDBMHEIGHT__
               result = NO_ACT;

               break;  //end NO_ACT

            case CHANGE_REP: // Replace the representative
               // Remove previous entry.
               currNode->RemoveEntry(subTree);

               // Try to add the new entry...
               insertIdx = currNode->AddEntry(promo[0].Rep->GetSerializedSize(),
                                              promo[0].Rep->Serialize(),
                                              promo[0].RootID);
               if (insertIdx >= 0){
                  // Swap OK. Fill data.
                  currNode->SetRadius(insertIdx, promo[0].Radius);
                  #ifdef __stDBMNENTRIES__
                     currNode->SetNEntries(insertIdx, promo[0].NObjects);
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     currNode->GetEntry(insertIdx).Height = promo[0].Height;
                  #endif //__stDBMHEIGHT__

                  // Is it the root node?
                  if (repObj != NULL){
                     // No, it is not the root node.
                     if (repObj->IsEqual(subRep)){
                        // The representative was removed. Choose a new one.
                        insertIdx = ChooseRepresentative(currNode);
                        // Propagate the changes.
                        promo[0].Rep->Unserialize(currNode->GetObject(insertIdx),
                                                  currNode->GetObjectSize(insertIdx));
                        result = CHANGE_REP;
                     }else{
                        // Replace the object that is not the representative of this node.
                        currNode->GetEntry(insertIdx).Distance =
                              this->myMetricEvaluator->GetDistance(repObj, promo[0].Rep);
                        // Cut it here
                        delete promo[0].Rep; // promo[0].rep will never be used again.
                        promo[0].Rep = NULL;
                        result = NO_ACT;
                     }//end if
                  }else{
                     Header->idxRoot = currNode->GetRepresentativeIndex();
                     // Yes, it is the root. Distance from the representative is...
                     if (Header->idxRoot >= 0){
                        // The representative of the root node was not removed, get it.
                        // Use promo[0].Rep to save memory allocation.
                        promo[0].Rep->Unserialize(currNode->GetObject(Header->idxRoot),
                                                  currNode->GetObjectSize(Header->idxRoot));
                        UpdateDistances(currNode, promo[0].Rep, Header->idxRoot);
                     }else{
                        // The representative of root node was removed.
                        // Choose a new representative and calculate the distances.
                        Header->idxRoot = ChooseRepresentative(currNode);
                     }//end if
                     // Do not return the new representative!
                     delete promo[0].Rep;
                     promo[0].Rep = NULL;
                     // Report the action.
                     result = NO_ACT;
                  }//end if

                  // If this option is set to true try to reinsert
                  // objects/subtrees in subTree only.
                  this->ReInsert(currNode);

                  // Set the information for the upper levels.
                  promo[0].Radius = currNode->GetMinimumRadius();
                  promo[0].RootID = currNode->GetPageID();
                  #ifdef __stDBMNENTRIES__
                     promo[0].NObjects = currNode->GetTotalObjectCount();
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     promo[0].Height = currNode->GetHeight() + 1;
                  #endif //__stDBMHEIGHT__

               }else{
                  // Split this node.
                  Split(currNode, promo);

                  // return the action.
                  result = PROMOTION; //Report split.
               }//end if

               break;  //end CHANGE_REP

            case PROMOTION: // Promotion!!!

               // Change the representative.
               currNode->RemoveEntry(subTree);
               // add the representative entries.
               idxRep = 0;
////if (promo->GetUsedSpace() + returnCollection->GetUsedSpace() < currNode->GetFree()){
               stop = false;  // At least there are 2 representatives.

               while (!stop){
                  if (promo[idxRep].Rep != NULL){
                     // Get a representative.
                     insertIdx = currNode->AddEntry(promo[idxRep].Rep->GetSerializedSize(),
                                                    promo[idxRep].Rep->Serialize(),
                                                    promo[idxRep].RootID);
                     // if there is space to store the first entry...
                     if (insertIdx >= 0){
                        // Get the others fields.
                        currNode->SetRadius(insertIdx, promo[idxRep].Radius);
                        #ifdef __stDBMNENTRIES__
                           currNode->SetNEntries(insertIdx, promo[idxRep].NObjects);
                        #endif //__stDBMNENTRIES__
                        #ifdef __stDBMHEIGHT__
                           currNode->GetEntry(insertIdx).Height = promo[idxRep].Height;
                        #endif //__stDBMHEIGHT__
                        // currNode->GetEntry(insertIdx).Distance is set in ChooseRepresentative
                        // Delete it.
                        delete promo[idxRep].Rep;
                        promo[idxRep].Rep = NULL;

                        // Update the representative to be insert.
                        idxRep++;
                        // stop?
                        stop = (idxRep == this->GetNumberOfClusters());
                     }else{
                        // Break the loop! There are some representatives that were
                        // not inserted in this node.
                        stop = true;
                     }//end if
                  }else{
                     // Update the representative to be insert.
                     idxRep++;
                     // stop?
                     stop = (idxRep == this->GetNumberOfClusters());
                  }//end if
               }//end while

               // Test if all representatives were inserted.
               if (idxRep == this->GetNumberOfClusters()){
                  // Get the total number of free object to insert in this node.
                  idxReturn = this->returnCollection->GetNumberOfEntries();
                  // Added all representatives.
                  // Now try to insert the others objets in this node, if there is some
                  // Test if the node have a minimum space to store all objects.
                  if (this->returnCollection->GetUsedSpace() < currNode->GetFree()){
                     // Test if there are objects in returnCollection. 
                     stop = (idxReturn == 0);
                     // Get the free entries and try to insert here.
                     while (!stop){
                        // Get the entry.
                        freeObj = (* this->returnCollection)[idxReturn-1]->GetObject();
                        // Try to add freeObj
                        insertIdx = currNode->AddEntry(freeObj->GetSerializedSize(),
                                                       freeObj->Serialize(),
                                                       (* this->returnCollection)[idxReturn-1]->GetPageID());
                        if (insertIdx >= 0){
                           // Insertion OK. Fill data.
                           currNode->SetRadius(insertIdx, (* this->returnCollection)[idxReturn-1]->GetRadius());
                           #ifdef __stDBMNENTRIES__
                              currNode->SetNEntries(insertIdx, (* this->returnCollection)[idxReturn-1]->GetNEntries());
                           #endif //__stDBMNENTRIES__
                           #ifdef __stDBMHEIGHT__
                              currNode->GetEntry(insertIdx).Height = (* this->returnCollection)[idxReturn-1]->GetHeight();
                           #endif //__stDBMHEIGHT__
                           // Distance from representative is set in ChooseRepresentative
                           // or in the Split method...
                           // currNode->GetEntry(insertIdx).Distance = ????;
                           // Remove the entry, it will not use.
                           this->returnCollection->RemoveLast();
                           // Update the number of entries in collection.
                           idxReturn = this->returnCollection->GetNumberOfEntries();
                           stop = (idxReturn == 0);
                        }else{
                           // Stop the loop!
                           stop = true;
                        }//end if
                     }//end while
                  }//end if

                  // what happened?
                  if (idxReturn > 0){
                     // Split this node because some objects in returnCollection
                     // were not inserted in this node
                     this->Split(currNode, promo);

                     // return the action.
                     result = PROMOTION; //Report split.
                     
                  }else{
                     // All the objects in returnCollection were added.
                     // Is it the root node?
                     if (repObj != NULL){
                        // No, it is not the root node.
                        if (repObj->IsEqual(subRep)){
                           // The representative was removed. Choose a new representative.
                           // update distances between the new representative and the others
                           insertIdx = ChooseRepresentative(currNode);
                           // Alocate the resource for the new representative.
                           promo[0].Rep = new ObjectType();
                           // propagate the changes.
                           promo[0].Rep->Unserialize(currNode->GetObject(insertIdx),
                                                     currNode->GetObjectSize(insertIdx));
                           // Report the action!
                           result = CHANGE_REP;
                        }else{
                           // The representative was not removed. Update the distances.
                           UpdateDistances(currNode, repObj, currNode->GetRepresentativeIndex());
                           // Report the action!
                           result = NO_ACT;
                        }//end if
                     }else{
                        // Get the index of this node representative.
                        Header->idxRoot = currNode->GetRepresentativeIndex();
                        // Yes, it is the root. Distance from the representative is...
                        if (Header->idxRoot >= 0){
                           // Alocate the resource for the new representative.
                           repObj = new ObjectType();
                           // Use repObj because to save memory allocation.
                           repObj->Unserialize(currNode->GetObject(Header->idxRoot),
                                               currNode->GetObjectSize(Header->idxRoot));
                           UpdateDistances(currNode, repObj, Header->idxRoot);
                           // Clean the memory.
                           delete repObj;
                           repObj = NULL;
                        }else{
                           // The representative of the root node was removed.
                           // choose a new representative and calculate the distances.
                           Header->idxRoot = ChooseRepresentative(currNode);
                        }//end if

                        // Do not return the representative!
                        // Report the action!
                        result = NO_ACT;
                     }//end if
                     // Try to reinsert objects/subtrees if this option was set to true.
                     this->ReInsert(currNode);

                     // set the information to high levels.
                     promo[0].Radius = currNode->GetMinimumRadius();
                     promo[0].RootID = currNode->GetPageID();
                     #ifdef __stDBMNENTRIES__
                        promo[0].NObjects = currNode->GetTotalObjectCount();
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        promo[0].Height = currNode->GetHeight() + 1;
                     #endif //__stDBMHEIGHT__
                  }//end if

               }else{
                  // Split it because at least one representative was not added.
                  this->Split(currNode, promo);

                  // Return the action.
                  result = PROMOTION; //Report split.
               }//end if

               break;  //end PROMOTION

            case REMOVE_FARTHEST:

               // Calculate distance and its new radius!
               // First update the information in this subtree.
               currNode->SetRadius(subTree, promo[0].Radius);
               currNode->GetEntry(subTree).PageID = promo[0].RootID;
               #ifdef __stDBMNENTRIES__
                  currNode->SetNEntries(subTree, promo[0].NObjects);
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  currNode->GetEntry(subTree).Height = promo[0].Height;
               #endif //__stDBMHEIGHT__

               // Try to insert the subtree in this node
               insertIdx = currNode->AddEntry(farthest.Rep->GetSerializedSize(),
                                              farthest.Rep->Serialize(),
                                              farthest.RootID);
               if (insertIdx >= 0){
                  // Fill entry's fields
                  currNode->SetRadius(insertIdx, farthest.Radius);
                  #ifdef __stDBMNENTRIES__
                     currNode->SetNEntries(insertIdx, farthest.NObjects);
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     currNode->GetEntry(insertIdx).Height = farthest.Height;
                  #endif //__stDBMHEIGHT__

                  // Is it the root node?
                  if (repObj != NULL){
                     // No, it is not the root node.
                     // Calculate the distance.
                     currNode->GetEntry(insertIdx).Distance =
                               this->myMetricEvaluator->GetDistance(repObj, farthest.Rep);
                     // set the information to upper levels.
                     promo[0].RootID = currNode->GetPageID();
                     promo[0].Radius = currNode->GetMinimumRadius();
                     #ifdef __stDBMNENTRIES__
                        promo[0].NObjects = currNode->GetTotalObjectCount();
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        promo[0].Height = currNode->GetHeight() + 1;
                     #endif //__stDBMHEIGHT__
                  }else{
                     Header->idxRoot = currNode->GetRepresentativeIndex();
                     // Yes, it is the root. Distance from the representative is...
                     if (Header->idxRoot >= 0){
                        // The representative of the root node was not removed, get it.
                        repObj = new ObjectType();
                        repObj->Unserialize(currNode->GetObject(Header->idxRoot),
                                            currNode->GetObjectSize(Header->idxRoot));
                        UpdateDistances(currNode, repObj, Header->idxRoot);
                        delete repObj;
                        repObj = NULL;
                        // set the information to upper levels.
                        promo[0].Radius = currNode->GetMinimumRadius();
                     }//end if
                  }//end if

                  delete farthest.Rep;
                  farthest.Rep = NULL;
                  // return the action.
                  result = NO_ACT;
               }else{
                  // There is not enough space to store farthest.Rep.
                  tSubtreeInfo insertFarthest;
                  insertFarthest.Rep = farthest.Rep;
                  insertFarthest.RootID = farthest.RootID;
                  insertFarthest.Radius = farthest.Radius;
                  #ifdef __stDBMNENTRIES__
                     insertFarthest.NEntries = farthest.NEntries;
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     insertFarthest.Height = farthest.Height;
                  #endif //__stDBMHEIGHT__
                  farthest.Rep = NULL;
                  // Is it possible to remove the farthest object?
                  if (RemoveFarthest(currNode, insertFarthest, farthest)){
                     // The method removed the farthest object.
                     // Update the information.
                     promo[0].RootID = currNode->GetPageID();
                     promo[0].Radius = currNode->GetMinimumRadius();
                     #ifdef __stDBMNENTRIES__
                        promo[0].NObjects = currNode->GetTotalObjectCount();
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        promo[0].Height = currNode->GetHeight() + 1;
                     #endif //__stDBMHEIGHT__
                     // Report it to the right levels.
                     if (this->IsReInsert()){
                        // The return object is in returnCollection.
                        result = NO_ACT;
                     }else{
                        // The return object is in newSubTree.
                        result = REMOVE_FARTHEST;
                     }//end if
                     delete insertFarthest.Rep;                  
                  }else{
                     promo[0].Rep = insertFarthest.Rep;
                     promo[0].RootID = insertFarthest.RootID;
                     promo[0].Radius = insertFarthest.Radius;
                     #ifdef __stDBMNENTRIES__
                        promo[0].NObjects = insertFarthest.NObjects;
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        promo[0].Height = insertFarthest.Height;
                     #endif //__stDBMHEIGHT__

                     // Split this node.
                     this->Split(currNode, promo);

                     // Return the action
                     result = PROMOTION; //Report split.
                  }//end if
               }//end if

               break;  //end REMOVE_FARTHEST

         }//end switch
         delete subRep;
         
      }else{
         // No. There is not a subtree. This entry is a free object.
         // Create a new node and grown down the tree, with the new object.
         stPage * newPage = this->NewPage();
         stDBMNode * newNode = new stDBMNode(newPage, true);

         // Add the parent object.
         insertIdx = newNode->AddEntry(subRep->GetSerializedSize(),
                                       subRep->Serialize(),
                                       0);
         // Fill entry's fields
         newNode->GetEntry(insertIdx).Distance = 0.0;
         newNode->SetRadius(insertIdx, 0.0);
         #ifdef __stDBMNENTRIES__
            newNode->SetNEntries(insertIdx, 0);
         #endif //__stDBMNENTRIES__
         #ifdef __stDBMHEIGHT__
            newNode->GetEntry(insertIdx).Height = 0;
         #endif //__stDBMHEIGHT__

         // Add the new object.
         insertIdx = newNode->AddEntry(newSubTree.Rep->GetSerializedSize(),
                                       newSubTree.Rep->Serialize(),
                                       newSubTree.RootID);
         // Fill entry's fields
         newNode->GetEntry(insertIdx).Distance =
                  this->myMetricEvaluator->GetDistance(subRep, newSubTree.Rep);
         newNode->SetRadius(insertIdx, newSubTree.Radius);
         #ifdef __stDBMNENTRIES__
            newNode->SetNEntries(insertIdx, newSubTree.NObjects);
         #endif //__stDBMNENTRIES__
         #ifdef __stDBMHEIGHT__
            newNode->GetEntry(insertIdx).Height = newSubTree.Height;
         #endif //__stDBMHEIGHT__

         // Remove the parent.
         currNode->RemoveEntry(subTree);
         // Try to add the new entry...
         insertIdx = currNode->AddEntry(subRep->GetSerializedSize(),
                                        subRep->Serialize(),
                                        newNode->GetPageID());
         // What happened?
         if (insertIdx >= 0){
            // Insertion OK.
            currNode->SetRadius(insertIdx, newNode->GetMinimumRadius());
            #ifdef __stDBMNENTRIES__
               currNode->SetNEntries(insertIdx, 2);
            #endif //__stDBMNENTRIES__
            #ifdef __stDBMHEIGHT__
               currNode->GetEntry(insertIdx).Height = 1;
            #endif //__stDBMHEIGHT__
            // It is the root node?
            if (repObj != NULL){
               // No, it is not the root node.
               if (repObj->IsEqual(subRep)){
                  // The representative was removed. Choose a new representative.
                  insertIdx = ChooseRepresentative(currNode);
                  promo[0].Rep = new ObjectType();
                  // propagate the changes.
                  promo[0].Rep->Unserialize(currNode->GetObject(insertIdx),
                                            currNode->GetObjectSize(insertIdx));
                  result = CHANGE_REP;
               }else{
                  // The representative was removed. Choose a new representative.
                  currNode->GetEntry(insertIdx).Distance =
                              this->myMetricEvaluator->GetDistance(subRep, repObj);
                  result = NO_ACT;
               }//end if
            }else{
               Header->idxRoot = currNode->GetRepresentativeIndex();
               // Yes, it is the root. Distance from the representative is...
               if (Header->idxRoot >= 0){
                  // The representative of the root node was not removed, get it.
                  repObj = new ObjectType();
                  repObj->Unserialize(currNode->GetObject(Header->idxRoot),
                                      currNode->GetObjectSize(Header->idxRoot));
                  // The representative was removed. Choose a new representative.
                  UpdateDistances(currNode, repObj, Header->idxRoot);
                  delete repObj;
                  repObj = NULL;
               }else{
                  // The representative of the root node was removed.
                  // choose a new representative and calculate the distances.
                  Header->idxRoot = ChooseRepresentative(currNode);
               }//end if
               // Do not return the new representative!
               // Return the action.
               result = NO_ACT;
            }//end if
            // lRep is the prevRep.
            promo[0].Radius = currNode->GetMinimumRadius();
            promo[0].RootID = currNode->GetPageID();
            #ifdef __stDBMNENTRIES__
               promo[0].NObjects = currNode->GetTotalObjectCount();
            #endif //__stDBMNENTRIES__
            #ifdef __stDBMHEIGHT__
               promo[0].Height = currNode->GetHeight() + 1;
            #endif //__stDBMHEIGHT__

         }else{
            // There is not sufficient space to store subRep in currNode.
            // It is the root node?
            if (repObj != NULL){
               // Allocate resources.
               promo[0].Rep = new ObjectType();
               // No, it is not the root node.
               if (repObj->IsEqual(subRep)){
                  // update distances between the new representative and the others
                  insertIdx = ChooseRepresentative(currNode);
                  // propagate the changes.
                  promo[0].Rep->Unserialize(currNode->GetObject(insertIdx),
                                            currNode->GetObjectSize(insertIdx));
               }else{
                  // The representative of the currNode was removed.
                  // choose a new representative and calculate the distances.
                  promo[0].Rep->Unserialize(currNode->GetObject(currNode->GetRepresentativeIndex()),
                                            currNode->GetObjectSize(currNode->GetRepresentativeIndex()));
               }//end if
            }else{
               // Allocate resources.
               promo[0].Rep = new ObjectType();
               Header->idxRoot = currNode->GetRepresentativeIndex();
               // Yes, it is the root. Distance from the representative is...
               if (Header->idxRoot >= 0){
                  promo[0].Rep->Unserialize(currNode->GetObject(Header->idxRoot),
                                            currNode->GetObjectSize(Header->idxRoot));
               }else{
                  // The representative of the root node was removed.
                  // choose a new representative and calculate the distances.
                  insertIdx = ChooseRepresentative(currNode);
                  promo[0].Rep->Unserialize(currNode->GetObject(insertIdx),
                                            currNode->GetObjectSize(insertIdx));
               }//end if
            }//end if

            promo[0].Radius = currNode->GetMinimumRadius();
            promo[0].RootID = currNode->GetPageID();
            #ifdef __stDBMNENTRIES__
               promo[0].NObjects = currNode->GetTotalObjectCount();
            #endif //__stDBMNENTRIES__
            #ifdef __stDBMHEIGHT__
               promo[0].Height = currNode->GetHeight() + 1;
            #endif //__stDBMHEIGHT__

            // rRep is the new rep.
            promo[1].Rep = subRep->Clone();
            promo[1].Radius = newNode->GetMinimumRadius();
            promo[1].RootID = newNode->GetPageID();
            #ifdef __stDBMNENTRIES__
               promo[1].NObjects = newNode->GetTotalObjectCount();
            #endif //__stDBMNENTRIES__
            #ifdef __stDBMHEIGHT__
               promo[1].Height = newNode->GetHeight() + 1;
            #endif //__stDBMHEIGHT__

            // Return the result.
            result = PROMOTION;
         }//end if

         // Write node.
         this->myPageManager->WritePage(newPage);
         //clean the house
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
      }//end if

   }else{
      // No, there is not a subtree that qualifies.
      // Try to insert in this node...
      insertIdx = currNode->AddEntry(newSubTree.Rep->GetSerializedSize(),
                                     newSubTree.Rep->Serialize(),
                                     newSubTree.RootID);

      if (insertIdx >= 0){
         // The insertion is ok!
         // Calculate the distance and verify its new radius.
         if (repObj == NULL){
            // It is a root node. Get the representative.
            repObj = new ObjectType();
            repObj->Unserialize(currNode->GetObject(Header->idxRoot),
                                currNode->GetObjectSize(Header->idxRoot));
            dist = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
            // clean the repObj.
            delete repObj;
            repObj = NULL;
         }else{
            dist = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
         }//end if

         // Fill entry's fields
         currNode->GetEntry(insertIdx).Distance = dist;
         currNode->SetRadius(insertIdx, newSubTree.Radius);
         #ifdef __stDBMNENTRIES__
            currNode->SetNEntries(insertIdx, newSubTree.NObjects);
         #endif //__stDBMNENTRIES__
         #ifdef __stDBMHEIGHT__
            currNode->GetEntry(insertIdx).Height = newSubTree.Height;
         #endif //__stDBMHEIGHT__
         // Set the return values.
         promo[0].Rep = NULL;
         promo[0].Radius = currNode->GetMinimumRadius();
         promo[0].RootID = currNode->GetPageID();
         #ifdef __stDBMNENTRIES__
            newSubTree.NObjects = currNode->GetTotalObjectCount();
         #endif //__stDBMNENTRIES__
         #ifdef __stDBMHEIGHT__
            promo[0].Height = currNode->GetHeight() + 1;
         #endif //__stDBMHEIGHT__
         // Report the action.
         result = NO_ACT;
      }else{
         // Is it possible to remove the farthest object?
         if (RemoveFarthest(currNode, newSubTree, farthest)){
            // The method removed the farthest object.
            // Update the information.
            promo[0].Radius = currNode->GetMinimumRadius();
            promo[0].RootID = currNode->GetPageID();
            #ifdef __stDBMNENTRIES__
               promo[0].NObjects = currNode->GetTotalObjectCount();
            #endif //__stDBMNENTRIES__
            #ifdef __stDBMHEIGHT__
               promo[0].Height = currNode->GetHeight() + 1;
            #endif //__stDBMHEIGHT__
            // Report it to the right levels.
            if (this->IsReInsert()){
               // The farthest object is in returnCollection.
               result = NO_ACT;
            }else{
               // The farthest object is in farthest.
               result = REMOVE_FARTHEST;
            }//end if
         }else{
            promo[0].Rep = (ObjectType *)(newSubTree.Rep)->Clone();
            promo[0].Radius = newSubTree.Radius;
            promo[0].RootID = newSubTree.RootID;
            #ifdef __stDBMNENTRIES__
               promo[0].NObjects = newSubTree.NObjects;
            #endif //__stDBMNENTRIES__
            #ifdef __stDBMHEIGHT__
               promo[0].Height = newSubTree.Height;
            #endif //__stDBMHEIGHT__

            // Split this node because the new object do not fit.
            this->Split(currNode, promo);
            // Return the action
            result = PROMOTION; // Report split.
         }//end if
      }//end if
   }//end if

   // Write the node.
   this->myPageManager->WritePage(currPage);
   // Clean
   delete currNode;
   this->myPageManager->ReleasePage(currPage);

   // Return the action.
   return result;
}//end stDBMTree::InsertRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::ReInsert(stDBMNode * node) {
   ObjectType  * subTreeObj, * obj;   // For free objects.
   stDistance distance, radius;
   int idx, idxSubTree, idxRep;
   bool foundSubTree, stop;

   if (this->IsReInsert()){
      obj = new ObjectType();
      subTreeObj = new ObjectType();
      idxRep = node->GetRepresentativeIndex();

      // For each entry of node.
      for (idx = node->GetNumberOfEntries()-1; idx >= 0; idx--){
         // test if this entry is not the representative of this node.
         if (idx != idxRep){
            // If this entry is a free object or a subtree, in the last case it is
            // not possible to be the root node.
            if ((!node->GetEntry(idx).PageID) || (node->GetPageID() != this->GetRoot())){
               // Its a subtree.... What I will do?
               // Get the subtree to be test.
               obj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
               radius = node->GetRadius(idx);
               // reset variables.
               idxSubTree = 0;
               stop = (idxSubTree >= node->GetNumberOfEntries());
               foundSubTree = false;
               while (!stop){
                  // if this entry is a subtree.
                  if ((node->GetEntry(idxSubTree).PageID) && (idxSubTree != idx)){
                     // Get out the object from Node
                     subTreeObj->Unserialize(node->GetObject(idxSubTree),
                                             node->GetObjectSize(idxSubTree));
                     // Calculate the distance.
                     distance = this->myMetricEvaluator->GetDistance(subTreeObj, obj);
                     // Find a subtree that covers the new object.
                     if (distance + radius <= node->GetRadius(idxSubTree)) {
                        stop = true;         // stop the search.
                        foundSubTree = true;
                     }//end if
                  }//end if
                  idxSubTree++;         // Increment the counter.
                  // if one of the these condicions are true, stop this while.
                  stop = stop || (idxSubTree >= node->GetNumberOfEntries());
               }//end while

               // is it possible to insert in any subtrees?
               if (foundSubTree){
                  // Insert it in the top of the tree.
                  reInsertCollection->AddEntry(obj->Clone(),
                                               #ifdef __stDBMHEIGHT__
                                                  node->GetEntry(idx).Height,
                                               #endif //__stDBMHEIGHT__
                                               node->GetEntry(idx).PageID,
                                               #ifdef __stDBMNENTRIES__
                                                  node->GetNEntries(idx),
                                               #endif //__stDBMNENTRIES__
                                               radius);
                  // yes! But first remove it.
                  node->RemoveEntry(idx);
               }//end if
            }//end if
         }//end if
      }//end for

      // If this node is the root node.
      if (node->GetPageID() == this->GetRoot()){
         // Ajust the global representative index.
         Header->idxRoot = node->GetRepresentativeIndex();
      }//end if /**/

      delete obj;
      delete subTreeObj;
   }//end if

}//end stDBMTree::ReInsert

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDBMTree<ObjectType, EvaluatorType>::RemoveFarthest(stDBMNode * node,
            tSubtreeInfo newSubTree, tSubtreeInfo & farthest) {
   ObjectType * repObj;
   stDistance distance, farthestDistance, finalRadius;
   stSize farthestSize, finalSize;
   stCount farthestIdx, numberOfEntries;
   int insertIdx, repIdx, idx;
   bool removed = false;

   if (this->IsRemoveFarthest()){
      repIdx = node->GetRepresentativeIndex();
      // Is there a representative entry?
      if ((repIdx >= 0) && (node->GetPageID() != this->GetRoot())){
         removed = false;
         numberOfEntries = node->GetNumberOfEntries();
         finalRadius = 0;
         // Get the farthest index.
         farthestIdx = node->GetFarthestObject();
         // Get the representative entry.
         repObj = new ObjectType();
         repObj->Unserialize(node->GetObject(repIdx), node->GetObjectSize(repIdx));
         // Calculate the distance from the new entry.
         distance = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
         // Test if the distance is less than the radius of node
         if (distance + newSubTree.Radius <= node->GetMinimumRadius()){
            // Test if the final radius is less than the actual.
            for (idx = 0; idx < numberOfEntries; idx++){
               if (idx != farthestIdx){
                  if (node->GetEntry(idx).Distance + node->GetRadius(idx) > finalRadius){
                     finalRadius = node->GetEntry(idx).Distance + node->GetRadius(idx);
                  }//end if
               }//end if
            }//end if
            // Calculate the final radius.
            if (distance + newSubTree.Radius > finalRadius){
               finalRadius = distance + newSubTree.Radius;
            }//end if
            // Test the final configuration.
            if (finalRadius < node->GetMinimumRadius()){
               // Ok. The insertion of the new entry will decrease the final radius.
               // Test if there is free space to store the new entry.
               finalSize = newSubTree.Rep->GetSerializedSize();
               if (newSubTree.RootID){
                  finalSize += sizeof(stDistance);  // Add the radius.
                  #ifdef __stDBMNENTRIES__
                     finalSize += sizeof(stCount);  // Add the number of entries.
                  #endif //__stDBMNENTRIES__
               }//end if
               farthestSize = node->GetObjectSize(farthestIdx); 
               if (node->GetEntry(farthestIdx).PageID){
                  farthestSize += sizeof(stDistance);  // Add the radius.
                  #ifdef __stDBMNENTRIES__
                     farthestSize += sizeof(stCount);  // Add the number of entries.
                  #endif //__stDBMNENTRIES__
               }//end if

               if (node->GetFree() + farthestSize >= finalSize){
                  // Is is possible to remove the farthest object and insert the new one.
                  farthest.Rep = new ObjectType();
                  // remove the farthest object.
                  farthest.Rep->Unserialize(node->GetObject(farthestIdx), node->GetObjectSize(farthestIdx));
                  farthest.RootID = node->GetEntry(farthestIdx).PageID;
                  farthest.Radius = node->GetRadius(farthestIdx);
                  #ifdef __stDBMNENTRIES__
                     farthest.NEntries = node->GetNEntries(farthestIdx);
                  #endif //__stDBMNENTRIES__
                  #ifdef __stDBMHEIGHT__
                     farthest.Height = node->GetEntry(farthestIdx).Height;
                  #endif //__stDBMHEIGHT__
                  farthestDistance = node->GetEntry(farthestIdx).Distance;
                  // remove the last.
                  node->RemoveEntry(farthestIdx);
                  // Try to insert the new one.
                  insertIdx = node->AddEntry(newSubTree.Rep->GetSerializedSize(),
                                             newSubTree.Rep->Serialize(),
                                             newSubTree.RootID);
                  if (insertIdx >= 0){  // Insertion Ok.
                     // Fill entry's fields
                     node->GetEntry(insertIdx).Distance = distance;
                     node->SetRadius(insertIdx, newSubTree.Radius);
                     #ifdef __stDBMNENTRIES__
                        node->SetNEntries(insertIdx, newSubTree.NEntries);
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        node->GetEntry(insertIdx).Height = newSubTree.Height;
                     #endif //__stDBMHEIGHT__
                     // Where I will put the farthest object?
                     if (this->IsReInsert()){
                        this->reInsertCollection->AddEntry(farthest.Rep,
                                                           #ifdef __stDBMHEIGHT__
                                                              farthest.Height,
                                                           #endif //__stDBMHEIGHT__
                                                           farthest.RootID,
                                                           #ifdef __stDBMNENTRIES__
                                                              farthest.NEntries,
                                                           #endif //__stDBMNENTRIES__
                                                           farthest.Radius);
                        // Now the owner is reInsertCollection.  
                        farthest.Rep = NULL;
                     }//end if
                     // Report the success.
                     removed = true;
                  }else{
                     // Failured the insertion. Reput the old entry in node.
                     insertIdx = node->AddEntry(farthest.Rep->GetSerializedSize(),
                                                farthest.Rep->Serialize(),
                                                farthest.RootID);
                     // Fill entry's fields
                     node->GetEntry(insertIdx).Distance = farthestDistance;
                     node->SetRadius(insertIdx, farthest.Radius);
                     #ifdef __stDBMNENTRIES__
                        node->SetNEntries(insertIdx, farthest.NEntries);
                     #endif //__stDBMNENTRIES__
                     #ifdef __stDBMHEIGHT__
                        node->GetEntry(insertIdx).Height = farthest.Height;
                     #endif //__stDBMHEIGHT__
                     // Delete the resources.
                     delete farthest.Rep;
                     farthest.Rep = NULL;
                  }//end if
               }//end if
            }//end if
         }//end if
         delete repObj;
      }//end if
   }//end if

   // return the response.
   return removed;
}//end stDBMTree::RemoveFarthest

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
int stDBMTree<ObjectType, EvaluatorType>::ChooseSubTree(
            stDBMNode * node, tSubtreeInfo newSubTree) {
   stCount idx, numberOfEntries;
   stDistance distance, distanceReps, radiusParent;
   stDistance minDistance = MAXDOUBLE;
   int subTree = -1;  // if there is not a subtree that qualifies, return -1.
   ObjectType * repObj = new ObjectType();
   ObjectType * tmpObj;
   bool foundSubTree;
   bool stop;

   numberOfEntries = node->GetNumberOfEntries();

   switch (this->GetChooseMethod()){
      case stDBMTree::cmMINDIST :
         for (idx = 0; idx < numberOfEntries; idx++){
            // if this is a subtree entry.
            if (node->GetEntry(idx).PageID){
               // Get out the object from Node
               repObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
               // Calculate the distance.
               distance = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
               // Find the first subtree that covers the new object.
               if ((distance < minDistance) &&
                   (distance + newSubTree.Radius <= node->GetRadius(idx))) {
                  minDistance = distance;             // set the distance.
                  subTree = idx;      // set the index.
               }//end if
            }//end if
         }//end for
         break; // end stDBMTree::cmMINDIST

      case stDBMTree::cmMINDISTWITHCOVER :
         idx = 0;
         stop = (idx >= numberOfEntries);
         while (!stop){
            // if this entry is a subtree.
            if (node->GetEntry(idx).PageID){
               // Get out the object from Node
               repObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
               // Calculate the distance.
               distance = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
               // Find the first subtree that covers the new object.
               if ((distance + newSubTree.Radius) <= node->GetRadius(idx)) {
                  minDistance = distance;
                  subTree = idx;       // set the index.
                  stop = true;         // stop the search.
               }else if (distance + newSubTree.Radius - node->GetRadius(idx) < minDistance) {
                  minDistance = distance + newSubTree.Radius - node->GetRadius(idx);
                  subTree = idx;
               }//end if
            }//end if
            idx++;         // Increment the counter.
            // if one of the these condicions are true, stop this while.
            stop = stop || (idx >= numberOfEntries);
         }//end while
         // Try to find a better subtree...
         while (idx < numberOfEntries) {
            // If this entry is a subtree.
            if (node->GetEntry(idx).PageID){
               // Get out the object from IndexNode.
               repObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
               // Calculate the distance.
               distance = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
               // Find a better subtree that covers the new object.
               if ((distance + newSubTree.Radius <= node->GetRadius(idx)) &&
                   (distance < minDistance)) {
                  minDistance = distance;
                  subTree = idx;
               }//end if
            }//end if
            idx++;         // Increment the counter.
         }//end while
         break; // end stDBMTree::cmMINDISTWITHCOVER

      case stDBMTree::cmMINDISTWITHOUTCOVER :
         // find something to insert this new object. It is possible the tree
         // to grown to the bottom.
         foundSubTree = false;
         // if there is just one subtree. Do not the job.
         if (numberOfEntries == node->GetNumberOfFreeObjects()){
            // No. There is not a subtree. Insert the object in the current node.
            subTree = -1;
         }else{
            // There is subtrees. How many?
            if (numberOfEntries == (node->GetNumberOfFreeObjects() + 1)){
               // There is only one subtree. Increment it to cover the new object.
               idx = 0;
               stop = (idx >= numberOfEntries);
               while (!stop){
                  // Find the first subtree that covers the new object.
                  if (node->GetEntry(idx).PageID) {
                     stop = true;         // stop the search.
                     subTree = idx;       // set the index.
                  }//end if
                  idx++;         // Increment the counter.
                  // if one of the these condicions are true, stop this while.
                  stop = stop || (idx >= numberOfEntries);
               }//end while
            }else{
               // There is more than one subtree. Choose the best.
               idx = 0;
               stop = (idx >= numberOfEntries);
               while (!stop){
                  // Get out the object from Node
                  repObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
                  // Calculate the distance.
                  distance = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
                  // Find the first subtree that covers the new object.
                  if (node->GetEntry(idx).PageID &&
                     (distance + newSubTree.Radius <= node->GetRadius(idx))) {
                     minDistance = distance;
                     subTree = idx;         // set the index.
                     stop = true;           // stop the search.
                     foundSubTree = true;   // there is a subtree that qualifies.
                  }else if (distance + newSubTree.Radius - node->GetRadius(idx) < minDistance) {
                     minDistance = distance + newSubTree.Radius - node->GetRadius(idx);
                     subTree = idx;
                  }//end if
                  idx++;         // Increment the counter.
                  // if one of the these condicions are true, stop this while.
                  stop = stop || (idx >= numberOfEntries);
               }//end while
               // Try to find a better subtree...
               if (foundSubTree){
                  while (idx < numberOfEntries) {
                     // If this entry is a subtree.
                     if (node->GetEntry(idx).PageID){
                        // Get out the object from Node.
                        repObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
                        // Calculate the distance.
                        distance = this->myMetricEvaluator->GetDistance(repObj, newSubTree.Rep);
                        // Is this entry better than the previous?
                        if ((distance + newSubTree.Radius <= node->GetRadius(idx)) &&
                            (distance < minDistance)) {
                           // Yes, get it.
                           minDistance = distance;
                           subTree = idx;
                        }//end if
                     }//end if
                     idx++;         // Increment the counter.
                  }//end while
               }else{
                  // No. it is a free object. Try to make a cluster.
                  // if there is not a subtree that qualifies.
                  // test if this new cluster will cover the others subtrees.
                  tmpObj = new ObjectType();
                  repObj->Unserialize(node->GetObject(subTree),
                                      node->GetObjectSize(subTree));
                  minDistance += newSubTree.Radius;
                  // pass the other subtrees in this node to do test.
                  for (idx = 0; idx < numberOfEntries; idx++){
                     if ((node->GetEntry(idx).PageID) && (subTree != idx)){
                        // get the distance from the others subtree
                        tmpObj->Unserialize(node->GetObject(idx),
                                            node->GetObjectSize(idx));
                        distanceReps = this->myMetricEvaluator->GetDistance(repObj, tmpObj);
                        // test if there is cover areas with others subtrees.
                        if (minDistance + node->GetRadius(idx) > distanceReps){
                           subTree = -1;    // There are not a entry that qualifies.
                           idx = numberOfEntries;        // break the loop.
                        }//end if
                     }//end if
                  }//end for
                  delete tmpObj;
               }//end if
            }//end if
         }//end if
         break; // end stDBMTree::cmMINDISTWITHOUTCOVER

      #ifdef __stDEBUG__
         default:
            throw logic_error("There is no ChooseSubTree method selected.");
      #endif //__stDEBUG__
   }//end switch

   // Clean.
   delete repObj;
   // Return the subtree.
   return subTree;
}//end stDBMTree::ChooseSubTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::AddNewRoot(tSubtreeInfo * promo){

   stPage * newPage;
   stDBMNode * newNode;
   ObjectType * freeObj;
   int insertIdx, idxReturn, idxPromo;

   // Debug mode!
   #ifdef __stDEBUG__
      if (promo[0].Rep == NULL){
         throw invalid_argument("Invalid object.");
      }//end if
   #endif //__stDEBUG__

   // Now we distribute the objects.
   // Create a new node
   newPage = this->NewPage();
   newNode = new stDBMNode(newPage, true);

   for (idxPromo = 0; idxPromo < GetNumberOfClusters(); idxPromo++){
      if (promo[idxPromo].Rep != NULL){
         insertIdx = newNode->AddEntry(promo[idxPromo].Rep->GetSerializedSize(),
                                       promo[idxPromo].Rep->Serialize(),
                                       promo[idxPromo].RootID);
         #ifdef __stDEBUG__
            if (insertIdx < 0){
               throw page_size_error("The page size is too small or the number of cluster is too high.");
            }//end if
         #endif //__stDEBUG__

         newNode->SetRadius(insertIdx, promo[idxPromo].Radius);
         #ifdef __stDBMNENTRIES__
            newNode->SetNEntries(insertIdx, promo[idxPromo].NObjects);
         #endif //__stDBMNENTRIES__
         #ifdef __stDBMHEIGHT__
            newNode->GetEntry(insertIdx).Height = promo[idxPromo].Height;
         #endif //__stDBMHEIGHT__
         // clean the variables
         delete promo[idxPromo].Rep;
         promo[idxPromo].Rep = NULL;
      }//end if
   }//end for

// Test if the node have a minimum space to store all objects.
//if (returnCollection->GetUsedSpace() < currNode->GetFree()){

   // Get the number of objects in returnCollection.
   idxReturn = this->returnCollection->GetNumberOfEntries();

   // While there are objects in returnCollection.
   while (idxReturn > 0){
      // Get a free object.
      freeObj = (* this->returnCollection)[idxReturn-1]->GetObject();
      // Try to add the free object into newNode.
      insertIdx = newNode->AddEntry(freeObj->GetSerializedSize(),
                                    freeObj->Serialize(),
                                    (* this->returnCollection)[idxReturn-1]->GetPageID());

      // Is the free object in the newNode?
      if (insertIdx >= 0){
         // Yes, it was added.
         newNode->SetRadius(insertIdx, (* this->returnCollection)[idxReturn-1]->GetRadius());
         #ifdef __stDBMNENTRIES__
            newNode->SetNEntries(insertIdx, (* this->returnCollection)[idxReturn-1]->GetNEntries());
         #endif //__stDBMNENTRIES__
         #ifdef __stDBMHEIGHT__
            newNode->GetEntry(insertIdx).Height = (* this->returnCollection)[idxReturn-1]->GetHeight();
         #endif //__stDBMHEIGHT__
         // Remove the Last from the collection.
         this->returnCollection->RemoveLast();
      }else{
         // No, there is not free space to add the free object. Split newNode!
         // Split into 2 new nodes.
         this->Split(newNode, promo);

         // Write the new node.
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         // alocate new resources for newNode
         newPage = this->NewPage();
         newNode = new stDBMNode(newPage, true);

         for (idxPromo = 0; idxPromo < GetNumberOfClusters(); idxPromo++){
            if (promo[idxPromo].Rep != NULL){
               insertIdx = newNode->AddEntry(promo[idxPromo].Rep->GetSerializedSize(),
                                             promo[idxPromo].Rep->Serialize(),
                                             promo[idxPromo].RootID);
               #ifdef __stDEBUG__
                  if (insertIdx < 0){
                     throw page_size_error("The page size is too small or the number of cluster is too high.");
                  }//end if
               #endif //__stDEBUG__

               newNode->SetRadius(insertIdx, promo[idxPromo].Radius);
               #ifdef __stDBMNENTRIES__
                  newNode->SetNEntries(insertIdx, promo[idxPromo].NObjects);
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  newNode->GetEntry(insertIdx).Height = promo[idxPromo].Height;
               #endif //__stDBMHEIGHT__
               // clean the variables
               delete promo[idxPromo].Rep;
               promo[idxPromo].Rep = NULL;
            }//end if
         }//end for

         #ifndef __stDBMHEIGHT__
            Header->Height++;
         #endif //__stDBMHEIGHT__
      }//end if
      // Update the number of objects in returnCollection.
      idxReturn = this->returnCollection->GetNumberOfEntries();
   }//end while

   // Update the tree
   Header->idxRoot = this->ChooseRepresentative(newNode);
   #ifdef __stDBMHEIGHT__
      Header->Height = newNode->GetHeight() + 1;
   #else
      Header->Height++;
   #endif //__stDBMHEIGHT__
   // Update the id of the new root.
   this->SetRoot(newPage->GetPageID());
   // Update the radius of the tree.
   promo[0].Radius = newNode->GetMinimumRadius();

   // Write the page.
   this->myPageManager->WritePage(newPage);
   delete newNode;
   // Dispose page
   this->myPageManager->ReleasePage(newPage);

}//end stDBMTree::AddNewRoot

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::Split(stDBMNode * oldNode,
                                                 tSubtreeInfo * promo){

   tLogicNode * logicNode;
   tMSTSplitter * mstSplitter;
   tDBMNSplitter * nSplitter;
   stCount TotalNumOfEntries, retCollNumberOfEntries;
   stDistance prevRadius;
   int idx, idxInsert;
   stCount idxRep; 
   ObjectType * repObj;
   stDistance finalRadius;
   stPage * newPage;
   stDBMNode * newNode;

   if (GetSplitMethod() != stDBMTree::smN_SPLIT){
      newPage = this->NewPage();
      newNode = new stDBMNode(newPage, true);
   }//end if

   // To use in MST_DB
   retCollNumberOfEntries = this->returnCollection->GetNumberOfEntries();
   
   // Calculate the total number of entries that will be use in logicNode.
   TotalNumOfEntries = oldNode->GetNumberOfEntries() + retCollNumberOfEntries;

   // For all the representatives.
   for (idx = 0; idx < this->GetNumberOfClusters(); idx++){
      if (promo[idx].Rep != NULL){
         TotalNumOfEntries++;
      }//end if
   }//end for

   // Create the new tLogicNode
   logicNode = new tLogicNode(TotalNumOfEntries);

   // update the maximum number of entries.
   this->SetMaxOccupation(oldNode->GetNumberOfEntries());
   // sets the minimum occupation of this nodes.
   logicNode->SetMinOccupation((stCount )(this->GetMaxOccupation() * this->GetMinOccupation()));

   // Add all objects from oldNode and returnCollection.
   logicNode->AddNode(oldNode);
   logicNode->AddCollection(this->returnCollection);

   // clean before use it.
   oldNode->RemoveAll();
   this->returnCollection->RemoveAll();

   // Add in logicNode all entries from promo.
   for (idx = 0; idx < this->GetNumberOfClusters(); idx++){
      if (promo[idx].Rep != NULL){
         idxInsert = logicNode->AddEntry(promo[idx].Rep);
         logicNode->SetEntry(idxInsert,
                             promo[idx].RootID
                             #ifdef __stDBMNENTRIES__
                                , promo[idx].NEntries
                             #endif //__stDBMNENTRIES__
                             , promo[idx].Radius
                             #ifdef __stDBMHEIGHT__
                                , promo[idx].Height
                             #endif //__stDBMHEIGHT__
                             );
         promo[idx].Rep = NULL;                             
      }//end if
   }//end for

   // Split it.
   switch (GetSplitMethod()) {
      case stDBMTree::smMIN_MAX:
         // Build the distance matrix.
         logicNode->BuildDistanceMatrix(this->myMetricEvaluator);
         // Emulate a Slim-tree?
         if (this->GetChooseMethod() == cmMINDISTWITHCOVER){
            // Test the distribution.
            this->MinMaxRadiusPromoteSlim(logicNode);
            // Distribute
            //logicNode->Distribute(oldNode, lRep, newNode, rRep, this->myMetricEvaluator);
            logicNode->Distribute(oldNode, newNode, promo, this->myMetricEvaluator);
         }else{
            // Test the distribution.
            this->MinMaxRadiusPromote(logicNode);
            // Distribute
            logicNode->Distribute(this->returnCollection, oldNode, newNode, promo, this->myMetricEvaluator);
         }//end if
         // Write the new node
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         break;  //end stDBMTree::smMIN_MAX

      case stDBMTree::smMIN_SUM:
         // Build the distance matrix.
         logicNode->BuildDistanceMatrix(this->myMetricEvaluator);
         // Emulate a Slim-tree?
         if (this->GetChooseMethod() == cmMINDISTWITHCOVER){
            // Test the distribution.
            this->MinSumRadiusPromoteSlim(logicNode);
            // Distribute
            logicNode->Distribute(oldNode, newNode, promo, this->myMetricEvaluator);
         }else{
            // Test the distribution.
            this->MinSumRadiusPromote(logicNode);
            // Distribute
            logicNode->Distribute(this->returnCollection, oldNode, newNode, promo, this->myMetricEvaluator);
         }//end if
         // Write the new node
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         break;  //end stDBMTree::smMIN_SUM

      case stDBMTree::smMIN_FREE:
         // Build the distance matrix.
         logicNode->BuildDistanceMatrix(this->myMetricEvaluator);
         // Test the distribution.
         this->MinFreeRadiusPromote(logicNode);
         // Distribute
         logicNode->Distribute(this->returnCollection, oldNode, newNode, promo, this->myMetricEvaluator);
         // Write the new node
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         break;  //end stDBMTree::smMIN_FREE

      case stDBMTree::smMIN_COVER:
         // Build the distance matrix.
         logicNode->BuildDistanceMatrix(this->myMetricEvaluator);
         // Test the distribution.
         this->MinCoverPromote(logicNode);
         // Distribute
         logicNode->Distribute(this->returnCollection, oldNode, newNode, promo, this->myMetricEvaluator);
         // Write the new node
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         break;  //end stDBMTree::smMIN_COVER

      case stDBMTree::smMAX_DISTANCE:
         // Build the distance matrix.
         logicNode->BuildDistanceMatrix(this->myMetricEvaluator);
         // Test the distribution.
         logicNode->MaxDistancePromote();
         // Distribute
         logicNode->Distribute(this->returnCollection, oldNode, newNode, promo, this->myMetricEvaluator);
         // choose another representative to minimize the radius.
         idxRep = ChooseRepresentative(newNode);
         // Get the rep0.
         promo[0].Rep->Unserialize(newNode->GetObject(idxRep),
                                   newNode->GetObjectSize(idxRep));

         idxRep = ChooseRepresentative(oldNode);
         // Get the rep1.
         promo[1].Rep->Unserialize(oldNode->GetObject(idxRep),
                                   oldNode->GetObjectSize(idxRep));
         // Write the new node
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         break;  //end stDBMTree::smMAX_DISTANCE

      case stDBMTree::smMST_SLIM:
         // MST Split
         mstSplitter = new tMSTSplitter(logicNode);
         // Perform MST
         mstSplitter->Distribute(oldNode, newNode, promo, this->myMetricEvaluator);
         // Clean home
         delete mstSplitter;
         // Write the new node
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         break;  //end stDBMTree::smMST_SLIM

      case stDBMTree::smMST_DB:
         // MST Split
         mstSplitter = new tMSTSplitter(logicNode);
         // Emulate a Slim-tree?
         if (this->GetChooseMethod() == cmMINDISTWITHCOVER){
            // Perform MST
            mstSplitter->Distribute(oldNode, newNode, promo, this->myMetricEvaluator);
         }else{
            // Perform MST
            mstSplitter->Distribute(this->returnCollection, oldNode, newNode, promo, this->myMetricEvaluator);
            // Tests...
            if ((this->returnCollection->GetNumberOfEntries() == retCollNumberOfEntries) &&
                (this->returnCollection->GetNumberOfEntries() != 0) &&
                ((oldNode->GetNumberOfEntries() == 1) || (newNode->GetNumberOfEntries() == 1))){
               #ifdef __stDEBUG__
                  cout << "oldNode " << oldNode->GetNumberOfEntries()
                       << " newNode " << newNode->GetNumberOfEntries()
                       << " returnCollection " << returnCollection->GetNumberOfEntries() << "\n";
               #endif //__stDEBUG__
               delete logicNode;
               // Create the new tLogicNode
               logicNode = new tLogicNode(TotalNumOfEntries);
               // sets the minimum occupation of this nodes.
               logicNode->SetMinOccupation((stCount)this->GetMinOccupation());
               // Add objects from oldNode and returnCollection.
               logicNode->AddNode(oldNode);
               logicNode->AddNode(newNode);
               logicNode->AddCollection(this->returnCollection);
               // Clean before use.
               this->returnCollection->RemoveAll();
               oldNode->RemoveAll();
               newNode->RemoveAll();
               // Build the distance matrix.
               logicNode->BuildDistanceMatrix(this->myMetricEvaluator);
               // Test the distribution.
               this->MinMaxRadiusPromote(logicNode);
               // Distribute
               logicNode->Distribute(this->returnCollection, oldNode, newNode, promo, this->myMetricEvaluator);
            }//end if /**/
         }//end if
         // Clean home
         delete mstSplitter;
         // Write the new node
         this->myPageManager->WritePage(newPage);
         delete newNode;
         this->myPageManager->ReleasePage(newPage);
         break;  //end stDBMTree::smMST_DB

      case stDBMTree::smN_SPLIT:
         // N-Split
         nSplitter = new tDBMNSplitter(logicNode, this->GetNumberOfClusters(), this->myPageManager);
         // Perform Cluster
         Header->NodeCount += nSplitter->Distribute(oldNode, this->returnCollection, promo, this->myMetricEvaluator);
         // Clean home
         delete nSplitter;
         break;  //end stDBMTree::smN_SPLIT

      #ifdef __stDEBUG__
         default:
            throw logic_error("There is no Split method selected.");
      #endif //__stDEBUG__
   };//end switch

   //Clean the home before leave.
   delete logicNode;
}//end stDBMTree::Split
/*
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::BulkLoad(ObjectType ** data, long numberOfObjects){
   stDBMNode * rootNode;

   tDBMBulkLoad * bulkLoad = new tDBMBulkLoad(data, numberOfObjects,
         this->GetMinOccupation(), this->GetNumberOfClusters(),
         this->myPageManager, this->myMetricEvaluator);

   rootNode = bulkLoad->GetNewRoot();

   // Set the header.
   this->SetRoot(rootNode->GetPageID());
   Header->Height = rootNode->GetHeight(); // Update Height
   Header->TreeRadius = rootNode->GetMinimumRadius();
   Header->idxRoot = this->ChooseRepresentative(rootNode);
   Header->ObjectCount = rootNode->GetTotalObjectCount();
   Header->TreeRadius = rootNode->GetMinimumRadius();

   // Notify modifications
   HeaderUpdate = true;

   //Clean the home before leave.
   delete bulkLoad;
}//end stDBMTree::BulkLoad
*/
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MinMaxRadiusPromote(tLogicNode * node) {
   stDistance radius0, radius1;
   stDistance minRadius = MAXDOUBLE;
   stCount minFreeObjects = MAXINT;
   stCount idx0, idx1;
   stCount i, j;
   stCount numberOfEntries = node->GetNumberOfEntries();
   tDBMCollection * returnCollection = new tDBMCollection(numberOfEntries);
   tDBMMemNode * node0 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());
   tDBMMemNode * node1 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());


   // For each entry do.
   for (i = 0; i < numberOfEntries-1; i++) {
      for (j = i + 1; j < numberOfEntries; j++) {
         node->SetRepresentative(i, j);
         // Distribute
         node->TestDistribution(returnCollection, node0, node1, this->myMetricEvaluator);
         radius0 = node0->GetMinimumRadius();
         radius1 = node1->GetMinimumRadius();
         // Get the greatest radius.
         if (radius0 < radius1){
            radius0 = radius1;
         }//end if

         // Test if the new radius is less than the previous radius.
         if (radius0 <= minRadius) {
            // This condition is for discrete distances.
            // If the distance is equal to the previous distance.
            if (radius0 == minRadius) {
               // Test if the new representatives generates less free object.
               if (returnCollection->GetNumberOfEntries() < minFreeObjects){
                  // This pairs will be the new representatives.
                  minFreeObjects = returnCollection->GetNumberOfEntries();
                  minRadius = radius0;
                  idx0 = i;
                  idx1 = j;
               }//end if
            }else{
               // Do not matter. The distance is less than the previous
               minFreeObjects = returnCollection->GetNumberOfEntries();
               minRadius = radius0;
               idx0 = i;
               idx1 = j;
            }//end if
         }//end if
      }//end for
   }//end for

   // Choose the representatives.
   node->SetRepresentative(idx0, idx1);
   // Clean.
   delete returnCollection;
   delete node0;
   delete node1;
}//end stDBMTree::MinMaxRadiusPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MinSumRadiusPromote(tLogicNode * node) {
   stDistance radius0, radius1;
   stDistance minSumRadius = MAXDOUBLE;
   stCount minFreeObjects = MAXINT;
   stCount idx0, idx1;
   stCount i, j;
   stCount numberOfEntries = node->GetNumberOfEntries();
   tDBMCollection * returnCollection = new tDBMCollection(numberOfEntries);
   tDBMMemNode * node0 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());
   tDBMMemNode * node1 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());

   // For each entry do.
   for (i = 0; i < numberOfEntries-1; i++) {
      for (j = i + 1; j < numberOfEntries; j++) {
         node->SetRepresentative(i, j);
         // Distribute
         node->TestDistribution(returnCollection, node0, node1, this->myMetricEvaluator);
         radius0 = node0->GetMinimumRadius();
         radius1 = node1->GetMinimumRadius();

         // Test if the new radius is less than the previous radius.
         if ((radius0 + radius1) <= minSumRadius) {
            // This condition is for discrete distances.
            // If the distance is equal to the previous distance.
            if ((radius0 + radius1) == minSumRadius) {
               // Test if the new representatives generates less free object.
               if (returnCollection->GetNumberOfEntries() < minFreeObjects){
                  // This pairs will be the new representatives.
                  minFreeObjects = returnCollection->GetNumberOfEntries();
                  minSumRadius = radius0 + radius1;
                  idx0 = i;
                  idx1 = j;
               }//end if
            }else{
               // Do not matter. The distance is less than the previous
               minFreeObjects = returnCollection->GetNumberOfEntries();
               minSumRadius = radius0 + radius1;
               idx0 = i;
               idx1 = j;
            }//end if
         }//end if
      }//end for
   }//end for

   // Choose representatives
   node->SetRepresentative(idx0, idx1);
   // Clean.
   delete returnCollection;
   delete node0;
   delete node1;
}//end stDBMTree::MinSumRadiusPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MinMaxRadiusPromoteSlim(tLogicNode * node) {
   stDistance radius0, radius1;
   stDistance minRadius = MAXDOUBLE;
   stCount idx0, idx1;
   stCount i, j;
   stCount numberOfEntries = node->GetNumberOfEntries();
   tDBMMemNode * node0 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());
   tDBMMemNode * node1 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());

   // For each entry do.
   for (i = 0; i < numberOfEntries-1; i++) {
      for (j = i + 1; j < numberOfEntries; j++) {
         node->SetRepresentative(i, j);
         // Distribute
         node->TestDistribution(node0, node1, this->myMetricEvaluator);
         radius0 = node0->GetMinimumRadius();
         radius1 = node1->GetMinimumRadius();
         // Get the greatest radius.
         if (radius0 < radius1){
            radius0 = radius1;
         }//end if

         // Test if the new radius is less than the previous radius.
         if (radius0 <= minRadius) {
            minRadius = radius0;
            idx0 = i;
            idx1 = j;
         }//end if
      }//end for
   }//end for

   // Choose the representatives.
   node->SetRepresentative(idx0, idx1);
   // Clean.
   delete node0;
   delete node1;
}//end stDBMTree::MinMaxRadiusPromoteSlim

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MinSumRadiusPromoteSlim(tLogicNode * node) {
   stDistance radius0, radius1;
   stDistance minSumRadius = MAXDOUBLE;
   stCount idx0, idx1;
   stCount i, j;
   stCount numberOfEntries = node->GetNumberOfEntries();
   tDBMMemNode * node0 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());
   tDBMMemNode * node1 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());

   // For each entry do.
   for (i = 0; i < numberOfEntries-1; i++) {
      for (j = i + 1; j < numberOfEntries; j++) {
         node->SetRepresentative(i, j);
         // Distribute
         node->TestDistribution(node0, node1, this->myMetricEvaluator);
         radius0 = node0->GetMinimumRadius();
         radius1 = node1->GetMinimumRadius();

         // Test if the new radius is less than the previous radius.
         if ((radius0 + radius1) <= minSumRadius) {
            minSumRadius = radius0 + radius1;
            idx0 = i;
            idx1 = j;
         }//end if
      }//end for
   }//end for

   // Choose representatives
   node->SetRepresentative(idx0, idx1);
   // Clean.
   delete node0;
   delete node1;
}//end stDBMTree::MinSumRadiusPromoteSlim

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MinFreeRadiusPromote(tLogicNode * node) {
   stDistance sumRadius;
   stDistance minRadius = MAXDOUBLE;
   stCount idx0, idx1;
   stCount i, j;
   stCount minFreeObjects = MAXINT;
   stCount numberOfEntries = node->GetNumberOfEntries();
   tDBMCollection * returnCollection = new tDBMCollection(numberOfEntries);
   tDBMMemNode * node0 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());
   tDBMMemNode * node1 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());

   // For each entry.
   for (i = 0; i < numberOfEntries-1; i++) {
      for (j = i + 1; j < numberOfEntries; j++) {
         node->SetRepresentative(i, j);
         // Distribute
         node->TestDistribution(returnCollection, node0, node1, this->myMetricEvaluator);
         sumRadius = node0->GetMinimumRadius() + node1->GetMinimumRadius();

         if (sumRadius < node1->GetMinimumRadius())
            sumRadius = node1->GetMinimumRadius();

         // If the sumRadius and the freeObject is less than the previous
         if (returnCollection->GetNumberOfEntries() <= minFreeObjects){
            if (returnCollection->GetNumberOfEntries() == minFreeObjects){
               if (sumRadius < minRadius){
                  // Set the information of the new representatives.
                  minFreeObjects = returnCollection->GetNumberOfEntries();
                  minRadius = sumRadius;
                  idx0 = i;
                  idx1 = j;
               }//end if
            }else{
               // Set the information of the new representatives.
               minFreeObjects = returnCollection->GetNumberOfEntries();
               minRadius = sumRadius;
               idx0 = i;
               idx1 = j;
            }//end if
         }//end if
      }//end for
   }//end for
   // Choose representatives
   node->SetRepresentative(idx0, idx1);
   // Clean.
   delete returnCollection;
   delete node0;
   delete node1;
}//end stDBMTree::MinFreeRadiusPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MinCoverPromote(tLogicNode * node){
   stDistance radius0, radius1, distanceReps;
   stDistance distance;
   stDistance minDistance = -MAXDOUBLE;
   stCount idx0, idx1;
   stCount i, j;
   stCount numberOfEntries = node->GetNumberOfEntries();
   tDBMCollection * returnCollection = new tDBMCollection(numberOfEntries);
   tDBMMemNode * node0 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());
   tDBMMemNode * node1 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());

   for (i = 0; i < numberOfEntries-1; i++) {
      for (j = i + 1; j < numberOfEntries; j++) {
         node->SetRepresentative(i, j);
         // Distribute
         node->TestDistribution(returnCollection, node0, node1, this->myMetricEvaluator);
         distanceReps = this->myMetricEvaluator->GetDistance(node->GetObject(i),
                                                       node->GetObject(j));
         radius0 = node0->GetMinimumRadius();
         radius1 = node1->GetMinimumRadius();
         distance = distanceReps - (radius0 + radius1);

         // Test if the new cover ares is less than the previous.
         if (distance > minDistance) {
            minDistance = distance;
            idx0 = i;
            idx1 = j;
         }//end if
      }//end for
   }//end for

   // Choose representatives
   node->SetRepresentative(idx0, idx1);
   // Clean.
   delete returnCollection;
   delete node0;
   delete node1;
}//end stDBMTree::MinCoverPromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MaxDistancePromote(tLogicNode * node){

   stDistance distanceReps;
   stDistance maxDistance = 0;
   stCount idx0, idx1;
   stCount i, j;
   stCount numberOfEntries = node->GetNumberOfEntries();
   tDBMCollection * returnCollection = new tDBMCollection(numberOfEntries);
   tDBMMemNode * node0 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());
   tDBMMemNode * node1 = new tDBMMemNode(this->myPageManager->GetMinimumPageSize());

   for (i = 0; i < numberOfEntries-1; i++) {
      for (j = i + 1; j < numberOfEntries; j++) {
         node->SetRepresentative(i, j);
         // Distribute
         node->TestDistribution(returnCollection, node0, node1, this->myMetricEvaluator);
         distanceReps = this->myMetricEvaluator->GetDistance(node->GetObject(i),
                                                       node->GetObject(j));
         // Test if the new distance of the two representatives is greater than the previous.
         if (distanceReps > maxDistance) {
            maxDistance = distanceReps;
            idx0 = i;
            idx1 = j;
         }//end if
      }//end for
   }//end for

   // Choose representatives
   node->SetRepresentative(idx0, idx1);
   // Clean.
   delete returnCollection;
   delete node0;
   delete node1;
}//end stDBMTree::MaxDistancePromote

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::UpdateDistances(stDBMNode * node,
            ObjectType * repObj, int repObjIdx){
   stCount idx;
   stCount numberOfEntries = node->GetNumberOfEntries();
   ObjectType * tmpObj = new ObjectType();

   // First test if there is a representative.
   if (repObjIdx >= 0){
      // Yes! There is.
      for (idx = 0; idx < numberOfEntries; idx++){
         if (idx != repObjIdx){
            tmpObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
            node->GetEntry(idx).Distance = this->myMetricEvaluator->GetDistance(repObj, tmpObj);
         }else{
            //it is the representative object
            node->GetEntry(idx).Distance = 0.0;
         }//end if
      }//end for
   }else{
      // There is not a representative. Choose one.
      this->ChooseRepresentative(node);
   }//end if

   //clean the house before exit.
   delete tmpObj;
}//end stDBMTree::UpdateDistances

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMTree<ObjectType, EvaluatorType>::ChooseRepresentative(stDBMNode * node){

   stCount idx, idxRep;
   stDistance newRadius = MAXDOUBLE;
   ObjectType * repObj = new ObjectType();
   stCount numberOfEntries = node->GetNumberOfEntries();

   //test what representative in the testNode is the best for the global representative.
   for (idx = 0; idx < numberOfEntries; idx++){
      repObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
      // Calculate the distances.
      UpdateDistances(node, repObj, idx);
      if (node->GetMinimumRadius() < newRadius){
         newRadius = node->GetMinimumRadius();
         idxRep = idx;
      }//end if
   }//end if

   // Get the best representative.
   repObj->Unserialize(node->GetObject(idxRep), node->GetObjectSize(idxRep));
   // Calculate the distances to the new representative.
   UpdateDistances(node, repObj, idxRep);
   // Clean.
   delete repObj;
   // Return the index of the new representative.
   return idxRep;
}//end stDBMTree::ChooseRepresentative

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance stDBMTree<ObjectType, EvaluatorType>::GetGreaterEstimatedDistance(){
   stDistance distance = 0;
   stDistance distanceTemp = 0;
   int idx, idx2;
   ObjectType ** objects;
   stCount size = 0;
   stPage * currPage, * currPage2;
   stDBMNode * currNode, * currNode2;

   // Is there a root ?
   if (this->GetRoot()){
      // Yes. Read the root node.
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = new stDBMNode(currPage);

      // estimate the maximum number of entries.
      objects = new ObjectType * [this->GetMaxOccupation() * currNode->GetNumberOfEntries()];

      // For each entry...
      for (idx = 0; idx < currNode->GetNumberOfEntries(); idx++) {
         // Get the pages.
         // Is it a index node?
         if (currNode->GetEntry(idx).PageID) {
            currPage2 = this->myPageManager->GetPage(currNode->GetEntry(idx).PageID);
            currNode2 = new stDBMNode(currPage2);

            // For each entry...
            for (idx2 = 0; idx2 < currNode2->GetNumberOfEntries(); idx2++) {
               // Rebuild the object
               objects[size] = new ObjectType();
               objects[size++]->Unserialize(currNode2->GetObject(idx2),
                                            currNode2->GetObjectSize(idx2));
            }//end for
            // Free it all
            delete currNode2;
            this->myPageManager->ReleasePage(currPage2);
         }else{
            // Rebuild the object
            objects[size] = new ObjectType();
            objects[size++]->Unserialize(currNode->GetObject(idx),
                                         currNode->GetObjectSize(idx));
         }//end if
      }//end for

      //search every entry in the root node
      for (idx = 0; idx < size-1; idx++){
         //combine with the others objects
         for (idx2 = idx+1; idx2 < size; idx2++){
            //calculate the distance of the two objects
            distanceTemp = this->myMetricEvaluator->GetDistance(objects[idx], objects[idx2]);
            //if this sum is greater than the previous...
            if (distanceTemp > distance)
               //store it to return this value
               distance = distanceTemp;
         }//end for
      }//end for
      
      //cleaning...
      for (idx = 0; idx < size; idx++){
         delete objects[idx];
      }//end for
      delete[] objects;

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   //return the maximum distance between 2 objects of this tree
   return distance;
}//end stDBMTree::GetGreaterEstimatedDistance


//==============================================================================
// Begin of Queries
//==============================================================================

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDBMTree<ObjectType, EvaluatorType>::RangeQuery(
            ObjectType * sample, stDistance range){
   tResult * result = new tResult();  // Create result
   ObjectType tmpObj;
   stPage * currPage;
   stDBMNode * currNode;
   stDistance distance, distanceRepres;
   stCount numberOfEntries, idx;
   #ifdef __stMAMVIEW__
      stMessageString title;
      stMessageString comment;
   #endif //__stMAMVIEW__

   result->SetQueryInfo(sample->Clone(), tResult::RANGEQUERY, -1, range, false);

   // Visualization support
   #ifdef __stMAMVIEW__
      MAMViewer->SetQueryInfo(0, range);
      MAMViewer->SetLevel(0);
      title.Append("DBM-Tree: Range Query with page size ");
      title.Append((int) this->myPageManager->GetMinimumPageSize());
      comment.Append("The radius of this range query is ");
      comment.Append((double) range);
      MAMViewer->BeginAnimation(title.GetStr(), comment.GetStr());
   #endif //__stMAMVIEW__

   // Let's search
   if (this->GetRoot() != 0){
      // Read the root node
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
      // Rebuild the representative object.
      tmpObj.Unserialize(currNode->GetObject(Header->idxRoot),
                         currNode->GetObjectSize(Header->idxRoot));
      // Evaluate the distance between the query object and the representative.
      distanceRepres = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

      // Visualization support
      #ifdef __stMAMVIEW__
         comment.Clear();
         comment.Append("Root is the node ");
         comment.Append((int) this->GetRoot());
         MAMViewer->BeginFrame(comment.GetStr());
         MAMViewer->SetNode(this->GetRoot(), &tmpObj,
                            this->GetTreeRadius(), 0, 0, true);

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            if (currNode->GetEntry(idx).PageID){
               // Add all child nodes all active
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->LevelUp();
               MAMViewer->SetNode(currNode->GetEntry(idx).PageID, &tmpObj,
                     currNode->GetRadius(idx), this->GetRoot(), 0, true);
               MAMViewer->LevelDown();
            }else{
               // Add objects to the node
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, this->GetRoot(), true);
            }//end if
         }//end for
         MAMViewer->SetResult(sample, result);
         MAMViewer->EndFrame();
      #endif //__stMAMVIEW__

      // Call the RangeQuery
      for (idx = 0; idx < numberOfEntries; idx++) {
         // use of the triangle inequality to cut a subtree
         if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
             range + currNode->GetRadius(idx)){
            // Rebuild the object
            tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
            // is it a Representative?
            if (idx != Header->idxRoot) {
               // No, it is not a representative. Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            }else{
               distance = distanceRepres;
            }//end if

            if (distance <= range + currNode->GetRadius(idx)){
               // tests if this is or not a representative
               if (!currNode->GetEntry(idx).PageID){
                  // No, there is not a subtree. But this object qualifies.
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);

                  #ifdef __stMAMVIEW__
                     comment.Clear();
                     comment.Append("Object from node");
                     comment.Append((int) this->GetRoot());
                     comment.Append(" added. The result has ");
                     comment.Append((int) result->GetNumOfEntries());
                     comment.Append(" object(s) and radius ");
                     comment.Append((double) result->GetMaximumDistance());
                     MAMViewer->BeginFrame(comment.GetStr());
                     MAMViewer->EnableNode(this->GetRoot());
                     MAMViewer->SetResult(sample, result);
                     MAMViewer->EndFrame();
                  #endif //__stMAMVIEW__

               }else{
                  // Yes! This subtree qualified !
                  this->RangeQuery(currNode->GetEntry(idx).PageID, result,
                                   sample, range, distance);
                  #ifdef __stMAMVIEW__
                     comment.Clear();
                     comment.Append("Returning to node ");
                     comment.Append((int) this->GetRoot());
                     comment.Append(" at level ");
                     comment.Append((int) MAMViewer->GetLevel());
                     MAMViewer->BeginFrame(comment.GetStr());
                     MAMViewer->EnableNode(this->GetRoot());
                     MAMViewer->EndFrame();
                  #endif //__stMAMVIEW__
               }//end if
            }//end if
         }//end if
      }//end for
      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if


   // Visualization support
   #ifdef __stMAMVIEW__
      // Add the last frame with the final result
      comment.Clear();
      comment.Append("The final result has ");
      comment.Append((int) result->GetNumOfEntries());
      comment.Append(" object(s) and radius ");
      comment.Append((double) result->GetMaximumDistance());
      MAMViewer->BeginFrame(comment.GetStr());
      MAMViewer->SetResult(sample, result);
      MAMViewer->EndFrame();
      MAMViewer->EndAnimation();
   #endif //__stMAMVIEW__

   return result;
}//end stDBMTree::RangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::RangeQuery(
         stPageID pageID, tResult * result, ObjectType * sample,
         stDistance range, stDistance distanceRepres){
   stPage * currPage;
   stDBMNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stCount idx, numberOfEntries;
   #ifdef __stMAMVIEW__
      stMessageString comment;
   #endif //__stMAMVIEW__   

   // Visualization support
   #ifdef __stMAMVIEW__
      MAMViewer->LevelUp();
   #endif //__stMAMVIEW__
   
   // Read node...
   currPage = this->myPageManager->GetPage(pageID);
   currNode = new stDBMNode(currPage);
   // Get the number of entries
   numberOfEntries = currNode->GetNumberOfEntries();

   // Visualization support
   #ifdef __stMAMVIEW__
      comment.Clear();
      comment.Append("Entering in the node ");
      comment.Append((int) pageID);
      comment.Append(" at level ");
      comment.Append((int) MAMViewer->GetLevel());
      MAMViewer->BeginFrame(comment.GetStr());
      MAMViewer->EnableNode(pageID);
         
      // for each entry...
      for (idx = 0; idx < numberOfEntries; idx++) {
         if (currNode->GetEntry(idx).PageID){
            // Add all child nodes all active
            tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
            MAMViewer->LevelUp();
            MAMViewer->SetNode(currNode->GetEntry(idx).PageID, &tmpObj,
                  currNode->GetRadius(idx), this->GetRoot(), 0, true);
            MAMViewer->LevelDown();
         }else{
            // Add objects to the node
            tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
            MAMViewer->SetObject(&tmpObj, this->GetRoot(), true);
         }//end if
      }//end for
      MAMViewer->SetResult(sample, result);
      MAMViewer->EndFrame();
   #endif //__stMAMVIEW__

   for (idx = 0; idx < numberOfEntries; idx++) {
      // use of the triangle inequality to cut a subtree
      if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
          range + currNode->GetRadius(idx)){
         // Rebuild the object
         tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
         // is it a Representative?
         if (currNode->GetEntry(idx).Distance!=0) {
            // No, it is not a representative. Evaluate distance
            distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
         }else{
            distance = distanceRepres;
         }//end if

         if (distance <= range + currNode->GetRadius(idx)){
            // is there a subtree?
            if (!currNode->GetEntry(idx).PageID){
               // No, there is not a subtree. But this object qualifies.
               result->AddPair(tmpObj.Clone(), distance);

               #ifdef __stMAMVIEW__
                  comment.Clear();
                  comment.Append("Object from node");
                  comment.Append((int) pageID);
                  comment.Append(" added. The result has ");
                  comment.Append((int) result->GetNumOfEntries());
                  comment.Append(" object(s) and radius ");
                  comment.Append((double) result->GetMaximumDistance());
                  MAMViewer->BeginFrame(comment.GetStr());
                  MAMViewer->EnableNode(pageID);
                  MAMViewer->SetResult(sample, result);
                  MAMViewer->EndFrame();
               #endif //__stMAMVIEW__

            }else{
               // Yes! This subtree qualified !
               this->RangeQuery(currNode->GetEntry(idx).PageID, result,
                                sample, range, distance);

               #ifdef __stMAMVIEW__
                  comment.Clear();
                  comment.Append("Returning to node ");
                  comment.Append((int) pageID);
                  comment.Append(" at level ");
                  comment.Append((int) MAMViewer->GetLevel());
                  MAMViewer->BeginFrame(comment.GetStr());
                  MAMViewer->EnableNode(pageID);
                  MAMViewer->EndFrame();
               #endif //__stMAMVIEW__

            }//end if
         }//end if
      }//end if
   }//end for

   #ifdef __stMAMVIEW__
      comment.Clear();
      comment.Append("The result after the node ");
      comment.Append((int) pageID);
      comment.Append(" at level ");
      comment.Append((int) MAMViewer->GetLevel());
      comment.Append(" has ");
      comment.Append((int) result->GetNumOfEntries());
      comment.Append(" object(s) and radius ");
      comment.Append((double) result->GetMaximumDistance());
      MAMViewer->BeginFrame(comment.GetStr());
      MAMViewer->SetResult(sample, result);
      MAMViewer->EndFrame();
   #endif //__stMAMVIEW__

   // Free it all
   delete currNode;
   this->myPageManager->ReleasePage(currPage);

   // Visualization support
   #ifdef __stMAMVIEW__
      MAMViewer->LevelDown();
   #endif //__stMAMVIEW__

}//end stDBMTree::RangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDBMTree<ObjectType, EvaluatorType>::NearestQuery(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   ObjectType tmpObj;
   stPage * currPage;
   stDBMNode * currNode;
   stDistance distance, distanceRepres;
   stDistance rangeK = MAXDOUBLE;
   stCount numberOfEntries, idx;
   stCount idxRep;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   tDynamicPriorityQueue * queue;
   #ifdef __stMAMVIEW__
      stMessageString title;
      stMessageString comment;
   #endif //__stMAMVIEW__

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);

   #ifdef __stMAMVIEW__
      MAMViewer->SetQueryInfo(k, 0);
      MAMViewer->SetLevel(0);
      title.Append("DBM-Tree: k-Nearest Neighbor Query with page size ");
      title.Append((int) this->myPageManager->GetMinimumPageSize());
      comment.Append("The k of this nearest neighbor query is ");
      comment.Append((int) k);
      MAMViewer->BeginAnimation(title.GetStr(), comment.GetStr());
   #endif //__stMAMVIEW__

   // Let's search
   if (this->GetRoot() != 0){
      #ifdef __stMAMVIEW__
         pqCurrValue.Parent = -1;
         pqCurrValue.Level = 0;
      #endif //__stMAMVIEW__
      
      // Create the Global Priority Queue.
      queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
      // Rebuild the object
      tmpObj.Unserialize(currNode->GetObject(Header->idxRoot),
                         currNode->GetObjectSize(Header->idxRoot));
      // Evaluate the distance.
      distanceRepres = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

      // Visualization support
      #ifdef __stMAMVIEW__
         comment.Clear();
         comment.Append("Root is the node ");
         comment.Append((int) this->GetRoot());
         comment.Append(" at level ");
         comment.Append(0);
         MAMViewer->SetLevel(1);
         MAMViewer->BeginFrame(comment.GetStr());
         MAMViewer->SetNode(this->GetRoot(), &tmpObj,
                            this->GetTreeRadius(), 0, 0, true);

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            if (currNode->GetEntry(idx).PageID){
               // Add all child nodes all active
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetNode(currNode->GetEntry(idx).PageID, &tmpObj,
                     currNode->GetRadius(idx), this->GetRoot(), 0, true);
            }else{
               // Add objects to the node
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, this->GetRoot(), true);
            }//end if
         }//end for
         MAMViewer->SetResult(sample, result);
         MAMViewer->EndFrame();
      #endif //__stMAMVIEW__

      // Is there any free objects in this node?
      if (currNode->GetNumberOfFreeObjects() > 0){

         // Now do it all free objects.
         for (idx = 0; idx < numberOfEntries; idx++) {
            // tests if this is a subtree.
            if (!currNode->GetEntry(idx).PageID){
               // use of the triangle inequality to cut a subtree
               if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <= rangeK){
                  // Rebuild the object
                  tmpObj.Unserialize(currNode->GetObject(idx),
                                     currNode->GetObjectSize(idx));
	               // is it a Representative?
	               if (currNode->GetEntry(idx).Distance != 0) {
	                  // No, it is not a representative. Evaluate distance
	                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
	               }else{
	                  distance = distanceRepres;
	               }//end if
	
	               if (distance <= rangeK){
	                  // No, there is not a subtree. But this object qualifies.
	                  // Add the object.
	                  result->AddPair(tmpObj.Clone(), distance);
	                  // there is more than k elements?
	                  if (result->GetNumOfEntries() >= k){
	                     //cut if there is more than k elements
	                     result->Cut(k);
	                     //may I use this for performance?
	                     rangeK = result->GetMaximumDistance();
	                  }//end if
	
	                  #ifdef __stMAMVIEW__
	                     comment.Clear();
	                     comment.Append("Object from root node added.");
	                     comment.Append("The result has ");
	                     comment.Append((int) result->GetNumOfEntries());
	                     comment.Append(" object(s) and radius ");
	                     comment.Append((double) result->GetMaximumDistance());
	                     MAMViewer->BeginFrame(comment.GetStr());
	                     MAMViewer->EnableNode(this->GetRoot());
	                     MAMViewer->SetResult(sample, result);
	                     MAMViewer->EndFrame();
	                  #endif //__stMAMVIEW__
	               }//end if
               }//end if
            }//end if
         }//end for

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("The result after the root node ");
            comment.Append((int) 0);
            comment.Append(" at level 0 has ");
            comment.Append((int) result->GetNumOfEntries());
            comment.Append(" object(s) and radius ");
            comment.Append((double) result->GetMaximumDistance());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

      }//end if

      // Now do it for subtrees.
      for (idx = 0; idx < numberOfEntries; idx++) {
         // tests if this is a subtree.
         if (currNode->GetEntry(idx).PageID){
            // use of the triangle inequality to cut a subtree
            if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
                rangeK + currNode->GetRadius(idx)){
               // Rebuild the object
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               // is it a Representative?
               if (idx != idxRep) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= rangeK + currNode->GetRadius(idx)){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
                  pqTmpValue.Radius = currNode->GetRadius(idx);
                  #ifdef __stMAMVIEW__
                     pqTmpValue.Parent = pqCurrValue.Parent;
                     pqTmpValue.Level = pqCurrValue.Level + 1;
                  #endif //__stMAMVIEW__                     
                  // this is a subtree, put in the queue.
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end if
      }//end for

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // If there is something in the queue.
      if (queue->GetSize() > 0){
         this->NearestQuery(result, sample, rangeK, k, queue);
      }//end if

      // Release the Global Priority Queue.
      delete queue;
   }//end if

   // Visualization support
   #ifdef __stMAMVIEW__
      // Add the last frame with the final result
      comment.Clear();
      comment.Append("The final result has ");
      comment.Append((int) result->GetNumOfEntries());
      comment.Append(" object(s) and radius ");
      comment.Append((double) result->GetMaximumDistance());
      MAMViewer->BeginFrame(comment.GetStr());
      MAMViewer->SetResult(sample, result);
      MAMViewer->EndFrame();
      MAMViewer->EndAnimation();
   #endif //__stMAMVIEW__

   return result;
}//end stDBMTree::NearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::NearestQuery(tResult * result,
         ObjectType * sample, stDistance rangeK, stCount k,
         tDynamicPriorityQueue * queue){

   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   bool stop;
   #ifdef __stMAMVIEW__
      stMessageString title;
      stMessageString comment;
   #endif //__stMAMVIEW__

   queue->Get(distance, pqCurrValue);
   distanceRepres = distance;

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();

      // Visualization support
      #ifdef __stMAMVIEW__
         comment.Clear();
         comment.Append("Entering in the node ");
         comment.Append((int) pqCurrValue.PageID);
         comment.Append(" at level ");
         comment.Append((int) MAMViewer->GetLevel());
         MAMViewer->SetLevel(pqCurrValue.Level + 1);
         MAMViewer->BeginFrame(comment.GetStr());
         MAMViewer->EnableNode(pqCurrValue.PageID);

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            if (currNode->GetEntry(idx).PageID){
               // Add all child nodes all active
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetNode(currNode->GetEntry(idx).PageID, &tmpObj,
                     currNode->GetRadius(idx), this->GetRoot(), 0, true);
            }else{
               // Add objects to the node
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, this->GetRoot(), true);
            }//end if
         }//end for
         MAMViewer->SetResult(sample, result);
         MAMViewer->EndFrame();
      #endif //__stMAMVIEW__

      // Is there any free objects in this node?
      if (currNode->GetNumberOfFreeObjects() > 0){

         // Now do it all free objects.
         for (idx = 0; idx < numberOfEntries; idx++) {
            // tests if this is a subtree.
            if (!currNode->GetEntry(idx).PageID){
               // use of the triangle inequality to cut a subtree
               if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <= rangeK){

	               // Rebuild the object.
	               tmpObj.Unserialize(currNode->GetObject(idx),
	                                  currNode->GetObjectSize(idx));
	               // is it a Representative?
	               if (currNode->GetEntry(idx).Distance != 0) {
	                  // No, it is not a representative. Evaluate distance
	                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
	               }else{
	                  distance = distanceRepres;
	               }//end if
	
	               if (distance <= rangeK){
	                  // No, there is not a subtree. But this object qualifies.
	                  // Add the object.
	                  result->AddPair(tmpObj.Clone(), distance);
	                  // there is more than k elements?
	                  if (result->GetNumOfEntries() >= k){
	                     //cut if there is more than k elements
	                     result->Cut(k);
	                     //may I use this for performance?
	                     rangeK = result->GetMaximumDistance();
	                  }//end if
	
	                  #ifdef __stMAMVIEW__
	                     comment.Clear();
	                     comment.Append("Object from node ");
	                     comment.Append((int) pqCurrValue.PageID);
	                     comment.Append(" added. The result has ");
	                     comment.Append((int) result->GetNumOfEntries());
	                     comment.Append(" object(s) and radius ");
	                     comment.Append((double) result->GetMaximumDistance());
	                     MAMViewer->BeginFrame(comment.GetStr());
	                     MAMViewer->EnableNode(pqCurrValue.PageID);
	                     MAMViewer->SetResult(sample, result);
	                     MAMViewer->EndFrame();
	                  #endif //__stMAMVIEW__
	
	               }//end if
               }//end if
            }//end if
         }//end for

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("The result after the node ");
            comment.Append((int) pqCurrValue.PageID);
            comment.Append(" at level ");
            comment.Append((int) pqCurrValue.Level);
            comment.Append(" has ");
            comment.Append((int) result->GetNumOfEntries());
            comment.Append(" object(s) and radius ");
            comment.Append((double) result->GetMaximumDistance());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

      }//end if

      // Now do it for subtrees.
      for (idx = 0; idx < numberOfEntries; idx++) {
         // tests if this is a subtree.
         if (currNode->GetEntry(idx).PageID){
            // use of the triangle inequality to cut a subtree
            if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
                rangeK + currNode->GetRadius(idx)){
               // Rebuild the object
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               // is it a Representative?
               if (currNode->GetEntry(idx).Distance != 0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= rangeK + currNode->GetRadius(idx)){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
                  pqTmpValue.Radius = currNode->GetRadius(idx);
                  #ifdef __stMAMVIEW__
                     pqTmpValue.Parent = pqCurrValue.Parent;
                     pqTmpValue.Level = pqCurrValue.Level + 1;
                  #endif //__stMAMVIEW__                     
                  // this is a subtree, put in the queue.
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end if
      }//end for

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
   
}//end stDBMTree::NearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDBMTree<ObjectType, EvaluatorType>::NearestQueryPriorityQueue(
      ObjectType * sample, stCount k, bool tie){
   tResult * result = new tResult();  // Create result
   ObjectType tmpObj;
   stPage * currPage;
   stDBMNode * currNode;
   stDistance distance, distanceRepres;
   stDistance rangeK = MAXDOUBLE;
   stCount numberOfEntries, idx;
   stCount numberOfFreeObjects;
   stCount idxRep;
   stDistanceIndex * distanceIndex;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   tDynamicPriorityQueue * queue;
   #ifdef __stMAMVIEW__
      stMessageString title;
      stMessageString comment;
   #endif //__stMAMVIEW__

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);

   #ifdef __stMAMVIEW__
      MAMViewer->SetQueryInfo(k, 0);
      MAMViewer->SetLevel(0);
      title.Append("DBM-Tree: k-Nearest Neighbor Query with page size ");
      title.Append((int) this->myPageManager->GetMinimumPageSize());
      comment.Append("The k of this nearest neighbor query is ");
      comment.Append((int) k);
      MAMViewer->BeginAnimation(title.GetStr(), comment.GetStr());
   #endif //__stMAMVIEW__

   // Let's search
   if (this->GetRoot() != 0){
      #ifdef __stMAMVIEW__
         pqCurrValue.Parent = -1;
         pqCurrValue.Level = 0;
      #endif //__stMAMVIEW__
      
      // Create the Global Priority Queue.
      queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
      // Rebuild the object
      tmpObj.Unserialize(currNode->GetObject(Header->idxRoot),
                         currNode->GetObjectSize(Header->idxRoot));
      // Evaluate the distance.
      distanceRepres = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

      numberOfFreeObjects = currNode->GetNumberOfFreeObjects();                      

      // Visualization support
      #ifdef __stMAMVIEW__
         comment.Clear();
         comment.Append("Root is the node ");
         comment.Append((int) this->GetRoot());
         comment.Append(" at level ");
         comment.Append(0);
         MAMViewer->SetLevel(1);
         MAMViewer->BeginFrame(comment.GetStr());
         MAMViewer->SetNode(this->GetRoot(), &tmpObj,
                            this->GetTreeRadius(), 0, 0, true);

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            if (currNode->GetEntry(idx).PageID){
               // Add all child nodes all active
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetNode(currNode->GetEntry(idx).PageID, &tmpObj,
                     currNode->GetRadius(idx), this->GetRoot(), 0, true);
            }else{
               // Add objects to the node
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, this->GetRoot(), true);
            }//end if
         }//end for
         MAMViewer->SetResult(sample, result);
         MAMViewer->EndFrame();
      #endif //__stMAMVIEW__

      // Is there any free objects in this node?
      if (numberOfFreeObjects > 0){

         // Init Map
         distanceIndex = new stDistanceIndex[numberOfFreeObjects];
         numberOfFreeObjects = 0;

         // Now do it all free objects.
         for (idx = 0; idx < numberOfEntries; idx++) {
            // tests if this is a subtree.
            if (!currNode->GetEntry(idx).PageID){
               // use of the triangle inequality to cut a subtree
               if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <= rangeK){
                  distance = fabs(distanceRepres - currNode->GetEntry(idx).Distance);

                  distanceIndex[numberOfFreeObjects].Distance = distance;
                  distanceIndex[numberOfFreeObjects].Index = idx;
                  numberOfFreeObjects++;
               }//end if
            }//end if
         }//end for

         // Sorting by distance...
         sort(distanceIndex, distanceIndex + numberOfFreeObjects);
      
         // Now do it all free objects.
         for (idx = 0; idx < numberOfFreeObjects; idx++) {
            // use of the triangle inequality to cut a subtree
            if (distanceIndex[idx].Distance <= rangeK){
               // Rebuild the object
               tmpObj.Unserialize(currNode->GetObject(distanceIndex[idx].Index),
                                  currNode->GetObjectSize(distanceIndex[idx].Index));
               // is it a Representative?
               if (currNode->GetEntry(distanceIndex[idx].Index).Distance != 0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= rangeK){
                  // No, there is not a subtree. But this object qualifies.
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
                  // there is more than k elements?
                  if (result->GetNumOfEntries() >= k){
                     //cut if there is more than k elements
                     result->Cut(k);
                     //may I use this for performance?
                     rangeK = result->GetMaximumDistance();
                  }//end if

                  #ifdef __stMAMVIEW__
                     comment.Clear();
                     comment.Append("Object from root node added.");
                     comment.Append("The result has ");
                     comment.Append((int) result->GetNumOfEntries());
                     comment.Append(" object(s) and radius ");
                     comment.Append((double) result->GetMaximumDistance());
                     MAMViewer->BeginFrame(comment.GetStr());
                     MAMViewer->EnableNode(this->GetRoot());
                     MAMViewer->SetResult(sample, result);
                     MAMViewer->EndFrame();
                  #endif //__stMAMVIEW__
               }//end if
            }//end if
         }//end for

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("The result after the root node ");
            comment.Append((int) 0);
            comment.Append(" at level 0 has ");
            comment.Append((int) result->GetNumOfEntries());
            comment.Append(" object(s) and radius ");
            comment.Append((double) result->GetMaximumDistance());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

         // And the free object queue.
         delete[] distanceIndex;
      }//end if

      // Now do it for subtrees.
      for (idx = 0; idx < numberOfEntries; idx++) {
         // tests if this is a subtree.
         if (currNode->GetEntry(idx).PageID){
            // use of the triangle inequality to cut a subtree
            if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
                rangeK + currNode->GetRadius(idx)){
               // Rebuild the object
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               // is it a Representative?
               if (idx != idxRep) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= rangeK + currNode->GetRadius(idx)){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
                  pqTmpValue.Radius = currNode->GetRadius(idx);
                  #ifdef __stMAMVIEW__
                     pqTmpValue.Parent = pqCurrValue.Parent;
                     pqTmpValue.Level = pqCurrValue.Level + 1;
                  #endif //__stMAMVIEW__                     
                  // this is a subtree, put in the queue.
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end if
      }//end for

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      // If there is something in the queue.
      if (queue->GetSize() > 0){
         this->NearestQueryPriorityQueue(result, sample, rangeK, k, queue);
      }//end if

      // Release the Global Priority Queue.
      delete queue;
   }//end if

   // Visualization support
   #ifdef __stMAMVIEW__
      // Add the last frame with the final result
      comment.Clear();
      comment.Append("The final result has ");
      comment.Append((int) result->GetNumOfEntries());
      comment.Append(" object(s) and radius ");
      comment.Append((double) result->GetMaximumDistance());
      MAMViewer->BeginFrame(comment.GetStr());
      MAMViewer->SetResult(sample, result);
      MAMViewer->EndFrame();
      MAMViewer->EndAnimation();
   #endif //__stMAMVIEW__

   return result;
}//end stDBMTree::NearestQueryPriorityQueue

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::NearestQueryPriorityQueue(tResult * result,
         ObjectType * sample, stDistance rangeK, stCount k,
         tDynamicPriorityQueue * queue){

   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistanceIndex * distanceIndex;
   stCount numberOfFreeObjects;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   bool stop;
   #ifdef __stMAMVIEW__
      stMessageString title;
      stMessageString comment;
   #endif //__stMAMVIEW__

   queue->Get(distance, pqCurrValue);
   distanceRepres = distance;

   // Init Map only one time.
   distanceIndex = new stDistanceIndex[this->GetMaxOccupation()];

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
      numberOfFreeObjects = 0;

      // Visualization support
      #ifdef __stMAMVIEW__
         comment.Clear();
         comment.Append("Entering in the node ");
         comment.Append((int) pqCurrValue.PageID);
         comment.Append(" at level ");
         comment.Append((int) MAMViewer->GetLevel());
         MAMViewer->SetLevel(pqCurrValue.Level + 1);
         MAMViewer->BeginFrame(comment.GetStr());
         MAMViewer->EnableNode(pqCurrValue.PageID);

         // for each entry...
         for (idx = 0; idx < numberOfEntries; idx++) {
            if (currNode->GetEntry(idx).PageID){
               // Add all child nodes all active
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetNode(currNode->GetEntry(idx).PageID, &tmpObj,
                     currNode->GetRadius(idx), this->GetRoot(), 0, true);
            }else{
               // Add objects to the node
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               MAMViewer->SetObject(&tmpObj, this->GetRoot(), true);
            }//end if
         }//end for
         MAMViewer->SetResult(sample, result);
         MAMViewer->EndFrame();
      #endif //__stMAMVIEW__

      numberOfFreeObjects = currNode->GetNumberOfFreeObjects();
      // Is there any free objects in this node?
      if (numberOfFreeObjects > 0){
         // Reset the number of entries.
         numberOfFreeObjects = 0;

         // Now do it all free objects.
         for (idx = 0; idx < numberOfEntries; idx++) {
            // tests if this is a subtree.
            if (!currNode->GetEntry(idx).PageID){
               // use of the triangle inequality to cut a subtree
               if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <= rangeK){
                  distance = fabs(distanceRepres - currNode->GetEntry(idx).Distance);

                  distanceIndex[numberOfFreeObjects].Distance = distance;
                  distanceIndex[numberOfFreeObjects].Index = idx;
                  numberOfFreeObjects++;
               }//end if
            }//end if
         }//end for

         // Sorting by distance...
         sort(distanceIndex, distanceIndex + numberOfFreeObjects);
      
         // Now do it all free objects.
         for (idx = 0; idx < numberOfFreeObjects; idx++) {
            // use of the triangle inequality to cut a subtree
            if (distanceIndex[idx].Distance <= rangeK){
               // Rebuild the object.
               tmpObj.Unserialize(currNode->GetObject(distanceIndex[idx].Index),
                                  currNode->GetObjectSize(distanceIndex[idx].Index));
               // is it a Representative?
               if (currNode->GetEntry(distanceIndex[idx].Index).Distance != 0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= rangeK){
                  // No, there is not a subtree. But this object qualifies.
                  // Add the object.
                  result->AddPair(tmpObj.Clone(), distance);
                  // there is more than k elements?
                  if (result->GetNumOfEntries() >= k){
                     //cut if there is more than k elements
                     result->Cut(k);
                     //may I use this for performance?
                     rangeK = result->GetMaximumDistance();
                  }//end if

                  #ifdef __stMAMVIEW__
                     comment.Clear();
                     comment.Append("Object from node ");
                     comment.Append((int) pqCurrValue.PageID);
                     comment.Append(" added. The result has ");
                     comment.Append((int) result->GetNumOfEntries());
                     comment.Append(" object(s) and radius ");
                     comment.Append((double) result->GetMaximumDistance());
                     MAMViewer->BeginFrame(comment.GetStr());
                     MAMViewer->EnableNode(pqCurrValue.PageID);
                     MAMViewer->SetResult(sample, result);
                     MAMViewer->EndFrame();
                  #endif //__stMAMVIEW__

               }//end if
            }//end if
         }//end for

         #ifdef __stMAMVIEW__
            comment.Clear();
            comment.Append("The result after the node ");
            comment.Append((int) pqCurrValue.PageID);
            comment.Append(" at level ");
            comment.Append((int) pqCurrValue.Level);
            comment.Append(" has ");
            comment.Append((int) result->GetNumOfEntries());
            comment.Append(" object(s) and radius ");
            comment.Append((double) result->GetMaximumDistance());
            MAMViewer->BeginFrame(comment.GetStr());
            MAMViewer->SetResult(sample, result);
            MAMViewer->EndFrame();
         #endif //__stMAMVIEW__

      }//end if

      // Now do it for subtrees.
      for (idx = 0; idx < numberOfEntries; idx++) {
         // tests if this is a subtree.
         if (currNode->GetEntry(idx).PageID){
            // use of the triangle inequality to cut a subtree
            if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
                rangeK + currNode->GetRadius(idx)){
               // Rebuild the object
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               // is it a Representative?
               if (currNode->GetEntry(idx).Distance != 0) {
                  // No, it is not a representative. Evaluate distance
                  distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= rangeK + currNode->GetRadius(idx)){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
                  pqTmpValue.Radius = currNode->GetRadius(idx);
                  #ifdef __stMAMVIEW__
                     pqTmpValue.Parent = pqCurrValue.Parent;
                     pqTmpValue.Level = pqCurrValue.Level + 1;
                  #endif //__stMAMVIEW__                     
                  // this is a subtree, put in the queue.
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end if
      }//end for

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
   
   // Clean the free object queue.
   delete[] distanceIndex;
}//end stDBMTree::NearestQueryPriorityQueue

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDBMTree<ObjectType, EvaluatorType>::PointQuery(
      ObjectType * sample){

   tResult * result = new tResult();  // Create result
   ObjectType tmpObj;
   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   stDistance distance, distanceRepres;
   stCount numberOfEntries;
   bool find = false;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   tDynamicPriorityQueue * queue;
   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::POINTQUERY);

   // Let's search
   if (this->GetRoot() != 0){
      // Create the Global Priority Queue
      queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);
      
      // Read node...
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
      // Rebuild the object
      tmpObj.Unserialize(currNode->GetObject(Header->idxRoot),
                         currNode->GetObjectSize(Header->idxRoot));
      // Evaluate the distance.
      distanceRepres = this->myMetricEvaluator->GetDistance(&tmpObj, sample);

      // Build global priority queue
      for (idx = 0; idx < numberOfEntries; idx++) {
         // use of the triangle inequality to cut a subtree
         if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
             currNode->GetRadius(idx)){
            // Rebuild the object
            tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
            // is it a Representative?
            if (currNode->GetEntry(idx).Distance != 0) {
               // No, it is not a representative. Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            }else{
               distance = distanceRepres;
            }//end if

            if (distance <= currNode->GetRadius(idx)){
               // tests if this is or not a representative
               if (!currNode->GetEntry(idx).PageID){
                  //test if the object qualify
                  if (distance == 0){
                     // Add the object.
                     result->AddPair(tmpObj.Clone(), distance);
                     // find the element, stop the query!
                     find = true;
                  }//end if
               }else{
                  // this is a subtree, put in the queue.
                  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
                  pqTmpValue.Radius = currNode->GetRadius(idx);
                  // this is a subtree, put in the queue.
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end if
      }//end for
      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
      
      // If the element has not found, call the recursive PointQuery.
      if (!find){
         this->PointQuery(result, sample, queue);
      }//end if

      // Release the Global Priority Queue
      delete queue;
   }//end if

   // return the result
   return result;
}//end stDBMTree::PointQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::PointQuery(
         tResult * result, ObjectType * sample, tDynamicPriorityQueue * queue){

   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres;
   stCount numberOfEntries;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   bool stop;
   bool find = false;

   queue->Get(distance, pqCurrValue);
   distanceRepres = distance;

   while ((pqCurrValue.PageID != 0) && (!find)){
      // Read node...
      currPage = this->myPageManager->GetPage(pqCurrValue.PageID);
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();

      // Build global priority queue
      for (idx = 0; idx < numberOfEntries; idx++) {
         // use of the triangle inequality to cut a subtree
         if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
             currNode->GetRadius(idx)){
            // Rebuild the object
            tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
            // is it a Representative?
            if (currNode->GetEntry(idx).Distance != 0) {
               // No, it is not a representative. Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
            }else{
               distance = distanceRepres;
            }//end if

            if (distance <= currNode->GetRadius(idx)){
               // tests if this is or not a representative
               if (!currNode->GetEntry(idx).PageID){
                  //test if the object qualify
                  if (distance == 0){
                     // Add the object.
                     result->AddPair(tmpObj.Clone(), distance);
                     // find the element, stop the query!
                     find = true;
                  }//end if
               }else{
                  // this is a subtree, put in the queue.
                  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
                  pqTmpValue.Radius = currNode->GetRadius(idx);
                  // this is a subtree, put in the queue.
                  queue->Add(distance, pqTmpValue);
               }//end if
            }//end if
         }//end if
      }//end for

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);

      if (!find){
         // Search... and feed query
         stop = false;
         do{
            if (queue->Get(distance, pqCurrValue)){
               // there is an element in the queue.
               distanceRepres = distance;
               // Break the while.
               stop = true;
            }else{
               // the queue is empty!
               pqCurrValue.PageID = 0;
               // Break the while.
               stop = true;
            }//end if
         }while (!stop);
      }//end if
   }// end while

}//end stDBMTree::PointQuery


//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDBMTree<ObjectType, EvaluatorType>::IncrementalNearestQuery(
      ObjectType * sample, stCount k, bool tie){

   tResult * result = new tResult();  // Create result
   stPage * rootPage;
   stDBMNode * rootNode;
   ObjectType tmpObj;
   stDistance distance;
   stDistance distanceRepres;
   stCount idx;
   stCount numberOfEntries;
   tPGenericHeap * queue = new tPGenericHeap(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE);

   if (this->GetRoot()){
      // Get node
      rootPage = this->myPageManager->GetPage(this->GetRoot());
      rootNode = new stDBMNode(rootPage);
      // Get the number of entries.
      numberOfEntries = rootNode->GetNumberOfEntries();
      // Rebuild the object
      tmpObj.Unserialize(rootNode->GetObject(Header->idxRoot),
                         rootNode->GetObjectSize(Header->idxRoot));
      // Evaluate the distance.
      distanceRepres = this->myMetricEvaluator->GetDistance(&tmpObj, sample);
      // for each entry...
      for (idx = 0; idx < numberOfEntries; idx++) {
         // Rebuild the object
         tmpObj.Unserialize(rootNode->GetObject(idx), rootNode->GetObjectSize(idx));
         // Test if it is a subtree or an object.
         if (rootNode->GetEntry(idx).PageID){
            // Put the Node in the Queue.
            queue->Add(tmpObj.Clone(), rootNode->GetEntry(idx).PageID,
                             0, rootNode->GetEntry(idx).Distance, distanceRepres,
                             rootNode->GetRadius(idx), 0, tPGenericHeap::APPROXIMATENODE);
         }else{
            // Put the Objects in the Queue.
            queue->Add(tmpObj.Clone(), -1,
                             0, rootNode->GetEntry(idx).Distance, distanceRepres,
                             0, 0, tPGenericHeap::APPROXIMATEOBJECT);
         }//end if
      }//end for
      // Call the Incremental.
      IncrementalNearestQuery(result, sample, k, queue);
      // Release the Priority Queue
      delete queue;
      //Free it all
      delete rootNode;
      this->myPageManager->ReleasePage(rootPage);
   }//end if

   // return the result
   return result;

}//end stDBMTree::InitializeIncrementalNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::IncrementalNearestQuery(tResult * result,
         ObjectType * sample, stCount k, tPGenericHeap * queue){
   stPage * currPage;
   stDBMNode * currNode;
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

   while (!stop && queue->Get(object, pageID,
                              distanceQuery, distanceRep, distanceRepQuery,
                              radius, height, type)){

      switch (type){
         case tPGenericHeap::NODE:
            // Read node...
            currPage = this->myPageManager->GetPage(pageID);
            currNode = new stDBMNode(currPage);
            // Get the number of entries.
            numberOfEntries = currNode->GetNumberOfEntries();

            // for each entry put in the global priority queue.
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Rebuild the object
               tmpObj.Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               if (currNode->GetEntry(idx).PageID){
                  // Add it in the queue.
                  queue->Add(tmpObj.Clone(), currNode->GetEntry(idx).PageID,
                                   0, currNode->GetEntry(idx).Distance, distanceQuery,
                                   currNode->GetRadius(idx), height+1, tPGenericHeap::APPROXIMATENODE);
               }else{
                  // Add it in the queue.
                  queue->Add(tmpObj.Clone(), -1,
                                   0, currNode->GetEntry(idx).Distance, distanceQuery,
                                   0, height+1, tPGenericHeap::APPROXIMATEOBJECT);
               }//end if
            }//end for
            //Free it all
            delete currNode;
            this->myPageManager->ReleasePage(currPage);
            break;
         case tPGenericHeap::APPROXIMATENODE :
            distance = this->myMetricEvaluator->GetDistance(object, sample);
            queue->Add(object, pageID,
                             distance, distanceRep, distanceRepQuery,
                             radius, height, tPGenericHeap::NODE);
            break;
         case tPGenericHeap::APPROXIMATEOBJECT :
            distance = this->myMetricEvaluator->GetDistance(object, sample);
            queue->Add(object, -1,
                             distance, 0, 0,
                             0, height, tPGenericHeap::OBJECT);
            break;
         case tPGenericHeap::OBJECT :
            // Add the object.
            result->AddPair(object, distanceQuery);
            // is it reach k elements?
            if (result->GetNumOfEntries() == k){
               stop = true;
            }//end if
            break;
      }//end switch
   }//end do

}//end stDBMTree::IncrementalNearestQuery

//==============================================================================
// End of Queries
//==============================================================================


//==============================================================================
// Begin of Visualization support
//==============================================================================
#ifdef __stMAMVIEW__
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MAMViewInit(){
   tObjectSample * sample;
   int size;

   size = int(float(this->GetNumberOfObjects()) * 0.1);
   if (size < 200){
      size = 200;
   }//end if

   sample = GetSample(size);
   MAMViewer->Init(sample);
   delete sample;
}//end stDBMTree::MAMViewInit


//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stMAMViewObjectSample <ObjectType> * stDBMTree<ObjectType, EvaluatorType>::GetSample(int sampleSize){
   tObjectSample * sample;

   // Create the sample object.
   sample = new tObjectSample(sampleSize, GetNumberOfObjects());

   // Get them!
   GetSampleRecursive(this->GetRoot(), sample);

   return sample;
}//end stDBMTree::GetSample

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::GetSampleRecursive(stPageID pageID,
                           tObjectSample * sample){
   stPage * currPage;
   stDBMNode * currNode;
   ObjectType tmp;
   stCount numberOfEntries;
   stCount i;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = new stDBMNode(currPage);
      // Get the number of entries.
      numberOfEntries = currNode->GetNumberOfEntries();

      // for each entry call GetSampleRecursive
      for (i = 0; i < numberOfEntries; i++) {
         // if the entry is a subtree
         if (currNode->GetEntry(i).PageID){
            // get recursive.
            GetSampleRecursive(currNode->GetEntry(i).PageID, sample);
         }else{
            // otherwise...
            if (sample->MayAdd()){
               // YES! I'll add it now.
               tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));
               sample->Add(tmp.Clone());
            }//end if
         }//end if
      }//end for

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if
}//end stDBMTree::GetSampleRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MAMViewDumpTree(){

   MAMViewer->BeginAnimation("Full tree dump.");
   MAMViewer->BeginFrame(NULL);
   MAMViewDumpTreeRecursive(this->GetRoot(), NULL, 0, 0);
   MAMViewer->EndFrame();
   MAMViewer->EndAnimation();
}//end stDBMTree::MAMViewDumpTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::MAMViewDumpTreeRecursive(stPageID pageID,
            ObjectType * rep, stDistance radius, stPageID parent){
   stPage * currPage;
   stDBMNode * currNode;
   ObjectType tmp;
   stCount numberOfEntries;
   stCount i;

   // Level Up
   MAMViewer->LevelUp();

   // Followinf the tree...
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = new stDBMNode(currPage);

      // Get the number of entries.
      numberOfEntries = currNode->GetNumberOfEntries();

      // Declare this node
      if (rep != NULL){
         MAMViewer->SetNode(pageID, rep, radius, parent, 0, false);
      }//end if

      // Adding all entries of this node.
      for (i = 0; i < numberOfEntries; i++) {
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         if (currNode->GetEntry(i).PageID){
            // Go down...
            MAMViewDumpTreeRecursive(currNode->GetEntry(i).PageID, &tmp,
                                     currNode->GetRadius(i), pageID);
         }else{
            // Declaring an object entry.
            MAMViewer->SetObject(&tmp, pageID, false);
         }//end if
      }//end for

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   // Level Down
   MAMViewer->LevelDown();
}//end stDBMTree::MAMViewDumpTree

#endif //__stMAMVIEW__
//==============================================================================
// End of Visualization support
//==============================================================================

//==============================================================================
// Begin of Tree Info
//==============================================================================
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stTreeInfoResult * stDBMTree<ObjectType, EvaluatorType>::GetTreeInfo(){
   stTreeInformation * info;

   // No cache of information. I think a cahe would be a good idea.
   info = new stTreeInformation(this->GetHeight(), this->GetNumberOfObjects());

   // Let's get the information!
   GetTreeInfoRecursive(this->GetRoot(), 0, info);

   // Optimal tree
   if (info->GetMeanObjectSize() != 0){
       info->CalculateOptimalTreeInfo(int(this->myPageManager->GetMinimumPageSize() /
       info->GetMeanObjectSize()));
   }//end if

   return info;
}//end stDBMTree::GetTreeInfo

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::GetTreeInfoRecursive(stPageID pageID, int level,
      stTreeInformation * info){
   stPage * currPage;
   stDBMNode * currNode;
   stCount i;
   stCount numberOfEntries;
   stCount lv;
   ObjectType tmp;

   // Let's search
   if (pageID != 0){
      // Update node count
      info->UpdateNodeCount(level);

      // Read node...
      currPage = this->myPageManager->GetPage(pageID);

      if (currPage!=NULL){
         // Get the node.
         currNode = new stDBMNode(currPage);
         // Get the number of entries.
         numberOfEntries = currNode->GetNumberOfEntries();
         // Object count
         info->UpdateObjectCount(level, numberOfEntries);

         // Scan all entries
         for (i = 0; i < numberOfEntries; i++){
            if (currNode->GetEntry(i).PageID){
               GetTreeInfoRecursive(currNode->GetEntry(i).PageID, level + 1, info);
            }else{
               // Update other statistics
               info->UpdateMeanObjectSize(currNode->GetObjectSize(i));

               // Compute intersections
               tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));
               // Compute intersections
               ObjectIntersectionsRecursive(this->GetRoot(), &tmp, 0, info);
            }//end if
         }//end for

         // Free it all
         delete currNode;
         this->myPageManager->ReleasePage(currPage);
      }//end if
   }//end if
}//end stDBMTree::GetTreeInfoRecursive

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::ObjectIntersectionsRecursive(stPageID pageID,
      ObjectType * obj, int level, stTreeInformation * info){
   stPage * currPage;
   stDBMNode * currNode;
   stCount numberOfEntries;
   stCount i;
   ObjectType tmp;
   stDistance distance;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      if (currPage != NULL){
         currNode = new stDBMNode(currPage);
         // Get the number of entries.
         numberOfEntries = currNode->GetNumberOfEntries();

         // Scan all entries
         for (i = 0; i < numberOfEntries; i++){
            // Is it a Index node?
            if (currNode->GetEntry(i).PageID) {
               tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));
               distance = this->myMetricEvaluator->GetDistance(&tmp, obj);
               if (distance <= currNode->GetRadius(i)){
                  // Intersection !!!!
                  info->UpdateIntersections(level);
                  ObjectIntersectionsRecursive(currNode->GetEntry(i).PageID,
                        obj, level + 1, info);
               }//end if
            }//end if
         }//end for

         // Free it all
         delete currNode;
         this->myPageManager->ReleasePage(currPage);
      }//end if
   }//end if
}//end stDBMTree::ObjectIntersectionsRecursive
//==============================================================================
// End of Tree Info
//==============================================================================


//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMTree<ObjectType, EvaluatorType>::GetRealObjectsCount(){
   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   stCount numberOfEntries, totalNumberOfEntries;

   totalNumberOfEntries = 0;
   // Let's search
   if (this->GetRoot() != 0){
      // Read the root node
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();

      // Call the RangeQuery
      for (idx = 0; idx < numberOfEntries; idx++) {
         // use of the triangle inequality to cut a subtree
         // tests if this is or not a representative
         if (!currNode->GetEntry(idx).PageID){
            // Add the object.
            totalNumberOfEntries++;
         }else{
            // Yes! This subtree qualified !
            totalNumberOfEntries += this->GetRealObjectsCount(currNode->GetEntry(idx).PageID);
         }//end if
      }//end for
      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   return totalNumberOfEntries;
}//end stDBMTree::GetRealObjectsCount

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMTree<ObjectType, EvaluatorType>::GetRealObjectsCount(stPageID pageID){
   stPage * currPage;
   stDBMNode * currNode;
   stCount idx, numberOfEntries, totalNumberOfEntries;

   totalNumberOfEntries = 0;
   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();

      for (idx = 0; idx < numberOfEntries; idx++) {
         // is there a subtree?
         if (!currNode->GetEntry(idx).PageID){
            // No, there is not a subtree. But this object qualifies.
            totalNumberOfEntries++;
         }else{
            // Yes! This subtree qualified !
            totalNumberOfEntries += this->GetRealObjectsCount(currNode->GetEntry(idx).PageID);
         }//end if
      }//end for

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   return totalNumberOfEntries;
}//end stDBMTree::GetRealObjectsCount

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMTree<ObjectType, EvaluatorType>::GetRealHeight(){
   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   stCount numberOfEntries, maxHeight, temp;

   maxHeight = 0;
   // Let's search
   if (this->GetRoot() != 0){
      // Read the root node
      currPage = this->myPageManager->GetPage(this->GetRoot());
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
      // Call the RangeQuery
      for (idx = 0; idx < numberOfEntries; idx++) {
         // use of the triangle inequality to cut a subtree
         // tests if this is or not a representative
         if (currNode->GetEntry(idx).PageID){
            temp = this->GetRealHeight(currNode->GetEntry(idx).PageID);
            if (temp > maxHeight){
               maxHeight = temp;
            }//end if
         }//end if
      }//end for
      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   return maxHeight + 1;
}//end stDBMTree::GetRealHeight

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stCount stDBMTree<ObjectType, EvaluatorType>::GetRealHeight(stPageID pageID){
   stPage * currPage;
   stDBMNode * currNode;
   stCount idx, numberOfEntries, height, maxHeight, tmp;

   maxHeight = 0;
   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = new stDBMNode(currPage);
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();

      for (idx = 0; idx < numberOfEntries; idx++) {
         // is there a subtree?
         if (currNode->GetEntry(idx).PageID){
            // Yes! This subtree qualified !
            tmp = this->GetRealHeight(currNode->GetEntry(idx).PageID);
            if (tmp > maxHeight){
               maxHeight = tmp;
            }//end if
         }//end if
      }//end for

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   return maxHeight + 1;
}//end stDBMTree::GetRealHeight

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDBMTree<ObjectType, EvaluatorType>::Consistency(){
   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   stCount numberOfEntries;
   stDistance distance, radius;
   ObjectType repObj, subRep, tmpObj;
   bool result = true;

   // Let's search
   if (this->GetRoot() == 0){
      // Problem!
      result = false;
   }else{
      // Read the root node
      currPage = this->myPageManager->GetPage(this->GetRoot());
      // Test the root pageID.
      if (currPage == NULL){
         #ifdef __stPRINTMSG__
            cout << "\nInvalid pageID in root!";
         #endif //__stPRINTMSG__
         // Problem!
         result = false;
      }else{
         // Get the node.
         currNode = new stDBMNode(currPage);
         // Get the number of entries
         numberOfEntries = currNode->GetNumberOfEntries();
         // Test if there is a representative.
         if ((Header->idxRoot < 0) ||
             (Header->idxRoot >= currNode->GetNumberOfEntries())){
            #ifdef __stPRINTMSG__
               cout << "\nThere is not a global representative.";
            #endif //__stPRINTMSG__
            // Problem!
            result = false;
         }else{
            result = true;
            // Get the global representative.
            repObj.Unserialize(currNode->GetObject(Header->idxRoot),
                               currNode->GetObjectSize(Header->idxRoot));
            // Check the tree radius.
            if (Header->TreeRadius != currNode->GetMinimumRadius()){
               #ifdef __stPRINTMSG__
                  cout << "\nThe Tree Radius is wrong. Header: " << Header->TreeRadius
                       << " Measured: " << currNode->GetMinimumRadius() << ".";
               #endif //__stPRINTMSG__
               // Problem!
               result = false;
            }//end if
            // Check the field entries.
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Test the subtree.
               if (currNode->GetEntry(idx).PageID){
                  // Get the subtree representative.
                  subRep.Unserialize(currNode->GetObject(idx),
                                     currNode->GetObjectSize(idx));
                  // Call it recursively.
                  result = result && this->Consistency(currNode->GetEntry(idx).PageID,
                                                       &subRep, radius);
                  // Test the subtree radius.
                  if (radius != currNode->GetRadius(idx)){
                     #ifdef __stPRINTMSG__
                        cout << "\nThere is a radius problem with entry " << idx
                             << " in pageID: " << currNode->GetEntry(idx).PageID
                             << " radius " << currNode->GetRadius(idx)
                             << " measured " << radius << ".";
                     #endif //__stPRINTMSG__
                     // Problem!
                     result = false;
                  }//end if
               }//end if
               // Calculate the distance.
               if (idx != (stCount )Header->idxRoot){
                  tmpObj.Unserialize(currNode->GetObject(idx),
                                     currNode->GetObjectSize(idx));
                  // Evaluate it.
                  distance = this->myMetricEvaluator->GetDistance(&repObj, &tmpObj);
               }else{
                  // The distance is already calculated.
                  distance = 0;
               }//end if
               // Test if the distances to the representative are ok.
               if (distance != currNode->GetEntry(idx).Distance){
                  #ifdef __stPRINTMSG__
                     cout << "\nThere is a distance problem with entry " << idx
                          << " in pageID " << currPage->GetPageID()
                          << " distance: " << currNode->GetEntry(idx).Distance
                          << " measured " << distance << ".";
                  #endif //__stPRINTMSG__
                  // Problem!
                  result = false;
               }//end if
            }//end for
            delete currNode;
         }//end if
         // Delete the resources.
         this->myPageManager->ReleasePage(currPage);
      }//end if
   }//end if

   return result;
}//end stDBMTree::Consistency

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDBMTree<ObjectType, EvaluatorType>::Consistency(stPageID pageID,
                                     ObjectType * repObj, stDistance & radius){
   stCount idx;
   stPage * currPage;
   stDBMNode * currNode;
   stCount numberOfEntries;
   stDistance distance;
   ObjectType subRep, localRep, tmpObj;
   int idxRep;
   radius = MAXDOUBLE;
   bool result = true;

   // Let's search
   if (pageID == 0){
      // Problem!
      result = false;
   }else{
      // Read the root node
      currPage = this->myPageManager->GetPage(pageID);
      // Test the pageID consistency.
      if (currPage == NULL){
         #ifdef __stPRINTMSG__
            cout << "\nInvalid pageID in " << pageID << ".";
         #endif //__stPRINTMSG__
         // Problem!
         result = false;
      }else{
         // Get the node.
         currNode = new stDBMNode(currPage);
         // Get the number of entries
         numberOfEntries = currNode->GetNumberOfEntries();
         // Get the representative entry of this node.
         idxRep = currNode->GetRepresentativeIndex();
         // Test if there is a representative.
         if ((idxRep < 0) || (idxRep >= currNode->GetNumberOfEntries())){
            #ifdef __stPRINTMSG__
               cout << "\nThere is not a representative entry in pageID "
                    << pageID << ".";
            #endif //__stPRINTMSG__
            // Problem!
            result = false;
         }else{
            // Get the representative.
            localRep.Unserialize(currNode->GetObject(idxRep),
                                 currNode->GetObjectSize(idxRep));
            // Check the field entries.
            result = true;
            // Test all entries in this node.
            for (idx = 0; idx < numberOfEntries; idx++) {
               // Get the subtree representative.
               subRep.Unserialize(currNode->GetObject(idx),
                                  currNode->GetObjectSize(idx));
               // Test the subtree.
               if (currNode->GetEntry(idx).PageID){
                  result = result && this->Consistency(currNode->GetEntry(idx).PageID,
                                                       &subRep, radius);
                  // Test the subtree radius with the local field.
                  if (radius != currNode->GetRadius(idx)){
                     #ifdef __stPRINTMSG__
                        cout << "\nThere is a radius problem with entry " << idx
                             << " in pageID: " << currNode->GetEntry(idx).PageID
                             << " radius " << currNode->GetRadius(idx)
                             << " measured " << radius << ".";
                     #endif //__stPRINTMSG__
                     // Problem!
                     result = false;
                  }//end if
               }//end if
               // Calculate the distance.
               if (idx != (stCount )idxRep){
                  tmpObj.Unserialize(currNode->GetObject(idx),
                                     currNode->GetObjectSize(idx));
                  // Evaluate it!
                  distance = this->myMetricEvaluator->GetDistance(repObj, &tmpObj);
               }else{
                  // The distance is already calculated.
                  distance = 0;
               }//end if
               // Test the distance for every entry.
               if (distance != currNode->GetEntry(idx).Distance){
                  #ifdef __stPRINTMSG__
                     cout << "\nThere is a distance problem in entry " << idx
                          << " in pageID " << currPage->GetPageID()
                          << " distance: " << currNode->GetEntry(idx).Distance
                          << " measured " << distance << ".";
                  #endif //__stPRINTMSG__
                  // Problem!
                  result = false;
               }//end if
            }//end for
            // Set the radius for the upper levels.
            radius = currNode->GetMinimumRadius();
            // Test if the representative is the same to the upper level.
            if (!localRep.IsEqual(repObj)){
               #ifdef __stPRINTMSG__
                  cout << "\nThere is a problem in the representative entry " << idxRep
                       << " in pageID " << currPage->GetPageID()
                       << ". The representative is not the same of the upper level.";
               #endif //__stPRINTMSG__
               // Problem!
               result = false;
            }//end if
            delete currNode;
         }//end if
         // Delete the resources.
         this->myPageManager->ReleasePage(currPage);
      }//end if
   }//end if

   return result;
}//end stDBMTree::Consistency

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::DeleteUniqueNodes(){
   // Start the algorith.
   this->DeleteUniqueNodes(this->GetRoot());
}//end stDBMTree::DeleteUniqueNodes

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stPageID stDBMTree<ObjectType, EvaluatorType>::DeleteUniqueNodes(stPageID pageID){
   stPage * currPage;
   stDBMNode * currNode;
   stPageID retPageID;
   stCount numberOfEntries;
   stCount idx;

   retPageID = 0;
   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = new stDBMNode(currPage);

      // If there are subtrees.
      if (currNode->GetNumberOfEntries() == 1){
         // There is only one subtree.
         if (currNode->GetEntry(0).PageID){
            // Get the pageID.
            retPageID = currNode->GetEntry(0).PageID;
            #ifdef __stPRINTMSG__
               cout << "Node " << retPageID << " is no more!\n";
            #endif //__stPRINTMSG__
            // Dispose the node.
            delete currNode;
            DisposePage(currPage);
         }else{
            // Report that it is impossible to dispose this page.
            retPageID = 0;
            // Free it all
            delete currNode;
            this->myPageManager->ReleasePage(currPage);
         }//end if
      }else{
         // Move on...
         for (idx = 0; idx < currNode->GetNumberOfEntries(); idx++){
            // If this entry is a subtree.
            if (currNode->GetEntry(idx).PageID){
               // Call the DeleteUniqueNodes for this subtree.
               retPageID = DeleteUniqueNodes(currNode->GetEntry(idx).PageID);
               // Test if a page was disposed.
               if (retPageID){
                  // Yes! update the new pageID.
                  currNode->GetEntry(idx).PageID = retPageID;
               }//end if
            }//end if
         }//end for
         // Write me.
         this->myPageManager->WritePage(currPage);
         // Free it all
         delete currNode;
         this->myPageManager->ReleasePage(currPage);
         // Report the action.
         retPageID = 0;
      }//end if
   }else{
      // This tree is corrupted or is empty.
      throw logic_error("The given tree is corrupted or empty.");
   }//end if

   // Return the pageID.
   return retPageID;   
}//end stDBMTree::DeleteUniqueNodes

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::Optimize(){
   // Start the Slim-Down algorith.
   Header->TreeRadius = this->ShrinkRecursive(this->GetRoot());
   // Notify the Header update!
   HeaderUpdate = true;
}//end stDBMTree::Optimize

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDistance stDBMTree<ObjectType, EvaluatorType>::ShrinkRecursive(stPageID pageID){
   stPage * currPage;
   stDBMNode * currNode;
   stDistance radius;
   stCount numberOfEntries;
   stCount idx;
   bool callRing;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = new stDBMNode(currPage);
      // Get the radius.
      radius = currNode->GetMinimumRadius();
      
      // Are there subtrees?
      if (currNode->GetNumberOfEntries() != currNode->GetNumberOfFreeObjects()){
         // Move on...
         for (idx = 0; idx < currNode->GetNumberOfEntries(); idx++){
            // If this entry is a subtree.
            if (currNode->GetEntry(idx).PageID){
               // Call the ShrinkRecursive for this subtree.
               currNode->SetRadius(idx, this->ShrinkRecursive(currNode->GetEntry(idx).PageID));
            }//end if
         }//end for

         // If there is only one subtree.
         callRing = (currNode->GetNumberOfEntries() == (currNode->GetNumberOfFreeObjects() + 1));

         // Write me and get the garbage.
         this->myPageManager->WritePage(currPage);
         delete currNode;
         this->myPageManager->ReleasePage(currPage);

         // Call the Shrink for this node.
         if (callRing){
            // Do the Shrink for the ring.
            this->ShrinkRing(pageID);
         }else{
            // Do the normal task.
            this->Shrink(pageID);
         }//end if

         // Read node again.
         currPage = this->myPageManager->GetPage(pageID);
         currNode = new stDBMNode(currPage);

         if (pageID == this->GetRoot()){
            Header->idxRoot = currNode->GetRepresentativeIndex();
         }//end if

         // Move on again...
         for (idx = 0; idx < currNode->GetNumberOfEntries(); idx++){
            // If this entry is a subtree.
            if (currNode->GetEntry(idx).PageID){
               // Call the ShrinkRecursive for this subtree.
               currNode->SetRadius(idx, this->ShrinkRecursive(currNode->GetEntry(idx).PageID));
            }//end if
         }//end for
         // Update my radius.
         radius = currNode->GetMinimumRadius();
         // Write me and get the garbage.
         this->myPageManager->WritePage(currPage);
      }//end if

      // Clean.
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
      // Return the final radius.
      return radius;
   }else{
      // This tree is corrupted or is empty.
      throw logic_error("The given tree is corrupted or empty.");
   }//end if
}//end stDBMTree::ShrinkRecursive

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::Shrink(stPageID pageID){
   stCount nodeCount, numberOfEntries, idx, i;
   tDBMShrinkNode ** memNodes;
   stPage * currPage;
   stPage * tmpPage;
   stDBMNode * currNode;
   stDBMNode * tmpNode;
   ObjectType * tmpObj;
   stCount maxSwaps;
   int idxRep;

   // Let's search
   if (pageID != 0){
      // Read node...
      currPage = this->myPageManager->GetPage(pageID);
      currNode = new stDBMNode(currPage);
      // Get the representative index.
      idxRep = currNode->GetRepresentativeIndex();
      // Get the numberOfEntries.
      numberOfEntries = currNode->GetNumberOfEntries();
      // Get the node count.
      nodeCount = numberOfEntries - 1;

      // Create all stDBMShrinkNodes
      memNodes = new tDBMShrinkNode * [nodeCount];
      maxSwaps = 0;
      i = 0;
      for (idx = 0; idx < numberOfEntries; idx++){
         // Is it a representative entry?
         if (idxRep != idx){
            // No!
            if (currNode->GetEntry(idx).PageID){
               // This entry is a subtree.
               // Read a page.
               tmpPage = this->myPageManager->GetPage(currNode->GetEntry(idx).PageID);
               tmpNode = new stDBMNode(tmpPage);
               // Update maxSwaps
               maxSwaps += tmpNode->GetNumberOfEntries();
               // Assemble memory version
               memNodes[i] = new tDBMShrinkNode(tmpNode);
               i++;
            }else{
               // This entry is an object.
               // Update maxSwaps
               maxSwaps++;
               // Get the object.
               tmpObj = new ObjectType();
               tmpObj->Unserialize(currNode->GetObject(idx), currNode->GetObjectSize(idx));
               // Assemble memory version
               memNodes[i] = new tDBMShrinkNode(tmpObj);
               i++;
            }//end if
         }//end if
      }//end for
      maxSwaps *= 3;

      // Execute the local Shrink
      this->LocalShrink(memNodes, nodeCount, maxSwaps);

      // Rebuild nodes and write them. Of course, the empty ones will be disposed.
      idx = 0;
      for (i = 0; i < nodeCount; i++){
         // Align the indexes.
         if (idx == currNode->GetRepresentativeIndex()){
            idx++;
         }//end if
         // Dispose memory version
         if (memNodes[i]->GetNumberOfEntries() != 0){
            // if it is a subtree.
            if (currNode->GetEntry(idx).PageID){
               tmpNode = memNodes[i]->ReleaseNode();
               delete memNodes[i];
               // Update entry
               currNode->SetRadius(idx, tmpNode->GetMinimumRadius());
               #ifdef __stDBMNENTRIES__
                  currNode->SetNEntries(idx, tmpNode->GetTotalNumberOfEntries());
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  currNode->SetHeight(idx, tmpNode->GetHeight());
               #endif //__stDBMHEIGHT__
               // Write back
               tmpPage = tmpNode->GetPage();
               this->myPageManager->WritePage(tmpPage);
               delete tmpNode;
               this->myPageManager->ReleasePage(tmpPage);
            }else{
               memNodes[i]->Release();
               delete memNodes[i];
            }//end if
            idx++;
         }else{
            // There is no entries.
            if (currNode->GetEntry(idx).PageID){
               // Empty node.
               tmpNode = memNodes[i]->ReleaseNode();
               delete memNodes[i];
               // Remove entry.
               currNode->RemoveEntry(idx);

               // Dispose empty node.
               tmpPage = tmpNode->GetPage();
               delete tmpNode;
               DisposePage(tmpPage);
            }else{
               // Empty entry.
               memNodes[i]->Release();
               delete memNodes[i];
               // Remove entry.
               currNode->RemoveEntry(idx);
            }//end if
         }//end if
      }//end for
      // Delete the array of memNodes points.
      delete[] memNodes;

      // Write me and get the garbage.
      this->myPageManager->WritePage(currPage);
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }else{
      // This tree is corrupted or is empty.
      throw logic_error("The given tree is corrupted or empty.");
   }//end if
}//end stDBMTree::Shrink

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::LocalShrink(
      stDBMTree<ObjectType, EvaluatorType>::tDBMShrinkNode ** memNodes,
      stCount nodeCount, stCount maxSwaps){
   int src, dst, i;
   stCount localSwapCount, swapCount = 0;
   stDistance minDist, tmpDist, radius;
   stPageID pageID;
   bool stop;
   #ifdef __stDBMNENTRIES__
      stCount nEntries;
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      stByte height;
   #endif //__stDBMHEIGHT__

   // main loop
   stop = false;
   while (!stop){
      // Try to swap them
      localSwapCount = 0;
      for (src = 0; src < nodeCount; src++){
         if (memNodes[src]->GetNumberOfEntries() > 0){
            // Look for the target...
            dst = -1;
            minDist = MAXDOUBLE;
            for (i = 0; i < nodeCount; i++){
               if (i != src){
                  if (this->ShrinkCanSwap(memNodes[src], memNodes[i], tmpDist)){
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
               pageID = memNodes[src]->LastPageID();
               radius = memNodes[src]->LastRadius();
               #ifdef __stDBMNENTRIES__
                  nEntries = memNodes[src]->LastNEntries();
               #endif //__stDBMNENTRIES__
               #ifdef __stDBMHEIGHT__
                  height = memNodes[src]->LastHeight();
               #endif //__stDBMHEIGHT__
               // Add the entry in dst.
               memNodes[dst]->Add(memNodes[src]->PopObject(), minDist, pageID, radius
                                  #ifdef __stDBMNENTRIES__
                                     , nEntries
                                  #endif //__stDBMNENTRIES__
                                  #ifdef __stDBMHEIGHT__
                                     , height
                                  #endif //__stDBMHEIGHT__
                                  );
            }//end if
         }//end if
      }//end for
      // Stop condition
      swapCount += localSwapCount;
      stop = (swapCount > maxSwaps) || (localSwapCount == 0);
   }//end while
}//end stDBMTree::LocalShrink

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDBMTree<ObjectType, EvaluatorType>::ShrinkCanSwap(
      stDBMTree<ObjectType, EvaluatorType>::tDBMShrinkNode * src,
      stDBMTree<ObjectType, EvaluatorType>::tDBMShrinkNode * dst,
      stDistance & distance){
   // Check to see if destination is empty
   if (dst->GetNumberOfEntries() == 0){
      return false;
   }//end if /**/

   // Calculate the distance between src's last object and dst's representative
   distance = this->myMetricEvaluator->GetDistance(src->LastObject(), dst->RepObject());
   // Test distances and occupation
   if (distance + src->LastRadius() <= dst->GetMinimumRadius()){
      if (dst->CanAdd(src->LastObject(), src->LastPageID())){
         // It will fit.
         return true;
      }else{
         // It will not fit!
         return false;
      }//end if
   }else{
      // dst does not cover.
      return false;
   }//end if
}//end stDBMTree::ShrinkCanSwap

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::ShrinkRing(stPageID pageID){

   tDBMShrinkNode * memNodesLevel1, * memNodesLevel2;
   stPage * pageLevel1, * pageLevel2;
   stDBMNode * nodeLevel1, * nodeLevel2;
   int idxRep1;

   // Let's search
   if (pageID != 0){
      // Read node...
      pageLevel1 = this->myPageManager->GetPage(pageID);
      nodeLevel1 = new stDBMNode(pageLevel1);
      // Get the representative index.
      idxRep1 = nodeLevel1->GetRepresentativeIndex();

      // The representative is the subtree.
      if (nodeLevel1->GetEntry(idxRep1).PageID){
         // Read other node...
         pageLevel2 = this->myPageManager->GetPage(nodeLevel1->GetEntry(idxRep1).PageID);
         nodeLevel2 = new stDBMNode(pageLevel2);

         // Assemble memory version
         memNodesLevel1 = new tDBMShrinkNode(nodeLevel1);
         memNodesLevel2 = new tDBMShrinkNode(nodeLevel2);

         // Execute the local Shrink2
         this->LocalShrinkRing(memNodesLevel1, memNodesLevel2);

         // Dispose memory version
         nodeLevel2 = memNodesLevel2->ReleaseNode();
         nodeLevel1 = memNodesLevel1->ReleaseNode();
         delete memNodesLevel2;
         delete memNodesLevel1;
         // Update the index of representative.
         idxRep1 = nodeLevel1->GetRepresentativeIndex();
         // Update entry
         nodeLevel1->SetRadius(idxRep1, nodeLevel2->GetMinimumRadius());
         #ifdef __stDBMNENTRIES__
            nodeLevel1->SetNEntries(idxRep1, nodeLevel2->GetTotalNumberOfEntries());
         #endif //__stDBMNENTRIES__
         #ifdef __stDBMHEIGHT__
            nodeLevel1->SetHeight(idxRep1, nodeLevel2->GetHeight());
         #endif //__stDBMHEIGHT__

         this->myPageManager->WritePage(pageLevel2);
         delete nodeLevel2;
         this->myPageManager->ReleasePage(pageLevel2);
         this->myPageManager->WritePage(pageLevel1);
         delete nodeLevel1;
         this->myPageManager->ReleasePage(pageLevel1);

      }else{
         delete nodeLevel1;
         this->myPageManager->ReleasePage(pageLevel1);
      } //end if
   }else{
      // This tree is corrupted or is empty.
      throw logic_error("The given tree is corrupted or empty.");
   }//end if
}//end stDBMTree::ShrinkRing

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stDBMTree<ObjectType, EvaluatorType>::LocalShrinkRing(
      stDBMTree<ObjectType, EvaluatorType>::tDBMShrinkNode * memNodeLevel1,
      stDBMTree<ObjectType, EvaluatorType>::tDBMShrinkNode * memNodeLevel2){
   bool stop = false;
   stDistance distance;

   while (!stop){
      // Test if memNodeLevel1 has more than one entry.
      if (memNodeLevel1->GetNumberOfEntries() > 1){
         // Is there space to store?
         if (memNodeLevel2->CanAdd(memNodeLevel1->ObjectAt(1), 0)){
            // Yes, swap it!
            distance = memNodeLevel1->DistanceAt(1);
            memNodeLevel2->Add(memNodeLevel1->Remove(1), distance, 0, 0
                               #ifdef __stDBMNENTRIES__
                                  , 0
                               #endif //__stDBMNENTRIES__
                               #ifdef __stDBMHEIGHT__
                                  , 0
                               #endif //__stDBMHEIGHT__
                               );
         }else{
            stop = true;
         }//end if
      }else{
         stop = true;
      }//end if
   }//end while
}//end stDBMTree::LocalShrinkRing

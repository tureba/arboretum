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
* This file implements the stDBMMemNode class.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2003 GBDI-ICMC-USP

//==============================================================================
// Class stDBMMemNode
//------------------------------------------------------------------------------
template <class ObjectType>
stDBMMemNode< ObjectType >::stDBMMemNode(stSize pageSize, stCount numberOfEntries){
   // Get the information to be ajust.
   this->NumEntries = 0;
   this->Capacity = numberOfEntries;
   this->MaximumSize = pageSize;
   this->UsedSize = stDBMNode::GetGlobalOverhead();
   // allocate new space.
   this->Entries = new stDBMMemNodeEntry[this->Capacity * sizeof(stDBMMemNodeEntry)];
}//end stDBMMemNode

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMMemNode< ObjectType >::RemoveAll(){
   stCount idx;
   // Get the information to be ajust.
   this->UsedSize = stDBMNode::GetGlobalOverhead();

   for (idx = 0; idx < this->NumEntries; idx++){
      this->Entries[idx].Object = NULL;
   }//end for
   this->NumEntries = 0;
}//end RemoveAll

//------------------------------------------------------------------------------
template <class ObjectType>
int stDBMMemNode< ObjectType >::AddEntry(stSize size, ObjectType * object,
      stPageID subTree){
   stSize entrySize;

   // Does it fit ?
   entrySize = size + sizeof(stDBMNode::stDBMEntry);

   // Is this a subtree entry?
   if (subTree){
      // Add the radius size.
      entrySize += sizeof(stDistance);
      #ifdef __stDBMNENTRIES__
         entrySize += sizeof(stCount);
      #endif //__stDBMNENTRIES__
   }//end if

   if (entrySize + this->UsedSize > this->MaximumSize){
      // No, it doesn't.
      return -1;
   }//end if

   // if there is free entry to store the new entry.
   if (this->Capacity == this->NumEntries){
      // resize the entries.
      this->Resize();
   }//end if
   
   // add the new entry in the right position.
   Entries[this->NumEntries].PageID = subTree;
   Entries[this->NumEntries].Object = object;

   // Update # of Entries
   this->NumEntries++; // One more!
   // Update the usedSize
   this->UsedSize += entrySize;
   // return the inserted position.
   return this->NumEntries-1;
}//end AddEntry

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMMemNode< ObjectType >::RemoveEntry(stCount idx){
   #ifdef __stDEBUG__
      if (idx >= this->NumEntries){
         throw invalid_argument("idx value is out of range.");
      }//end if
   #endif //__stDEBUG__

   // Update the usedSize
   this->UsedSize -= (Entries[idx].Object->GetSerializedSize() +
                      stDBMNode::GetEntryOverhead());

   // Is this a subtree entry?
   if (Entries[idx].PageID){
      // The radius space.
      this->UsedSize -= sizeof(stDistance);
      #ifdef __stDBMNENTRIES__
         this->UsedSize -= sizeof(stCount);
      #endif //__stDBMNENTRIES__
   }//end if

   // Lets move the data, according to idx.
   while (idx < (this->NumEntries - 1)){
      Entries[idx].Object = Entries[idx+1].Object;
      Entries[idx].PageID = Entries[idx+1].PageID;
      Entries[idx].Distance = Entries[idx+1].Distance;
      Entries[idx].Height = Entries[idx+1].Height;
      Entries[idx].NEntries = Entries[idx+1].NEntries;
      Entries[idx].Radius = Entries[idx+1].Radius;
      idx++;
   }//end while

   // Update # of Entries
   this->NumEntries--; // One less!
}//end RemoveEntry

//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stDBMMemNode< ObjectType >::GetMinimumRadius(){
   stCount i;
   stDistance maxDistance = 0;
   stDistance distance;

   for (i = 0; i < this->NumEntries; i++){
      distance = GetEntry(i).Distance;
      // is this a representative subtree?
      if (GetEntry(i).PageID){
         // Yes, add the Radius of this subtree.
         distance += GetRadius(i);
      }//end if
      if (distance > maxDistance){
         maxDistance = distance;
      }//end if
   }//end for

   return maxDistance;
}//end GetMinimumRadius

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMMemNode< ObjectType >::Resize(stCount incSize){
   stDBMMemNodeEntry * tmpEntries;

   // Is incSize correct?
   // allocate new space.
   tmpEntries = new stDBMMemNodeEntry[(this->NumEntries + incSize) * sizeof(stDBMMemNodeEntry)];
   // copy the entries.
   memcpy(tmpEntries, this->Entries, this->NumEntries * sizeof(stDBMMemNodeEntry));
   // delete the old data.
   delete this->Entries;
   // ajust the pointers.
   this->Entries = tmpEntries;
   this->Capacity += incSize;
}//end Resize



//==============================================================================
// Class stDBMShrinkNode
//------------------------------------------------------------------------------
template <class ObjectType>
stDBMShrinkNode< ObjectType >::stDBMShrinkNode(stDBMNode * node){
   stCount idx, numberOfEntries;
   ObjectType * obj;

   numberOfEntries = node->GetNumberOfEntries();
   // Get the information to be ajust.
   this->numEntries = 0;
   this->capacity = numberOfEntries;
   this->maximumSize = (node->GetPage())->GetPageSize();
   this->usedSize = stDBMNode::GetGlobalOverhead();
   this->srcNode = node;
   // Allocate memory for new entries.
   this->Entries = new stDBMShrinkNodeEntry[numberOfEntries * sizeof(stDBMShrinkNodeEntry)];

   // insert the entries of leafNode.
   for (idx = 0; idx < numberOfEntries; idx++){
      obj = new ObjectType();
      // Get the first object in node.
      obj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
      // Add data.
      this->Add(obj, node->GetEntry(idx).Distance,
                node->GetEntry(idx).PageID, node->GetRadius(idx)
                #ifdef __stDBMNENTRIES__
                   , node->GetNEntries(idx)
                #endif //__stDBMNENTRIES__
                #ifdef __stDBMHEIGHT__
                   , node->GetHeight
                #endif //__stDBMHEIGHT__
                );
   }//end for

   // remove the entry from the node.
   srcNode->RemoveAll();
}//end stDBMShrinkNode

//------------------------------------------------------------------------------
template <class ObjectType>
stDBMShrinkNode< ObjectType >::stDBMShrinkNode(ObjectType * object){
   // Get the information to be ajust.
   this->capacity = 1;
   this->maximumSize = object->GetSerializedSize() + stDBMNode::GetEntryOverhead();
   this->srcNode = NULL;
   // Allocate memory for new entries.
   this->Entries = new stDBMShrinkNodeEntry[sizeof(stDBMShrinkNodeEntry)];

   // Add the new entry in the first position.
   Entries[0].Object = object;
   Entries[0].Distance = 0;
   Entries[0].PageID = 0;
   Entries[0].Radius = 0;
   #ifdef __stDBMNENTRIES__
      Entries[0].NEntries = 0;
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      Entries[0].Height = 0;
   #endif //__stDBMHEIGHT__
   // Update the information.
   this->numEntries = this->capacity;
   this->usedSize = this->maximumSize;
}//end stDBMShrinkNode

//------------------------------------------------------------------------------
template <class ObjectType>
stDBMNode * stDBMShrinkNode< ObjectType >::ReleaseNode(){
   int idx;
   int insertIdx;
   ObjectType * obj;

   for (idx = 0; idx < this->numEntries; idx++){
      // Get a object in idx.
      obj = this->ObjectAt(idx);
      // insert this entry in srcNode.
      insertIdx = srcNode->AddEntry(obj->GetSerializedSize(), 
                                    obj->Serialize(), 
                                    this->PageIDAt(idx));

      // if there is some problem in insertion.
      #ifdef __stDEBUG__
         if (insertIdx < 0){
            throw invalid_argument("The srcNode does not have enough space to store objects.");
         }//end if
      #endif //__stDEBUG__
      // Fill entry's fields
      srcNode->GetEntry(insertIdx).Distance = this->DistanceAt(idx);
      srcNode->SetRadius(insertIdx, this->RadiusAt(idx));
      #ifdef __stDBMNENTRIES__
         srcNode->SetHeight(insertIdx, this->HeightAt(idx));
      #endif //__stDBMNENTRIES__
      #ifdef __stDBMHEIGHT__
         srcNode->SetNEntries(insertIdx, this->NEntriesAt(idx));
      #endif //__stDBMHEIGHT__
   }//end for

   // release the resources.
   for (idx = this->numEntries-1; idx >= 0; idx--){
      delete this->ObjectAt(idx);
   }//end for
   delete[] Entries;
   
   // update fields.
   this->numEntries = 0;
   this->usedSize = stDBMNode::GetGlobalOverhead();
   // return the node
   return srcNode;
}//end ReleaseNode

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMShrinkNode< ObjectType >::Release(){
   if (this->numEntries > 0){
      delete this->ObjectAt(0);
   }//end if
   delete[] Entries;

   // update fields.
   this->numEntries = 0;
   this->usedSize = stDBMNode::GetGlobalOverhead();
}//end Release

//------------------------------------------------------------------------------
template <class ObjectType>
bool stDBMShrinkNode< ObjectType >::Add(ObjectType * obj, stDistance distance,
                                          stPageID pageID, stDistance radius
                                          #ifdef __stDBMNENTRIES__
                                             , stCount nEntries
                                          #endif //__stDBMNENTRIES__
                                          #ifdef __stDBMHEIGHT__
                                             , stByte height
                                          #endif //__stDBMHEIGHT__
                                          ){

   int insertIdx, idx;

   #ifdef __stDEBUG__
      if (obj == NULL){
         throw invalid_argument("The object is NULL.");
      }//end if
   #endif //__stDEBUG__

   // Does it fit?
   if (!this->CanAdd(obj, pageID)){
      // No. There is no space in the node to put this object!
      return false;
   }//end if

   // if there is free entry to store the new entry.
   if (this->capacity <= this->numEntries){
      // resize the entries.
      this->Resize();
   }//end if
   
   // Look the right position to insert the new object.
   insertIdx = InsertPosition(distance);
   // Get the number of entries.
   idx = this->numEntries;
   // Lets move the data, according to insertIdx.
   while (insertIdx != idx){
      idx--;
      Entries[idx+1].Distance = Entries[idx].Distance;
      Entries[idx+1].Object = Entries[idx].Object;
      Entries[idx+1].PageID = Entries[idx].PageID;
      Entries[idx+1].Radius = Entries[idx].Radius;
      #ifdef __stDBMNENTRIES__
         Entries[idx+1].NEntries = Entries[idx].NEntries;
      #endif //__stDBMNENTRIES__
      #ifdef __stDBMHEIGHT__
         Entries[idx+1].Height = Entries[idx].Height;
      #endif //__stDBMHEIGHT__
   }//end while
   // add the new entry in the right position.
   Entries[idx].Distance = distance;
   Entries[idx].Object = obj;
   Entries[idx].PageID = pageID;
   Entries[idx].Radius = radius;
   #ifdef __stDBMNENTRIES__
      Entries[idx].NEntries = nEntries;
   #endif //__stDBMNENTRIES__
   #ifdef __stDBMHEIGHT__
      Entries[idx].Height = height;
   #endif //__stDBMHEIGHT__

   // Update # of Entries
   this->numEntries++; // One more!
   // Update the usedSize
   this->usedSize += obj->GetSerializedSize() + stDBMNode::GetEntryOverhead();
   // For subtrees.
   if (pageID){
      this->usedSize += sizeof(stDistance);  // Add the size of radius
      #ifdef __stDBMNENTRIES__
         this->usedSize += sizeof(stCount);  // Add the size of NEntries
      #endif //__stDBMNENTRIES__
   }//end if

   return true;
}//end Add

//------------------------------------------------------------------------------
template <class ObjectType>
ObjectType * stDBMShrinkNode< ObjectType >::RepObject(){
   stCount i, idx = 0;

   for (i = 0; i < this->numEntries; i++){
      if (Entries[i].Distance == 0){
         idx = i;
      }//end if
   }//end for
   return Entries[idx].Object;
}//end RepObject

//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stDBMShrinkNode< ObjectType >::GetMinimumRadius(){
   stCount idx;
   stDistance radius = 0;

   for (idx = 0; idx < this->numEntries; idx++){
      if (radius < Entries[idx].Distance + Entries[idx].Radius){
         radius = Entries[idx].Distance + Entries[idx].Radius;
      }//end if
   }//end for
   return radius;
}//end GetMinimumRadius

//------------------------------------------------------------------------------
template <class ObjectType>
ObjectType * stDBMShrinkNode< ObjectType >::Remove(stCount idx){
   ObjectType * obj;
   stPageID pageID;

   #ifdef __stDEBUG__
      if (idx >= this->numEntries){
         throw invalid_argument("idx value is out of range.");
      }//end if
   #endif //__stDEBUG__

   // copy the entry.
   obj = this->ObjectAt(idx);
   pageID = this->PageIDAt(idx);

   // Lets move the data, according to idx.
   while (idx < (this->numEntries - 1)){
      Entries[idx].Distance = Entries[idx+1].Distance;
      Entries[idx].Object = Entries[idx+1].Object;
      Entries[idx].PageID = Entries[idx+1].PageID;
      Entries[idx].Radius = Entries[idx+1].Radius;
      #ifdef __stDBMNENTRIES__
         Entries[idx].NEntries = Entries[idx+1].NEntries;
      #endif //__stDBMNENTRIES__
      #ifdef __stDBMHEIGHT__
         Entries[idx].Height = Entries[idx+1].Height;
      #endif //__stDBMHEIGHT__
      idx++;
   }//end while

   // Update # of Entries
   this->numEntries--; // One less!
   // Update the usedSize
   this->usedSize -= (obj->GetSerializedSize() +
                      stDBMNode::GetEntryOverhead());
   // for subtrees.
   if (pageID){
      this->usedSize -= sizeof(stDistance); // sub the radius
      #ifdef __stDBMNENTRIES__
         this->usedSize -= sizeof(stCount); // sub the NEntries
      #endif //__stDBMNENTRIES__
   }//end if
   // return the removed entry.
   return obj;
}//end Remove

//------------------------------------------------------------------------------
template <class ObjectType>
ObjectType * stDBMShrinkNode< ObjectType >::PopObject(){
   ObjectType * obj;
   stPageID pageID;

   #ifdef __stDEBUG__
      if (this->numEntries==0){
         throw invalid_argument("There is no object.");
      }//end if
   #endif //__stDEBUG__

   // copy the entry.
   obj = this->ObjectAt(this->numEntries-1);
   pageID = this->PageIDAt(this->numEntries-1);
   // Update # of Entries
   this->numEntries--; // One less!
   // Update the usedSize
   this->usedSize -= (obj->GetSerializedSize() +
                      stDBMNode::GetEntryOverhead());
   // for subtrees.
   if (pageID){
      this->usedSize -= sizeof(stDistance); // sub the radius
      #ifdef __stDBMNENTRIES__
         this->usedSize -= sizeof(stCount); // sub the NEntries
      #endif //__stDBMNENTRIES__
   }//end if
   // return the removed entry.
   return obj;
}//end PopObject

//------------------------------------------------------------------------------
template <class ObjectType>
int stDBMShrinkNode< ObjectType >::InsertPosition(stDistance distance){
   int idx;

   #ifdef __stDEBUG__
      if (distance < 0){
         throw invalid_argument("The distance is less than 0.");
      }//end if
   #endif //__stDEBUG__

   // if there is no entries, return 0.
   if (this->numEntries==0){
      return 0;
   }//end if

   // Lets search the correct position, according to its distance.
   idx = this->numEntries;
   while ((distance < Entries[idx-1].Distance) && (idx > 0)){
      idx--;
   }//end while

   return idx;
}//end InsertPosition

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMShrinkNode< ObjectType >::Resize(stCount incSize){
   stDBMShrinkNodeEntry * tmpEntries;

   // allocate new space.
   tmpEntries = new stDBMShrinkNodeEntry[(this->numEntries + incSize) * sizeof(stDBMShrinkNodeEntry)];
   // copy the entries.
   memcpy(tmpEntries, this->Entries, this->numEntries * sizeof(stDBMShrinkNodeEntry));
   // delete the old data.
   delete this->Entries;
   // ajust the pointers.
   this->Entries = tmpEntries;
   this->capacity += incSize;
}//end Resize

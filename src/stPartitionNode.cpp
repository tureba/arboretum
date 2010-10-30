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
* This file implements the blocks of Partition.
*
* @version 1.0
* $Revision: 1.15 $
* $Date: 2004/11/16 18:29:08 $
* $Author: enzo $
* @author Enzo Seraphim(seraphim@icmc.usp.br)
*/

#include <arboretum/stPartitionNode.h>

//------------------------------------------------------------------------------
// class stPartitionRegionDesc
//------------------------------------------------------------------------------
int stPartitionRegionDesc::GetFirstDigit(){
   return (int)(Desc.Region / pow (Desc.NumberRep, (long double)(Desc.NumberRep - 1)));
}//end stPartitionRegionDesc::GetFirstDigit

//------------------------------------------------------------------------------

void stPartitionRegionDesc::ConvertToBins(stByte base, stByte *result){
   stQWord res = Desc.Region;
   int i = 0;
   stQWord powbase;

   for (int i =0; i < base; i++){
      powbase = (stQWord) pow (Desc.NumberRep, (long double)(base - i - 1));
      result[i] = (stByte)(res / powbase);
      res = res - (powbase * result[i]);
   }//end while
} //end stPartitionRegionDesc::ConvertToBins

//-----------------------------------------------------------------------------
// Class stPartitionListRegionDesc
//-----------------------------------------------------------------------------
stPartitionListRegionDesc::~stPartitionListRegionDesc(){
   IdActual = 0;
   for (;IdActual < NumberItem;IdActual++){
     Actual = Root;
     Root = Actual->Next;
     delete Actual;
     IdActual++;
   }//end for
}//end stPartitionListRegionDesc::~stPartitionRegionDesc

//------------------------------------------------------------------------------
stPartitionRegionDesc stPartitionListRegionDesc::GetRegionDesc(int id){
   #ifdef __stDEBUG__
   if ((id < 0) && (id >= NumberItem)){
      throw invalid_argument("id value is out of range.");
   }//end if
   #endif //__stDEBUG__

   //if index is the first
   if(id == 0){
      return Root->RegionDesc;
   }
   //if index is the last
   if(id == NumberItem){
      return Last->RegionDesc;
   }
   //otherwise
   if (IdActual > id){
      Actual = Root;
      for (IdActual=0;IdActual<id;IdActual++){
         Actual = Actual->Next;
      }//end for
   }else{
      for (;IdActual<id;IdActual++){
         Actual = Actual->Next;
      }//end for
   }//end if
   return Actual->RegionDesc;
}//end stPartitionListRegionDesc::GetRegionDesc

//------------------------------------------------------------------------------
int stPartitionListRegionDesc::Add(stPartitionRegionDesc regDesc){
   //locating previous item to insered position
   stItemRegionDesc * ant;
   stItemRegionDesc * aux = Root;
   IdActual = 0;
   while((IdActual < NumberItem) && (aux->RegionDesc.GetRegion() < regDesc.GetRegion())){
      ant = aux;
      aux = aux->Next;
      IdActual++;
   }//end while

   //inserting item
   Actual = new stItemRegionDesc;
   Actual->RegionDesc = regDesc;
   Actual->Next = aux;

   //if change the last item
   if (IdActual == NumberItem){
      Last = Actual;
   }//end if
   //if change the root
   if (IdActual == 0){
      Root = Actual;
   }else{
      ant->Next = Actual;
   }//end if

   //update number of representatives
   NumberItem++;
   return IdActual;
}//end stPartitionListRegionDesc::Add

//------------------------------------------------------------------------------
// class stPartitionNode
//------------------------------------------------------------------------------
stPartitionNode * stPartitionNode::CreateBucket(stPage * page){
   stPartitionNode::stPartitionNodeHeader * header;

   header = (stPartitionNodeHeader *)(page->GetData());
   switch (header->Type){
   case INDEX:
      // Create an index page
      return new stPartitionIndexBucket(page, false);
   case LEAF:
      // Create a leaf page
      return new stPartitionLeafBucket(page, false);
   default:
      return NULL;
   }//end switch
}//end stPartitionNode::CreateBucket()

//------------------------------------------------------------------------------
// class stPartitionIndexBucket
//------------------------------------------------------------------------------
stPartitionIndexBucket::stPartitionIndexBucket(stPage * page, bool create):
      stPartitionNode(page){
   // Attention to this manouver! It is the brain of this
   // implementation.
   Entries = (stPartitionIndexEntry*)(page->GetData() + sizeof(stPartitionNodeHeader));

   // Initialize page
   if (create){
      #ifdef __stDEBUG__
      Page->Clear();
      #endif //__stDEBUG__
      Header->Type = INDEX;
      Header->Occupation = 0;
      Header->NextBucket = 0;
   }//end if
}//end stPartitionIndexBucket::stPartitionIndexBucket()

//------------------------------------------------------------------------------
stSize stPartitionIndexBucket::GetFree(){
   stSize usedsize;

   // Fixed size
   usedsize = sizeof(stPartitionNodeHeader);

   // Entries
   if (GetNumberOfEntries() > 0){
      usedsize +=
         // Total size of entries
         (sizeof(stPartitionIndexEntry) * GetNumberOfEntries());
   }//end if

   return Page->GetPageSize() - usedsize;
}//end stPartitionIndexBucket::GetFree()

//------------------------------------------------------------------------------
// class stPartitionLeafBucket
//------------------------------------------------------------------------------
stPartitionLeafBucket::stPartitionLeafBucket(stPage * page, bool create):
      stPartitionNode(page){
   Entries = (stPartitionLeafEntry*)(page->GetData() + sizeof(stPartitionNodeHeader));
   // Initialize page
   if (create){
      #ifdef __stDEBUG__
      Page->Clear();
      #endif //__stDEBUG__
      Header->Type = LEAF;
      Header->Occupation = 0;
      Header->NextBucket = 0;
   }//end if
}//end stPartitionLeafBucket::stPartitionLeafBucket()

//------------------------------------------------------------------------------
stSize stPartitionLeafBucket::GetObjectSize(int id){
   #ifdef __stDEBUG__
   if ((id < 0) && (id >= GetNumberOfEntries())){
      throw invalid_argument("id value is out of range.");
   }//end if
   #endif //__stDEBUG__

   if (id == 0){
      // First object
      return Page->GetPageSize() - Entries[0].Offset;
   }else{
      // Any other
      return Entries[id - 1].Offset - Entries[id].Offset;
   }//end if
}//end stPartitionLeafBucket::GetObjectSize()
//------------------------------------------------------------------------------
stSize stPartitionLeafBucket::GetFree(){
   stSize usedsize;

   // Fixed size
   usedsize = sizeof(stPartitionNodeHeader);

   // Entries
   if (GetNumberOfEntries() > 0){
      usedsize +=
         // Total size of entries
         (sizeof(stPartitionLeafEntry) * GetNumberOfEntries()) +
         // Total object size
         (Page->GetPageSize() - Entries[GetNumberOfEntries() - 1].Offset);
   }//end if

   return Page->GetPageSize() - usedsize;
}//end stPartitionLeafBucket::GetFree()

//-----------------------------------------------------------------------------
// Class stPartitionRepresentativesNode
//-----------------------------------------------------------------------------
stPartitionRepresentativesNode::stPartitionRepresentativesNode(
      stPage * page, bool create):stPartitionNode(page){

   // Attention to this manouver! It is the brain of this
   // implementation.
   Entries    = (stSize *)((this->Page->GetData())+
      sizeof(stPartitionNodeHeader));

   // Initialize page
   if (create){
      #ifdef __stDEBUG__
      Page->Clear();
      #endif //__stDEBUG__
      Header->Type = REPRESENT;
      Header->Occupation = 0;
      Header->NextBucket = 0;
   }//end if
}//end stPartitionRepresentativesNode::stPartitionRepresentativesNode()

//-----------------------------------------------------------------------------
int stPartitionRepresentativesNode::AddEntry(stSize size, const stByte * object){
   stSize entrysize;

   #ifdef __stDEBUG__
   if (size == 0){
      throw invalid_argument("The object size is 0.");
   }//end if
   #endif //__stDEBUG__

   // Does it fit ?
   entrysize = size + sizeof(stSize);
   if (entrysize > this->GetFree()){
      // No, it doesn't.
      return -1;
   }//end if

   // Ok. I can put it. Lets put it in the last position.
   // Adding the object. Take care with these pointers or you will destroy the
   // node. The idea is to put the object of an entry in the reverse order
   // in the data array.
   if (Header->Occupation == 0){
      Entries[Header->Occupation] = Page->GetPageSize() - size;
   }else{
      Entries[Header->Occupation] = Entries[Header->Occupation - 1] - size;
   }//end if

   memcpy((void*)(Page->GetData() + Entries[Header->Occupation]),
            (void*)object, size);

   // Update # of entries
   Header->Occupation++; // One more!

   return Header->Occupation - 1;
}//end stPartitionRepresentativesNode::AddEntry()

//------------------------------------------------------------------------------
stSize stPartitionRepresentativesNode::GetObjectSize(int id){
   #ifdef __stDEBUG__
   if ((id < 0) && (id >= GetNumberOfEntries())){
      throw invalid_argument("id value is out of range.");
   }//end if
   #endif //__stDEBUG__

   if (id == 0){
      // First object
      return Page->GetPageSize() - Entries[0];
   }else{
      // Any otherstPartitionLeafBucket
      return Entries[id - 1] - Entries[id];
   }//end if
}//end stPartitionRepresentativesNode::GetObjectSize()

//------------------------------------------------------------------------------
stSize stPartitionRepresentativesNode::GetFree(){
   stSize usedsize;

   // Fixed size
   usedsize = sizeof(stPartitionNodeHeader);

   // Entries
   if (GetNumberOfEntries() > 0){

      usedsize +=
         // Total size of entries
         (sizeof(stSize) * GetNumberOfEntries()) +
         // Total object size
         (Page->GetPageSize() - Entries[GetNumberOfEntries() - 1]);
   }//end if

   return Page->GetPageSize() - usedsize;
}//end stPartitionRepresentativesNode::GetFree()


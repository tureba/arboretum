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
* This file implements the dbmTree nodes.
*
* @version 1.0
* $Revision: 1.9 $
* $Date: 2005/03/08 19:44:19 $
* $Author: marcos $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.sc.usp.br)
*/
// Copyright (c) 2003 GBDI-ICMC-USP
#include <arboretum/stDBMNode.h>

//------------------------------------------------------------------------------
// class stDBMNode
//------------------------------------------------------------------------------

int stDBMNode::AddEntry(stSize size, const stByte * object, stPageID subTree){
   stSize entrySize;

   #ifdef __stDEBUG__
   if (size == 0){
      throw invalid_argument("The object size is 0.");
   }//end if
   #endif //__stDEBUG__

   // Does it fit ?
   entrySize = size + sizeof(stDBMEntry);

   // Is this a subtree entry?
   if (subTree){
      // Add the radius size.
      entrySize += sizeof(stDistance);
      #ifdef __stDBMNENTRIES__
         entrySize += sizeof(stCount);
      #endif //__stDBMNENTRIES__
   }//end if

   if (entrySize > this->GetFree()){
      // No, it doesn't.
      return -1;
   }//end if

   // Mark this entry as...
   Entries[Header->Occupation].PageID = subTree;  // subtree

   // Ok. I can put it. Lets put it in the last position.

   // Adding the object. Take care with these pointers or you will destroy the
   // node. The idea is to put the object of an entry in the reverse order
   // in the data array.
   if (Header->Occupation == 0){
      Entries[0].Offset = Page->GetPageSize() - size;
   }else{
      Entries[Header->Occupation].Offset = Entries[Header->Occupation - 1].Offset - size;
   }//end if

   // Discount the Radius if this is a subtree.
   if (subTree){
      Entries[Header->Occupation].Offset -= sizeof(stDistance);
      #ifdef __stDBMNENTRIES__
         Entries[Header->Occupation].Offset -= sizeof(stCount);
      #endif //__stDBMNENTRIES__
   }//end if

   memcpy((void *)(Page->GetData() + Entries[Header->Occupation].Offset),
          (void *)object, size);
   // Update # of entries
   Header->Occupation++; // One more!

   return Header->Occupation - 1;
}//end stDBMNode::AddEntry

//------------------------------------------------------------------------------
int stDBMNode::GetRepresentativeIndex(){
   stCount i;
   bool stop;

   // Looking for it
   i = 0;
   stop = (i == Header->Occupation);
   while (!stop){
      if (Entries[i].Distance == 0.0){
         // Found!
         stop = true;
      }else{
         // Next...
         i++;
         stop = (i == Header->Occupation);
      }//end if
   }//end while

   // Output
   if (i == Header->Occupation){
      // Empty or not found.
      return -1;
   }else{
      // Found!
      return i;
   }//end if
}//end stDBMNode::GetRepresentativeIndex

//------------------------------------------------------------------------------
const stByte * stDBMNode::GetObject(stCount idx){

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   return Page->GetData() + Entries[idx].Offset;
}//end stDBMNode::GetObject

//------------------------------------------------------------------------------
stSize stDBMNode::GetObjectSize(stCount idx){
   stSize entrySize;

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   if (idx == 0){
      // First object
      entrySize = Page->GetPageSize() - Entries[0].Offset;
   }else{
      // Any other
      entrySize = Entries[idx - 1].Offset - Entries[idx].Offset;
   }//end if

   // Is this a subtree entry?
   if (Entries[idx].PageID){
      // Discount the radius
      entrySize -= sizeof(stDistance);
      #ifdef __stDBMNENTRIES__
         entrySize -= sizeof(stCount);
      #endif //__stDBMNENTRIES__
   }//end if

   return entrySize;
}//end stDBMNode::GetObjectSize

//------------------------------------------------------------------------------
void stDBMNode::RemoveEntry(int idx){
   stCount i;
   stCount lastID;
   stSize rObjSize;

   // Programmer's note: This procedure is simple but tricky! See the
   // stDBMNode structure documentation for more details.

   #ifdef __stDEBUG__
   if ((idx < 0) || (idx >= (int )GetNumberOfEntries())){
      // Oops! This id doesn't exists.
      throw range_error("Invalid idx!");
   }//end if
   #endif //__stDEBUG__

   // Let's remove
   lastID = Header->Occupation - 1; // The id of the last object. This
                                    // value will be very useful.
   // Do I need to move something ?
   if (lastID != (unsigned int)idx){
      // Yes, I do. Save the removed object size
      rObjSize = GetObjectSize(idx);

      // If this is a subtree, add the size of Radius.
      if (Entries[idx].PageID){
         rObjSize += sizeof(stDistance);
         #ifdef __stDBMNENTRIES__
            rObjSize += sizeof(stCount);
         #endif //__stDBMNENTRIES__
         }//end if

      // Let's move objects first. We will use memmove() from stdlib because
      // it handles the overlap between src and dst. Remember that src is the
      // offset of the last object and the dst is the offset of the last
      // object plus removed object size.
      memmove(Page->GetData() + Entries[lastID].Offset + rObjSize,
              Page->GetData() + Entries[lastID].Offset,
              Entries[idx].Offset - Entries[lastID].Offset);
      // Let's move entries...
      for (i = idx; i < lastID; i++){
         // Copy all fields with memcpy (it's faster than field copy).
         memcpy(Entries + i, Entries + i + 1, sizeof(stDBMEntry));

         // Update offset by adding the removed object size to it. It will
         // reflect the previous move operation.
         Entries[i].Offset += rObjSize;
      }//end for
   }//end if

   // Update counter...
   Header->Occupation--;
}//end stDBMNode::RemoveEntry

//------------------------------------------------------------------------------
stSize stDBMNode::GetUsed(){
   stSize usedSize;

   // Fixed size
   usedSize = sizeof(stDBMNodeHeader);

   // Entries
   if (GetNumberOfEntries() > 0){
      usedSize +=
         // Total size of entries
         (sizeof(stDBMEntry) * GetNumberOfEntries()) +
         // Total object size
         (Page->GetPageSize() - Entries[GetNumberOfEntries() - 1].Offset);
   }//end if

   return usedSize;
}//end stDBMNode::GetUsed

//------------------------------------------------------------------------------
stByte stDBMNode::GetHeight(){
   stCount i;
   stByte maxHeight = 0;

   #ifdef __stDBMHEIGHT__
   for (i = 0; i < GetNumberOfEntries(); i++){
      if (maxHeight < GetEntry(i).Height){
         maxHeight = GetEntry(i).Height;
      }//end if
   }//end for
   #endif //__stDBMHEIGHT__

   return maxHeight;
}//end stDBMNode::GetHeight

//------------------------------------------------------------------------------
stDistance stDBMNode::GetMinimumRadius(){
   stCount i;
   stDistance maxDistance = 0;
   stDistance distance;

   for (i = 0; i < GetNumberOfEntries(); i++){
      distance = GetEntry(i).Distance;
      // is this a representative subtree?
      if (GetEntry(i).PageID){
         // Yes, add the Radius of this subtree.
         distance += GetRadius(i);
      }//end if
      if (maxDistance < distance){
         maxDistance = distance;
      }//end if
   }//end for

   return maxDistance;
}//end stDBMNode::GetMinimumRadius

//------------------------------------------------------------------------------
stCount stDBMNode::GetTotalObjectCount(){
   stCount i;
   stSize count;

   #ifdef __stDBMNENTRIES__
      count = 0;
      for (i = 0; i < GetNumberOfEntries(); i++){
         // is this a representative subtree?
         if (GetEntry(i).PageID){
            // Yes, add the NEntries of this subtree.
            count += GetNEntries(i);
         }else{
            count++;
         }//end if
      }//end for
   #else
      count = GetNumberOfEntries();
   #endif //__stDBMNENTRIES__

   //return the total object count.
   return count;
}//end stDBMNode::GetTotalObjectCount

//------------------------------------------------------------------------------
stCount stDBMNode::GetNumberOfFreeObjects(){
   stCount i;
   stSize count = 0;

   for (i = 0; i < GetNumberOfEntries(); i++){
      if (!GetEntry(i).PageID){
         count++;
      }//end if
   }//end for
   // return the number of free objects.
   return count;
}//end stDBMNode::GetNumberOfFreeObjects

//------------------------------------------------------------------------------
stCount stDBMNode::GetFarthestObject(){
   stCount i, idx = 0;
   stDistance maxDistance = 0;

   for (i = 0; i < GetNumberOfEntries(); i++){
      if (GetEntry(i).Distance > maxDistance){
         maxDistance = GetEntry(i).Distance;
         idx = i;
      }//end if
   }//end for

   return idx;
}//end stDBMNode::GetFarthestObject

//------------------------------------------------------------------------------
stCount stDBMNode::GetNEntries(stCount idx){
   #ifdef __stDBMNENTRIES__
      stCount * NEntries;
      #ifdef __stDEBUG__
      if (idx >= GetNumberOfEntries()){
         throw invalid_argument("idx value is out of range.");
      }//end if
      #endif //__stDEBUG__

      // if this entry does not have subtree.
      if (!Entries[idx].PageID){
         return 0;
      }//end if

      if (idx == 0){
         // First object
         NEntries = (stCount *)(Page->GetData() + Page->GetPageSize() - sizeof(stCount) - sizeof(stDistance));
      }else{
         // Any other
         NEntries = (stCount *)(Page->GetData() + Entries[idx - 1].Offset - sizeof(stCount) - sizeof(stDistance));
      }//end if
      // return NEntries.
      return *NEntries;
   #else
      return 0;
   #endif //__stDBMNENTRIES__
}//end stDBMNode::GetNEntries

//------------------------------------------------------------------------------
void stDBMNode::SetNEntries(stCount idx, stCount NEntries){
   #ifdef __stDBMNENTRIES__
      stCount * nEntries;

      #ifdef __stDEBUG__
      if (idx >= GetNumberOfEntries()){
         throw invalid_argument("idx value is out of range.");
      }//end if
      #endif //__stDEBUG__

      // Adding the object. Take care with these pointers or you will destroy the
      // node. The idea is to put the object of an entry in the reverse order
      // in the data array.
      if (idx == 0){
         // First object
         nEntries = (stCount *)(Page->GetData() + Page->GetPageSize() - sizeof(stCount) - sizeof(stDistance));
      }else{
         // Any other
         nEntries = (stCount *)(Page->GetData() + Entries[idx - 1].Offset - sizeof(stCount) - sizeof(stDistance));
      }//end if
      // set the new NEntries.
      *nEntries = NEntries;
   #endif //__stDBMNENTRIES__
}//end stDBMNode::SetNEntries

//------------------------------------------------------------------------------
stDistance stDBMNode::GetRadius(stCount idx){
   stDistance * Radius;
   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   // if this entry does not have subtree.
   if (!Entries[idx].PageID){
      return 0.0;
   }//end if

   if (idx == 0){
      // First object
      Radius = (stDistance *)(Page->GetData() + Page->GetPageSize() - sizeof(stDistance));
   }else{
      // Any other
      Radius = (stDistance *)(Page->GetData() + Entries[idx - 1].Offset - sizeof(stDistance));
   }//end if
   // return the Radius.
   return *Radius;
}//end stDBMNode::GetRadius

//------------------------------------------------------------------------------
void stDBMNode::SetRadius(stCount idx, stDistance radius){
   stDistance * distance;

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   // if this entry has a space to store NEntries.
   if (GetEntry(idx).PageID){
      // Adding the object. Take care with these pointers or you will destroy the
      // node. The idea is to put the object of an entry in the reverse order
      // in the data array.
      if (idx == 0){
         // First object
         distance = (stDistance *)(Page->GetData() + Page->GetPageSize() - sizeof(stDistance));
      }else{
         // Any other
         distance = (stDistance *)(Page->GetData() + Entries[idx - 1].Offset - sizeof(stDistance));
      }//end if
      // set the new Distance.
      *distance = radius;
   }//end if
}//end stDBMNode::SetRadius

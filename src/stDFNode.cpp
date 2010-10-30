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
* This file implements the DFTree nodes.
*
* @version 1.0
* $Revision: 1.4 $
* $Date: 2005/03/08 19:44:19 $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Joselene Marques (joselene@icmc.usp.br)
*/
// Copyright (c) 2002 GBDI-ICMC-USP
#include <arboretum/stDFNode.h>

//------------------------------------------------------------------------------
// class stDFNode
//------------------------------------------------------------------------------
stDFNode * stDFNode::CreateNode(stPage * page){
   stDFNode::stDFNodeHeader * header;

   header = (stDFNodeHeader *)(page->GetData());
   switch (header->Type){
      case INDEX:
         // Create an index page
         return new stDFIndexNode(page, false);
      case LEAF:
         // Create a leaf page
         return new stDFLeafNode(page, false);
      default:
         return NULL;
   }//end switch
}//end stDFNode::CreateNode()

//------------------------------------------------------------------------------
// class stDFIndexNode
//------------------------------------------------------------------------------
stDFIndexNode::stDFIndexNode(stPage * page, bool create):
      stDFNode(page){

   // Attention to this manouver! It is the brain of this
   // implementation.
   Entries = (stDFIndexEntry *)(page->GetData() + sizeof(stDFNodeHeader));

   // Initialize page
   if (create){
      #ifdef __stDEBUG__
      Page->Clear();
      #endif //__stDEBUG__
      this->Header->Type = INDEX;
      this->Header->Occupation = 0;
   }//end if
}//end stDFIndexNode::stDFIndexNode()

//------------------------------------------------------------------------------
int stDFIndexNode::AddEntry(stSize size, const stByte * object){
   stSize entrySize;

   #ifdef __stDEBUG__
   if (size == 0){
      throw invalid_argument("The object size is 0.");
   }//end if
   #endif //__stDEBUG__

   // Does it fit ?
   entrySize = size + sizeof(stDFIndexEntry);
   if (entrySize > this->GetFree()){
      // No, it doesn't.
      return -1;
   }//end if

   // Ok. I can put it. Lets put it in the last position.

   // Adding the object. Take care with these pointers or you will destroy the
   // node. The idea is to put the object of an entry in the reverse order
   // in the data array.
   if (Header->Occupation == 0){
      Entries[Header->Occupation].Offset = Page->GetPageSize() - size;
   }else{
      Entries[Header->Occupation].Offset = Entries[Header->Occupation - 1].Offset - size;
   }//end if
   memcpy((void *)(Page->GetData() + Entries[Header->Occupation].Offset),
          (void *)object, size);
   // Update # of entries
   Header->Occupation++; // One more!

   //alocate space to GR 
   Entries[Header->Occupation - 1].FieldDistance = new stDistance[STFOCUS];

   return Header->Occupation - 1;
}//end stDFIndexNode::AddEntry()

//------------------------------------------------------------------------------
int stDFIndexNode::GetRepresentativeEntry(){
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
}//end stDFIndexNode::GetRepresentativeEntry()

//------------------------------------------------------------------------------
const stByte * stDFIndexNode::GetObject(stCount idx){

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   return Page->GetData() + Entries[idx].Offset;
}//end stDFIndexNode::GetObject()

//------------------------------------------------------------------------------
stSize stDFIndexNode::GetObjectSize(stCount idx){
   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   if (idx == 0){
      // First object
      return Page->GetPageSize() - Entries[0].Offset;
   }else{
      // Any other
      return Entries[idx - 1].Offset - Entries[idx].Offset;
   }//end if
}//end stDFIndexNode::GetObjectSize()

//------------------------------------------------------------------------------
void stDFIndexNode::RemoveEntry(stCount idx){
   stSize rObjSize;
   stCount i, lastID;

   // Programmer's note: This procedure is simple but tricky! See the
   // DFIndexNode structure documentation for more details.

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      // Oops! This id doesn't exists.
      throw range_error("Invalid idx!");
   }//end if
   #endif //__stDEBUG__

   // Let's remove
   lastID = Header->Occupation - 1; // The id of the last object. This
                                    // value will be very useful.
   // Do I need to move something ?
   if (idx != lastID){
      // Yes, I do.
      rObjSize = GetObjectSize(idx);    // Save the removed object size

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
         memcpy(Entries + i, Entries + i + 1, sizeof(stDFIndexEntry));

         // Update offset by adding the removed object size to it. It will
         // reflect the previous move operation.
         Entries[i].Offset += rObjSize;
      }//end for
   }//end if

   // Update counter...
   Header->Occupation--;
}//end stDFIndexNode::RemoveEntry()

//------------------------------------------------------------------------------
stSize stDFIndexNode::GetFree(){
   stSize usedSize;

   // Fixed size
   usedSize = sizeof(stDFNodeHeader);

   // Entries
   if (GetNumberOfEntries() > 0){
      usedSize +=
         // Total size of entries
         (sizeof(stDFIndexEntry) * GetNumberOfEntries()) +
         // Total object size
         (Page->GetPageSize() - Entries[GetNumberOfEntries() - 1].Offset);
   }//end if

   return Page->GetPageSize() - usedSize;
}//end stDFIndexNode::GetFree()

//------------------------------------------------------------------------------
stDistance stDFIndexNode::GetMinimumRadius(){
   stDistance minRadius = 0;
   stDistance distance;
   stCount i;

   // For each entry.
   for (i = 0; i < GetNumberOfEntries(); i++){
      distance = GetIndexEntry(i).Distance + GetIndexEntry(i).Radius;
      if (minRadius < distance){
         minRadius = distance;
      }//end if
   }//end for

   return minRadius;
}//end stDFIndexNode::GetMinimumRadius

//------------------------------------------------------------------------------
stCount stDFIndexNode::GetTotalObjectCount(){
   stSize count;
   stCount i;

   count = 0;
   for (i = 0; i < GetNumberOfEntries(); i++){
      count += GetIndexEntry(i).NEntries;
   }//end for

   return count;
}//end stDFIndexNode::GetTotalObjectCount

//------------------------------------------------------------------------------
// class stDFLeafNode
//------------------------------------------------------------------------------
stDFLeafNode::stDFLeafNode(stPage * page, bool create):
      stDFNode(page){

   // Attention to this manouver! It is the brain of this
   // implementation.
   Entries = (stDFLeafEntry *)(page->GetData() + sizeof(stDFNodeHeader));

   // Initialize page
   if (create){
      #ifdef __stDEBUG__
      Page->Clear();
      #endif //__stDEBUG__
      this->Header->Type = LEAF;
      this->Header->Occupation = 0;
   }//end if
}//end stDFLeafNode::stDFLeafNode()

//------------------------------------------------------------------------------
int stDFLeafNode::AddEntry(stSize size, const stByte * object){
   stSize entrySize;

   #ifdef __stDEBUG__
   if (size == 0){
      throw invalid_argument("The object size is 0.");
   }//end if
   #endif //__stDEBUG__


   // Does it fit ?
   entrySize = size + sizeof(stDFLeafEntry);
   if (entrySize > this->GetFree()){
      // No, it doesn't.
      return -1;
   }//end if

   // Ok. I can put it. Lets put it in the last position.

   // Adding the object. Take care with these pointers or you will destroy the
   // node. The idea is to put the object of an entry in the reverse order
   // in the data array.
   if (Header->Occupation == 0){
      Entries[Header->Occupation].Offset = Page->GetPageSize() - size;
   }else{
      Entries[Header->Occupation].Offset = Entries[Header->Occupation - 1].Offset - size;
   }//end if
   memcpy((void *)(Page->GetData() + Entries[Header->Occupation].Offset),
          (void *)object, size);

   // Update # of entries
   Header->Occupation++; // One more!

   //alocate space to GR
   Entries[Header->Occupation - 1].FieldDistance = new stDistance[STFOCUS];

   return Header->Occupation - 1;
}//end stDFLeafNode::AddEntry()

//------------------------------------------------------------------------------
int stDFLeafNode::GetRepresentativeEntry(){
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
}//end stDFLeafNode::GetRepresentativeEntry()

//------------------------------------------------------------------------------
const stByte * stDFLeafNode::GetObject(stCount idx){

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   return Page->GetData() + Entries[idx].Offset;
}//end stDFLeafNode::GetObject()

//------------------------------------------------------------------------------
stSize stDFLeafNode::GetObjectSize(stCount idx){
   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   if (idx == 0){
      // First object
      return Page->GetPageSize() - Entries[0].Offset;
   }else{
      // Any other
      return Entries[idx - 1].Offset - Entries[idx].Offset;
   }//end if
}//end stDFLeafIndexNode::GetObjectSize()

//------------------------------------------------------------------------------
void stDFLeafNode::RemoveEntry(stCount idx){
   stSize rObjSize;
   stCount i, lastID;

   // Programmer's note: This procedure is simple but tricky! See the
   // DFIndexNode structure documentation for more details.

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      // Oops! This id doesn't exists.
      throw range_error("Invalid idx!");
   }//end if
   #endif //__stDEBUG__

   // Let's remove
   lastID = Header->Occupation - 1; // The id of the last object. This
                                    // value will be very useful.
   // Do I need to move something ?
   if (idx != lastID){
      // Yes, I do.
      rObjSize = GetObjectSize(idx);    // Save the removed object size

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
         memcpy(Entries + i, Entries + i + 1, sizeof(stDFLeafEntry));

         // Update offset by adding the removed object size to it. It will
         // reflect the previous move operation.
         Entries[i].Offset += rObjSize;
      }//end for
   }//end if

   // Update counter...
   Header->Occupation--;
}//end stDFLeafNode::RemoveEntry

//------------------------------------------------------------------------------
stDistance stDFLeafNode::GetMinimumRadius(){
   stDistance minRadius = 0;
   stCount i;

   // For each entry.
   for (i = 0; i < GetNumberOfEntries(); i++){
      if (minRadius < GetLeafEntry(i).Distance){
         minRadius = GetLeafEntry(i).Distance;
      }//end if
   }//end for

   return minRadius;
}//end stDFLeafNode::GetMinimumRadius

//------------------------------------------------------------------------------
stSize stDFLeafNode::GetFree(){
   stSize usedSize;

   // Fixed size
   usedSize = sizeof(stDFNodeHeader);

   // Entries
   if (GetNumberOfEntries() > 0){

      usedSize +=
         // Total size of entries
         (sizeof(stDFLeafEntry) * GetNumberOfEntries()) +
         // Total object size
         (Page->GetPageSize() - Entries[GetNumberOfEntries() - 1].Offset);
   }//end if

   return Page->GetPageSize() - usedSize;
}//end stDFLeafNode::GetFree()

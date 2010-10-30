/**
* @file
*
* This file implements the GH Tree node.
*
* @version 1.0
* $Date: 2004/06/01 17:29:46 $
* @author Ives Renê Venturini Pola (ives@icmc.sc.usp.br)
*/
// Copyright (c) 2004 GBDI-ICMC-USP
#include <arboretum/stGHNode.h>

//-----------------------------------------------------------------------------
// class stGHNode
//-----------------------------------------------------------------------------

stGHNode::stGHNode(stPage * page, bool create){

   // Set elements
   this->Page = page;
   this->Header = (stGHNodeHeader *) this->Page->GetData();
   this->Entries = (stGHNodeEntry *)(this->Page->GetData() +
                   sizeof(stGHNodeHeader));
   // if create is true, we must to zero fill the page
   if (create){
      #ifdef __stDEBUG__
      Page->Clear();
      #endif //__stDEBUG__
      this->Header->Occupation = 0;
   }//end if
}//end stGHNode::stGHNode

//------------------------------------------------------------------------------
int stGHNode::AddEntry(stSize size, const stByte * object){
   stSize entrySize;

   #ifdef __stDEBUG__
   if (size == 0){
      throw invalid_argument("The object size is 0.");
   }//end if
   #endif //__stDEBUG__

   // Does it fit ?
   entrySize = size + sizeof(stGHNodeEntry);
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
      if (Header->Occupation == 1){
         Entries[Header->Occupation].Offset = Entries[Header->Occupation - 1].Offset - size;
      }else{
         return -1; // It cannot store more than two objects per node
      }//end if
   }//end if
   // Set the pageID.
   this->Entries[Header->Occupation].PageID = 0;
   memcpy((void *)(Page->GetData() + Entries[Header->Occupation].Offset),
          (void *)object, size);
   // Update # of entries
   Header->Occupation++; // One more!

   return Header->Occupation - 1;
}//end stGHNode::AddEntry

//------------------------------------------------------------------------------
const stByte * stGHNode::GetObject(stCount idx){

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      throw invalid_argument("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   return Page->GetData() + Entries[idx].Offset;
}//end stGHNode::GetObject

//------------------------------------------------------------------------------
stSize stGHNode::GetObjectSize(stCount idx){
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
}//end stGHNode::GetObjectSize

//------------------------------------------------------------------------------
void stGHNode::RemoveEntry(stCount idx){
   stCount lastID;
   stCount i;
   stSize rObjSize;

   // Programmer's note: This procedure is simple but tricky! See the
   // stGHNode structure documentation for more details.

   #ifdef __stDEBUG__
   if (idx >= GetNumberOfEntries()){
      // Oops! This idx doesn't exists.
      throw range_error("idx value is out of range.");
   }//end if
   #endif //__stDEBUG__

   // Let's remove
   lastID = Header->Occupation - 1; // The idx of the last object. This
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
         memcpy(Entries + i, Entries + i + 1, sizeof(stGHNodeEntry));

         // Update offset by adding the removed object size to it. It will
         // reflect the previous move operation.
         Entries[i].Offset += rObjSize;
      }//end for
   }//end if

   // Update counter...
   Header->Occupation--;
}//end stGHNode::RemoveEntry
                        
//------------------------------------------------------------------------------
stSize stGHNode::GetFree(){                                                     
   stSize usedSize;

   // Fixed size
   usedSize = sizeof(stGHNodeHeader);

   // Entries
   if (GetNumberOfEntries() > 0){

      usedSize +=
         // Total size of entries
         (sizeof(stGHNodeEntry) * GetNumberOfEntries()) +
         // Total object size
         (Page->GetPageSize() - Entries[GetNumberOfEntries() - 1].Offset);
   }//end if

   return Page->GetPageSize() - usedSize;
}//end stGHNode::GetFree

/**
* @file
*
* This file implements the VPTree node.
*
* @version 1.0
* $Date: 2004/06/01 17:29:46 $
* @author Ives Renê Venturini Pola (ives@icmc.usp.br)
*/
// Copyright (c) 2004 GBDI-ICMC-USP
#include <arboretum/stVPNode.h>

//-----------------------------------------------------------------------------
// class stVPNode
//-----------------------------------------------------------------------------
stVPNode::stVPNode(stPage * page, bool create){

   this->Page = page;

   // if create is true, we must to zero fill the page
   if (create){
      Page->Clear();
   }//end if

   // Set elements
   this->Header = (stVPNodeHeader *) this->Page->GetData();
   this->Entry = (stSize *)(this->Page->GetData() + sizeof(stVPNodeHeader));
}//end stVPNode::stVPNode

//------------------------------------------------------------------------------
bool stVPNode::AddEntry(stSize size, const stByte * object){
   stSize totalsize;
   stSize offs;

   totalsize = size + sizeof(stSize);
   if (totalsize <= GetFree()){

      offs = Page->GetPageSize() - size;

      // Update entry offset
      *Entry = offs;
      // Write object
      memcpy( (void *) (Page->GetData() + *Entry), (void *) object, size);

      return true;
   }else{
      // there is no room for the object
      return false;
   }//end if
}//end stVPNode::AddEntry

//------------------------------------------------------------------------------
const stByte * stVPNode::GetObject(){
   return (stByte *) Page->GetData() + *Entry;
}//end stVPNode::GetObject

//------------------------------------------------------------------------------
stSize stVPNode::GetObjectSize(){
   return (stSize) Page->GetPageSize() - *Entry;
}//end stVPNode::GetObjectSize

//------------------------------------------------------------------------------
stSize stVPNode::GetFree(){
   return Page->GetPageSize() - sizeof(stVPNodeHeader);
}//end stVPNode::GetFree

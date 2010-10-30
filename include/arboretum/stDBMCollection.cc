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
* This file defines a Collection of Objects to be promoted in high levels of
* the tree.
*
* @version 1.0
* $Revision: 1.6 $
* $Date: 2004/03/07 19:21:32 $
* $Author: marcos $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Review of documentation.
*/
// Copyright (c) 2003 GBDI-ICMC-USP


//==============================================================================
// Class stDBMCollectionVector
//==============================================================================

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMCollectionVector<ObjectType>::RemoveAll(){
   int idx;
   int size = Pairs.size();

   // Delete all object that are in the vector.
   for (idx=size; idx > 0; idx--){
      if (Pairs[idx-1] != NULL){
         delete Pairs[idx-1];
      }//end if
   }//end for
   // Clean the vector.
   Pairs.clear();
}//end RemoveAll

//------------------------------------------------------------------------------
template <class ObjectType>
stCount stDBMCollectionVector<ObjectType>::GetNumberOfFreeObjects(){
   stCount idx;
   stCount freeObj = 0;
   // Foa all entries.
   for (idx=0; idx < Pairs.size(); idx++){
      // Test if this is a free object.
      if (!Pairs[idx]->GetPageID())
         freeObj++;
   }//end for
   return freeObj;
}//end GetNumberOfFreeObjects

//------------------------------------------------------------------------------
template <class ObjectType>
stSize stDBMCollectionVector<ObjectType>::GetUsedSpace(){
   stCount idx;
   stCount size = Pairs.size();
   stSize usedSpace = 0;

   usedSpace = size * stDBMNode::GetEntryOverhead();
   // For each entry
   for (idx = 0; idx < size; idx++){
      usedSpace += (Pairs[idx]->GetObject())->GetSerializedSize();
      // If this entry is a subtree.
      if (Pairs[idx]->GetPageID()){
         // Add the radius.
         usedSpace += sizeof(stDistance);
         #ifdef __stDBMNENTRIES__
            usedSpace += sizeof(stCount);
         #endif //__stDBMNENTRIES__
      }//end if
   }//end for

   return usedSpace;
}//end GetUsedSpace

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMCollectionVector<ObjectType>::SetMineForAllObjects(bool option){
   stCount size = Pairs.size();

   for (stCount idx=0; idx<size; idx++){
      Pairs[idx]->SetMine(option);
   }//end for
}//end SetMineForAllObjects


//==============================================================================
// Class stDBMCollection
//==============================================================================

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMCollection<ObjectType>::AddEntry(tObject * object,
                    #ifdef __stDBMHEIGHT__
                       stByte height,
                    #endif //__stDBMHEIGHT__
                    stPageID pageID,
                    #ifdef __stDBMNENTRIES__
                       stCount nEntries,
                    #endif //__stDBMNENTRIES__
                    stDistance radius,
                    bool mine){
	// Resize me if required.
	if (Size == Capacity){
		Resize();
	}//end if
   Pairs[Size].SetObject(object);
   Pairs[Size].SetPageID(pageID);
   Pairs[Size].SetRadius(radius);
   #ifdef __stDBMHEIGHT__
      Pairs[Size].SetHeight(height);
   #endif //__stDBMHEIGHT__
   #ifdef __stDBMNENTRIES__
      Pairs[Size].SetNEntries(nEntries);
   #endif //__stDBMNENTRIES__
   Pairs[Size].SetMine(mine);
	Size++;
}//end AddPair

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMCollection<ObjectType>::RemoveAll(){
   // Delete the vector.
   if (Pairs != NULL){
      for (stCount idx=0; idx < Size; idx++){
         if (Pairs[idx].GetMine()){
            delete Pairs[idx].GetObject();
            Pairs[idx].SetObject(NULL);
         }//end if  
      }//end for
      Size = 0;
   }//end if
}//end RemoveAll

//------------------------------------------------------------------------------
template <class ObjectType>
stCount stDBMCollection<ObjectType>::GetNumberOfFreeObjects(){
   int idx;
   int freeObj = 0;

   for (idx=0; idx < Size; idx++){
      if (!(Pairs[idx].GetPageID()))
         freeObj++;
   }//end for
   return freeObj;
}//end GetNumberOfFreeObjects

//------------------------------------------------------------------------------
template <class ObjectType>
stSize stDBMCollection<ObjectType>::GetUsedSpace(){
   stSize usedSpace = 0;

   usedSpace = Size * stDBMNode::GetEntryOverhead();
   // For each entry
   for (stCount idx=0; idx<Size; idx++){
      usedSpace += Pairs[idx].GetObject()->GetSerializedSize();
      // If this entry is a subtree.
      if (Pairs[idx].GetPageID()){
         // Add the radius.
         usedSpace += sizeof(stDistance);
         #ifdef __stDBMNENTRIES__
            usedSpace += sizeof(stCount);
         #endif //__stDBMNENTRIES__
      }//end if
   }//end for

   return usedSpace;
}//end GetUsedSpace

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMCollection<ObjectType>::SetMineForAllObjects(bool option){
   for (stCount idx=0; idx < Size; idx++){
      Pairs[idx].SetMine(option);
   }//end for
}//end SetMineForAllObjects

//------------------------------------------------------------------------------
template <class ObjectType>
void stDBMCollection<ObjectType>::Resize(){
   tPair * newPairs;
   bool mine;

   // store the status of objects.
   mine = Pairs[0].GetMine();
   SetMineForAllObjects(false);
	// New entry vector
	newPairs = new tPair[Capacity + Increment];
	memcpy(newPairs, Pairs, Capacity * sizeof(tPair));
	// Delete the old vector.
	delete[] Pairs;
   // Ajust the pointers.
	Pairs = newPairs;
   // restore the status of objects.
   SetMineForAllObjects(mine);
   // Update the new value of capacity.
	Capacity += Increment;
}//end Resize

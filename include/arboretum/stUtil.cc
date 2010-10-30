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
* This file has the implementaiton of all template utility classes.
*
* @version 1.0
* $Revision: 1.13 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP

//----------------------------------------------------------------------------
// Class template stGenericMatrix
//----------------------------------------------------------------------------
template < class Type >
void stGenericMatrix<Type>::SetSize(int cols, int rows){
   int i;
   int newSize;

   // New data size
   newSize = cols * rows;

   // Reallocate Data if required
   if (DataSize < newSize){
      if (Data != NULL){
         delete[] Data;
      }//end if
      Data = new Type[cols * rows];
      DataSize = newSize;
   }//end if

   if (PRowsSize < rows){
      if (PRows != NULL){
         delete[] PRows;
      }//end if
      PRows = new Type * [rows];
      PRowsSize = rows;
   }//end if

   Rows = rows;
   Cols = cols;
   PRows[0] = Data;
   for (i = 1; i < rows; i++){
      PRows[i] = PRows[i - 1] + cols;
   }//end for
}//end stGenericMatrix<Type>::SetSize
   
//----------------------------------------------------------------------------
// Class template stGenericMatrix
//----------------------------------------------------------------------------
template < class Type >
stHugeGenericMatrix<Type>::~stHugeGenericMatrix(){

   DisposeRows();
}//end stHugeGenericMatrix<Type>::~stHugeGenericMatrix

//----------------------------------------------------------------------------
template < class Type >
void stHugeGenericMatrix<Type>::SetSize(int cols, int rows){
   int i;

   if ((cols != Cols) || (rows != Rows)){
      // Kill previows matrix
      DisposeRows();

      Rows = rows;
      Cols = cols;
      PRows = new Type * [rows];
      for (i = 0; i < rows; i++){
         PRows[i] = new Type[cols];
      }//end for
   //}else{
      // Do nothing. He he he...
   }//end if
}//end stGenericMatrix<Type>::SetSize

//----------------------------------------------------------------------------
template < class Type >
void stHugeGenericMatrix<Type>::DisposeRows(){
   Type ** i;

   if (PRows != NULL){
      for (i = PRows; i < PRows + Rows; i++){
         delete[] *i;
      }//end for
      delete[] PRows;
   }//end if
}//end stGenericMatrix<Type>::DisposeRows

//----------------------------------------------------------------------------
// template class stRPriorityQueue
//----------------------------------------------------------------------------
template < class TKey, class TValue >
bool stRPriorityQueue < TKey, TValue >::Get(TKey & key, TValue & value){
   int child;
   int parent;
   
   if (size > 0){
      // Remove first
      key = entries[0].key;
      value = entries[0].value;
      size --;
      
      // Reinsert last.
      parent = 0;
      child = GetMinChild(parent);
      // Do not enable complete boolena evaluation or it will fail.
      while ((child >= 0) && (entries[size].key > entries[child].key)){
         // Move child up
         entries[parent].key = entries[child].key; 
         entries[parent].value = entries[child].value;
         parent = child;
         child = GetMinChild(parent);
      }//end while
      // Put it in place.
      entries[parent].key = entries[size].key; 
      entries[parent].value = entries[size].value;          
      return true;
   }else{
      // Empty!
      return false;
   }//end if         
}//end stRPriorityQueue::Get

//----------------------------------------------------------------------------
template < class TKey, class TValue >
void stRPriorityQueue < TKey, TValue >::Add(const TKey & key, const TValue & value){
   int child;
   int parent;
   
   // No error checking when __stDEBUG__ is not defined!
   #ifdef __stDEBUG__
   if (size < maxSize){
   #endif //__stDEBUG__    
      child = size;
      parent = (child - 1) / 2;
      // Do not enable complete boolena evaluation or it will fail.
      while ((child > 0) && (entries[parent].key > key)){
         // Move parent down.
         entries[child].key = entries[parent].key; 
         entries[child].value = entries[parent].value;
         // Next...
         child = parent;
         parent = (child - 1) / 2;
      }//end while
      
      // Add in the proper position
      entries[child].key = key;
      entries[child].value = value;
      size++;
   #ifdef __stDEBUG__
   }else{
      throw logic_error("Unable to add more entries.");
   }//end if         
   #endif //__stDEBUG__
}//end stRPriorityQueue::Add
      
//----------------------------------------------------------------------------
// template class stDynamicRPriorityQueue
//----------------------------------------------------------------------------
template < class TKey, class TValue >
bool stDynamicRPriorityQueue < TKey, TValue>::Get(
   TKey & key, TValue & value){
   int child;
   int parent;
   
   if (size > 0){
      // Remove first
      key = entries[0].key;
      value = entries[0].value;
      size--;
      
      // Reinsert last.
      parent = 0;
      child = GetMinChild(parent);
      // Do not enable complete boolena evaluation or it will fail.
      while ((child >= 0) && (entries[size].key > entries[child].key)){
         // Move child up
         entries[parent].key = entries[child].key; 
         entries[parent].value = entries[child].value;
         parent = child;
         child = GetMinChild(parent);
      }//end while
      // Put it in place.
      entries[parent].key = entries[size].key; 
      entries[parent].value = entries[size].value;
      return true;
   }else{
      // Empty!
      return false;
   }//end if         
}//end stDynamicRPriorityQueue::Get

//----------------------------------------------------------------------------
template < class TKey, class TValue >
void stDynamicRPriorityQueue < TKey, TValue>::Add(
   const TKey & key, const TValue & value){
   int child;
   int parent;

   // Resize me if required.
   if (size == maxSize){
      Resize();
   }//end if
   
   child = size;
   parent = (child - 1) / 2;
   // Do not enable complete boolena evaluation or it will fail.
   while ((child > 0) && (entries[parent].key > key)){
      // Move parent down.
      entries[child].key = entries[parent].key; 
      entries[child].value = entries[parent].value;
      // Next...
      child = parent;
      parent = (child - 1) / 2;
   }//end while
   
   // Add in the proper position
   entries[child].key = key;
   entries[child].value = value;
   size++;
}//end stDynamicRPriorityQueue::Add

//----------------------------------------------------------------------------
template < class TKey, class TValue >
void stDynamicRPriorityQueue < TKey, TValue >::Resize(){
   tEntry * newEntries;
   
   // New entry vector
   newEntries = new tEntry[maxSize + increment];
   memcpy(newEntries, entries, sizeof(tEntry) * size);
   
   // Delete old vector
   delete[] entries;
   entries = newEntries;
   this->maxSize += increment;
}//end stDynamicRPriorityQueue::Resize

//----------------------------------------------------------------------------
// template class stDynamicRReversedPriorityQueue
//----------------------------------------------------------------------------
template < class TKey, class TValue >
bool stDynamicRReversedPriorityQueue < TKey, TValue>::Get(
   TKey & key, TValue & value){
   int child;
   int parent;
   
   if (size > 0){
      // Remove first
      key = entries[0].key;
      value = entries[0].value;
      size--;
      
      // Reinsert last.
      parent = 0;
      child = GetMinChild(parent);
      // Do not enable complete boolena evaluation or it will fail.
      while ((child >= 0) && (entries[size].key < entries[child].key)){
         // Move child up
         entries[parent].key = entries[child].key;
         entries[parent].value = entries[child].value;
         parent = child;
         child = GetMinChild(parent);
      }//end while
      // Put it in place.
      entries[parent].key = entries[size].key; 
      entries[parent].value = entries[size].value;
      return true;
   }else{
      // Empty!
      return false;
   }//end if         
}//end stDynamicRReversedPriorityQueue::Get

//----------------------------------------------------------------------------
template < class TKey, class TValue >
void stDynamicRReversedPriorityQueue < TKey, TValue>::Add(
   const TKey & key, const TValue & value){
   int child;
   int parent;

   // Resize me if required.
   if (size == maxSize){
      Resize();
   }//end if
   
   child = size;
   parent = (child - 1) / 2;
   // Do not enable complete boolena evaluation or it will fail.
   while ((child > 0) && (entries[parent].key < key)){
      // Move parent down.
      entries[child].key = entries[parent].key; 
      entries[child].value = entries[parent].value;
      // Next...
      child = parent;
      parent = (child - 1) / 2;
   }//end while
   
   // Add in the proper position
   entries[child].key = key;
   entries[child].value = value;
   size++;
}//end stDynamicRReversedPriorityQueue::Add

//----------------------------------------------------------------------------
template < class TKey, class TValue >
void stDynamicRReversedPriorityQueue < TKey, TValue >::Resize(){
   tEntry * newEntries;
   
   // New entry vector
   newEntries = new tEntry[maxSize + increment];
   memcpy(newEntries, entries, sizeof(tEntry) * size);
   
   // Delete old vector
   delete[] entries;
   entries = newEntries;
   this->maxSize += increment;
}//end stDynamicRReversedPriorityQueue::Resize

//----------------------------------------------------------------------------
// Class template stInstanceCache
//----------------------------------------------------------------------------
template < class ClassType, class AllocatorType>
stInstanceCache<ClassType, AllocatorType>::~stInstanceCache(){
   int i;

   for (i = 0; i < size; i++){
      allocator->Dispose(instances[i]);
   }//end for

   delete[] instances;
   delete allocator;
}//end stInstanceCache< class ClassType, class AllocatorType>::~stInstanceCache()

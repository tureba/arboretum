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
* This file defines a set of utility classes.
*
* @version 1.0
* $Revision: 1.45 $
* $Date: 2005/03/13 19:31:39 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#ifndef __STUTIL_H
#define __STUTIL_H

#include <arboretum/stException.h>
#include <arboretum/stTypes.h>
#include <string.h>
#include <stdio.h>

using namespace std;

//----------------------------------------------------------------------------
// Debug tools
//----------------------------------------------------------------------------
#ifdef __stDEBUG__
   // Includes
   #include <iostream>
   using namespace std;
#endif //__stDEBUG__

//----------------------------------------------------------------------------
// Class template stGenericMatrix
//----------------------------------------------------------------------------
/**
* This class template implements a generic nxm matrix.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup util
*/
template < class Type > class stGenericMatrix{
   public:
      /**
      * Creates a new Matrix with 0 cols and 0 rows. Use SetSize() to change the
      * size of this matrix.
      */
      stGenericMatrix(){
         Rows = 0;
         Cols = 0;
         DataSize = 0;
         PRowsSize = 0;
         Data = NULL;
         PRows = NULL;
      }//end stGenericMatrix

      /**
      * Creates a new matrix with a given size.
      *
      * @param col Columns.
      * @param row Rows.
      */
      stGenericMatrix(int cols, int rows){
         Data = NULL;
         SetSize(cols, rows);
      }//end stGenericMatrix

      /**
      * Disposes this matrix.
      */
      ~stGenericMatrix(){

         if (Data != NULL){
            delete[] Data;
         }//end if
         if (PRows != NULL){
            delete[] PRows;
         }//end if
      }//end ~stGenericMatrix

      /**
      * Sets the size of this matrix. All stored data will be lost!
      *
      * @param col Columns.
      * @param row Rows.
      */
      void SetSize(int cols, int rows);

      /**
      * Returns the number of rows.
      */
      int GetRows(){
         return Rows;
      }//end GetRows

      /**
      * Returns the number of rows.
      */
      int GetCols(){
         return Cols;
      }//end GetCols

      /**
      * Return a position of this matrix.
      *
      * @param col Column.
      * @param row Row.
      */
      Type & Get(int row, int col){
         return (*this)[row][col];
      }//end Get

      /**
      * This operator returns a row of this matrix. You may
      * use this operator as matrix[row][col].
      *
      * @param row Row.
      */
      Type * operator [] (int row){
         return PRows[row];
      }//end operator []

   private:

      /**
      * Number of rows.
      */
      int Rows;

      /**
      * Number of columns.
      */
      int Cols;

      /**
      * True size of Data in positions.
      */
      int DataSize;

      /**
      * True size of PRows in positions.
      */
      int PRowsSize;

      /**
      * Data vector.
      */
      Type * Data;

      /**
      * Speed up pointers.
      */
      Type ** PRows;
};//end stGenericMatrix

//----------------------------------------------------------------------------
// Class template stHugeGenericMatrix
//----------------------------------------------------------------------------
/**
* This class template implements is a variant of stGenericMatrix which is able
* to store a very large matrix.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup util
*/
template < class Type > class stHugeGenericMatrix{
   public:
      /**
      * Creates a new Matrix with 0 cols and 0 rows. Use SetSize() to change the
      * size of this matrix.
      */
      stHugeGenericMatrix(){
         Rows = 0;
         Cols = 0;
         PRows = NULL;
      }//end stHugeGenericMatrix

      /**
      * Creates a new matrix with a given size.
      *
      * @param col Columns.
      * @param row Rows.
      */
      stHugeGenericMatrix(int cols, int rows){
         PRows = NULL;
         SetSize(cols, rows);
      }//end stHugeGenericMatrix

      /**
      * Disposes this matrix.
      */
      virtual ~stHugeGenericMatrix();

      /**
      * Sets the size of this matrix. All stored data will be lost!
      *
      * @param col Columns.
      * @param row Rows.
      */
      void SetSize(int cols, int rows);

      /**
      * Returns the number of rows.
      */
      int GetRows(){
         return Rows;
      }//end GetRows

      /**
      * Returns the number of rows.
      */
      int GetCols(){
         return Cols;
      }//end GetCols

      /**
      * Return a position of this matrix.
      *
      * @param col Column.
      * @param row Row.
      */
      Type & Get(int row, int col){
         return (*this)[row][col];
      }//end Get

      /**
      * This operator returns a row of this matrix. You may
      * use this operator as matrix[row][col].
      *
      * @param row Row.
      */
      Type * operator [] (int row){
         return PRows[row];
      }//end operator []

   private:

      /**
      * Number of rows.
      */
      int Rows;

      /**
      * Number of columns.
      */
      int Cols;

      /**
      * Data pointers.
      */
      Type ** PRows;

      /**
      * Disposes all rows.
      */
      void DisposeRows();
};//end stHugeGenericMatrix

//----------------------------------------------------------------------------
// Structure stDistanceIndex
//----------------------------------------------------------------------------
/**
* This structure holds a pair Distance/Index. It also defines a comparison
* operator which allows the use of sort algorithm from stdc++lib.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup util
*/
typedef struct stDistanceIndex{
   /**
   * The index.
   */
   int Index;

   /**
   * The associated distance.
   */
   double Distance;

   /**
   * The operator =.
   */
   stDistanceIndex() : Index(0), Distance(0) {};

   struct stDistanceIndex & operator = (const struct stDistanceIndex & x){
      this->Distance = x.Distance;
      this->Index = x.Index;
      return (*this);
   }//end operator =

} stDistanceIndex;

/**
* This operator compares 2 stDistanceIndex pairs. It is required by sort()
* template.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo It must be placed in stTypes.h.
* @ingroup util
*/
bool operator < (const stDistanceIndex & x, const stDistanceIndex & y);

//----------------------------------------------------------------------------
// template MinN
//----------------------------------------------------------------------------
/**
* Finds the minimum value among n values. It uses an O(N) algorithm to find it
* and returns the index of the minimum value in the array. Each entry must
* support at least the &lt; operator.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
template <class T> int MinN(int n, T * values){
   int i;
   int min;

   min = 0;
   for (i = 1; i < n; i++){
      if (values[i] < values[min]){
         min = i;
      }//end if
   }//end for

   return min;
}//end MinN

//----------------------------------------------------------------------------
// template MaxN
//----------------------------------------------------------------------------
/**
* Finds the maximum value among n values. It uses an O(N) algorithm to find it
* and returns the index of the maximum value in the array. Each entry must
* support at least the &gt; operator.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
template <class T> int MaxN(int n, T * values){
   int i;
   int min;

   min = 0;
   for (i = 1; i < n; i++){
      if (values[i] > values[min]){
         min = i;
      }//end if
   }//end for

   return min;
}//end MinN

//----------------------------------------------------------------------------
// Function TriangularInequality
//----------------------------------------------------------------------------
/**
* Tests the triangular inequality.
*
* @param ab Distance between A and B.
* @param ac Distance between A and C.
* @param bc Distance between B and C.
* @returns True if the 3 distaces don't break the triangular inequality.
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
inline bool TriangularInequality(stDistance ab, stDistance ac,
      stDistance bc){

   return (ab + ac >= bc) && (ab + bc >= ac);
}//end TriangularInequality

//----------------------------------------------------------------------------
// template class stRPriorityQueue
//----------------------------------------------------------------------------
/**
* This class template implements a generic reverse priority queue. In other words,
* the priority of an entry grows in reverse proportion to its key value.
*
* <p>The template parameter TKey must support the =, @< and @> operators and
* TValue must support = operator.
*
* <p>This reverse priority queue is implemented using a simple heap which does not
* support dynamic growning. In other words, be aware of the maximum capacity chosen
* during its creation.
*
* There is a variation of this class template, called stDynamicRPriorityQueue,
* wich supports dynamic growing of the queue in exchange of a little performance
* degradation.
*
* <p>NEVER USE THIS CLASS when the exact maximum size of the queue can not be
* predicted before the creation of an instance of this class. If unsure, use
* stDynamicRPriorityQueue instead of this class to avoid potential buffer 
* overflows.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup util
* @see stDynamicRPriorityQueue
* @warning Due to optmization issues, this class does not report memory overflows
* unless __stDEBUG__ macro is defined. Be aware of the maximum capacity of this
* queue or suffer the consequences! He he he...
*/
template <class TKey, class TValue>
class stRPriorityQueue{
   public:
      /**
      * Creates a new reverse priority queue with a given maximum capacity.
      *
      * @param maxSize Maximum size of this queue.
      */
      stRPriorityQueue(int maxSize){
         this->maxSize = maxSize;
         this->size = 0;
         entries = new tEntry[maxSize];
      }//end maxSize

      /**
      * Disposes this queue.
      */
      ~stRPriorityQueue(){
         if (entries != NULL){
            delete[] entries;
         }//end if
      }//end ~stRPriorityQueue
      
      /**
      * Gets the next pair key/value with the minimum key value. This pair
      * is removed from the queue.
      *
      * @retval key The key value.
      * @retval key The key value.
      * @return True for success or false it the queue is empty.
      */
      bool Get(TKey & key, TValue & value);
   
      /**
      * Adds a new entry to the queue. This method will fail if the number of
      * objects exceeds the maximum number of positions.
      *
      * @param key The key to be inserted.
      * @param value The value to be inserted.
      * @warning Due to optmization issues, this method does not report memory
      * overflows unless __stDEBUG__ macro is defined.
      */
      void Add(const TKey & key, const TValue & value);  
      
      /**
      * Returns the size of this queue.
      */
      int GetSize(){
         return size;
      }//end GetSize
   private:
      /**
      * This structure is used to store a key/value pair.
      */
      struct tEntry{
         /**
         * The key.
         */
         TKey key;
         /**
         * The value.
         */
         TValue value;
      };//end tEntry

      /**
      * The entries' array.
      */
      tEntry * entries;
      
      /**
      * Maximum size of this queue.
      */
      int maxSize;
      
      /**
      * Current size of this queue.
      */
      int size;      
      
      /**
      * Returns the id of the child which has the smaller key value.
      *
      * @param parent The id of the parent.
      * @return The id of the child or -1 if the parent has no child.
      */
      int GetMinChild(int parent){
         int c = (parent * 2) + 1;
         
         if ((c + 1 < size) && (entries[c + 1].key < entries[c].key)){
            return c + 1;
         }else{
            if (c < size){
               return c;
            }else{
               return -1;
            }//end if            
         }//end if
      }//end GetMinChild
};//end stRPriorityQueue

//----------------------------------------------------------------------------
// template class stDynamicRPriorityQueue
//----------------------------------------------------------------------------
/**
* This class template implements a dynamic generic reverse priority queue. In
* other words, the priority of an entry grows in reverse proportion to its key
* value.
*
* <p>The template parameter TKey must support the =, @< and @> operators and
* TValue must support = operator.
*
* <p>This class template is a variation of the class template stRPriorityQueue,
* which can grow dynamically when required. This adds a little overhead which
* degrades the performance of the original stRPriorityQueue implementation. So,
* when the maximum capacity of the queue is known before its creation, use 
* stRPriorityQueue instead of this dynamic version.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup util
* @see stRPriorityQueue
* @waning Due to optimization issues, the maximum capacity of this queue
* will grow when required but will never be reduced.
* @note It is a variation of stRPriorityQueue, not an extension of it.
*/
template <class TKey, class TValue>
class stDynamicRPriorityQueue{
   public:
      /**
      * Creates a new reverse priority queue. The initialSize is used to
      * reserve a certain number of entries in this queue while increment
      * is the number of entries to add to the capacity when the addition of
      * a new entry overflows the current capacity (resize).
      *
      * <p>To increase the performance of this class it is necessary set
      * the paramenters initialSize and increment to optimal values which can
      * be chosen according to the nature of the problem.
      *
      * <p>Usually large values for initialSize and increment tend to enhance
      * the performance in exchange of memory usage and small values will
      * optimize the memory usage in exchange of performance.
      *
      * @param initialSize Initial capacity of this queue.
      * @param increment The number of entries to add to the capacity when
      * required.
      * @note If the value of initialSize is optimal (a number greater or equal
      * to the real maximum size of the queue, the performance of this
      * implementation will be almost as fast as stRPriorityQueue.
      */
      stDynamicRPriorityQueue(int initialSize, int increment = 32){
         this->increment = increment;
         this->maxSize = initialSize;
         this->size = 0;
         entries = new tEntry[maxSize];
      }//end stDynamicRPriorityQueue

      /**
      * Disposes this queue.
      */
      ~stDynamicRPriorityQueue(){
         if (entries != NULL){
            delete[] entries;
         }//end if
      }//end ~stDynamicRPriorityQueue

      /**
      * Gets the next pair key/value with the minimum key value. This pair
      * is removed from the queue.
      *
      * @retval key The key value.
      * @retval value The value.
      * @return True for success or false it the queue is empty.
      */
      bool Get(TKey & key, TValue & value);
   
      /**
      * Adds a new entry to the queue. This method will fail if the number of
      * objects exceeds the maximum number of positions.
      *
      * @param key The key to be inserted.
      * @param value The value to be inserted.
      */
      void Add(const TKey & key, const TValue & value);  
      
      /**
      * Returns the size of this queue.
      */
      int GetSize(){
         return size;
      }//end GetSize
   private:
      /**
      * This structure is used to store a key/value pair.
      */
      struct tEntry{
         /**
         * The key.
         */
         TKey key;
         /**
         * The value1.
         */
         TValue value;
      };//end tEntry

      /**
      * The entries' array.
      */
      tEntry * entries;
      
      /**
      * Maximum size of this queue.
      */
      int maxSize;
      
      /**
      * Current size of this queue.
      */
      int size;

      /**
      * Increment.
      */
      int increment;
      
      /**
      * Returns the id of the child which has the smaller key value.
      *
      * @param parent The id of the parent.
      * @return The id of the child or -1 if the parent has no child.
      */
      int GetMinChild(int parent){
         int c = (parent * 2) + 1;
         
         if ((c + 1 < size) && (entries[c + 1].key < entries[c].key)){
            return c + 1;
         }else{
            if (c < size){
               return c;
            }else{
               return -1;
            }//end if            
         }//end if
      }//end GetMinChild
   
      /**
      * Expands the capacity of this heap when necessary by adding increment
      * entries to the current capacity.
      */
      void Resize();
      
};//end stDynamicRPriorityQueue

//----------------------------------------------------------------------------
// template class stDynamicRReversedPriorityQueue
//----------------------------------------------------------------------------
/**
* This class template implements a dynamic generic reverse priority queue. In
* other words, the priority of an entry grows in reverse proportion to its key
* value.
*
* <p>The template parameter TKey must support the =, @< and @> operators and
* TValue must support = operator.
*
* <p>This class template is a variation of the class template stRPriorityQueue,
* which can grow dynamically when required. This adds a little overhead which
* degrades the performance of the original stRPriorityQueue implementation. So,
* when the maximum capacity of the queue is known before its creation, use 
* stRPriorityQueue instead of this dynamic version.
*
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup util
* @see stRPriorityQueue
* @waning Due to optimization issues, the maximum capacity of this queue
* will grow when required but will never be reduced.
* @note It is a variation of stRPriorityQueue, not an extension of it.
*/
template <class TKey, class TValue>
class stDynamicRReversedPriorityQueue{
   public:
      /**
      * Creates a new reverse priority queue. The initialSize is used to
      * reserve a certain number of entries in this queue while increment
      * is the number of entries to add to the capacity when the addition of
      * a new entry overflows the current capacity (resize).
      *
      * <p>To increase the performance of this class it is necessary set
      * the paramenters initialSize and increment to optimal values which can
      * be chosen according to the nature of the problem.
      *
      * <p>Usually large values for initialSize and increment tend to enhance
      * the performance in exchange of memory usage and small values will
      * optimize the memory usage in exchange of performance.
      *
      * @param initialSize Initial capacity of this queue.
      * @param increment The number of entries to add to the capacity when
      * required.
      * @note If the value of initialSize is optimal (a number greater or equal
      * to the real maximum size of the queue, the performance of this
      * implementation will be almost as fast as stRPriorityQueue.
      */
      stDynamicRReversedPriorityQueue(int initialSize, int increment = 32){
         this->increment = increment;
         this->maxSize = initialSize;
         this->size = 0;
         entries = new tEntry[maxSize];
      }//end stDynamicRReversedPriorityQueue

      /**
      * Disposes this queue.
      */
      ~stDynamicRReversedPriorityQueue(){
         if (entries != NULL){
            delete[] entries;
         }//end if
      }//end ~stDynamicRReversedPriorityQueue

      /**
      * Gets the next pair key/value with the minimum key value. This pair
      * is removed from the queue.
      *
      * @retval key The key value.
      * @retval value The value.
      * @return True for success or false it the queue is empty.
      */
      bool Get(TKey & key, TValue & value);
   
      /**
      * Adds a new entry to the queue. This method will fail if the number of
      * objects exceeds the maximum number of positions.
      *
      * @param key The key to be inserted.
      * @param value The value to be inserted.
      */
      void Add(const TKey & key, const TValue & value);  
      
      /**
      * Returns the size of this queue.
      */
      int GetSize(){
         return size;
      }//end GetSize
      
   private:
      /**
      * This structure is used to store a key/value pair.
      */
      struct tEntry{
         /**
         * The key.
         */
         TKey key;
         /**
         * The value1.
         */
         TValue value;
      };//end tEntry

      /**
      * The entries' array.
      */
      tEntry * entries;
      
      /**
      * Maximum size of this queue.
      */
      int maxSize;
      
      /**
      * Current size of this queue.
      */
      int size;

      /**
      * Increment.
      */
      int increment;
      
      /**
      * Returns the id of the child which has the greater key value.
      *
      * @param parent The id of the parent.
      * @return The id of the child or -1 if the parent has no child.
      */
      int GetMinChild(int parent){
         int c = (parent * 2) + 1;
         
         if ((c + 1 < size) && (entries[c + 1].key > entries[c].key)){
            return c + 1;
         }else{
            if (c < size){
               return c;
            }else{
               return -1;
            }//end if            
         }//end if
      }//end GetMinChild
   
      /**
      * Expands the capacity of this heap when necessary by adding increment
      * entries to the current capacity.
      */
      void Resize();
      
};//end stDynamicRReversedPriorityQueue

//----------------------------------------------------------------------------
// Global Query Priority Queue
//----------------------------------------------------------------------------
/**
* This struct implements the value type used in the global priority queue used
* by queries. It implements the operator = as required by stDynamicRPriorityQueue
* and stRPriorityQueue.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup util
* @see stRPriorityQueue
* @see stDynamicRPriorityQueue
*/
typedef struct stQueryPriorityQueueValue{
   /**
   * ID of the node.
   */
   stPageID PageID;
   
   /**
   * Radius of the node.
   */
   stDistance Radius;
   
   #ifdef __stMAMVIEW__
      /**
      * Parent of this node.
      */
      stPageID Parent;
   
      /**
      * Level of this node.
      */
      int Level;  
   #endif //__stMAMVIEW__
   
   /**
   * Operator = . 
   *
   * @param v Another instance of this class.
   */
   const stQueryPriorityQueueValue & operator = (const stQueryPriorityQueueValue & v){
      
      this->PageID = v.PageID;
      this->Radius = v.Radius;
      #ifdef __stMAMVIEW__
         this->Parent = v.Parent;
         this->Level = v.Level;
      #endif //__stMAMVIEW__
      return *this;
   }//end operator =
} stQueryPQueueValue;


//----------------------------------------------------------------------------
// Random utility functions
//----------------------------------------------------------------------------
/**
* Returns a random number between 0 and 1.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup util
*/
#define drand() (((double)rand())/((double)RAND_MAX))

/**
* Returns a random number between 2 arbitrary values. This
* function will not work if <b>low</b> is larger or equal
* to <b>hi</b>.
*
* @param low The lower bound.
* @param hi The uppper bound.
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup util
*/
#define rngrand(low, hi) ((((hi) - (low)) * drand()) + (low))

/**
* Simulates a random event with a probability <b>p</b> of success. The value
* of <b>p</b> will never be verified.
*
* @param p The probability of success.
* @return True for success or false otherwise.
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup util
*/
#define randomevent(p) (drand() <= (p))

//----------------------------------------------------------------------------
// File manipulation constants and functions.
//----------------------------------------------------------------------------
#ifdef __WIN32__  
   /**
   * This macro is used to define the system directory separator.
   *
   * @author Fabio Jun Takada Chino (chino@icmc.usp.br)
   * @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
   * @ingroup util
   */
   #define DIRSEP '\\'
#else
   /**
   * This macro is used to define the system directory separator.
   *
   * @author Fabio Jun Takada Chino (chino@icmc.usp.br)
   * @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
   * @ingroup util
   */
   #define DIRSEP '/'
#endif //__WIN32__

//----------------------------------------------------------------------------
// Class template stInstanceAllocator
//----------------------------------------------------------------------------
/**
* This class template implements a generic allocator used by the stInstanceCache
* template. It uses the default constructor to create new classes and the delete
* operator to dispose them.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @warning There are classes which this concept is not suitable.
* @ingroup util
*/
template < class ClassType> class stInstanceAllocator{
   public:
      /**
      * Creates a new instance of the ClassType using the default constructor.
      */
      ClassType * Create(){
         return new ClassType();
      }//end Create
      
      /**
      * Disposes a given instance of the ClassType using the operator delete.
      */
      void Dispose(ClassType * instance){
         delete instance;
      }//end Dispose
};//end stInstanceAllocator

//----------------------------------------------------------------------------
// Class template stInstanceCache
//----------------------------------------------------------------------------
/**
* This class template implements a generic instance cache. The idea behind this
* class is to create a replacement to new and delete operations
* (Get() and Put()). The creation of a new instance is avoided if there is
* a non-used instance available otherwise a new instance is created. The same principle is
* valid to deletion but in a reverse way. In other words, it ca be used to cache
* instances of a given class to be used later.
*
* <p>To manage the class creations and deletions, there is an interface called stInstanceAllocator
* that can manage class creations and deletions. This interface defines only two methods:
*     - ClassType * Create(): Creates a new instance.
*     - void Dispose(ClassType * instance): Disposes a given instance.
* There is a default implementation called stInstanceAllocator (a template with the same name of
* the interface) which supports the creation using a constructor with no parameters.
*
* <p>This class template was designet to work with stPage instances but may be
* used in any other situations where an existing instance can be reused. This
* can make some operations faster because the new and delete operations are usually
* slow.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @warning There are classes which this concept is not suitable.
* @ingroup util
* @see stInstanceAllocator
*/
template < class ClassType, class AllocatorType> class stInstanceCache{
   public:
      /**
      * Creates a new instance of this instance cache. This method will claim
      * the owneship of the allocator.
      *
      * @param capacity Maximum number of entries in the cache.
      * @param allocator The allocator of the ClassType.
      */
      stInstanceCache(int capacity, AllocatorType * allocator){
         this->allocator = allocator;
         this->capacity = capacity;
         this->instances = new ClassType * [capacity];
         this->size = 0;
      }//end stInstance
      
      /**
      * Disposes this cache and all cached instances. The allocator is also disposed.
      */
      ~stInstanceCache();
   
      /**
      * Gets ans instance from the cache.
      */
      ClassType * Get(){
         if (size > 0){
            size--;
            return instances[size];
         }else{
            return allocator->Create();
         }//end if
      }//end Get

      /**
      * Puts an instance back into this cache.
      *
      * @param instance The instance to be put back in the cache.
      */    
      void Put(ClassType * instance){
         if (size < capacity){
            instances[size] = instance;
            size++;
         }else{
            allocator->Dispose(instance);
         }//end if
      }//end Put     

   private:

      /**
      * The stack of available instances.
      */
      ClassType ** instances;
      
      /**
      * Current number of instances in Instances.
      */
      int size;
      
      /**
      * Maximum capacity of Instances.
      */
      int capacity;   
      
      /**
      * The allocator of ClassType.
      */
      AllocatorType * allocator;
      
};//end stInstanceCache

//----------------------------------------------------------------------------
// Class stMessageString
//----------------------------------------------------------------------------
/**
* This class implements a message string. It can be used to create messages
* string that can be used to output errors and other issues.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup util
*/
class stMessageString{

   public:

      /**
      * Appends a string to this message.
      *
      * @param str The string.
      */
      void Append(const char * str){
         this->str.append(str);
      }//end Append

      /**
      * Appends a double to this message.
      *
      * @param d The double.
      */
      void Append(double d){
         char tmp[32];
         sprintf(tmp, "%f", d);
         Append(tmp);
      }//end Append

      /**
      * Appends an integer to this message.
      *
      * @param i The integer.
      */
      void Append(int i){
         char tmp[32];
         sprintf(tmp, "%d", i);
         Append(tmp);
      }//end Append

      /**
      * Clears the contents of this string.
      */
      void Clear(){
         str = "";
      }//end Clear

      /**
      * Returns the contents of this message as a null terminated string.
      */
      const char * GetStr(){
         return str.c_str();
      }//end GetStr

   private:

      /**
      * The string stream.
      */
      string str;

};//end stMessageString

#include <arboretum/stUtil.cc>
#endif //__STUTIL_H

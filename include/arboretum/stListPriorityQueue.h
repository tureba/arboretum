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
#ifndef __STLIST_H
#define __STLIST_H

#include <arboretum/stTypes.h>
#include <arboretum/stCommon.h>

//----------------------------------------------------------------------------
// class stEntry
//----------------------------------------------------------------------------
/**
* This class defines node type of stListPriorityQueue.
*
*
* @author Adriano Siqueira Arantes (arantes@icmc.usp.br)
* @version 1.0
* @ingroup util
* @todo Tests.
* @todo Documentation update.
*/
class stEntry {
   public:

      /**
      * Set the PageID of this entry.
      *
      * @param pageID the pageID.
      */
      void SetPageID(stPageID pageID){
         PageID = pageID;
      }//end SetPageID

      /**
      * Returns the PageID.
      */
      stPageID GetPageID(){
         return PageID;
      }//end GetPageID

      /**
      * Set the Distance of this entry.
      *
      * @param distance distance of this entry.
      */
      void SetDistance(stDistance distance){
         Distance = distance;
      }//end SetDistance

      /**
      * Returns the Distance.
      */
      stDistance GetDistance(){
         return Distance;
      }//end GetDistance

      /**
      * Set the Radius of this entry.
      *
      * @param radius subtree radius of this entry.
      */
      void SetRadius(stDistance radius){
         Radius = radius;
      }//end SetRadius

      /**
      * Returns the Radius.
      */
      stDistance GetRadius(){
         return Radius;
      }//end GetRadius

      /**
      * Set the Next entry.
      *
      * @param next next entry.
      */
      void SetNext(stEntry * next){
         Next = next;
      }//end SetNext

      /**
      * Returns the next Entry.
      */
      stEntry * GetNext(){
         return Next;
      }//end GetNext

   private:

      /**
      * The ID of a Page.
      */
      stPageID PageID;

      /**
      * The Distance between this object and the query object.
      */
      stDistance Distance;

      /**
      * The subtree radius of this representative.
      */
      stDistance Radius;

      /**
      * Link for the other Entry in the List.
      */
      stEntry * Next;
};//end stEntry


//----------------------------------------------------------------------------
// class stListPriorityQueue
//----------------------------------------------------------------------------
/**
* This class implements a priority queue based on chained list.
*
* <p>This priority queue is implemented using a simple chained list which.
*
* @author Marcos Rodrigues Vieira
* @version 1.0
* @ingroup util
* @todo Documentation update.
*/
class stListPriorityQueue{
   public:

      /**
      * Creates a new priority queue.
      *
      */
      stListPriorityQueue(){
         capacity = 0;
         head = NULL;
      }//end stListPriorityQueue

      /**
      * Disposes this queue.
      */
      ~stListPriorityQueue();

      /**
      * Adds a new entry to the queue.
      *
      * @param pageID The pageID to be inserted.
      * @param w  The node value in pageID to be inserted.
      * @param dist The distance value to be inserted.
      */
      void Add(stPageID pageID, stDistance distance, stDistance radius);

      /**
      * Gets the next node. This node is removed from the queue.
      *
      * @return The head node for success or NULL if the queue is empty.
      */
      stEntry * Get();

      /**
      * Returns the size of this queue.
      */
      stCount GetSize(){
         return capacity;
      }//end GetSize
      
   private:

      /**
      * Pointer to beginning of this queue.
      */
      stEntry * head;

      /**
      * Current size of this queue.
      */
      stCount capacity;

};//end stListPriorityQueue

#endif //__STLIST_H

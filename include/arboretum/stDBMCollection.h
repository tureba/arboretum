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
* $Revision: 1.4 $
* $Date: 2004/03/07 19:21:32 $
* $Author: marcos $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Review of documentation.
*/
// Copyright (c) 2003 GBDI-ICMC-USP

#ifndef __STDBMCOLLECTION_H
#define __STDBMCOLLECTION_H

#include <arboretum/stTypes.h>
#include <arboretum/stException.h>

//----------------------------------------------------------------------------
// Class template stResultPair
//----------------------------------------------------------------------------
/**
* This class defines the pair Object/Distance returned as the result of a query.
*
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup db
*/
template < class ObjectType > class stDBMEntryCollection{

   public:
      /**
      * Type of the object.
      */
      typedef ObjectType tObject;

      /**
      * Creates a new pair DBEntry.
      *
      * <P>This instance will be the owner of the object. In other words,
      * it will not dispose the object when it is no loger necessary.
      *
      * @param object The object.
      * @param height The height of the subtree.
      * @param pageID The pageID of the subtree.
      * @param nEntries The nEntries of the subtree.
      * @param radius The radius of the subtree.
      */
      stDBMEntryCollection(){
         this->Object = NULL;
         this->Mine = false;
         #ifdef __stDBMHEIGHT__
            this->Height = 0;
         #endif //__stDBMHEIGHT__
         this->PageID = 0;
         #ifdef __stDBMNENTRIES__
            this->NEntries = 0;
         #endif //__stDBMNENTRIES__
         this->Radius = 0;
      }//end stDBMEntryCollection

      /**
      * Creates a new pair DBEntry.
      *
      * <P>This instance will be the owner of the object. In other words,
      * it will not dispose the object when it is no loger necessary.
      *
      * @param object The object.
      * @param height The height of the subtree.
      * @param pageID The pageID of the subtree.
      * @param nEntries The nEntries of the subtree.
      * @param radius The radius of the subtree.
      */
      stDBMEntryCollection(tObject * object,
                          #ifdef __stDBMHEIGHT__
                             stByte height,
                          #endif //__stDBMHEIGHT__
                          stPageID pageID,
                          #ifdef __stDBMNENTRIES__
                             stCount nEntries,
                          #endif //__stDBMNENTRIES__
                          stDistance radius,
                          bool mine = true){
         this->Object = object;
         #ifdef __stDBMHEIGHT__
            this->Height = height;
         #endif //__stDBMHEIGHT__
         this->PageID = pageID;
         #ifdef __stDBMNENTRIES__
            this->NEntries = nEntries;
         #endif //__stDBMNENTRIES__
         this->Radius = radius;
         this->Mine = mine;
      }//end stDBMEntryCollection

      /**
      * This destructor method destructs the Object only if
      * it the owner of object.
      */
      ~stDBMEntryCollection(){
         if (this->Mine && Object!=NULL){
            delete Object;
            Object = NULL;
         }//end if
      }//end ~stDBMEntryCollection

      /**
      * This method set who is the owner of Object.
      *
      * @param owner if true, the owner is not the stDBMEntryCollection.
      */
      void SetMine(bool owner){
         this->Mine = owner;
      }//end SetMine

      /**
      * This method gets who is the owner of Object.
      */
      bool GetMine(){
         return Mine;
      }//end GetMine

      /**
      * This method returns the object.
      */
      tObject * GetObject(){
         return Object;
      }//end GetObject

      /**
      * This method sets the object.
      */
      void SetObject(tObject * object){
         Object = object;
      }//end SetObject

      #ifdef __stDBMHEIGHT__
         /**
         * This method gets the height.
         */
         stByte GetHeight(){
            return Height;
         }//end GetHeight

         /**
         * This method sets the height.
         */
         void SetHeight(stByte height){
            Height = height;
         }//end SetHeight
      #endif //__stDBMHEIGHT__

      /**
      * This method gets the pageID.
      */
      stPageID GetPageID(){
         return PageID;
      }//end GetPageID

      /**
      * This method sets the pageID.
      */
      void SetPageID(stPageID pageID){
         PageID = pageID;
      }//end SetPageID

      #ifdef __stDBMNENTRIES__
         /**
         * This method gets the nEntries.
         */
         stCount GetNEntries(){
            return NEntries;
         }//end GetNEntries

         /**
         * This method sets the nEntries.
         */
         void SetNEntries(stCount nEntries){
            NEntries = nEntries;
         }//end SetNEntries
      #endif //__stDBMNENTRIES__

      /**
      * This method gets the radius.
      */
      stDistance GetRadius(){
         return Radius;
      }//end GetRadius

      /**
      * This method sets the radius.
      */
      void SetRadius(stDistance radius){
         Radius = radius;
      }//end SetRadius

   private:

      /**
      * The owner of the object.
      */
      bool Mine;

      /**
      * The object.
      */
      tObject * Object;

      /**
      * Height of the subtree.
      */
      #ifdef __stDBMHEIGHT__
         stByte Height;
      #endif //__stDBMHEIGHT__

      /**
      * ID of the page.
      */
      stPageID PageID;

      /**
      * Number of entries in the sub-tree.
      */
      #ifdef __stDBMNENTRIES__
         stCount NEntries;
      #endif //__stDBMNENTRIES__

      /**
      * Radius of the sub-tree.
      */
      stDistance Radius;

};//end stDBMEntryCollection

//-----------------------------------------------------------------------------
// Class stDBMCollectionVector
//-----------------------------------------------------------------------------

/**
* This class implements a collection of object based on a vector.
*
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation.
* @ingroup db
*/
template <class ObjectType> class stDBMCollectionVector{

   public:

      /**
      * Type of the object.
      */
      typedef ObjectType tObject;

      /**
      * This type defines the stResult Pair used by this class.
      */
      typedef stDBMEntryCollection < ObjectType > tPair;

      /**
      * This method will create a new instance of this class. The parameter
      * hint is used to prepare this instance to hold at least <i>hint</i>
      * objects (it is not a upper bound limit).
      *
      * @param hint The projected number of results (default = 32).
      */
      stDBMCollectionVector(int hint = 32){
         // Reserve results
         Pairs.reserve(hint);
      }//end stDBMCollectionVector

      /**
      * This method disposes this instance and releases all allocated resources.
      */
      ~stDBMCollectionVector(){
         RemoveAll();
      }//end ~stDBMCollectionVector
      
      /**
      * This operator allow the access to a pair.
      */
      tPair * operator [] (int idx){
         return Pairs[idx];
      }//end operator []

      /**
      * This method will remove the last entry from the collection.
      */
      void RemoveLast(){
         if (Pairs.size() > 0){
            if (Pairs[Pairs.size() - 1] != NULL){
               delete Pairs[Pairs.size() - 1];
            }//end if
            Pairs.pop_back();
         }//end if
      }//end RemoveLast

      /**
      * This method will remove all entries from the collection.
      */
      void RemoveAll();

      /**
      * This method returns the number of entries in this result.
      */
      stCount GetNumberOfEntries(){
         return Pairs.size();
      }//end GetNumberOfEntries

      /**
      * This method returns the number of entries that it is not a
      * representative of a subtree.
      */
      stCount GetNumberOfFreeObjects();

      /**
      * This method returns the amount of used space for all entries.
      */
      stSize GetUsedSpace();

      /**
      * This method set the Mine option for all objects.
      */
      void SetMineForAllObjects(bool option);

      /**
      * This method adds a entry in the collection.
      *
      * @param object The object.
      * @param height The height of the subtree.
      * @param pageID The pageID of the subtree.
      * @param nEntries The nEntries of the subtree.
      * @param radius The radius of the subtree.
      * @param mine Who is the owner of object.
      */
      void AddEntry(tObject * object,
                    #ifdef __stDBMHEIGHT__
                       stByte height,
                    #endif //__stDBMHEIGHT__
                    stPageID pageID,
                    #ifdef __stDBMNENTRIES__
                       stCount nEntries,
                    #endif //__stDBMNENTRIES__
                    stDistance radius,
                    bool mine = true){
         Pairs.insert(Pairs.begin(),
                      new tPair(object,
                      #ifdef __stDBMHEIGHT__
                         height,
                      #endif //__stDBMHEIGHT__
                      pageID,
                      #ifdef __stDBMNENTRIES__
                         nEntries,
                      #endif //__stDBMNENTRIES__
                      radius,
                      mine));
      }//end AddPair

   private:

      /**
      * The vector of stDBMEntryCollection.
      */
      vector < tPair * > Pairs;

};//end stDBMCollectionVector


//-----------------------------------------------------------------------------
// Class stDBMCollection
//-----------------------------------------------------------------------------

/**
* This class implements a collection of object.
*
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation.
* @ingroup db
*/
template <class ObjectType> class stDBMCollection{

   public:

      /**
      * Type of the object.
      */
      typedef ObjectType tObject;

      /**
      * This type defines the stResult Pair used by this class.
      */
      typedef stDBMEntryCollection < ObjectType > tPair;

      /**
      * This method will create a new instance of this class. The parameter
      * hint is used to prepare this instance to hold at least <i>hint</i>
      * objects (it is not a upper bound limit).
      *
      * @param hint The projected number of results (default = 32).
      */
      stDBMCollection(stCount initialSize = 32, stCount increment = 32){
         Capacity = initialSize;
         Increment = increment;
         Size = 0;
         // Reserve results
         Pairs = new tPair[Capacity];
      }//end stDBMCollection

      /**
      * This method disposes this instance and releases all allocated resources.
      */
      ~stDBMCollection(){
         // First remove all entries.
         RemoveAll();
         // Then delete the vector.
			if (Pairs != NULL){
				delete[] Pairs;
            Pairs = NULL;
			}//end if
         Capacity = 0;         
      }//end ~stDBMCollection
      
      /**
      * This operator allow the access to a pair.
      */
      tPair * operator [] (int idx){
         return &Pairs[idx];
      }//end operator []

      /**
      * This method will remove the last entry from the collection.
      */
      void RemoveLast(){
         if (Size > 0){
            if (Pairs[Size - 1].GetMine()){
               delete Pairs[Size - 1].GetObject();
               Pairs[Size - 1].SetObject(NULL);
            }//end if
            Size--;
         }//end if
      }//end RemoveLast

      /**
      * This method will remove all entries from the collection.
      */
      void RemoveAll();

      /**
      * This method returns the number of entries in this result.
      */
      stCount GetNumberOfEntries(){
         return Size;
      }//end GetNumberOfEntries

      /**
      * This method returns the number of entries in this result.
      */
      stCount GetSize(){
         return Size;
      }//end GetSize

      /**
      * This method returns the number of entries that it is not a
      * representative of a subtree.
      */
      stCount GetNumberOfFreeObjects();

      /**
      * This method returns the amount of used space for all entries.
      */
      stSize GetUsedSpace();

      /**
      * This method set the Mine option for all objects.
      */
      void SetMineForAllObjects(bool option);

      /**
      * This method adds a entry in the collection.
      *
      * @param object The object.
      * @param height The height of the subtree.
      * @param pageID The pageID of the subtree.
      * @param nEntries The nEntries of the subtree.
      * @param radius The radius of the subtree.
      */
      void AddEntry(tObject * object,
                    #ifdef __stDBMHEIGHT__
                       stByte height,
                    #endif //__stDBMHEIGHT__
                    stPageID pageID,
                    #ifdef __stDBMNENTRIES__
                       stCount nEntries,
                    #endif //__stDBMNENTRIES__
                    stDistance radius,
                    bool mine = true);

   private:

      /**
      * The Used Size.
      */
      stCount Size;

      /**
      * The Maximum Capacity.
      */
      stCount Capacity;

		/**
		* Increment.
		*/
      stCount Increment;

      /**
      * The vector of stDBMEntryCollection.
      */
      tPair * Pairs;

		/**
		* Expands the capacity of this heap when necessary by adding increment
		* entries to the current capacity.
		*/
		void Resize();
      
};//end stDBMCollection

// Include implementation
#include "stDBMCollection.cc"

#endif //__STDBMCOLLECTION_H

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
* This file defines the SlimTree nodes.
*
* @version 1.0
* $Revision: 1.45 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
* @todo Review of documentation.
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP

#ifndef __STSLIMNODE_H
#define __STSLIMNODE_H

#include <arboretum/stPage.h>
#include <arboretum/stTypes.h>
#include <arboretum/stException.h>

//-----------------------------------------------------------------------------
// Class stSlimNode
//-----------------------------------------------------------------------------
/**
* This abstract class is the basic SlimTree node. All classes that implement
* SlimTree nodes must extend this class.
*
* <p>The main function of this class is to provide a way to identify a disk node
* and create the required node instance to manipulate the node.
*
* <p>The structure of Index Node follows:
* @image html slimnode.png "Slim node structure"
*
* <p>The <b>Header</b> holds the information about the node itself.
*     - Type: Type of this node. It may be stSlimNode::INDEX or stSlimNode::LEAF.
*     - Occupation: Number of entries in this node.
*
* <p>The <b>Node Data</b> is the segment of the node which holds the particular information
* of each type of the node. This class does not know how this information is organized.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
* @version 1.0
* @todo Documentation review.
* @see stSlimIndexNode
* @see stSlimLeafNode
* @ingroup slim
*/
class stSlimNode{
   public:
      /**
      * Node type.
      */
      enum stSlimNodeType{
         /**
         * ID of an index node.
         */
         INDEX = 0x4449, // In little endian "ID"

         /**
         * ID of a leaf node.
         */
         LEAF = 0x464C // In little endian "LF"
      };//end stNodeType

      /**
      * This method will dispose this instance and all associated resources.
      */
      virtual ~stSlimNode(){
      }//end ~stSlimNode()

      /**
      * Returns the type of this SlimTree node (Leaf or Index).
      *
      * @return the type of node.
      * @see stNodeType
      */
      stNodeType GetNodeType(){
         return Header->Type;
      }//end GetNodeType

      /**
      * Returns the associated page.
      *
      * @return The associated page.
      */
      stPage * GetPage(){
         return Page;
      }//end GetPage

      /**
      * Returns the ID of the associated page.
      *
      * @return The ID of the associated page.
      */
      stPageID GetPageID(){
         return Page->GetPageID();
      }//end GetPage

      /**
      * This is a virtual method that defines a interface for the instantiate
      * the correct specialization of this class.
      *
      * @param page The instance of stPage.
      */
      static stSlimNode * CreateNode(stPage * page);

      /**
      * Returns the number of entries in this node.
      *
      * @return the number of entries.
      * @see GetEntry()
      * @see GetObject()
      * @see GetObjectSize()
      */
      stCount GetNumberOfEntries(){
         return this->Header->Occupation;
      }//end GetNumberOfEntries

      /**
      * This is a virtual method that defines a interface in the insertion of a
      * new Object in a Node.
      *
      * @param size The size of the object in bytes.
      * @param object The object data.
      * @warning The parameter size is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The position in the vector Entries.
      * @see RemoveObject()
      */
      virtual int AddEntry(stSize size, const stByte * object) = 0;

      /**
      * Gets the serialized object. Use GetObjectSize to determine the size of
      * the object.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      */
      virtual const stByte * GetObject(stCount idx) = 0;

      /**
      * Returns the size of the object in bytes. Use GetObject() to get the
      * object data.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The size of the serialized object.
      * @see GetObject()
      */
      virtual stSize GetObjectSize(stCount idx) = 0;

      /**
      * Returns the minimum radius of this node.
      */
      virtual stDistance GetMinimumRadius() = 0;

      /**
      * Remove All entries.
      */
      void RemoveAll(){

         #ifdef __stDEBUG__
         stNodeType type;
         type = Header->Type;
         Page->Clear();
         Header->Type = type;
         #else
         this->Header->Occupation = 0;
         #endif //__stDEBUG__
      }//end RemoveAll

      /**
      * Returns the total number of objects in the subtree.
      */
      virtual stCount GetTotalObjectCount() = 0;

      /**
      * Returns the global overhead of a slim node (header size) in bytes.
      */
      static stSize GetGlobalOverhead(){
         return sizeof(stSlimNodeHeader);
      }//end GetGlobalOverhead()
      
   protected:
      /**
      * This is the structure of the Header of a SlimTree node.
      */
      #pragma pack(1)
      typedef struct stSlimNodeHeader{
         /**
         * Node type.
         */
         stNodeType Type;

         /**
         * Number of entries.
         */
         stCount Occupation;
      } stSlimNodeHeader; //end stHeader
      #pragma pack()

      /**
      * Header of this page.
      */
      stSlimNodeHeader * Header;

      /**
      * The page related with this class.
      */
      stPage * Page;

      /**
      * Creates a new instance of this class.
      *
      * @param page An instance of stPage.
      */
      stSlimNode(stPage * page){
         this->Page = page;
         Header = (stSlimNodeHeader *)(this->Page->GetData());
      }//end stSlimNode
};//end stSlimNode

//-----------------------------------------------------------------------------
// Class stSlimIndexNode
//-----------------------------------------------------------------------------
/**
* This class implements the index node of the SlimTree.
*
* <P>The SlimTree index node...
*
* <P>The structure of Index Node follows:
* @image html indexnode.png "Index node structure"
*
* <P>The <b>Header</b> holds the information about the node itself.
*     - Type: Type of this node. It is always stSlimNode::INDEX (0x4449).
*     - Occupation: Number of entries in this node.
*
* <P>The <b>Entry</b> holds the information of the link to the other node.
*  - PageID: The identifier of the page which holds the root of the sub tree.
*       - Distance: The distance of this object from the representative object.
*       - NEntries: Number of objects in the sub tree.
*       - Radius: Radius of the sub tree.
*       - Offset: The offset of the object in the page. DO NOT MODIFY ITS VALUE.
*
* <P>The <b>Object</b> is an array of bytes that holds the information required to rebuild
* the original object.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation review.
* @see stSlimNode
* @see stSlimLeafNode
* @ingroup slim
*/
// +---------------------------------------------------------------------------------------------------------------------------------------------------+
// | Type | Occupation | PgID0 | Dist0 | NEnt0 | Radius0 | OffSet0 |...|PgIDn | Distn | NEntn | Radiusn | OffSetn | <-- blankspace --> |Objn |...|Obj0 |
// +---------------------------------------------------------------------------------------------------------------------------------------------------+
class stSlimIndexNode: public stSlimNode{
   public:
      /**
      * This type represents a slim tree index node entry.
      */
      #pragma pack(1)
      typedef struct stSlimIndexEntry{
         /**
         * ID of the page.
         */
         stPageID PageID;

         /**
         * Distance from the representative.
         */
         stDistance Distance;

         /**
         * Number of entries in the sub-tree.
         */
         stCount NEntries;

         /**
         * Radius of the sub-tree.
         */
         stDistance Radius;

         /**
         * Offset of the object.
         * @warning NEVER MODIFY THIS FIELD. YOU MAY DAMAGE THE STRUCTURE OF
         * THIS NODE.
         */
         stSize Offset;
      } stSlimIndexEntry; //end stIndexEntry
      #pragma pack()

      /**
      * Creates a new instance of this class. The parameter <i>page</i> is an
      * instance of stPage that hold the node data.
      *
      * <P>The parameter <i>create</i> tells to stIndexPage what operation will
      * be performed. True means that the page will be initialized and false
      * means that the page will be used as it is. The default value is false.
      *
      * @param page The page that hold the data of this node.
      * @param create The operation to be performed.
      */
      stSlimIndexNode(stPage * page, bool create = false);

      /**
      * Returns the reference of the desired entry. You may use this method to
      * read and modify the entry information.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return stIndexEntry the reference of the desired entry.
      * @see stLeafEntry
      * @see GetNumberOfEntries()
      * @see GetObject()
      * @see GetObjectSize()
      */
      stSlimIndexEntry & GetIndexEntry(stCount idx){
         #ifdef __stDEBUG__
         if (idx >= GetNumberOfEntries()){
            throw invalid_argument("idx value is out of range.");
         }//end if
         #endif //__stDEBUG__

         return Entries[idx];
      }//end GetIndexEntry

      /**
      * Adds a new entry to this node. This method will return the idx of the new
      * node or a negative value for failure.
      *
      * <P>This method will fail if there is not enough space to hold the
      * new object.
      *
      * <P>If you have added a new entry successfully, you may edit the entry
      * fields using the method GetEntry().
      *
      * @param size The size of the object in bytes.
      * @param object The object data.
      * @warning The parameter size is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The position in the vector Entries.
      * @see RemoveEntry()
      * @see GetEntry()
      */
      virtual int AddEntry(stSize size, const stByte * object);

      /**
      * Returns the entry idx that hold the representaive object.
      */
      int GetRepresentativeEntry();

      /**
      * Gets the serialized object. Use GetObjectSize to determine the size of
      * the object.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation.
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      */
      const stByte * GetObject(stCount idx);

      /**
      * Returns the size of the object. Use GetObject() to get the object data.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The size of the serialized object.
      * @see GetObject()
      */
      stSize GetObjectSize(stCount idx);

      /**
      * Removes an entry from this object.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @see GetObjectSize()
      */
      void RemoveEntry(stCount idx);

      /**
      * Returns the minimum radius of this node.
      */
      virtual stDistance GetMinimumRadius();

      /**
      * Returns the total number of objects in the subtree.
      */
      virtual stCount GetTotalObjectCount();
      
      /**
      * Returns the overhead of each index node entry in bytes.
      */
      static stSize GetIndexEntryOverhead(){
         return sizeof(stSlimIndexEntry);
      }//end GetIndexEntryOverhead()

   private:

      /**
      * Entry pointer
      */
      stSlimIndexEntry * Entries;

      /**
      * Returns the amount of the free space in this node.
      */
      stSize GetFree();

};//end stSlimIndexPage

//-----------------------------------------------------------------------------
// Class stSlimLeafNode
//-----------------------------------------------------------------------------
/**
* This class implements the Leaf node of the SlimTree.
*
* <P>The SlimTree leaf node...
* The structure of Leaf Node follows:
* @image html leafnode.png "Leaf node structure"
*
* <P>The <b>Header</b> holds the information about the node itself.
*     - Type: Type of this node. It is always stSlimNode::LEAF (0x464C).
*     - Occupation: Number of entries in this node.
*
* <P>The <b>Entry</b> holds the information of the link to the other node.
*       - Distance: The distance of this object from the representative object.
*       - Offset: The offset of the object in the page. DO NOT MODIFY ITS VALUE.
*
* <P>The <b>Object</b> is an array of bytes that holds the information required
* to rebuild the original object.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Josiel Maimone de Figueiredo (josiel@icmc.usp.br)
* @todo Documentation review.
* @see stSlimNode
* @see stSlimIndexNode
* @see stSlimMemLeafNode
* @ingroup slim
*/
// +--------------------------------------------------------------------------------------------+
// | Type | Occupation | Dist0 | OffSet0 | Distn | OffSetn | <-- blankspace --> |Objn |...|Obj0 |
// +--------------------------------------------------------------------------------------------+
class stSlimLeafNode: public stSlimNode{
   public:
      /**
      * This type represents a slim tree leaf node entry.
      */
      #pragma pack(1)
      typedef struct stSlimLeafEntry{
         /**
         * Distance from the representative.
         */
         stDistance Distance;

         /**
         * Offset of the object.
         * @warning NEVER MODIFY THIS FIELD. YOU MAY DAMAGE THE STRUCTURE OF
         * THIS NODE.
         */
         stSize Offset;
      } stSlimLeafEntry; //end stLeafEntry
      #pragma pack()

      /**
      * Creates a new instance of this class. The paramenter <i>page</i> is an
      * instance of stPage that hold the node data.
      *
      * <P>The parameter <i>create</i> tells to stLeafPage what operation will
      * be performed. True means that the page will be initialized and false
      * means that the page will be used as it is. The default value is false.
      *
      * @param page The page that hold the data of this node.
      * @param create The operation to be performed.
      */
      stSlimLeafNode(stPage * page, bool create = false);

      /**
      * Returns the reference of the desired leaf entry. You may use this method to
      * read and modify the leaf entry information.
      *
      * @param idx The idx of the leaf entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return stLeafEntry the reference of the desired entry.
      * @see stLeafEntry
      * @see GetNumberOfEntries()
      * @see GetObject()
      * @see GetObjectSize()
      */
      stSlimLeafEntry & GetLeafEntry(stCount idx){
         #ifdef __stDEBUG__
         if (idx >= GetNumberOfEntries()){
            throw invalid_argument("idx value is out of range.");
         }//end if
         #endif //__stDEBUG__

         return Entries[idx];
      }//end GetLeafEntry

      /**
      * Adds a new entry to this node. This method will return the idx of the new
      * node or a negative value for failure.
      *
      * <P>This method will fail if there is not enough space to hold the
      * new object.
      *
      * <P>If you have added a new entry successfully, you may edit the entry
      * fields using the method GetEntry().
      *
      * @param size The size of the object in bytes.
      * @param object The object data.
      * @warning The parameter size is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The position in the vector Entries.
      * @see RemoveEntry()
      * @see GetEntry()
      */
      virtual int AddEntry(stSize size, const stByte * object);

      /**
      * Returns the entry idx that hold the representative object.
      * @return -1 if there is not a representative in the current node.
      *         Otherwise there is.
      */
      int GetRepresentativeEntry();

      /**
      * Gets the serialized object. Use GetObjectSize() to determine the size of
      * the object.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      */
      const stByte * GetObject(stCount idx);

      /**
      * Returns the size of the object. Use GetObject() to get the object data.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The size of the serialized object.
      * @see GetObject()
      */
      stSize GetObjectSize(stCount idx);

      /**
      * Removes an entry from this object.
      *
      * @param idx The idx of the entry.
      * @warning The parameter idx is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      * @todo This method is not implemented yet.
      */
      void RemoveEntry(stCount idx);

      /**
      * Returns the minimum radius of this node.
      */
      virtual stDistance GetMinimumRadius();

      /**
      * Returns the total number of objects in the subtree.
      */
      virtual stCount GetTotalObjectCount(){
         return (stCount)GetNumberOfEntries();
      }//end GetTotalObjectCount()
      
      /**
      * Returns the overhead of each leaf node entry in bytes.
      */
      static stSize GetLeafEntryOverhead(){
         return sizeof(stSlimLeafEntry);
      }//end GetLeafEntryOverhead()
      
   private:
      /**
      * Entry pointer
      */
      stSlimLeafEntry * Entries;

      /**
      * Returns the amount of the free space in this node.
      */
      stSize GetFree();
};//end stSlimLeafNode

//-----------------------------------------------------------------------------
// Class stSlimMemLeafNode
//-----------------------------------------------------------------------------
/**
* This class template implements a memory shell for a stSlimLeafNode instance.
* It implements a memory optimized leaf node wich simulates the physical
* occupation of the original node.
*
* <p>Since all objects are stored using their implementation (not the serialized
* form), this class is very useful to optimize heavy object manipulations such
* as SlimDown and MinMax algorithms.
*
* <p>By the other side, this class will unserialize all objects of the node
* without exceptions. Due to that, the use of this class is not recommended
* for procedures with low rate of object unserializations/serializations.
* Another disadvantage of this class is the potential high memory usage.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @see stSlimLeafNode
* @ingroup slim
*/
template < class ObjectType >
class stSlimMemLeafNode{
   public:

      /**
      * Creates a new stSlimMemLeafNode instance from a stSlimLeafNode.
      *
      * @param leafNode The leafNode to be insert.
      * @warning This method will remove entries from leafNode.
      */
      stSlimMemLeafNode(stSlimLeafNode * leafNode);
      
      /**
      * Release the resources of this class template and construct a new
      * slimLeafNode that has all entries in this instance.
      *
      * @note The resulting node will have all objects sorted according to
      * their distance from the representative object (in ascendent order).
      * @return A new instance of stSlimLeafNode.
      */
      stSlimLeafNode * ReleaseNode();
   
      /**
      * Adds an object to this node.
      *
      * <p>All entries are sorted in crescent order of distance to allow complex
      * object manipulation such as SlimDown.
      *
      * @param obj An object to be insert.
      * @param distance A correspondent distance of the new entry.
      * @return True for success or false otherwise.
      */
      bool Add(ObjectType * obj, stDistance distance);

      /**
      * Returns the number of entries.
      */
      stCount GetNumberOfEntries(){
         return this->numEntries;
      }//end GetNumberOfObjects

      /**
      * Returns the object at position idx.
      *
      * @param idx Index of the object.
      * @return An object at position idx.
      * @warning Do not modify/dispose the object using this method.
      */
      ObjectType * ObjectAt(stCount idx){
         #ifdef __stDEBUG__
            if (idx >= this->numEntries){
               throw invalid_argument("idx value is out of range.");
            }//end if
         #endif //__stDEBUG__
         return Entries[idx].Object;
      }//end ObjectAt

      /**
      * Returns a pointer to the last object of the node. This object is special
      * because it has the largest distance from the representative.
      *
      * @return A pointer to the last object of this node.
      */
      ObjectType * LastObject(){
         return Entries[this->numEntries-1].Object;
      }//end LastObject

      /**
      * Returns a pointer to the representative object of the node.
      *
      * @return A pointer to the representative object of this node.
      */
      ObjectType * RepObject(){
         return Entries[0].Object;
      }//end RepObject

      /**
      * Returns the distance associated with a given entry.
      *
      * @param idx Index of the object.
      * @return The distance.
      */
      stDistance DistanceAt(stCount idx){
         #ifdef __stDEBUG__
            if (idx >= this->numEntries){
               throw invalid_argument("idx value is out of range.");
            }//end if
         #endif //__stDEBUG__

         return Entries[idx].Distance;
      }//end DistanceAt

      /**
      * Returns the distance of the last object. It is a synonym of
      * GetMinimumRadius().
      */
      stDistance LastDistance(){
         return Entries[this->numEntries-1].Distance;
      }//end LastDistance

      /**
      * Removes an entry in idx from this node.
      *
      * @param idx Index of the object.
      * @return The removed object.
      */
      ObjectType * Remove(stCount idx);

      /**
      * Remove the last object from this tree.
      */
      ObjectType * PopObject();

      /**
      * Returns true if there is enough free space to add the given object.
      *
      * @return True for success or false otherwise.
      */
      bool CanAdd(ObjectType * obj){
         int entrySize;

         // Does it fit ?
         entrySize = obj->GetSerializedSize() + stSlimLeafNode::GetLeafEntryOverhead();
         if (entrySize + this->usedSize > this->maximumSize){
            // No, it doesn't.
            return false;
         }//end if
         // yes, it does.
         return true;
      }//end CanAdd

      /**
      * Returns the minimum radius of this node.
      *
      * @return The radius.
      */
      stDistance GetMinimumRadius(){
         return Entries[this->numEntries-1].Distance;
      }//end GetMinimumRadius                    

      /**
      * Returns the free space of this node.
      *
      * @return The free space.
      */
      stSize GetFreeSize(){
         return (this->maximumSize - this->usedSize);
      }//end GetFreeSize

   private:
      /**
      * This struct holds all information required to store an leaf node entry.
      */
      struct stSlimMemNodeEntry{
         /**
         * Object.
         */
         ObjectType * Object;

         /**
         * Distance from representative.
         */
         stDistance Distance;
      };

      /**
      * Number of entries in this node.
      */
      stCount numEntries;
      
      /**
      * Current capacity of this node.
      */
      stCount capacity;
      
      /**
      * Entries of this node.
      */
      stSlimMemNodeEntry * Entries;
      
      /**
      * Maximum size of the node in bytes.
      */
      stSize maximumSize;
      
      /**
      * Used size of the node in bytes.
      */
      stSize usedSize;
      
      /**
      * Point to the source leafNode.
      */
      stSlimLeafNode * srcLeafNode; 

      /**
      * Returns the insert position for a given distance.
      *
      * @param distance The distance of the object to be insert.
      * @return The position that will be insert.
      * @todo This code needs optimizations. I suggest a binary search
      * implementation.
      */
      int InsertPosition(stDistance distance); 
      
      /**
      * Resizes the entries vector to hold more entries. It will at 16
      * positions to the capacity for each call.
      */
      void Resize(stCount incSize = 16);
      
};//end stSlimMemLeafNode

// Include implementation
#include "stSlimNode.cc"

#endif //__STSLIMNODE_H

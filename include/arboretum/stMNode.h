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
* This file defines the MTree nodes.
*
* @version 1.0
* $Revision: 1.1 $
* $Date: 2004/04/27 15:58:57 $
* $Author: marcos $
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2004 GBDI-ICMC-USP

#ifndef __STMNODE_H
#define __STMNODE_H

#include <arboretum/stPage.h>
#include <arboretum/stTypes.h>
#include <arboretum/stException.h>

//-----------------------------------------------------------------------------
// Class stMNode
//-----------------------------------------------------------------------------
/**
* This abstract class is the basic MTree node. All classes that implement
* MTree nodes must extend this class.
*
* <p>The main function of this class is to provide a way to identify a disk node
* and create the required node instance to manipulate the node.
*
* <p>The structure of Index Node follows:
* @image html slimnode.png "M node structure"
*
* <p>The <b>Header</b> holds the information about the node itself.
*     - Type: Type of this node. It may be stMNode::INDEX or stMNode::LEAF.
*     - Occupation: Number of entries in this node.
*
* <p>The <b>Node Data</b> is the segment of the node which holds the particular information
* of each type of the node. This class does not know how this information is organized.
*
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @todo Documentation review.
* @see stMIndexNode
* @see stMLeafNode
* @ingroup slim
*/
class stMNode{
   public:
      /**
      * Node type.
      */
      enum stMNodeType{
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
      virtual ~stMNode(){
      }//end ~stMNode()

      /**
      * Returns the type of this MTree node (Leaf or Index).
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
      static stMNode * CreateNode(stPage * page);

      /**
      * Returns the number of entries in this node.
      *
      * @return the number of entries.
      * @see GetEntry()
      * @see GetObject()
      * @see GetObjectSize()
      */
      int GetNumberOfEntries(){
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
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      */
      virtual const stByte * GetObject(int id)=0;

      /**
      * Returns the size of the object in bytes. Use GetObject() to get the
      * object data.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The size of the serialized object.
      * @see GetObject()
      */
      virtual stSize GetObjectSize(int id)=0;

      /**
      * Returns the minimum radius of this node.
      */
      virtual stDistance GetMinimumRadius()=0;

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
      * Returns the global overhead of a slim node (header size) in bytes.
      */
      static stSize GetGlobalOverhead(){
         return sizeof(stMNodeHeader);
      }//end GetGlobalOverhead()
      
   protected:
      /**
      * This is the structure of the Header of a MTree node.
      */
      #pragma pack(1)
      typedef struct stMNodeHeader{
         /**
         * Node type.
         */
         stNodeType Type;

         /**
         * Number of entries.
         */
         stCount Occupation;
      } stMNodeHeader; //end stHeader
      #pragma pack()

      /**
      * Header of this page.
      */
      stMNodeHeader * Header;

      /**
      * The page related with this class.
      */
      stPage * Page;

      /**
      * Creates a new instance of this class.
      *
      * @param page An instance of stPage.
      */
      stMNode(stPage * page){
         this->Page = page;
         Header = (stMNodeHeader *)(this->Page->GetData());
      }//end stMNode
};//end stMNode

//-----------------------------------------------------------------------------
// Class stMIndexNode
//-----------------------------------------------------------------------------
/**
* This class implements the index node of the MTree.
*
* <P>The MTree index node...
*
* <P>The structure of Index Node follows:
* @image html indexnode.png "Index node structure"
*
* <P>The <b>Header</b> holds the information about the node itself.
*     - Type: Type of this node. It is always stMNode::INDEX (0x4449).
*     - Occupation: Number of entries in this node.
*
* <P>The <b>Entry</b> holds the information of the link to the other node.
*  - PageID: The identifier of the page which holds the root of the sub tree.
*       - Distance: The distance of this object from the representative object.
*       - Radius: Radius of the sub tree.
*       - Offset: The offset of the object in the page. DO NOT MODIFY ITS VALUE.
*
* <P>The <b>Object</b> is an array of bytes that holds the information required to rebuild
* the original object.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation review.
* @see stMNode
* @see stMLeafNode
* @ingroup slim
*/
// +-----------------------------------------------------------------------------------------------------------------------------------+
// | Type | Occupation | PgID0 | Dist0 | Radius0 | OffSet0 |...|PgIDn | Distn | Radiusn | OffSetn | <-- blankspace --> |Objn |...|Obj0 |
// +-----------------------------------------------------------------------------------------------------------------------------------+
class stMIndexNode: public stMNode{
   public:
      /**
      * This type represents a slim tree index node entry.
      */
      #pragma pack(1)
      typedef struct stMIndexEntry{
         /**
         * ID of the page.
         */
         stPageID PageID;

         /**
         * Distance from the representative.
         */
         stDistance Distance;

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
      } stMIndexEntry; //end stIndexEntry
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
      stMIndexNode(stPage * page, bool create = false);

      /**
      * Returns the reference of the desired entry. You may use this method to
      * read and modify the entry information.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return stIndexEntry the reference of the desired entry.
      * @see stLeafEntry
      * @see GetNumberOfEntries()
      * @see GetObject()
      * @see GetObjectSize()
      */
      stMIndexEntry & GetIndexEntry(int id){
         #ifdef __stDEBUG__
         if ((id < 0) && (id >= GetNumberOfEntries())){
            throw invalid_argument("id value is out of range.");
         }//end if
         #endif //__stDEBUG__

         return Entries[id];
      }//end GetIndexEntry

      /**
      * Adds a new entry to this node. This method will return the id of the new
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
      * Returns the entry id that hold the representaive object.
      */
      int GetRepresentativeEntry();

      /**
      * Gets the serialized object. Use GetObjectSize to determine the size of
      * the object.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation.
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      */
      const stByte * GetObject(int id);

      /**
      * Returns the size of the object. Use GetObject() to get the object data.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The size of the serialized object.
      * @see GetObject()
      */
      stSize GetObjectSize(int id);

      /**
      * Removes an entry from this object.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @see GetObjectSize()
      */
      void RemoveEntry(int id);

      /**
      * Returns the minimum radius of this node.
      */
      virtual stDistance GetMinimumRadius();

      /**
      * Returns the overhead of each index node entry in bytes.
      */
      static stSize GetIndexEntryOverhead(){
         return sizeof(stMIndexEntry);
      }//end GetIndexEntryOverhead()

   private:

      /**
      * Entry pointer
      */
      stMIndexEntry * Entries;

      /**
      * Returns the amount of the free space in this node.
      */
      stSize GetFree();

};//end stMIndexPage

//-----------------------------------------------------------------------------
// Class stMLeafNode
//-----------------------------------------------------------------------------
/**
* This class implements the Leaf node of the MTree.
*
* <P>The MTree leaf node...
* The structure of Leaf Node follows:
* @image html leafnode.png "Leaf node structure"
*
* <P>The <b>Header</b> holds the information about the node itself.
*     - Type: Type of this node. It is always stMNode::LEAF (0x464C).
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
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation review.
* @see stMNode
* @see stMIndexNode
* @ingroup slim
*/
// +--------------------------------------------------------------------------------------------+
// | Type | Occupation | Dist0 | OffSet0 | Distn | OffSetn | <-- blankspace --> |Objn |...|Obj0 |
// +--------------------------------------------------------------------------------------------+
class stMLeafNode: public stMNode{
   public:
      /**
      * This type represents a slim tree leaf node entry.
      */
      #pragma pack(1)
      typedef struct stMLeafEntry{
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
      } stMLeafEntry; //end stLeafEntry
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
      stMLeafNode(stPage * page, bool create = false);

      /**
      * Returns the reference of the desired leaf entry. You may use this method to
      * read and modify the leaf entry information.
      *
      * @param id The id of the leaf entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return stLeafEntry the reference of the desired entry.
      * @see stLeafEntry
      * @see GetNumberOfEntries()
      * @see GetObject()
      * @see GetObjectSize()
      */
      stMLeafEntry &GetLeafEntry(int id){
         #ifdef __stDEBUG__
         if ((id < 0) && (id >= GetNumberOfEntries())){
            throw invalid_argument("id value is out of range.");
         }//end if
         #endif //__stDEBUG__

         return Entries[id];
      }//end GetLeafEntry

      /**
      * Adds a new entry to this node. This method will return the id of the new
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
      * Returns the entry id that hold the representative object.
      */
      int GetRepresentativeEntry();

      /**
      * Gets the serialized object. Use GetObjectSize() to determine the size of
      * the object.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      */
      const stByte * GetObject(int id);

      /**
      * Returns the size of the object. Use GetObject() to get the object data.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return The size of the serialized object.
      * @see GetObject()
      */
      stSize GetObjectSize(int id);

      /**
      * Removes an entry from this object.
      *
      * @param id The id of the entry.
      * @warning The parameter id is not verified by this implementation
      * unless __stDEBUG__ is defined at compile time.
      * @return A pointer to the serialized object.
      * @see GetObjectSize()
      * @todo This method is not implemented yet.
      */
      void RemoveEntry(int id);

      /**
      * Returns the minimum radius of this node.
      */
      virtual stDistance GetMinimumRadius();

      /**
      * Returns the overhead of each leaf node entry in bytes.
      */
      static stSize GetLeafEntryOverhead(){
         return sizeof(stMLeafEntry);
      }//end GetLeafEntryOverhead()
      
   private:
      /**
      * Entry pointer
      */
      stMLeafEntry * Entries;

      /**
      * Returns the amount of the free space in this node.
      */
      stSize GetFree();
};//end stMLeafNode

#endif //__STMNODE_H

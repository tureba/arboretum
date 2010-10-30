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
* This file defines a generic array object that implements all methods required
* by the stObject interface. This object may be used in combination with the
* metric evaluators defined in the file stBasicMetricEvaluator.h.
*
* @version 2.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#ifndef __STBASICOBJECTS_H
#define __STBASICOBJECTS_H

#include <arboretum/stTypes.h>
#include <arboretum/stException.h>
#include <arboretum/stObject.h>

//==============================================================================
//  class stBasicArrayObject
//------------------------------------------------------------------------------
/**
* This class template implements an array object that can be indexed by all
* metric trees in this library.
*
* <P>This object holds an array of some type and its OID. The OID may be used to
* hold an additional information about the object (like the primary key in a
* database) and will never be used by the distance functions.
*
* <P>The template parameter <b>DataType</b> defines the data type of each unit
* of the array. It may be any C/C++ standard numeric types (integers, characters
* and floating points). The template parameter <b>OIDType</b> defines the type
* of the OID to be used. As <b>DataType</b>, it may assume any numeric type. By
* default, it will always be and <b>int</b> value.
*
* <P>The Serialized version of this class is organized as follows:<br>
* I'll provide the figure later...
*
* <P>This class template was designed to work with the metric evaluators
* defined in stBasicMetricEvaluators.h.
*
* @version 1.1
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @see stObject
* @todo Documentation review.
* @todo It needs optimizations.
* @ingroup user
*/
template <class DataType, class OIDType = int>
class stBasicArrayObject : public stObject{
   public:
      /**
      * Default constructor. It creates an array with 0 elements. Use SetSize()
      * to change the number of entries before use this instance.
      *
      * <P>This constructor is required by the stObject interface.
      */
      stBasicArrayObject(){
         Size = 0;
         TrueData = NULL;
         Data = NULL;
         OID = NULL;
      }//end stBasicArrayObject

      /**
      * Creates a new instance of this class with n entries. If the parameter
      * data is not NULL, the content pointed by it will be used to initializate
      * this instance.
      *
      * @param n Number of entries.
      * @param data The initial values of NULL (default).
      */
      stBasicArrayObject(stSize n, const DataType * data = NULL){

         // Allocate data
         Size = 0;
         TrueData = NULL;
         Data = NULL;
         OID = NULL;
         SetSize(n);

         // Initialize values
         if (data != NULL){
            SetData(data);
         }//end if
      }//end stBasicArrayObject

      /**
      * Disposes this instance and releases all associated resources.
      */
      virtual ~stBasicArrayObject(){

         if (TrueData != NULL){
            delete[] TrueData;
         }//end if
      }//end ~stBasicArrayObject

      /**
      * Copies the content of the array data to this object. The array pointed
      * by data must have at least GetSize() entries.
      *
      * @param data The array with the values.
      */
      void SetData(const DataType * data){

         memcpy(Data, data, sizeof(DataType) * Size);
      }//end SetData

      /**
      * Returns the pointer to the data array.
      */
      DataType * GetData(){
          return Data;
      }//end GetData

      /**
      * Returns the number of the elements of this array.
      */
      stSize GetSize(){
         return Size;
      }//end stSize

      /**
      * This operator can be used to access all positions of the array for
      * reading or writing.
      *
      * @param idx The index of the element.
      * @warning This operator does not check the range of idx.
      */
      DataType & operator [] (int idx){
         return Data[idx];
      }//end operator []

      /**
      * Gets the value of the element at a given position.
      *
      * @param idx The index of the desired entry.
      * @exception out_of_range If the index is out of range.
      */
      DataType Get(stCount idx);

      /**
      * Returns the OID associated with this instance.
      *
      * @warning This method returns nothing if the object size is 0.
      */
      long GetOID(){
         if (OID == NULL){
            return 0;
         }else{
            return * OID;
         }//end if
      }//end GetOID

      /**
      * Sets the OID associated with this instance.
      *
      * @param oid The new OID value.
      * @warning This method does nothing if the object size is 0.
      */
      void SetOID(OIDType oid){
         if (OID != NULL){
            *OID = oid;
         }//end if
      }//end SetOID

      // stObject interface
      /**
      * @copydoc stObject::Clone()
      */
      stBasicArrayObject * Clone(){
         stBasicArrayObject * tmp = new stBasicArrayObject(Size, Data);
         tmp->SetOID(this->GetOID());
         return tmp;
      }//end Clone

      /**
      * @copydoc stObject::GetSerializedSize()
      */
      stSize GetSerializedSize(){
         return sizeof(DataType) * Size + sizeof(OIDType);
      }//end GetSerializedSize

      /**
      * @copydoc stObject::Serialize()
      */
      const stByte * Serialize(){
         return TrueData;
      }//end Serialize

      /**
      * @copydoc stObject::Unserialize()
      */
      void Unserialize(const stByte * data, stSize dataSize){

         // Resize data
         SetSize((dataSize - sizeof(OIDType)) / sizeof(DataType));

         memcpy(TrueData, data, dataSize);
      }//end Unserialize

      /**
      * @copydoc stObject::IsEqual()
      */
      bool IsEqual(stObject * obj);
      
   private:
   
      /**
      * Data array. It points to a position in the TrueData array.
      */
      DataType * Data;

      /**
      * OID Type. It points to a position in the TrueData array.
      */
      OIDType * OID;

      /**
      * This is the true data array. Both OID and Data points
      * to positions in this array.
      */
      stByte * TrueData;

      /**
      * Number of entries.
      */
      stSize Size;

      /**
      * Sets the size of this array. All previous values will be lost.
      *
      * @param n The new number of objects of this array.
      */
      void SetSize(stSize n){

         if (Size != n){
            if (TrueData != NULL){
               delete[] TrueData;
            }//end if

            // Create array.
            TrueData = new stByte[(sizeof(DataType) * n) + sizeof(OIDType)];
            Size = n;
            OID = (OIDType *)TrueData;
            Data = (DataType *) (TrueData + sizeof(OIDType));
            *OID = 0; // Set OID to 0
         }//end if
      }//end SetSize
};//end stBasicArrayObject


//==============================================================================
//  class stBasicStringObject
//------------------------------------------------------------------------------
/**
* This class template implements an string object that can be indexed by all
* metric trees in this library.
*
* <P>This object holds a null terminated string of some type and a OID. The OID
* may be used to hold an additional information about the object (like the
* primary key in a database) and will never be used by the distance functions.
*
* <P>The template parameter <b>DataType</b> defines the data type of each unit
* of the array. It may be any C/C++ standard numeric types (integers, characters
* and floating points). The template parameter <b>OIDType</b> defines the type
* of the OID to be used. As <b>DataType</b>, it may assume any numeric type. By
* default, it will always be and <b>int</b> value.
*
* <P>The Serialized version of this class is organized as follows:<br>
* <CODE>
* +-----+------+<BR>
* | OID | Data |<BR>
* +-----+------+<BR>
* </CODE>
*
* Since Data has a variable size (associated with the string object), the
* serialized form will also have a variable number of bytes.
*
* <P>This class template was designed to work with the metric evaluators
* defined in stBasicMetricEvaluators.h.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @see stObject
* @ingroup user
*/
template <class DataType, class OIDType = int>
class stBasicStringObject{
   public:
      /**
      * Default constructor. It creates an array with 0 elements. Use SetSize()
      * to change the number of entries before use this instance.
      *
      * <P>This constructor is required by the stObject interface.
      */
      stBasicStringObject(){
         Len = 0;
         TrueLen = 0;
         TrueData = NULL;
         Data = NULL;
         OID = NULL;
      }//end stBasicStringObject

      /**
      * Creates a new instance of this class and initializes it with a given
      * null terminated string.
      *
      * @param str A null terminated string.
      */
      stBasicStringObject(const DataType * str){

         TrueLen = 0;
         Len = 0;
         Data = NULL;
         OID = NULL;
         TrueData = NULL;
         // Initializing...
         SetString(str);
      }//end stBasicStringObject

      /**
      * Creates a new instance of this class and initializes it with a given
      * null terminated string.
      *
      * @param str A null terminated string.
      */
      stBasicStringObject(stBasicStringObject * str){

         TrueLen = 0;
         TrueData = NULL;
         Len = str->Len;
         SetSize(Len);

         // Initializing...
         memcpy(TrueData, str->TrueData, sizeof(DataType) * (Len + 1) + sizeof(OIDType));
      }//end stBasicStringObject

      /**
      * Disposes this instance and releases all associated resources.
      */
      virtual ~stBasicStringObject(){

         if (TrueData != NULL){
            delete[] TrueData;
         }//end if
      }//end ~stBasicStringObject

      /**
      * Returns the length of this string. It is a synonym to GetLength().
      * This name is used by the basic metric evaluators templates.
      */
      stSize GetSize(){
         return Len;
      }//end GetSize

      /**
      * Returns the length of this string. It is a synonym to GetSize().
      */
      stSize GetLength(){
         return Len;
      }//end GetLength

      /**
      * This operator can be used to access all positions of the array for
      * reading or writing.
      *
      * @param idx The index of the element.
      * @warning This operator does not check the range of idx.
      */
      DataType & operator [] (int idx){
         return Data[idx];
      }//end operator []

      /**
      * Gets the value of the element at a given position.
      *
      * @param idx The index of the desired entry.
      * @exception out_of_range If the index is out of range.
      */
      DataType Get(int idx){
         if ((idx < 0) || (idx >= Len)){
            throw out_of_range("Index out of range.");
         }else{
            return Data[idx];
         }//end if
      }//end Get

      /**
      * Returns the OID associated with this instance.
      *
      * @warning This method returns nothing if the object size is 0.
      */
      OIDType GetOID(){

         if (OID == NULL){
            return 0;
         }else{
            return *OID;
         }//end if
      }//end GetOID

      /**
      * Sets the OID associated with this instance.
      *
      * @param OID The new OID value.
      * @warning This method does nothing if the object size is 0.
      */
      void SetOID(OIDType oid){
         if (OID != NULL){
            *OID = oid;
         }//end if
      }//end SetOID

      /**
      * Copies a null terminated string to this instance.
      *
      * @param str A null terminates string.
      * @see GetString()
      */
      void SetString(const DataType * str){

         Len = StrLen(str);
         SetSize(Len);
         memcpy(Data, str, sizeof(DataType) * (Len + 1));
      }//end SetString

      /**
      * Returns the contents of this object as a null terminated string.
      *
      * @warning This method is meaningless if the contents of this instance is
      * not a null terminated string.
      * @see SetString()
      */
      const DataType * GetString(){
         return Data;
      }//end GetString

      // stObject interface
      /**
      * @copydoc stObject::Clone()
      */
      stBasicStringObject * Clone(){
         stBasicStringObject * tmp = new stBasicStringObject(this);
         tmp->SetOID(this->GetOID());
         return tmp;
      }//end Clone

      /**
      * @copydoc stObject::GetSerializedSize()
      */
      stSize GetSerializedSize(){
         return (sizeof(DataType) * Len) + sizeof(OIDType);
      }//end GetSerializedSize

      /**
      * @copydoc stObject::Serialize()
      */
      const stByte * Serialize(){
         return TrueData;
      }//end Serialize

      /**
      * @copydoc stObject::Unserialize()
      */
      void Unserialize(const stByte * data, stSize dataSize){

         // How many entries
         Len = (dataSize - sizeof(OIDType)) / sizeof(DataType);

         // Resize TrueData to accomodate the new object
         SetSize(Len);
         memcpy(TrueData, data, dataSize);
         Data[Len] = '\0'; // Null!!!
      }//end Unserialize

      /**
      * @copydoc stObject::IsEqual()
      */
      bool IsEqual(stBasicStringObject * obj);

   private:
   
      /**
      * Data array. It points to a position in the TrueData array.
      */
      DataType * Data;

      /**
      * OID Type. It points to a position in the TrueData array.
      */
      OIDType * OID;

      /**
      * This is the true data array. Both OID and Data points
      * to positions in this array. It contains the OID and
      * Data array. It is used for the Serialized method. 
      */
      stByte * TrueData;

      /**
      * String length of the Data used.
      */
      stSize Len;

      /**
      * True size of the vector data (Data).
      */
      stSize TrueLen;

      /**
      * Calculates the size of a NULL terminates string.
      *
      * @param str The string.
      */
      int StrLen(const DataType * str);

      /**
      * Resizes the string if required. Only the OID value will be preserved.
      *
      * @param len The new length.
      */
      void SetSize(stSize len){
         // Do not modify Len.
         OIDType oldOid;

         // This string will grow forever ! Or at least until it is not discarded.
         if (TrueLen < len + 1){
            // I must resize it.
            if (TrueData != NULL){
               oldOid = *OID;
               delete[] TrueData;
            }else{
               oldOid = 0;
            }//end if

            // Allocate
            TrueData = new stByte[(sizeof(DataType) * (len + 1)) + sizeof(OIDType)];
            TrueLen = len;
            OID = (OIDType*)TrueData;
            Data = (DataType*)(TrueData + sizeof(OIDType));
            *OID = oldOid;
         }//end if
      }//end SetSize
      
};//end stBasicStringObject

// Include implementation
#include "stBasicObjects.cc"

#endif //__STBASICOBJECTS_H

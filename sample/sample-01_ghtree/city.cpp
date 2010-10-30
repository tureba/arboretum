//---------------------------------------------------------------------------
// city.cpp - Implementation of the User Layer
//
// In this file we have the implementation of TCity::Serialize(),
// TCity::Unserialize() and an output operator for TCity (which is not required
// by user layer).
//
// Authors: Marcos Rodrigues Vieira (mrvieira@icmc.sc.usp.br)
//         Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
// Copyright (c) 2003 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#pragma hdrstop
#include "city.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
// Class TCity
//---------------------------------------------------------------------------
/**
* Returns the serialized version of this object.
* This method is required  by  stObject interface.
* @warning If you don't know how to serialize an object, this methos may
* be a good example.
*/
const stByte * TCity::Serialize(){
   double * d;

   // Is there a seralized version ?
   if (Serialized == NULL){
      // No! Lets build the serialized version.

      // The first thing we need to do is to allocate resources...
      Serialized = new stByte[GetSerializedSize()];

      // We will organize it in this manner:
      // +----------+-----------+--------+
      // | Latitude | Longitude | Name[] |
      // +----------+-----------+--------+
      // So, write the Longitude and Latitude should be written to serialized
      // version as follows
      d = (double *) Serialized; // If you ar not familiar with pointers, this
                                 // action may be tricky! Be careful!
      d[0] = Latitude;
      d[1] = Longitude;

      // Now, write the name after the 2 doubles...
      memcpy(Serialized + (sizeof(double) * 2), Name.c_str(), Name.length());
   }//end if

   return Serialized;
}//end TCity::Serialize

/**
* Rebuilds a serialized object.
* This method is required  by  stObject interface.
*
* @param data The serialized object.
* @param datasize The size of the serialized object in bytes.
* @warning If you don't know how to serialize an object, this methos may
* be a good example.
*/
void TCity::Unserialize(const stByte *data, stSize datasize){
   double * d;
   stSize strl;

   // This is the reverse of Serialize(). So the steps are similar.
   // Remember, the format of the serizalized object is
   // +----------+-----------+--------+
   // | Latitude | Longitude | Name[] |
   // +----------+-----------+--------+

   // Read Longitude and Latitude
   d = (double *) data;  // If you ar not familiar with pointers, this
                         // action may be tricky! Be careful!
   Latitude = d[0];
   Longitude = d[1];

   // To read the name, we must discover its size first. Since it is the only
   // variable length field, we can get it back by subtract the fixed size
   // from the serialized size.
   strl = datasize - (sizeof(double) * 2);

   // Now we know the size, lets get it from the serialized version.
   Name.assign((char *)(data + (sizeof(double) * 2)), strl);

   // Since we have changed the object contents, we must invalidate the old
   // serialized version if it exists. In fact we, may copy the given serialized
   // version of tbe new object to the buffer but we don't want to spend memory.
   if (Serialized != NULL){
      delete [] Serialized;
      Serialized = NULL;
   }//end if
}//end TCity::Unserialize

//---------------------------------------------------------------------------
// Output operator
//---------------------------------------------------------------------------
/**
* This operator will write a string representation of a city to an outputstream.
*/
ostream & operator << (ostream & out, TCity & city){

   out << "[City=" << city.GetName() << ";Lat=" <<
         city.GetLatitude() << ";Lon=" <<
         city.GetLongitude() << "]";
   return out;
}//end operator <<


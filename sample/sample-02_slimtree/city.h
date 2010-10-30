//---------------------------------------------------------------------------
// city.h - Implementation of the User Layer
//
// This file implements the 2 classes required by the SlimTree Library User
// Layer.
//
// TCity is the object which will be indexed by a metric tree. It abstracts a
// city. Each city has a name, a latitude and a longitude. TCity defines an
// interface to manipulate its information and also implements the stObject
// interface.
//
// TCityDistanceEvaluator implements the stMetricEvaluator interface. It will
// mesure the distance between 2 TCity instances.
//
// With these classes, it is possible to define and instantiate any metric tree
// defined by the SlimTree Library.
//
// Authors: Marcos Rodrigues Vieira (mrvieira@icmc.sc.usp.br)
//          Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
// Copyright (c) 2002 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#ifndef cityH
#define cityH

#include <math.h>
#include <string>
#include <ostream>
using namespace std;

// Metric Tree includes
#include <arboretum/stUserLayerUtil.h>
#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>

//---------------------------------------------------------------------------
// Class TCity
//---------------------------------------------------------------------------
/**
* This class abstracts a city in a map. Each city has a name and a pair
* latitude/longitude.
*
* <P>In addition to data manipulation methods (such as GetLatitude(), GetName()
* and others), this class implements the stObject interface. This interface
* qualifies this object to be indexed by a metric tree implemented by GBDI
* SlimTree Library.
*
* <P>This interface requires no inheritance (because of the use of class
* templates in the Structure Layer) but requires the following methods:
*     - TCity() - A default constructor.
*     - Clone() - Creates a clone of this object.
*     - IsEqual() - Checks if this instance is equal to another.
*     - GetSerializedSize() - Gets the size of the serialized version of this object.
*     - Serialize() - Gets the serialzied version of this object.
*     - Unserialize() - Restores a serialzied object.
*
* <P>Since the array which contains the serialized version of the object must be
* created and destroyed by each object instance, this class will hold this array
* as a buffer of the serialized version of this instance. This buffer will be
* created only if required and will be invalidated every time the object changes
* its values.
*
* <P>The serialized version of the object will be created as follows:<BR>
* <CODE>
* +----------+-----------+--------+<BR>
* | Latitude | Longitude | Name[] |<BR>
* +----------+-----------+--------+<BR>
* </CODE>
*
* <P>Latitude and Logitude are stored as doubles (2 64-bit IEEE floating point
* value) and Name[] is an array of chars with no terminator. Since Name[] has
* a variable size (associated with the name of the city), the serialized form
* will also have a variable number of bytes.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
*/
class TCity{
   public:
      /**
      * Default constructor. It creates a city with no name and longitude and
      * latitude set to 0. This constructor is required by stObject interface.
      */
      TCity(){
         Name = "";
         Latitude = 0;
         Longitude = 0;

         // Invalidate Serialized buffer.
         Serialized = NULL;
      }//end TCity

      /**
      * Creates a new city.
      *
      * @param name The name of the city.
      * @param latitude Latitude.
      * @param longitude Longitude.
      */
      TCity(const string name, double latitude, double longitude){
         Name = name;
         Latitude = latitude;
         Longitude = longitude;

         // Invalidate Serialized buffer.
         Serialized = NULL;
      }//end TCity

      /**
      * Destroys this instance and releases all associated resources.
      */
      ~TCity(){

         // Does Serialized exist ?
         if (Serialized != NULL){
            // Yes! Dispose it!
            delete [] Serialized;
         }//end if
      }//end TCity

      /**
      * Gets the latitude of the city.
      */
      double GetLatitude(){
         return Latitude;
      }//end GetLatitude

      /**
      * Gets the longitude of the city.
      */
      double GetLongitude(){
         return Longitude;
      }//end GetLongitude

      /**
      * Gets the name of the city.
      */
      const string & GetName(){
         return Name;
      }//end GetName

      // The following methods are required by the stObject interface.
      /**
      * Creates a perfect clone of this object. This method is required by
      * stObject interface.
      *
      * @return A new instance of TCity wich is a perfect clone of the original
      * instance.
      */
      TCity * Clone(){
         return new TCity(Name, Latitude, Longitude);
      }//end Clone

      /**
      * Checks to see if this object is equal to other. This method is required
      * by  stObject interface.
      *
      * @param obj Another instance of TCity.
      * @return True if they are equal or false otherwise.
      */
      bool IsEqual(TCity *obj){

         return (Latitude == obj->GetLatitude()) &&
               (Longitude == obj->GetLongitude());
      }//end IsEqual

      /**
      * Returns the size of the serialized version of this object in bytes.
      * This method is required  by  stObject interface.
      */
      stSize GetSerializedSize(){

         return (sizeof(double) * 2) + Name.length();
      }//end GetSerializedSize

      /**
      * Returns the serialized version of this object.
      * This method is required  by  stObject interface.
      *
      * @warning If you don't know how to serialize an object, this methos may
      * be a good example.
      */
      const stByte * Serialize();

      /**
      * Rebuilds a serialized object.
      * This method is required  by  stObject interface.
      *
      * @param data The serialized object.
      * @param datasize The size of the serialized object in bytes.
      * @warning If you don't know how to serialize an object, this methos may
      * be a good example.
      */
      void Unserialize (const stByte *data, stSize datasize);
   private:
      /**
      * The name of the city.
      */
      string Name;

      /**
      * City's longitude.
      */
      double Longitude;

      /**
      * City's latitude.
      */
      double Latitude;

      /**
      * Serialized version. If NULL, the serialized version is not created.
      */
      stByte * Serialized;
};//end TCity

//---------------------------------------------------------------------------
// Class TCityDistanceEvaluator
//---------------------------------------------------------------------------
/**
* This class implements a metric evaluator for TCity instances. It calculates
* the distance between cities by performing a euclidean distance between city
* coordinates (I know it is not accurate but is is only a sample!!!).
*
* <P>It implements the stMetricEvaluator interface. As stObject interface, the
* stMetricEvaluator interface requires no inheritance and defines 2 methods:
*     - GetDistance() - Calculates the distance between 2 objects.
*     - GetDistance2()  - Calculates the distance between 2 objects raised by 2.
*
* <P>Both methods are defined due to optmization reasons. Since euclidean
* distance raised by 2 is easier to calculate, It will implement GetDistance2()
* and use it to calculate GetDistance() result.
*
* @version 1.0
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
*/
class TCityDistanceEvaluator : public stMetricEvaluatorStatistics{
   public:
      /**
      * Returns the distance between 2 cities. This method is required by
      * stMetricEvaluator interface.
      *
      * @param obj1 Object 1.
      * @param obj2 Object 2.
      */
      stDistance GetDistance(TCity *obj1, TCity *obj2){
         return sqrt(GetDistance2(obj1, obj2));
      }//end GetDistance

      /**
      * Returns the distance between 2 cities raised by the power of 2.
      * This method is required by stMetricEvaluator interface.
      *
      * @param obj1 Object 1.
      * @param obj2 Object 2.
      */
      stDistance GetDistance2(TCity *obj1, TCity *obj2){
         double delta1, delta2;

         UpdateDistanceCount(); // Update Statistics

         delta1 = obj1->GetLatitude() - obj2->GetLatitude();
         delta2 = obj1->GetLongitude() - obj2->GetLongitude();
         return (delta1 * delta1) + (delta2 * delta2);
      }//end GetDistance2

};//end TCityDistanceEvaluator

//---------------------------------------------------------------------------
// Output operator
//---------------------------------------------------------------------------
/**
* This operator will write a string representation of a city to an outputstream.
*/
ostream & operator << (ostream & out, TCity & city);

#endif //end myobjectH

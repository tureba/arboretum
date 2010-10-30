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
* This file defines the class template stFastMapper and stFastMapImage.
*
* @version 1.0
* $Revision: 1.19 $
* $Date: 2004/12/13 13:23:59 $
*
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#ifndef __STFASTMAPPER_H
#define __STFASTMAPPER_H


//----------------------------------------------------------------------------
// Simple euclidean distance
//----------------------------------------------------------------------------
/**
* Returns the euclidean distance of 2 vectors raised by the power of 2.
*
* @param a The vector a.
* @param b The vector b.
* @param n The number of dimensions.
* @returns The Euclidean distance raised by the power of 2.
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @ingroup fastmap
*/
template <class T> T Euclidean2(const T * a, const T * b, int n){
   int i;
   T tmp;
   T d;

   d = 0;
   for (i = 0; i < n; i++){
      tmp = a[i] - b[i];
      d += tmp * tmp;
   }//end for

   return d;
}//end Eucliden2

//------------------------------------------------------------------------------
// Class stFastMapImage
//------------------------------------------------------------------------------
/**
* This class template is a utility class for stFastMapper which can hold an
* object image (the projected coordinates and a reference to the original
* object). It is used during the pivot election and to hold the pivot
* information inside an instance of stFastMapper.
*
* <p>The object type (ObjectType) must implement at least a method named
* %Clone() which can create a perfect clone of the object.
*
* <p>This class template is a companion of stFastMapper and is not supposed
* to be used by other classes.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @version 1.0
* @ingroup fastmap
* @warning This class template used by stFastMapper and should not be used by
* other classes. 
*/
template <class ObjectType>
class stFastMapImage{
   public:
      /**
      * Creates a new instance of this class.
      *
      * @param object The associated object.
      * @param dim The number of dimensions.
      * @param ownership A flag which tells this image to take ownership of the
      * given object.
      */
      stFastMapImage(ObjectType * object, int dim, bool ownership = false){

         // Initialize
         Object = object;
         Mine = ownership;
         Dim = dim;
         Coords = new double[dim];
      }//end stFastMapImage

      /**
      * Disposes this instance. The object associated with this image will
      * be deleted if this instance owns it. 
      */
      virtual ~stFastMapImage(){

         if (Coords != NULL){
            delete[] Coords;
         }//end if

         if ((Object != NULL) && (Mine)){
            delete Object;
         }//end if
      }//end ~stFastMapImage

      /**
      * Returns the object associated with this instance.
      */
      const ObjectType * GetObject(){
         return Object;
      }//end GetObject

      /**
      * Changes the object. Used for recycle purposes.
      *
      * @param obj The new object.
      * @param ownership A flag which tells this image to take ownership of the
      * given object.
      */
      void SetObject(ObjectType * obj, bool ownership = false){

         if ((Object != NULL) && (Mine)){
            delete Object;
         }//end if

         Object = obj;
         Mine = ownership;
      }//end GetObject

      /**
      * This operator allows the direct access to each image coordinate.
      */
      double & operator [] (int idx){
         return Coords[idx];
      }//end operator []

      /**
      * Returns all coordinates. The output vector must have at least dim
      * positions.
      *
      * @retval coords The output vector.
      */
      void GetCoords(double * coords){
         memcpy(coords, Coords, sizeof(double) * Dim);
      }//end GetCoords

      /**
      * Returns all coordinates in a vector. Do NOT modify these values using
      * this method.
      */
      const double * GetCoords(){
         return Coords;
      }//end GetCoords

      /**
      * Sets all coordinates of this image using the input vector of values.
      *
      * @param coords The input vector.
      */
      void SetCoords(const double * coords){
         memcpy(Coords, coords, sizeof(double) * Dim);
      }//end GetCoords

      /**
      * Sets all coordinates to a given value.
      *
      * @param value A value which will be used to initialize all coordinates.
      */
      void SetCoordsTo(double value = 0);

      /**
      * Returns the number of dimensions of this image.
      */
      int GetDimension(){
         return Dim;
      }//end GetDimension

      /**
      * This method creates a perfect clone of this object.
      */
      stFastMapImage * Clone(){
         stFastMapImage * c;

         c = new stFastMapImage(Object->Clone(), Dim, true);
         c->SetCoords(Coords);
         return c;
      }//end Clone
   private:
      /**
      * The number of dimensions of this image.
      */
      int Dim;

      /**
      * The coordinate vector.
      */
      double * Coords;

      /**
      * The related object.
      */
      ObjectType * Object;

      /**
      * Is the object mine ?
      */
      bool Mine;
};//end stFastMapImage

//------------------------------------------------------------------------------
// stFastMapper
//------------------------------------------------------------------------------
/**
* This class implements the FastMap algorithm as an object mapper. In other words,
* it can calculate the map (FastMap image) of a single object according to a 
* predefined set of pivots. The most common use of the stFastMapper can be resumed
* as follows:
*     - create an instance of stFastMapper;
*     - initialize the pivot set;
*     - map objects;
*     - dispose the instance.
*
* <p>In the creation of an instance, it is possible to determine the number of
* dimensions of the target Euclidean space and the number of interactions performed
* by the FindMostDistantObject() as described in the original FastMap article.
*
* <p>To initialize the pivot set, this implementation of FastMap provides 2 distinct
* ways. The first one is based on a selection from a set of candidate objects while
* the second allows users to set their own set of pivots without the need of the
* expensive pivot selection process.
*
* <p>The first approach requires a set of representative objects from dominion
* espace (all if possible) to be the possible candidates for pivots. They must
* be passed to the method ChoosePivots() that will choose the pivots from this
* set in order to initialize the mapper.
*
* <p>The second approach let the user to determine each pivot directly. In other
* words, it may be used to set the set of pivots when the selection process is
* note necessary because the best set of pivots is already known. It is done
* by methods SetPivot() and UpdatePivotMaps().
*
* <p>The most common use of this feature is to rebuild a previously created
* instance of stFastMapper using a saved set of pivots which were choosen
* im previous run by an execution of ChoosePivots().
*
* <p>An additional function of this class is the ability to calculate the stress
* generated by a given map. It is done by the method GetStress() which takes some
* informations acquired by the ChoosePivots() method.
*
* <p>This class template takes 2 parameters, one to determine the object type
* and other to determine the metric evaluator to be used. The object type is
* supposed to follow the stObject interface but, in fact, it only requires the
* %Clone() method. The evaluator type follows the stMetricEvaluator interface.
*
* <p>The stFastMapper was originally designed to be used by MAMView Extraction
* Module.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
*
* @version 1.0
* @ingroup fastmap
*/
template <class ObjectType, class EvaluatorType>
class stFastMapper{
   public:
      /**
      * Creates a new fastmapper for a given number of dimensions.
      * By default, the number of dimensions will be 3.
      *
      * @param eval The evaluator to be used. This instance will never be
      * destroyed.
      * @param dim The number of dimensions.
      * @param dLoopCount Number of loops to perform during the selection of the
      * most distant object.
      */
      stFastMapper(EvaluatorType * eval, int dim = 3, int dLoopCount = 5){
         // Initialize
         Eval = eval;
         Dim = dim;
         DLoopCount = dLoopCount;

         // Make room for pivots
         PivotDist = new double[Dim];
         PivotDist2 = new double[Dim];
         Pivots = new stFastMapImage<ObjectType> * [Dim * 2];
         memset(Pivots, 0, sizeof(stFastMapImage<ObjectType> *) * Dim * 2);
      }//end stFastMapper

      /**
      * Disposes this fastmapper.
      */
      ~stFastMapper();

      /**
      * This method will map an object using the current pivots. This method
      * will claim the ownership of the given object.
      *
      * @param obj The object to be mapped.
      * @retval map The object image.
      * @warning The returned image must be destroyed by user.
      */
      void Map(ObjectType * obj, double * map);

      /**
      * This method returns the number of dimensions of this FastMapper.
      */
      int GetDimensions(){
         return Dim;
      }//end GetDimensions
      
      /**
      * Chooses the fastmap pivos in a set of candidates. The number of candidates
      * must be at least (do not forget to put the value here).
      *
      * <p>The parameter objs must point to a representative subset of the
      * object set to allow the selection of a good set of pivots because it
      * will determine the quality of the mapper. To avoid pointer problems, the
      * objects in objs will never be modified or deleted.
      *
      * <p>Both maps and pivotsids are optional returning values. They exist
      * for enthusiastics who do not want to loose processing time by remapping
      * these candidates. They will be ignored when NULL.
      *
      * <p>maps must have at least n * GetDimension() positions and will contain
      * a sequence of concatenated maps. pivotids must have at least 2 *
      * GetDimension() positions.
      *
      * @param objs The candidates to pivot.
      * @param n The number of candidates.
      * @retval maps The maps of all candidates.
      * @retval pivotids The indexes of the selected pivotsin the original vector
      * of candidates.
      *
      * @see SetPivot()
      */
      void ChoosePivots(ObjectType ** objs, int n,
            double * maps = NULL, int * pivotids = NULL);

      /**
      * This method converts a given pivot identification to its correct
      * pivot id.
      *
      * @param axis The number of the axis. Values from 0 to GetDimensions() - 1.
      * @param name The name of the pivot. Use 0 for A and 1 for B.
      */
      int GetPivotIdx(int axis, int name){
         return (axis * 2) + name;
      }//end GetPivotIdx

      /**
      * Returns the object of the pivot identified by idx. To create the
      * idx value from the pivot description, use GetPivotIdx().
      *
      * @param idx The id of the pivot.
      * @see GetPivotIdx()
      */
      const ObjectType * GetPivotObject(int idx){
         if (Pivots[idx] != NULL){
            return Pivots[idx]->GetObject();
         }else{
            return NULL;
         }//end if
      }//end GetPivotObject

      /**
      * Returns the map of the pivot identified by idx. To create the
      * idx value from the pivot description, use GetPivotIdx().
      *
      * <p>The vector map must have at least GetDimension() positions.
      *
      * @param idx The id of the pivot.
      * @retval map The map of the pivot.
      * @see GetPivotIdx()
      */
      void GetPivotMap(int idx, double * map){
         if (Pivots[idx] != NULL){
            Pivots[idx]->GetCoords(map);
         }//end if
      }//end GetPivotMap

      /**
      * Determines the object and the map of the pivot identified by idx.
      * This method is used to initializes this mapper with a given set of
      * pivots.
      *
      * <p>After all pivots are set, call UpdatePivotMaps() to update the pivot
      * information.
      *
      * <p>To create the idx value from the pivot description, use GetPivotIdx().
      *
      * @param idx The id of the pivot.
      * @param The object of the pivot. This object will be cloned to avoid
      * problems with pointers.
      * @param map Initial map values. It may be null.
      * @see ChoosePivots()
      * @see GetPivotIdx()
      * @see UpdatePivotMaps()
      */
      void SetPivot(int idx, const ObjectType * obj, const double * map){

         // Create image if required.
         if (Pivots[idx] == NULL){
            Pivots[idx] = new stFastMapImage<ObjectType>(((ObjectType *)obj)->Clone(), Dim);
         }else{
            Pivots[idx]->SetObject(((ObjectType *)obj)->Clone());
         }//end if

         // Replace image contents
         if (map != NULL){
            Pivots[idx]->SetCoords(map);
         }//end if
      }//end SetPivot

      /**
      * This method rebuilds the maps for the pivots. All pivots must be set
      * before call this method or undesirable effects may arise.
      *
      * @see SetPivot()
      */
      void UpdatePivotMaps();

      /**
      * Calculates the stress of a given mapping. The input of this method uses
      * the maps array returned by the function ChoosePivots(). Since it is an
      * expensive method \f$O(N^2)\f$, avoid to call it unless it is really
      * necessary.
      *
      * @param n The number of objects.
      * @param objs The objects.
      * @param maps The maps of these objects.
      * @todo This method requires more tests.
      */
      double GetStress(int n, const ObjectType ** objs, const double ** maps);

      /**
      * Verifies if this FastMapper is ready to use. The test consists to
      * verify if the last pivot is set.
      *
      * @warning It performs a fast check and may fail due to improper
      * use of SetPivot() method, such a lack of pivots.
      */
      bool IsReady(){
         return (Pivots[(Dim * 2) - 1] != NULL);
      }//end IsReady
   private:
      /**
      * Number of dimensions of this FastMapper.
      */
      int Dim;

      /**
      * Number of distance loop counts.
      */
      int DLoopCount;

      /**
      * The metric evaluator.
      */
      EvaluatorType * Eval;

      /**
      * Pivots.
      */
      stFastMapImage<ObjectType> ** Pivots;

      /**
      * A cache of pivot distances raised by the power of 2.
      */
      double * PivotDist;

      /**
      * A cache of pivot distances raised by the power of 2.
      */
      double * PivotDist2;

      /**
      * Creates an image vector from a set of objects. The objects will not
      * be disposed by the image instances (they will now own the objects).
      *
      * @param objs The source object array.
      * @param n The number of objects.
      */
      stFastMapImage<ObjectType> ** CreateTmpImageVector(ObjectType ** objs, int n);

      /**
      * Disposes a vector of images.
      *
      * @param vec The vector to be disposed.
      * @param n The number of entries.
      */
      void DisposeImageVector(stFastMapImage<ObjectType> ** vec, int n);

      /**
      * Gets all maps from a vector of images.
      *
      * @param imgs The image vector.
      * @param n The number of images in the vector.
      * @retval Tne vector of maps. I must have at least n * Dim positions.
      */
      void GetImageMaps(stFastMapImage<ObjectType> ** imgs, int n, double * maps);

      /**
      * Find the best pair of pivots for a given axis.
      *
      * @param imgs A vector of images.
      * @param n The number of images.
      * @param axis The desired axis.
      * @retval pa Index of pivot a.
      * @retval pb Index of pivot b.
      * @retval dist The distance between them.
      */
      void FindPivots(stFastMapImage<ObjectType> ** imgs, int n, int axis, int & pa, int & pb,
            double & dist);

      /**
      * Project an object for a given axis.
      *
      * @param obj The object to be projected.
      * @param map The current map of this object.
      * @param axis The axis to project.
      */
      double Project(const ObjectType * obj, const double * map, int axis);

      /**
      * This method implements the FastMap distance function
      * \f$d'^2(o_1,o_2) = d(o_1,o_2)^2 - (x_1 - x_2)^2\f$.
      *
      * @param o1 Object 1.
      * @param map1 The current image of o1.
      * @param o2 Object 2.
      * @param map2 The current image of o2.
      * @param axis The axis identification.
      * @return The FastMap distance for the given axis raised by the power of
      * 2.
      */
      double GetFMDistance2(const ObjectType * o1, const double * map1,
            const ObjectType * o2, const double * map2, int axis){

         return Eval->GetDistance2((ObjectType *) o1, (ObjectType *)o2)
               - Euclidean2(map1, map2, axis);
      }//end GetFMDistance2
};//end stFastMapper

// Include Template source
#include <arboretum/stFastMapper.cc>

#endif //__STFASTMAPPER_H

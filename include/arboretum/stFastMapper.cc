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
* This file implements the class template stFastMapper and stFastMapImage.
*
* @version 1.0
* $Revision: 1.11 $
* $Date: 2004/06/08 14:45:08 $
*
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
* @todo There is no implementation or definition.
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
//------------------------------------------------------------------------------
// Class stFastMapImage
//------------------------------------------------------------------------------
template <class ObjectType>
void stFastMapImage< ObjectType >::SetCoordsTo(double value){
   int i;

   for (i = 0; i < Dim; i++){
      Coords[i] = value;
   }//end for
}//end stFastMapImage< ObjectType >::SetCoordsTo

//------------------------------------------------------------------------------
// Class stFastMapper
//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stFastMapper<ObjectType, EvaluatorType>::~stFastMapper(){

   if (Pivots != NULL){
      DisposeImageVector(Pivots, Dim * 2);
   }//end if
   
   delete[] PivotDist;
   delete[] PivotDist2;
}//end stFastMapper<ObjectType, EvaluatorType>::~stFastMapper

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stFastMapper<ObjectType, EvaluatorType>::Map(ObjectType * obj,
      double * map){
   int i;

   for (i = 0; i < Dim; i++){
      map[i] = Project(obj, map, i);
   }//end for
}//end stFastMapper<ObjectType, EvaluatorType>::Map

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stFastMapper<ObjectType, EvaluatorType>::ChoosePivots(
      ObjectType ** objs, int n, double * maps, int * pivotids){
   int axis;            // Current axis
   int pid;             // Current pivot id
   int i;               // A generic counter
   stFastMapImage<ObjectType> ** images;    // Temporary image vector
   int * pidxs;         // The idx of the pivots in the image vector it
                        // will be discarded at the end unless pivotsids
                        // is not NULL. This class does not use it outside this
                        // method.

   // This is the core of the pivot selection. if you do not know what is
   // happening here, it's better to take a look on the original FastMap
   // article because it is the same algorithm described there.

   // Initializes the output
   if (pivotids == NULL){
      pidxs = new int[Dim * 2];
   }else{
      pidxs = pivotids;
   }//end if

   // Creates the vector of images
   images = CreateTmpImageVector(objs, n);

   // Map the entire set
   pid = 0;
   for (axis = 0; axis < Dim; axis++){
      // Choose pivots. It will also cache the distance beween them for later
      // uses.
      FindPivots(images, n, axis, pidxs[pid], pidxs[pid + 1], PivotDist[axis]);
      PivotDist2[axis] = PivotDist[axis] * PivotDist[axis];

      // Add the new pivots.   pidxs[pid]
      SetPivot(pid, images[pidxs[pid]]->GetObject(),
            images[pidxs[pid]]->GetCoords());
      SetPivot(pid + 1, images[pidxs[pid + 1]]->GetObject(),
            images[pidxs[pid + 1]]->GetCoords());

      // Map everybody
      for (i = 0; i < n; i++){
         if (i == pidxs[pid]){
            // I'm the pivot 1
            (*(images[i]))[axis] = 0;
         }else if (i == pidxs[pid + 1]){
            // I'm the pivot 2
            (*(images[i]))[axis] = PivotDist[axis];
         }else{
            // I'm someone else.
            (*(images[i]))[axis] = Project(images[i]->GetObject(),
                  images[i]->GetCoords(), axis);
         }//end if
      }//end for

      // Update the pivot maps
      pid = pid + 2;
      for (i = 0; i < pid; i++){
         (*(Pivots[i]))[axis] = (*(images[pidxs[i]]))[axis];
      }//end for
   }//end for

   // Prepares the output
   if (maps != NULL){
      GetImageMaps(images, n, maps);
   }//end if

   // Clear home
   if (pivotids == NULL){
      delete[] pidxs;// Don't be afraid! It is correct since if pivotsid is
                     // NULL, this is an internal vector created by me.
   }//end if
}//end stFastMapper<ObjectType, EvaluatorType>::ChoosePivots

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stFastMapper<ObjectType, EvaluatorType>::UpdatePivotMaps(){
   int axis;            // Current axis
   int pid;             // Current pivot id. It's used to avoid pid = axis * 2.
   int i;               // A generic counter

   // This method is a quite similar to ChoosePivots(). It does not need to
   // perform the FindPivots() method and works direct over the pivot array, so
   // it must not be updated everytime.
   pid = 0;
   for (axis = 0; axis < Dim; axis++){
      // Get the distance between pivots
      PivotDist2[axis] = GetFMDistance2(
            Pivots[pid]->GetObject(), Pivots[pid]->GetCoords(),
            Pivots[pid + 1]->GetObject(), Pivots[pid + 1]->GetCoords(),
            axis);
      PivotDist[axis] = sqrt(PivotDist2[axis]);

      // Map all pivots
      for (i = 0; i < Dim * 2; i++){
         if (i == pid){
            // I'm the pivot 1
            (*(Pivots[i]))[axis] = 0;
         }else if (i == pid + 1){
            // I'm the pivot 2
            (*(Pivots[i]))[axis] = PivotDist[axis];
         }else{
            // I'm someone else.
            (*(Pivots[i]))[axis] = Project(Pivots[i]->GetObject(),
                  Pivots[i]->GetCoords(), axis);
         }//end if
      }//end for

      // Next pid
      pid = pid + 2;
   }//end for
}//end stFastMapper<ObjectType, EvaluatorType>::UpdatePivotMaps

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
double stFastMapper<ObjectType, EvaluatorType>::GetStress(
      int n, const ObjectType ** objs, const double ** maps){
   int w1;           // Counter 1
   int w2;           // Counter 2
   double imgdiff;   // Sum of distance differences
   double realdist;  // Sum of real sistances
   double dist2;     // Distance cache
   double tmp;       // Cheap pow
   double * m1;      // Used to follow maps
   double * m2;      // Used to follow maps

   // Init values
   m1 = maps;
   imgdiff = 0;
   realdist = 0;

   // Calculate values
   for (w1 = 0; w1 < n; w1++){
      m2 = maps;
      for (w2 = 0; w2 < n; w2++){
         if (w1 != w2){
            // Evaluate real distance
            dist2 = Eval->GetDistance2(objs[w1], objs[w2]);

            // Real Distance sum
            realdist += dist2;
            
            // Difference between image and real distance
            tmp = sqrt(Euclidean2(m1, m2, Dim)) - sqrt(dist2);
            imgdiff += tmp * tmp;
         }//end if
         m2 = m2 + Dim; // Next Map. Danger move!
      }//end for
      m1 = m1 + Dim; // Next Map. Danger move!
   }//end for

   // Finish values
   return sqrt(imgdiff/realdist);
}//end stFastMapper<ObjectType, EvaluatorType>::GetStress

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stFastMapImage<ObjectType> **
stFastMapper<ObjectType, EvaluatorType>::CreateTmpImageVector(
      ObjectType ** objs, int n){
   stFastMapImage<ObjectType> ** vec;
   int i;

   vec = new stFastMapImage<ObjectType> *[n];

   for (i = 0; i < n; i++){
      vec[i] = new stFastMapImage<ObjectType>(objs[i], Dim, false);
      vec[i]->SetCoordsTo(0);
   }//end for

   return vec;
}//end stFastMapper<ObjectType, EvaluatorType>::CreateTmpImageVector

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stFastMapper<ObjectType, EvaluatorType>::DisposeImageVector(
      stFastMapImage<ObjectType> ** vec, int n){
   int i;

   for (i = 0; i < n; i++){
      delete vec[i];
   }//end for

   delete[] vec;
}//end stFastMapper<ObjectType, EvaluatorType>::DisposeImageVector

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stFastMapper<ObjectType, EvaluatorType>::GetImageMaps(
      stFastMapImage<ObjectType> ** imgs, int n, double * maps){
   int i;

   for (i = 0; i < n; i++){
      imgs[i]->GetCoords(maps);
      maps = maps + Dim;
   }//end for
}//end stFastMapper<ObjectType, EvaluatorType>::GetImageMaps

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void stFastMapper<ObjectType, EvaluatorType>::FindPivots(
      stFastMapImage<ObjectType> ** imgs, int n, int axis, int & pa, int & pb, double & dist){
   double tmpdist;
   int oldpa;
   int step;
   int i;

   // Search for me!!
   pa = 0;
   oldpa = 0;
   step = 0;
   while (step < DLoopCount){
      // Locate the object wich is the most distant from pa
      dist = 0;
      for (i = 0; i < n; i++){
         if (i != pa){
            tmpdist = GetFMDistance2(
                  imgs[pa]->GetObject(), imgs[pa]->GetCoords(),
                  imgs[i]->GetObject(), imgs[i]->GetCoords(),
                  axis);
            if (tmpdist < 0)
                tmpdist = 0;
            tmpdist = sqrt(tmpdist);
            if (tmpdist > dist){
               dist = tmpdist;
               pb = i;
            }//end if
         }//end if
      }//end for

      if (pb == oldpa){
         // pb is the most distant from pa and vice versa
         step = DLoopCount; // Terminate it now !
      }else{
         // Prepare the next step
         step++;
         oldpa = pa;
         pa = pb;
         pb = oldpa;
      }//end it
   }//end while

   // The final status is pa and pb are set and dist hold the distance between
   // them.
}//end stFastMapper<ObjectType, EvaluatorType>::FindPivots

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
double stFastMapper<ObjectType, EvaluatorType>::Project(
      const ObjectType * obj, const double * map, int axis){
   int pid;

   // This is the core of everithing.
   pid = axis * 2;

   // Distances

   return (GetFMDistance2(obj, map, Pivots[pid]->GetObject(),
         Pivots[pid]->GetCoords(), axis) +                      // d(pa, o)^2

         (PivotDist2[axis]) -                                   // d(pa, pb)^2

         GetFMDistance2(obj, map, Pivots[pid + 1]->GetObject(), // d(pb, o)
         Pivots[pid + 1]->GetCoords(), axis)) /
         (2 * PivotDist[axis]);                                 // 2d(pa, pb)
}//end stFastMapper<ObjectType, EvaluatorType>::Project


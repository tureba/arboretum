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
* This file implements all basic metric evaluators templates.
*
* @version 1.0
* $Revision: 1.27.2.2 $
* $Date: 2006/08/25 05:18:59 $
*
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation review and tests.
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#ifndef __STBASICMETRICEVALUATORS_H
#define __STBASICMETRICEVALUATORS_H

#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>
#include <arboretum/stUserLayerUtil.h>
#include <stdlib.h>


//==============================================================================
// Class template stBasicMetricEvaluator
//------------------------------------------------------------------------------
/**
* This abstract class is the basic class for all stBasicXXXXMetricEvaluator classes.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup user
*/
template <class ObjectType>
class stBasicMetricEvaluator: public stMetricEvaluatorStatistics{
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2) = 0;

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2) = 0;
};//end stBasicMetricEvaluator

//==============================================================================
// Class template stBasicLInfinityMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using the L0 or LInfinity distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @see stBasicEuclideanMetricEvaluator
* @see stBasicManhatanMetricEvaluator
* @see stBasicLxMetricEvaluator
* @ingroup user
*/
template <class ObjectType>
class stBasicLInfinityMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicLInfinityMetricEvaluator

//==============================================================================
// Class template stBasicEuclideanMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using the L2 or the Euclidean distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @see stBasicLInfinityMetricEvaluator
* @see stBasicManhatanMetricEvaluator
* @see stBasicLxMetricEvaluator
* @ingroup user
*/
template <class ObjectType>
class stBasicEuclideanMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2){

         return sqrt(GetDistance2(o1, o2));
      }//end GetDistance

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2);

};//end stBasicEuclideanMetricEvaluator

//==============================================================================
// Class template stBasicManhatanMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using the L1 or the Manhatan distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @see stBasicLInfinityMetricEvaluator
* @see stBasicEuclideanMetricEvaluator
* @see stBasicLxMetricEvaluator
* @ingroup user
*/
template <class ObjectType>
class stBasicManhatanMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicManhatanMetricEvaluator

//==============================================================================
// Class template stBasicCanberraMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using the Canberra distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* @author Javier Montoya (jmontoya@lis.ic.unicamp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @ingroup user
*/

template <class ObjectType>
class stBasicCanberraMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicCanberraMetricEvaluator

//==============================================================================
// Class template stBasicBrayCurtisMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using the Bray Curtis distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* @author Javier Montoya (jmontoya@lis.ic.unicamp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @ingroup user
*/

template <class ObjectType>
class stBasicBrayCurtisMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicBrayCurtisMetricEvaluator

//==============================================================================
// Class template stBasicSquaredChordMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using the Bray Curtis distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* @author Javier Montoya (jmontoya@lis.ic.unicamp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @ingroup user
*/

template <class ObjectType>
class stBasicSquaredChordMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicSquaredChordMetricEvaluator

//==============================================================================
// Class template stBasicBasicSquareChiSquaredMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using the Bray Curtis distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* @author Javier Montoya (jmontoya@lis.ic.unicamp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @ingroup user
*/

template <class ObjectType>
class stBasicBasicSquareChiSquaredMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicBasicSquareChiSquaredMetricEvaluator

//==============================================================================
// Class template stBasicLpMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between objects using an Lx distance function. The value of p may be
* any number greater than 0. In other words, this class implements L1,
* L2, L3, ..., Lp distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* <P>The template parameter <b>P</b> is the order of L. For example, it it is
* set to 3, it will implement the L3 distance function. If you set it to 2, it
* will implements the L2 or the Euclidean distance function.
*
* <P>Important Note: For L1 or L2 it is possible to use this class template with
* the P set as 1 or 2 but it is better to use stBasicManhatanMetricEvaluator or
* stBasicEuclideanMetricEvaluator instead because they have better
* implementations.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @see stBasicLInfinityMetricEvaluator
* @see stBasicEuclideanMetricEvaluator
* @see stBasicManhatanMetricEvaluator
* @ingroup user
* @warning Certain values of p may break the triangular inequality.
* /
template <class ObjectType, int P = 3>
class stBasicLpMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      * /
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      * /
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicLpMetricEvaluator
*/
//==============================================================================
// Class template stBasicLp2MetricEvaluator
//------------------------------------------------------------------------------
/**
* This class is very similar to stBasicLpMetricEvaluator except that it's
* constructor allows passing the P parameter at runtime.
*
* <P>This class implements a generic metric evaluator that calculates the distance
* between objects using an Lx distance function. The value of p may be
* any number greater than 0. In other words, this class implements L1,
* L2, L3, ..., Lp distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* <P>The template parameter <b>P</b> is the order of L. For example, it it is
* set to 3, it will implement the L3 distance function. If you set it to 2, it
* will implements the L2 or the Euclidean distance function.
*
* <P>Important Note: For L1 or L2 it is possible to use this class template with
* the P set as 1 or 2 but it is better to use stBasicManhatanMetricEvaluator or
* stBasicEuclideanMetricEvaluator instead because they have better
* implementations.
*
* @author Humberto Razente (hlr@icmc.usp.br)
* @warning Both objects must have the same number of entries.
* @todo Documentation.
* @todo Tests.
* @see stMetricEvaluator
* @see stBasicLInfinityMetricEvaluator
* @see stBasicEuclideanMetricEvaluator
* @see stBasicManhatanMetricEvaluator
* @ingroup user
* @warning Certain values of p may break the triangular inequality.
* /
template <class ObjectType>
class stBasicLp2MetricEvaluator: public stBasicMetricEvaluator < ObjectType > {

   private:
       int P;

   public:
      /**
      * This constructor allows passing the P parameter at runtime.
      *
      * @param p Value of P.
      * /
      stBasicLp2MetricEvaluator(int p) {
         if (p > 0){
            P = p;
         }else{
            P = 3;
         }//end if
      }//end stBasicLp2MetricEvaluator

      /**
      * @copydoc stMetricEvaluator::GetDistance()
      * /
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2);

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      * /
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
};//end stBasicLp2MetricEvaluator
*/
//==============================================================================
// Class template stBasicLEditMetricEvaluator
//------------------------------------------------------------------------------
/**
* This class implements a generic metric evaluator that calculates the distance
* between strings using the LEdit distance function.
*
* <P>The template parameter <b>ObjectType</b> defines the object type used by
* this metric evaluator. It was designed to work with instances of stBasicArray
* objects but any array object that defines the following methods may be used:
*  - <b>type * Get(n)</b>: Returns the element at position <i>n</i>.
*  - <b>int * GetSize()</b>: Returns the number of element int hte array.
*
* <P>Note that to be fully compatible with the metric trees, the desired object
* must meet the requirements of the stObject interface also.
*
* <P>The template parameter DataType defines the type of each character. By
* default, it is set to char.
*
* <P>This implementation was based on the article "Levenshtein Distance, in
* Three Flavors" written by
* <a href="http://www.mgilleland.com/">Michael Gilleland</a>.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation.
* @todo Tests. I don't know if this implementation is reflexive.
* @see stMetricEvaluator
* @ingroup user
*/
template <class ObjectType, class DataType = char>
class stBasicLEditMetricEvaluator: public stBasicMetricEvaluator < ObjectType > {
   public:
      /**
      * @copydoc stMetricEvaluator::GetDistance()
      */
      virtual stDistance GetDistance(ObjectType * o1, ObjectType * o2){
         
         // Statistic support
         this->UpdateDistanceCount();
         return LEdit(*o2, *o1);
      }//end GetDistance

      /**
      * @copydoc stMetricEvaluator::GetDistance2()
      */
      virtual stDistance GetDistance2(ObjectType * o1, ObjectType * o2){
         stDistance d;

         d = GetDistance(o1, o2);

         return d * d;
      }//end GetDistance2
      
   private:
      /**
      * Type of the diff matrix.
      */
      typedef stGenericMatrix <DataType> tMatrix;

      /**
      * The diff matrix.
      */
      tMatrix diff;      

      /**
      * Returns the minimum value between 3 values.
      *
      * @param a Value a.
      * @param b Value b.
      * @param c Value c.
      */
      DataType Min(DataType a, DataType b, DataType c){
         if (a < b){
            if (a < c){
               return a;
            }else{
               return c;
            }//end if
         }else{
            if (b < c){
               return b;
            }else{
               return c;
            }//end if
         }//end if
      }//end Min

      /**
      * Performs the LEdit.
      *
      * @param si String 1.
      * @param sj String 2.
      * @return The distance beween wShort and wLong.
      */
      stDistance LEdit(ObjectType & si, ObjectType & sj);
      
};//end stBasicLEditMetricEvaluator

// Include implementation
#include "stBasicMetricEvaluators.cc"

#endif //__STBASICMETRICEVALUATORS_H

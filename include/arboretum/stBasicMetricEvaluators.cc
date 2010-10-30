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
* $Revision: 1.4.2.2 $
* $Date: 2006/08/25 05:18:59 $
*
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @todo Documentation review and tests.
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP

//==============================================================================
// Class template stBasicLInfinityMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicLInfinityMetricEvaluator< ObjectType >::GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmp;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmp = fabs(o1->Get(i) - o2->Get(i));
      if (tmp > d){
         d = tmp;
      }//end if
   }//end for

   // Statistic support
   this->UpdateDistanceCount();

   return d;
}//end GetDistance

//==============================================================================
// Class template stBasicEuclideanMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicEuclideanMetricEvaluator< ObjectType >::GetDistance2(
         ObjectType * o1, ObjectType * o2){
   unsigned int i;
   stDistance d;
   stDistance tmp;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmp = o1->Get(i) - o2->Get(i);
      d = d + (tmp * tmp);
   }//end for

   // Statistic support
   this->UpdateDistanceCount();

   return d;
}//end GetDistance2

//==============================================================================
// Class template stBasicManhatanMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicManhatanMetricEvaluator< ObjectType >:: GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmp;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmp = fabs(o1->Get(i) - o2->Get(i));
      d = d + tmp;
   }//end for
         
   // Statistic support
   this->UpdateDistanceCount();

   return d;
}//end GetDistance

//==============================================================================
// Class template stBasicCanberraMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicCanberraMetricEvaluator< ObjectType >:: GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmpNumerator;
   stDistance tmpDenominator;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmpNumerator   = fabs(o1->Get(i)  - o2->Get(i));
      tmpDenominator = fabs(o1->Get(i)) + fabs(o2->Get(i));
      d = d + (tmpNumerator/tmpDenominator);
   }//end for
         
   // Statistic support
   this->UpdateDistanceCount();

   return d;
}//end GetDistance

//==============================================================================
// Class template stBasicBrayCurtisMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicBrayCurtisMetricEvaluator< ObjectType >:: GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmpNumerator;
   stDistance tmpDenominator;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmpNumerator   = fabs(o1->Get(i) -  o2->Get(i));
      tmpDenominator =     o1->Get(i)  +  o2->Get(i);
      d = d + (tmpNumerator/tmpDenominator);
   }//end for
         
   // Statistic support
   this->UpdateDistanceCount();

   return d;
}//end GetDistance

//==============================================================================
// Class template stBasicSquaredChordMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicSquaredChordMetricEvaluator< ObjectType >:: GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmp;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmp = sqrt(o1->Get(i)) - sqrt(o2->Get(i));
      d = d + (tmp*tmp);
   }//end for
         
   // Statistic support
   this->UpdateDistanceCount();

   return d;
}//end GetDistance

//==============================================================================
// Class template stBasicBasicSquareChiSquaredMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicBasicSquareChiSquaredMetricEvaluator< ObjectType >:: GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmpNumerator;
   stDistance tmpDenominator;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmpNumerator   = (o1->Get(i) - o2->Get(i)) * (o1->Get(i) - o2->Get(i));
      tmpDenominator = o1->Get(i) + o2->Get(i);
      d = d + (tmpNumerator/tmpDenominator);
   }//end for
         
   // Statistic support
   this->UpdateDistanceCount();

   return d;
}//end GetDistance

/*
//==============================================================================
// Class template stBasicLpMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType, int P>
stDistance stBasicLpMetricEvaluator< ObjectType>::GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmp;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmp = fabs(o1->Get(i) - o2->Get(i));
      d = d + pow(tmp, P);
   }//end for

   // Statistic support
   this->UpdateDistanceCount();

   return pow(d, 1.0 / double(P));
}//end GetDistance

//==============================================================================
// Class template stBasicLp2MetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType>
stDistance stBasicLp2MetricEvaluator< ObjectType >::GetDistance(
         ObjectType * o1, ObjectType * o2){
   int i;
   stDistance d;
   stDistance tmp;

   d = 0;
   for (i = 0; i < o1->GetSize(); i++){
      tmp = fabs(o1->Get(i) - o2->Get(i));
      d = d + pow(tmp, P);
   }//end for

   // Statistic support
   this->UpdateDistanceCount();

   return pow(d, 1.0 / double(P));
}//end GetDistance
*/
//==============================================================================
// Class template stBasicLEditMetricEvaluator
//------------------------------------------------------------------------------
template <class ObjectType, class DataType>
stDistance stBasicLEditMetricEvaluator< ObjectType, DataType>::LEdit(
         ObjectType & si, ObjectType & sj){
   int i;
   int j;
   int cost;

   // Resize diff matrix
   diff.SetSize(sj.GetSize() + 1, si.GetSize() + 1);

   // Initialize
   diff[0][0] = 0;
   for (i = 1; i <= si.GetSize(); i++){
      diff[i][0] = i;
   }//end for
   for (j = 1; j <= sj.GetSize(); j++){
      diff[0][j] = j;
   }//end for

   for (i = 1; i <= si.GetSize(); i++){
      for (j = 1; j <= sj.GetSize(); j++){
         // Cost
         cost = (si.Get(i - 1) == sj.Get(j - 1)) ? 0 : 1;
         diff[i][j] = Min(
               diff[i - 1][j] + 1,
               diff[i][j - 1] + 1,
               diff[i - 1][j - 1] + cost);
      }//end for
   }//end for

   // Returning value...
   return diff[si.GetSize()][sj.GetSize()];
}//end LEdit

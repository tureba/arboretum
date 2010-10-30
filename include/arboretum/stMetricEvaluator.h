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
* This file defines the abstract class stMetricEvaluator.
*
* @version 1.0
* $Revision: 1.10 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002 GBDI-ICMC-USP

#ifndef __STMETRICEVALUATOR_H
#define __STMETRICEVALUATOR_H

#include  <arboretum/stObject.h>

/**
* This abstract class implements the stMetricEvaluator interface. This interface is
* required by the metric distance functions that will be used by the metric
* trees implemented by this library. Since these metric trees are implemented as
* class templates, any class that implements all methods defined in this
* interface may be used by a metric tree.
*
* <P>The class required by stSlimTree to calculate the metric distances
* must have 2 distinct methods. One of them, GetDistance(), must return
* the distance itself while the other, GetDistance2(), must return the
* same value raised by the power of 2.
*
* <P>They are defined as distinct methods because some procedures will
* require one or both of these values. Since some distance functions may
* have better algorithms for the distance or distance raised by 2,
* its better to keep both implementations. For example, the Euclidean
* function has a better algorithm for the distance raised by 2 than for
* the distance itself.
*
* <p>This functional separation exists to avoid things such as
* @f$ d^2 = (\sqrt[2]{d^2})^2 @f$ which is an undesireble situation
* because square roots are not cheap.
*
* <p>As an optional function, a metric evaluator is the responsible to compute
* the number of distances calculated WHEN IT IS REQUIRED (it is not used by the
* Structure Layer).
*
* <p>The implementation of this feature has no standard interface and may be
* implemented freely but to make the implementation easier, there
* is a class called stMetricEvaluatorStatistics that implements the basic
* functions necessary to accomplish this task.
*
* <P>This class may be used as the base class for classes that implements the
* metric evaluators which will be used by a metric tree to compute distances
* but it is not recommended.
*
* @version 1.0
* @ingroup userinterface
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @see stObject
* @see stMetricEvaluatorStatistics
*/
class stMetricEvaluator{
   public:
      /**
      * This method calculates the metric distance between 2 objects.
      *
      * @param obj1 Object 1.
      * @param obj2 Object 2.
      * @return The distance between to objects.
      * @note This method is required to fulfil the stMetricEvaluator interface.
      */
      virtual stDistance GetDistance(stObject * obj1, stObject * obj2) = 0;

      /**
      * This method calculates the metric distance between 2 objects raised
      * by the power of 2.
      *
      * @param obj1 Object 1.
      * @param obj2 Object 2.
      * @note This method is required to fulfil the stMetricEvaluator interface.
      */
      virtual stDistance GetDistance2(stObject * obj1, stObject * obj2) = 0;
};//end stMetricEvaluator
#endif //__STMETRICEVALUATOR_H

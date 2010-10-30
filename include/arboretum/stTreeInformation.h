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
* This file defines all classes used to acquire a tree information.
*
* @version 1.0
* $Revision: 1.6 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#ifndef __STTREEINFORMATION_H
#define __STTREEINFORMATION_H

#include <math.h>
#include <arboretum/stCommon.h>
#include <arboretum/stTypes.h>

//------------------------------------------------------------------------------
// Class stTreeInfoResult
//------------------------------------------------------------------------------
/**
* This class abstracts the statistics of a given tree.
*
* <p>The current version implements:
*     - mean object size;
*     - level statistics;
*     - optimal tree statistics;
*     - FatFactor (specific, local and absolute).
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup struct
*/
class stTreeInfoResult{
   public:
      /**
      * Diposes this instance an release all allocated resources.
      */
      virtual ~stTreeInfoResult(){}
      
      /**
      * Returns the maximum height of the tree.
      */
      virtual int GetHeight() = 0;
      
      /**
      * Returns the minimum height of the tree.
      */
      virtual int GetMinHeight() = 0;
      
      /**
      * Returns the optimal height of the tree.
      * The result of this method is meanless if HasOptimalTree() returned false.
      */
      virtual int GetOptimalHeight() = 0;
      
      /**
      * Returns the number of objects indexed by the tree.
      */
      virtual int GetObjectCount() = 0;
      
      /**
      * Returns the FatFactor for a given level.
      *
      * @param level The level number.
      */
      virtual double GetLocalFatFactor(int level) = 0;
      
      /**
      * Returns the global FatFactor.
      */
      virtual double GetGlobalFatFactor() = 0;
      
      /**
      * Returns the mean size of the objects stored in the tree. This value is useful
      * to help tree developers to calculate the optimal tree.
      */
      virtual double GetMeanObjectSize() = 0;
      
      /**
      * Returns the absolute FatFactor for this tree.       
      * The result of this method is meanless if HasOptimalTree() returned false.
      */    
      virtual double GetAbsoluteFatFactor() = 0;
      
      /**
      * This method invalidates the current calculated FatFactor data. These
      * data will be recalculated again when required.
      */
      virtual void Invalidate() = 0;
      
      /**
      * Returns true if this information includes the optimal tree data.
      */
      virtual bool HasOptimalTree() = 0;
};//end stTreeInfoResult

//------------------------------------------------------------------------------
// Class stTreeInformation
//------------------------------------------------------------------------------
/**
* This class implements the class stTreeInfoResult. It works as a form that each
* tree must fill in order to provide its statistics to users.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @ingroup userlayer
*/
class stTreeInformation: public stTreeInfoResult{
   public:
      /**
      * Creates a new FatFactor result for a tree with a given height.
      *
      * @param height The maximum height of the tree.
      * @warning For tree use only.
      */
      stTreeInformation(int height, int objectCount);
      
      /**
      * Disposes this FatFactor result.
      */
      virtual ~stTreeInformation();    
   
      /**
      * Returns the maximum height of the tree.
      */
      virtual int GetHeight(){
         return height;
      }//end GetHeight
      
      /**
      * Returns the minimum height of the tree.
      */
      virtual int GetMinHeight(){
         return minHeight;
      }//end GetMinHeight
      
      /**
      * Returns the optimal height of the tree.
      * The result of this method is meanless if HasOptimalTree() returned false.
      */
      virtual int GetOptimalHeight(){
         return optimalHeight;
      }//end GetOptimalHeight
      
      /**
      * Returns the number of objects indexed by the tree.
      */
      virtual int GetObjectCount(){
         return objectCount;
      }//end GetObjectCount
      
      /**
      * Returns the FatFactor for a given level.
      *
      * @param level The level number.
      */
      virtual double GetLocalFatFactor(int level){
         Validate();
         return levelData[level].FatFactor;
      }//end GetLocalFatFactor
      
      /**
      * Returns the global FatFactor.
      */
      virtual double GetGlobalFatFactor(){
         Validate();
         return fatFactor;
      }//end GetGlobalFatFactor
      
      /**
      * Returns the mean size of the objects stored in the tree. This value is useful
      * to help tree developers to calculate the optimal tree.
      */
      virtual double GetMeanObjectSize(){
         return double(objectSizeSum) / double(objectSizeCount);
      }//end GetMeanObjectSize
      
      /**
      * Returns the absolute FatFactor for this tree.       
      * The result of this method is meanless if HasOptimalTree() returned false.
      */    
      virtual double GetAbsoluteFatFactor(){
         Validate();
         return bloatFactor;
      }//end GetAbsoluteFatFactor
      
      /**
      * This method invalidates the current calculated FatFactor data. These
      * data will be recalculated again when required.
      */
      virtual void Invalidate(){
         ready = false;
      }//end Invalidate    
      
      /**
      * Updates the number of intersections of a given level.
      *
      * @param level The desired level.
      * @param n Number of intersections to add (default = 1).
      * @warning For tree use only.
      */
      void UpdateIntersections(int level, int n = 1){
         Invalidate();
         levelData[level].Intersections += n;
      }//end UpdateIntersections
      
      /**
      * Updates the number of nodes of a given level.
      *
      * @param level The desired level.
      * @param n Number of nodes to add (default = 1).
      * @warning For tree use only.
      */
      void UpdateNodeCount(int level, int n = 1){
         Invalidate();
         levelData[level].NodeCount += n;
      }//end UpdateNodeCount
   
      /**
      * Updates the number of objects of a given level.
      *
      * @param level The desired level.
      * @param n Number of objects to add (default = 1).
      * @warning For tree use only.
      */
      void UpdateObjectCount(int level, int n = 1){
         Invalidate();
         levelData[level].ObjectCount += n;
      }//end UpdateObjectCount
      
      /**
      * Updates the mean size of the object by adding a new object entry.
      * To make this computation more accurate, avoid to add a given object
      * more than once.
      *
      * @param size Size of the serialized object in bytes.
      * @warning For tree use only.
      */
      void UpdateMeanObjectSize(stSize size){
         objectSizeSum += size;
         objectSizeCount++;
      }//end UpdateMeanObjectSize
      
      /**
      * Updates the minimum height of the tree. If the minimum height is constant,
      * this method may be avoided.
      *
      * @param height The height of a minimum
      * @warning For tree use only.
      */
      void UpdateMinHeight(int height){
         if (height < minHeight){
            minHeight = height;
         }//end if         
      }//end UpdateHeight
      
      /**
      * Returns true if this information includes the optimal tree data.
      */
      virtual bool HasOptimalTree(){
         return optimalLevelData != NULL;
      }//end HasOptimalTree
      
      /**
      * Sets the information obout the optimal tree. This information is used to calculate
      * some of the tree's statistics.
      *
      * @param height The optimal height of the tree.
      * @param objectCount The number of objects per level in the optimal tree.
      * @param nodeCount The number of nodes per level of the optimal tree.
      * @see CalculateOptimalTreeInfo()
      * @warning For tree use only.
      */
      void SetOptimalTreeInfo(int height, int * objectCount, int * nodeCount);
      
      /**
      * This method calculates the optimal tree info based on the same rules
      * used to calculate the best M-Tree and Slim-Tree configuration.  It
      * assumes that all objects are stored in the last level of the tree and
      * the number of links in an internal node is equal to occupation.
      *
      * <p>Since not all trees follow these rules, a method called
      * SetOptimalTreeInfo() may be used to fill the information about the optimal
      * tree.
      *
      * @param occupation The maximum occupation of a node.
      * @see SetOptimalTreeInfo()
      * @warning For tree use only.
      */
      void CalculateOptimalTreeInfo(int occupation);
      
      /**
      * Returns a clone of this instance.
      *
      * @todo This method is not complete.
      */
      stTreeInformation * Clone();
   protected:
      /**
      * This struct stores the information about a given level.
      */
      struct stLevelInfo{
         /**
         * Number of intersections.
         */
         int Intersections;
         /**
         * Number of nodes.
         */
         int NodeCount;
         /**
         * Number of objects.
         */
         int ObjectCount;
         /**
         * FatFactor.
         */
         double FatFactor;
      };//end stLevelInfo
   
      /**
      * This struct stores the information about a given level of the
      * optimal tree.
      */
      struct stOptimalLevelInfo{
         /**
         * Number of nodes.
         */
         int NodeCount;
         /**
         * Number of objects.
         */
         int ObjectCount;
         /**
         * FatFactor.
         * @warning Unused field. It lacks implementation.
         */
         double BloatFactor;
      };//end stOptimalLevelInfo
      
      /**
      * Height of the tree.
      */
      int height;

      /**
      * Minimum height.
      */
      int minHeight;
                  
      /**
      * Value of the global FatFactor.
      */
      double fatFactor;
      
      /**
      * Value of the BloatFactor (the absolute FatFactor ?).
      */
      double bloatFactor;
      
      /**
      * Number of objects indexed by the tree.
      */
      int objectCount;
      
      /**
      * Total object size used to compute the mean object size.
      */
      stSize objectSizeSum;
      
      /**
      * Number of objects used to compute the mean object size.
      */    
      stSize objectSizeCount;
      
      /**
      * Information about each level.
      */
      stLevelInfo * levelData;
      
      /**
      * Information about each level of the optmal tree.
      */
      stOptimalLevelInfo * optimalLevelData;
      
      /**
      * The height of the optmal tree.
      */
      int optimalHeight;
      
      /**
      * This flag is used to determine the availability of the FatFactor
      * information. If this flag is false, it is necessary to call Calculate()
      * to make the information ready to use.
      * 
      * @see Validate()
      * @see Invalidate()
      */
      bool ready;
      
      /**
      * Updates the FatFactor data when required. It must be called once before
      * all FatFactor returning method to assure the consistency of data.
      *
      * @see Invalidate()
      */
      void Validate(){
         if (!ready){
            Calculate();
         }//end if
      }//end Validate
      
      /**
      * Resets all data acquired from the tree. All fields are set to
      * 0.
      */
      void ResetData();
      
      /**
      * This method uses the information available to calculate the FatFactor.
      *
      * <p>Since this method is a quite expensive, the flag ready is used to
      * avoid unnecessary calls to this method. To assure this, never call
      * this method directly, call Validate() instead.
      */
      void Calculate();
};//end stTreeInformation

#endif //__STTREEINFORMATION_H

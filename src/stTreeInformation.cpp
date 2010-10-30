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
* This file has the implementation of the functions defined in the
* file stTreeInformation.h.
*
* @version 1.0
* $Revision: 1.7 $
* $Date: 2005/03/08 19:44:19 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002-2003 GBDI-ICMC-USP
#include <arboretum/stTreeInformation.h>

/**
* Calculates the @f$log_{y}(x)@f$.
*/
#define logxy(x, y) (log((double)x) / log((double)y))

//------------------------------------------------------------------------------
// Class stTreeInformation
//------------------------------------------------------------------------------
stTreeInformation::stTreeInformation(int height, int objectCount):stTreeInfoResult(){
   
   // Creating resources.
   this->height = height;
   minHeight = height;
   levelData = new stLevelInfo[height];
   
   // Optimal tree
   optimalLevelData = NULL;
   
   // Initialize data
   ResetData();   
   Invalidate();
   this->objectCount = objectCount;
}//end stTreeInformation::stTreeInformation

//------------------------------------------------------------------------------
stTreeInformation::~stTreeInformation(){
   
   delete[] levelData;
   
   if (optimalLevelData != NULL){
      delete[] optimalLevelData;
   }//end if
}//end stTreeInformation::~stTreeInformation

//------------------------------------------------------------------------------
void stTreeInformation::ResetData(){
   
   // Set all fields to 0.
   memset(levelData, 0, sizeof(stLevelInfo) * height);
   
   // Mean object size  
   objectSizeSum = 0;
   objectSizeCount = 0;
}//end stTreeInformation::ResetData

//------------------------------------------------------------------------------
void stTreeInformation::Calculate(){
   int i;
   int numIntersections;
   int numNodes;
   int optNumNodes;

   // This method is the core of the FatFactor. It uses the data acquired for
   // each level to calculate all FatFactor info.
   //
   // RE note: This method replaces part of the functionality of FinishFatFactor()
   // and MeasureFatFactorTree().

   // Totals
   numIntersections = 0;
   numNodes = 0;
   for (i = 0; i < height; i++){
      numIntersections += levelData[i].Intersections;
      numNodes += levelData[i].NodeCount;
   }//end for

   // Local FatFactor is...
   // Note: I was tempted to put this loop inside the totals but I think
   // it is not a good legibility practice so...
   for (i = 0; i < height; i++){
      levelData[i].FatFactor =
            double(levelData[i].ObjectCount - levelData[i].Intersections) /
            double(levelData[i].ObjectCount) /
            double(levelData[i].NodeCount);
      // levelData[height].BloatFactor = ...;
   }//end for
   
   // The FatFactor is...
   // The original is:
   // fat=(TotIntersect-double(SlT->height+1.)*double(numobjects))/
   //    (double(numobjects)*double(SlT->pagecount-(SlT->height+1.)));
   fatFactor = (double(numIntersections) - double(height + 1) * double(objectCount)) /
         (double(objectCount) * double(numNodes - height - 1));
   
   // The BloatFactor is...
   if (optimalLevelData != NULL){
      // Totals
      optNumNodes = 0;
      for (i = 0; i < optimalHeight; i++){
         optNumNodes += optimalLevelData[i].NodeCount;
      }//end for

      // The original is:
      // bl=(TotIntersect-double(TeorHeight+1)*double(numobjects))/
      //    (double(numobjects)*double(totTeorCountNodes-(TeorHeight+1)));
      bloatFactor = (numIntersections - double(optimalHeight + 1) * double(objectCount))/
            (double(objectCount) * double(optNumNodes) - (optimalHeight + 1));
   }//end if
   
   // Mark data as ready to use.
   ready = true;
}//end stTreeInformation::Calculate

//------------------------------------------------------------------------------
void stTreeInformation::SetOptimalTreeInfo(int height, int * objectCount, int * nodeCount){
   int i;

   // Initialize fields
   this->optimalHeight = height;
   optimalLevelData = new stOptimalLevelInfo[height];

   for (i = 0; i < height; i++){
      optimalLevelData[i].NodeCount = nodeCount[i];
      optimalLevelData[i].ObjectCount = objectCount[i];
   }//end for
   
   Invalidate();  
}//end stTreeInformation::SetOptimalTreeInfo

//------------------------------------------------------------------------------
void stTreeInformation::CalculateOptimalTreeInfo(int occupation){
   int i;

   // This method simulates the most perfect tree possible for a given number
   // of entries. It assumes that almost all nodes are full and the height is the
   // minimum possible. It's important to remember that all indexed objects are
   // stored in the last level as Slim-Tree.

   // Initialize fields
   optimalHeight = int(ceil(logxy(objectCount, occupation)));
   optimalLevelData = new stOptimalLevelInfo[optimalHeight];

   // Last level
   optimalLevelData[optimalHeight - 1].NodeCount = int(ceil(double(objectCount)/double(occupation)));
   optimalLevelData[optimalHeight - 1].ObjectCount = objectCount;

   // Other levels
   for (i = optimalHeight - 1; i > 0; i--){
      optimalLevelData[i - 1].NodeCount = int(ceil(double(optimalLevelData[i].NodeCount) / double(occupation)));
      optimalLevelData[i - 1].ObjectCount = optimalLevelData[i].NodeCount;
   }//end for

   #ifdef __stDEBUG__
   // cout << "oh=" << optimalHeight << ";oc=" << occupation << "\n";
   // for (i = 0; i < optimalHeight; i++){
   //    cout << "lv="<< i << ";oc=" << optimalLevelData[i].ObjectCount << ";nc=" <<
   //          optimalLevelData[i].NodeCount << "\n";
   // }//end for
   #endif //__stDEBUG__

   Invalidate();
}//end stTreeInformation::CalculateOptimalTreeInfo

//------------------------------------------------------------------------------
stTreeInformation * stTreeInformation::Clone(){
   stTreeInformation * clone;
   
   // Create the new instance.
   clone = new stTreeInformation(height, objectCount);
   
   // Fill data
   clone->minHeight = minHeight;
   clone->fatFactor = fatFactor;
   clone->bloatFactor = bloatFactor;
   clone->objectSizeSum = objectSizeSum;
   clone->objectSizeCount = objectSizeCount;
   clone->optimalHeight = optimalHeight;
   clone->ready = ready;
   memcpy(clone->levelData, levelData, sizeof(stLevelInfo) * height);
   if (optimalLevelData != NULL){
      clone->optimalLevelData = new stOptimalLevelInfo[optimalHeight];
      memcpy(clone->optimalLevelData, optimalLevelData, sizeof(stOptimalLevelInfo) * optimalHeight);
   }//end if
   
   return clone;
}//end stTreeInformation::Clone
//------------------------------------------------------------------------------

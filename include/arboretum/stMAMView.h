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
* This file defines all class templates used by the MAMView data extraction module.
*
* @version 1.0
* $Revision: 1.12 $
* $Date: 2005/01/14 16:54:15 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
*/
// Copyright (c) 2002 GBDI-ICMC-USP
#ifndef __STMAMVIEW_H
#define __STMAMVIEW_H

#include <arboretum/stCommon.h>
#include <arboretum/stUtil.h>
#include <arboretum/stResult.h>
#include <arboretum/stTypes.h>
#include <arboretum/stFastMapper.h>
#include <arboretum/stException.h>
#include <string>
#include <stack>

#ifndef __stMAMVIEW__
/**
* This macro defines the MAMView version.
*
* @ingroup mamview
*/
#define stMAMViewVersion "Not supported"

#else
/**
* This macro defines the MAMView version.
*
* @ingroup mamview
*/
#define stMAMViewVersion "0.1"

//----------------------------------------------------------------------------
// Exception unsupported_method_error
//----------------------------------------------------------------------------
/**
* This exception occurs when something goes wrong with the stMAMViewExtractor.
*
* <p>The occurrence of this exception must be avoided at all costs because it
* indicates that the use of stMAMViewExtractor is not correct.
*
* @author Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
* @version 1.0
* @ingroup mamview
*/
class mamview_error: public logic_error{
   public:
      mamview_error(const string &what_arg): logic_error(what_arg){
      }//end mamview_error
};//end mamview_error

//-----------------------------------------------------------------------------
// class stMAMViewObjectSample
//-----------------------------------------------------------------------------
/**
* This class template implements a special object vector which can help metric tree
* implementators to get a set of random objects from the structure.
*
* <p>The basic use of this class is:
* @verbatim
* for all objects in the MAM do
*    if list.MayAdd() then
*       list.Add(Object)
*    end if
* End for
* @endverbatim
*
* <p>The template parameter ObjectType determines the type of object to handle and
* has no special requirements.
*
* @version 1.0
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @ingroup mamview
*/
template <class ObjectType>
class stMAMViewObjectSample{
   public:
      /**
      * Creates a new instance of this class. The parameter size controls the number
      * ob desired objectis in the sample and the parameter total determines the total
      * number of objects in the universe. Both values are used to calculate the
      * probability of acceptance of an object of the universe (using an uniform
      * probability distribution - \f$p = {size \over total} \f$).
      *
      * @param size The desired size of the sample.
      * @param total Total number of objects.
      */
      stMAMViewObjectSample(stSize size, stSize total){
         // Calculate the acceptance an object. 
         P = ((double)size)/((double)total);
         MaxSize = size;
         Size = 0;
         Objs = new ObjectType *[MaxSize];
      }//end stMAMViewObjectSample
      
      /**
      * @todo Implement it later or you will have a veeeerrryyy good memory leak.
      */
      ~stMAMViewObjectSample();

      /**
      * Ask this class if a given object from the tree may be added. This method should not
      * be invoked more than once per object.
      *
      * <p>According to probability laws, at the end of all insertion attempts, this
      * method will allow about GetDesiredSampleSize() insertions.
      *
      * @return True if the object may be added or false otherwise.
      */
      bool MayAdd(){
         if (Size < MaxSize){
            return randomevent(P);
         }else{
            // I'm full.
            return false;
         }//end if
      }//end AddThis
   
      /**
      * Adds a new object to the set.
      *
      * @param obj The object to be added. This method will claim the ownership of
      * this instance.
      */
      void Add(ObjectType * obj){
         if (Size < MaxSize){
            Objs[Size] = obj;
            Size++;
         }//end if
      }//end Add
      
      /**
      * Returns the array of objects. This method is suitable for
      * stFastMapper::ChoosePivots().
      */
      ObjectType ** GetObjects(){
         return Objs;
      }//end GetObjects
      
      /**
      * Returns the number of objects in this set.
      */
      stSize GetSize(){
         return Size;
      }//end GetSize
      
      /**
      * Returns the desired size of the set.
      */
      stSize GetDesiredSampleSize(){
         return MaxSize;
      }//end GetDesiredSampleSize
   private:
      /**
      * The probability of acceptance of an object.
      */
      double P;
      
      /**
      * Current size.
      */
      stSize Size;
      
      /**
      * Maximum size.
      */
      stSize MaxSize;
      
      /**
      * Vector of objects.
      */
      ObjectType ** Objs;
};//end class

//-----------------------------------------------------------------------------
// class stMAMViewExtractor
//-----------------------------------------------------------------------------
/**
* This class template implements the MAM View extractor.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @ingroup mamview
* @todo Documentation missing.
* @todo Pivot storage support not implemented.
*/
template <class ObjectType, class EvaluatorType>
class stMAMViewExtractor{
   public:
      /**
      * Creates a new instance of this extractor. This instance will not be
      * ready to use until the first call of Init().
      *
      * @param eval An instance of the metric evaluator. This instance will
      * never be disposed.
      */
      stMAMViewExtractor(EvaluatorType * eval){
         Eval = eval;
         Serial = 0;
         Output = NULL;
         Mapper = NULL;

         // First state.
         StateStack.push(ST_DISABLE);
      }//end EvaluatorType
      
      virtual ~stMAMViewExtractor();

      /**
      * Initializes the mapper to be used by this extractor. It requires a
      * set of objects (at least 50) which must be representative to the whole
      * set of objects.
      *
      * @param sample A sample of objects.
      */
      void Init(stMAMViewObjectSample<ObjectType> * sample);

      /**
      * Declares the beginning of an animation. It must be called before
      * any other declaration methods. Both title and comment parameter are
      * optional.
      *
      * @param title The title of this animation.
      * @param comment A little comment text about the animation.
      * @bug Both fields do not support &gt; and &lt; characters.
      */
      void BeginAnimation(const char * title = NULL, const char * comment = NULL);

      /**
      * Declares the end of the animation. It must be the last command of an
      * animation.
      */
      void EndAnimation();

      /**
      * Declares the beginning of a frame.
      *
      * @param comment A little comment text about the frame.
      * @warning This command is not allowed outside an  animation.
      */
      void BeginFrame(const char * comment);

      /**
      * Declares the end of a frame.
      *
      * @warning This command is not allowed outside an  animation.
      */
      void EndFrame();

      /**
      * Declares a node with multiple representative objects. If the node already
      * exists, it will be overwritten.
      *
      * @param nodeID The ID of the node. It must be unique for each node.
      * @param reps The set of representative objects.
      * @param radii The set of radius for each representative.
      * @param n The number of representative.
      * @param parent The parent node.
      * @param type The type ID.
      * @param active The declaration state.
      * @warning This command is not allowed outside a frame.
      */
      void SetNode(stPageID nodeID, ObjectType ** reps, stDistance * radii,
            int n, stPageID parent, int type, bool active = false);

      /**
      * Declares a node with only one representative object.
      *
      * @param nodeID The ID of the node. It must be unique for each node.
      * @param rep The set of representative objects.
      * @param radius The set of radius for each representative.
      * @param parent The parent node.
      * @param type The type ID.
      * @param active The declaration state.
      * @warning This command is not allowed outside a frame.
      */
      void SetNode(stPageID nodeID, ObjectType * rep, stDistance radius,
            stPageID parent, int type, bool active = false){
         SetNode(nodeID, &rep, &radius, 1, parent, type, active);
      }//end SetNode

      /**
      * Changes the state of a node to enabled. The node must exists or the command
      * will not produce any result. 
      *
      * @param nodeID The ID of the node. 
      * @warning This command is not allowed outside a frame.
      */
      void EnableNode(stPageID nodeID);

      /**
      * Declares an object. The object is identified by its coordinates and its
      * parent. The existing object will be overwritten.
      *
      * @param obj The object.
      * @param parent Its parent.
      * @param active The current state.
      * @warning This command is not allowed outside a frame.
      * @bug Two objects with the same pair coordinates/parent will be
      * considered the same. A solution to this is on he way.
      */
      void SetObject(ObjectType * obj, stPageID parent, bool active = false);

      /**
      * Declares the result state.
      *
      * @param queryObj The query object.
      * @param result Current result state.
      * @param k Desired k or 0 if it is not important.
      * @param radius Desired radius or 0 if it is not important.
      * @see SetResult(ObjectType,stResult<ObjectType>)
      * @warning This command is not allowed outside a frame.
      */
      void SetResult(ObjectType * queryObj, stResult <ObjectType> * result,
            stCount k, stDistance radius);

      /**
      * Declares the result state using the default k and radius values. See
      * SetQueryInfo() for more details.
      *
      * @param queryObj The query object.
      * @param result Current result state.
      * @see SetResult(ObjectType,stResult<ObjectType>,stCount,stDistance)
      * @see SetQueryInfo()
      * @warning This command is not allowed outside a frame.
      */
      void SetResult(ObjectType * queryObj, stResult <ObjectType> * result){
         SetResult(queryObj, result, K, Radius);
      }//end SetResult
      
      /**
      * Declares a result with only the sample object. It may be used to declare
      * the input object of an add operation.
      *
      * @param queryObj The query object.
      * @warning This command is not allowed outside a frame.
      * @todo This method is not implemented yet.
      */
      void SetSample(ObjectType * queryObj){}

      /**
      * Sets the default values to k and radius of a query. Useful when both
      * values do not change during an animation. This method is used in
      * combination with SetResult(ObjectType,stResult<ObjectType>) method. 
      *
      * @param k Desired k or 0 if it is not important.
      * @param radius Desired radius or 0 if it is not important.
      */
      void SetQueryInfo(int k, stDistance radius){
         K = k;
         Radius = radius;
      }//end SetQueryInfo

      /**
      * Sets the current level for all declaration commands.
      *
      * @param level Current level.
      * @see GetLevel()
      * @see LevelUp()
      * @see LevelDown()
      */
      void SetLevel(int level){
         Level = level;
      }//end SetLevel

      /**
      * Returns the current level for all declaration commands.
      *
      * @see LevelUp()
      * @see LevelDown()
      * @see SetLevel()
      */
      int GetLevel(){
         return Level;
      }//end GetLevel

      /**
      * Adds 1 to the current level.
      *
      * @see GetLevel()
      * @see LevelDown()
      * @see SetLevel()
      */
      void LevelUp(){
         Level++;
      }//end LevelUp

      /**
      * Subtracts 1 from current level.
      *
      * @see GetLevel()
      * @see LevelUp()
      * @see SetLevel()
      */
      void LevelDown(){
         Level--;
      }//end LevelDown

      /**
      * Set the output directory. All XML files will be stored in this
      * directory.
      *
      * @param outputDir The output directory. If none, it will assume the
      * current directory as the output directory.
      */
      void SetOutputDir(const char * outputDir);

      /**
      * Sets the serial number of the animation. By default it starts with 0.
      *
      * @param serial The first serial number for animations.
      */
      void SetSerial(int serial){
         Serial = serial;
      }//end SetSerial

      /**
      * Gets the serial number of the animation. Useful to save the state of the
      * visualization.
      *
      * @param serial The first serial number for animations.
      */
      int GetSerial(){
         return Serial;
      }//end GetSerial

   private:

      /**
      * The fastmapper type.
      */
      typedef stFastMapper <ObjectType, EvaluatorType> tMapper;

      /**
      * States of the report.
      */
      enum tStateIDs{
         ST_DISABLE = 0,
         ST_ANIMATION = 1,
         ST_FRAME = 2
      };//end tEntityIDs

      /**
      * The evaluator used by this instance. It was borowed from the tree. DO
      * NOT delete it.
      */
      EvaluatorType * Eval;

      /**
      * The mapper used to map objects.
      */
      tMapper * Mapper;

      /**
      * The state stack. It is used to track report errors. 
      */
      stack <int> StateStack;

      /**
      * The XML output stream. 
      */
      ostream * Output;

      /**
      * The output directory.
      */
      string OutputDir;

      /**
      * Number of frames in the animation.
      */
      int FrameCount;

      /**
      * Number of elements in the frame.
      */
      int FrameElementCount;

      /**
      * Number of results in the frame.
      */
      int FrameResultCount;

      /**
      * Current animation serial.
      */
      int Serial;

      /**
      * Current level of the objects.
      */
      int Level;

      /**
      * Desired K of the query.
      */
      stCount K;

      /**
      * Desired radius of the query.
      */
      stDistance Radius;

      /**
      * Opens and creates the first XML definitions.
      */
      void OpenNextXML();

      /**
      * Closes the XML file and checks for errors.
      */
      void CloseXML();

      /**
      * Writes the xyz attributes of a tag.
      *
      * @param map The xyz coordinates.
      * @warning The Output must not be NULL.
      */
      void WriteXYZ(double * map);      
};//end stMAMViewExtractor

#include <arboretum/stMAMView.cc>

#endif //__stMAMVIEW__

#endif //__STMAMVIEW_H

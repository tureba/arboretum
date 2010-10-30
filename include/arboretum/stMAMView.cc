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
* This file implements all class templates used by the MAMView data extraction module.
*
* @version 1.0
* $Revision: 1.12 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
*/
// Copyright (c) 2002 GBDI-ICMC-USP

/**
* The size of the file name.
*/
#define FILENAMESIZE 8

// Include for debug purpouses. It must be removed later.
#include <iostream>
#include <fstream>

//-----------------------------------------------------------------------------
// class stMAMViewObjectSample
//-----------------------------------------------------------------------------
template <class ObjectType>
stMAMViewObjectSample<ObjectType>::~stMAMViewObjectSample(){
   unsigned int i;

   for (i = 0; i < Size; i++){
      delete Objs[i];
   }//end for
   delete[] Objs;
}//end stMAMViewObjectSample<ObjectType>::~stMAMViewObjectSample

//-----------------------------------------------------------------------------
// class stMAMViewExtractor
//-----------------------------------------------------------------------------
// This macro is used to replace the expression:
//    stMAMViewExtractor<ObjectType, EvaluatorType>
// and make the code easy to read.
#define tmpl_stMAMViewExtractor stMAMViewExtractor<ObjectType, EvaluatorType>

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
tmpl_stMAMViewExtractor::~stMAMViewExtractor(){

   if (Mapper != NULL){
      delete Mapper;
   }//end if
}//end stMAMViewExtractor::~stMAMViewExtractor


//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::Init(
      stMAMViewObjectSample<ObjectType> * sample){

   if (Mapper != NULL){
      delete Mapper;
   }//end if
   Mapper = new tMapper(Eval);
   Mapper->ChoosePivots(sample->GetObjects(), sample->GetSize());
}//end stMAMViewExtractor::Init

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::SetOutputDir(const char * outputDir){

   OutputDir = outputDir;

   // Fix directory name if required.
   if (OutputDir.size() > 0){
      if (*(OutputDir.end() - 1) != DIRSEP){
         OutputDir.append(1, DIRSEP);
      }//end if
   }//end if
}//end stMAMViewExtractor::SetOutputDir

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::OpenNextXML(){
   string fileName;
   string fileTitle;
   char tmp[16];
   int len;
   ofstream * out;

   // Create file title
   //itoa(Serial, tmp, 10);
   sprintf(tmp, "%d", Serial);
   len = strlen(tmp);
   fileTitle.append(FILENAMESIZE - len, '0');
   fileTitle.append(tmp);
   fileTitle.append(".mva");
   Serial++;

   // Create the file name.
   fileName = OutputDir + fileTitle;

   // Open file
   out = new ofstream(fileName.c_str());
   Output = out;

   // Checking errors
   if (!out->is_open()){
      throw mamview_error("Unable to create the output file.");
   }//end if

   // Write XML header
   (*Output) << "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n";
   (*Output) << "<!DOCTYPE mamview SYSTEM \"mamview.dtd\">\n";
}//end stMAMViewExtractor::OpenNextXML

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::BeginAnimation(const char * title,
      const char * comment){

   // Error checking
   if (Output != NULL){
      throw mamview_error("Unable to begin a new animation.");
   }//end if
   if (Mapper == NULL){
      throw mamview_error("The mapper must be initialized first.");
   }//end if

   // Create the new XML file.
   OpenNextXML();

   // Initialize
   StateStack.push(ST_ANIMATION);
   (*Output) << "<animation>\n";

   // Description tags
   if ((title != NULL) || (comment != NULL)){
      (*Output) << "\t<description>\n";
      if (title != NULL){
         (*Output) << "\t\t<title>\n";
         (*Output) << "\t\t\t" << title << "\n";
         (*Output) << "\t\t</title>\n";
      }//end if
      if (comment != NULL){
         (*Output) << "\t\t<comment>\n";
         (*Output) << "\t\t\t" << comment << "\n";
         (*Output) << "\t\t</comment>\n";
      }//end if
      (*Output) << "\t</description>\n";
   }//end if

   FrameCount = 0;
   Level = 0;
}//end stMAMViewExtractor::BeginAnimation
//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::EndAnimation(){

   // Error checking
   if (Output == NULL){
      throw mamview_error("Unable to terminate the animation.");
   }//end if

   // Check fastmapper too

   // Check errors
   if (StateStack.top() != ST_ANIMATION){
      throw mamview_error("Unable to end the animation.");
   }else{
      if (FrameCount == 0){
         throw mamview_error("This animation has no frames.");
      }//end if
      StateStack.pop();
      (*Output) << "</animation>\n";
   }//end if

   delete Output;
   Output = NULL;
}//end stMAMViewExtractor::EndAnimation
//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::BeginFrame(const char * comment){

   if (StateStack.top() != ST_ANIMATION){
      throw mamview_error("Unable to begin a frame.");
   }else{
      // Initialize frame
      FrameResultCount = 0;
      FrameElementCount = 0;
      (*Output) << "\t<frame id=\"" << FrameCount << "\">\n";
      StateStack.push(ST_FRAME);

      if (comment != NULL){
        (*Output) << "\t\t<comment>\n";
        (*Output) << "\t\t\t" << comment << "\n";
        (*Output) << "\t\t</comment>\n";
      }//end comment

      // Update Frame counter
      FrameCount++;
   }//end if
}//end stMAMViewExtractor::BeginFrame

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::EndFrame(){

   if (StateStack.top() != ST_FRAME){
      throw mamview_error("Unable to end the frame.");
   }else{
      // Initialize frame
      if (FrameElementCount == 0){
         throw mamview_error("This frame is empty.");
      }//end if
      (*Output) << "\t</frame>\n";
      StateStack.pop();
   }//end if
}//end stMAMViewExtractor::EndFrame

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::WriteXYZ(double * map){

   (*Output) << "x=\"" << map[0] << "\" y=\"" << map[1] <<
         "\" z=\"" << map[2] << "\"";
}//end stMAMViewExtractor::WriteXYZ

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::SetResult(ObjectType * queryObj,
      stResult <ObjectType> * result, stCount k, stDistance radius){
   double map[3];
   int i;

   if (StateStack.top() != ST_FRAME){
      throw mamview_error("Unable add a result outside a frame.");
   }else{
      // Initialize frame
      if (FrameResultCount > 0){
         throw mamview_error("Only one result per frame is allowed.");
      }//end if

      // Update counters
      FrameResultCount++;
      FrameElementCount++;

      // Write output XML
      // BoQ
      (*Output) << "\t\t<query>\n";

      // Elements
      Mapper->Map(queryObj, map);
      (*Output) << "\t\t\t<sample ";
      WriteXYZ(map);
      (*Output) << " k=\"" << k << "\" radius=\"" << result->GetMaximumDistance() <<
            "\" innerRadius=\"0\" outerRadius=\"" << radius << "\" />\n";

      // Results
      for (i = 0; i < result->GetNumOfEntries(); i++){
         Mapper->Map((ObjectType *) ((*result)[i].GetObject()), map);
         (*Output) << "\t\t\t<answer ";
         WriteXYZ(map);
         (*Output) << " distance=\"" << (*result)[i].GetDistance() << "\"";

         (*Output) << " />\n";
      }//end for

      // EoQ
      (*Output) << "\t\t</query>\n";
   }//end if
}//end stMAMViewExtractor::SetResult

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::SetObject(ObjectType * obj, stPageID parent,
      bool active){
   double map[3];

   if (StateStack.top() != ST_FRAME){
      throw mamview_error("Unable add an object outside a frame.");
   }else{
      // Update counters
      FrameElementCount++;

      //Map object
      Mapper->Map(obj, map);

      // Write output XML
      if (active){
         (*Output) << "\t\t<object active=\"true\" ";
      }else{
         (*Output) << "\t\t<object active=\"false\" ";
      }//end if
      WriteXYZ(map);
      (*Output) << " level=\"" << Level << "\" parent=\"" << parent << "\" />\n";
   }//end if
}//end stMAMViewExtractor::SetObject

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::SetNode(stPageID nodeID, ObjectType ** reps,
      stDistance * radii, int n, stPageID parent, int type, bool active){
   double map[3];
   int i;

   if (StateStack.top() != ST_FRAME){
      throw mamview_error("Unable add a node outside a frame.");
   }else{
      // Update counters
      FrameElementCount++;

      if (active){
         (*Output) << "\t\t<node active=\"true\" ";
      }else{
         (*Output) << "\t\t<node active=\"false\" ";
      }//end if
      (*Output) << "id=\"" << nodeID << "\" parent=\"" << parent <<
            "\" type=\"" << type << "\" level=\""<< Level << "\" >\n";

      for (i = 0; i < n; i++){
         //Map object
         Mapper->Map(reps[i], map);
         // Write tag
         (*Output) << "\t\t\t<repobj radius=\"" << radii[i] << "\" ";
         WriteXYZ(map);
         (*Output) << " />\n";
      }//end for

      (*Output) << "\t\t</node>\n";
   }//end if
}//end stMAMViewExtractor::SetNode

//-----------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
void tmpl_stMAMViewExtractor::EnableNode(stPageID nodeID){
   double map[3];

   if (StateStack.top() != ST_FRAME){
      throw mamview_error("Unable add a command outside a frame.");
   }else{
      // Update counters
      FrameElementCount++;

      (*Output) << "\t\t<command name=\"ACTIVATENODE\">\n";
      (*Output) << "\t\t\t<param name=\"nodeid\" value=\"" << nodeID << "\" />\n";
      (*Output) << "\t\t</command>\n";
   }//end if
}//end stMAMViewExtractor::SetNode


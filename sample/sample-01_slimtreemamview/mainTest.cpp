//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "app.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
//---------------------------------------------------------------------------
#pragma argsused

#define __ubDEBUG__
#define __stDEBUG__

int main(int argc, char* argv[]){

   TApp app;
   // Init application.
   app.Init();
   // Run it.
   app.Run();
   // Release resources.
   app.Done();
   // Pause
   printf("\n\nPress enter to continue...");
   getchar();

   return 0;
}
//---------------------------------------------------------------------------

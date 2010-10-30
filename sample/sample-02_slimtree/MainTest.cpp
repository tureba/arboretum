//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("App.cpp", Form1);
USEFORM("AboutBox.cpp", AboutBox);
USERES("MainTest.res");
USELIB("..\..\lib\arboretum.lib");
USEUNIT("city.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   try
   {
       Application->Initialize();
       Application->Title = "Slim-Tree Demo";
       Application->CreateForm(__classid(TForm1), &Form1);
       Application->Run();
   }
   catch (Exception &exception)
   {
       Application->ShowException(&exception);
   }
   return 0;
}
//---------------------------------------------------------------------------

#ifndef CUTSUTILITIES_H
#define CUTSUTILITIES_H

#include <TTree.h>
#include <TTreeFormula.h>
#include <TChain.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include <TBranch.h>

#include <iostream>

//using namespace std;

#define SETCUTSFORMULA(x,y,z,w)(w = CutsUtilities::buildDataSetWithFraction(x,y,z))

namespace CutsUtilities
{
void activateBranches(TTree* tree, TObjArray* list, const char* cut, const char* var1 = 0, const char* var2 = 0, const char* var3 = 0, const char* var4 = 0);
TTreeFormula* buildDataSetWithFraction(TTree* ntuple, const char* vars, const char* cut);
TObjArray* stringDecode(const char* input, const char c);
const char* stringInArray(TObjArray* array, int ii);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* stringInArray(TObjArray* array, int i)
{
  if(i>array->GetLast())
     return 0;
  return((TObjString*)array->At(i))->GetString().Data();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TObjArray* stringDecode(const char* input, const char c)
{
  if(!input)
     return 0;

  if(strlen(input)==0)
     return 0; //  0 length string.

  TObjArray* container = new TObjArray();
  if(!container)
     return 0;

  Int_t i;
  Int_t j;
  TObjString *tmp = 0;

  TString* thisSubString = new TString("");
  if(!thisSubString)
  {
     delete container;
     return 0;
  }

  j=0;
  for(i=0; i<strlen(input); i++)
  {
      if(input[i]!=c)
      {
         thisSubString->Append(input[i]);
      }
      else
      {
         tmp = new TObjString(thisSubString->Data());
         if(tmp)
            container->AddLast(tmp);
         j++;
         delete thisSubString;
         thisSubString = new TString("");
         if(!thisSubString)
            return container;
      }
  }

  tmp = new TObjString(thisSubString->Data());
  if(tmp)
     container->AddLast(tmp);
  j++;
  delete thisSubString;
  return container;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void activateBranches(TTree* tree, TObjArray* list, const char* cut, const char* var1, const char* var2, const char* var3, const char* var4)
{
  if(!tree || !cut)
      return;

  if(list->GetLast()<0)
  {
     TObjArray* mybranches = tree->GetListOfBranches();
     TString mycut(cut);

     for(int b=0; b<mybranches->GetLast()+1; b++)
     {
         if(mycut.Contains(mybranches->At(b)->GetName()))
         {
            list->AddLast((new TObjString(mybranches->At(b)->GetName())));
         }
     }
     if(var1 && strlen(var1))
        list->AddLast((new TObjString(var1)));
     if(var2 && strlen(var2))
        list->AddLast((new TObjString(var2)));
     if(var3 && strlen(var3))
        list->AddLast((new TObjString(var3)));
     if(var4 && strlen(var4))
        list->AddLast((new TObjString(var4)));
  }

  // activate relevant branches only
  tree->SetBranchStatus("*", 0);
  for(int e=0; e<list->GetLast()+1; e++)
  {
      tree->SetBranchStatus(((TObjString*)(list->At(e)))->GetString().Data(),1);
      std::cout << __PRETTY_FUNCTION__ << "activate branches ... " <<  ((TObjString *)(list->At(e)))->GetString().Data() << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TTreeFormula* buildDataSetWithFraction(TTree* ntuple, const char* vars, const char* cut)
{
  TObjArray* listOfActiveBranches = 0;
  listOfActiveBranches = new TObjArray;

  TString mytmp(vars);
  if(strlen(vars)!=0)
     mytmp.Append(" && ");
  mytmp.Append(  cut );

  std::cout << __PRETTY_FUNCTION__ << "activate tree using string " << mytmp.Data() << std::endl;
  //activateBranches((TTree*)ntuple, listOfActiveBranches, mytmp.Data());

  if(strlen(vars)!=0)
  {
      std::cout << __PRETTY_FUNCTION__<< "strlen(vars)!=0" << std::endl;
      TObjArray* listOfVars = 0;
      listOfVars = stringDecode(vars,':');
      TObjArray listOfVarFormulas;
      for(int i=0; i<listOfVars->GetLast()+1; i++)
      {
          const char*   myvar = stringInArray(listOfVars,i);
          TTreeFormula* avar  = new TTreeFormula(myvar, myvar, ntuple);
          if(avar)
          {
             std::cout << __PRETTY_FUNCTION__ << "information: tree variable - " <<  myvar << std::endl;
             listOfVarFormulas.AddLast(avar);
          }
      }
  }

  TTreeFormula* common_cut = 0;
  if(strlen(cut))
     common_cut = new TTreeFormula(cut, cut, ntuple);

  return common_cut;
}

}
#endif // CUTSUTILITIES_H



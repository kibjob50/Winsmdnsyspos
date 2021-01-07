// Define DllExport to declare exported symbols.
#include "pch.h"
#include <windows.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <windows.system.profile.systemmanufacturers.h>
#include <roapi.h>
#include <stdio.h> // Horrors! Mixing C and C++!
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#include <shlobj_core.h>

#include <utility>
#include <limits.h>
#include <ole2.h>
#include "atlbase.h"
#include "atlstr.h"
#include "comdef.h"
#include <stdlib.h>


//using namespace System;
using namespace std;
namespace WRL = Microsoft::WRL;
namespace spsm = ABI::Windows::System::Profile::SystemManufacturers;
using Microsoft::WRL::Wrappers::HString;
using Microsoft::WRL::Wrappers::HStringReference;

#define DllExport __declspec( dllexport )
extern "C" DllExport   VARIANT_BOOL report0(signed short in_par);

// ______________________________________________
void spazzola()
{
    ifstream infile;
    infile.open("C:\\tmp\\ProveCDLL\\Sandbxsysval.spt", ios::binary | ios::in);
    ofstream oufile;
    oufile.open("C:\\tmp\\ProveCDLL\\pisq1.ttt", ios::binary | ios::out);
    // rest of program
    char x;
    while (infile.eof() == false)
    {
        infile.read(&x, 1); // reads 1 bytes into a cell that is either 2 or 4
        if (infile.eof() == false) oufile.write(&x, 1);
    }
    infile.close(); oufile.close();
}
// ______________________________________________
 

 
 



   int report1()
{
       TCHAR cartella[MAX_PATH];
       ofstream fileDllou;
       ifstream fileDllin;

       if (SUCCEEDED(SHGetFolderPath(NULL,
           CSIDL_PROGRAM_FILESX86    ,
           NULL,
           0,
           cartella)))
       {
           PathAppend(cartella, TEXT("ServicePlanning_BC"));
           PathAppend(cartella, TEXT("Winsmdnsysrate.dll"));

           fileDllin.open(cartella);
           if (fileDllin) {
               fileDllin.close();
               return 0;
           }
           else {
               fileDllou.open(cartella);
               fileDllou << "eccoci quà" << endl;
               return 1;
           }
       }
        
    //int __cdecl wmain(int, wchar_t**);
    const wchar_t* numser;
   
    size_t convertedChars = 0;
     
    CoInitialize(NULL);
    WRL::ComPtr<spsm::ISmbiosInformationStatics> statics;
    HString serialNumber;
    RoGetActivationFactory(HStringReference(
        RuntimeClass_Windows_System_Profile_SystemManufacturers_SmbiosInformation)
        .Get(), IID_PPV_ARGS(&statics));
    statics->get_SerialNumber(serialNumber.GetAddressOf());
    numser = serialNumber.GetRawBuffer(nullptr);
    size_t numsersize = wcslen(numser) + 1;
    const size_t newsize = numsersize * 2;
    char* nstring = new char[newsize];
    wcstombs_s(&convertedChars, nstring, newsize, numser, _TRUNCATE);
    //_mbscat_s((unsigned char*)nstring, newsize , (unsigned char*)strConcat);

    // BSTR str = SysAllocString(numseria);
    string firstName = "John ";
    string lastName = "Doe";
    string fullName = nstring + lastName;

    fileDllou.open("C:\\tmp\\ciccioexample.txt" );
    fileDllou << nstring << endl;

    fileDllou.close();
    return 0;
    //wstring ws(numseria);
    //string str(ws.begin(), ws.end());


    //SysFreeString(str);
    // wprintf(L"Serial number = %ls\n", serialNumber.GetRawBuffer(nullptr));

}
   extern "C" DllExport   VARIANT_BOOL report0(signed short in_par)
   {
       if (in_par == 0X1)
       {
           int retco = report1();
           return false;
       }
       if (in_par == 0x2)
           return true;
       return false;
   }
   // ______________________________________________ TESORIZZA
//wstring cercadirCorrente() {
//    TCHAR buffer[MAX_PATH] = { 0 };
//    GetModuleFileName(NULL, buffer, MAX_PATH);
//    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
//    return std::wstring(buffer).substr(0, pos);
//}
// ______________________________________________

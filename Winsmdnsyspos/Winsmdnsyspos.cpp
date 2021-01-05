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


// Prototype the function.
// Use 'extern "C"' to minimize name mangling.
extern "C" DllExport   int report1();

// Define the function.
extern "C" DllExport   int report1()

{
    //int __cdecl wmain(int, wchar_t**);
    const wchar_t* numser;
    ofstream file;
    size_t convertedChars = 0;
    //int snlen;


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

    file.open("C:\\tmp\\ciccioexample.txt");
    file << nstring << endl;

    file.close();
    return 0;
    //wstring ws(numseria);
    //string str(ws.begin(), ws.end());


    //SysFreeString(str);
    // wprintf(L"Serial number = %ls\n", serialNumber.GetRawBuffer(nullptr));

}
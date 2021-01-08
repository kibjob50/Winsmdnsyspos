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
extern "C" DllExport   signed short report0(signed short in_par);

// Variabili Globali
TCHAR cartellaInst[MAX_PATH];
TCHAR fulFnameToChkCopy[MAX_PATH];
TCHAR fulFnameToCreate[MAX_PATH];
ofstream fileDllou;
ifstream fileDllin;
char* numSeriale;


// ______________________________________________

int getPgmRif() // compila cartellaInst fulFnameDLL
// ritorna:     0 OK 1 KO
{
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_PROGRAM_FILESX86,
		NULL,
		0,
		cartellaInst)))
	{
		PathAppend(cartellaInst, TEXT("ServicePlanning_BC"));
		PathAppend(fulFnameToChkCopy, cartellaInst);
		PathAppend(fulFnameToChkCopy, TEXT("Sandbxsysval.spt"));
		PathAppend(fulFnameToCreate, cartellaInst);
		PathAppend(fulFnameToCreate, TEXT("Winsmdnsysrate.dll"));


		fileDllin.open(fulFnameToChkCopy);
		if (fileDllin) {
			fileDllin.close();
			return 0;
		}
		else   return 3;
	}
	return 3;
}
// ______________________________________________
void  getSnumb()
{
	size_t convertedChars = 0;
	const wchar_t* numser;

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
	numSeriale = new char[newsize];
	wcstombs_s(&convertedChars, numSeriale, newsize, numser, _TRUNCATE);


	//_mbscat_s((unsigned char*)nstring, newsize , (unsigned char*)strConcat);
}
// ______________________________________________
int checkSN()
//				0 OK  
// ritorna:     1 se non esiste DLL
//              2 se esiste ma con SN diverso
//  
{
	char x;
	char car;
	int y = 0, z = 0, retcode = 0;
	TCHAR fileVerifica[MAX_PATH];
	ifstream infile;
	int lenSN; int leninf;
	char* numRilevato; char ca1;
	getSnumb();
	if (getPgmRif() == 0) {
		lenSN = strlen(numSeriale);
		PathAppend(fileVerifica, fulFnameToCreate);

		infile.open(fileVerifica, ios::binary | ios::in);
		if (!fileVerifica) {
			retcode = 1;
			goto esci;
		}


		while (infile.eof() == false and y < 9301)
		{
			infile.read(&x, 1); // reads 1 bytes into a cell that is either 2 or 4
			if (infile.eof() == true) {
				retcode = 1;
				goto esci;
			}
			if (y == 9300) {
				leninf = x;

				if (leninf != lenSN) {
					retcode = 2;
					goto esci;
				}
				while (z < lenSN) {
					infile.read(&x, 1);
					car = numSeriale[z] + z + 30;
					if (car != x) {
						retcode = 2;
						goto esci;
					}
					z += 1;
				}
				retcode = 0;
				goto esci;
			}
			y += 1;
		}
	}
	else {
		retcode = 3;
	}
esci:
	infile.close();
	return retcode;
}


// ______________________________________________

int creaDLSN() // copia un pezzo di ServicePlan.accd(br) in Winsmdnsysgenerate.dll
			   // inserendo SN 
// ritorna:     0 OK 1 KO         
{
	TCHAR fileDaCopiare[MAX_PATH];
	TCHAR fileDaCreare[MAX_PATH];
	ifstream infile; ofstream oufile;
	int lenSN;
	getSnumb();
	if (getPgmRif() == 0) {
		lenSN = strlen(numSeriale);
		PathAppend(fileDaCopiare, fulFnameToChkCopy);

		infile.open(fileDaCopiare, ios::binary | ios::in);
		if (!fileDaCopiare)
			return 1;
		PathAppend(fileDaCreare, fulFnameToCreate);
		oufile.open(fileDaCreare, ios::binary | ios::out);

		char x;
		char car;
		int y = 0, z = 0;
		while (infile.eof() == false)
		{
			infile.read(&x, 1); // reads 1 bytes into a cell that is either 2 or 4
			if (infile.eof() == false) oufile.write(&x, 1);
			y += 1;
			if (y == 9300) {
				char ca1 = lenSN;
				oufile.write(&ca1, 1);
				while (z < lenSN) {
					car = numSeriale[z] + z + 30;
					oufile.write(&car, 1);
					z += 1;
				}
			}
		}
		infile.close(); oufile.close();
		return 0;
	}
	return 1;
}
// ______________________________________________

extern "C" DllExport  signed short report0(signed short in_par)
{
	if (in_par == 0X1)	return creaDLSN();
	if (in_par == 0x2)	return checkSN();
	return 9;
}
// ______________________________________________ TESORIZZA
//wstring cercadirCorrente() {
//    TCHAR buffer[MAX_PATH] = { 0 };
//    GetModuleFileName(NULL, buffer, MAX_PATH);
//    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
//    return std::wstring(buffer).substr(0, pos);
//}
// ______________________________________________

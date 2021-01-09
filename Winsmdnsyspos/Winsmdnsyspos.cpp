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

TCHAR fulFnameToChkCopy[MAX_PATH];
TCHAR fulFnameToCreate[MAX_PATH];
TCHAR installPath[MAX_PATH];
TCHAR pgdataPath[MAX_PATH];
TCHAR fulFnameWorka[MAX_PATH];
wchar_t* ProgramFilesX86 = new wchar_t[MAX_PATH];
wchar_t* ProgramData = new wchar_t[MAX_PATH];
 
TCHAR fileDaCopiare[MAX_PATH];
TCHAR fileDaCreare[MAX_PATH];
ifstream infile; ofstream oufile;
int lenSN;
char* numSeriale;
typedef wchar_t* LPWSTR, * PWSTR;

// ______________________________________________

int getPgmRif() // compila cartellaInst fulFnameDLL
// ritorna:     0 OK 1 KO
{

	HRESULT hrpg = SHGetKnownFolderPath(FOLDERID_ProgramFilesX86, 0, NULL, &ProgramFilesX86);
	HRESULT hrdd = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &ProgramData);

	if (SUCCEEDED(hrpg) && SUCCEEDED(hrdd))
	{
		PathAppend(installPath, ProgramFilesX86);
		PathAppend(installPath, TEXT("ServicePlanning_BC"));
		PathAppend(pgdataPath, ProgramData);
		PathAppend(pgdataPath, TEXT("WinSysDbs"));

		if (CreateDirectory(pgdataPath, NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{
			return 0;
		}
		else
		{
			return 3;
		}
	}
	return 3;
}
// ______________________________________________
int  getSnumb()
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
	if (numsersize < 5) return 1;
	else return 0;

	//_mbscat_s((unsigned char*)nstring, newsize , (unsigned char*)strConcat);
}
// ______________________________________________


	int lavora()	// ritorna:     0 OK 1 KO         
	{
	infile.open(fileDaCopiare, ios::binary | ios::in);
	if (!fileDaCopiare)
		return 1;

	oufile.open(fileDaCreare, ios::binary | ios::out);
	char x;
	char car;
	//copio i primi 128 bytes

	for (int a = 1; a <= 128; a = a + 1) {
		infile.read(&x, 1);
		if (infile.eof() == true) return 1;
		oufile.write(&x, 1);
	}

	//skip 20000   bytes
	for (int a = 1; a <= 20000; a = a + 1) {
		if (infile.eof() == true) return 1;
		infile.read(&x, 1);
	}
	//copio altri 40000 bytes

	for (int a = 1; a <= 40000; a = a + 1) {
		infile.read(&x, 1);
		if (infile.eof() == true) return 1;
		oufile.write(&x, 1);
	}
	//  inserto SN

	int z = 0;
	char ca1 = lenSN;
	oufile.write(&ca1, 1);
	while (z < lenSN) {
		car = numSeriale[z] + z + 30;
		oufile.write(&car, 1);
		z += 1;
	}

	//copio altri xxxxxx bytes

	for (int a = 1; a <= 480000; a = a + 1) {
		infile.read(&x, 1);
		if (infile.eof() == true) return 0; // stavolta mi va bene
		oufile.write(&x, 1);
	}
	//skip 40000   bytes
	for (int a = 1; a <= 20000; a = a + 1) {
		if (infile.eof() == true) return 1;
		infile.read(&x, 1);
	}
	//copio altri xxxxxx bytes

	for (int a = 1; a <= 380000; a = a + 1) {
		infile.read(&x, 1);
		if (infile.eof() == true) return 0; // stavolta mi va bene
		oufile.write(&x, 1);
	}
	return 0;


	infile.close(); oufile.close();
	return 0;
}

	// -------------------------------------------------------------------
int primaInst()	// ritorna:     0 OK 1 KO         
	{
		if (!getSnumb() == 0 || !getPgmRif() == 0)  return 1;

		// copio as is Sandbxsysval.spt come wyntol4ssvv.dll 
		PathAppend(fileDaCopiare, installPath);
		PathAppend(fileDaCopiare, TEXT("Sandbxsysval.spt"));
		PathAppend(fileDaCreare, pgdataPath);
		PathAppend(fileDaCreare, TEXT("wyntol4ssvv.dll"));

		ifstream source(fileDaCopiare, ios::binary);
		ofstream dest(fileDaCreare, ios::binary);
		dest << source.rdbuf();
		source.close();
		dest.close();

 		lenSN = strlen(numSeriale);
		ZeroMemory(fileDaCopiare, sizeof(fileDaCopiare));
		PathAppend(fileDaCopiare, pgdataPath);
		PathAppend(fileDaCopiare, TEXT("wyntol4ssvv.dll"));
		ZeroMemory(fileDaCreare, sizeof(fileDaCreare));
		PathAppend(fileDaCreare, pgdataPath);
		PathAppend(fileDaCreare, TEXT("Winsmdnsysrate.dll"));
		return lavora();
	}
// ______________________________________________
int rinnovo()	// ritorna:     0 OK 1 KO         
{
	if (!getSnumb() == 0 || !getPgmRif() == 0)  return 1;

 // ricreo Winsmdnsysrate  con inserto SN
	lenSN = strlen(numSeriale);
	ZeroMemory(fileDaCopiare, sizeof(fileDaCopiare));
	PathAppend(fileDaCopiare, pgdataPath);
	PathAppend(fileDaCopiare, TEXT("wyntol4ssvv.dll"));
	ZeroMemory(fileDaCreare, sizeof(fileDaCreare));
	PathAppend(fileDaCreare, pgdataPath);
	PathAppend(fileDaCreare, TEXT("Winsmdnsysrate.dll"));
	return lavora();
	}
// ______________________________________________
int controllo()
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

	getSnumb();

	if (!getPgmRif() == 0)  return 1;

	lenSN = strlen(numSeriale);
	PathAppend(fileVerifica, pgdataPath);
	PathAppend(fileVerifica, TEXT("Winsmdnsysrate.dll"));

	infile.open(fileVerifica, ios::binary | ios::in);
	if (!fileVerifica) {
		retcode = 1;
		goto esci;
	}

	//skip 40128   bytes
	for (int a = 1; a <= 40129; a = a + 1) {
		if (infile.eof() == true) return 1;
		infile.read(&x, 1);
	}
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

esci:
	infile.close();
	return retcode;
} // controllo() fine
// ______________________________________________

extern "C" DllExport  signed short report0(signed short in_par)
{
	if (in_par == 0X0)	return primaInst();
	if (in_par == 0x1)	return controllo();
	if (in_par == 0X2)	return rinnovo();
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

#include <windows.h>
#include "archiver.h"

CArchiver::CArchiver()
{
}


CArchiver::~CArchiver()
{
}


void CArchiver::Unzip(unsigned char *buffer, int size, wstring path)
{
	WCHAR oldcurdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, oldcurdir);
	SetCurrentDirectory(path.c_str());

	HZIP hz = OpenZip(buffer,size,0);
    ZIPENTRY ze;
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int zi=0; zi<numitems; zi++)
	{ 
		ZIPENTRY ze; 
		GetZipItem(hz, zi, &ze);
		UnzipItem(hz, zi, ze.name);       
	}
	CloseZip(hz);
	
	SetCurrentDirectory(oldcurdir);
}

void CArchiver::Unzip(wstring arcfile, wstring path)
{
	WCHAR oldcurdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, oldcurdir);
	SetCurrentDirectory(path.c_str());

	HZIP hz = OpenZip(arcfile.c_str(), NULL);
    ZIPENTRY ze;
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int zi=0; zi<numitems; zi++)
	{ 
		ZIPENTRY ze; 
		GetZipItem(hz, zi, &ze);
		UnzipItem(hz, zi, ze.name);       
	}
	CloseZip(hz);
	SetCurrentDirectory(oldcurdir);
}

void CArchiver::ZipRecursively(wstring root, wstring path, HZIP hz)
{
	wstring dir = path + L"\\*";
	WIN32_FIND_DATA ffdata;
	HANDLE find = FindFirstFile(dir.c_str(), &ffdata);
	do
	{
		if (lstrcmp(ffdata.cFileName,L".") !=0 && 
			lstrcmp(ffdata.cFileName,L"..") != 0 &&
			lstrcmp(ffdata.cFileName,L"template.docx") != 0)
		{
			if ( ffdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				wstring cdir = path + L"\\" + ffdata.cFileName;
				ZipRecursively(root, cdir,hz);
			}
			else
			{
				wstring dfile = path + L"\\" + ffdata.cFileName;
				dfile.erase(0, root.length()+1);
				ZipAdd(hz,(const TCHAR *)dfile.c_str(),(const TCHAR *)dfile.c_str());
			}
		}
	}
	while(FindNextFile(find, &ffdata));
	FindClose(find);
}

void CArchiver::Zip(wstring path)
{
	WCHAR oldcurdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, oldcurdir);
	SetCurrentDirectory(path.c_str());

	HZIP hz = CreateZip(L"template.pptx",0);
	ZipRecursively(path, path, hz);
	CloseZip(hz);

	SetCurrentDirectory(oldcurdir);
}
#pragma once
#include <string>
using namespace std;
//#include "consts.h"
#include "ziputils/zip.h"
#include "ziputils/unzip.h"

class CArchiver
{
private:
	void ZipRecursively(wstring root, wstring path, HZIP hz);
public:
	CArchiver();
	~CArchiver();

	void Unzip(unsigned char *buffer, int size, wstring path);
	void Unzip(wstring arcfile, wstring path);
	void Zip(wstring path);
};


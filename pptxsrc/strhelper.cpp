#include "strhelper.h"

void trim(wstring& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

void replace_with(wstring & src, const wstring & what, const wstring & with)
{    
    if (what != with) {
        wstring temp;
        wstring::size_type prev_pos = 0, pos = src.find(what, 0);
        while ( wstring::npos != pos ) {
            temp += wstring(src.begin() + prev_pos, src.begin() + pos) + with;
            prev_pos = pos + what.size();
            pos = src.find(what, prev_pos);
        }
        if ( !temp.empty() ) {
            src = temp + wstring(src.begin() + prev_pos, src.end());
            if (wstring::npos == with.find(what)) {
                replace_with(src, what, with);
            }
        }
    }
}

vector<wstring> split(wstring &str, wchar_t sep)
{
	vector<wstring> result;
	wstring s;

	int b = 0;
	int n = str.find(sep);
	for(;;)
	{
		if (n==-1)
		{
			s = str.substr(b,str.length()-b);
			if (s != L"")
				result.push_back(s);
			break;
		}
		s = str.substr(b,n-b);
		if (s != L"")
			result.push_back(s);
		b = n+1;
		n = str.find(sep,b);
	}
	return result;


}

wstring lower(wstring s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}
	
wstring make_str1(wstring ws1, int i1, wstring ws2)
{
	wstringstream wss;
	wss << ws1;
	wss << i1;
	wss << ws2;
	return wss.str();
}
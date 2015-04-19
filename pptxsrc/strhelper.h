#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

void trim(wstring& str);
void replace_with(wstring & src, const wstring & what, const wstring & with);
vector<wstring> split(wstring &str, wchar_t sep);
wstring lower(wstring s);
wstring make_str1(wstring ws1, int i1, wstring ws2);
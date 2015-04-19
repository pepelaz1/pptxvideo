#pragma once
#import "MSXML3.dll" named_guids 
using namespace MSXML2;

#include <string>
#include <vector>
using namespace std;

// --- CXmlNodeWrapper ---
class CXmlNodeWrapper  
{
private:
	bool m_bAutoRelease;
	MSXML2::IXMLDOMNodePtr m_xmlnode;
public:
	CXmlNodeWrapper();
	CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,bool bAutoRelease = true);
	void operator=(MSXML2::IXMLDOMNodePtr pNode);
	MSXML2::IXMLDOMNode* Interface();
	bool IsValid();
	wstring Name();
	long ChildrenCount();
	MSXML2::IXMLDOMNodePtr RemoveNode(MSXML2::IXMLDOMNodePtr node);
	MSXML2::IXMLDOMNodePtr InsertChildNode(int index,string nodeName, string namespaceUri="");
	MSXML2::IXMLDOMNodePtr GetChildNode(int nodeIndex);
	MSXML2::IXMLDOMNodePtr FindChildNode(string searchString);
	MSXML2::IXMLDOMNodePtr InsertChildAfter(MSXML2::IXMLDOMNodePtr refNode, string nodeName, string namespaceUri="");
	MSXML2::IXMLDOMNodePtr InsertChildAfter2(MSXML2::IXMLDOMNodePtr refNode, string nodeName);
	MSXML2::IXMLDOMNodePtr InsertChildBefore(MSXML2::IXMLDOMNodePtr refNode, string nodeName, string namespaceUri="");
	void SetText(string text);
	void SetTextRaw(_bstr_t text);
	void SetTextW(vector<unsigned short> &text);
	void SetAttrValue(string attrName, string value);
	void SetAttrValueTrick(string attrName,string value);
	wstring GetAttrValue(wstring attrName);
	_bstr_t GetText();

};

// --- CXmlDocWrapper ---
class CXmlDocWrapper
{
private:
	MSXML2::IXMLDOMDocumentPtr m_xmldoc;
public:
	CXmlDocWrapper();
	~CXmlDocWrapper();

	void Release();
	bool IsValid();
	bool Load(wstring file);
	bool Save(wstring file = L"");
	MSXML2::IXMLDOMNodePtr AsNode();
};


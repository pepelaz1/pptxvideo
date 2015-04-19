#include <Windows.h>
#include "xmlwrapper.h"


// --- CXmlNodeWrapper ---
CXmlNodeWrapper::CXmlNodeWrapper()
{
}

CXmlNodeWrapper::CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,bool bAutoRelease)
{
	m_xmlnode = pNode;
	m_bAutoRelease = bAutoRelease;
}

void CXmlNodeWrapper::operator=(MSXML2::IXMLDOMNodePtr pNode)
{
	m_xmlnode = pNode;
}

bool CXmlNodeWrapper::IsValid()
{
	if (m_xmlnode == NULL)
		return false;
	if (m_xmlnode.GetInterfacePtr() == NULL)
		return FALSE;
	return true;
}


MSXML2::IXMLDOMNode* CXmlNodeWrapper::Interface()
{
	if (IsValid())
		return m_xmlnode;
	return NULL;

}
wstring CXmlNodeWrapper::Name()
{
	if (IsValid())
		return (LPWSTR)m_xmlnode->GetbaseName();
	return L"";
}

long CXmlNodeWrapper::ChildrenCount()
{
	if (IsValid())
	{
		return m_xmlnode->GetchildNodes()->Getlength();
	}
	else
		return 0;
}

MSXML2::IXMLDOMNodePtr CXmlNodeWrapper::InsertChildNode(int index, string nodeName,string namespaceUri)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		if (namespaceUri.empty())
			namespaceUri = m_xmlnode->GetnamespaceURI();

		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT)
			,nodeName.c_str(),namespaceUri.c_str());
		MSXML2::IXMLDOMNodePtr refNode = GetChildNode(index);
		if (refNode)
			newNode = m_xmlnode->insertBefore(newNode,_variant_t(refNode.GetInterfacePtr()));
		else
			newNode = m_xmlnode->appendChild(newNode);
		return newNode;
	}
	return NULL;
}

MSXML2::IXMLDOMNodePtr CXmlNodeWrapper::RemoveNode(MSXML2::IXMLDOMNodePtr node)
{
	if (!IsValid())
		return NULL;
	return m_xmlnode->removeChild(node);
}

MSXML2::IXMLDOMNodePtr CXmlNodeWrapper::GetChildNode(int nodeIndex)
{
	if (!IsValid())
		return NULL;
	return m_xmlnode->GetchildNodes()->Getitem(nodeIndex);
}

MSXML2::IXMLDOMNodePtr CXmlNodeWrapper::FindChildNode(string searchString)
{
	if (!IsValid())
		return NULL;
	try{
		return m_xmlnode->selectSingleNode(searchString.data());
	}
	catch (_com_error e)
	{
		wstring err = e.ErrorMessage();
	}
	return NULL;
}

MSXML2::IXMLDOMNodePtr CXmlNodeWrapper::InsertChildBefore(MSXML2::IXMLDOMNodePtr refNode, string nodeName, string namespaceUri)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		if (namespaceUri.empty())
			namespaceUri = m_xmlnode->GetnamespaceURI();

		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName.c_str(),namespaceUri.c_str());
		newNode = m_xmlnode->insertBefore(newNode,_variant_t(refNode.GetInterfacePtr()));

		return newNode;
	}
	return NULL;
}

MSXML2::IXMLDOMNodePtr CXmlNodeWrapper::InsertChildAfter(MSXML2::IXMLDOMNodePtr refNode, string nodeName, string namespaceUri)
{
	
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		if (namespaceUri.empty())
			namespaceUri = m_xmlnode->GetnamespaceURI();

		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName.c_str(),namespaceUri.c_str());
		MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
		if (nextNode.GetInterfacePtr() != NULL)
			newNode = m_xmlnode->insertBefore(newNode,_variant_t(nextNode.GetInterfacePtr()));
		else
			newNode = m_xmlnode->appendChild(newNode);
		return newNode;
	}
	return NULL;
}

MSXML2::IXMLDOMNodePtr CXmlNodeWrapper::InsertChildAfter2(MSXML2::IXMLDOMNodePtr refNode, string nodeName)
{
	
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		
		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName.c_str(),"");
		MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
		if (nextNode.GetInterfacePtr() != NULL)
			newNode = m_xmlnode->insertBefore(newNode,_variant_t(nextNode.GetInterfacePtr()));
		else
			newNode = m_xmlnode->appendChild(newNode);
		return newNode;
	}
	return NULL;
}

void CXmlNodeWrapper::SetText(string text)
{
	if (m_xmlnode)
	{
		//_bstr_t str = L"\xFEFF\x0020\x91CD\x8981\x987B\x77E5";
		//_bstr_t str = L"\x47\x4E\x55\x20";
	//	m_xmlnode->Puttext(str);
		m_xmlnode->Puttext(text.c_str());
	}
}

void CXmlNodeWrapper::SetTextRaw(_bstr_t text)
{
	if (m_xmlnode)
		m_xmlnode->Puttext(text);
}

void CXmlNodeWrapper::SetTextW(vector<unsigned short> &text)
{
	if (this->m_xmlnode)
	{
		_bstr_t str = "";
		for ( unsigned int i = 0; i < text.size(); i++)
		{
			WCHAR w[8];
			wsprintfW(w,L"%c", text[i]);
			str += w;
		}
		m_xmlnode->Puttext(str);
	}
}

_bstr_t CXmlNodeWrapper::GetText()
{
	if (IsValid())
		return m_xmlnode->Gettext();
	else
		return "";
}

void CXmlNodeWrapper::SetAttrValue(string attrName,string value)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attributes = m_xmlnode->Getattributes();
		if (attributes)
		{
			MSXML2::IXMLDOMAttributePtr attribute = xmlDocument->createAttribute(attrName.c_str());
			if (attribute)
			{
				
				attribute->Puttext(value.c_str());
				attributes->setNamedItem(attribute);
			}
		}
	}
}

void CXmlNodeWrapper::SetAttrValueTrick(string attrName,string value)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attributes = m_xmlnode->Getattributes();
		if (attributes)
		{
			_variant_t nt =  2;
			MSXML2::IXMLDOMAttributePtr attribute = xmlDocument->createNode(nt, attrName.c_str(), "r");
			if (attribute)
			{
				attribute->Puttext(value.c_str());
				attributes->setNamedItem(attribute);
			}
		}
	}
}


wstring CXmlNodeWrapper::GetAttrValue(wstring attrName)
{
	if (!IsValid())
		return L"";

	MSXML2::IXMLDOMNodePtr attribute = m_xmlnode->Getattributes()->getNamedItem(attrName.c_str());
	if (attribute)
	{
		return (LPWSTR)attribute->Gettext();
	}
	return L"";
}

// --- CXmlDocWrapper ---
CXmlDocWrapper::CXmlDocWrapper()
{
	CoInitialize(NULL);
	m_xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);
}


CXmlDocWrapper::~CXmlDocWrapper()
{
//	CoUninitialize();
}

bool CXmlDocWrapper::IsValid()
{
	if (m_xmldoc == NULL)
		return false;
	if (m_xmldoc.GetInterfacePtr() == NULL)
		return false;
	return true;
}


void CXmlDocWrapper::Release()
{
	if (IsValid())
	{
		m_xmldoc.Release();
		m_xmldoc = NULL;
	}
}

bool CXmlDocWrapper::Load(wstring file)
{
	if (!IsValid())
		return false;

	_variant_t v(file.c_str());
	m_xmldoc->put_async(VARIANT_FALSE);
	VARIANT_BOOL success = m_xmldoc->load(v);

	m_xmldoc->put_preserveWhiteSpace(VARIANT_TRUE);
	if (success == VARIANT_TRUE)
		return true;
	else
		return false;
}

bool CXmlDocWrapper::Save(wstring file)
{
	try
	{
		if (!IsValid())
			return false;

		wstring sfile = file;
		if (sfile.empty())
			sfile = m_xmldoc->Geturl();
		_variant_t v(sfile.c_str());
		if (FAILED(m_xmldoc->save(v)))
			return false;
		else
			return true;
	}
	catch(...)
	{
		return false;
	}
}

MSXML2::IXMLDOMNodePtr CXmlDocWrapper::AsNode()
{
	if (!IsValid())
		return NULL;
	return m_xmldoc->GetdocumentElement();
}



#include <windows.h>
#include "parser.h"
#include <shlwapi.h>
#include "strhelper.h"


CParser::CParser(void) : m_path (L"")
{
}


CParser::~CParser(void)
{
}


void CParser::Load(wstring file, CDoc &doc)
{
	m_file = file;

	// Create temp path
	WCHAR s[MAX_PATH];
	GetTempPath(MAX_PATH,s);
	wstring path = s;
	
	GUID *guid = new GUID();
	CoCreateGuid(guid); 
	RPC_WSTR guid_s;
	UuidToString(guid, &guid_s);
	delete guid;
	
	path.append(L"\\");
	path.append((WCHAR *)guid_s);
	CreateDirectory(path.c_str(),NULL);

	// Extract data into temp path
	CArchiver arc;
	arc.Unzip(m_file,path);

	// Parse files into document
	Parse(path, doc);

	// Clear temporary directory
	ClearDirectory(path.c_str());
}

void CParser::ClearDirectory(wstring directory)
{
	wstring dir = directory + L"\\*";
	WIN32_FIND_DATA ffdata;
	HANDLE find = FindFirstFile(dir.c_str(), &ffdata);
	do
	{
		if (lstrcmp(ffdata.cFileName,L".") !=0 && 
			lstrcmp(ffdata.cFileName,L"..") != 0)
		{
			if ( ffdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				wstring cdir = directory + L"\\" + ffdata.cFileName;
				ClearDirectory(cdir);
				RemoveDirectory(cdir.c_str());
			}
			else
			{
				wstring dfile = directory + L"\\" + ffdata.cFileName;
				DeleteFile(dfile.c_str());
			}
		}
	}
	while(FindNextFile(find, &ffdata));
	FindClose(find);

	// Also remove directory itself
	RemoveDirectory(directory.c_str());
}

void CParser::ParsePresProps(wstring path, CDoc &doc)
{
	wstringstream wss;
	wss << m_path;
	wss << L"\\ppt\\presProps.xml";
	CXmlDocWrapper xml;
	if (!xml.Load(wss.str()))
		return;

	CXmlNodeWrapper root(xml.AsNode());
	CXmlNodeWrapper showPr(root.FindChildNode("p:showPr"));
	doc.SetLoop((bool)_wtoi(showPr.GetAttrValue(L"loop").c_str()));

	CXmlNodeWrapper kiosk(showPr.FindChildNode("p:kiosk"));
	doc.SetDisableClics(kiosk.IsValid());
}

void CParser::Parse(wstring path, CDoc &doc)
{
	m_path = path;

	ParsePresProps(path, doc);

	int num = 1;
	for(;;)
	{
		wstringstream wss;
		wss << m_path;
		wss << L"\\ppt\\slides\\slide";
		wss << num;
		wss << L".xml";
		
		CXmlDocWrapper xml;
		if (!xml.Load(wss.str()))
			break;

		CSlide *sld = new CSlide();
		ParseSlide(num, sld, xml);
		
		doc.AddSlide(sld);
		num++;
	}

	if (num == 1)
	{
		// add fake slide
		CSlide *sld = new CSlide();
		sld->SetNum(0);
		doc.AddSlide(sld);
	}

	if (!doc.IsLoop())
	{
		// Add "End of presentation" slide
		CSlide *sld = new CSlide();
		sld->SetNum(num-1);
		sld->SetBkColor(RGB(0,0,0));
		CSldText *txt = new CSldText();
		CPara para;
		CRow row;
		row.SetText(wstring(L"End of presentation"));
		row.SetTextColor(RGB(255,255,255));
		row.SetFontname(wstring(L"Arial"));
		row.SetFontsize(2000);
		row.SetAlign(1);
		para.AddRow(row);
		txt->SetPosition(0,324130, 9144000.0,524130);								   
		txt->AddPara(para);
		sld->AddObj(txt);
		doc.AddSlide(sld);
	}
}

void CParser::ParseSlide(int num, CSlide *sld, CXmlDocWrapper &xml)
{
	sld->SetNum(num-1);

	// Parse slide time
	CXmlNodeWrapper root(xml.AsNode());
	CXmlNodeWrapper AlternateContent(root.FindChildNode("mc:AlternateContent"));
	CXmlNodeWrapper Choice(AlternateContent.FindChildNode("mc:Choice"));
	CXmlNodeWrapper transition(Choice.FindChildNode("p:transition"));

	wstring wdur = transition.GetAttrValue(L"advTm");
	sld->SetDuration(_wtoi(wdur.c_str()));
	
	// Parse slide background
	CXmlNodeWrapper cSld(root.FindChildNode("p:cSld"));
	CXmlNodeWrapper bg(cSld.FindChildNode("p:bg"));
	CXmlNodeWrapper bgPr(bg.FindChildNode("p:bgPr"));
	CXmlNodeWrapper solidFill(bgPr.FindChildNode("a:solidFill"));
	CXmlNodeWrapper srgbClr(solidFill.FindChildNode("a:srgbClr"));

	wstring wbkclr = L"0x" + srgbClr.GetAttrValue(L"val");
	int val = 0;
	if (StrToIntExW(wbkclr.c_str(), STIF_SUPPORT_HEX, &val))
		sld->SetBkColor(val);

	// Parse slide picture
	CXmlNodeWrapper spTree(cSld.FindChildNode("p:spTree"));
	CXmlNodeWrapper pic(spTree.FindChildNode("p:pic"));
	ParseSldImg(sld,pic);
	
	// Parse slide text
	CXmlNodeWrapper sp(spTree.FindChildNode("p:sp"));
	ParseSldText(sld,sp);

}

void CParser::ParseSldImg( CSlide *sld,  CXmlNodeWrapper &pic)
{
	CXmlNodeWrapper blipFill(pic.FindChildNode("p:blipFill"));
	CXmlNodeWrapper blip(blipFill.FindChildNode("a:blip"));
	wstring imgid = blip.GetAttrValue(L"r:embed");

	wstringstream wss;
	wss << m_path;
	wss << L"\\ppt\\slides\\_rels\\slide";
	wss << sld->GetNum()+1;
	wss << L".xml.rels";

	CXmlDocWrapper xml2;
	xml2.Load(wss.str());

	CXmlNodeWrapper root(xml2.AsNode());
	CSldImage *img = NULL;
	for ( int i = 0; i < root.ChildrenCount(); i++)
	{
		CXmlNodeWrapper chld(root.GetChildNode(i));
		wstring id = chld.GetAttrValue(L"Id");
		if (imgid == id)
		{
			img = new CSldImage();

			wstringstream wss1;
			wss1 << m_path;
			wss1 << L"\\ppt\\";
			wstring trg = chld.GetAttrValue(L"Target");
			replace_with( trg,L"../",L"\\");
			replace_with( trg,L"/",L"\\");
			wss1 << trg;

			img->Load( wss1.str());
			sld->AddObj(img);
			break;
		}
	}
	if (img)
	{
		CXmlNodeWrapper spPr(pic.FindChildNode("p:spPr"));
		CXmlNodeWrapper xfrm(spPr.FindChildNode("a:xfrm"));
		CXmlNodeWrapper off(xfrm.FindChildNode("a:off"));
		CXmlNodeWrapper ext(xfrm.FindChildNode("a:ext"));
		img->SetPosition(_wtoi(off.GetAttrValue(L"x").c_str()), _wtoi(off.GetAttrValue(L"y").c_str()), 
			_wtoi(ext.GetAttrValue(L"cx").c_str()),_wtoi(ext.GetAttrValue(L"cy").c_str()));
	}
}

void CParser::ParseSldText( CSlide *sld,  CXmlNodeWrapper &sp)
{
	CSldText *text = new CSldText();
	CXmlNodeWrapper spPr(sp.FindChildNode("p:spPr"));
	CXmlNodeWrapper xfrm(spPr.FindChildNode("a:xfrm"));
	CXmlNodeWrapper off(xfrm.FindChildNode("a:off"));
	CXmlNodeWrapper ext(xfrm.FindChildNode("a:ext"));
	text->SetPosition(_wtoi(off.GetAttrValue(L"x").c_str()), _wtoi(off.GetAttrValue(L"y").c_str()), 
			_wtoi(ext.GetAttrValue(L"cx").c_str()),_wtoi(ext.GetAttrValue(L"cy").c_str()));

	CXmlNodeWrapper txBody(sp.FindChildNode("p:txBody"));
	for ( int i = 0; i < (int)txBody.ChildrenCount(); i++)
	{
		CXmlNodeWrapper chld(txBody.GetChildNode(i));
		if ( chld.Name() == L"p")
		{
			CPara p;

			int align = 0;
			for ( int j = 0; j < (int)chld.ChildrenCount(); j++)
			{
				CXmlNodeWrapper cchld(chld.GetChildNode(j));
				if ( cchld.Name() == L"pPr")
				{
					wstring walign = cchld.GetAttrValue(L"algn");
					if( walign == L"ctr")
						align = 1;

					CXmlNodeWrapper buChar(cchld.FindChildNode("a:buChar"));
					wstring wbuchar = buChar.GetAttrValue(L"char");
					p.SetBuChar(wbuchar);

				}
				else if ( cchld.Name() == L"r")
				{
					CRow r;

					CXmlNodeWrapper rPr(cchld.FindChildNode("a:rPr"));
					r.SetFontsize(_wtoi(rPr.GetAttrValue(L"sz").c_str()));
					r.SetBold(_wtoi(rPr.GetAttrValue(L"b").c_str()));
					r.SetItalic(_wtoi(rPr.GetAttrValue(L"i").c_str()));
					r.SetUnderline(rPr.GetAttrValue(L"u").c_str());

					CXmlNodeWrapper solidFill(rPr.FindChildNode("a:solidFill"));
					CXmlNodeWrapper srgbClr(solidFill.FindChildNode("a:srgbClr"));				
					wstring clr = L"0x" + srgbClr.GetAttrValue(L"val");
					int val = 0;
					if (StrToIntExW(clr.c_str(), STIF_SUPPORT_HEX, &val))
						r.SetTextColor(val);

					CXmlNodeWrapper cs(rPr.FindChildNode("a:cs"));
					r.SetFontname(cs.GetAttrValue(L"typeface"));

					CXmlNodeWrapper t(cchld.FindChildNode("a:t"));
					wstring s = t.GetText();
					if (s == L"") s = L" ";
					r.SetText(s);

					r.SetAlign(align);
					p.AddRow(r);
				}
			}
			text->AddPara(p);			
		}
	}
	sld->AddObj(text);
}
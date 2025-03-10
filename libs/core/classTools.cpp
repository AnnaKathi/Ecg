//---------------------------------------------------------------------------
#pragma hdrstop


#include "classTools.h"

#include "core.h"
#include <system.sysutils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
cTools::cTools()
	{
	}
//---------------------------------------------------------------------------
/***************************************************************************/
/**************   Formulardaten laden und speichern   **********************/
/***************************************************************************/
//---------------------------------------------------------------------------
String cTools::GetApplicationPath()
	{
	String path = ExtractFilePath(ParamStr(0));
	if (path.Length()) path = path.SubString(1, path.Length()-1);
	return path;
	}
//---------------------------------------------------------------------------
String cTools::GetProgramPath()
	{
	String p = ExtractFilePath(ParamStr(0));
	if (!p.Length()) return "";

	char path[MAX_PATH];
	sprintf(path, "%s", AnsiString(p.SubString(1, p.Length()-1)));
	char* pt = strrchr(path, '\\');
	if (!pt) return "";
	*pt = 0; //Verzeichnis "bin"

	pt = strrchr(path, '\\');
	if (!pt) return "";
	*pt = 0; //Verzeichnis "out" entfernt -> z.B. s:\Anna\src\ecg

	return String(path);
	}
//---------------------------------------------------------------------------
String cTools::GetIniFile()
	{
	//Inifile erstellen, falls nicht vorhanden
	char inifile[MAX_PATH];
	sprintf(inifile, "%s\\ecg.ini", AnsiString(GetApplicationPath().c_str()));

	if (!FileExists(inifile))
		{
		FILE* fp = fopen(AnsiString(inifile).c_str(), "w+");
		fprintf(fp, "[Ecg]\n");
		fclose(fp);
		}

	return String(inifile);
	}
//---------------------------------------------------------------------------
void cTools::FormLoad(TForm* fm)
	{
	TIniFile* Ini = new TIniFile(GetIniFile());

	for (int i = 0; i < fm->ComponentCount; i++)
		{
		TComponent* comp = fm->Components[i];
		if (String(comp->ClassName()) == "TEdit")
			{
			TEdit* ed = (TEdit*) comp;
			String key = fm->Name + "." + ed->Name;
			ed->Text = Ini->ReadString("FormEdits", key, "");
			}

		else if (String(comp->ClassName()) == "TComboBox")
			{
			TComboBox* cb = (TComboBox*) comp;
			String key = fm->Name + "." + cb->Name;
			cb->ItemIndex = Ini->ReadInteger("FormCombos", key, 0);
			}

		else if (String(comp->ClassName()) == "TCheckBox")
			{
			TCheckBox* cx = (TCheckBox*) comp;
			String key = fm->Name + "." + cx->Name;
			cx->Checked = Ini->ReadBool("FormChecks", key, false);
			}
		}

	//Formularpositionen laden
	int left = Ini->ReadInteger("FormPos", fm->Name + ".Left", -1);
	if (left < 0) //kein Wert gespeichert
		left = (Screen->DesktopWidth - fm->Width)/2;  //mittig ausrichten
	else if ((left + fm->Width) > Screen->DesktopWidth)
		left = Screen->DesktopWidth - fm->Width; //rechts ausrichten
	fm->Left = left;

	int top = Ini->ReadInteger("FormPos", fm->Name + ".Top", -1);
	if (top < 0) //kein Wert gespeichert
		top = (Screen->DesktopHeight - fm->Height)/2;  //mittig ausrichten
	else if ((top + fm->Height) > Screen->DesktopHeight)
		top = Screen->DesktopHeight - fm->Height; //unten ausrichten
	fm->Top = top;

	//Formulardimensionen laden
	//diese Width ist evtl anders als die vordesignte fm->Width
	int width = Ini->ReadInteger("FormPos", fm->Name + ".Width", -1);
	if (width > 0 && ((fm->Left + width) <= Screen->DesktopWidth))
		fm->Width = width;

	int height = Ini->ReadInteger("FormPos", fm->Name + ".Height", -1);
	if (height > 0 && ((fm->Top + height) <= Screen->DesktopHeight))
		fm->Height = height;

	delete Ini;
	}
//---------------------------------------------------------------------------
void cTools::FormSave(TForm* fm)
	{
	TIniFile* Ini = new TIniFile(GetIniFile());

	for (int i = 0; i < fm->ComponentCount; i++)
		{
		TComponent* comp = fm->Components[i];
		if (String(comp->ClassName()) == "TEdit")
			{
			TEdit* ed = (TEdit*) comp;
			String key = fm->Name + "." + ed->Name;
			Ini->WriteString("FormEdits", key, ed->Text);
			}

		else if (String(comp->ClassName()) == "TComboBox")
			{
			TComboBox* cb = (TComboBox*) comp;
			String key = fm->Name + "." + cb->Name;
			Ini->WriteInteger("FormCombos", key, cb->ItemIndex);
			}

		else if (String(comp->ClassName()) == "TCheckBox")
			{
			TCheckBox* cx = (TCheckBox*) comp;
			String key = fm->Name + "." + cx->Name;
			Ini->WriteBool("FormChecks", key, cx->Checked);
			}
		}

	//Formularposition speichern
	Ini->WriteInteger("FormPos", fm->Name + ".Left", fm->Left);
	Ini->WriteInteger("FormPos", fm->Name + ".Top",  fm->Top);

	//Formulardimensionen speichern
	Ini->WriteInteger("FormPos", fm->Name + ".Width",  fm->Width);
	Ini->WriteInteger("FormPos", fm->Name + ".Height", fm->Height);

	Ini->UpdateFile();
	delete Ini;
	}
//---------------------------------------------------------------------------
void cTools::ListViewLoad(TForm* fm, TListView* lv)
	{
	if (lv == NULL) return;
	TIniFile* Ini = new TIniFile(GetIniFile());
	String sec = "FormListView." + fm->Name + "." + lv->Name;
	if (!Ini->SectionExists(sec))
		{
		delete Ini;
		return;
		}

	String rowkey; String key;
	TListItem* item;
	int row = 0;
	while (row < 9999)
		{
		rowkey = String(row) + ".";
		if (!Ini->ValueExists(sec, rowkey + "caption"))
			break;

		item = lv->Items->Add();
		item->Data    = (void*)Ini->ReadInteger(sec, rowkey + "data", 0);
		item->Caption = Ini->ReadString(sec, rowkey + "caption", "");

		for (int col = 0; col < (lv->Columns->Count-1); col++)
			{
			key = rowkey + "col" + String(col);
			item->SubItems->Add(Ini->ReadString(sec, key, ""));
			}

		row++;
		}

	Ini->UpdateFile();
	delete Ini;
	}
//---------------------------------------------------------------------------
void cTools::ListViewSave(TForm* fm, TListView* lv)
	{
	if (lv == NULL) return;
	TIniFile* Ini = new TIniFile(GetIniFile());

	String sec = "FormListView." + fm->Name + "." + lv->Name;
	Ini->EraseSection(sec);

	String rowkey; String key;
	for (int row = 0; row < lv->Items->Count; row++)
		{
		TListItem* item = lv->Items->Item[row];
		rowkey = String(row) + ".";

		Ini->WriteInteger(sec, rowkey + "data",    (int)item->Data);
		Ini->WriteString(sec, rowkey + "caption", item->Caption);

		for (int col = 0; col < (lv->Columns->Count-1); col++)
			{
			key = rowkey + "col" + String(col);
			Ini->WriteString(sec, key, item->SubItems->Strings[col]);
			}
		}

	Ini->UpdateFile();
	delete Ini;
	}
//---------------------------------------------------------------------------
/***************************************************************************/
/**************   Bildschirm- und Printmeldungen   *************************/
/***************************************************************************/
//---------------------------------------------------------------------------
void cTools::ErrBox(const String& msg, ...)
	{
	wchar_t buffer[512];
	int     nsiz;
	va_list argptr;

	va_start(argptr, msg);
	nsiz = vsnwprintf(0, 0, msg.c_str(), argptr);
	if (nsiz >= sizeof(buffer)-2) nsiz = sizeof(buffer)-2;

	vsnwprintf(buffer, nsiz, msg.c_str(), argptr);
	buffer[nsiz] = 0;
	va_end(argptr);

	Application->MessageBox(buffer, L"Fehler", MB_OK);
	}
//---------------------------------------------------------------------------
void cTools::MsgBox(const String& msg, ...)
	{
	wchar_t buffer[512];
	int     nsiz;
	va_list argptr;

	va_start(argptr, msg);
	nsiz = vsnwprintf(0, 0, msg.c_str(), argptr);
	if (nsiz >= sizeof(buffer)-2) nsiz = sizeof(buffer)-2;

	vsnwprintf(buffer, nsiz, msg.c_str(), argptr);
	buffer[nsiz] = 0;
	va_end(argptr);

	Application->MessageBox(buffer, L"Meldung", MB_OK);
	}
//---------------------------------------------------------------------------
/***************************************************************************/
/**************   String-Funktionen   **************************************/
/***************************************************************************/
//---------------------------------------------------------------------------
String cTools::fmt(const wchar_t* msg, ...)
	{
	wchar_t buffer[512];
	int     nsiz;
	va_list argptr;

	va_start(argptr, msg);
	nsiz = vsnwprintf(0, 0, msg, argptr);
	if (nsiz >= sizeof(buffer)-2) nsiz = sizeof(buffer)-2;

	vsnwprintf(buffer, nsiz, msg, argptr);
	buffer[nsiz] = 0;
	va_end(argptr);

	return String(buffer);
	}
//---------------------------------------------------------------------------
String cTools::fmt(const char* msg, ...)
	{
	wchar_t buffer[512];
	int     nsiz;
	va_list argptr;

	va_start(argptr, msg);
	String wstr = msg;
	nsiz = vsnwprintf(0, 0, wstr.c_str(), argptr);
	if (nsiz >= sizeof(buffer)-2) nsiz = sizeof(buffer)-2;

	vsnwprintf(buffer, nsiz, wstr.c_str(), argptr);
	buffer[nsiz] = 0;
	va_end(argptr);

	return String(buffer);
	}
//---------------------------------------------------------------------------
int cTools::replace(String& str, String old, String neu)
	{
	if (str == "") return 0;
	int count = 0;

	int pos;
	String left, right;
	while ((pos = str.Pos(old)) > 0)
		{
		left  = str.SubString(0, pos-1);
		right = str.SubString(pos + old.Length(), 999);

		str = left + neu + right;
		count++;

		if (count > 100) //sicherheitsabfrage
			break;
		}

	return count;
	}
//---------------------------------------------------------------------------
iarray_t cTools::TextToArray(String text, String delim)
	{
	iarray_t result; result.clear();
	if (text == "" || delim == "") return result;

	int pos; String ww;
	int ix = 0;
	String feld;
	double wert;
	while ((pos = text.Pos(delim)) > 0)
		{
		feld = text.SubString(0, pos-1);
		text = text.SubString(pos+1, 99999);

		replace(feld, ",", ".");
		result[ix].push_back(ix);
		result[ix].push_back(_wtof(feld.c_str()));
		ix++;

		if (ix >= 4000) break; //Schutz vor Endlosschleifen
		}

	if (text != "")
		{
		replace(text, ",", ".");
		result[ix].push_back(ix);
		result[ix].push_back(_wtof(text.c_str()));
		}

	return result;
	}
//---------------------------------------------------------------------------
String cTools::ArrayToText(iarray_t array, String delim)
	{
	//Erwartet ein Array der Form:
	//		ix: Wert (double)
	String s = "";
	if (array.size() <= 0 || delim == "") return s;

	char feld[64]; double wert;

	for (iarray_itr itr = array.begin(); itr != array.end(); itr++)
		{
		ilist_t& v = itr->second;

		if (s == "")
			s = String(v[1]);
		else
			s += delim + String(v[1]);
		}

	return s;
	}
//---------------------------------------------------------------------------
/***************************************************************************/
/**************   Sonstige Funktionen   ************************************/
/***************************************************************************/
//---------------------------------------------------------------------------
bool cTools::IsDebug()
	{
	TIniFile* Ini = new TIniFile(GetIniFile());
	int found = Ini->ReadInteger("HomeStations", GetComputer(), 0);
	delete Ini;
	return found == 1 ? true : false;
	}
//---------------------------------------------------------------------------
String cTools::GetComputer()
	{
	TCHAR comp[64];
	DWORD nSize = sizeof(comp);
	GetComputerName(comp, &nSize);
	return String(comp);
	}
//---------------------------------------------------------------------------
String cTools::GetComputerBS()
	{
#ifdef LINUX
	return "Linux";

#elif defined(WIN32)
	return "Windows";

#elif defined(WIN64)
	return "Windows";
	fcpData.BSProzessor = "64Bit";
#endif

	return "";
	}
//---------------------------------------------------------------------------
String cTools::GetComputerProzessor()
	{
#ifdef LINUX
	return "Linux";

#elif defined(WIN32)
	return "32Bit";

#elif defined(WIN64)
	return "64Bit"; //TODO funktioniert nicht !!

#endif

	return "";
	}
//---------------------------------------------------------------------------
bool cTools::Log(const char* msg, ...)
	{
	String dat = GetApplicationPath() + L"\\ecg.log";
	FILE* fp = fopen(AnsiString(dat).c_str(), "a+");
	if (fp == NULL) return false;

	wchar_t buffer[512];
	int     nsiz;
	va_list argptr;

	va_start(argptr, msg);
	String wstr = msg;
	nsiz = vsnwprintf(0, 0, wstr.c_str(), argptr);
	if (nsiz >= sizeof(buffer)-2) nsiz = sizeof(buffer)-2;

	vsnwprintf(buffer, nsiz, wstr.c_str(), argptr);
	buffer[nsiz] = 0;
	va_end(argptr);

	fprintf(fp, "%s\n", buffer);
	fclose(fp);
	return true;
	}
//---------------------------------------------------------------------------


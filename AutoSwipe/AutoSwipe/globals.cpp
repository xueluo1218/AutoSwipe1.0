// Globals.cpp
// 一些全局函数
//

#include "stdafx.h"
#include "MyDirFileSearch.h"
#include "globals.h"
#include "MyIniFile.h"
#include "math.h"
#include "winspool.h"
#include <shlwapi.h>

#pragma warning(disable:4996) //_CRT_NON_CONFORMING_SWPRINTFS _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"Shlwapi.lib") //如果没有这行，会出现link错误

// 角度->弧度
double degToRad(double ang)
{
    return ang * (double)pi / (double)180.0;
}

// 弧度->角度
double radToDeg(double ang)
{
    return ang * (double)180.0 / (double)pi;
}

double sround(const double& num)
{
	double n = num;
	int in = (int)n;
	double mn = n - in;
	if(mn < 0.5)
		n = floor(n);
	else
		n = ceil(n);
	return n;
}

//*********************************************************************************
// Function name		- GetFolder
// Description			- Get a folder path
// 泥蜞 祛滂翳赅鲨?	- 25.09.2000
// 叔?祛滂翳鲨痤忄磬	- S. Sokolenko
// In					-
//						  strSelectedFolder - reference to string for store folder path
// Out				-
//						  lpszTitle - title for caption
//						  hwndOwner - reference to parent window 
//						  strRootFolder - root folder 
//						  strStartFolder - current foldet
// Return				- TRUE if user select OK, else FALSE.
//*********************************************************************************
static CString strTmpPath;
//static TCHAR   strTmpPath[MAX_PATH];


int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
/*	TCHAR szDir[MAX_PATH];
	switch (uMsg) {
	case BFFM_INITIALIZED:
		if (lpData) 
		{
			_tcscpy(szDir, strTmpPath.GetBuffer(strTmpPath.GetLength()));
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
		}
		break;
	case BFFM_SELCHANGED: 
		if (SHGetPathFromIDList((LPITEMIDLIST)lParam, szDir))
		{
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
		}

		break;
	default:
		break;
	}
*/
	TCHAR   gszRootDir[MAX_PATH];
	_tcscpy(gszRootDir, strTmpPath);
	if(uMsg == BFFM_INITIALIZED)
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)gszRootDir);
	else if (uMsg == BFFM_SELCHANGED)
	{
		if (SHGetPathFromIDList((LPITEMIDLIST)lParam, gszRootDir))
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)gszRootDir);
	}
	return 0;
}

// 老界面
BOOL GetFolder(CString* strSelectedFolder,
				   const TCHAR* lpszTitle,
				   const HWND hwndOwner, 
				   CString strRootFolder, 
				   CString strStartFolder)
{
	TCHAR pszDisplayName[MAX_PATH];
	LPITEMIDLIST lpID;
	BROWSEINFO bi;
	
	bi.hwndOwner = hwndOwner;
	if (strRootFolder.IsEmpty())
	{
		bi.pidlRoot = NULL;
	}
	else
	{
	   LPITEMIDLIST  pIdl = NULL;
	   IShellFolder* pDesktopFolder;
	   TCHAR         szPath[MAX_PATH];
	   OLECHAR       olePath[MAX_PATH];
	   ULONG         chEaten;
	   ULONG         dwAttributes;

	   //USES_CONVERSION;
	   //strcpy(szPath, (LPCTSTR)strRootFolder);
	   lstrcpyn(szPath, strRootFolder, sizeof(szPath));
	   if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
	   {
		   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, LPCSTR(szPath), -1, olePath, MAX_PATH);
		   pDesktopFolder->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pIdl, &dwAttributes);
		   pDesktopFolder->Release();
	   }
	   bi.pidlRoot = pIdl;
	}
	bi.pszDisplayName = pszDisplayName;
	bi.lpszTitle = lpszTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	bi.lpfn = BrowseCallbackProc;
	if (strStartFolder.IsEmpty())
	{
		bi.lParam = FALSE;
	}
	else
	{
		strTmpPath.Format(_T("%s"), strStartFolder);
		bi.lParam = TRUE;
	}
	bi.iImage = NULL;
	lpID = SHBrowseForFolder(&bi);
	if (lpID != NULL){
		BOOL b = SHGetPathFromIDList(lpID, pszDisplayName);
		if (b == TRUE){
			strSelectedFolder->Format(_T("%s"),pszDisplayName);
			return TRUE;
		}
	}
	else
		strSelectedFolder->Empty();

	return FALSE;
}


// 新界面
#ifndef BIF_USENEWUI
#define BIF_USENEWUI 0x0050
#endif

bool GetFolderX(CString* folderpath, const TCHAR* szCaption , const HWND hOwner )
{
	bool retVal = false;

	// The BROWSEINFO struct tells the shell 
	// how it should display the dialog.
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

//	bi.ulFlags   = BIF_USENEWUI ;	
	bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_STATUSTEXT;
	bi.hwndOwner = hOwner;
	bi.lpszTitle = szCaption;

	// must call this if using BIF_USENEWUI
	::OleInitialize(NULL);

	// Show the dialog and get the itemIDList for the selected folder.
	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if(pIDL != NULL)
	{
		// Create a buffer to store the path, then get the path.
		TCHAR buffer[_MAX_PATH] = {'\0'};
		if(::SHGetPathFromIDList(pIDL, buffer) != 0)
		{
			// Set the string value.
//			folderpath = buffer;
			folderpath->Format(_T("%s"), buffer);
			retVal = true;
		}		

		// free the item id list
		CoTaskMemFree(pIDL);
	}

	::OleUninitialize();

	return retVal;
}




static TCHAR   gszRootDir[MAX_PATH];
int  CALLBACK BrowseCallbackProc1(HWND hwnd, UINT msg, LPARAM lp, LPARAM Data)
{
	if (msg == BFFM_INITIALIZED)
	{
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)gszRootDir);
	}
	return 0;
}
// bIncludeFiles, 缺省=FALSE，不包含文件，只显示目录
BOOL GetFolderX(CString* folderpath, CString strInitPath, const TCHAR* szCaption, const HWND hOwner, BOOL bIncludeFiles)
{
	USES_CONVERSION;
	lstrcpyn(gszRootDir, strInitPath, sizeof(gszRootDir));			// 点开时，树结构的缺省目录
	BROWSEINFO   bi;
	bi.hwndOwner = hOwner;
	bi.pidlRoot = 0;
	bi.pszDisplayName = 0;
	bi.lpszTitle = szCaption;					// 对话框树结构上边的提示字符串
	bi.lpfn = BrowseCallbackProc1;
	bi.lParam = 0;
	//bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	//bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_STATUSTEXT;//  | BIF_USENEWUI;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_STATUSTEXT  | BIF_USENEWUI;
	bi.ulFlags &= ~BIF_EDITBOX;					// 去掉文件夹编辑框
	bi.ulFlags |= BIF_NONEWFOLDERBUTTON;		// 去掉“新建文件夹”按钮
	if(!bIncludeFiles)
		bi.ulFlags &= ~BIF_BROWSEINCLUDEFILES;	// 去掉文件

	LPITEMIDLIST   pidl;
	if (pidl = SHBrowseForFolder(&bi))
	{
		TCHAR buffer[_MAX_PATH] = { '\0' };
		if (::SHGetPathFromIDList(pidl, buffer) != 0)
		{
			// Set the string value.
			folderpath->Format(_T("%s"), buffer);
			return   TRUE;
		}
	}
	return   FALSE;
}


////////////////////////////////////////////////////////////////////////////////////
// 
// GetInstallDir() - THLog安装目录：C:\THLog
//
CString GetInstallDir()
{
	CString strInstallDir, strBinDir;

	strBinDir = GetBinDir();
	int i = strBinDir.ReverseFind('\\');		// 去掉尾部斜杠
	if (i < strBinDir.GetLength() )
		strInstallDir = strBinDir.Left(i);

	return strInstallDir;
}

// 
// GetConfigDir() - 获得Cofig目录，使用相对路径
//
CString GetConfigDir()
{
	CString strPath = GetBinDir();

	// 安装路径c:\log2000
	int i = strPath.ReverseFind('\\');
	CString m_strAppPath = strPath.Left(i);
	
	// Config路径
	CString m_strConfigPath = m_strAppPath + _T("\\Config");
	return m_strConfigPath;
}

// 
// GetFormatDir() - 获得Format录，使用相对路径
//
CString GetFormatDir()
{
	CString strPath = GetBinDir();

	// 安装路径c:\log2000
	int i = strPath.ReverseFind('\\');
	CString m_strAppPath = strPath.Left(i);
	
	// Config路径
	CString m_strFormatPath = m_strAppPath + _T("\\Format");
	return m_strFormatPath;
}

CString GetManualDir()
{
	CString strPath = GetBinDir();

	// 安装路径c:\log2000
	int i = strPath.ReverseFind('\\');
	CString m_strAppPath = strPath.Left(i);

	// Menual路径
	return m_strAppPath + _T("\\Manual");
}

// 
// GetLibDir() - 获得Format录，使用相对路径
//
CString GetLibDir()
{
	CString strPath = GetBinDir();

	// 安装路径c:\log2000
	int i = strPath.ReverseFind('\\');
	CString m_strAppPath = strPath.Left(i);
	
	// Config路径
	CString m_strFormatPath = m_strAppPath + _T("\\Lib");
	return m_strFormatPath;
}

// 数据文件目录\\井名\\井名.prj
CString GetProjectFilename()
{
	return GetDataDir() + _T("\\") + GetWellPath() + _T("\\") + GetWellPath() + _T(".prj");
}

// 
// GetDataDir() - 获得Data目录，使用ini文件设置的路径
//
// 2017/12/03修改：以前从ini文件中读取，当整个目录搬到D/E等其他盘时，返回错误目录
// 2020/12/25修改：区分服务类型，根据服务类型，分别返回不同的文件目录
CString GetDataDir()
{
	CIni m_ini;
#ifdef LOG2000
	m_ini.SetPathName(GetLibDir() + _T("\\Log2000.ini"));
#else
	m_ini.SetPathName(GetConfigDir() + _T("\\Log.ini"));
#endif	
	CString m_strDataPath = m_ini.GetString(_T("General"), _T("DataPath"));
	CString m_strDataPath2 = m_ini.GetString(_T("General"), _T("DataPath2"));
	BOOL bDataPathSame = m_ini.GetBool(_T("General"), _T("DataPathSame"), FALSE);
	BOOL bDefaultServiceType = m_ini.GetBool(_T("General"), _T("LastServiceType"), FALSE);

//	if (m_strDataPath.CompareNoCase(m_strDataPath2) == 0)
	if(bDataPathSame)
		return m_strDataPath;
	else
	{
		if(bDefaultServiceType)
			return m_strDataPath2;
		else
			return m_strDataPath;
	}
}


CString GetTempDir()
{
	CString strPath = GetBinDir();

	// 安装路径c:\log2000
	int i = strPath.ReverseFind('\\');
	CString m_strAppPath = strPath.Left(i);
	
	// Config路径
	CString m_strFormatPath = m_strAppPath + _T("\\Temp");
	return m_strFormatPath;
}

CString GetSimDir()
{
	CString strPath = GetBinDir();

	// 安装路径c:\log2000
	int i = strPath.ReverseFind('\\');
	CString m_strAppPath = strPath.Left(i);
	
	// Config路径
	CString m_strFormatPath = m_strAppPath + _T("\\Sim");
	return m_strFormatPath;
}

// 取得当前可执行文件的路径，结尾不带斜杠
CString GetBinDir()
{
	CString exeFullPath;
	GetModuleFileName((HMODULE)NULL, exeFullPath.GetBuffer(MAX_PATH), MAX_PATH);
	exeFullPath.ReleaseBuffer();

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

#ifdef UNICODE
	errno_t  err = _wsplitpath_s(exeFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname,
		_MAX_FNAME, ext, _MAX_EXT);
#else
	errno_t  err = _splitpath_s(exeFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname,
		_MAX_FNAME, ext, _MAX_EXT);
#endif

	CString strDrive, strDir;
	strDrive.Format(_T("%s"), drive);	

	strDir.Format(_T("%s"), dir);
	int i = strDir.ReverseFind('\\');		// 去掉尾部斜杠
	if(i== strDir.GetLength()-1)
		strDir = strDir.Left(i);

	return strDrive + strDir;
}

CString GetFilenameFromFullpath(CString strFullPath)	// 取完整路径名中的文件名.扩展名
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

#ifdef UNICODE
	errno_t  err = _wsplitpath_s(strFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname,
		_MAX_FNAME, ext, _MAX_EXT);
#else
	errno_t  err = _splitpath_s(strFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname,
		_MAX_FNAME, ext, _MAX_EXT);
#endif

	CString strFileName;
	strFileName.Format(_T("%s%s"), fname, ext);
	return strFileName;
}

CString GetExeName()
{
	// 取得当前路径
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	// 取运行路径c:\LOG2000\BIN\xxx.name
	CString strPath(szPath);
	int i = strPath.ReverseFind('.');
	strPath = strPath.Left(i);
	CString m_strExeName = strPath;
	return m_strExeName;
}

// CString m_strDataPath;		保存在Log.INI中的数据文件目录，如C:\DATA，不包含子目录
// CString m_strProjectPath;	最终找到的子目录
bool FindDataPath(CString strDataPath, CString& strProjectPath)
{
	TCHAR tempDir[MAX_PATH+1];
#ifdef UNICODE
	wcscpy_s(tempDir, strDataPath);	
#else
	strcpy_s(tempDir, strDataPath);	
#endif

	CMyDirFileSearch dr;
	CMyDirFileSearch::SAFileVector::const_iterator fit;
	CMyDirFileSearch::SAFileVector &files = dr.Files();
	
	// look in the temp folder, no recursively
	dr.GetDirs(tempDir, false);
	
	// dump the current directory list
	CMyDirFileSearch::SADirVector &dirs = dr.Dirs();
	
	// clear that list
	dr.ClearFiles();
	
	dr.GetFiles(_T("*.*"), false, true);
	dr.SortFiles(2, true);			// eSortWriteDate
	
	CString str;
	fit = files.begin();			// 第一个目录
	if(fit != files.end())			// 最新的目录
	{
		CString strFile = (*fit).m_sName;
		TRACE(_T("%s\n"), strFile);
		
		strProjectPath = strFile;
		if( ::SetCurrentDirectory( strProjectPath ) == 0)
		{
			str.Format(_T("没能进入数据文件目录%s, 请检查出现的问题!"), strProjectPath);
			strProjectPath = strDataPath;
			return false;
		}
		else
		{
			str.Format(_T("没能找到数据文件目录%s, 请建立该目录!"), strDataPath);
			AfxMessageBox(str);
			strProjectPath = "c:\\";
			return false;
		}
	}
	
	return true;
}
/*
unsigned short GetFileIndex()
{
    // Build file name
    FILE    *FilePtr;
    unsigned short   number;

	if( (FilePtr = fopen("number","r+b")) == NULL )
	{
		FilePtr = fopen("number","wb");
		number	= 0;
		fwrite(&number, sizeof(short), 1, FilePtr);
		fclose( FilePtr );
	}
	else
    {
		fread(&number, sizeof(short),1,FilePtr);
		number ++;
		fseek(FilePtr, 0, SEEK_SET);
		fwrite(&number,sizeof(short),1,FilePtr);
		fclose(FilePtr);
	}

	return number;
}
*/

CString GetLocalDateTime()
{
	const int nBufSize = 256;
    TCHAR chBuf[nBufSize];
	
	//获取当地的时间。
	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal);
	
	//显示时间的间隔
	wsprintf(chBuf,_T("%u/%u/%u %u:%u:%u"),
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
/*	
	wsprintf(chBuf,_T("%u/%u/%u %u:%u:%u:%u %d"),                 
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond,
		stLocal.wMilliseconds,stLocal.wDayOfWeek);
*/
	return chBuf;
}

#include <sys/types.h>
#include <sys/stat.h>

long GetFileSize(CString filename)
{
    CFileStatus stat;
    if( !CFile::GetStatus(filename, stat) )		// 失败返回FALSE
	{
        return -1;
    }
    return (long)stat.m_size;
}


char Char2Hex(char ch)
{
	if((ch>='0')&&(ch<='9'))
		return ch-0x30;
	else if((ch>='A')&&(ch<='F'))
		return ch-'A'+10;
	else if((ch>='a')&&(ch<='f'))
		return ch-'a'+10;
	else return (-1);
}

/////////////////////////////////////////////////////////
//功能：求权
//输入：int base                    进制基数
//      int times                   权级数
//输出：
//返回：unsigned long               当前数据位的权
//////////////////////////////////////////////////////////
unsigned long power(int base, int times)
{
    int i;
    unsigned long rslt = 1;
    for(i=0; i<times; i++)
        rslt *= base;
    return rslt;
}

//////////////////////////////////////////////////////////
//功能：BCD转10进制
//输入：const unsigned char *bcd     待转换的BCD码
//      int length                   BCD码数据长度
//输出：
//返回：unsigned long               当前数据位的权
//思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
//      先求每个字符所表示的十进制值，然后乘以权
//////////////////////////////////////////////////////////
unsigned long  BCD2Dec(const unsigned char *bcd, int length)
{
	int i, tmp;
	unsigned long dec = 0;
	for(i=0; i<length; i++)
	{
        tmp = ((bcd[i]>>4)&0x0F)*10 + (bcd[i]&0x0F);   
        dec += tmp * power(100, length-1-i);          
	}
	return dec;
}

/////////////////////////////////////////////////////////
//功能：十进制转BCD码
//输入：int Dec                      待转换的十进制数据
//      int length                   BCD码数据长度
//输出：unsigned char *Bcd           转换后的BCD码
//返回：0  success
//思路：原理同BCD码转十进制
//////////////////////////////////////////////////////////
int Dec2BCD(int Dec, unsigned char *Bcd, int length)
{
	int i;
	int temp;
	for(i=length-1; i>=0; i--)
	{
		temp = Dec%100;
		Bcd[i] = ((temp/10)<<4) + ((temp%10) & 0x0F);
		Dec /= 100;
	}
	return 0;
}

void DelayMS(DWORD ms)
{
	DWORD dwStart = GetTickCount();
	DWORD dwEnd = dwStart;
	do
	{
		MSG msg;
		GetMessage(&msg,NULL,0,0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		dwEnd = GetTickCount()-dwStart;
	}while(dwEnd < ms);
}

void DelayS(DWORD second)
{
	COleDateTime  start_time = COleDateTime::GetCurrentTime(); 
	COleDateTimeSpan  end_time = COleDateTime::GetCurrentTime() - start_time; 
	while(end_time.GetTotalSeconds() <= second) 
	{  
		MSG  msg;  
		GetMessage(&msg,NULL,0,0);  
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
		end_time = COleDateTime::GetCurrentTime() - start_time; 
	}
}

BYTE Hex2Num(BYTE ch)
{
	return ch/16 * 10 + ch%16;
}

// HEX转BCD   
// bcd_data(<0x255,>0)   
unsigned char BCD2HEX(unsigned int bcd_data)   
{   
    unsigned char temp;   
    temp=((bcd_data>>8)*100)|((bcd_data>>4)*10)|(bcd_data&0x0f);   
    return temp;   
}   
// HEX转BCD   
// hex_data(<0xff,>0)   
unsigned int HEX2BCD(unsigned char hex_data)   
{   
    unsigned int bcd_data;   
    unsigned char temp;   
    temp=hex_data%100;   
    bcd_data=((unsigned int)hex_data)/100<<8;   
    bcd_data=bcd_data|temp/10<<4;   
    bcd_data=bcd_data|temp%10;   
    return bcd_data;   
}  

UCHAR dec2hex(UCHAR dec)
{
	UCHAR hex = 10* (dec / 16)  + dec % 16;
	return hex;
}

CString GetFilePath(CString path_buffer)	// 从完整路径中获得盘符和路径
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

#ifdef UNICODE
	_wsplitpath_s(path_buffer, drive, dir, fname, ext);
#else
	_splitpath_s(path_buffer, drive, dir, fname, ext);
#endif
	CString str;
	str.Format(_T("%s%s"), drive, dir);
	return str;
}

CString GetFileName(CString path_buffer)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
		   
#ifdef UNICODE
	_wsplitpath_s(path_buffer, drive, dir, fname, ext);
#else
	_splitpath_s(path_buffer, drive, dir, fname, ext);
#endif
	return fname;
}

CString GetFileExt(CString path_buffer)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	
#ifdef UNICODE
	_wsplitpath_s(path_buffer, drive, dir, fname, ext);
#else
	_splitpath_s(path_buffer, drive, dir, fname, ext);
#endif
	return ext;
}

CString GetFileNameAndExt(CString path_buffer)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

#ifdef UNICODE
	_wsplitpath_s(path_buffer, drive, dir, fname, ext);
#else
	_splitpath_s(path_buffer, drive, dir, fname, ext);
#endif
	CString str;
	str.Format(_T("%s%s"), fname, ext);
	return str;
}

#include <winbase.h>
#include <winnt.h>
#include <time.h>

void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;
	
	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = (unsigned long)(ll >> 32);
}

void UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst)
{
	FILETIME ft;
	
	UnixTimeToFileTime(t, &ft);
	FileTimeToSystemTime(&ft, pst);
}

// 以下两个函数将系统时间转换成Unit时间
time_t FileTimetoUnixTime(FILETIME ft)
{
	// takes the last modified date
	LARGE_INTEGER date, adjust, result;
	date.HighPart = ft.dwHighDateTime;
	date.LowPart = ft.dwLowDateTime;
	time_t t;
	
	// 100-nanoseconds = milliseconds * 10000
	adjust.QuadPart = 11644473600000 * 10000;
	
	// removes the diff between 1970 and 1601
	date.QuadPart -= adjust.QuadPart;
	
	// converts back from 100-nanoseconds to seconds
	result.QuadPart = date.QuadPart / 10000000;
	
	if (result.QuadPart >= 0)
	{
		t = (time_t) result.LowPart;
		if (result.QuadPart == (unsigned int) t)
			return t;
	}
	
	return (time_t) -1;
}

time_t SystemTimeToUnixTime(LPSYSTEMTIME pst)
{
	FILETIME ft;
	SystemTimeToFileTime(pst, &ft);
	time_t tm =  FileTimetoUnixTime(ft);
	return tm;
}

// 判断字符串是否全部是数字
BOOL IsDigital(LPCTSTR lpszSrc) 
{ 
	CString Src = lpszSrc; 
	return (Src ==  Src.SpanIncluding( _T("0123456789" ) )); 
} 

BOOL IsValue(LPCTSTR lpszSrc) 
{ 
	CString Src = lpszSrc; 
	return (Src ==  Src.SpanIncluding( _T(".-0123456789" ) )); 
} 

void ExitThisProcess()
{
	DWORD dwProcessID = ::GetCurrentProcessId();
	HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
	::TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
}


// GetTmpPath() - 获得Temp目录，使用相对路径
// 如果安装目录下没有Temp目录，则使用系统Temp目录
CString GetTmpPath()
{
	CString strPath = GetBinDir();
	
	// 安装路径
	int i = strPath.ReverseFind('\\');
	CString strAppPath = strPath.Left(i);
	
	// Temp路径
	CString strTempPath = strAppPath + _T("\\Temp");

	if(!PathFileExists(strTempPath))
	{
		DWORD dwResult;
		TCHAR szTempPath[MAX_PATH];
		dwResult = GetTempPath(MAX_PATH, szTempPath);
		if(!SUCCEEDED(dwResult))
		{
//			AfxMessageBox("搜索临时文件路径错误!临时文件置于c:\\目录下");
			return _T("c:\\");
		}
		strTempPath = szTempPath;
	}

	return strTempPath;
}

CString GetWellPath()
{
	CIni iniLog;
#ifdef LOG2000
	iniLog.SetPathName(GetLibDir() + _T("\\Log2000.ini"));
#else
	iniLog.SetPathName(GetConfigDir() + _T("\\Log.ini"));
#endif
	BOOL bDataPathSame = iniLog.GetBool(_T("General"), _T("DataPathSame"), FALSE);
	BOOL bServiceType = iniLog.GetBool(_T("General"), _T("LastServiceType"), FALSE);
	CString strWellname = iniLog.GetString(_T("Acquisition"), _T("WellId"), _T(""));
	CString strWellname2 = iniLog.GetString(_T("Acquisition"), _T("WellId2"), _T(""));
	if(bDataPathSame)					// 如果控制面板设定使用同一目录，则只使用常规目录
		return strWellname;
	else
	{
		if (bServiceType == 0)			// 是常规测井？
			return strWellname;
		else							// 是射孔？
			return strWellname2;
	}
}

//// 2020.12.28 根据服务类型（常规测井、射孔）决定是否当前工作目录
//CString GetWellPath(BOOL g_LastServiceType)
//{
//	CIni iniLog;
//	iniLog.SetPathName(GetConfigDir() + L"\\Log.ini");
//	BOOL bDataPathSame = iniLog.GetBool(L"General", L"DataPathSame", FALSE);
////	BOOL bServiceType = iniLog.GetBool(L"General", L"LastServiceType", FALSE);
//	BOOL bServiceType = g_LastServiceType;
//	CString strWellname = iniLog.GetString(L"Acquisition", L"WellId", L"");
//	CString strWellname2 = iniLog.GetString(L"Acquisition", L"WellId2", L"");
////	if(strWellname.CompareNoCase(strWellname2) == 0)
//	if(bDataPathSame)
//		return strWellname;
//	else
//	{
//		if (bServiceType == 0)
//			return strWellname;
//		else 
//			return strWellname2;
//	}
//}

// 数据文件目录+\\+井名
CString GetCurrentWellDir()
{
	CString strDataDir = GetDataDir();

	CIni iniLog;
#ifdef LOG2000
	iniLog.SetPathName(GetLibDir() + _T("\\Log2000.ini"));
#else
	iniLog.SetPathName(GetConfigDir() + _T("\\Log.ini"));
#endif
	CString strWellname = GetWellPath();
	CString strWellDir = strDataDir + _T("\\") + strWellname;
	return strWellDir;
}

// 测试系统是否为win7
/*
if (isWin7())
{
	MessageBox("当前系统是Win7", "提示", MB_OK | MB_ICONINFORMATION);
}
else
{
	MessageBox("当前系统不是Win7", "提示", MB_OK | MB_ICONINFORMATION);
}

bool isWin7()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
	
	// win7的系统版本为NT6.1
	if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId &&  
		osvi.dwMajorVersion == 6 && 
		osvi.dwMinorVersion == 1 )
	{
		return true;	
	}
	else
	{
		return false;
	}
}
*/
// Date string should be "yyyy-MM-dd hh:mm:ss"
SYSTEMTIME String2SystemTime(const char *dateTimeString)
{
	SYSTEMTIME systime;
	
	ZeroMemory(&systime, sizeof(systime));
	
    sscanf_s(dateTimeString, "%d-%d-%d %d:%d:%d", (int*)&systime.wYear, (int*)&systime.wMonth, (int*)&systime.wDay,
		(int*)&systime.wHour, (int*)&systime.wMinute, (int*)&systime.wSecond);
	
    return systime;
}

// 把CTime格式的时间转换成UnixTime，便于存到数据库
double CTime2UnixTime(CTime time)
{
	SYSTEMTIME st;
	time.GetAsSystemTime(st);
	return (double)SystemTimeToUnixTime(&st);
}



static LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	
    if (retval == ERROR_SUCCESS) {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata,data);
        RegCloseKey(hkey);
    }
	
    return retval;
}

BOOL GotoURL(CString url, CString cmdLine, int showcmd, DWORD* dwProcessId)
{
	BOOL result(FALSE);
    TCHAR key[MAX_PATH + MAX_PATH];

	// 判断文件类型，.exe直接执行，并把进程ID，进程名称保存起来
	if( !GetFileExt(url).CompareNoCase( _T(".EXE") ) ) 
	{
		PROCESS_INFORMATION processInformation = {0};
		STARTUPINFO startupInfo                = {0};
		startupInfo.cb                         = sizeof(startupInfo);
		int nStrBuffer                         = cmdLine.GetLength() + 50;
		
		// 2020/01/05
		LPWSTR szCmdline;
		szCmdline = _tcsdup(url + _T(" ") + cmdLine);

		// Create the process
		result = CreateProcess(NULL, szCmdline,//cmdLine.GetBuffer(nStrBuffer),
				NULL, NULL, FALSE,
			NORMAL_PRIORITY_CLASS,// | CREATE_NO_WINDOW, 
			NULL, NULL, &startupInfo, &processInformation);
		url.ReleaseBuffer();
		cmdLine.ReleaseBuffer();
		free(szCmdline);

		*dwProcessId = processInformation.dwProcessId;
/*		
		if(result)
		{	
			// 加入进程列表
			CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
			stStartedProcess process;
			process.dwId = processInformation.dwProcessId;
			strcpy(process.chName, GetFileName(url));
			theApp.m_arStartedProcess.Add(process);
			
			// 加入Log
		}
*/		
	}
	else 
	{
		// First try ShellExecute()
		HINSTANCE ret = ShellExecute(NULL, _T("open"), url, NULL, NULL, showcmd);
		
		// If it failed, get the .htm regkey and lookup the program
		if ((UINT)ret <= HINSTANCE_ERROR) 
		{
			if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) 
			{
				lstrcat(key, _T("\\shell\\open\\command"));
				
				if (GetRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
				{
					TCHAR *pos;
					pos = _tcsstr(key, _T("\"%1\""));
					if (pos == NULL) {                     // No quotes found
						pos = _tcsstr(key, _T("%1"));      // Check for %1, without quotes 
						if (pos == NULL)                   // No parameter at all...
							pos = key+lstrlen(key)-1;
						else
							*pos = '\0';                   // Remove the parameter
					}
					else
						*pos = '\0';                       // Remove the parameter
					
					lstrcat(pos, _T(" "));
					
					// 要把命令行参数用引号包起来，否则无法处理带空格的文件名
					lstrcat(pos, _T("\""));
					lstrcat(pos, (url));
					lstrcat(pos, _T("\""));
					
					USES_CONVERSION;
#ifdef UNICODE
					ret = (HINSTANCE)WinExec(T2A(key), showcmd);
#else
					ret = (HINSTANCE)WinExec((key), showcmd);
#endif
					if((UINT)ret > HINSTANCE_ERROR)
						result = TRUE;
				}
			}
		}
		else 
			result = TRUE;
	}
	
    return result;
}


int FindPortNumber(CString strName)
{
	int nPort;
	CString strPort;
	
	int nStartPos = strName.Find(_T("(COM"));
	if (nStartPos >= 0)
	{
		int nEndPos = strName.Find(_T(')'), ++nStartPos);
		if (nEndPos > 0)
		{
			strPort = strName.Mid(nStartPos+3, nEndPos - nStartPos);
		}
	}
				
	nPort = _ttoi(strPort);
	return nPort;
}

// 提权函数
BOOL AdjustPrivilege()
{
    BOOL bRet = FALSE;
    TOKEN_PRIVILEGES tp = { 0 };					// 令牌权限结构
    HANDLE hToken = NULL;							// 令牌句柄

    do 
    {
        //打开当前进程令牌,并且获取它				// 令牌权限修改和查询
        if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|
            TOKEN_QUERY,&hToken))
            break;
        // 获取关机注销重启的LUID(Locally Unique Identifier),局部唯一标识
        if (!LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tp.Privileges[0].Luid))
            break;
        tp.PrivilegeCount = 1;//修改权限的个数
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;		// 激活SE_SHUTDOWN_NAME这个权限
        // 提升权限 // FALSE表示可以修改权限// 把需要修改的权限传进来
        if(!AdjustTokenPrivileges(hToken,FALSE,&tp,0,(PTOKEN_PRIVILEGES)NULL,0))
            break;
        bRet = TRUE;
    } while (FALSE);

    if (hToken)
        CloseHandle(hToken);
    
	return bRet;
}

#include <vector>
using namespace std;

// 字符串分割
int SplitString(const CString str, char split, CStringArray &strArray)
{
	strArray.RemoveAll();
	CString strTemp = str;
	int iIndex = 0;
	while (1)
	{
		iIndex = strTemp.Find(split);
		if (iIndex >= 0)
		{
			strArray.Add(strTemp.Left(iIndex));
			strTemp = strTemp.Right(strTemp.GetLength() - iIndex - 1);
		}
		else
		{
			break;
		}
	}
	strArray.Add(strTemp);

	return strArray.GetSize();
}



// CStringT Tokenize(_In_ PCXSTR pszTokens, _Inout_ int& iStart) const
// 功能介绍：从iStart位置取出字符串中含pszTokens分割符间的内容；istart是开始分割的位置，一般设为0，下面是一段运用实例 :
void SplitCString(CString strSource, CString ch, vector <CString> &vecString)
{
	vecString.clear();

	int iPos = 0;
	CString strTmp;

		strTmp = strSource.Tokenize(ch, iPos);
		while (strTmp.Trim() != _T(""))
		{
			vecString.push_back(strTmp);
			strTmp = strSource.Tokenize(ch, iPos);
		}
}


/*
#include <atlbase.h>
#include <atlconv.h>

wchar_t* a2w(const char* sz)
{
	USES_CONVERSION;

	return A2W(sz);
}

char* w2a(const wchar_t* wsz)
{
	USES_CONVERSION;

	return W2A(wsz);
}


// 把TCHAR转为char
// *tchar是TCHAR类型指针，*_char是char类型指针   
void TcharToChar(const TCHAR* tchar, char* _char)
{
	int iLength;
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char   
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}
// 把char转为TCHAR
void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}
*/


BOOL isExistFile(CString strFile)
{
	return PathFileExists(strFile);
}

std::string ConvertCStringToUTF8(CString strValue)
{
	std::wstring wbuffer;
	int length;
#ifdef _UNICODE
	wbuffer.assign(strValue.GetString(), strValue.GetLength());
#else
	/*
	* 转换ANSI到UNICODE
	* 获取转换后长度
	*/
	length = ::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, (LPCTSTR)strValue, -1, NULL, 0);
	wbuffer.resize(length);
	/* 转换 */
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strValue, -1, (LPWSTR)(wbuffer.data()), wbuffer.length());
#endif

	/* 获取转换后长度 */
	length = WideCharToMultiByte(CP_UTF8, 0, wbuffer.data(), wbuffer.size(), NULL, 0, NULL, NULL);
	/* 获取转换后内容 */
	std::string buffer;
	buffer.resize(length);

	WideCharToMultiByte(CP_UTF8, 0, strValue, -1, (LPSTR)(buffer.data()), length, NULL, NULL);
	return(buffer);
}


CString ConvertUTF8ToCString(std::string utf8str)
{
	/* 预转换，得到所需空间的大小 */
	int nLen = ::MultiByteToWideChar(CP_UTF8, NULL,
		utf8str.data(), utf8str.size(), NULL, 0);
	/* 转换为Unicode */
	std::wstring wbuffer;
	wbuffer.resize(nLen);
	::MultiByteToWideChar(CP_UTF8, NULL, utf8str.data(), utf8str.size(),
		(LPWSTR)(wbuffer.data()), wbuffer.length());

#ifdef UNICODE
	return(CString(wbuffer.data(), wbuffer.length()));
#else
	/*
	* 转换为ANSI
	* 得到转换后长度
	*/
	nLen = WideCharToMultiByte(CP_ACP, 0,
		wbuffer.data(), wbuffer.length(), NULL, 0, NULL, NULL);

	std::string ansistr;
	ansistr.resize(nLen);

	/* 把unicode转成ansi */
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)(wbuffer.data()), wbuffer.length(),
		(LPSTR)(ansistr.data()), ansistr.size(), NULL, NULL);
	return(CString(ansistr.data(), ansistr.length()));
#endif
}

#include <ctime>

ULONG GetTickCountClock()
{
	return (ULONG)((LONGLONG)clock() * 1000 / CLOCKS_PER_SEC);
}

// 从m_tree的hSrcItem节点开始，查找字符串为strSrcText的子项，如果相同，则返回到hDstItem，没有则hDstItem=NULL
void FindItemChildString(CTreeCtrl& m_tree, HTREEITEM hSrcItem, CString strSrcText, HTREEITEM& hDstItem)
{
	if (hSrcItem == NULL) return;

	HTREEITEM hChildItem = m_tree.GetChildItem(hSrcItem);
	while (hChildItem != NULL)
	{
		CString str = m_tree.GetItemText(hChildItem);
		if (str.CompareNoCase(strSrcText) == 0)
		{
			hDstItem = hChildItem;
			return;
		}

		//FindItemString(m_tree, hChildItem, strSrcText, hDstItem);
		hChildItem = m_tree.GetNextSiblingItem(hChildItem);
	}
}

// FindMenuItem() will find a menu item string from the specified
// popup menu and returns its position (0-based) in the specified 
// popup menu. It returns -1 if no such menu item string is found.
int FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
	ASSERT(Menu);
	ASSERT(::IsMenu(Menu->GetSafeHmenu()));

	int count = Menu->GetMenuItemCount();
	for (int i = 0; i < count; i++)
	{
		CString str;
		int pos = Menu->GetMenuString(i, str, MF_BYPOSITION);
		if ( pos && (str.Compare(MenuString) == 0) )
			return i;
	}

	return -1;
}

BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode)  
{  
	// if NULL is passed, then assume we are setting app object's  
	// devmode and devnames  
	if (phDevMode == NULL || phDevNames == NULL)  
		return FALSE;  

	// Open printer  
	HANDLE hPrinter;  
	if (OpenPrinter(pszPrinterName, &hPrinter, NULL) == FALSE)  
		return FALSE;  

	// obtain PRINTER_INFO_2 structure and close printer  
	DWORD dwBytesReturned, dwBytesNeeded;  
	GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);  
	PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR,  
		dwBytesNeeded);  
	if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded,  
		&dwBytesReturned) == 0) {  
			GlobalFree(p2);  
			ClosePrinter(hPrinter);  
			return FALSE;  
	}  
	ClosePrinter(hPrinter);  

	// Allocate a global handle for DEVMODE  
	HGLOBAL  hDevMode = GlobalAlloc(GHND, sizeof(*p2->pDevMode) +  
		p2->pDevMode->dmDriverExtra);  
	ASSERT(hDevMode);  
	DEVMODE* pDevMode = (DEVMODE*)GlobalLock(hDevMode);  
	ASSERT(pDevMode);  

	// copy DEVMODE data from PRINTER_INFO_2::pDevMode  
	memcpy(pDevMode, p2->pDevMode, sizeof(*p2->pDevMode) +  
		p2->pDevMode->dmDriverExtra);  
	GlobalUnlock(hDevMode);  

	// Compute size of DEVNAMES structure from PRINTER_INFO_2's data  
	DWORD drvNameLen = lstrlen(p2->pDriverName)+1;  // driver name  
	DWORD ptrNameLen = lstrlen(p2->pPrinterName)+1; // printer name  
	DWORD porNameLen = lstrlen(p2->pPortName)+1;    // port name  

	// Allocate a global handle big enough to hold DEVNAMES.  
	HGLOBAL hDevNames = GlobalAlloc(GHND,  
		sizeof(DEVNAMES) +  
		(drvNameLen + ptrNameLen + porNameLen)*sizeof(TCHAR));  
	ASSERT(hDevNames);  
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(hDevNames);  
	ASSERT(pDevNames);  

	// Copy the DEVNAMES information from PRINTER_INFO_2  
	// tcOffset = TCHAR Offset into structure  
	int tcOffset = sizeof(DEVNAMES)/sizeof(TCHAR);  
	ASSERT(sizeof(DEVNAMES) == tcOffset*sizeof(TCHAR));  

	pDevNames->wDriverOffset = tcOffset;  
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pDriverName,  
		drvNameLen*sizeof(TCHAR));  
	tcOffset += drvNameLen;  

	pDevNames->wDeviceOffset = tcOffset;  
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPrinterName,  
		ptrNameLen*sizeof(TCHAR));  
	tcOffset += ptrNameLen;  

	pDevNames->wOutputOffset = tcOffset;  
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPortName,  
		porNameLen*sizeof(TCHAR));  
	pDevNames->wDefault = 0;  

	GlobalUnlock(hDevNames);  
	GlobalFree(p2);   // free PRINTER_INFO_2  

	// set the new hDevMode and hDevNames  
	*phDevMode = hDevMode;  
	*phDevNames = hDevNames;  
	return TRUE;  
} 

// TCHAR *test[4] = {_T("0xFFFF"), _T("0xabcd"), _T("ffff"), _T("ABCD")};
// _httoi(test[i])
int _httoi(const TCHAR *value)
{
	struct CHexMap
	{
		TCHAR chr;
		int value;
	};
	const int HexMapL = 16;
	CHexMap HexMap[HexMapL] =
	{
		{'0', 0}, {'1', 1},
		{'2', 2}, {'3', 3},
		{'4', 4}, {'5', 5},
		{'6', 6}, {'7', 7},
		{'8', 8}, {'9', 9},
		{'A', 10}, {'B', 11},
		{'C', 12}, {'D', 13},
		{'E', 14}, {'F', 15}
	};
	TCHAR *mstr = _tcsupr(_tcsdup(value));
	TCHAR *s = mstr;
	int result = 0;
	if (*s == '0' && *(s + 1) == 'X') s += 2;
	bool firsttime = true;
	while (*s != '\0')
	{
		bool found = false;
		for (int i = 0; i < HexMapL; i++)
		{
			if (*s == HexMap[i].chr)
			{
				if (!firsttime) result <<= 4;
				result |= HexMap[i].value;
				found = true;
				break;
			}
		}
		if (!found) break;
		s++;
		firsttime = false;
	}
	free(mstr);
	return result;
}

// Generate random numbers in the half-closed interval
// [range_min, range_max]. In other words,
// range_min <= random number < range_max
// sample: GenerateRangedRand(-5, 5);
float GenerateRandNoise(int range_min, int range_max)
{
	float f = (float)((double)rand() / (RAND_MAX + 1) * (range_max - range_min)	+ range_min);
	return f;
}

// 根据线型style得到该线型对应的数据
void GetLineStylePattern(CDC *pDC, DWORD* pattern, int *n, int style, int width)
{
	//	ASSERT((style>DL_SOLID) || (style <= DL_2DASH2DOT));
	if(style > DL_2DASH2DOT) style = DL_SOLID;

	switch (style)
	{
	case DL_SOLID:
		*n = 0;
		pattern[0] = 0;
		break;
	case DL_LONGDASH:
		*n = 2;
		pattern[0] = 50;
		pattern[1] = 20;
		break;
	case DL_SHORTDASH:
		*n = 2;
		pattern[0] = 30;
		pattern[1] = 20;
		break;
	case DL_LONGDOT:
		*n = 2;
		pattern[0] = 5;
		pattern[1] = 20;
		break;
	case DL_SHORTDOT:
		*n = 2;
		pattern[0] = 5;
		pattern[1] = 10;
		break;
	case DL_DASH1DOT:
		*n = 4;
		pattern[0] = 50;
		pattern[1] = 20;
		pattern[2] = 5;
		pattern[3] = 20;
		break;
	case DL_DASH2DOT:
		*n = 6;
		pattern[0] = 50;
		pattern[1] = 20;
		pattern[2] = 5;
		pattern[3] = 20;
		pattern[4] = 5;
		pattern[5] = 20;
		break;
	case DL_DASH3DOT:
		*n = 8;
		pattern[0] = 50;
		pattern[1] = 20;
		pattern[2] = 5;
		pattern[3] = 20;
		pattern[4] = 5;
		pattern[5] = 20;
		pattern[6] = 5;
		pattern[7] = 20;
		break;
	case DL_2DASH2DOT:
		*n = 8;
		pattern[0] = 50;
		pattern[1] = 20;
		pattern[2] = 50;
		pattern[3] = 20;
		pattern[4] = 5;
		pattern[5] = 20;
		pattern[6] = 5;
		pattern[7] = 20;
		break;
	}	

	// 线宽不等于1时, 线两端变成, 挤压了空隙长度, 应该把空隙长度增加
	for(int i=0; i<*n; i+=2)
	{
		pattern[i  ] = DWORD(pattern[i  ] + width*1.2);
		pattern[i+1] = DWORD(pattern[i+1] + width*1.8);
	}

	if(!pDC->IsPrinting())
	{
		for(int i=0; i<*n; i++)
			pattern[i] = (int)( pattern[i] * pDC->GetDeviceCaps(LOGPIXELSX) / 254.);
	}			
}	

// 二进制补码转换成十进制浮点数，用于双极性模拟输入的ADC，当前用的是ADS7805，正负10V量程
// 
float BinaryTwosComplement2Dec(short v_short)
{
	float v_f;

	if( v_short >= 0 )
		//		v_f = (float)v_short * (10.F - 0.305F * 0.001F) / 32767.F;
		v_f = 10.F * (float)v_short / 32768.F;
	else 
	{ 
		if( v_short == short(0x8000) )		// 负数最大值，单独处理。不能直接用0x8000
		{
			v_f = -10.F;
		}
		else
		{
			v_short = ~v_short + 1;
			v_f = -10.F * v_short / 32768.F;
		}
	}

	// 约束数据范围
	if(v_f<-10.0F)v_f = -10.0F;
	if(v_f> 10.0F)v_f =  10.0F;
	return v_f;
}

// 二进制补码转换成十进制电压值，单位：V。用于双极性模拟输入的ADC，当前用的是LTC1412，12位，正负2.5V量程
// 补码变十进制数：最高位=1，则为负数。先把补码取反，然后+1，
float BinaryTwosComplement2DecV_Bipolar_12bit_2dot5v(short v_short_adc)
{
	short v_short = v_short_adc & 0xFFF;	// 12bit adc

	float sign(1);
	if( (v_short & 0x800) != 0 )			// 最高位=1，则为负数
	{ 
		sign = -1;
		v_short = (~v_short & 0xFFF) + 1;	// 取反，+1
	}
	float vlot_v = sign * 2.5F * (float)v_short / 2048.F;

	return vlot_v;
}


void swap_int(int *p1,int *p2)
{
	int temp;
	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

void swap_float(float *p1, float *p2)
{
	float temp;
	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}



using namespace  std;
template<typename Type>
void template_swap(Type &x, Type &y)
{
	Type temp = x;
	x = y;
	y = temp;
}

//int x=2,int y=5;
//template_swap(x,y);
//
//float a=2.3,b=34.5;
//template_swap(x,y);

// 把接箍深度、套管长度、挂挡的上提量等数值标记在图上
// 要求：该浮点数是带小数点后三位的浮点数（mm），取两位（cm），不进位，不要四舍五入
// 例如，0.12X --> 0.12; 不管第三位X是0~9的任意数字
// 4374.86 = 4374.8598，截取后变成4374.85
float mm2cm_float(double x)
{
	// 如果小数点后第四位=9，则先进位
	{
		int a=int(x*10000)/1000;
		int b=int(x*10000-a*1000)/100;
		int c=int(x*10000-a*1000-b*100)/10;
		int d=int(x*10000-a*1000-b*100-c*10);
		if(d==c)
			x += 0.0001F;
		//if(c==9)
		//	x += 0.001F;
		//else if(d==9)
		//	x += 0.001F;
	}
	return (int(x * 100))/100.0F;
}

CString mm2cm_string(float x)
{
	float y = mm2cm_float(x);
	CString str;
	str.Format(_T("%.2f"), y);
	return str;
}

// 从m_tree的hSrcItem节点开始，查找字符串strSrcText，如果相同，则返回到hDstItem，没有则hDstItem=NULL
//	HTREEITEM hSrcItem = m_tree.GetRootItem(), hDstItem = NULL;
//	CString strSrcText = _T("111111");
//	FindItemString(m_tree, hSrcItem, strSrcText, hDstItem);
void FindItemString(CTreeCtrl& m_tree, HTREEITEM hSrcItem, CString strSrcText, HTREEITEM& hDstItem)
{
	if(hSrcItem == NULL)return;
	CString str = m_tree.GetItemText(hSrcItem);	
	if(str.CompareNoCase(strSrcText) == 0)
	{
		hDstItem = hSrcItem;
		return;
	}

	// 获取子项
	HTREEITEM hChildItem = m_tree.GetChildItem(hSrcItem); 
	if(hChildItem)  
		FindItemString(m_tree, hChildItem, strSrcText, hDstItem);

	// 获取兄项
	HTREEITEM hNextItem = m_tree.GetNextItem(hSrcItem,TVGN_NEXT); 
	if(hNextItem)  
		FindItemString(m_tree, hNextItem, strSrcText, hDstItem);
}

// 从m_tree的hSrcItem节点开始，查找被选中的节点，有则返回节点到hDstItem，没有则hDstItem=NULL
//	// 去掉已经选中的节点
//	HTREEITEM hSrcItem = m_tree.GetRootItem(), hDstItem = NULL;
//	FindItemIsSelected(m_tree, hSrcItem, hDstItem);
//	if(hDstItem)
//		m_tree.SetItemState(hDstItem, 0, TVIS_SELECTED);
void FindItemIsSelected(CTreeCtrl& m_tree, HTREEITEM hSrcItem, HTREEITEM& hDstItem)
{
	if(hSrcItem == NULL)return;
	UINT state = m_tree.GetItemState(hSrcItem, TVIS_SELECTED);	
	if((state & TVIS_SELECTED) == TVIS_SELECTED)
	{
		hDstItem = hSrcItem;
		return;
	}

	// 获取子项
	HTREEITEM hChildItem = m_tree.GetChildItem(hSrcItem); 
	if(hChildItem)  
		FindItemIsSelected(m_tree, hChildItem, hDstItem);

	// 获取兄项
	HTREEITEM hNextItem = m_tree.GetNextItem(hSrcItem,TVGN_NEXT); 
	if(hNextItem)  
		FindItemIsSelected(m_tree, hNextItem, hDstItem);
}

// 由像素密度(dp)计算字体大小
int CalcFontHeight(int dp)	
{
	// 字体不需要匹配分辨率，它们需要匹配像素密度。
	// 像素密度以像素每英寸（PPI）或像素/厘米来度量。还有一个称为密度无关像素（DP）的度量单位。定义1dp为一个像素在160 PPI屏幕上的大小。
	// 获取屏幕DPI
	HDC hdc = ::GetDC(NULL);
	int dpi_x = GetDeviceCaps(hdc, LOGPIXELSX);		// 水平方向每逻辑英寸多少个像素点
	int dpi_y = GetDeviceCaps(hdc, LOGPIXELSY);		// 垂直方向每逻辑英寸多少个像素点
	int pt = int(dp * dpi_x / 160.0F);

	return pt;
} 

// 把校正深度中的误差位去掉，做0.25归一化
// 输入：nDepth：mm深度，wc：万米误差，暂未用
long FormatCorrectDepth(long nDepth, int wc)
{
	return (nDepth+2);				// 初测+2是可行的
	//return nDepth;
}	


// 把校正深度中的误差位去掉，做0.25归一化
// 输入：nDepth：m深度，wc：万米误差，暂未用
float FormatCorrectDepth(float fDepth, int wc)
{
	//return float(long(fDepth*1000 + 2) / 25 * 25)/1000.0f;
	return fDepth;
}

// 运行外部程序
void Exec(CString exefile, CString parafile)
{
	SHELLEXECUTEINFO Info;
	ZeroMemory(&Info, sizeof(Info));
	Info.cbSize = sizeof(Info);
	Info.lpVerb = _T("open");
	Info.lpFile = exefile;
	Info.lpParameters = parafile;
	Info.fMask = SEE_MASK_NOCLOSEPROCESS;
	Info.nShow = SW_SHOWDEFAULT;

	if (!ShellExecuteEx(&Info))
	{
		CString str;
		str.Format(_T("函数ShellExecuteEx执行错误，错误代码:%d"), GetLastError());
		AfxMessageBox(str);
		return;
	}
}

// 运行外部程序，等待其运行结束
BOOL Exec_Wait(CString exefile, CString parafile)
{
	SHELLEXECUTEINFO Info;
	ZeroMemory(&Info, sizeof(Info));
	Info.cbSize = sizeof(Info);
	Info.lpVerb = _T("open");
	Info.lpFile = exefile;
	Info.lpParameters = parafile;
	Info.fMask = SEE_MASK_NOCLOSEPROCESS;
	Info.nShow = SW_SHOW;//DEFAULT;

	if (!ShellExecuteEx(&Info))
	{
		CString str;
		str.Format(_T("函数ShellExecuteEx执行错误，错误代码:%d"), GetLastError());
		AfxMessageBox(str);
		return FALSE;
	}
	else
	{
		if(Info.hProcess)		// 指定SEE_MASK_NOCLOSEPROCESS，并且成功执行，则会返回进程句柄
		{
			//WaitForSingleObject(Info.hProcess, INFINITE);

			MSG msg;
			while (::WaitForSingleObject(Info.hProcess, INFINITE) == WAIT_TIMEOUT)
			{
				// get and dispatch message
				if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}

		}

		return TRUE;
	}
}

// 如果运行外部程序出错，显示错误信息
void ReportError(int nError)
{
	CString str;
	switch (nError) {
	case 0:                       str = _T("The operating system is out\nof memory or resources."); break;
	case SE_ERR_PNF:              str = _T("The specified path was not found."); break;
	case SE_ERR_FNF:              str = _T("The specified file was not found."); break;
	case ERROR_BAD_FORMAT:        str = "The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image)."; break;
	case SE_ERR_ACCESSDENIED:     str = "The operating system denied\naccess to the specified file."; break;
	case SE_ERR_ASSOCINCOMPLETE:  str = "The filename association is\nincomplete or invalid."; break;
	case SE_ERR_DDEBUSY:          str = "The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed."; break;
	case SE_ERR_DDEFAIL:          str = "The DDE transaction failed."; break;
	case SE_ERR_DDETIMEOUT:       str = "The DDE transaction could not\nbe completed because the request timed out."; break;
	case SE_ERR_DLLNOTFOUND:      str = "The specified dynamic-link library was not found."; break;
	case SE_ERR_NOASSOC:          str = "There is no application associated\nwith the given filename extension."; break;
	case SE_ERR_OOM:              str = "There was not enough memory to complete the operation."; break;
	case SE_ERR_SHARE:            str = "A sharing violation occurred. ";
	default:                      str.Format(_T("Unknown Error (%d) occurred."), nError); break;
	}
	str = _T("Unable to open hyperlink:\n\n") + str;
	AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
}

// 把绝对路径的文件strSrcFile转换成以strFormPath为起点路径的相对路径
// strFormPath：当前路径，比如：c:\THLog\Bin32
// strSrcFile：绝对路径的文件
CString GetRelativePath(CString strFormPath, CString strSrcFile)
{
	// Get the relative path from the current path to the file path.
	TCHAR szOut[MAX_PATH] = _T("");
	PathRelativePathTo(szOut, strFormPath, FILE_ATTRIBUTE_DIRECTORY, strSrcFile, FILE_ATTRIBUTE_NORMAL);
	return szOut;
}

// 把相对路径的目录文件转换成绝对路径
// strFormPath：当前路径，比如：c:\THLog\Bin32
// strRelativePath：相对路径的文件
CString GetAbsolutePath(CString strFormPath, CString strRelativePath)
{
	TCHAR full[_MAX_PATH] = {0};
	if(_wfullpath(full, strFormPath + _T("\\") + strRelativePath, _MAX_PATH) != NULL )
		return full;
	else
		return _T("");
}
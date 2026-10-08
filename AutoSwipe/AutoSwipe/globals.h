#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <vector>
using namespace std;

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#ifndef pi
#define pi (double)3.1415926535897932384626433832795
#endif

#ifndef INFINITY
#define INFINITY (double)999999.999f
#endif

#ifndef EPSILON
#define EPSILON (double)0.000001f
#endif

#ifndef MAX
#define MAX(X, Y) (X) > (Y) ? (X) : (Y)
#endif				

// 角度->弧度
double degToRad(double ang);

// 弧度->角度
double radToDeg(double ang);

// 随机数
double sround(const double& );

CString GetInstallDir();		// THLog安装目录：C:\THLog
CString GetBinDir();			// \bin目录
CString GetFormatDir();			// \format目录
CString GetConfigDir();			// \config目录
CString GetManualDir();			// \munual目录
CString GetTempDir();
CString GetLibDir();			// \Lib目录

CString GetDataDir();			// 从log.ini中读取数据文件保存的目录，如c:\data
//CString GetDataDir(BOOL g_LastServiceType);			// 从log.ini中读取数据文件保存的目录，如c:\data

CString GetWellPath();			// 用作路径名的井名，根据服务类型返回井名。
//CString GetWellPath(BOOL g_LastServiceType);			// 用作路径名的井名，参数=0-常规测井，1-射孔 
CString GetSimDir();
CString GetTmpPath();			// \temp目录，如果不存在，则获得系统temp目录

CString GetCurrentWellDir();	// 数据文件目录\\井名

CString GetProjectFilename();	// 数据文件目录\\井名\\井名.prj

CString GetFilenameFromFullpath(CString strFullPath);	// 取完整路径名中的文件名.扩展名
CString GetExeName();

// 自m_strDataPath目录中查找最新建立的目录，作为新井目录，并进入该目录
bool FindDataPath(CString m_strDataPath, CString& m_strProjectPath);
//unsigned short GetFileIndex();

// 使用老界面选择目录
BOOL GetFolder(CString* strSelectedFolder, const TCHAR* lpszTitle, const HWND hwndOwner, 
				   CString strRootFolder, CString strStartFolder);

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

// 使用新界面选择目录
BOOL GetFolderX(CString* folderpath, CString strInitParh, const TCHAR* szCaption = NULL, const HWND hOwner = NULL, BOOL bIncludeFiles = FALSE);

CString GetLocalDateTime();

long GetFileSize(CString filename);			// 获取文件大小
CString GetFilePath(CString path_buffer);	// 从完整路径中获得盘符和路径
CString GetFileName(CString path_buffer);	// 从完整路径中获得文件名
CString GetFileExt(CString path_buffer);	// 从完整路径中获得文件扩展名
CString GetFileNameAndExt(CString path_buffer);

BOOL isExistFile(CString strFile);			// 判断文件是否存在，存在则返回TRUE，否则返回FALSE
/*
CString GetRunNumber();
CString GetWellId();
CString GetDataPath();
CString GetService();
CString GetTelemetryFormat();
CString GetUnit();
*/

// 启动.exe可执行文件
BOOL GotoURL(CString url, CString cmdLine, int showcmd, DWORD* dwProcessId);

// 这是一个将字符转换为相应的十六进制值的函数
// 若是在0-F之间的字符，则转换为相应的十六进制字符，否则返回-1
char Char2Hex(char ch); 

// 十进制转BCD码
int Dec2BCD(int Dec, unsigned char *Bcd, int length);

// BCD转10进制
unsigned long  BCD2Dec(const unsigned char *bcd, int length);

BYTE Hex2Num(BYTE ch);

// 延时函数，不太精确，期间可以响应消息
void DelayMS(DWORD ms);
void DelayS(DWORD second);

// HEX转BCD   
// bcd_data(<0x255,>0)   
unsigned char BCD2HEX(unsigned int bcd_data);   

// HEX转BCD   
// hex_data(<0xff,>0)   
unsigned int HEX2BCD(unsigned char hex_data);   

UCHAR dec2hex(UCHAR hex);

void UnixTimeToFileTime(time_t t, LPFILETIME pft);
void UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst);
time_t FileTimetoUnixTime(FILETIME ft);
time_t SystemTimeToUnixTime(LPSYSTEMTIME pst);

SYSTEMTIME String2SystemTime(const char *dateTimeString);		// "yyyy-MM-dd hh:mm:ss" -> SYSTEMTIME

// 把CTime格式的时间转换成UnixTime，便于存到数据库
double CTime2UnixTime(CTime time);

BOOL IsDigital(LPCTSTR lpszSrc);		// 判断字符串是否全部是数字
BOOL IsValue(LPCTSTR lpszSrc);			// 判断字符串是否是数据

void ExitThisProcess();					// 退出当前应用程序

bool isWin7();							// 返回true，系统为win7；否则不是win7

int FindPortNumber(CString strName);

// 提权函数
BOOL AdjustPrivilege();

// 字符串分割
// CStringT Tokenize(_In_ PCXSTR pszTokens, _Inout_ int& iStart) const
// 功能介绍：从iStart位置取出字符串中含pszTokens分割符间的内容；istart是开始分割的位置，一般设为0，下面是一段运用实例 :
void SplitCString(CString strSource, CString ch, vector <CString> &vecString);

// 字符串分割，字符分割
int SplitString(const CString str, char split, CStringArray &strArray);

// CString <==> utf8
CString ConvertUTF8ToCString(std::string utf8str);
std::string ConvertCStringToUTF8(CString strValue);

ULONG GetTickCountClock();

int FindMenuItem(CMenu* Menu, LPCTSTR MenuString);

BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);

// 十六进制转换为整数
int _httoi(const TCHAR *value);

// 半闭区间内产生随机数噪声
float GenerateRandNoise(int range_min, int range_max);

enum {	DL_SOLID, DL_LONGDASH, DL_SHORTDASH, DL_LONGDOT, DL_SHORTDOT, 
	DL_DASH1DOT, DL_DASH2DOT, DL_DASH3DOT, DL_2DASH2DOT}; 

// 根据线型style得到该线型对应的数据
void GetLineStylePattern(CDC *pDC, DWORD* pattern, int *n, int style, int width);

// 二进制补码转换成十进制浮点数，用于双极性模拟输入的ADC，当前用的是ADS7805，正负10V量程
float BinaryTwosComplement2Dec(short v_short);

// 二进制补码转换成十进制浮点数，用于双极性模拟输入的ADC，当前用的是LTC1412，12位，正负2.5V量程
float BinaryTwosComplement2DecV_Bipolar_12bit_2dot5v(short v_short);

void swap_int(int *p1,int *p2);
void swap_float(float *p1, float *p2);

using namespace  std;
template<typename Type>
void template_swap(Type &x, Type &y);

// 把接箍深度、套管长度、挂挡的上提量等数值标记在图上
// 要求：该浮点数是带小数点后三位的浮点数（mm），取两位（cm），不进位，不要四舍五入
// 例如，0.12X --> 0.12; 不管第三位X是0~9的任意数字
float mm2cm_float(double x);
CString mm2cm_string(float x);

// 从m_tree的hSrcItem节点开始，查找字符串为strSrcText的子项，如果相同，则返回到hDstItem，没有则hDstItem=NULL
void FindItemChildString(CTreeCtrl& m_tree, HTREEITEM hSrcItem, CString strSrcText, HTREEITEM& hDstItem);

// 从m_tree的hSrcItem节点开始，查找字符串strSrcText，如果相同，则返回到hDstItem，没有则hDstItem=NULL
void FindItemString(CTreeCtrl& m_tree, HTREEITEM hSrcItem, CString strSrcText, HTREEITEM& hDstItem);

// 从m_tree的hSrcItem节点开始，查找被选中的节点，有则返回节点到hDstItem，没有则hDstItem=NULL
void FindItemIsSelected(CTreeCtrl& m_tree, HTREEITEM hSrcItem, HTREEITEM& hDstItem);

// 由像素密度(dp)计算字体大小，返回对应的pt值
int CalcFontHeight(int dp);

float FormatCorrectDepth(float fDepth, int wc=0);
long FormatCorrectDepth(long nDepth, int wc=0);

// 运行外部程序
void Exec(CString exefile, CString parafile);

// 运行外部程序，等待其运行结束
BOOL Exec_Wait(CString exefile, CString parafile);

// 如果运行外部程序出错，显示错误信息
void ReportError(int nError);

// 把绝对路径的文件strSrcFile转换成以strFormPath为起点路径的相对路径
// strFormPath：当前路径，比如：c:\THLog\Bin32
// strSrcFile：绝对路径的文件
CString GetRelativePath(CString strFormPath, CString strSrcFile);

// 把相对路径的目录文件转换成绝对路径
// strFormPath：当前路径，比如：c:\THLog\Bin32
// strRelativePath：相对路径的文件
CString GetAbsolutePath(CString strFormPath, CString strRelativePath);

#endif




/*1.改变当前目录到EXE所在的目录
在VC++开发环境中直接运行程序，当前目录不是EXE所在的目录，
这样会造成一些麻烦，比如使用相对路径打开文件。使用以下代码将当前目录设成EXE所在的目录：*/
void ChangeCurDirToExe()  
{  
    CString strPath;  
        
    ::GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH);  
    strPath.ReleaseBuffer();  
    strPath = strPath.Left(strPath.ReverseFind(L'\\') +1 );  
    ::SetCurrentDirectory(strPath.GetBuffer());  
        
} 
   
/*2.开机自运行*/
HKEY m_regkey;  
TCHAR filename[_MAX_PATH];  
GetModuleFileName(NULL,filename,_MAX_PATH);  
RegOpenKey(HKEY_LOCAL_MACHINE,L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&m_regkey);  
RegSetValueEx(m_regkey,L"程序名称",0,REG_SZ,(const unsigned char *)filename,MAX_PATH);  
RegCloseKey(m_regkey); 
  
/*3.取消开机自运行*/
HKEY hKey;  
if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)  
{  
    RegDeleteValue(hKey,L"程序名称");   
    RegCloseKey(hKey);  
}  
  
/*4.退出后重启*/
TCHAR szPath[MAX_PATH];   
GetModuleFileName(NULL, szPath, MAX_PATH);   
    
STARTUPINFO startupInfo;  
PROCESS_INFORMATION procInfo;  
memset(&startupInfo,0x00,sizeof(STARTUPINFO));  
startupInfo.cb = sizeof(STARTUPINFO);  
::CreateProcess(szPath,NULL,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&startupInfo,&procInfo); 
  
/*5.只运行一个实例*/
HANDLE m_hMutex = CreateMutex(NULL, FALSE, _T("AetasServer"));  
    
if (GetLastError() == ERROR_ALREADY_EXISTS)  
{  
    AfxMessageBox(L"程序名称,已经运行");  
    CloseHandle(m_hMutex);  
    m_hMutex = NULL;  
    return FALSE;  
} 
   
/*6.链接到库文件*/
#pragma comment(lib, "winspool.lib") 
  
/*7.在窗口程序中输出信息到控制台*/
#include <io.h>  
#include <fcntl.h>  
    
void InitConsole()  
{  
    int nRet= 0;  
    FILE* fp;  
    AllocConsole();  
    nRet= _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);  
    fp = _fdopen(nRet, "w");  
    *stdout = *fp;  
    setvbuf(stdout, NULL, _IONBF, 0);  
}  
//程序退出时调用
FreeConsole(VOID);


/*8. ansi和unicode互转*/
#include <locale.h>
#include <stdlib.h>

setlocale(LC_ALL,"");             //要先设置语言环境
 
//Unicode  to Anst
WCHAR wszStr = "Unicode String";
int nLength = wcstombs(NULL,(WCHAR*)lpBuffer,0) + 1;       //得到要转换字符的个数
char* strBuffer = (char*)LocalAlloc(LPTR, nLength);
 wcstombs(strBuffer,(WCHAR*)lpBuffer,nLength);
 
//Anst to Unicode
char szStr = "Ansi String";
int nLength = mbstowcs(NULL,(char*)lpBuffer,0) + 1;    //得到要转换字符的个数
WCHAR* wcsBuffer = (WCHAR*)LocalAlloc(LPTR, nLength * sizeof(WCHAR));
mbstowcs(wcsBuffer,(char*)lpBuffer,nLength);



/*9. 精确计算时间差，可以精确到微秒*/
LARGE_INTEGER m_nFreq;
LARGE_INTEGER m_nBeginTime;
LARGE_INTEGER nEndTime;
QueryPerformanceFrequency(&m_nFreq); // 获取时钟周期
QueryPerformanceCounter(&m_nBeginTime); // 获取时钟计数
//Sleep(2);
QueryPerformanceCounter(&nEndTime);
int timeUsed = (nEndTime.QuadPart-m_nBeginTime.QuadPart)*1000/m_nFreq.QuadPart;


/*10. 动态加载DLL*/
// File:  RUNTIME.C
// A simple program that uses LoadLibrary and 
// GetProcAddress to access myPuts from MYPUTS.DLL. 
#include <stdio.h> 
#include <windows.h> 
typedef VOID (*MYPROC)(LPTSTR); 
VOID main(VOID) 
{ 
    HINSTANCE hinstLib; 
    MYPROC ProcAdd; 
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 
    // Get a handle to the DLL module.
    hinstLib = LoadLibrary("myputs"); 
    // If the handle is valid, try to get the function address.
    if (hinstLib != NULL) 
    { 
        ProcAdd = (MYPROC) GetProcAddress(hinstLib, "myPuts"); 
        // If the function address is valid, call the function.
        if (fRunTimeLinkSuccess = (ProcAdd != NULL)) 
            (ProcAdd) ("message via DLL function/n"); 
        // Free the DLL module.
        fFreeResult = FreeLibrary(hinstLib); 
    } 
    // If unable to call the DLL function, use an alternative.
    if (! fRunTimeLinkSuccess) 
        printf("message via alternative method/n"); 
} 


/*11. 通过注册表寻找应用程序路径*/
// 打开键
    HKEY hKEY;
    LPCTSTR Rgspath = "WinRAR//shell//open//command";
    LONG ret = RegOpenKeyEx(HKEY_CLASSES_ROOT, Rgspath, 0, KEY_READ, &hKEY);
    
    if(ret != ERROR_SUCCESS)
    { 
        RegCloseKey(hKEY);
        return FALSE;
    }
    // 读取键值内容
    DWORD dwInfoSize;
    DWORD type = REG_SZ;
    BYTE UserInfo[255];
    // zjc 08.04.24 added begin
    /*
    注意RegQueryValueEx最后一个参数是个双向参数，入参时表示的是前一个参数的缓冲区大小，出参时表示的是返回的大小。
    所以最好把UserInfo的大小给dwInfoSize,防止UserInfo溢出。
    */
    dwInfoSize = sizeof(UserInfo)/sizeof(BYTE); 
    
    ret = RegQueryValueEx(hKEY, NULL, NULL, &type, UserInfo, &dwInfoSize);
    if(ret!=ERROR_SUCCESS)
    { 
        LPVOID lpMsgBuf;
        DWORD dw = GetLastError(); 
        
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );  
        MessageBox((LPCTSTR)lpMsgBuf, "系统错误", MB_OK|MB_ICONSTOP); 
        LocalFree(lpMsgBuf);    
        RegCloseKey(hKEY);
        return FALSE;
    }
    
    CString csRarAppPath;
    csRarAppPath.Format("%s",UserInfo);
    int nCount = csRarAppPath.ReverseFind('//');
    csRarAppPath = csRarAppPath.Mid(0,nCount);
    // 关闭键
    RegCloseKey(hKEY);


/*12. 输出调用出错信息*/
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 
        
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );  
    MessageBox((LPCTSTR)lpMsgBuf, "系统错误", MB_OK|MB_ICONSTOP); 
    LocalFree(lpMsgBuf);    


/*13. limits.h*/
#define SCHAR_MIN   (-128)      /* minimum signed char value */  
#define SCHAR_MAX     127       /* maximum signed char value */  
#define CHAR_MIN    SCHAR_MIN   /* mimimum char value */  
#define CHAR_MAX    SCHAR_MAX   /* maximum char value */  
#define SHRT_MIN    (-32768)        /* minimum (signed) short value */  
#define SHRT_MAX      32767         /* maximum (signed) short value */  
#define USHRT_MAX     0xffff        /* maximum unsigned short value */  
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */  
#define INT_MAX       2147483647    /* maximum (signed) int value */  
#define UINT_MAX      0xffffffff    /* maximum unsigned int value */  
#define LONG_MIN    (-2147483647L - 1) /* minimum (signed) long value */  
#define LONG_MAX      2147483647L   /* maximum (signed) long value */  
#define ULONG_MAX     0xffffffffUL  /* maximum unsigned long value */  
#define LLONG_MAX     9223372036854775807i64       /* maximum signed long long int value */  
#define LLONG_MIN   (-9223372036854775807i64 - 1)  /* minimum signed long long int value */  
#define ULLONG_MAX    0xffffffffffffffffui64       /* maximum unsigned long long int value */  


/*14. 计时*/ 
#if !defined(_TIME_COUNTER_H) 
#define _TIME_COUNTER_H 
 
#pragma warning(push)
#pragma warning( disable : 4710 )

class CTimeCounter
{
public:
    CTimeCounter(void)
    {
        QueryPerformanceFrequency(&m_nFreq);
        QueryPerformanceCounter(&m_nBeginTime);
    }
 
    virtual ~CTimeCounter(void){

    } 
    __int64 GetExecutionTime()
    {
        LARGE_INTEGER nEndTime;
        __int64 nCalcTime;
 
        QueryPerformanceCounter(&nEndTime);
        nCalcTime = (nEndTime.QuadPart - m_nBeginTime.QuadPart) *
            1000/m_nFreq.QuadPart;
 
        delete this;
        return nCalcTime;
    }
protected:
    LARGE_INTEGER m_nFreq;
    LARGE_INTEGER m_nBeginTime;
};
#pragma warning(pop)
#endif    // _TIME_COUNTER_H


/*15.快速清零*/
void *memzero(void *const mem, const size_t n)
{
    size_t i, j, offset;
    unsigned long long *q;
    const unsigned long long qzero = 0ULL;
    unsigned char *b;
    const unsigned char bzero = 0U;
 
    assert(mem != NULL);
    assert(n > 0);
 
    i = 0;
 
    if (n-i >= 2*sizeof(qzero)) {
        b = (unsigned char*)mem;
        offset = 8;
        offset -= (size_t) b % sizeof(qzero);
        switch (offset) {
            case 7: *b = bzero; b++; i++;
            case 6: *b = bzero; b++; i++;
            case 5: *b = bzero; b++; i++;
            case 4: *b = bzero; b++; i++;
            case 3: *b = bzero; b++; i++;
            case 2: *b = bzero; b++; i++;
            case 1: *b = bzero; b++; i++;
            case 0: break;
        }
        q = (unsigned long long *) b;
        q[0] = qzero;
        for (j = 1; j < (n-i)/sizeof(qzero); j++) {
            q[j] = q[j-1];
        }
        i += j*sizeof(qzero);
    }
 
    if (i >= n) {
        return mem;
    }
 
    b = (unsigned char*)mem;
    b += i;
    b[0] = bzero;
    for (j = 1; j < n-i; j++) {
        b[j] = b[j-1];
    }
 
    return mem;
}


/*16. 快速拷贝内存*/
void *memcpy_optimized(void *dst, const void *src, size_t sz)
{
    void *r = dst;
 
    //先进行uint64_t长度的拷贝，一般而言，内存地址都是对齐的，
    size_t n = sz & ~(sizeof(uint64_t) - 1);
    uint64_t *src_u64 = (uint64_t *) src;
    uint64_t *dst_u64 = (uint64_t *) dst;
 
    while (n)
    {
        *dst_u64++ = *src_u64++;
        n -= sizeof(uint64_t);
    }
 
    //将没有非8字节字长取整的部分copy
    n = sz & (sizeof(uint64_t) - 1);
    byte *src_u8 = (byte *) src;
    byte *dst_u8 = (byte *) dst;
    while (n-- )
    {
        (*dst_u8++ = *src_u8++);
    }
 
    return r;
 
    return NULL;
}


/*17. Get application start directory*/
TCHAR path[MAX_PATH];
 
::GetModuleFileName(NULL, path, MAX_PATH);
 
TCHAR drive[_MAX_DRIVE];
TCHAR dir[_MAX_DIR];
 
_tsplitpath(path, drive, dir, NULL, NULL);
 
m_strAppDir = CString(drive) + CString(dir);
m_strConfigDir = m_strAppDir + _T("Config\\");
 
::CreateDirectory(GetConfigDir(), 0);


/*18. 打开资源管理器，定位文件的位置*/
CString filename = pJobObj->GetFilePath();
ShellExecute(NULL, NULL,_T("explorer"), _T("/select, ")+filename, NULL,SW_SHOW);

/*19. MFC中使用CImage显示缩略图的方法*/
CWnd* pWnd;  
pWnd=GetDlgItem(IDC_IMAGE1);  
CDC* pDC=pWnd->GetDC();  
HDC hDC = pDC->m_hDC;  
  
CRect rect_frame;  
CImage image;  
pWnd->GetClientRect(&rect_frame);  
image.Load(fileName);  
  
  
::SetStretchBltMode(hDC,HALFTONE);  
::SetBrushOrgEx(hDC,0,0,NULL);  
  
image.Draw(hDC,rect_frame);  
ReleaseDC(pDC);//释放picture控件的DC 

/*20. 在doc里通过GetFirstViewPosition();来遍历所有的视图*/
//遍历所有的视图  
    POSITION pos = GetFirstViewPosition();  
  
  
    while (pos != NULL)  
    {  
        CView* pView=GetNextView(pos);     
  
  
        if( pView->IsKindOf(RUNTIME_CLASS(testView)))//这里的testView就是要做操作的视图的名字  
        {  
            CVehicleInpectionStickersDetectionDemoView *myView = (CVehicleInpectionStickersDetectionDemoView*)pView;  
  
  
           myView->OnUpdate();//执行一个该视图中的函数，函数的内容就是修改视图中控件的一些内容来判断是否获得视图指针成功  
        }  
          
    }   

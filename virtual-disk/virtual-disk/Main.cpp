
#include "..\virtual-disk-dll\virtual-disk.h"
#include <stdio.h>
#include <Windows.h>
#include <iostream>

typedef IVirtualDiskProxy* (*Func)(void);

int main()
{
    //加载动态连接库。
    HMODULE hDllLib = LoadLibrary("virtual-disk-dll.dll");
    if(hDllLib != NULL)
    {
        //获取动态连接库里的函数地址。
        Func fpFun = (Func)GetProcAddress(hDllLib, "GetVirtualDiskProxy");

        //调用函数运行。
        IVirtualDiskProxy *proxy = (*fpFun)();

        char str[10000];
        int code = 0;
        while(1)
        {
            gets(str);
            code = proxy->ExecCommand(str);
#if 0
            printf("\n///////////\n\n");
            IVolumnProxy *volumnProxy = proxy->GetVolumnByIdx(0);
            IDirProxy *dirProxy = volumnProxy->GetRootDir();

            IFindResult *fResult = NULL;
            fResult = dirProxy->Find("*.txt", true);
            std::cout << fResult->GetCount() << std::endl;
            fResult = dirProxy->Find("a?.c", false);
            std::cout << fResult->GetCount() << std::endl;
            
            fResult->Release();
            printf("\n///////////\n\n");
#endif
            if(code == -1)
                break;
#if 0
            else
                printf("code = %d\n", code);
#endif
        }

        //delete proxy;
        //释放动态连接库。
        FreeLibrary(hDllLib);
    }
    return 0;
}
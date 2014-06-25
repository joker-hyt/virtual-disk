#ifndef VIRTUAL_DISK_H
#define VIRTUAL_DISK_H
#include "vld.h"

enum ETYPE
{
	IFile,
	IDir
};

class IDiskObj
{
public:
	virtual const char *GetName() = 0;
	virtual ETYPE GetType() = 0;
};

class IFindResult
{
public:
	virtual unsigned int GetCount() = 0;
	virtual void Release() = 0;
	virtual IDiskObj *GetItemByIdx(unsigned int) = 0;
};

class IDirProxy : public IDiskObj
{
public:
	// findstr: Ҫ���ҵ��ļ���(������Ŀ¼��)��Ҫ֧��ͨ���* ��
	// bRecursion: �Ƿ�ݹ����
	virtual IFindResult *Find(const char *findstr, bool bRecursion) = 0;
};

class IFileProxy: public IDiskObj
{
};


class IVolumnProxy
{
public:
	virtual IDirProxy *GetRootDir() = 0;
};

class IVirtualDiskProxy
{
public:
	virtual int ExecCommand(const char *command) = 0; //����1��ʾ�ɹ���0��ʾʧ�ܣ�-1�˳�
	virtual int GetVolumnCount() = 0;
	virtual IVolumnProxy *GetVolumnByIdx(int) = 0;
};

// �����ӿ�
#ifdef DLL_IMPLEMENT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

extern "C" DLL_API IVirtualDiskProxy *GetVirtualDiskProxy();

#endif
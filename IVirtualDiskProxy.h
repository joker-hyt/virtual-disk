#pragma once

enum ETYPE
{
	IFile,
	IDir
};
class IDiskObj
{
public:
	virtual const char* GetName()=0;
	virtual ETYPE  GetType()=0;
};

class IFindResult
{
public:
	virtual void Release() =0;
	virtual unsigned int GetCount()=0;
	virtual IDiskObj* GetItemByIdx(unsigned int)=0;

};



class IDirProxy:public IDiskObj
{
public:
	//findstr:要查找的文件名(可能是目录名)，要支持通配符* ？
	//bRecursion：是否递归查找
	virtual IFindResult* Find(const char* findstr,bool bRecursion)=0;

};

class IFileProxy: public IDiskObj
{

};


class IVolumnProxy
{
public:
	virtual IDirProxy* GetRootDir()=0;

};

class IVirtualDiskProxy
{
public:
	virtual int ExecCommand(const char* command)=0; //返回1表示成功，0表示失败
	virtual int GetVolumnCount()=0;
	virtual IVolumnProxy* GetVolumnByIdx(int)=0;

};



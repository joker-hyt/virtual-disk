/**
 * VirtualDisk�ࡢVolume�ࡢFindResult��FileNode�ඨ��
 * ����ļ�ϵͳ�ṹ�Ķ���
 */

#ifndef FILESYS_H
#define FILESYS_H

#define DLL_IMPLEMENT

#include <stdio.h>
#include "virtual-disk.h"
#include "MyString.h"
#include "ErrMsg.h"
#include <string.h>
#include <time.h>

enum FType
{
    Type_Dir,
    Type_File,
    Type_Dir_File
};

/**
 * ����·���ڵ����
 */
class FindResult;

class FileNode
{
public:
    FileNode();
    FileNode(MyString &name, time_t time);
    virtual ~FileNode();

    virtual bool isLeaf() const = 0;

    virtual void setParent(FileNode *parent) { m_parent = parent; }
    virtual void setBrother(FileNode *brother) { m_brother = brother; }
    virtual void setChild(FileNode *child) {}

    virtual void setName(MyString &name) { m_name = name; }
    virtual void setTime(time_t time) { m_time = time; }

    virtual FileNode *getParent() const { return m_parent; }
    virtual FileNode *getBrother() const { return m_brother; }
    virtual FileNode *getChild() const { return NULL; };

    virtual MyString getName() const { return m_name; }
    virtual time_t getTime() const { return m_time; }

    virtual void setFile(char *file_data, unsigned long size) {}
    virtual char *getFileData() const { return NULL; }
    virtual unsigned long getSize() const { return 0; }


    virtual void getPathStr(MyString &path);

    virtual void del(FileNode *node, int &numFile, int &numDir);  // ɾ����ǰ�ڵ���ӽڵ�node
    virtual void del(MyString &name, FType type, bool bRecursion, int &numFile, int &numDir);  // ɾ���ڵ�
    
    virtual void dir(int &numFile, int &numDir);  // ��ӡ��ǰ�ڵ�
    virtual void dir(MyString &name, FType type, bool bRecursion, int &numFile, int &numDir);  // ��ӡ�ڵ�

    virtual FileNode *find(MyString &name, FType type, bool isWildSupport);  // ���ҵ�ǰ�ڵ����Ƿ������Ϊname�Ľڵ�

    virtual FileNode *create(MyString &name, time_t t = NULL, char *data = NULL, unsigned long size = 0);  // �����ڵ㣬�����ش����Ľڵ�

    virtual FindResult *FindStr(MyString &findName, bool bRecursion);  // �����ַ�������

protected:

    FileNode *m_parent;
    FileNode *m_brother;
    
    MyString m_name;
    time_t m_time;
};

/**
 * FindResult�ඨ�壺����ģʽ
 */
class FindResult : public IFindResult
{
public:
    static FindResult *getInstance();

    void setFileNode(FileNode *fileNode);
    void resize(int size);

    // �����ӿ�
    virtual unsigned int GetCount() { return m_count; }
    virtual void Release();
    virtual IDiskObj *GetItemByIdx(unsigned int idx);

private:
	FindResult();
    ~FindResult();
    static const unsigned int INIT_SIZE;
    unsigned int m_count;  // �洢���ٸ��ڵ�
    unsigned int m_size;  // �洢�ռ��С
    FileNode **m_file_node;

    static FindResult *m_find_result;
    class Garbo
    {
    public:
        ~Garbo()
        {
            if(FindResult::m_find_result != NULL)
            {
                delete FindResult::m_find_result;
                FindResult::m_find_result = NULL;
            }
        }
    };
    static Garbo garbo;
};

/**
 * �����ļ��ڵ㣬�̳���FileNode
 */
class LeafNode : public FileNode, public IFileProxy
{
public:
    LeafNode();
    LeafNode(MyString &name, time_t time);
    LeafNode(MyString &name, time_t time, char *file_data, int size);
    ~LeafNode();

    virtual bool isLeaf() const { return true; }
    
    virtual void setFile(char *file_data, unsigned long size);
    virtual char *getFileData() const { return m_file_data; }
    virtual unsigned long getSize() const { return m_size; }

    // �����ӿ�
    virtual const char *GetName() { return m_name.c_str(); }
    virtual ETYPE GetType() { return IFile; }

private:
    char *m_file_data;
    unsigned long m_size;
};

/**
 * ����Ŀ¼�ڵ㣬�̳���FileNode
 */
class BranchNode : public FileNode, public IDirProxy
{
public:
    BranchNode();
    BranchNode(MyString &name, time_t time);
    ~BranchNode();

    virtual bool isLeaf() const { return false; }
    
    virtual void setChild(FileNode *child) { m_child = child; }
    virtual FileNode *getChild() const { return m_child; }

    // �����ӿ�
    virtual const char *GetName() { return m_name.c_str(); }
    virtual ETYPE GetType() { return IDir; }
    virtual IFindResult *Find(const char *findstr, bool bRecursion);

private:
    FileNode *m_child;
};

/**
 * �������������ļ���Ϣ
 * ��copy����ʹ��
 */
struct FileInfo
{
    MyString m_path;
    MyString m_name;
    unsigned long m_size;
    time_t m_time;
};

/**
 * �������������ļ��б�
 * ��copy����ʹ��
 */
class FileList
{
public:
    FileList();
    ~FileList();

    void add(MyString &path, MyString &name, unsigned long size, time_t time);
    MyString &getPathByIdx(int idx) const { return m_file_info[idx].m_path; }
    MyString &getNameByIdx(int idx) const { return m_file_info[idx].m_name; }
    unsigned long getSizeByIdx(int idx) const { return m_file_info[idx].m_size; }
    time_t getTimeByIdx(int idx) { return m_file_info[idx].m_time; }
    int getCount() const { return m_count; }

private:
    void resize(int size);

    static const int INIT_SIZE;
    int m_size;
    int m_count;
    FileInfo *m_file_info;
};

/**
 * ������
 */
class Volume : public IVolumnProxy
{
public:
    Volume();
    Volume(MyString &name);
    ~Volume();

    FileNode *getNodeByStr(MyString &path, FileNode *&parentNode, int &pos);  // ͨ��·����ڵ�
    int createNode(MyString &path, int startPos);  // ����·���½��ڵ㼰���ӽڵ�

    FileNode *getCurDir() { return m_cur_node; }
    FileNode *getRootDir() { return m_root_node; }
    void setCurDir(FileNode *cur_dir) { m_cur_node = cur_dir; }

    // �����ӿ�
    virtual IDirProxy *GetRootDir() { return dynamic_cast<BranchNode *>(m_root_node); }

private:
    FileNode *m_root_node;
    FileNode *m_cur_node;
};

/**
 * �����������
 */
class VirtualDisk : public IVirtualDiskProxy
{
public:
    static VirtualDisk *getInstance(int numVolume);

    int Cd(MyString &path);  // ת��·��
    int Compare(MyString &realPath, MyString &virtualPath);  // �Ƚ������ļ��Ƿ����
    int Dir(MyString &path, FType type, bool bRecursion);  // dir����
    int Rmdir(MyString &path, bool bRecursion);  // ɾ��·��
    int Mkdir(MyString &path);  // ����·��
    int Del(MyString &path, bool bRecursion);  // ɾ���ļ�
    int Copy(MyString &realPath, MyString &virtualPath);  // �����ļ�
    int SwitchVolume(MyString &path);  // ���ת��

    int checkPath(MyString &path);  // �������·���Ƿ�Ϸ�

    int copyToOne(FileNode *node, FileList &fileList, MyString &name);  // ��������ļ�Ϊһ���ļ�
    int copyFile(FileNode *node, FileList &fileList, MyString &name);  // ��������ļ�Ϊ����ļ�

    void showPath(Volume *volume);  // ��ӡ��ǰ·��
    int readFile(char *fileData, MyString &path, unsigned long len);  // ��ʵ�ʴ��̶�ȡ�ļ�
    bool isBinary(char *buf, unsigned long len);  // �ж��ļ��Ƿ��Ƕ������ļ�
    void printNotMatch(unsigned char *buf, unsigned long pos, bool isBinary);  // Compare����������ƥ��ʱ����ӡ��Ϣ
    void getDirAndFile(MyString &realPath, MyString &dir, MyString &file);  // ��ȡ�����ڶ���·������Ŀ¼��
    bool existDir(MyString &path, int start);  // �ж�path[start]�����Ƿ����Ŀ¼
    int findRealFile(MyString &realPath, FileList &fileList, bool bRecursion);  // �����Ǵ����в����ļ���������fileList��
    MyString &genName(MyString &realName, MyString &virtualName, MyString &retName);  // copyʱ��������������ļ�������

    Volume *getVolumeByStr(MyString &path, int &pos);  // ͨ��·�����ұ��
    Volume *getCurVolume() { return m_cur_volume; }
    Volume *getVolumeByIdx(int idx) { if(idx < 0 || idx >= m_count) return NULL; else return m_volume[idx]; }

    // �����ӿ�
    virtual int ExecCommand(const char *command); //����1��ʾ�ɹ���0��ʾʧ��
    virtual int GetVolumnCount() { return m_count; }
    virtual IVolumnProxy *GetVolumnByIdx(int idx) { if(idx < 0 || idx >= m_count) return NULL; else return m_volume[idx]; }

private:
    static const int MAX_COUNT;
    static const int COMPARE_PRINT_LEN;
    static const int MAX_PATH_NAME_LEN;
    int m_count;
    Volume *m_cur_volume;
    Volume **m_volume;

private:
    VirtualDisk(int count);
    ~VirtualDisk();

    static VirtualDisk *m_virtual_disk;
    class Garbo
    {
    public:
        ~Garbo()
        {
            if(VirtualDisk::m_virtual_disk != NULL)
            {
                delete VirtualDisk::m_virtual_disk;
                VirtualDisk::m_virtual_disk = NULL;
            }
        }
    };
    static Garbo garbo;
};

#endif
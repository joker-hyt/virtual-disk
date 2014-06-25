/**
 * MyString�ඨ��
 */

#ifndef MYSTRING_H
#define MYSTRING_H

/**
 * MyString�ࣺ�ṩ�ַ�������
 */
class MyString
{
public:
    MyString();
    MyString(const char *str);
    MyString(const MyString &other);
    MyString(const MyString &other, int start, int end);
    MyString &operator =(const MyString &other);
    ~MyString(void);

    // �ַ�����
    char &operator [](int n);
    const char *c_str() const;

    int size() const;

    // �ַ�������
    MyString &operator +=(const MyString &other);

    bool operator ==(const MyString &other);
    bool operator !=(const MyString &other);
    bool wildCmp(const MyString &other);  // �ж�ͨ����ַ����Ƿ����
    bool find(const char c);  // �ַ����Ƿ����ĳ���ַ�

private:
    char *m_str;
    int m_size;
};

#endif

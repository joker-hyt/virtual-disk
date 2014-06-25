/**
 * MyString类定义
 */

#ifndef MYSTRING_H
#define MYSTRING_H

/**
 * MyString类：提供字符串操作
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

    // 字符操作
    char &operator [](int n);
    const char *c_str() const;

    int size() const;

    // 字符串连接
    MyString &operator +=(const MyString &other);

    bool operator ==(const MyString &other);
    bool operator !=(const MyString &other);
    bool wildCmp(const MyString &other);  // 判断通配符字符串是否相等
    bool find(const char c);  // 字符串是否包含某个字符

private:
    char *m_str;
    int m_size;
};

#endif

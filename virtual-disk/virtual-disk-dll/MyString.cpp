/**
* MyString类实现
*/

#include "MyString.h"
#include <string.h>

MyString::MyString()
{
    m_size = 0;
    m_str = new char[m_size + 1];
    *m_str = '\0';
}

MyString::MyString(const char *str)
{
    if(str == NULL)
    {
        m_size = 0;
        m_str = new char[m_size + 1];
        *m_str = '\0';
    }
    else
    {
        m_size = strlen(str);
        m_str = new char[m_size + 1];
        strcpy(m_str, str);
        m_str[m_size] = '\0';
    }
}

MyString::MyString(const MyString &other)
{
    m_size = strlen(other.m_str);
    m_str = new char[m_size + 1];
    strcpy(m_str, other.m_str);
    m_str[m_size] = '\0';
}

MyString::MyString(const MyString &other, int start, int end)
{
    if(start >= 0 && end <= other.m_size && end > start)
    {
        m_size = end - start;
        m_str = new char[m_size + 1];
        for(int i = 0; i < m_size; ++i)
            m_str[i] = other.m_str[i + start];
        m_str[m_size] = '\0';
    }
    else
    {
        m_size = 0;
        m_str = new char[m_size + 1];
        *m_str = '\0';
    }
}

MyString &MyString::operator =(const MyString &other)
{
    if(this == &other)
        return *this;

    delete []m_str;

    m_size = strlen(other.m_str);
    m_str = new char[m_size + 1];
    strcpy(m_str, other.m_str);
    m_str[m_size] = '\0';

    return *this;
}

MyString::~MyString(void)
{
    delete []m_str;
    m_str = NULL;
    m_size = 0;
}

char &MyString::operator [](int n)
{
    return m_str[n];
}

const char *MyString::c_str() const
{
    return m_str;
}


int MyString::size() const
{
    return m_size;
}

MyString &MyString::operator +=(const MyString &other)
{
    if(other.m_size == 0)
        return *this;
    else if(m_size == 0)
    {
        *this = other;
        return *this;
    }
    else
    {
        int len = m_size + other.m_size;
        char *str = new char[len + 1];
        strcpy(str, m_str);
        strcat(str, other.m_str);
        str[len] = '\0';

        delete []m_str;
        m_str = str;
        m_size = len;
    }
    return *this;
}

static inline char lowcase(char c)
{
    const char dist = 'a' - 'A';
    if(c >= 'A' && c <= 'Z')
        return c + dist;
    else
        return c;
}
static inline bool isEqual(char c1, char c2)
{
    return (lowcase(c1) == lowcase(c2) ? true : false);
}

bool MyString::operator ==(const MyString &other)
{
    if(m_size != other.m_size)
        return false;
    else
    {
        if(strcmp(m_str, other.m_str) == 0)
            return true;
        else
        {
            for(int i = 0; i < m_size; ++i)
            {
                if(!isEqual(m_str[i], other.m_str[i]))
                {
                    return false;
                }
            }
            return true;
        }
    }
}

bool MyString::operator !=(const MyString &other)
{
    if(m_size != other.m_size)
        return true;
    else
    {
        if(strcmp(m_str, other.m_str) == 0)
            return false;
        else
        {
            for(int i = 0; i < m_size; ++i)
            {
                if(!isEqual(m_str[i], other.m_str[i]))
                {
                    return true;
                }
            }
            return false;
        }
    }
}

bool MyString::wildCmp(const MyString &other)
{
    // *.txt == *.txt*
    MyString tmpThis = *this;
    if(m_size >= 5)
    {
        MyString tmp(*this, m_size - 5, m_size);
        if(tmp == "*.txt")
        {
            tmpThis += "*";
        }
    }
    // *.* == *
    if(*this == "*.*")
        *this = "*";

    const char *wild = tmpThis.m_str;
    const char *ostr = other.m_str;
    const char *cp = NULL;
	const char *mp = NULL;
	// 此循环匹配
    while((*ostr != '\0') && (*wild != '*')) 
	{
        if((!isEqual(*wild, *ostr)) && (*wild != '?'))
		{
            return false;
        }
        else if((!isEqual(*wild, *ostr)) && (*wild == '?') && (*ostr == '.'))
        {
            --ostr;
        }
        ++wild;
        ++ostr;
    }
	
    while(*ostr != '\0')
	{
        if(*wild == '*')
		{
            if(*++wild == '\0')    //wild以*结尾
			{
                return true;
            }
            mp = wild;
            cp = ostr + 1;
        }
		else if(isEqual(*wild, *ostr))
		{
            ++wild;
            ++ostr;
        }
        else if(*wild == '?')
        {
            if(*ostr == '.')
                --ostr;
            ++wild;
            ++ostr;
        }
		else
		{
            wild = mp;
            ostr = cp++;
        } 
    }

    while(*wild == '*')
    {
        ++wild;
    }
    return (*wild == '\0');
}

bool MyString::find(const char c)
{
    for(int i = 0; i < m_size; ++i)
    {
        if(isEqual(m_str[i], c))
            return true;
    }
    return false;
}
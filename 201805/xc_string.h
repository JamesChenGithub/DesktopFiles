//
//  xc_string.h
//  XLive_Demo_Mac
//
//  Created by AlexiChen on 2018/5/14.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#ifndef xc_string_h
#define xc_string_h

#include <iostream>
#include <cstring>
#include <cctype>

namespace xc_api
{
    
    class xc_string
    {
    private:
        char *m_str = nullptr;
        int m_size = 0;
    private:
        static const int xc_string_per_size = 64;
        
        
    public:
        int length() const
        {
            if (m_str)
            {
                return (int)strlen(m_str);
            }
            return 0;
        }
        const char *c_str() const
        {
            return m_str;
        }
        
    private:
        void release()
        {
            if (m_str)
            {
                free(m_str);
                m_str = nullptr;
            }
        }
        
    private:
        static char *alloc(int &size)
        {
            char *ret = (char *)malloc(xc_string_per_size);
            memset(ret, 0, xc_string_per_size);
            size = xc_string_per_size;
            return ret;
        }
        
        static char *alloc_cpy(const char *str, int &size)
        {
            if (str == nullptr)
            {
                return alloc(size);
            }
            else
            {
                int strsize = (int)strlen(str);
                int alsize = xc_string_per_size;
                if (strsize > xc_string_per_size)
                {
                    alsize = (strsize / xc_string_per_size) * 2 * xc_string_per_size;
                }
                
                char *ret = (char *)malloc(alsize);
                memset(ret, 0, alsize);
                strcpy(ret, str);
                size = alsize;
                return ret;
            }
            
        }
        
        static char *alloc_cat(char *dest, const char *src, int &size)
        {
            int dlen = (int)strlen(dest);
            int slen = (int)strlen(src);
            
            // realloc
            int strsize = dlen + slen;
            int alsize = xc_string_per_size;
            if (strsize > xc_string_per_size)
            {
                alsize = (strsize / xc_string_per_size) * 2 * xc_string_per_size;
            }
            
            char *ret = (char *)malloc(alsize);
            memset(ret, 0, alsize);
            strcpy(ret, dest);
            strcpy(ret+dlen, src);
            return ret;
            
        }
        static char *cat_alloc_cpy(char *dest, const char *src, int &size)
        {
            int dlen = (int)strlen(dest);
            int reset = size - dlen;
            int slen = (int)strlen(src);
            
            if (reset > slen)
            {
                // cat direct
                strcat(dest, src);
                return dest;
            }
            else
            {
                // realloc
                int strsize = slen;
                int alsize = xc_string_per_size;
                if (strsize > xc_string_per_size)
                {
                    alsize = (strsize / xc_string_per_size) * 2 * xc_string_per_size;
                }
                
                alsize += size;
                
                char *ret = (char *)malloc(alsize);
                memset(ret, 0, alsize);
                strcpy(ret, dest);
                strcpy(ret+dlen, src);
                size = alsize;
                free(dest);
                dest = nullptr;
                dest = ret;
                return ret;
            }
        }
        
       
        
    public:
        xc_string()
        {
            this->m_str = nullptr;
            this->m_size = 0;
        }
        xc_string(const char *str)
        {
            this->m_str = xc_string::alloc_cpy(str, this->m_size);
        }
        
        
        xc_string(const xc_string &xcstr)
        {
            this->m_str = xc_string::alloc_cpy(xcstr.c_str(), this->m_size);
        }
        
        ~xc_string()
        {
            release();
        }
        
    public:
        xc_string& operator = (const xc_string &xcstr)
        {
            this->m_str = xc_string::alloc_cpy(xcstr.c_str(), this->m_size);
            return *this;
        }
        
        xc_string operator + (const xc_string &xcstr)
        {
            int size = this->m_size;
            const char *dest = xc_string::alloc_cat(this->m_str, xcstr.c_str(), size);
            
            xc_string str;
            str.m_str = xc_string::alloc_cpy(dest, size);
            str.m_size = size;
            return str;
        }

        xc_string operator + (const char *str)
        {
            int size = this->m_size;
            const char *dest = xc_string::alloc_cat(this->m_str, str, size);
            
            xc_string xcstr;
            xcstr.m_str = xc_string::alloc_cpy(dest, size);
            xcstr.m_size = size;
            return str;
        }

        xc_string& operator +=(const xc_string &xcstr)
        {
            xc_string::cat_alloc_cpy(this->m_str, xcstr.c_str(), this->m_size);
            return *this;
        }

        xc_string& operator+=(const char *str)
        {
            xc_string::cat_alloc_cpy(this->m_str, str, this->m_size);
            return *this;
        }

        bool operator==(const xc_string &xcstr)
        {
            if(strcmp(this->m_str, xcstr.m_str) == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool operator==(const char *str)
        {
            if(strcmp(this->m_str, str) == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool operator !=(const xc_string &xcstr)
        {
            if(strcmp(this->m_str, xcstr.m_str) == 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        bool operator != (const char *str)
        {
            if(strcmp(this->m_str, str) == 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        char operator[](int position)
        {
            return *(m_str + position);
        }

        friend std::istream& operator >> (std::istream &ins, xc_string& str)
        {
            ins >> str.m_str;
            return ins;
        }
        friend std::ostream& operator << (std::ostream &os, const xc_string& str)
        {
            os << str.m_str;
            return os;
        }
    };
}


#endif /* xc_string_h */

#include<iostream>
#include<string>
#include<cstring>
#include<algorithm>
#ifndef Longint_h
#define Longint_h

enum Symbol {negative,positive};
Symbol reversed_symbol(const Symbol &sym);

class Longint{
    //重载运算符：
    friend Longint operator+(const Longint &l1,const Longint &l2);
    friend Longint operator-(const Longint &l1,const Longint &l2);
    friend Longint operator*(const Longint &l1,const Longint &l2);
    friend Longint operator/(const Longint &l1,const Longint &l2);//整除，向下取整
    friend Longint operator%(const Longint &l1,const Longint &l2);
    
    public:
        std::string number="";
        Symbol symbol=positive;
    public:
        friend std::ostream & operator<<(std::ostream & os,const Longint &obj);
        Longint &operator=(const Longint &other);
        Longint &operator=(const std::string&s);
        Longint &operator=(const char[]);
        Longint &operator=(int a);
        Longint &operator=(long long b);
        explicit operator double()const;
    //构造函数：
        Longint(const std::string &input_string);
        explicit Longint(int input_int);
        explicit Longint(long long input_long_long);
        Longint()=default;
    //拷贝构造函数：
        Longint(const Longint &l1);
        Longint(Longint &&other);
    //析构函数：
};
#endif
#ifndef aData_h
#define aData_h
#include "Longint.cpp"
class Data{
  public:
  //数据类型
    int type;
  //数据
    std::string name;//type=0
    Longint num; //type=1
    double d; //type=2
    std::string s; //type=3
    bool b; //type=4
    //type=5 表示数据为 None
    bool isname();
  //构造函数
    explicit Data(Longint L);
    explicit Data(double D);
    Data(std::string S);
    Data(const char ch[]);
    explicit Data(bool B);
    Data(const char[],bool B);
    Data(std::string _name,bool b1,bool b2);
    Data(const char[],bool b1,bool b2);
    Data();
  //type_类型转换函数
    Data __longint()const;
    Data __double()const;
    Data __string()const;
    Data __bool()const;
  //比较运算符
    bool operator>(const Data &other)const;
    bool operator<(const Data &other)const;
    bool operator>=(const Data &other)const;
    bool operator<=(const Data &other)const;
    bool operator==(const Data &other)const;
    bool operator!=(const Data &other)const;

  //算数运算符
    Data operator+(const Data &other)const;
    Data operator*(const Data &other)const;
    Data operator-(const Data &other)const;
    Data operator/(const Data &other)const;
    Data div(const Data &other)const;
    Data operator%(const Data &other)const;
    Data operator-()const;
    Data operator+()const;
  //算数赋值运算符
    Data operator+=(const Data &other);
    Data operator-=(const Data &other);
    Data operator*=(const Data &other);
    Data operator/=(const Data &other);
    Data div_eq(const Data &other);
    Data operator%=(const Data &other);
  //类型转换函数
    explicit operator bool() const;
    Longint get_longint() const;
    Data get_value() const;

    friend std::ostream& operator<<(std::ostream& os,Data data);
};

std::ostream& operator<<(std::ostream& os,Data data);
#endif
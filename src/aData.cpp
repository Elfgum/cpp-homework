#include "aData.h"

bool Data::isname() {return !type;}
//构造函数
  Data::Data(Longint L):type(1){num=L;}
  Data::Data(double D):type(2),d(D){};
  Data::Data(std::string S):type(3),s(S){};
  Data::Data(const char ch[]):type(3){
      s="";
      for (int i=0;ch[i]!='\0';i++) s+=ch[i];
  }
  Data::Data(bool B):type(4),b(B){};
  Data::Data(const char ch[],bool B):type(5){};
  Data::Data(std::string _name,bool b1,bool b2):type(0),name(_name){};
  Data::Data(const char ch[],bool b1,bool b2):type(0){
      name="";
      for (int i=0;ch[i]!='\0';i++) name+=ch[i];
  };
  Data::Data():type(5){};
//type_类型转换函数
  Data Data::__longint()const{
    Longint L1,L2;
    L1="0",L2="1";
    Data ret(L1);
    switch (type)
    {
      case 1: return *this;
      case 2:{
        std::string tmp=std::to_string(d);
        tmp=tmp.substr(0,tmp.length()-7);
        ret.num=tmp;
        break;
      }
      case 3: ret.num=s; break;
      case 4: ret.num=(b?L2:L1);
    }
    return ret;
  }
  Data Data::__double()const{
    double tmpd=0.0;
    Data ret(tmpd); 
    switch(type)
    {
      case 1: ret.d=(double) num; break;
      case 2: return *this;
      case 4: ret.d=int(b);
    }
    return ret;
  }
  Data Data::__string()const{
    std::string tmps="";
    Data ret(tmps);
    switch(type)
    {
      case 1: {
        if (num.symbol==negative) ret.s="-";
        ret.s=ret.s+num.number;
      }
      case 2: ret.s=std::to_string(d); break;
      case 3: return *this;
      case 4: ret.s=(b?"True":"False");
    }
    return ret;
  }
  Data Data::__bool()const{
    switch (type){
      case 1: return Data(num.number!="0");
      case 2: return Data(d!=0);
      case 3: return Data(s!="");
      case 4: return *this;
    }
  }
//比较运算符
  bool Data::operator>(const Data &other)const{
    if(type==3&&other.type==3) return s>other.s;
    if(type==2||other.type==2){
      bool judge=(__string().d>other.__string().d);
      return judge;
    }
    Longint tmpL=__longint().num-other.__longint().num;
    return (tmpL.symbol==positive&&tmpL.number!="0");
  }
  bool Data::operator<(const Data &other)const{
    bool judge=(other>*this);
    return judge;
  }
  bool Data::operator>=(const Data &other)const{
    bool judge=!(*this<other);
    return judge;
  }
  bool Data::operator<=(const Data &other)const{
    bool judge=!(*this>other);
    return judge;
  }
  bool Data::operator==(const Data &other)const{
    if (type==3&&other.type==3) return s==other.s;
    if (type==5||other.type==5){
      bool judge1=(type!=5||other.type!=5)?false:true;
      return judge1;
    }
    bool judge2=!(*this>other||*this<other);
    return judge2;
  }
  bool Data::operator!=(const Data &other)const{
    bool judge=!(*this==other);
    return judge;
  }

//算数运算符
  Data Data::operator+(const Data &other)const{
    if (type==3&&other.type==3) return Data(s+other.s);
    if (type==2||other.type==2){
      double retd=__double().d+other.__double().d;
      return Data(retd);
    }
    Longint tempL=__longint().num+other.__longint().num;
    return Data(tempL);
  }
  Data Data::operator*(const Data &other)const{
    if(type==3){//字符串与数字相乘，那么这个数字显然不能是大整数。
      double tmp=other.__double().d;
      int times=int(tmp);
      if (times<1) return Data("");
      std::string ret="";
      for(int i=0;i<times;i++) ret+=s;
      return Data(ret);
    }
    if(other.type==3){
      double tmp=__double().d;
      int times=int(tmp);
      if (times<1) return Data("");
      std::string ret="";
      for(int i=0;i<times;i++) ret+=other.s;
      return Data(ret);
    }
    if(type==2||other.type==2){
      double retd=__double().d*other.__double().d;
      return Data(retd);
    }
    Longint tempL=__longint().num*other.__longint().num;
    return Data(tempL);
  }
  Data Data::operator-(const Data &other)const{
    if(type==2||other.type==2){
      double retd=__double().d-other.__double().d;
      return Data(retd);
    }
    Longint tempL=__longint().num-other.__longint().num;
    return Data(tempL);
  }
  Data Data::operator/(const Data &other)const{
    double retd=__double().d/other.__double().d;
    return Data(retd);
  }
  Data Data::div(const Data &other)const{ //整除 返回值type=1
    return Data(this->__longint().num/other.__longint().num); 
  }//整除仅大整数可用
  Data Data::operator%(const Data &other)const{ //取余 返回值type=1
    return Data(this->__longint().num%other.__longint().num);
  }
  Data Data::operator-()const{
    Data ret=*this;
    switch (type){
      case 1: ret.num.symbol=(ret.num.symbol==positive? negative:positive); break;
      case 2: ret.d=-ret.d; break;
      case 4: ret.num=-int(b); ret.type=1;
    }
    return ret;
  }
  Data Data::operator+()const{
    if (type==4){
      Data ret=*this;
      ret.type=1;
      ret.num=int(b);
      return ret;
    }
    return *this;
  }
//算数赋值运算符
  Data Data::operator+=(const Data &other){
    *this=*this+other;
    return *this;
  }
  Data Data::operator-=(const Data &other){
    *this=*this-other;
    return *this;
  }
  Data Data::operator*=(const Data &other){
    *this=*this*other;
    return *this;
  }
  Data Data::operator/=(const Data &other){
    *this=*this/other;
    return *this;
  }
  Data Data::div_eq(const Data &other){
    *this=this->div(other);
    return *this;
  }
  Data Data::operator%=(const Data &other){
    *this=*this%other;
    return *this;
  }
//类型转换函数(有两个在Evalvisitor.cpp里实现)
  Longint Data::get_longint() const{
     return __longint().num; 
  }

#include "Longint.h"

Symbol reversed_symbol(const Symbol &sym){
    return (sym==positive?negative:positive);
}

/************************************************/
/*-------------------构造函数-------------------*/
Longint::Longint(const std::string &input_string)
{
    if(input_string!="")        {
    std::string temp;
    if(input_string[0]=='-') {
        symbol=negative;
        temp=input_string.substr(1,input_string.size()-1);
    }else 
        temp=input_string;
//    for(register int i=0;i<input_string.size();i++) temp[i]=input_string[i+1];
   
    number=temp;                }
}

Longint::Longint(int input_int)
{
    if(input_int<0) symbol=negative,input_int*=-1;
//    number=Num_input_to_Longint(input_int);
    number = std::to_string(input_int);
}

Longint::Longint(long long input_long_long)
{
    if(input_long_long<0) symbol=negative,input_long_long*=-1;
//    number=Num_input_to_Longint(input_long_long);
    number = std::to_string(input_long_long);
}

Longint::Longint(const Longint &other):number(other.number),symbol(other.symbol){}//以后可能改为字符数组实现这个类，先占个位置。

Longint::Longint(Longint &&other):number(other.number),symbol(other.symbol){}//以后可能改为字符数组实现这个类，先占个位置。
/************************************************/
/*------------------重载运算符------------------*/

Longint::operator double() const{
    double tmp=0;
    for (int i=0;i<number.size();i++){
        tmp=tmp*10+int(number[i]-'0');
    }
    if (symbol==negative) tmp*=-1;
    return tmp;
}
std::ostream &operator<<(std::ostream & os,const Longint &obj){
    if( (obj.symbol==negative)&&(obj.number!="0") ) os<<'-';
    os<<obj.number;
    return os;
}

Longint &Longint::operator=(const std::string &s){
    if(s!="")        {
    std::string temp;
    if(s[0]=='-') {
        symbol=negative;
        temp=s.substr(1,s.size()-1);
    }else{
        symbol=positive;
        temp=s;}
    number=temp;     }
    return *this;
}

Longint &Longint::operator=(const char ch[] ){
    std::string temp;
    if(ch[0]=='-'){
        symbol=negative;
        int i=1;
        while(ch[i]!='\0') {
            number+=ch[i++];
        }
    }else{
        symbol=positive;
        int i=0;
        while(ch[i]!='\0') {
            number+=ch[i++];
        }
    }
    return *this;
}
Longint &Longint::operator=(const Longint &other){
    symbol=other.symbol;
    number=other.number;
    return *this;
}

Longint &Longint::operator=(int a){
    if(a>0) symbol=positive;
        else symbol=negative,a*=-1;
    number=std::to_string(a);
    return *this;
}

Longint &Longint::operator=(long long b){
    if(b>0) symbol=positive;
        else symbol=negative,b*=-1;
    number=std::to_string(b);
    return *this;
}

Longint operator+(const Longint &l1,const Longint &l2){
    if(l1.symbol!=l2.symbol){
        Longint temp(l2);
        temp.symbol=reversed_symbol(l2.symbol);
        temp=l1-temp;
        return temp;
    }

    register int i=l1.number.size(),j=l2.number.size();
    int length=std::max(i,j),mini=std::min(i,j);
    int r=0;
    Longint s;
    if(l1.symbol==positive) s.symbol=positive;
    if(l1.symbol==negative) s.symbol=negative;
    std::string S1=l1.number,S2=l2.number;
    reverse(S1.begin(),S1.end());
    reverse(S2.begin(),S2.end());
    for(register int k=0;k<mini;k++){
        int p=(S1[k]-'0')+(S2[k]-'0')+r;
        r=(p/10?1:0);
        s.number+=char(p%10+'0');
    }
    if(i>j) for(register int k=mini;k<length;k++) {
        int p=S1[k]-'0'+r;
        r=(p/10?1:0);
        s.number+=char(p%10+'0');
    }
    else for(register int k=mini;k<length;k++){
        int p=S2[k]-'0'+r;
        r=(p/10?1:0);
        s.number+=char(p%10+'0');
    }
    if(r) s.number+='1';
    reverse(s.number.begin(),s.number.end());
    return s;
}

Longint operator-(const Longint &l1,const Longint &l2){
    if(l1.symbol!=l2.symbol){
        Longint temp(l2);
        temp.symbol=reversed_symbol(l2.symbol);
        temp=l1+temp;
        return temp;
    }
    Longint s; 
    if( (l1.number<l2.number&&l1.number.size()==l2.number.size())||l1.number.size()<l2.number.size() ){
        s=l2-l1;
        s.symbol=reversed_symbol(s.symbol);
        return s;
    }
    s.symbol=l1.symbol;
    std::string S1=l1.number,S2=l2.number;
    reverse(S1.begin(),S1.end());
    reverse(S2.begin(),S2.end());
    S2.append(l1.number.size()-l2.number.size(),'0');
    int a[l1.number.size()+2]={0};
    for(register int i=0;i<l1.number.size();i++) a[i]=S1[i]-S2[i];
    for(register int i=0;i<l1.number.size();i++) {
        if(a[i]<0) --a[i+1],a[i]+=10;
    }
    register int i;
    for(i=l1.number.size();i>=0&&a[i]==0;i--);
    if( i==-1 ) {
        s.number="0";
        s.symbol=positive;
        return s;
    }
    for(register int j=i;j>=0;j--) s.number+=char(a[j]+'0');
    return s;
}

Longint operator*(const Longint &l1,const Longint &l2){
    Longint value;
    if(l1.symbol!=l2.symbol) value.symbol=negative;
    if(l1.number=="0"||l2.number=="0") {
        value.number="0";
        value.symbol=positive;
        return value;
    }
    int a[l1.number.size()+2]={0},b[l2.number.size()+2]={0},s[l1.number.size()+l2.number.size()+5]={0},c;
    for(register int i=0; i<l1.number.size(); i++) a[i]=l1.number[l1.number.size()-1-i]-'0';
    for(register int i=0; i<l2.number.size(); i++) b[i]=l2.number[l2.number.size()-1-i]-'0';
    for(register int i=0; i<l1.number.size(); i++){
        c=0;
        for(register int j=0;j<l2.number.size();j++){
            s[i+j]+=a[i]*b[j]+c;
            c=s[i+j]/10;
            s[i+j]%=10;
        }
        s[i+l2.number.size()]+=c;
    }
    int i;
    for(i=l1.number.size()+l2.number.size()+4;i>=0&&!s[i];i--);
    for(register int j=i;j>=0;j--) value.number+=char(s[j]+'0');
    return value;
}

Longint operator/(const Longint &l1,const Longint &l2){
    Longint value;
    if(l1.symbol!=l2.symbol) value.symbol=negative;
    if(l2.number=="0") exit(-1);
    if(l1.number=="0"){
        value.number="0";
        value.symbol=positive;
        return value;        
    }
    Longint a(l1),b(l2);
    a.symbol=b.symbol=positive;
    std::string s="",v="";  //s存商,v存余数
    int La=l1.number.size(),Lb=l2.number.size(),i,tp=La;   //a，b是整形数组表示被除数，除数，tp保存被除数的长度
    int r[La+Lb+2]={0};
    if(La<Lb || (La==Lb && l1.number<l2.number)) {
        value.number="0";
        value.symbol=positive;
        return value;
    }
    int t=La-Lb;   //被除数和除数的位数之差
    b.number.append(t,'0');
    for(int j=0;j<=t;j++) {
        Longint temp;
        while((temp=a-b).symbol==positive||(temp=a-b).number=="0") {  //如果被除数比除数大继续减
            a=temp;
            r[t-j]++;
        }
        if(a.number!="0") a.number+="0";
    }
    for(i=0;i<La+Lb;i++) r[i+1]+=r[i]/10,r[i]%=10;    //统一处理进位
    while(!r[i]) i--;    //将整形数组表示的商转化成字符串表示的
    while(i>=0) s+=char(r[i--]+'0');
    value.number=s;
    return value;
}

Longint operator%(const Longint &l1,const Longint &l2){
    Longint temp;
    temp=l1-(l1/l2)*l2;
    return temp;
}
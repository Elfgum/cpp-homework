#include "Evalvisitor.h"
#include <iomanip>

using std::map;
map<std::string, Data>dict[2002];
map<std::string, Python3Parser::ParametersContext*>fpara;
map<std::string, Python3Parser::SuiteContext*>fcont;
int dep=0;
bool (Data::*func_comp_op[6])(const Data &) const= {&Data::operator<,&Data::operator>,&Data::operator==,&Data::operator>=,&Data::operator<=,&Data::operator!=};
Data (Data::*func_conv[4])() const= {&Data::__longint,&Data::__double,&Data::__string,&Data::__bool};
Data (Data::*func_augass[6])(const Data &)= {&Data::operator+=,&Data::operator-=,&Data::operator*=,&Data::operator/=,&Data::div_eq,&Data::operator%=};
std::string keyword[14]={"None","True","False","def","return","break","continue","if","elif","else","while","or","and","not"};
std::string built_in_func[]={"int","float","str","bool","print"};

Data fetch(std::string name){ 
  if(dict[dep].count(name)) return dict[dep][name];
  if(dict[0].count(name)) return dict[0][name];
}

Data Data::get_value() const{
  if (type&&type!=6) return *this;
  if (type==6) return Data("none",true);
  return fetch(name);
}

void annih(int depth){
  if(!dict[depth].empty()) dict[depth].erase(dict[depth].begin(),dict[depth].end());
}

Data::operator bool() const{
  if (!type) return fetch(name).__bool().b;
  return __bool().b;
}

std::ostream& operator<<(std::ostream& os,Data data){
  if (!data.type) data=fetch(data.name);
  switch (data.type){
    case 1: os<<data.num; break;
    case 2: printf("%.6f",data.d);break;
    case 3: os<<data.s; break;
    case 4: os<<(data.b?"True":"False"); break;
    case 5: os<<"None"; 
  }
  return os;
}

Data get_first(antlrcpp::Any tmp){
  if(tmp.is<Data>()) return tmp.as<Data>();
  if(tmp.is<std::vector<Data>>()) return tmp.as<std::vector<Data>>()[0];
  exit(-1);
}

Data get_first_value(antlrcpp::Any tmp){
  return get_first(tmp).get_value();
}

class EvalVisitor : public Python3BaseVisitor
{
  antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx){
    for (auto stmtele:ctx->stmt()) visit(stmtele);
    return 0;
  }

  antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx){
    std::string nam=ctx->NAME()->toString();
    fpara[nam]=ctx->parameters();
    fcont[nam]=ctx->suite();
    return Data(true);
  }

  antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx){
    if(ctx->typedargslist()) return visit(ctx->typedargslist());
    std::vector<Data> ret;
    return ret;
  }

  antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx){
    std::vector<Data> ret;
    for(int i=0; i<ctx->tfpdef().size()-ctx->test().size(); i++){
      std::string valname=ctx->tfpdef(i)->NAME()->toString();
      ret.push_back(Data(valname,true,true));
    }
    for(int i=ctx->tfpdef().size()-ctx->test().size(); i<ctx->tfpdef().size(); i++){
      std::string valname=ctx->tfpdef(i)->NAME()->toString();
      Data data=visit(ctx->test( i-(ctx->tfpdef().size()-ctx->test().size()) )).as<Data>();
      dict[dep+1][valname]=data;
      ret.push_back(Data(valname,true,true));
    }
    return ret;
  }

  antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx){
    return Data(true);
  }

  antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx){
    if(ctx->simple_stmt()) return visit(ctx->simple_stmt());
    if(ctx->compound_stmt()) return visit(ctx->compound_stmt());
  }

  antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx){
    return visit(ctx->small_stmt());
  }

  antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx){
    if(ctx->expr_stmt()) return visit(ctx->expr_stmt());
    if(ctx->flow_stmt()) return visit(ctx->flow_stmt());
  }

  antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx){
    if(ctx->augassign()) {
      Data lef=(visit(ctx->testlist(0)).as<std::vector<Data>>())[0],rig=(visit(ctx->testlist(1)).as<std::vector<Data>>())[0];
      std::string nam=lef.name;
      lef=lef.get_value(), rig=rig.get_value();
      (lef.*(func_augass[visit(ctx->augassign()).as<int>()])) (rig);
      dict[dep][nam]=lef;
    }else{
      std::vector<Data> assi( visit(ctx->testlist(ctx->testlist().size()-1)).as<std::vector<Data>>() ); 
      for (int i=ctx->testlist().size()-2; i>=0; i--){
        std::vector<Data> tmp( visit(ctx->testlist(i)).as<std::vector<Data>>() );
        for(int j=0; j<assi.size(); j++)
          if (tmp[j].isname()) dict[dep][tmp[j].name]=assi[j].get_value();
      }   
    }
      return Data(true);
  }

  antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx){
    if(ctx->ADD_ASSIGN()) return 0;
    if(ctx->SUB_ASSIGN()) return 1;
    if(ctx->MULT_ASSIGN()) return 2;
    if(ctx->DIV_ASSIGN()) return 3;
    if(ctx->IDIV_ASSIGN()) return 4;
    if(ctx->MOD_ASSIGN()) return 5;
  }

  antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx){
    if(ctx->break_stmt()) return visit(ctx->break_stmt());
    if(ctx->continue_stmt()) return visit(ctx->continue_stmt());
    if(ctx->return_stmt()) return visit(ctx->return_stmt());
  }

  antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx){
    return 111;
  }

  antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx){
    return 112;
  }

  antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx){
    if(ctx->testlist()) {
      auto ret=visit(ctx->testlist()).as<std::vector<Data>>();
      if(ret[0].type==5) return ret;
      for(int i=0;i<ret.size();i++) ret[i]=Data(ret[i].get_value());
      return ret;
    }
    std::vector<Data> ret;
    ret.push_back(Data("None",true));
    return ret;
  }

  antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx){
    if(ctx->if_stmt()) return visit(ctx->if_stmt());
    if(ctx->while_stmt()) return visit(ctx->while_stmt());
    if(ctx->funcdef()) return visit(ctx->funcdef());
  }

  antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx){
    for (int i=0; i<ctx->test().size(); i++){
      if(bool(get_first_value(visit(ctx->test(i))))) return visit(ctx->suite(i));
      //auto tmp=visit(ctx->test(i));
      //if(tmp.is<std::vector<Data>>()) {
      //  if (bool(tmp.as<std::vector<Data>>()[0])) return visit(ctx->suite(i));
      //}else if(bool(tmp.as<Data>())) return visit(ctx->suite(i));
    }
    if (ctx->ELSE()) return visit(ctx->suite(ctx->suite().size()-1));
    return Data(false);
  }

  antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx){
    while (1) {
      //antlrcpp::Any tmp1=visit(ctx->test());
      //if(tmp1.is<std::vector<Data>>()){ 
      //  if(!bool(tmp1.as<std::vector<Data>>()[0])) return Data(true);
      //}else if(!bool(tmp1.as<Data>())) return Data(true);
      if(!bool(get_first_value(visit(ctx->test())))) return Data(true);
      antlrcpp::Any tmp2=visit(ctx->suite());
      if (tmp2.is<int>()&&tmp2.as<int>()==111) return Data(true);
    }
  }

  antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx){
    if(ctx->simple_stmt()) return visit(ctx->simple_stmt());
    for(int i=0; i<ctx->stmt().size(); i++){
      auto tmp=visit(ctx->stmt(i));
      if(tmp.is<int>())
        return (tmp.as<int>()==111?111:112);//111 for break; 112 for continue.
      //if(tmp.is<Data>()&&tmp.as<Data>().type==5) {
        //std::vector<Data> ret00;
        //ret00.push_back(Data("None",true));
        //return ret00;
        //return Data("None",true);
      //}
      if(tmp.is<std::vector<Data>>()) return tmp.as<std::vector<Data>>();
    }
    //std::vector<Data> ret00;
    //ret00.push_back(Data("None",true));
    //return ret00;
    Data _specialret(true);
    _specialret.type=6;
    return _specialret;
  }

  antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) {
    return visit(ctx->or_test());
  }

  antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx){
    if(!ctx->OR().size()) return visit(ctx->and_test()[0]);
    for(int i=0; i<ctx->and_test().size(); i++) 
      if (bool( get_first_value(visit(ctx->and_test(i))) )) return Data(true);
    return Data(false);
  }

  antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx){
    if(!ctx->AND().size()) return visit(ctx->not_test()[0]);
    for(int i=0; i<ctx->not_test().size(); i++)
      if (! bool( get_first_value(visit(ctx->not_test(i))) )) return Data(false);
    return Data(true);
  }

  antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx){
    if(ctx->comparison()) return visit(ctx->comparison());
    Data res=get_first_value(visit(ctx->not_test()));
    res=(bool(res)? Data(false):Data(true));
    return res;
  }

  antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx){
    if(!ctx->comp_op().size()) return visit(ctx->arith_expr(0));
    Data lef=get_first_value(visit(ctx->arith_expr(0)));
    for(int i=0;i<ctx->comp_op().size();i++){
      Data rig=get_first_value(visit(ctx->arith_expr(i+1)));
      if(! ((lef.*( func_comp_op [visit(ctx->comp_op(i)).as<int>()] )) (rig)) ) return Data(false);
      lef=rig;
    }
    return Data(true);
  }

  antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) {
    if (ctx->LESS_THAN()) return 0;
    if (ctx->GREATER_THAN()) return 1;
    if (ctx->EQUALS()) return 2;
    if (ctx->GT_EQ()) return 3;
    if (ctx->LT_EQ()) return 4;
    if (ctx->NOT_EQ_2()) return 5;
  }

  antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx){
    if (ctx->term().size()==1) return visit(ctx->term(0));
    std::vector<std::pair<int,int>>order;//index,type; 1 for +, 2 for -
    for(int i=0;i<ctx->ADD().size();i++) order.push_back(std::make_pair(ctx->ADD()[i]->getSymbol()->getTokenIndex(),1) );
    for(int i=0;i<ctx->MINUS().size();i++) order.push_back(std::make_pair(ctx->MINUS()[i]->getSymbol()->getTokenIndex(),2) );
    std::sort(order.begin(),order.end());
    Data lef=visit(ctx->term(0)).as<Data>().get_value(), rig(false);
    for (int i=0;i<order.size();i++){
      auto atemp=visit(ctx->term(i+1));
      if (atemp.is<std::vector<Data>>()) rig=atemp.as<std::vector<Data>>()[0].get_value();
        else rig=atemp.as<Data>().get_value();
      if(order[i].second==2) lef-=rig;
        else lef+=rig;
    }
    return lef;
  }

  antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx){
    if(ctx->factor().size()==1) return visit(ctx->factor()[0]);
    std::vector<std::pair<int,int>>order;//<index,type>; 1 for *, 2 for /, 3 for //, 4 for %
    order.clear();
    for(int i=0;i<ctx->STAR().size();i++) order.push_back(std::make_pair(ctx->STAR()[i]->getSymbol()->getTokenIndex(),1) );
    for(int i=0;i<ctx->DIV().size();i++) order.push_back(std::make_pair(ctx->DIV()[i]->getSymbol()->getTokenIndex(),2) );
    for(int i=0;i<ctx->IDIV().size();i++) order.push_back(std::make_pair(ctx->IDIV()[i]->getSymbol()->getTokenIndex(),3) );
    for(int i=0;i<ctx->MOD().size();i++) order.push_back(std::make_pair(ctx->MOD()[i]->getSymbol()->getTokenIndex(),4) );
    std::sort(order.begin(),order.end());
    auto tmp=visit(ctx->factor()[0]);
    Data lef(false),rig(false);
    if(tmp.is<Data>()) lef=tmp.as<Data>();
    if(tmp.is<std::vector<Data>>()) lef=tmp.as<std::vector<Data>>()[0];
    if(lef.isname()) lef=fetch(lef.name);
    for(int i=0;i<order.size();i++){
      auto tmp1=visit(ctx->factor()[i+1]);
      if(tmp1.is<Data>()) rig=tmp1.as<Data>();
      if(tmp1.is<std::vector<Data>>()) rig=tmp1.as<std::vector<Data>>()[0];
      if(rig.isname()) rig=fetch(rig.name);
      switch (order[i].second){
        case 1: lef*=rig; break;
        case 2: lef/=rig; break;
        case 3: lef.div_eq(rig); break;
        case 4: lef%=rig;
      }
    }
    return lef;
  }

  antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx){
    if(!ctx->atom_expr()) {
      auto tmp=visit(ctx->factor());
      Data ret("None",true);
      if(tmp.is<Data>()) ret=tmp.as<Data>();
      if(tmp.is<std::vector<Data>>()) ret=tmp.as<std::vector<Data>>()[0];
      if(ret.isname()) ret=fetch(ret.name);
      if(ctx->ADD()) return ret;
      if(ctx->MINUS()) return -ret;
    }
    return visit(ctx->atom_expr());
  }

  antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx){
    if(!ctx->trailer()) return visit(ctx->atom());
    std::string func_call=ctx->atom()->NAME()->toString();
    for(int i=0; i<4; i++) 
      if (func_call==built_in_func[i]) {
        std::vector<Data> arg(visit(ctx->trailer()).as<std::vector<Data>>());
        Data data=arg[0].get_value();
        annih(dep+1);
        return (data.*(func_conv[i]))();
      }
    if(func_call=="print"){ //  "print"   print()是允许的
      auto tmp0=visit(ctx->trailer());
      if(tmp0.is<Data>()&&tmp0.as<Data>().type>=5) {
        std::cout<<"None"<<std::endl;
        return Data(true);
      }
      std::vector<Data> arg(tmp0.as<std::vector<Data>>());
      for (int i=0; i+1<arg.size(); i++) std::cout<<arg[i]<<' ';
      if (arg.size()) {
        std::cout<<arg[arg.size()-1];}
      std::cout<<std::endl;
      annih(dep+1);
      return Data(true);
    }
    if(fpara.count(func_call)){
      std::vector<Data> matchlist=visit(fpara[func_call]).as<std::vector<Data>>();
      std::vector<Data> arg(visit(ctx->trailer()).as<std::vector<Data>>());
      int i=0,j=0; dep++;
      while(j<arg.size()){
        if(arg[j].isname()) break;
        dep--;
        dict[dep+1][matchlist[i].name]=arg[j];
        dep++, i++, j++;
      }
      std::vector<Data> ret;
      antlrcpp::Any atmp=visit(fcont[func_call]);
      if(atmp.is<std::vector<Data>>())
        ret=atmp.as<std::vector<Data>>();
        else ret.push_back(Data("None",true));
      annih(dep);
      dep--;
      return ret;
    }
  }//内置函数已写好

  antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx){
    if(ctx->arglist()) return visit(ctx->arglist());
    std::vector<Data> ret;//无参数
    return ret;
  }//返回 vector<Data>

  antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx){
    if(ctx->NONE()) return Data("NONE",true);
    if(ctx->TRUE()) return Data(true);
    if(ctx->FALSE()) return Data(false);
    if(ctx->NAME()) return Data(ctx->NAME()->toString(),true,true);
    if(ctx->NUMBER()) {
      Data ret(ctx->NUMBER()->toString());
      if(ret.s.find('.')!=std::string::npos) {
        ret.type=2;
        sscanf(ret.s.c_str(),"%lf",&ret.d);
      }
      else{
        ret.type=1;
        ret.num=ret.s;
      }
      return ret;
    }
    if(ctx->STRING().size()) {
      Data ret("");
      for (int i=0; i<ctx->STRING().size(); i++) ret+=Data(ctx->STRING(i)->toString().substr(1,ctx->STRING(i)->toString().size()-2));
      return ret;
    }
    if(ctx->test()) return visit(ctx->test());
  }

  antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx){
    std::vector<Data> ret;
    for (int i=0; i<ctx->test().size(); i++) {
      auto tmp=visit(ctx->test()[i]);
      if (tmp.is<std::vector<Data>>())
        for(int i=0;i<tmp.as<std::vector<Data>>().size();i++) ret.push_back(tmp.as<std::vector<Data>>()[i]);///return a,foo(b)
      else ret.push_back( tmp.as<Data>() );
    }
    return ret;
  }//返回 vector<Data>

  antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx){
    std::vector<Data> ret;
    for (auto argus:ctx->argument()) {
      auto tmp=visit(argus);
      if(tmp.is<std::vector<Data>>()) 
        for(int i=0;i<tmp.as<std::vector<Data>>().size();i++) ret.push_back(tmp.as<std::vector<Data>>()[i]);//print(a,foo(b),c)
      else ret.push_back(tmp.as<Data>());
    }
    return ret;
  }

  antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx){
    if (!ctx->NAME()) {
      auto tmp=visit(ctx->test());
      if(tmp.is<std::vector<Data>>()) return tmp.as<std::vector<Data>>(); 
      return tmp.as<Data>().get_value();
    }
    std::string valname=ctx->NAME()->toString();
    dict[dep+1][valname]=visit(ctx->test()).as<Data>();
    return Data(valname,true,true);
  }
};

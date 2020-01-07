#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include "aData.cpp"
#include "Python3BaseVisitor.h"
#include <vector>
#include <map>

Data fetch(std::string name);
void annih(int depth);
Data get_first(antlrcpp::Any tmp);
Data get_first_value(antlrcpp::Any tmp);

#endif //PYTHON_INTERPRETER_EVALVISITOR_H

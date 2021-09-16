#include <iostream>
#include <vector>
#include "interface.hpp"

using namespace std;

class interface {
    public:
        virtual ~interface(){cout << "DESTRUCTOR interface RAN" << endl;};
};

class classA : public interface {
    public:
        ~classA(){cout << "DESTRUCTOR classA RAN" << endl;};
};

class Str {
    public:
        string a;
        int size;
};

int main() {
    cout << "hello world" << endl;

    std::vector<string>* prepend = new std::vector<string>();
    prepend->push_back("driver");
    prepend->push_back("serial");

    std::vector<string> command;
    command.push_back("serial");
    command.push_back("-p");
    command.push_back("1");

    command[0] = prepend->at(0);
    command.insert(command.begin()+1, prepend->begin()+1, prepend->end());

    for (int x=0; x<command.size();x++) {
        cout << command[x] << endl;
    }

    cout << "---" << endl;
}
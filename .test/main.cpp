#include <iostream>
#include "interface.hpp"
#include <vector>

using namespace std;

class interface {
    public:
        virtual void f1() = 0;
        virtual void f2(){cout << "f2 - predef" << endl;};
};

class classA : public interface {
    public:
        class classB {
            public:
                string lol = "jup";
        };

        void f1() {cout << "f1 - yay" << endl;};
        void f2() {cout << "f2 - yay" << endl;};
        string test = "hello";
        classA copy(){return *this;};
};

classA foo() {
    classA origin;
    cout << origin.test << endl;
    classA copy = origin.copy();
    origin.test = "awdiawbd";
    cout << origin.test << endl;
    cout << copy.test << endl;
    return copy;
}

int main() {
    cout << "hello world" << endl;

    auto v = new vector<int>();
    v->push_back(1);
    v->push_back(2);
    v->push_back(3);
    v->push_back(4);
    
    v->erase(v->begin() + 1);
    v->operator[](1) = 5;

    for (int i=0;i<v->size();i++) {
        cout << v->at(i) << endl;
    }

    cout << foo().test << endl;

    cout << "---" << endl;
}
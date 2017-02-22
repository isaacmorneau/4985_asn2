#include "testset.h"
#include <vector>
#include <string>
#include <tuple>
#include <chrono>

using namespace std;

TestSet* TestSet::instance;

TestSet *TestSet::getTestSets(){
    if(TestSet::instance == 0)
        TestSet::instance = new TestSet();
    return TestSet::instance;
}

void TestSet::destroyTestSets(){
    if(TestSet::instance){
        delete TestSet::instance;
    }
}

TestSet::TestSet()
{
    TestSet::instance = 0;
}

void TestSet::addToTest(double sent, double lost, double size){
    newest = chrono::high_resolution_clock::now();
    auto tup = tests.back();
    sent += get<0>(tup);
    lost += get<1>(tup);
    size += get<2>(tup);
    tuple<int,int,int,string,timedur> updated = make_tuple(sent, lost, size, get<3>(tup),
                                   chrono::duration_cast<chrono::microseconds>(newest-start));
    tests.pop_back();
    tests.push_back(updated);

}

void TestSet::newTest(string protocol){
    tests.push_back(make_tuple(0, 0, 0, protocol, chrono::milliseconds{0}));
    start = chrono::high_resolution_clock::now();
}

void TestSet::extractSets(QtCharts::QBarSet *outTotal, QtCharts::QBarSet *outLost, QtCharts::QBarSet *outSize){
    //if theres no real data dont try and make empty sets
    if(!size() || (size() == 1 && get<0>(tests.at(0)) == 0))
        return;
    for(auto i = tests.begin(); i != tests.end(); ++i){
        *outTotal << get<0>(*i);
        *outLost << get<1>(*i);
        *outSize << get<2>(*i);
    }
}

tuple<int,int,int,string,timedur> TestSet::at(int index){
    return tests.at(index);
}

int TestSet::size(){
    return tests.size();
}

void TestSet::clear(){
    tests.clear();
}

void TestSet::inputCode(const string& code){
    int read = 0, sent, psize, tmp;
    char buffer[256];
    for(int i = 0, j = 0, sz = size(); i < static_cast<int>(code.size()) && j < sz ; i += read , j++){
        sscanf(&code.c_str()[i],"%X %X ",&sent, &psize);
        sprintf(buffer,"%X %X ",sent, psize);
        read = strlen(buffer);

        if(get<3>(tests[j]) == string("TCP")){
            if((tmp = get<0>(tests[j])) > sent)
                get<1>(tests[j]) = tmp - sent;
        } else {
            if((tmp = get<0>(tests[j])) < sent)
                get<1>(tests[j]) = sent - tmp;
        }
    }
}

string TestSet::outputCode(){
    if(!size())
        return "";
    char buff[1024];
    for(int i = 0, j = 0, sz = size(); i < 1024 && j < sz ; i = strlen(buff) , j++){
        sprintf(&buff[i],"%X %X ",get<0>(tests[j]), get<2>(tests[j]));
    }
    return string(buff);
}

#include "testset.h"
#include <vector>
#include <string>
#include <tuple>

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
    auto tup = tests.back();
    sent += std::get<0>(tup);
    lost += std::get<1>(tup);
    size += std::get<2>(tup);
    auto updated = std::make_tuple(sent, lost, size, std::get<3>(tup));
    tests.pop_back();
    tests.push_back(updated);

}

void TestSet::newTest(std::string protocol){
    tests.push_back(std::make_tuple(0, 0, 0, protocol));
}

void TestSet::extractSets(QtCharts::QBarSet *outTotal, QtCharts::QBarSet *outLost, QtCharts::QBarSet *outSize){
    //if theres no real data dont try and make empty sets
    if(!size() || (size() == 1 && std::get<0>(tests.at(0)) == 0))
        return;
    for(auto i = tests.begin(); i != tests.end(); ++i){
        *outTotal << std::get<0>(*i);
        *outLost << std::get<1>(*i);
        *outSize << std::get<2>(*i);
    }
}

std::tuple<int,int,int,std::string> TestSet::at(int index){
    return tests.at(index);
}

int TestSet::size(){
    return tests.size();
}

void TestSet::clear(){
    tests.clear();

}

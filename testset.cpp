#include "testset.h"
#include <vector>

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

void TestSet::addToTest(double sent, double lost, double total){
    if(!size()){
        newTest();
    }
    double tmp = sent + testsSent.back();
    testsSent.pop_back();
    testsSent.push_back(tmp);

    tmp = lost + testsLost.back();
    testsLost.pop_back();
    testsLost.push_back(tmp);

    tmp = total + testsSize.back();
    testsSize.pop_back();
    testsSize.push_back(tmp);
}

void TestSet::newTest(){
    testsSent.push_back(0);
    testsLost.push_back(0);
    testsSize.push_back(0);
}

void TestSet::extractSets(QtCharts::QBarSet *outTotal, QtCharts::QBarSet *outLost, QtCharts::QBarSet *outSize){
    if(!size()){
        newTest();
    }
    for(auto i = testsSent.begin(); i != testsSent.end(); ++i)
        *outTotal << *i;
    for(auto i = testsLost.begin(); i != testsLost.end(); ++i)
        *outLost << *i;
    for(auto i = testsSize.begin(); i != testsSize.end(); ++i)
        *outSize << *i;
}

int TestSet::size(){
    return testsSent.size();
}

void TestSet::clear(){
    testsLost.clear();
    testsSent.clear();
    testsSize.clear();
}

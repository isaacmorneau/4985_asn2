#ifndef TESTSET_H
#define TESTSET_H
#include <vector>
#include <string>
#include <chrono>
#include <QtCharts/QBarSet>

typedef std::chrono::time_point<std::chrono::system_clock> timepnt;
typedef std::chrono::microseconds timedur;

class TestSet
{
public:
    static TestSet *getTestSets();
    static void destroyTestSets();
    static TestSet *instance;

    void addToTest(double sent, double lost, double size);
    void newTest(std::string protocol);
    void extractSets(QtCharts::QBarSet *outTotal, QtCharts::QBarSet *outLost, QtCharts::QBarSet *outSize);
    int size();
    void clear();
    std::tuple<int,int,int,std::string,timedur> at(int index);
private:
    //total sent, total lost, total size
    std::vector<std::tuple<int,int,int,std::string, timedur>> tests;
    timepnt start, newest;
    TestSet();
};

#endif // TESTSET_H

#pragma once
#include <iomanip>
#include <fstream>
#include <ostream>
using ostream = std::basic_ostream<char, std::char_traits<char>>;

// Write same data to two ostreams
class Log
{
public:
    Log(ostream& os_one, std::string logPath)
        : os1(os_one), os2(logFile) {
        logFile.open(logPath);
    }

    ~Log() {
        logFile.close();
    }

    ostream& os1;
    ostream& os2;
private:
    std::ofstream logFile;
};

// For data: int, long , ...
template <class Data>
static Log& operator<<(Log& osf, Data d)
{
    osf.os1 << d;
    osf.os2 << d;
    return osf;
}
// For manipulators: endl, flush
static Log& operator<<(Log& osf, ostream& (*f)(ostream&))
{
    osf.os1 << f;
    osf.os2 << f;
    return osf;
}

// For setw() , ...
template<class ManipData>
static Log& operator<<(Log& osf, ostream& (*f)(ostream&, ManipData))
{
    osf.os1 << f;
    osf.os2 << f;
    return osf;
}

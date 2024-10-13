#pragma once
#include <string>
#include <fstream>
#include "../dp&&ds/singleton/singleton_template.h"
using namespace std;

namespace logger{

#define debug(format, ...) \
    single::Singleton<Logger>::instance()->log(Logger::DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define info(format, ...) \
    single::Singleton<Logger>::instance()->log(Logger::INFO, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define warn(format, ...) \
    single::Singleton<Logger>::instance()->log(Logger::WARN, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define error(format, ...) \
    single::Singleton<Logger>::instance()->log(Logger::ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define fatal(format, ...) \
    single::Singleton<Logger>::instance()->log(Logger::FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__);


class Logger{
    friend class single::Singleton<Logger>;
public:
    enum Level{
        DEBUG = 0,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT
    };
    void openFile(const string& fimename);
    void closeFile();
    void setLevel(Level level);
    void setFileSize(int size);
    void log(Level level, const char* file, int line, const char* format, ...);
private:
    Logger();
    ~Logger();
    void rotate();
private:
    string filename_;
    ofstream fout_;
    Level level_;
    int fileSize_ = 0;
    int len_ = 0;
    static const char* level2str_[LEVEL_COUNT];
};
} // namespace logger
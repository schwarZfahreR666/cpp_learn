#include "Logger.h"
#include <time.h>
#include <stdexcept>
#include <stdarg.h>

using namespace logger;

const char* Logger::level2str_[Logger::LEVEL_COUNT] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

Logger::Logger(){

}

Logger::~Logger(){
    closeFile();
}

void Logger::openFile(const string& filename){
    filename_ = filename;
    fout_.open(filename, ios::app);
    if(fout_.fail()){
        throw std::logic_error("open file failed: " + filename);
    }

    //获取刚打开文件的长度
    fout_.seekp(0, ios::end);
    len_ = fout_.tellp();
}

void Logger::closeFile(){
    fout_.close();
}

void Logger::setLevel(Logger::Level level){
    level_ = level;
}
void Logger::setFileSize(int size){
    fileSize_ = size;
}

void Logger::rotate(){
    closeFile();
    time_t tick = time(NULL);
    struct tm* time = localtime(&tick);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), ".%Y-%m-%d_%H:%M:%S", time);
    string filename = filename_ + timestamp;
    //这里有bug:如果打印过快，时间戳精度到秒无法区分文件，会导致覆盖。最好搞个递增id
    if(rename(filename_.c_str(), filename.c_str()) != 0){
        throw std::logic_error("rename log file failed");
    }

    openFile(filename_);

}

void Logger::log(Level level, const char* file, int line, const char* format, ...){
    if(level < level_) return;

    if(fout_.fail()){
        throw std::logic_error("open file failed: " + filename_);
    }

    time_t tick = time(NULL);
    struct tm* time = localtime(&tick);
    char timestamp[32] = {0};
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time);

    const char* fmt = "%s [%s] <%s:%d>: ";
    //获取字符串长度到size
    int size = snprintf(NULL, 0, fmt, timestamp, level2str_[level], file, line);
    if(size > 0){
        char* buffer = new char[size + 1];
        snprintf(buffer, size + 1, fmt, timestamp, level2str_[level], file, line);
        buffer[size] = '\0';
        fout_ << buffer;
        len_ += size;
        delete[] buffer;
    }

    va_list args;
    va_start(args, format);
    size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    if(size > 0){
        char* content = new char[size + 1];
        va_start(args, format);
        vsnprintf(content, size + 1, format, args);
        va_end(args);
        content[size] = '\0';
        len_ += size;
        fout_ << content << std::endl;
        delete[] content;
    }
    //刷盘
    fout_.flush();

    if (fileSize_ > 0 && len_ >= fileSize_){
        rotate();
    }
}
#include "Logger.h"
using namespace logger;
int main(){
    char* content = "logger";
    auto logger = single::Singleton<logger::Logger>::instance();
    logger->openFile("./test.log");
    // logger->log(Logger::INFO, __FILE__, __LINE__, "hello:%s", content);
    logger->setLevel(Logger::INFO);
    logger->setFileSize(1024);
    for(int i=0; i < 10 ; i++){
        debug("ok ok %s", content);
        info("ok ok %s", content);
        warn("ok ok %s", content);
        error("ok ok %s", content);
        fatal("ok ok %s", content);
    }
    logger->closeFile();
}
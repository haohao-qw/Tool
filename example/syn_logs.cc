#include "iostream"
#include "syn_log.hpp"
int main(){
    Logger::ptr logger= LOG_NAME("test");
    LogFormatter::ptr formatter(new LogFormatter);
    LogAppender::ptr appender(new FileLogAppender("../tmp/log/syn_log.txt"));
    appender->setFormatter(formatter);
    logger->addAppender(appender);
    logger->setLevel(LogLevel::NOTSET);
    for(int i=0;i<1000000;i++) {
        LOG_FMT_DEBUG(logger, "fatal %s: %d", __FILE__, __LINE__);
        LOG_FATAL(logger) << "FATAL is ok";
        LOG_ERROR(logger) << "error is ok";
        LOG_NOTICE(logger) << "notice is ok";
        LOG_DEBUG(logger) << "debug is ok";
    }
    Logger::ptr root=LOG_ROOT();
    LogFormatter::ptr  log(new LogFormatter);
    LogAppender::ptr rappender(new StdoutLogAppender);
    rappender->setFormatter(log);
    root->addAppender(rappender);
    root->setLevel(LogLevel::NOTICE);///
    LOG_ERROR(root)<<"root error is ok";
    LOG_DEBUG(root)<<"root debug is ok";
    LOG_FATAL(root)<<"root fatal is ok";
    LOG_WARN(root)<<"root warn is ok";


    return 0;
}
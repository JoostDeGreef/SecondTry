#include "Log.h"

#include <iostream>
 
Logger & Logger::Get()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{}

void Logger::SyncCOut::Output(
            const Level level,
            const std::string & file,
            const int line,
            const std::string & message)
{
    std::cout << message << std::endl;
}

void Logger::SyncCErr::Output(
            const Level level,
            const std::string & file,
            const int line,
            const std::string & message)
{
    std::cerr << message << std::endl;
}

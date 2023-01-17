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
            const std::source_location& location,
            const std::string & message)
{
    std::cout << message;
}

void Logger::SyncCErr::Output(
            const Level level,
            const std::source_location& location,
            const std::string & message)
{
    std::cerr << message;
}

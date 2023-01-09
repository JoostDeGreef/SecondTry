#pragma once

#include <array>
#include <vector>
#include <memory>
#include <mutex>

#include "fmt/core.h"

// C++ 20 would allow somethng like this:
// #include <iostream>
// #include <string_view>
// #include <source_location>
// 
// void Log(std::string_view message,
//          const std::source_location& location = std::source_location::current())
// {
//     std::cout << "info:"
//               << location.file_name() << ":"
//               << location.line() << ":"
//               << location.function_name() << " "
//               << message << '\n';
// }

class Logger
{
public:
    enum class Level
    {
        Debug   = 0,
        Info    = 1,
        Warning = 2,
        Error   = 3
    };

    class Sync
    {
    public:
        virtual void Output(
            const Level level,
            const std::string & file,
            const int line,
            const std::string & message) {}
    };

    class SyncCOut : public Sync
    {
    public:
        void Output(
            const Level level,
            const std::string & file,
            const int line,
            const std::string & message) override;
    };

    class SyncCErr : public Sync
    {
    public:
        void Output(
            const Level level,
            const std::string & file,
            const int line,
            const std::string & message) override;
    };

    template<typename SYNC, typename ...ARGS>
    static void AddSync(
        const Level level,
        ARGS & ... args)
    {
        Logger & logger = Get();
        std::unique_lock<std::mutex> lock(logger.m_mutex);
        logger.m_syncs[static_cast<int>(level)].emplace_back(std::make_shared<SYNC>(args...));
    }

    template<typename ...ARGS>
    static void Output(
        const Level level,
        const std::string & file,
        const int line,
        const std::string & message,
        ARGS & ... args)
    {
        Logger & logger = Get();
        std::unique_lock<std::mutex> lock(logger.m_mutex);
        auto & syncs = logger.m_syncs[static_cast<int>(level)];
        if(!syncs.empty())
        {
            std::string formattedMessage = fmt::format(message,args...);
            for(auto & sync:syncs)
            {
                sync->Output(level,file,line,formattedMessage);
            }
        }
    }

private:
    Logger();
    static Logger & Get();

    std::array<std::vector<std::shared_ptr<Sync>>, 4> m_syncs;
    std::mutex m_mutex;
};

#define Log(level,msg...) Logger::Output(level,__FILE__,__LINE__,msg);
#define LogDebug(msg...) Log(Logger::Level::Debug,msg)
#define LogInfo(msg...) Log(Logger::Level::Info,msg)
#define LogWarning(msg...) Log(Logger::Level::Warning,msg)
#define LogError(msg...) Log(Logger::Level::Error,msg)


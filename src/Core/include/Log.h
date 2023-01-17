#pragma once

#include <array>
#include <vector>
#include <memory>
#include <mutex>

#include "fmt/core.h"

#include <iostream>
#include <string_view>
#include <source_location>
 
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
            const std::source_location& location,
            const std::string & message) {}
    };

    class SyncCOut : public Sync
    {
    public:
        void Output(
            const Level level,
            const std::source_location& location,
            const std::string & message) override;
    };

    class SyncCErr : public Sync
    {
    public:
        void Output(
            const Level level,
            const std::source_location& location,
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
        const std::source_location& location,
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
                sync->Output(level,location,formattedMessage);
            }
        }
    }

private:
    Logger();
    static Logger & Get();

    std::array<std::vector<std::shared_ptr<Sync>>, 4> m_syncs;
    std::mutex m_mutex;
};

#define Log(level,msg,...) Logger::Output(level,std::source_location::current(), msg, __VA_ARGS__);
#define LogDebug(msg,...) Log(Logger::Level::Debug, msg, __VA_ARGS__)
#define LogInfo(msg,...) Log(Logger::Level::Info, msg, __VA_ARGS__)
#define LogWarning(msg,...) Log(Logger::Level::Warning, msg, __VA_ARGS__)
#define LogError(msg,...) Log(Logger::Level::Error, msg, __VA_ARGS__)


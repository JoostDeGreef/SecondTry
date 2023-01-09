#include "gtest/gtest.h"
#include "Log.h"

using namespace std;
using namespace testing;

class SyncTest : public Logger::Sync
{
public:
    SyncTest(std::vector<std::string> & storage)
        : m_storage(storage)
    {}

    void Output(
        const Logger::Level level,
        const std::string & file,
        const int line,
        const std::string & message) override
    {
        m_storage.emplace_back(message);
    }
private:
    std::vector<std::string> & m_storage;
};

class LogTest : public Test
{
protected:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    std::vector<std::string> m_storage_debug;
    std::vector<std::string> m_storage_info;
};

TEST_F(LogTest, Debug)
{
    Logger::AddSync<SyncTest>(Logger::Level::Debug, m_storage_debug);
    LogDebug("hello world - {}", "debug");
    EXPECT_EQ(1,m_storage_debug.size());
    EXPECT_EQ(0,m_storage_info.size());
}

TEST_F(LogTest, Info)
{
    Logger::AddSync<SyncTest>(Logger::Level::Info, m_storage_info);
    LogInfo("hello world - info");
    EXPECT_EQ(0,m_storage_debug.size());
    EXPECT_EQ(1,m_storage_info.size());
}

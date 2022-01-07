#include "Log.h"
#include "UI.h"

void InitializeLogger()
{
    Logger::AddSync<Logger::SyncCOut>(Logger::Level::Debug);
    Logger::AddSync<Logger::SyncCOut>(Logger::Level::Info);
    Logger::AddSync<Logger::SyncCErr>(Logger::Level::Warning);
    Logger::AddSync<Logger::SyncCErr>(Logger::Level::Error);
    LogInfo("Logger Initialized\n");
}

int main(int argc, char* argv[])
{
    InitializeLogger();
    UI ui;
    return ui.Run();
}

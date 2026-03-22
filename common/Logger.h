#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <ctime>
#include <memory>
#include <atomic>
#include "SharedVector.h"

class Logger {
    SharedVector<std::string> m_vecSharedLogQ;
    std::atomic<bool> m_Exit;
    int m_process_delay_in_ns; // delay for processing in nano seconds
    int m_nLogLevel; // delay for processing in nano seconds

    void ProcessQ() {
        do {
            if (m_vecSharedLogQ.size() > 0) {
                // module is processing log data from Q 

                loginternal(m_vecSharedLogQ.pop_front());

            }

            std::this_thread::sleep_for(std::chrono::nanoseconds(m_process_delay_in_ns));
        } while (!m_Exit);
    }
public:
    enum Level {    ERRR,
                    WARNING,
                    INFO,
                    EXTRAINFO};
     

    // Get the Singleton instance
    static Logger* getInstance() {
        if (m_pInstance == nullptr) 
            m_pInstance = new Logger();

        return m_pInstance;
    }

    /*void InIt(IniReader* pReader) {


        std::string filename = "";

        filename = pReader->getstring("General", "LogFile", "logfile.log");

        if (filename[filename.size() - 1] == '//')
            filename += "logfile.log";


        m_process_delay_in_ns = pReader->getint("General", "ProcessDelayInNS", 500);
        if (m_process_delay_in_ns < 500)
            m_process_delay_in_ns = 500; // minimum delay should be 500ns
        
        m_nLogLevel = pReader->getint("General", "LogLevel", 2);


        logFile.open(filename, std::ios::app); // Append mode
        if (!logFile) {
            std::cerr << "Logger: Failed to open log file!" << std::endl;
        }

        std::thread LoggerThread([&]() {
            ProcessQ();
            });

        LoggerThread.detach();  // ✅ Now runs independently
    }*/

   
    void InIt() {

        std::string filename = "logs";

        if (filename[filename.size() - 1] == '//') {
            std::string str_curtime = getCurrentTime();
            filename += str_curtime + "_LOG.log"; 
        }

        m_process_delay_in_ns = 500; // minimum delay should be 500ns
        
        m_nLogLevel = 2;


        logFile.open(filename, std::ios::app); // Append mode
        if (!logFile) {
            std::cerr << "Logger: Failed to open log file!" << std::endl;
        }

        std::thread LoggerThread([&]() {
            ProcessQ();
            });

        LoggerThread.detach();  // ✅ Now runs independently
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // Log message with level
    void log(Level level, const std::string& message) {
        std::string logEntry = formatLogEntry(level, message);

        // Print to console
        if(m_nPrintToConsole)
            std::cout << logEntry << std::endl;

        if(m_nLogLevel <= level)
            m_vecSharedLogQ.push_back(logEntry);
    }


    void loginternal(const std::string& message) {

        // Write to file
        logFile << message << std::endl;
        logFile.flush();
    }

    // Convenience methods
    void info(const std::string& message)    { log(Level::INFO, message); }
    void warning(const std::string& message) { log(Level::WARNING, message); }
    void error(const std::string& message)   { log(Level::ERRR, message); }

    void SetPrintToConsole(bool val) { m_nPrintToConsole = val; }
    void SetExit(bool val) { m_Exit = val; }

private:
    bool m_nPrintToConsole;
    static Logger* m_pInstance;  // Smart pointer to manage instance
    std::ofstream logFile;

    // Private constructor for Singleton pattern
    explicit Logger() : m_nPrintToConsole(0), m_Exit(0) , m_nLogLevel(2){
    }

    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Get current timestamp
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        char buffer[20];

         
        struct tm timeInfo;

        // Use localtime_s (safe version of localtime)
        localtime_r( &time, &timeInfo);

        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return std::string(buffer);
    }

    // Format log entry
    std::string formatLogEntry(Level level, const std::string& message) {
        std::string levelStr;
        switch (level) {
            case Level::EXTRAINFO:
            case Level::INFO:    levelStr = "[INFO] "; break; 
            case Level::WARNING: levelStr = "[WARNING] "; break;
            case Level::ERRR:   levelStr = "[ERROR] "; break;
        }
        return getCurrentTime() + " " + levelStr + message;
    }
};


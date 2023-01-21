#pragma once
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include "Core/Log.hpp"

namespace App::Debug {

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult
    {
        std::string name;
        FloatingPointMicroseconds start;
        std::chrono::microseconds elapsedTime;
        std::thread::id threadOd;
    };

    struct InstrumentationSession
    {
        const std::string name;
        explicit InstrumentationSession(std::string name) : name(std::move(name))
        {}
    };

    class Instrumentor
    {
    public:
        Instrumentor(const Instrumentor&) = delete;
        Instrumentor(Instrumentor&&) = delete;
        Instrumentor& operator=(Instrumentor other) = delete;
        Instrumentor& operator=(Instrumentor&& other) = delete;

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            std::lock_guard lock(m_mutex);

            if(m_currentSession != nullptr)
            {
                // If there is already a current session, then close it before beginning new one.
                // Subsequent profiling output meant for the original session will end up in the
                // newly opened session instead.  That's better than having badly formatted
                // profiling output.
                APP_ERROR("Instrumentor::begin_session('{0}') when session '{1}' already open.",
                          name,
                          m_currentSession->name);
                InternalEndSession();
            }
            m_outputStream.open(filepath);

            if(m_outputStream.is_open())
            {
                m_currentSession = std::make_unique<InstrumentationSession>(name);
                WriteHeader();
            }
            else
            {
                APP_ERROR("Instrumentor could not open results file '{0}'.", filepath);
            }
        }

        void EndSession()
        {
            std::lock_guard lock(m_mutex);
            InternalEndSession();
        }

        void WriteProfile(const ProfileResult& result)
        {
            std::stringstream json;

            std::string name{result.name};
            std::replace(name.begin(), name.end(), '"', '\'');

            json << std::setprecision(3) << std::fixed;
            json << ",{";
            json << R"("cat":"function",)";
            json << "\"dur\":" << (result.elapsedTime.count()) << ',';
            json << R"("name":")" << name << "\",";
            json << R"("ph":"X",)";
            json << "\"pid\":0,";
            json << R"("tid":")" << result.threadOd << "\",";
            json << "\"ts\":" << result.start.count();
            json << "}";

            std::lock_guard lock(m_mutex);
            if(m_currentSession != nullptr)
            {
                m_outputStream << json.str();
                m_outputStream.flush();
            }
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:
        Instrumentor() : m_currentSession(nullptr)
        {}

        ~Instrumentor()
        {
            EndSession();
        }

        void WriteHeader()
        {
            m_outputStream << R"({"otherData": {},"traceEvents":[{})";
            m_outputStream.flush();
        }

        void WriteFooter()
        {
            m_outputStream << "]}";
            m_outputStream.flush();
        }

        // Note: you must already own lock on m_Mutex before
        // calling InternalEndSession()
        void InternalEndSession()
        {
            if(m_currentSession != nullptr)
            {
                WriteFooter();
                m_outputStream.close();
            }
        }

        std::mutex m_mutex;
        std::unique_ptr<InstrumentationSession> m_currentSession;
        std::ofstream m_outputStream;
    };

    class InstrumentationTimer
    {
    public:
        explicit InstrumentationTimer(std::string name)
                : m_name(std::move(name)),
                  m_startTimePoint(std::chrono::steady_clock::now())
        {}

        InstrumentationTimer(const InstrumentationTimer&) = delete;
        InstrumentationTimer(InstrumentationTimer&&) = delete;
        InstrumentationTimer& operator=(InstrumentationTimer other) = delete;
        InstrumentationTimer& operator=(InstrumentationTimer&& other) = delete;

        ~InstrumentationTimer()
        {
            if(!m_stopped)
            {
                Stop();
            }
        }

        void Stop()
        {
            const auto end_time_point{std::chrono::steady_clock::now()};
            const auto highResStart{FloatingPointMicroseconds{m_startTimePoint.time_since_epoch()}};
            const auto elapsedTime{
                    std::chrono::time_point_cast<std::chrono::microseconds>(end_time_point).time_since_epoch()
                    - std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch()
            };

            Instrumentor::Get().WriteProfile(
                    {m_name, highResStart, elapsedTime, std::this_thread::get_id()});

            m_stopped = true;
        }

    private:
        const std::string m_name;
        bool m_stopped{false};
        const std::chrono::time_point<std::chrono::steady_clock> m_startTimePoint;
    };

}

#if APP_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || \
    (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
// There is an implicit decay of an array into a pointer.
// NOLINTNEXTLINE
#define APP_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define APP_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define APP_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || \
    (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define APP_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define APP_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define APP_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define APP_FUNC_SIG __func__
#else
#define APP_FUNC_SIG "APP_FUNC_SIG unknown!"
#endif

#define JOIN_AGAIN(x, y) x##y
#define JOIN(x, y) JOIN_AGAIN(x, y)
#define APP_PROFILE_BEGIN_SESSION(name) ::App::Debug::Instrumentor::Get().BeginSession(name)
#define APP_PROFILE_BEGIN_SESSION_WITH_FILE(name, filePath) \
  ::App::Debug::Instrumentor::Get().BeginSession(name, filePath)
#define APP_PROFILE_END_SESSION() ::App::Debug::Instrumentor::Get().EndSession()
#define APP_PROFILE_SCOPE(name)                                    \
  const ::App::Debug::InstrumentationTimer JOIN(timer, __LINE__) { \
    name                                                           \
  }
#define APP_PROFILE_FUNCTION() APP_PROFILE_SCOPE(APP_FUNC_SIG)
#else
#define APP_PROFILE_BEGIN_SESSION(name)
#define APP_PROFILE_BEGIN_SESSION_WITH_FILE(name, filePath)
#define APP_PROFILE_END_SESSION()
#define APP_PROFILE_SCOPE(name)
#define APP_PROFILE_FUNCTION()
#endif

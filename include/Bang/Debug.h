#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <sstream>
#include <iostream>

#include "Bang/String.h"
#include "Bang/StreamOperators.h"

NAMESPACE_BANG_BEGIN

FORWARD class Shader;

class Debug
{
public:
    static void Log(const String &str, int line, const String &fileName);
    static void Warn(const String &str, int line, const String &fileName);
    static void Error(const String &str, int line, const String &fileName);

    static void PrintUniforms(Shader *shader);
    static void PrintUniforms(uint shaderProgramId);
    static void PrintAllUniforms();
    static void PrintUniforms();

private:
    Debug();

    static const String c_logPrefix;
    static const String c_warnPrefix;
    static const String c_errorPrefix;
};

#define Debug_Log(msg) do{\
    std::ostringstream log;\
    log << std::boolalpha << msg;\
    log.flush();\
    Debug::Log(log.str(), __LINE__, __FILE__); \
} while (0)

#ifdef DEBUG
#define Debug_DLog(msg) Debug_Log(msg)
#else
#define Debug_DLog(msg)
#endif

#define Debug_Peek(varName) Debug_Log(#varName << ": " << (varName))

#define Debug_Warn(msg) do{\
    std::ostringstream log;\
    log << std::boolalpha << msg;\
    log.flush();\
    Debug::Warn(log.str(), __LINE__, __FILE__); \
} while (0)

#define Debug_Error(msg) do{\
    std::ostringstream log;\
    log << std::boolalpha << msg;\
    log.flush();\
    Debug::Error(log.str(), __LINE__, __FILE__); \
} while (0)

NAMESPACE_BANG_END

#endif // DEBUG_H

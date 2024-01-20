#pragma once
#include <queue>
#include <vector>
#include <string>
#include <bitset>
#include <memory>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <unordered_map>

// include glew
#define GLEW_STATIC
#include <GL/glew.h>

// include entt
#include <entt/entt.hpp>

// include glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

// include spdlog
#define FMT_HEADER_ONLY 
#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// import, export
#ifdef EMPY_EXPORT
    #ifdef _MSC_VER
        #define EMPY_API __declspec(dllexport)
    #else
        #define EMPY_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define EMPY_API __declspec(dllimport)
    #else
        #define EMPY_API
    #endif
#endif

// runtime assertion
#define EMPY_ASSERT assert

// compile assertion
#if defined(__clang__) || defined(__gcc__)
    #define EMPY_STATIC_ASSERT _Static_assert
#else
    #define EMPY_STATIC_ASSERT static_assert
#endif

// function inlining
#if defined(__clang__) || defined(__gcc__)
    #define EMPY_INLINE __attribute__((always_inline)) inline
    #define EMPY_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define EMPY_INLINE __forceinline
    #define EMPY_NOINLINE __declspec(noinline)
#else
    #define EMPY_INLINE inline
    #define EMPY_NOINLINE
#endif

// core features
namespace Empy
{
    // entity identifier
    using EntityID = entt::entity;
    // null entity
    constexpr EntityID NENTT = entt::null;  
    // entity registry
    using EntityRegistry = entt::registry;

    // runtime type
    template <typename T>
    EMPY_INLINE constexpr uint32_t TypeID()
    {
        return static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&typeid(T)));
    }

    // console logging
    struct EMPY_API Logger 
    { 
        using SPDLog = std::shared_ptr<spdlog::logger>;

        EMPY_INLINE Logger()
        {
            m_SPD = spdlog::stdout_color_mt("stdout");
            spdlog::set_level(spdlog::level::trace);              
            spdlog::set_pattern("%^[%T]: [#%t] %v%$");
        }

        EMPY_INLINE static SPDLog& Ref() 
        {
            static Logger logger;
            return logger.m_SPD;
        }   

    private:
        SPDLog m_SPD;         
    };
}

    // logging macros 
#ifdef EMPY_ENABLE_LOG
    #define EMPY_TRACE(...) Empy::Logger::Ref()->trace(__VA_ARGS__)
    #define EMPY_DEBUG(...) Empy::Logger::Ref()->debug(__VA_ARGS__)
    #define EMPY_INFO(...)  Empy::Logger::Ref()->info(__VA_ARGS__)
    #define EMPY_WARN(...)  Empy::Logger::Ref()->warn(__VA_ARGS__)
    #define EMPY_ERROR(...) Empy::Logger::Ref()->error(__VA_ARGS__)
    #define EMPY_FATAL(...) Empy::Logger::Ref()->critical(__VA_ARGS__) 
#else
	#define EMPY_TRACE
	#define EMPY_DEBUG
	#define EMPY_ERROR	
	#define EMPY_FATAL
	#define EMPY_INFO
    #define EMPY_WARN
#endif

// free allocated memory
#define EMPY_DELETE(ptr) if (ptr != nullptr) { delete (ptr); ptr = nullptr; }

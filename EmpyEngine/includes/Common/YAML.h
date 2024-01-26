#pragma once
#include "Core.h"
#include <yaml-cpp/yaml.h>

namespace YAML 
{
    template <>
    struct convert<glm::vec3> 
    {
        EMPY_INLINE static Node encode(const glm::vec3 &rhs) 
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        EMPY_INLINE static bool decode(const Node &node, glm::vec3 &rhs) 
        {
            if (!node.IsSequence() || node.size() != 3) 
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            
            return true;
        }
    };

    // stream operator
    EMPY_INLINE Emitter &operator << (Emitter &emitter, const glm::vec3 &v) 
    {
        emitter << Flow;
        emitter << BeginSeq << v.x << v.y << v.z << EndSeq;
        return emitter;
    }
}
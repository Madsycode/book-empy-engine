#pragma once
#include "Animation.h"

namespace Empy
{
    struct Animator 
    {
        EMPY_INLINE auto& Animate(float deltaTime)
        {
            if(m_Sequence < m_Animations.size()) 
            { 
                m_Time += m_Animations[m_Sequence].Speed * deltaTime;
                m_Time = fmod(m_Time, m_Animations[m_Sequence].Duration);
                UpdateJoints(m_Root, glm::identity<glm::mat4>());
            }
            return m_Transforms;
        }
    
    private:                   
        EMPY_INLINE glm::mat4 Interpolate(const KeyFrame& prev, const KeyFrame& next, float progression) 
        {
            return (glm::translate(glm::mat4(1.0f), glm::mix(prev.Position, next.Position, progression)) * 
            glm::toMat4(glm::normalize(glm::slerp(prev.Rotation, next.Rotation, progression))) * 
            glm::scale(glm::mat4(1.0f), glm::mix(prev.Scale, next.Scale, progression)));
        }
           
        EMPY_INLINE void GetPreviousAndNextFrames(Joint& joint, KeyFrame& prev, KeyFrame& next) 
        {          
            for (uint32_t i = 1u; i < joint.Keys.size(); i++) 
            {
				if (m_Time < joint.Keys[i].TimeStamp) 
                {
					prev = joint.Keys[i - 1];
					next = joint.Keys[i];
					return;
				}
			}
		}

        EMPY_INLINE void UpdateJoints(Joint& joint, const glm::mat4& parentTransform) 
        {
            KeyFrame prev, next;

            // get previous and next frames
            GetPreviousAndNextFrames(joint, prev, next);

            // compute interpolation factor
            float progression = (m_Time - prev.TimeStamp) /(next.TimeStamp - prev.TimeStamp);

            // compute joint new transform
            glm::mat4 transform = parentTransform * Interpolate(prev, next, progression);

            // update joint transform 
            m_Transforms[joint.Index] = (transform * m_GlobalTransform * joint.Offset);

            // update children joints
            for (auto& child : joint.Children) { UpdateJoints(child, transform); }
        }    
                
    private:
        std::vector<Animation> m_Animations;
        std::vector<glm::mat4> m_Transforms;
        glm::mat4 m_GlobalTransform;
        friend struct SkeletalModel;
        int32_t m_Sequence = 0;
        float m_Time = 0.0f;
		Joint m_Root;
    };
}
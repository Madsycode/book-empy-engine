#pragma once
#include "Context.h"

namespace Empy
{
    struct AppInterface
    {
        EMPY_INLINE virtual ~AppInterface() = default;
        
        // create entity
        template <typename Entt, typename... Args>
        EMPY_INLINE Entt CreateEntt(Args&&... args) 
        {            
            EMPY_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
            return std::move(Entt(&m_Context->Scene, std::forward<Args>(args)...));
        }       

        // convert id to entity
        template<typename Entt>
        EMPY_INLINE Entt ToEntt(EntityID entity) 
        { 
            EMPY_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
            return std::move(Entt(&m_Context->Scene, entity)); 
        }

        // loop through entities
        template<typename Entt, typename Comp, typename Task>
        EMPY_INLINE void EnttView(Task&& task) 
        {
            EMPY_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
            m_Context->Scene.view<Comp>().each([this, &task] 
            (auto entity, auto& comp) 
            { 
                task(std::move(Entt(&m_Context->Scene, entity)), comp);
            });
        }

        // loop through entities
        template<typename Task>
        EMPY_INLINE void AssetView(Task&& task) 
        {
            m_Context->Assets->View([&] (auto asset) { task(asset); });
        }
        
        // --

        // attach event callback
        template <typename Event, typename Callback>
        EMPY_INLINE void AttachCallback(Callback&& callback) 
        { 
            m_Context->Dispatcher.AttachCallback<Event>(std::move(callback), m_LayerID); 
        } 

        // post event
        template <typename Event, typename... Args>
        EMPY_INLINE void PostEvent(Args&&...args) 
        { 
            m_Context->Dispatcher.PostEvent<Event>(std::forward<Args>(args)...); 
        }

        // post task event
        template <typename Task>
        EMPY_INLINE void PostTask(Task&& task) 
        { 
            m_Context->Dispatcher.PostTask(std::move(task)); 
        } 

        // detach callback
        template <typename Event> 
        EMPY_INLINE void DetachCallback() 
        { 
            m_Context->Dispatcher.DetachCallback<Event>(m_LayerID); 
        } 

        // ++

        // attach layer
        template<typename Layer, typename... Args> 
        EMPY_INLINE Layer* AttachLayer(Args&&... args) 
        {
            // check layer's type compliance 
            EMPY_STATIC_ASSERT(std::is_base_of<AppInterface, Layer>::value);

            // check if layer allready exist
            if(GetLayer<Layer>() != nullptr)
            {
                EMPY_ERROR("Layer allready attached!");
                return nullptr;
            }

            // create layer and add to collection
            auto layer = new Layer(std::forward<Args>(args)...);
            m_Context->Layers.push_back(layer);
            layer->m_LayerID = TypeID<Layer>();
            layer->m_Context = m_Context;
            layer->OnStart();   
            return layer;         
        }

        // detach layer
        template<typename Layer> 
        EMPY_INLINE void DetachLayer()
        {   
            EMPY_STATIC_ASSERT(std::is_base_of<AppInterface, Layer>::value);
            m_Context->Dispatcher.PostTask([this] 
            {
                m_Context->Layers.erase(std::remove_if(m_Context->Layers.begin(), 
                m_Context->Layers.end(), [this] (auto& layer) 
                {
                    if(layer->m_LayerID == TypeID<Layer>())
                    {                
                        m_Context->Dispatcher.EraseListener(layer->m_LayerID);
                        EMPY_DELETE(layer);
                        return true;
                    }
                    return false;
                }), 
                m_Context->Layers.end());   
            });     
        }

        // retreive layer
        template<typename Layer> 
        EMPY_INLINE Layer* GetLayer() 
        { 
            EMPY_STATIC_ASSERT(std::is_base_of<AppInterface, Layer>::value);

            auto itr = std::find_if(m_Context->Layers.begin(), 
            m_Context->Layers.end(), [this] (auto layer) 
            {
                return (layer->m_LayerID == TypeID<Layer>());                   
            });    

            if(itr != m_Context->Layers.end())
            {
                return static_cast<Layer*>(*itr);
            }
            return nullptr;
        }

        // --

        EMPY_INLINE GLFWwindow* GetWindowHandle() 
        {
            return m_Context->Window->Handle();
        }

        EMPY_INLINE uint32_t GetSceneFrame() 
        {
            return m_Context->Renderer->GetFrame();
        }

    protected:
        EMPY_INLINE virtual void OnUpdate() {}
        EMPY_INLINE virtual void OnStart() {}

    private:
        friend struct Application;
        AppContext* m_Context;
        uint32_t m_LayerID;
    };
}
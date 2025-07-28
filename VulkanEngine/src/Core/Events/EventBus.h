#pragma once
#include "Event.h"

#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
namespace CHIKU
{
    class EventBus {
    public:
        using HandlerID = size_t;

        template<typename EventType>
        using HandlerFunc = std::function<bool(EventType&)>;

        template<typename EventType>
        HandlerID Subscribe(const HandlerFunc<EventType>& handler) 
        {
            auto wrapper = [handler](Event& e) -> bool 
                {
                    return handler(static_cast<EventType&>(e));
                };

            HandlerID id = nextHandlerId++;
            handlers[std::type_index(typeid(EventType))].emplace_back(id, wrapper);
            return id;
        }

        template<typename EventType>
        void Unsubscribe(HandlerID id) {
            auto& vec = handlers[std::type_index(typeid(EventType))];

            auto newEnd = std::remove_if(vec.begin(), vec.end(),
                [id](const auto& pair) { return pair.first == id; });
            vec.erase(newEnd, vec.end());
        }

        void Publish(Event& e) 
        {    
            auto it = handlers.find(std::type_index(typeid(e)));   
            if (it != handlers.end()) 
            {
                for (auto& [id, func] : it->second) 
                {
                    if (func(e)) 
                    {
                        e.Handled = true;
                        break;
                    }
                }
            }
        }

    private:
        using WrappedHandler = std::pair<HandlerID, std::function<bool(Event&)>>;
        std::unordered_map<std::type_index, std::vector<WrappedHandler>> handlers;
        HandlerID nextHandlerId = 0;
    };
}
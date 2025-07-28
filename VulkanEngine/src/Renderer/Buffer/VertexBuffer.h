#pragma once

#include "Utils/Utils.h"
#include <glm/glm.hpp>
#include <bitset>

namespace CHIKU
{
	//GLTF attrute order and name
    constexpr std::array<std::string_view, 8> VertexAttributesArray = {
        "POSITION", "NORMAL", "TEXCOORD_0", "TEXCOORD_1", "COLOR_0", "TANGENT" , "JOINTS_0", "WEIGHTS_0"
    };

    enum VertexAttributeBits {
        ATTR_POSITION = 0,
        ATTR_NORMAL,
        ATTR_TEXCOORD_0,
        ATTR_TEXCOORD_1,
        ATTR_COLOR_0,
        ATTR_TANGENT,
        ATTR_JOINTS_0,
        ATTR_WEIGHTS_0,
        ATTR_COUNT
    };

    enum class VertexComponentType : uint8_t
    {
        Float,          // 32-bit float
        Int,            // 32-bit signed int
        Byte,          // 4 x unsigned byte (e.g., packed color, often normalized)
        Short
    };

    enum class VertexAttributeType : uint8_t
    {
        SCALAR,
        Vec2,           // 2 x 32-bit float (e.g., UVs)
        Vec3,           // 3 x 32-bit float (e.g., position, normal)
        Vec4,           // 4 x 32-bit float (e.g., color, tangent, weights)        
    };

    struct VertexAttribute
    {
        uint32_t Offset = 0; // byte offset within vertex struct
        uint8_t size;
        VertexComponentType ComponentType;
        VertexAttributeType AttributeType;
        //1 byte can be squeezed in here 
    };

    struct VertexBufferLayout
    {
        uint32_t Stride;    
        std::bitset<ATTR_COUNT> Mask;
        std::vector<VertexAttribute> VertexElements;
    };

    struct VertexBufferMetaData
    {
        uint64_t Count;            // number of vertices
        VertexBufferLayout Layout;
    };

    inline bool operator==(const VertexAttribute& a, const VertexAttribute& b) {
        return a.Offset == b.Offset &&
            a.size == b.size &&
            a.ComponentType == b.ComponentType &&
            a.AttributeType == b.AttributeType;
    }

    inline bool operator==(const VertexBufferLayout& a, const VertexBufferLayout& b) {
        return a.Stride == b.Stride &&
            a.Mask == b.Mask &&
            a.VertexElements == b.VertexElements;
    }

    inline bool operator==(const VertexBufferMetaData& a, const VertexBufferMetaData& b) {
        return a.Count == b.Count &&
            a.Layout == b.Layout;
    }

	class VertexBuffer
	{
    public:
        virtual void CreateVertexBuffer(const std::vector<uint8_t>& vertices) = 0;
        virtual void Bind() const = 0;
        virtual void CleanUp() = 0;

        void SetBinding(uint32_t binding) { m_Binding = binding; }
		
        virtual void SetMetaData(const VertexBufferMetaData& metaData) 
        { 
            m_MetaData = metaData; 
        }

        inline virtual uint64_t GetCount() const final { return m_MetaData.Count; }
		inline virtual VertexBufferMetaData GetMetaData() const final { return m_MetaData; }

		static std::shared_ptr<VertexBuffer> Create();

    protected:
        uint32_t m_Binding = 0;
        VertexBufferMetaData m_MetaData;
	};
}

namespace std {

    template<>
    struct hash<CHIKU::VertexAttribute> {
        size_t operator()(const CHIKU::VertexAttribute& attr) const {
            size_t seed = 0;
            CHIKU::Utils::hash_combine(seed, std::hash<uint32_t>()(attr.Offset));
            CHIKU::Utils::hash_combine(seed, std::hash<uint8_t>()(attr.size));
            CHIKU::Utils::hash_combine(seed, std::hash<uint8_t>()(static_cast<uint8_t>(attr.ComponentType)));
            CHIKU::Utils::hash_combine(seed, std::hash<uint8_t>()(static_cast<uint8_t>(attr.AttributeType)));
            return seed;
        }
    };

    template<>
    struct hash<CHIKU::VertexBufferLayout> {
        size_t operator()(const CHIKU::VertexBufferLayout& layout) const {
            size_t seed = 0;
            CHIKU::Utils::hash_combine(seed, std::hash<uint32_t>()(layout.Stride));
            CHIKU::Utils::hash_combine(seed, std::hash<std::bitset<CHIKU::ATTR_COUNT>>()(layout.Mask));
            for (const auto& attr : layout.VertexElements)
                CHIKU::Utils::hash_combine(seed, std::hash<CHIKU::VertexAttribute>()(attr));
            return seed;
        }
    };

    template<>
    struct hash<CHIKU::VertexBufferMetaData> {
        size_t operator()(const CHIKU::VertexBufferMetaData& meta) const {
            size_t seed = 0;
            CHIKU::Utils::hash_combine(seed, std::hash<uint64_t>()(meta.Count));
            CHIKU::Utils::hash_combine(seed, std::hash<CHIKU::VertexBufferLayout>()(meta.Layout));
            return seed;
        }
    };
}

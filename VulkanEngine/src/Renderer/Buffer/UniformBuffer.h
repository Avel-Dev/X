#pragma once
#include "EngineHeader.h"
#include <bitset>

namespace CHIKU
{
    enum ShaderStages : uint32_t
    {
        Stage_None,
        Stage_Vertex,
        Stage_Tessellation ,
        Stage_Geometry,
        Stage_Fragment,
        Stage_Compute,
        Stage_Count
    };

	using ShaderStageBits = std::bitset<ShaderStages::Stage_Count>;

    enum class UniformPlainDataType : uint32_t
    {
        Float,
        Int,
        UInt,
        Bool,
        Vec2,
        Vec3,
        Vec4,
        IVec2,
        IVec3,
        IVec4,
        UVec2,
        UVec3,
        UVec4,
        BVec2,
        BVec3,
        BVec4,
        Mat2,
        Mat3,
        Mat4
    };

    enum class SamplerType : uint8_t {
        Sampler1D,
        Sampler2D,
        Sampler3D,
        SamplerCube,
        SamplerArray,
        Unknown
    };

    enum class UniformOpaqueDataType : uint16_t
    {
        DIFFUSE_TEXTURE,
        NORMAL_TEXTURE,
        SPECULAR_TEXTURE,
        EMISSIVE_TEXTURE,
        HEIGHT_TEXTURE,
        SHADOW_MAP_TEXTURE,
        SHADOW_CUBE_MAP_TEXTURE,
        SHADOW_MAP_ARRAY_TEXTURE,
        SHADOW_CUBE_MAP_ARRAY_TEXTURE,
        SHADOW_MAP_ARRAY_TEXTURE_2D,
        SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D,
        SHADOW_MAP_ARRAY_TEXTURE_3D,
        SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D,
        SHADOW_MAP_ARRAY_TEXTURE_2D_ARRAY,
        SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D_ARRAY,
        SHADOW_MAP_ARRAY_TEXTURE_3D_ARRAY,
        SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D_ARRAY,
        none
    };

    struct UniformMemberInfo
    {
        UniformPlainDataType Type;
        uint32_t Offset;
        uint32_t Size;
    };

    struct SamplerInfo
    {
        uint16_t Count;
        UniformOpaqueDataType DescriptorType = UniformOpaqueDataType::none;

        bool isValid() const {
            return DescriptorType != UniformOpaqueDataType::none;
        }
    };

    struct UniformBuffer
    {
        SamplerInfo BufferSamplerInfo;
        std::vector<UniformMemberInfo> UniformBufferInfo;
        std::bitset<ShaderStages::Stage_Count> Stages;
        uint32_t Size;

        bool isUBO() const {
            return UniformBufferInfo.size() > 0;
        }

        bool isSampler() const {
            return BufferSamplerInfo.isValid();
        }

        bool isValid() const {
            return !(isUBO() && isSampler());
        }
    };

	using UniformBufferSet = std::map<uint32_t, UniformBuffer>; // Key is the binding number
    using UniformBufferDescription = std::map<uint32_t, UniformBufferSet>; //Key is the set number

    struct TextureData;
    struct UniformBufferDescriptorInfo;

    static const char* ToString(UniformPlainDataType type)
    {
        switch (type)
        {
        case UniformPlainDataType::Float:  return "float";
        case UniformPlainDataType::Int:    return "int";
        case UniformPlainDataType::UInt:   return "uint";
        case UniformPlainDataType::Bool:   return "bool";
        case UniformPlainDataType::Vec2:   return "vec2";
        case UniformPlainDataType::Vec3:   return "vec3";
        case UniformPlainDataType::Vec4:   return "vec4";
        case UniformPlainDataType::IVec2:  return "ivec2";
        case UniformPlainDataType::IVec3:  return "ivec3";
        case UniformPlainDataType::IVec4:  return "ivec4";
        case UniformPlainDataType::UVec2:  return "uvec2";
        case UniformPlainDataType::UVec3:  return "uvec3";
        case UniformPlainDataType::UVec4:  return "uvec4";
        case UniformPlainDataType::BVec2:  return "bvec2";
        case UniformPlainDataType::BVec3:  return "bvec3";
        case UniformPlainDataType::BVec4:  return "bvec4";
        case UniformPlainDataType::Mat2:   return "mat2";
        case UniformPlainDataType::Mat3:   return "mat3";
        case UniformPlainDataType::Mat4:   return "mat4";
        default:                           return "(unknown)";
        }
    }

    static const char* ToString(UniformOpaqueDataType type)
    {
        switch (type)
        {
        case UniformOpaqueDataType::DIFFUSE_TEXTURE:                    return "DIFFUSE_TEXTURE";
        case UniformOpaqueDataType::NORMAL_TEXTURE:                     return "NORMAL_TEXTURE";
        case UniformOpaqueDataType::SPECULAR_TEXTURE:                   return "SPECULAR_TEXTURE";
        case UniformOpaqueDataType::EMISSIVE_TEXTURE:                   return "EMISSIVE_TEXTURE";
        case UniformOpaqueDataType::HEIGHT_TEXTURE:                     return "HEIGHT_TEXTURE";
        case UniformOpaqueDataType::SHADOW_MAP_TEXTURE:                 return "SHADOW_MAP_TEXTURE";
        case UniformOpaqueDataType::SHADOW_CUBE_MAP_TEXTURE:            return "SHADOW_CUBE_MAP_TEXTURE";
        case UniformOpaqueDataType::SHADOW_MAP_ARRAY_TEXTURE:           return "SHADOW_MAP_ARRAY_TEXTURE";
        case UniformOpaqueDataType::SHADOW_CUBE_MAP_ARRAY_TEXTURE:      return "SHADOW_CUBE_MAP_ARRAY_TEXTURE";
        case UniformOpaqueDataType::SHADOW_MAP_ARRAY_TEXTURE_2D:        return "SHADOW_MAP_ARRAY_TEXTURE_2D";
        case UniformOpaqueDataType::SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D:   return "SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D";
        case UniformOpaqueDataType::SHADOW_MAP_ARRAY_TEXTURE_3D:        return "SHADOW_MAP_ARRAY_TEXTURE_3D";
        case UniformOpaqueDataType::SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D:   return "SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D";
        case UniformOpaqueDataType::SHADOW_MAP_ARRAY_TEXTURE_2D_ARRAY:  return "SHADOW_MAP_ARRAY_TEXTURE_2D_ARRAY";
        case UniformOpaqueDataType::SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D_ARRAY: return "SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D_ARRAY";
        case UniformOpaqueDataType::SHADOW_MAP_ARRAY_TEXTURE_3D_ARRAY:  return "SHADOW_MAP_ARRAY_TEXTURE_3D_ARRAY";
        case UniformOpaqueDataType::SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D_ARRAY: return "SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D_ARRAY";
        case UniformOpaqueDataType::none:                               return "none";
        default:                                                        return "(unknown)";
        }
    }
}
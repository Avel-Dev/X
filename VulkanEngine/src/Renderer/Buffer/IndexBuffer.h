#pragma once
#include "EngineHeader.h"

namespace CHIKU
{
	class IndexBuffer
	{
    public:
        virtual void CreateIndexBuffer(const std::vector<uint32_t>& indices) = 0;
        virtual void Bind() const = 0;
        virtual void CleanUp() = 0;

        virtual uint32_t GetCount() const final { return count; }

		static std::shared_ptr<IndexBuffer> Create();

    protected:
        uint32_t count;
	};
}
#pragma once
#include "YEngine_System/common.h"
#include <array>

const constexpr uint32_t MAX_FRAMES_IN_FLIGHT =2 ;

template<typename T>
class FrameResources
{
public:
	T& operator[](uint32_t index) { return m_resources[index]; }
	T& getFrameResource(uint32_t index) { return m_resources[index]; }
	void release()
	{
		for (auto& resource : m_resources)
			resource.release();
	}
	auto begin() { return m_resources.begin(); }
	auto end() { return m_resources.end(); }
	constexpr uint32_t size() { return MAX_FRAMES_IN_FLIGHT; }
private:
	std::array<T, MAX_FRAMES_IN_FLIGHT> m_resources;
};


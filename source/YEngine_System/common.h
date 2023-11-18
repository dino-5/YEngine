#pragma once
#include <stdexcept>
using uint = unsigned int;

// place this define after first public to get the right behaviour
#define DesctructorVulkanObject(VulkanClassName) private:\
bool _isRealesed = false;\
public:\
~VulkanClassName(){ if(!_isRealesed) release();}

#define VulkanObjectReleased() _isRealesed = true
#define VulkanObjectInitialized() _isRealesed = false 

#define NON_COPYABLE(className) className(const className&)=delete;\
className operator=(const className&)=delete;

#define ASSERT(exp) if(!(exp)) std::runtime_error("failed");
#define ASSERT(exp, msg) if(!(exp)) std::runtime_error(msg);



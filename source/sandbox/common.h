#pragma once

// place this define after first public to get the right behaviour
#define DesctructorVulkanObject(VulkanClassName) private:\
bool _isRealesed = false;\
public:\
~VulkanClassName(){ if(!_isRealesed) Release();}

#define VulkanObjectReleased() _isRealesed = true
#define VulkanObjectInitialized() _isRealesed = false 


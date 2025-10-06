#pragma once

// Springs are basically bonds between each particle
struct Spring {
    int a, b;       // indices of connected particles
    float restLen;  
    float k;
	float maxLen; // length at which spring breaks  
};

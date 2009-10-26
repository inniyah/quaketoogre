#ifndef __COMMON_H__
#define __COMMON_H__

#include "tinyxml.h"

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <locale>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

using namespace std;

#include "StringUtil.h"

struct GlobalSettings
{
	GlobalSettings();

	bool convertCoords;
	bool writeMaterials;
};

#endif

#pragma once

#if defined(TARGET_PIXELVTX)
#include "targets/pixelVTX.h"
#elif defined(TARGET_PIXELVTX_COLOR)
#include "targets/pixelVTXcolor.h"
#elif defined(TARGET_GENERIC_VTX)
#include "targets/genericVTX.h"
#elif defined(TARGET_OPENOSDX)
#include "targets/openOSD-X.h"
#else
#include "targets/generic.h"
#endif

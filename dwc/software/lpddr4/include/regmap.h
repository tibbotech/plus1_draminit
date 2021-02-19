#ifndef __INC_REGMAP_H
#define __INC_REGMAP_H

#include <config.h>

#ifdef PLATFORM_PENTAGRAM
#include <regmap_pentagram.h>
#elif defined(PLATFORM_I143)
#include <regmap_i143.h>
#else
#error "NO PLATFORM_XXXX defined."
#endif

#endif /* __INC_REGMAP_H */

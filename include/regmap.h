#ifndef __INC_REGMAP_H
#define __INC_REGMAP_H

#include <config.h>

#ifdef PLATFORM_GEMINI
#include <regmap_gemini.h>
#else
#error "NO PLATFORM_XXXX defined."
#endif

#endif /* __INC_REGMAP_H */

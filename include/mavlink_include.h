#pragma once

#if defined(__has_include)
  #if __has_include(<mavlink/common/mavlink.h>)
    #include <mavlink/common/mavlink.h>
  #elif __has_include(<mavlink.h>)
    #include <mavlink.h>
  #elif __has_include("MAVLink.h")
    #include "MAVLink.h"
  #else
    #error "MAVLink headers not found. Install MAVLink library (ArduPilot) or include generated headers."
  #endif
#else
  #include <mavlink/common/mavlink.h>
#endif

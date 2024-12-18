#pragma once
#define NOMINMAX
#define GLFW_EXPOSE_NATIVE_WIN32
#define _USE_MATH_DEFINES
#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <windows.h>
#include <thread>
#include <future>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <GLFW/glfw3native.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cmath>
#include <type_traits>
#include <cstdint>

/* Operators for enum classes using flags:
  i have ZERO clue why those doens't work as intended in the first place 
   */

template <typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, Enum>::type
operator|(Enum lhs, Enum rhs) {
  using T = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

// Bitwise AND operator
template <typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, Enum>::type
operator&(Enum lhs, Enum rhs) {
  using T = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

// Bitwise OR-assignment operator
template <typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, Enum&>::type
operator|=(Enum& lhs, Enum rhs) {
  lhs = lhs | rhs;
  return lhs;
}

// Bitwise AND-assignment operator
template <typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, Enum&>::type
operator&=(Enum& lhs, Enum rhs) {
  lhs = lhs & rhs;
  return lhs;
}

// Bitwise NOT operator
template <typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, Enum>::type
operator~(Enum e) {
  using T = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(~static_cast<T>(e));
}

/* Endof : operators */

#define ___QF_EMPTY_STRING ""
#define __QF_DONT_CARE -1
#define __QF_UNDEFINED -333
#define __QF_DEBUG_LEVEL 1

#define __QF_SHUTDOWN_MSG() \
    std::cout << "\033[31m" << \
    "+------------------------------------------------------+" << std::endl << \
    "|  retard alert                                 - [] x |" << std::endl << \
    "+------------------------------------------------------+" << std::endl << \
    "|                & SHUTTING DOWN QF &                  |" << std::endl << \
    "|                                                      |" << std::endl << \
    "|             || SAVE FROM CRASH-STATE ||              |" << std::endl << \
    "|          *dumb retarded fuck revise ur code          |" << std::endl << \
    "+------------------------------------------------------+" << std::endl << \
    "\033[0m" << std::endl;

namespace QF {
  namespace Utils {
    class Vec2;
    class BasicAnim;
    class Rect; 
  };

  namespace UI {
    namespace Components {
      class Element; 

      class EventSystem;

      class Panel;
      class Window; 

      class Element;

      class Panel;
      class Window;
    }
    class App;
  };
};

#include "Utils/QFUtils.h"
#include "UI/QFUI.h"
#include "Core/QFCore.h"

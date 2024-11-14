#pragma once
#define NOMINMAX
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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define ___QF_EMPTY_STRING ""
#define __QF_DONT_CARE -1
#define __QF_UNDEFINED -333

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

#include "Utils/QFUtils.h"
#include "UI/QFUI.h"
#include "Core/QFCore.h"

#include "../../QF.h"

QF::UI::Window::Resize::Resize(QF::UI::Window* _Window)
 : m_ParentWindow{ _Window }

{
 m_Size = {15, 15};
 m_Image = new QF::Utils::Image(QF::Utils::Filesystem::Helpers::g_InCurrentPath("iconresize.png"));
 std::cout << QF::Utils::Filesystem::Helpers::g_InCurrentPath("").string()<< "\n";
} 

QF::UI::Window::Resize::~Resize()
{
 if (m_Image == nullptr) return; 

 delete m_Image;
}

/* Rendering in called in hk_Render to avoid imgui's invaild window calls */
bool QF::UI::Window::Resize::hk_MainLoop() 
{ 
 /* Take action */
 hdl_Resize();
  /*  Update positions */
 func_UpdatePositions();

 return m_IsResizing;
}

void QF::UI::Window::Resize::hk_MouseClick()
{
 hdl_StartResize();
}

void QF::UI::Window::Resize::func_UpdatePositions()
{ 
 /* Set positions based on m_ParentWindow */
 m_Position = (m_ParentWindow->g_Size() - m_Size);
}

void QF::UI::Window::Resize::hk_Render()
{
 this->hdl_Render();
}

void QF::UI::Window::Resize::hdl_Render() 
{
 ImDrawList* _DrawList = ImGui::GetWindowDrawList();

 _DrawList->AddImage(m_Image->g_ImGuiTextureure(), m_Position.g_ImVec2(), (m_Position + m_Size).g_ImVec2());
 //std::cout << m_Image->g_ImGuiTextureure() << "\n";
}

bool QF::UI::Window::Resize::hdl_StartResize()
{
 /* Check if not resizing already*/
 if (m_IsResizing) return false; 

 const QF::Utils::Vec2 _MousePosition = m_ParentWindow->g_MousePosition();

 /* Check for mouse in bounds */
 if (QF::Utils::Math::is_InBounds(
  _MousePosition, m_Position, m_Size
 ) == false) return false; 

 m_IsResizing = true; 
 m_StartPosition = (_MousePosition - m_Position);

 return true; 
}

void QF::UI::Window::Resize::hdl_Resize()
{
 if (!m_IsResizing) return;

 /* Check for left mouse button down */
 if (glfwGetMouseButton(
   m_ParentWindow->g_GLFWwindow(), GLFW_MOUSE_BUTTON_1
   ) != GLFW_PRESS)
   {
    /* Update vars & abort */
    m_IsResizing = false; 
    return; 
   }

 /* Update window size */
 const QF::Utils::Vec2 _NewSize = (m_ParentWindow->g_AbsoluteMousePosition() - 
  m_ParentWindow->g_WindowPosition() + m_StartPosition);

 m_ParentWindow->s_Size(_NewSize);
}
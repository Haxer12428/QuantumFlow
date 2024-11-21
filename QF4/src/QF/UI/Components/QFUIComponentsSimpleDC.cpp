#include "QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
namespace utils = QF::Utils;
using self = QF::UI::Components::SimpleDC;

components::SimpleDC::SimpleDC(Panel* _Element)
	: m_Panel{ _Element } 
{
	/* SOOO LONG DEBUGGING SESSION JUST BECAUSE CONTEXT WAS NOT SET
	* ANY OTHER IMGUIS OPERATIONS FROM THIS MOMENT IN TIME WILL BE FORWARDED BY IMGUIS CONTEXT SET!!!
	*/
	ImGui::SetCurrentContext(_Element->g_AbsoluteParent()->g_GLFWobject()->g_ImGuiContext());

	m_DrawList = ImGui::GetWindowDrawList();
	alignInitFunctions();

	utils::Rect clientRect = _Element->g_AbsoluteParent()->g_Window()->g_GLFWobject()->g_ClientAreaRect();

	if (_Element->g_Flags() & static_cast<std::underlying_type<Panel::Flags>::type>(Panel::Flags::m_DontCareAboutClipRectWhenRendering)) {
		m_DrawList->PushClipRect(_Element->g_FixedPosition(), _Element->g_FinalPositionFixed()); return; 
	}

	m_DrawList->PushClipRect(
		_Element->g_FixedPosition().clamp(clientRect.g_Position(), clientRect.g_FinalPosition()), _Element->g_FinalPositionFixed().clamp(clientRect.g_Position(), clientRect.g_FinalPosition())
		);
}

components::SimpleDC::~SimpleDC()  {
	m_DrawList->PopClipRect();
};

/* drawing help operators */
	/* push font & pop font have basic safety to avoid assertion fails */
	void components::SimpleDC::pushFont(ImFont* _Font) {
		if (_Font == nullptr) return;
		ImGui::PushFont(_Font);
		m_FontPushed = true; 
	}
	
	void components::SimpleDC::popFont() {
		if (!m_FontPushed) return;
		ImGui::PopFont();
	}

/* drawing operators */
	void components::SimpleDC::putRect(const utils::Vec2& _Pos, const utils::Vec2& _Size, ImColor _Clr, float _Rounding, float _Thickness, DrawingFlags _Flags, ImDrawFlags _ImGuiFlags)
	{
		const utils::Rect alignedRect = g_AlignedRect(_Pos, _Size, _Flags);
		m_DrawList->AddRect(fixVec2(alignedRect.g_Position()), fixVec2(alignedRect.g_FinalPosition()), _Clr, _Rounding, _ImGuiFlags);
	}

	void components::SimpleDC::putRectFilled(const QF::Utils::Vec2& _Pos, const QF::Utils::Vec2& _Size, ImColor _Clr, float _Rounding, DrawingFlags _Flags, ImDrawFlags _ImGuiFlags)
	{
		const utils::Rect alignedRect = g_AlignedRect(_Pos, _Size, _Flags);
		m_DrawList->AddRectFilled(fixVec2(alignedRect.g_Position()), fixVec2(alignedRect.g_FinalPosition()), _Clr, _Rounding, _ImGuiFlags);
	}
/* section -> private */
	void components::SimpleDC::alignInitFunctions() {
		/* Centering */
		m_AlignFunctions.push_back([&](const utils::Vec2& _Pos, const utils::Vec2& _Size, DrawingFlags _Flags) -> utils::Rect
			{
				utils::Vec2 panelSize = m_Panel->g_FixedSize();

				/* ignore: Boundary flags limitation */
				if (utils::Compare::bitFlagPoolMatchEnumClass(_Flags, DrawingFlags::m_AlignIgnoreBoundaryLimits)) {
					panelSize = m_Panel->g_Size();
				}

				utils::Vec2 centeredSize = (panelSize / 2.0f);
				utils::Vec2 centeredPos = (centeredSize + _Pos - (_Size / 2.0f));

				utils::Vec2 outputPos = _Pos;

				/* align: x */
				if (utils::Compare::bitFlagPoolMatchEnumClass(_Flags, DrawingFlags::m_AlignCenterX)) {
					outputPos.x = centeredPos.x;
				}
				/* align: y */
				if (utils::Compare::bitFlagPoolMatchEnumClass(_Flags, DrawingFlags::m_AlignCenterY)) {
					outputPos.y = centeredPos.y;
				}

				return { outputPos, _Size };
			});
	}

	const utils::Vec2 components::SimpleDC::fixVec2(const utils::Vec2& _Vec) const {
		return (m_Panel->g_FixedPosition() + _Vec);
	}

	const utils::Rect components::SimpleDC::g_AlignedRect(const utils::Vec2& _Pos, const utils::Vec2& _Size, DrawingFlags _Flags) {
		utils::Rect RectNew = { _Pos, _Size };

		for (auto _AlignFunc : m_AlignFunctions) {
			RectNew = _AlignFunc(RectNew.g_Position(), RectNew.g_Size(), _Flags);
		}

		

		return RectNew;
	}

	void components::SimpleDC::putText(const utils::Vec2& _Pos, const std::string& _Text, ImColor _Clr, DrawingFlags _Flags) {
		utils::Vec2 textSize = ImGui::CalcTextSize(_Text.c_str());

		utils::Rect alignedRect = g_AlignedRect(_Pos, textSize, _Flags);
		m_DrawList->AddText(fixVec2(alignedRect.g_Position()), _Clr, _Text.c_str());
	}

	void self::putTexture(const QF::Utils::Vec2& _Pos, const QF::Utils::Vec2& _Size, GLuint _GLTextureID, DrawingFlags _Flags, ImU32 color, const QF::Utils::Vec2& uv_min, const QF::Utils::Vec2& uv_max) {
		/* Abort if flag is set && texture == undefined */
		if (_GLTextureID == 0 && utils::Compare::bitFlagPoolMatchEnumClass(
			DrawingFlags::m_putTextureAbortIfTextureUndefined, _Flags)
			)
			return; 
			
		utils::Rect alignedRect = g_AlignedRect(_Pos, _Size, _Flags);

		m_DrawList->AddImage(reinterpret_cast<ImTextureID>(_GLTextureID), fixVec2(alignedRect.g_Position()), fixVec2(alignedRect.g_FinalPosition()), uv_min, uv_max, color);
	}

	void self::putLine(const utils::Vec2& _Pos1, const utils::Vec2& _Pos2, ImU32 _Color, float _Thickness, DrawingFlags _Flags) {
		utils::Rect alignedRect = g_AlignedRect(_Pos1,
			{ _Pos2.x - _Pos1.x, _Pos2.y - _Pos1.y }, _Flags
			);

		m_DrawList->AddLine(fixVec2(alignedRect.g_Position()), fixVec2(alignedRect.g_FinalPosition()), _Color, _Thickness);
	}

	void self::putCircleFilled(const QF::Utils::Vec2& _Center, float _Radius, ImU32 _Color, int _Segments, DrawingFlags _Flags) {
		utils::Rect alignedRect = g_AlignedRect(_Center, {0.0f}, _Flags);

		m_DrawList->AddCircleFilled(fixVec2(alignedRect.g_Position()), _Radius, _Color, _Segments);
	}
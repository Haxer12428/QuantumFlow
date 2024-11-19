#include "QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
namespace utils = QF::Utils;

components::SimpleDC::SimpleDC(Panel* _Element)
	: m_Panel{ _Element } 
{
	m_DrawList = ImGui::GetWindowDrawList();
	alignInitFunctions();
}

components::SimpleDC::~SimpleDC() = default; 

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

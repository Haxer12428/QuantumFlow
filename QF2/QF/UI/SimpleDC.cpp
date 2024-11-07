#include "../QF.h"

/*========================= Constructor & Destructor =========================*/
	QF::UI::SimpleDC::SimpleDC(Panel* _Panel)
		: m_Panel{_Panel}
	{
		/* Check is panel is nullptr */
		if (m_Panel == nullptr)
		{
			/* Log debug */
			QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR,
				__FUNCTION__, "Provided panel == nullptr"
			);
			/* Throw exception */
			throw std::exception(
				"Provided panel == nullptr, cannot continue"
			);
		}
		func_StartDrawing();
	}

	QF::UI::SimpleDC::~SimpleDC()
	{
		/* Handle drawing regions */
		func_StopDrawing();
	}
/*========================= Get vec2 fixed  =========================*/
	const QF::Utils::Vec2 QF::UI::SimpleDC::g_Vec2(const float& _x, const float& _y) const
	{
		return (m_Panel->g_PositionOffset() + QF::Utils::Vec2(_x, _y));
	}

	const QF::Utils::Vec2 QF::UI::SimpleDC::g_Vec2(const QF::Utils::Vec2& _Vector) const
	{
		return (m_Panel->g_PositionOffset() + _Vector);
	}
/*========================= Drawing helpers  =========================*/
	void QF::UI::SimpleDC::func_StartDrawing()
	{
		/* Initialize Drawing List */
		m_DrawList = ImGui::GetWindowDrawList();

		/* Push clip rect */
		m_DrawList->PushClipRect(
			m_Panel->g_PositionOffset().g_ImVec2(), m_Panel->g_FinalPositionOffset().g_ImVec2(), true
			);
	}

	void QF::UI::SimpleDC::func_StopDrawing()
	{
		/* Destroy clip rect */
		m_DrawList->PopClipRect();
	}
/*========================= Drawing operations  =========================*/
	void QF::UI::SimpleDC::draw_Rect(
		const QF::Utils::Vec2& _Position, const QF::Utils::Vec2& _Size, const ImColor& _Color,
		const float& _Rounding, ImDrawFlags _Flags, const float& _Thickness
	) {
		/* Add to draw list */
		m_DrawList->AddRect(
			g_Vec2(_Position).g_ImVec2(),
			g_Vec2((_Position + _Size)).g_ImVec2(),
			_Color, _Rounding, _Flags, _Thickness
		);
	}

	void QF::UI::SimpleDC::draw_RectFilled(
		const QF::Utils::Vec2& _Position, const QF::Utils::Vec2& _Size, const ImColor& _Color,
		const float& _Rounding, ImDrawFlags _Flags
	) {
		/* Add to draw list */
		m_DrawList->AddRectFilled(
			g_Vec2(_Position).g_ImVec2(),
			g_Vec2((_Position + _Size)).g_ImVec2(),
			_Color, _Rounding, _Flags
		);
	}

	void QF::UI::SimpleDC::draw_Text(
		const QF::Utils::Vec2& _Position, const ImColor& _Color, const std::string& _Text, ImFont* _Font)
	{
		/* If font given push it */
		if (_Font != nullptr) ImGui::PushFont(_Font);
		/* Draw text */
		m_DrawList->AddText(g_Vec2(_Position).g_ImVec2(), _Color, _Text.c_str());
		/* Pop font */
		if (_Font != nullptr) ImGui::PopFont();
	}

	void QF::UI::SimpleDC::draw_Image(
		QF::Utils::Image* _Image, const QF::Utils::Vec2& _Position, const QF::Utils::Vec2& _Size, const ImColor& _Color, const QF::Utils::Vec2 _UvMin, const QF::Utils::Vec2 _UvMax
	) {
		if (_Image == nullptr) return; 

		m_DrawList->AddImage(
			_Image->g_ImGuiTextureure(),
			g_Vec2(_Position).g_ImVec2(),
			g_Vec2(_Position + _Size).g_ImVec2(),
			_UvMin.g_ImVec2(),
			_UvMax.g_ImVec2(),
			_Color
		);
	}
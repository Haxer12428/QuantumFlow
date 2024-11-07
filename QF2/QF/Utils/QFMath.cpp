#include "../QF.h"

const QF::Utils::Vec2 QF::Utils::Math::g_TextSize(
	const std::string& _Text, ImFont* _Font)
{
	if (_Font != nullptr) ImGui::PushFont(_Font);
	/* Calculate */
	const ImVec2 _Size = ImGui::CalcTextSize(_Text.c_str());
	if (_Font != nullptr) ImGui::PopFont();

	/* Return calculated */
	return { _Size.x, _Size.y };
}

const QF::Utils::Vec2 QF::Utils::Math::g_TextCenteredPosition(const Vec2& _Position, const Vec2& _Size, const std::string& _Text, ImFont* _Font, const Vec2& _Offsets)
{
	const Vec2 _TextSize = g_TextSize(_Text, _Font);
	const Vec2 _SizeCenter = (_Size / 2);
	const Vec2 _TextCetner = (_TextSize / 2);
	const Vec2 _FinalTextPosition = (_SizeCenter - _TextCetner + _Offsets);

	/* Apply position offset */
	return (_Position + _FinalTextPosition);
}

const bool QF::Utils::Math::is_InBounds(const Vec2& _Position, const Vec2& _Starting, const Vec2& _Size)
{
	const Vec2& _Final = (_Starting + _Size);
	/* x axis */
	if (_Position.x < _Starting.x 
		|| _Position.x > _Final.x)
		return false; 
	if (_Position.y < _Starting.y
		|| _Position.y > _Final.y)
		return false; 
	/* Good */
	return true; 
}
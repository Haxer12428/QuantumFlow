#include "../QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
namespace comp_built = components::Built;
namespace utils = QF::Utils;
using self = comp_built::Button;

/* Constructor & Destructor */
	comp_built::Button::Button(components::Element* _Parent, Hints& _Hints) 
		: m_Hints{_Hints},
		
		components::Panel(_Parent, _Hints.m_Pos, _Hints.m_Size, _Hints.m_PanelFlags)
	{
		/* Bind render event */
		g_EventHandler()->Subscribe<components::EventSystem::Events::Render>(this, &comp_built::Button::Render);
		g_EventHandler()->Subscribe<components::EventSystem::Events::MouseClickEvent>(this, &self::MouseClickEvent);

		bool hasButtonBeenAnimated = false; 
		/* Animation handler */
		utilAddPreRenderCallback([&](components::Panel* _Obj) {
			if (!m_BGTransitionAnim->is_Finished()) return; 
			const bool mouseInBounds = _Obj->is_MouseOnPanel();
			
			ImColor wantedColor, transitionFromColor; 

			if (mouseInBounds) { wantedColor = m_Hints.m_BGActiveColor; transitionFromColor = m_Hints.m_BGColor; };
			if (!mouseInBounds) { wantedColor = m_Hints.m_BGColor; transitionFromColor = m_Hints.m_BGActiveColor; }

			if (m_BGTransitionAnim->g_Last() == std::vector<float>{
				wantedColor.Value.x, wantedColor.Value.y, wantedColor.Value.z, wantedColor.Value.w
			}) {
				return;
			}

			m_BGTransitionAnim->s_Starting(transitionFromColor);
			m_BGTransitionAnim->s_Wanted(wantedColor);

			m_BGTransitionAnim->startAnimation();
			});
	}

	comp_built::Button::~Button() {
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Destructed.");
	}

	comp_built::Button::Hints& comp_built::Button::g_Hints() {
		return m_Hints;
	}

	void comp_built::Button::Render(components::EventSystem::Events::Render& _r) {
		components::SimpleDC canvas{this};
		/* Render bg */
		
		canvas.putRectFilled({ 0.0f }, g_Size(), utils::BasicAnim::g_ImColor(m_BGTransitionAnim->g_Animated(200)));
	
		canvas.putTexture({ 0.0f }, g_Size(), m_Hints.m_TextureID, components::SimpleDC::DrawingFlags::m_putTextureAbortIfTextureUndefined);
	}

	void self::MouseClickEvent(components::EventSystem::Events::MouseClickEvent& _evt) {
		std::cout << this->g_ImmutableId() << " <= id\n";
		for (auto &_Callback : m_OnClickCallbacks) {
			if (this != nullptr) {
				_Callback.m_Callback(this, _evt);
			}
		}
	}

	const uint64_t self::addOnClickCallback(const std::function<void(comp_built::Button*, EventSystem::Events::MouseClickEvent& _Event)> _Callback) {
		m_OnClickCallbacks.push_back({ m_OnClickCallbacksIdManager->g_New(), _Callback});
		return m_OnClickCallbacks.back().m_UniqueID;
	}

	const bool self::destroyOnClickCallback(uint64_t UniqueId) {
		size_t _Iterator = 0; 
		for (auto _Callback : m_OnClickCallbacks) {
			if (_Callback.m_UniqueID == UniqueId) { m_OnClickCallbacks.erase(
				(m_OnClickCallbacks.begin() + _Iterator)
			); return true; }
		}
		__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, "Failed to destroy callback, it was never assigned or destroyed before.");

		return false; 
	}
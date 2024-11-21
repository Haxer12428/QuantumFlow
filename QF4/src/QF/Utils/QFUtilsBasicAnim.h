#pragma once
#define NOMINMAX
#include "../QF.h"
#include "QFUtilsVec2.h"
#include <vector>

namespace QF {
	namespace Utils {
		/* Class that handles basic float animations 
			Support's singular floats & QFVec2 
		*/
		class BasicAnim 
		{
		public:
			BasicAnim(); 
			~BasicAnim();

			void s_Starting(float _Value); 
			void s_Starting(QF::Utils::Vec2 _Value);
			void s_Starting(std::vector<float> _Set);
			void s_Starting(ImColor _Value);

			void s_Wanted(float _Value); 
			void s_Wanted(QF::Utils::Vec2 _Value);
			void s_Wanted(std::vector<float> _Set);
			void s_Wanted(ImColor _Value);
			const std::vector<float> g_Last() const; 

			const std::vector<float> g_Wanted() const; 
			
			static const ImColor g_ImColor(const std::vector<float>& _ValueList);

			const std::vector<float> g_Animated(int _MsToFinish); 
			void startAnimation();
			
			const bool is_Finished() const; 
		private:
			std::vector<float> m_ValuesStarting; 
			std::vector<float> m_ValuesWanted; 
			std::vector<float> m_ValuesLast = {};

			std::chrono::high_resolution_clock::time_point m_StartedAnimation;
		};
	}
}
#pragma once
#include "../../QF/QF.h"
#include <ctime>
#include <cstdlib>

class QFExampleTetris : public QF::UI::Components::Window { public: 
	QFExampleTetris(QF::UI::App* _ParentingApp); 
	~QFExampleTetris();
private:
	class Game : public QF::UI::Components::Panel {
	public:
		Game(QF::UI::Components::Window* _Parent);
		virtual ~Game(); 
	private:
		std::vector<std::vector<std::vector<uint16_t>>> m_BlockTypes = {
	{ /* square */
		{ 1, 1 },
		{ 1, 1 }
	},
	{
		{ 1, 1 },
			{1},
			{1}
	},
		{
			{1},
			{1},
			{1},
			{1},
			{1}
},
			{
			{0, 1},
			{1, 1},
			{1, 0}
			},
			{
			{1, 1},
			{0, 1},
			{0, 1}
			}
		};
		std::unique_ptr<QF::Utils::Image> m_BlockTexture;
		std::unique_ptr<QF::Utils::Image> m_BGTexture;

		std::vector<std::vector<uint16_t>> m_CurrentBlockPatern = {};
		QF::Utils::Vec2 m_CurrentBlockPosition;

		QF::Utils::Vec2 m_BoardSize = { 10.0f , 20.0f };
		QF::Utils::Vec2 m_BlockSize = 50.0f;

		int m_MsPerUpdate = 200;
		std::vector<std::vector<uint16_t>> m_Board;
		std::vector<std::vector<uint16_t>> m_BlockBoard;
		int m_MsPerBlockShift = 100;

		std::chrono::high_resolution_clock::time_point m_LastBlockUpdate = std::chrono::high_resolution_clock::now();

		std::unique_ptr<QF::Utils::BasicAnim> m_BlockBoardAnimX = std::make_unique<QF::Utils::BasicAnim>();
		std::unique_ptr<QF::Utils::BasicAnim> m_BlockBoardAnimY = std::make_unique<QF::Utils::BasicAnim>();

		void blockBoardRotateBlock(QF::UI::Components::EventSystem::Events::BeforeRender&);
		const bool is_BlockBoardColliding() const;
		const std::vector<std::vector<uint16_t>> createBoard() const;
		void renderBoard(QF::UI::Components::EventSystem::Events::Render& _R);
		void updateBoardPositionX(QF::UI::Components::EventSystem::Events::BeforeRender& _br);
		void updateBoardPositions(QF::UI::Components::EventSystem::Events::BeforeRender& _br); 
		void generateBoardBlock(QF::UI::Components::EventSystem::Events::BeforeRender& _br);
		void combineBoards();
		void clearBoardLines(QF::UI::Components::EventSystem::Events::BeforeRender& _br);

		void updatedPosForBlockBoard(const QF::Utils::Vec2& _Old, const QF::Utils::Vec2& _New);

		const size_t g_FurthestYInBlockBoard() const; 

		void putBlockIntoFrame();

		bool m_RotateButtonHeldLastFrame = false; 
		bool m_UpdatePositionXHeldButtonLastFrame = false; 
		std::chrono::high_resolution_clock::time_point m_UpdatePositionXHeldButtonSince = std::chrono::high_resolution_clock::now();
	};
private:
	Game* m_Game;
};
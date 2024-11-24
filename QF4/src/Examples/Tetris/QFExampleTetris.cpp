#include "QFExampleTetris.h"

namespace utils = QF::Utils;
namespace components = QF::UI::Components;
using app = QF::UI::App;
using self = QFExampleTetris;
using evts = QF::UI::Components::EventSystem::Events;
namespace chrono = std::chrono;
using ccl = chrono::high_resolution_clock;

/* Constructor & Destructor */
	self::QFExampleTetris(app* _Parent) : 
		components::Window(_Parent, __QF_DONT_CARE, { 500.0f , 1000.0f}, "QF Tetris Example") {
		
		g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);
		g_GLFWobject()->s_VsyncState(false);
		m_Game = new Game(this);
	}

	self::~QFExampleTetris() = default; 
/* Game */
	/* Constructor & Destructor */
		self::Game::Game(components::Window* _Window) :
			components::Panel(_Window, 0, 1) {
			/* Load texture */
			m_BlockTexture = std::make_unique<utils::Image>(std::filesystem::current_path().string() + "/tetrisblock.jpg");
			m_BGTexture = std::make_unique<utils::Image>(std::filesystem::current_path().string() + "/tetrisbg.jfif");

			/* Create board */
			m_Board = createBoard();

			/* Handle window size */
			auto& GLFWobject = g_AbsoluteParent()->g_GLFWobject();
			const utils::Vec2 titlebarSizeY = utils::Vec2{ 0.0f, GLFWobject->g_TitleBarRect().g_Size().y };
			const utils::Vec2 wantedSize = m_BlockSize * m_BoardSize;
			
			GLFWobject->s_Size((titlebarSizeY + wantedSize));

			/* Align size with client rect's */
			s_Size(g_AbsoluteParent()->g_GLFWobject()->g_ClientAreaRect().g_Size());

			/* Bind events */
			g_EventHandler()->Subscribe<evts::Render>(this, &self::Game::renderBoard);
			g_EventHandler()->Subscribe<evts::BeforeRender>(this, &self::Game::blockBoardRotateBlock);
			g_EventHandler()->Subscribe<evts::BeforeRender>(this, &self::Game::clearBoardLines);
			g_EventHandler()->Subscribe<evts::BeforeRender>(this, &self::Game::generateBoardBlock);
			g_EventHandler()->Subscribe<evts::BeforeRender>(this, &self::Game::updateBoardPositionX);
			g_EventHandler()->Subscribe<evts::BeforeRender>(this, &self::Game::updateBoardPositions);
		}

		self::Game::~Game() = default; 
	/* Board control */
		void self::Game::clearBoardLines(evts::BeforeRender&) {
			std::vector<size_t> linesToClear;

			/* Get to clear */
			for (int _y = 0; _y < m_Board.size(); _y++) { 
				bool clearThisLine = true; 
				for (int _x = 0; _x < m_Board[_y].size(); _x++) 
				{
					if (m_Board[_y][_x] == 0) clearThisLine = false;
				}
				if (clearThisLine) linesToClear.push_back(_y);
			}
			/* Clear */
			std::vector<std::vector<uint16_t>> newBoard; 

			for (auto _l : linesToClear) newBoard.push_back(std::vector<uint16_t>(m_BoardSize.x, 0));

			size_t yIterator = 0;
			for (auto& _line : m_Board) {
				bool pushThisLine = true; 
				for (auto& _l : linesToClear) {
					if (_l == yIterator) pushThisLine = false; 
				}
				if (pushThisLine) {
					newBoard.push_back(_line);
				}
				yIterator++;
			}
			/* Update board */
			m_Board = newBoard;
		}

		const std::vector<std::vector<uint16_t>> self::Game::createBoard() const {
			std::vector<std::vector<uint16_t>> newBoard = {}; 

			for (int y = 0; y < static_cast<int>(m_BoardSize.y); y++) {
				std::vector<uint16_t> currentYPlacement; 
				for (int x = 0; x < static_cast<int>(m_BoardSize.x); x++) {
					currentYPlacement.push_back(0);
				} 
				newBoard.push_back(currentYPlacement);
			};
			return newBoard;
		}

		void self::Game::generateBoardBlock(evts::BeforeRender& br) {
			if (m_CurrentBlockPatern.size() != 0) return; 

			std::srand(static_cast<unsigned>(std::time(nullptr)));
			m_CurrentBlockPatern = m_BlockTypes[rand() % m_BlockTypes.size()];
			//updatedPosForBlockBoard(0.0f, 0.0f);
			
			m_BlockBoardAnimX->s_Starting(0.0f);
			m_BlockBoardAnimX->s_Wanted(0.0f);
			m_BlockBoardAnimX->startAnimation();
			m_BlockBoardAnimY->s_Starting(0.0f);
			m_BlockBoardAnimY->s_Wanted(0.0f);
			m_BlockBoardAnimY->startAnimation();

			m_CurrentBlockPosition = 0.0f;

			putBlockIntoFrame();
		}

		void self::Game::updatedPosForBlockBoard(const utils::Vec2& _Old, const utils::Vec2& _New) {
			if (_New.x != _Old.x) {
				m_BlockBoardAnimX->s_Starting(_Old.x);
				m_BlockBoardAnimX->s_Wanted(_New.x);
				m_BlockBoardAnimX->startAnimation();
			}
			if (_New.y == _Old.y) return; 
			m_BlockBoardAnimY->s_Starting(_Old.y);
			m_BlockBoardAnimY->s_Wanted(_New.y);
			m_BlockBoardAnimY->startAnimation();
		}

		void self::Game::updateBoardPositionX(evts::BeforeRender& br) {
			GLFWwindow* window = g_AbsoluteParent()->g_GLFWobject()->g_Object();
			
			/* Check for pressed keys associated with x axis movement */
			const bool dPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
			const bool aPressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
			const bool anyPressed = (aPressed || dPressed);
			
			auto is_CollidingFrom = [&](int dir) -> const bool {
				for (int _y = 0; _y < m_BlockBoard.size(); _y++) { for (int _x = 0; _x < m_BlockBoard[_y].size(); _x++) {
					if (m_BlockBoard[_y].size() > (_x + dir) && (_x + dir) >= 0) {
						if (m_BlockBoard[_y][_x] == 1 && m_Board[_y][_x + dir] == 1) return true; 
					}}};
				return false; 
			};

			/* Update held status */
			if (!anyPressed) {
				m_UpdatePositionXHeldButtonLastFrame = false; return; 
			}

			bool updatedTime = false; 
			/* Update time */
			if (anyPressed && !m_UpdatePositionXHeldButtonLastFrame) {
				m_UpdatePositionXHeldButtonSince = ccl::now();
				updatedTime = true; 
			}	
			m_UpdatePositionXHeldButtonLastFrame = true; 

			/* Get moving direction */
			int moveDir = 0; 
			if (dPressed) moveDir++;
			if (aPressed) moveDir--;

			int longestIndexType = 0; 
			for (auto _t : m_CurrentBlockPatern)
				for (size_t _Index = 0; _Index < _t.size(); _Index++) {
					if (_Index > longestIndexType && _t[_Index] == 1) longestIndexType = _Index;
				}

			int positionX = std::max(0, std::min((int)m_Board[0].size() - longestIndexType - 1, (int)m_CurrentBlockPosition.x + moveDir));
			const utils::Vec2 newPos = utils::Vec2(positionX, m_CurrentBlockPosition.y);

			if (is_CollidingFrom(moveDir)) {
				m_UpdatePositionXHeldButtonSince = ccl::now(); 
				
				return;
			}

			/* UPdate */
			if (updatedTime) {
				updatedPosForBlockBoard(m_CurrentBlockPosition, newPos);
				m_CurrentBlockPosition = newPos;
				putBlockIntoFrame();
			}
			/* Shifting: check for time */
			if (chrono::duration_cast<chrono::milliseconds>(ccl::now() - m_UpdatePositionXHeldButtonSince).count()
				< m_MsPerBlockShift) return; 

			/* Update */
			updatedPosForBlockBoard(m_CurrentBlockPosition, newPos);
			m_UpdatePositionXHeldButtonSince = ccl::now();
			m_CurrentBlockPosition = newPos;	
			putBlockIntoFrame();
		}

		void self::Game::blockBoardRotateBlock(evts::BeforeRender&) {
			const bool rotateKeyHeld = glfwGetKey(g_AbsoluteParent()->g_GLFWobject()->g_Object(), GLFW_KEY_F) == GLFW_PRESS;

			if (m_RotateButtonHeldLastFrame && rotateKeyHeld) return;

			if (m_RotateButtonHeldLastFrame && !rotateKeyHeld) {
				m_RotateButtonHeldLastFrame = false; return; 
			}

			if (!m_RotateButtonHeldLastFrame && !rotateKeyHeld) return;

			m_RotateButtonHeldLastFrame = true; 
			
			/* Chat gpt code :)*/
			auto rotate90Clockwise = [&](const std::vector<std::vector<uint16_t>>& matrix) -> std::vector<std::vector<uint16_t>> {
				int rows = matrix.size();
				int cols = matrix[0].size();

				// Create a new matrix with swapped dimensions and correct type
				std::vector<std::vector<uint16_t>> rotated((size_t)cols, std::vector<uint16_t>(rows));

				// Fill the rotated matrix with elements from the original matrix
				for (int i = 0; i < rows; ++i) {
					for (int j = 0; j < cols; ++j) {
						rotated[j][rows - i] = matrix[i][j];
					}
				}

				return rotated;
			};
			std::vector<std::vector<uint16_t>> newPatern = rotate90Clockwise(m_CurrentBlockPatern);
			size_t newPaternLongestX = 0; 
			for (auto& _v : newPatern) {
				for (size_t w : _v) {
					if (w > newPaternLongestX) newPaternLongestX = w; 
				}
			}
			std::cout << "Longest x: " << newPaternLongestX << "\n";

			std::vector<std::vector<uint16_t>> patternCopy = m_CurrentBlockPatern;

			size_t oldPatternLongestX = 0; 
			for (auto &_v : patternCopy) {
				for (size_t w : _v) {
					if (w > oldPatternLongestX) oldPatternLongestX = w; 
				}
			}


			const size_t patternSizeDif = std::min(size_t(0), size_t(m_BlockBoard[0].size() - 1 - m_CurrentBlockPosition.x + newPaternLongestX));
			std::cout << patternSizeDif << "\n";


			m_CurrentBlockPosition = (utils::Vec2(std::max(size_t(0), (size_t)m_CurrentBlockPosition.x + patternSizeDif), m_CurrentBlockPosition.y));
			
			if (m_CurrentBlockPosition.x == 0) {
				m_CurrentBlockPosition.x = 1.0f;
			}
			
			m_CurrentBlockPatern = rotate90Clockwise(m_CurrentBlockPatern);

			if (is_BlockBoardColliding()) {
				m_CurrentBlockPatern = patternCopy;
			}
		};

		void self::Game::updateBoardPositions(evts::BeforeRender& br) {
			int timeWentFromLastUpdateMs = chrono::duration_cast<chrono::milliseconds>(
				ccl::now() - m_LastBlockUpdate
				).count();

			/* Check if need's updating */
			if (timeWentFromLastUpdateMs < m_MsPerUpdate) return; 
			
			std::cout << g_FurthestYInBlockBoard() << "d \n";
			if (g_FurthestYInBlockBoard() == (m_BlockBoard.size() - 1)) {
				combineBoards();
				m_CurrentBlockPatern = {};
				return;
			}

			/* Collision check */
			if (is_BlockBoardColliding()) {
				combineBoards();
				m_CurrentBlockPatern = {}; 
				return; 
			}

			/* Update */ 
			updatedPosForBlockBoard(m_CurrentBlockPosition, m_CurrentBlockPosition + utils::Vec2(0, 1));
			m_CurrentBlockPosition = (m_CurrentBlockPosition + utils::Vec2(0, 1));
			m_LastBlockUpdate = ccl::now();

			putBlockIntoFrame();
		}

		void self::Game::putBlockIntoFrame() {
			if (m_Board.size() == 0 || m_Board[0].size() == 0) return; 

			auto g_BoardPosition = [&](const utils::Vec2& _Pos) -> utils::Vec2 {
				return m_CurrentBlockPosition + _Pos;
			};
			/* Override block board */
			m_BlockBoard = createBoard();
			
			for (size_t _y = 0; _y < m_CurrentBlockPatern.size(); _y++) { for (size_t _x = 0; _x < m_CurrentBlockPatern[_y].size(); _x++) {
				const utils::Vec2 currentPos = g_BoardPosition(utils::Vec2(_x, _y));

				m_BlockBoard[static_cast<size_t>(currentPos.y)][static_cast<size_t>(currentPos.x)] =
					m_CurrentBlockPatern[static_cast<size_t>(_y)][static_cast<size_t>(_x)];
			}}
		}

		void self::Game::combineBoards() {
			for (int _y = 0; _y < m_BlockBoard.size(); _y++) { for (int _x = 0; _x < m_BlockBoard[_y].size(); _x++) {
				if (m_BlockBoard[_y][_x] == 1) {
					m_Board[_y][_x] = 1;
				}
			}};
		}

		const bool self::Game::is_BlockBoardColliding() const {
			for (int _y = 0; _y < m_BlockBoard.size(); _y++) { for (int _x = 0; _x < m_BlockBoard[_y].size(); _x++) {
				if (m_Board.size() > (_y + 1) && m_Board[(_y + 1)][_x] == true && m_BlockBoard[_y][_x] == 1) return true;
			}};
			return false; 
		}
	/* Rendering */
		void self::Game::renderBoard(evts::Render& _r) {
			components::SimpleDC canvas{this};
			/* Draw bg */
			canvas.putTexture(0.0f, g_FixedSize(), m_BGTexture->g_GLTextureID());

			/* Draw buffer */
			for (int _y = 0; _y < m_Board.size(); _y++) { for (int _x = 0; _x < m_Board[_y].size(); _x++) {
				if (m_Board[_y][_x] == static_cast<uint16_t>(1) || m_BlockBoard[_y][_x] == static_cast<uint16_t>(1)) {

					/* Get position */
					utils::Vec2 texturePosition = (utils::Vec2(_x, _y) * m_BlockSize);
					if (m_BlockBoard[_y][_x] == static_cast<uint16_t>(1)) {

						texturePosition = (utils::Vec2(m_BlockBoardAnimX->g_Animated(m_MsPerBlockShift)[0], m_BlockBoardAnimY->g_Animated(m_MsPerUpdate)[0])
							- (m_CurrentBlockPosition - utils::Vec2(_x, _y))
							) * m_BlockSize;
					}

					/* Draw texture */
					canvas.putTexture(texturePosition, m_BlockSize, m_BlockTexture->g_GLTextureID());
				}
			}};

			//canvas.putTexture(m_CurrentBlockPosition * utils::Vec2{0.0f, m_BlockSize.y}, 50.0f, m_BlockTexture->g_GLTextureID());
		}
	/* Helpers */
		const size_t self::Game::g_FurthestYInBlockBoard() const {
			size_t yIterator = 0; 
			size_t yIteratorMax = 0; 

			for (auto _yv : m_BlockBoard) {
				for (int _v : _yv) {
					if (_v == 1) {
						yIteratorMax = yIterator;
					}
				}
				yIterator++;
			};
			return yIteratorMax;
		}
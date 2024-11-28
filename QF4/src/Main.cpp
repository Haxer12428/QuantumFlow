#include "QF/QF.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Examples/Paint/QFExamplePaint.h"
#include "Examples/Tetris/QFExampleTetris.h"


namespace utils = QF::Utils;
namespace components = QF::UI::Components;
/*

float calculateAngle(float x1, float y1, float x2, float y2) {
	// Calculate the angle in radians using atan2
	float angle_rad = std::atan2(y2 - y1, x2 - x1);

	// Convert the angle to degrees
	float angle_deg = angle_rad * (180.0f / M_PI);

	return angle_deg;
}

utils::Vec2 directionFromAngle(float angle_deg) {
	// Convert angle to radians
	float angle_rad = angle_deg * (M_PI / 180.0f);

	// Calculate the direction vector (unit vector)
	utils::Vec2 dir;
	dir.x = std::cos(angle_rad);
	dir.y = std::sin(angle_rad);
	return dir;
}

utils::Vec2 moveInDirection(const utils::Vec2& v, float angle_deg, float distance) {
	// Get the direction vector from the angle
	utils::Vec2 direction = directionFromAngle(angle_deg);

	// Scale the direction vector by the distance
	direction.x *= distance;
	direction.y *= distance;

	// Add the scaled direction vector to the original vector
	utils::Vec2 newVec = { v.x + direction.x, v.y + direction.y };

	return newVec;
}

struct NodeConfig
{
	float n_Range = 125;
};

NodeConfig nConfig; 

std::vector<utils::Vec2> m_NodePositions; 
uint64_t m_NodeID = 0;

components::Panel* mainNodePanel = nullptr;

components::Built::Button::Hints buttonhints;


struct CreateNewDescriptionWindow {
	std::string m_Name; 
	std::string m_Description;
};

CreateNewDescriptionWindow nextDescWindowParams; 
bool createNextDescWindow = false;

class DescriptionWindow : public components::Window {
public:
	virtual ~DescriptionWindow() = default;

	DescriptionWindow(QF::UI::App* _App, const std::string& _Description, const std::string& _Name) : 
		components::Window(_App, { __QF_DONT_CARE }, { 400, 400 }, _Name) {
		g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);

	} 


};

DescriptionWindow* descWindow = nullptr;

class PanelNode : public components::Built::Button
{
public:
	virtual ~PanelNode()
	{
		printf(" panel node has destructed\n");
	}

	PanelNode(components::Panel* _Parent, PanelNode* _ParentPanelNode = nullptr, bool _ConnectorNode = false) : components::Built::Button{ _Parent, buttonhints }, m_MainFrame{ _Parent },
		m_ParentPanelNode{ _ParentPanelNode }, m_IsConnectorNode{ _ConnectorNode }
	{
		s_Size({ 20.0f });
		if (_ConnectorNode) {
			s_Size({0.0f} );
			m_Hints.m_BGActiveColor = ImColor(0, 0, 0, 0);
			m_Hints.m_BGColor = ImColor(0, 0, 0, 0);
		}
		else
		{
	
		}

		m_NodeID++;

		PanelNode* m_ParentIntoNode = this;

		if (_ParentPanelNode != nullptr) {
			m_ParentIntoNode = _ParentPanelNode;
			m_NodeColor = ImColor(255, 255, 255);
			_ParentPanelNode->m_Children.push_back(this);
		}
		m_ParentPanelNode = m_ParentIntoNode;

		std::cout << m_NodeID << "\n";

		applyPos();
		m_NodePositions.push_back(g_Position());

		

		g_EventHandler()->Subscribe<components::EventSystem::Events::Render>(this, &PanelNode::Render);
		g_EventHandler()->Subscribe<components::EventSystem::Events::MouseClickEvent>(this, &PanelNode::ButtonClickCallback);

		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Created node.");

		m_CurrentID = m_NodeID;
	};

	PanelNode* m_ConnectorNode = nullptr;
	uint64_t m_CurrentID;
	ImColor m_NodeColor = ImColor(0, 128, 255);
	PanelNode* m_ParentPanelNode = nullptr;
	components::Panel* m_MainFrame;
	bool m_IsConnectorNode = false; 
	std::vector<PanelNode*> m_Children; 

	

	std::string m_Name = "QF node";
	std::string m_Description = "QF node description\n second line";
private:
	void ButtonClickCallback(components::EventSystem::Events::MouseClickEvent& _r) {
		nextDescWindowParams = { m_Name, m_Description };
		createNextDescWindow = true; 
	}

	void Render(components::EventSystem::Events::Render& _r) {
		components::SimpleDC canvas{this};
		
		components::SimpleDC canvasForMainFrame{m_MainFrame};
		
		if (!m_IsConnectorNode) {
			canvasForMainFrame.putText(g_CenterPosition() + utils::Vec2{ImGui::CalcTextSize(m_Name.c_str()).x / 2.0f * -1.0f, g_Size().y / 2.0f + 5.0f}, m_Name, ImColor(255, 255, 125));
		}

		if (m_ParentPanelNode != this) {
			utils::Vec2 pos = g_CenterPosition();
			utils::Vec2 finalPos = m_ParentPanelNode->g_CenterPosition();

			if (!m_IsConnectorNode) {
				pos = moveInDirection(pos, calculateAngle(pos.x, pos.y, finalPos.x, finalPos.y), 20);
			}

			if (!m_ParentPanelNode->m_IsConnectorNode) {
				finalPos = moveInDirection(finalPos, calculateAngle(finalPos.x, finalPos.y, g_CenterPosition().x, g_CenterPosition().y), 20);

			};

			//canvasForMainFrame.putLine(pos, finalPos, ImColor(128, 128, 128, 50), 1);
			canvasForMainFrame.putLine(pos, finalPos, ImColor(128, 128, 128, 50), 1);
		}
	};

	void applyPos() {
		if (m_ParentPanelNode == this) {
			printf("Set dif parent\n");
			const utils::Vec2 screenCenter = m_MainFrame->g_Size() / 2;

			s_Position(screenCenter);
			return; 
		}
		utils::Vec2 firstFree;
		int freeCount = 0;


		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				if (!(x == 1 && y == 1))
				{
					utils::Vec2 currentPos = (
						m_ParentPanelNode->g_Position() - utils::Vec2{nConfig.n_Range}
					);


					const float n_RangeTotal = nConfig.n_Range * 2;

					currentPos = (currentPos
						+ (QF::Utils::Vec2{ n_RangeTotal / 2.0f } *QF::Utils::Vec2{static_cast<float>(x), static_cast<float>(y)}
					));

					bool positionMatched = false;
					for (const utils::Vec2& _NodePos : m_NodePositions) {
						if (_NodePos == currentPos) {
							positionMatched = true;
						}
					}


						if (!positionMatched) {
							if (freeCount == 0) {
								firstFree = currentPos;
							}
							freeCount++;
						};
					
				}
			}
		}
		if (freeCount == 1 && !m_IsConnectorNode) {
			PanelNode* connectorNode = new PanelNode(m_MainFrame, m_ParentPanelNode, true);
			connectorNode->m_NodeColor = ImColor(60, 60, 60);
			m_ParentPanelNode->m_ConnectorNode = connectorNode;
			m_ParentPanelNode = connectorNode;
			applyPos();
		}
		else {
			s_Position(firstFree);
		}
		if (freeCount == 0) {
			m_ParentPanelNode = m_ParentPanelNode->m_ConnectorNode;
			applyPos();
		}
	}
 
};

long long descWindowImmutableID = -1;


class MainFrame : public components::Panel {
public:
	MainFrame(components::Window* _Parent) : components::Panel(_Parent, { 0.0f }, { 100000.0f }, components::Panel::Flags::m_DontCareAboutClipRectWhenRendering) {
		utilAddPreRenderCallback([=](components::Panel* _Panel) {
			GLFWwindow* window = _Parent->g_GLFWobject()->g_Object();

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
				_Panel->s_Position(_Panel->g_Position() + utils::Vec2{0, 4.0f});
			}

			// Check if the DOWN arrow key is pressed
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				_Panel->s_Position(_Panel->g_Position() - utils::Vec2{0, 4.0f});
			}

			// Check if the LEFT arrow key is pressed
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				_Panel->s_Position(_Panel->g_Position() + utils::Vec2{4.0f, 0});
			}

			// Check if the RIGHT arrow key is pressed
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				_Panel->s_Position(_Panel->g_Position() - utils::Vec2{4.0f, 0});
			}

		});

		g_EventHandler()->Subscribe< components::EventSystem::Events::Render>(this, &MainFrame::renderCallback);
	}

private:
	void renderCallback(components::EventSystem::Events::Render& r) {
		if (!createNextDescWindow) return;

		if (descWindow != nullptr) {
			QF::UI::Components::Window* win = g_AbsoluteParent()->g_Application()->g_WindowHandler()->g_Window(descWindowImmutableID);
			if (win != nullptr) {
				win->g_GLFWobject()->destroy();
			}
		}

		descWindow = new DescriptionWindow(g_AbsoluteParent()->g_Application(), "a", "d");
		descWindowImmutableID = descWindow->g_ImmutableID();

		descWindow->g_GLFWobject()->s_VsyncState(false);

		createNextDescWindow = false;
	}
};


class Window : public components::Window
{
public:
	Window(QF::UI::App* _App) : components::Window(_App, { __QF_DONT_CARE }, { 1080, 1000 }, "QF Api Nodes") {
		g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);

		buttonhints.m_PanelFlags = QF::UI::Components::Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero;
		buttonhints.m_BGColor = ImColor(60, 60, 60, 255);
		buttonhints.m_BGActiveColor = ImColor(255, 255, 255, 255);
		
		g_GLFWobject()->s_VsyncState(false);

		components::Panel* mainFrame = new MainFrame(this);
		

		PanelNode* nodeQF = new PanelNode(mainFrame);
		nodeQF->m_Name = "QF";
		nodeQF->m_Description = "QF's one and only at the time namespace";

		PanelNode* nodeCore = new PanelNode(mainFrame, nodeQF);
		nodeCore->m_Name = "Core";
		nodeCore->m_Description = "QF's core system, initializing everything";

		// Adding the 'initQF' function as a namespace under the Core node
		PanelNode* nodeCoreInit = new PanelNode(mainFrame, nodeCore);
		nodeCoreInit->m_Name = "initQF";
		nodeCoreInit->m_Description = "Initializes QF system";
		
		PanelNode* nodeUI = new PanelNode(mainFrame, nodeQF);
		nodeUI->m_Name = "UI";

		PanelNode* nodeComponents = new PanelNode(mainFrame, nodeUI);
		nodeComponents->m_Name = "Components";

		PanelNode* nodePanel = new PanelNode(mainFrame, nodeComponents);
		nodePanel->m_Name = "Panel";

		PanelNode* nodeWindow = new PanelNode(mainFrame, nodeComponents);
		nodeWindow->m_Name = "Window";

		PanelNode* nodeApp = new PanelNode(mainFrame, nodeUI);
		nodeApp->m_Name = "App";
		
		PanelNode* nodeWindowHandler = new PanelNode(mainFrame, nodeApp);
		nodeWindowHandler->m_Name = "WindowHandler";

		PanelNode* nodeSimpleDC = new PanelNode(mainFrame, nodeComponents);
		nodeSimpleDC->m_Name = "SimpleDC";

		PanelNode* nodeBuilt = new PanelNode(mainFrame, nodeComponents);
		nodeBuilt->m_Name = "Built";

		PanelNode* nodeButton = new PanelNode(mainFrame, nodeBuilt);
		nodeButton->m_Name = "Button";

		PanelNode* nodeEventSystem = new PanelNode(mainFrame, nodeComponents);
		nodeEventSystem->m_Name = "EventSystem";

		PanelNode* nodeElement = new PanelNode(mainFrame, nodeComponents);
		nodeElement->m_Name = "Element";

		PanelNode* nodeGLFWObjet = new PanelNode(mainFrame, nodeWindow);
		nodeGLFWObjet->m_Name = "GLFWObject";

		PanelNode* nodeCustomTitleBar = new PanelNode(mainFrame, nodeGLFWObjet);
		nodeCustomTitleBar->m_Name = "CustomTitleBar";

		PanelNode* nodeUtils = new PanelNode(mainFrame, nodeQF);
		nodeUtils->m_Name = "Utils";

		PanelNode* nodeEvent = new PanelNode(mainFrame, nodeEventSystem);
		nodeEvent->m_Name = "Event";

		PanelNode* nodeEventEvent = new PanelNode(mainFrame, nodeEvent);
		nodeEventEvent->m_Name = "Event(obj)";

		PanelNode* nodeEventHandler = new PanelNode(mainFrame, nodeEventSystem);
		nodeEventHandler->m_Name = "EventHandler";

		PanelNode* nodeEventSubscribe = new PanelNode(mainFrame, nodeEventHandler);
		nodeEventSubscribe->m_Name = "Subscribe";

		PanelNode* nodeEventDispatch = new PanelNode(mainFrame, nodeEventHandler);
		nodeEventDispatch->m_Name = "Dispatch";

		PanelNode* nodeEvents = new PanelNode(mainFrame, nodeEventSystem);
		nodeEvents->m_Name = "Events";

		PanelNode* nodeMEvent = new PanelNode(mainFrame, nodeEvents);
		nodeMEvent->m_Name = "MouseEvent";

		PanelNode* nodeMMEvent = new PanelNode(mainFrame, nodeMEvent);
		nodeMMEvent->m_Name = "MouseMotionEvent";

		PanelNode* nodeMCEvent = new PanelNode(mainFrame, nodeMEvent);
		nodeMCEvent->m_Name = "MouseClickEvent";

		PanelNode* nodeREvent = new PanelNode(mainFrame, nodeEvents);
		nodeREvent->m_Name = "RenderEvent";

		PanelNode* nodePAEvent = new PanelNode(mainFrame, nodeEvents);
		nodePAEvent->m_Name = "PanelAssignedEvent";

		PanelNode* nodeBasicAnim = new PanelNode(mainFrame, nodeUtils);
		nodeBasicAnim->m_Name = "BasicAnim";

		PanelNode* nodeCompare = new PanelNode(mainFrame, nodeUtils);
		nodeCompare->m_Name = "Compare";

		PanelNode* nodeDebug = new PanelNode(mainFrame, nodeUtils);
		nodeDebug->m_Name = "Debug";

		PanelNode* nodeIdMenager = new PanelNode(mainFrame, nodeUtils);
		nodeIdMenager->m_Name = "IdMenager";

		PanelNode* nodeImage = new PanelNode(mainFrame, nodeUtils);
		nodeImage->m_Name = "Image";

		PanelNode* nodeMath = new PanelNode(mainFrame, nodeUtils);
		nodeMath->m_Name = "Math";

		PanelNode* nodeRect = new PanelNode(mainFrame, nodeUtils);
		nodeRect->m_Name = "Rect";

		PanelNode* nodeSystem = new PanelNode(mainFrame, nodeUtils);
		nodeSystem->m_Name = "System";

		PanelNode* nodeVec2 = new PanelNode(mainFrame, nodeUtils);
		nodeVec2->m_Name = "Vec2";



		mainFrame->s_Position((mainFrame->g_Size() / 2.0f - g_GLFWobject()->g_ClientAreaRect().g_Size() / 2.0f) * -1.0f);
		//mainNodePanel->s_Position(mainNodePanel->g_Size() - g_GLFWobject()->g_ClientAreaRect().g_Size());
	};
};

*/

#include "QF/Experimental/FileDialog/QFExperimentalFileDialog.h"

std::vector<std::string> getFileLines(const std::string& filePath) {
	std::vector<std::string> lines;
	std::ifstream file(filePath);
	std::string line;

	if (file.is_open()) {
		while (std::getline(file, line)) {
			lines.push_back(line);
		}
		file.close();
	}
	else {
		std::cerr << "Unable to open file: " << filePath << std::endl;
	}

	return lines;
}


using ccl = std::chrono::high_resolution_clock;

class Application : QF::UI::App
{
public:
	Application()
		: QF::UI::App()
	{};

	const bool onInit() override {
		const std::filesystem::path enginedir = utils::Filesystem::g_InCurrentDirectory("src/QF");

		if (std::filesystem::is_directory(enginedir)) {
			uint64_t lineCount = 0; 

			for (const std::filesystem::directory_entry& _Entry : std::filesystem::recursive_directory_iterator(
				enginedir
			)) {
				if (_Entry.is_regular_file()) {
					const std::string entryExtension = _Entry.path().extension().string();
					if (entryExtension == ".h" || entryExtension == ".cpp" || entryExtension == ".hpp") {
						lineCount += getFileLines(_Entry.path().string()).size();
					}
				}
			}

			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, std::format("Warning: You are a nerd with no life, engine has: {} lines.", lineCount));
		}

		//Window* win = new Window(this); // api nodes 
		/* Might come back to this someday, i dont want to code it cause its boring as fuck 
			na i dont mean hard, the only hard part was optimizing texture loading i think but thats fixed
			and loading the texture, thats also fixed. (QFTexture): i have no clue why soil can read visual studio's 
			resource but cannot read the same exact thing in the exact format. Fixed it by loading it opengl 
			Might rebuilt texture system later on to add flags and kinda 'replicate' soil but the one that works 
			probably a skill issue but ngl 
		*/
		//new QFExamplePaint(this);

		/* i dont really want to code rest of this shit, kinda works i coded worst part now its boooring*/
		//new QFExampleTetris(this);

		QF::UI::Components::Window* win = new QF::Experimental::FileDialog(this, "C:\\");
		win->g_GLFWobject()->s_VsyncState(false);


		return this->Execute();
	}
};

/* 
	OPENING WINDOWS MIGHT BE PROBLEMATIC? WHY STACK (SAME AS ON PANEL)!!!!
		ATLEAST THAT MIGHT BE THE CAUSE: 
		FOR FUTURE ME: LOOK INTO THIS PLEASE AS U FIGURED IT NOW 
			I WILL HATE MYSELF FOR THAT COMMENT IF I IMPLEMENT WHOLE SYSTEM FOR NOTHING
	
*/

int main()
{
	__QF_INIT();
	QF::Utils::Debug::s_ToAllPrintHints(true);


	QF::UI::App::implementApplication<Application>();

}
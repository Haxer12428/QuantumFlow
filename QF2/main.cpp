#include "QF/QF.h"
#include <random>



class Calculator : public QF::UI::Panel
{
public:
 Calculator(QF::UI::Window* _Window)
    : QF::UI::Panel(_Window, {0, 0}, _Window->g_Size())
 {
    g_EventHandler()->Subscribe<QF::UI::EventSystem::RenderEvent>(this,&Calculator::hk_Render);
   
   func_CreateSlots();
 }

 void func_CreateSlots()
 {
    std::srand(static_cast<unsigned>(std::time(0)));

    QF::UI::Components::Button::Hints _Hints;
    _Hints.m_Parent = this; 
    _Hints.m_Size = {20,20};
    _Hints.m_Position = {0, 0};
    _Hints.m_ColorDefault = ImColor(20, 20, 20);
    _Hints.m_ColorActivated = ImColor(90, 90, 90);
    _Hints.m_ImageSizeFactor = 0.25f;
    _Hints.m_DrawText = true;

    for (int _x = 0; _x < m_ButtonGrid[0]; _x++)
    {
        for (int _y = 0; _y < m_ButtonGrid[1]; _y++)
        {
            m_Buttons.push_back(new QF::UI::Components::Button(_Hints));
        }
    }

    
    // Define the distribution (uniform distribution between 1 and 100)


    int _BombCount = 0;

    for (size_t _i = 0; _i < m_Buttons.size(); _i++)
    {   

        // Generate a random integer between 1 and 100
        int _RandomNum = std::rand() % 10; // rand() % (max - min + 1) + min
        bool _IsBomb = _RandomNum == 0;

        if (_IsBomb)
        {
            m_Bombs.push_back(_i);
            
            _BombCount++;
        }
    }

    size_t _Iterator = 0; 
    for (auto _Button : m_Buttons)
    {
        

        std::cout << "assigned cllback\n";
        _Button->func_SetCallback([&](QF::UI::Components::Button* _panel) {
            if (m_GameOver) return; 

            size_t _At = 0;

            for (auto _Bt : m_Buttons)
            {
                if (_panel->g_Id() == _Bt->g_Id()) break;
                _At++;
            }

            for (const size_t _Bomb : m_Bombs)
            {
                if (_Bomb == _At)
                {
                    /* BOOM FUCKER */ m_GameOver = true; return;
                }
            }
            /* Im good 0-0 */
            int _DestroyRange = 1;

            const int _row = (_At / m_ButtonGrid[0]);
            const int _inrow = (_At - (_row * m_ButtonGrid[0]));

            ImU32 _ColorNew = ImColor(0,255,0);
            _panel->m_Hints.m_ColorActivated = _ColorNew;
            _panel->m_Hints.m_ColorDefault = _ColorNew;

            for (int _rangex = -_DestroyRange; _rangex <= _DestroyRange; _rangex++)
            {
                 for (int _rangey = -_DestroyRange; _rangey <= _DestroyRange; _rangey++)
                {   
                    const size_t _Index = g_Index(_inrow + _rangex, _row + _rangey);
                    if (_Index != -1)
                    {
                        bool _IsBomb = false;
                        for (const size_t _Bomb : m_Bombs)
                        {
                            if (_Bomb == _Index) { _IsBomb = true; m_BombsUnrevealed++;
                            break;}
                        }
                        const ImU32 _Clr = (_IsBomb ? ImColor(255, 0, 0) : ImColor(0, 255, 0));

                        m_Buttons[_Index]->m_Hints.m_ColorDefault = _Clr;
                        m_Buttons[_Index]->m_Hints.m_ColorActivated = _Clr;

                        m_Buttons[_Index]->func_SetCallback([&](QF::UI::Components::Button*){});

                        m_Score += 100;
                    }
                }
            }

            _panel->func_SetCallback([&](QF::UI::Components::Button*){});
            m_Score += 100;
        });
     _Iterator++;
    }
 }

private:
 size_t g_Index(const int& _inrow, const int& row) const 
 {
    const size_t _Pos = (_inrow + (row * m_ButtonGrid[0]));
    if (_Pos < 0) return -1;
    if (_Pos < m_Buttons.size()) return _Pos;
    return -1;
 }

 void func_AssignSelfPlacement()
 {
    s_Size(g_AbsoluteParent()->g_FixedSize());
 }

 void func_AssignButtonsPlacement()
 {
    const float _ButtonSpacing = 2.0f;

    const QF::Utils::Vec2 _ButtonsStart = {0.0f, m_TopSpaceY + _ButtonSpacing};
    const QF::Utils::Vec2 _ButtonsBB = (g_Size() - _ButtonsStart - 
        QF::Utils::Vec2{_ButtonSpacing * (float)(m_ButtonGrid[0] + 1), _ButtonSpacing * (float)(m_ButtonGrid[1] + 1)}
    );

    const QF::Utils::Vec2 _ButtonSize = {_ButtonsBB.g_X() / (float)m_ButtonGrid[0],
         _ButtonsBB.g_Y() / (float)m_ButtonGrid[1] };


    for (size_t _Iterator = 0; _Iterator < m_Buttons.size(); _Iterator++)
    {
        auto _Button = m_Buttons[_Iterator];

        const int _y = (_Iterator / m_ButtonGrid[0]);
        const int _x = (_Iterator - (_y * m_ButtonGrid[0]));
        
        const float _cx = (
            _ButtonSpacing +  _ButtonsStart.g_X() + ((_ButtonSize.g_X() + _ButtonSpacing)* _x)
        );

         const float _cy = (
            _ButtonSpacing + _ButtonsStart.g_Y() + ((_ButtonSize.g_Y() + _ButtonSpacing)* _y)
        );

        _Button->s_Position({_cx, _cy});
        _Button->s_Size(_ButtonSize);
    }


 }

 void hk_Render(QF::UI::EventSystem::RenderEvent& _Event) 
 {
   func_AssignSelfPlacement();
      func_AssignButtonsPlacement();

   QF::UI::SimpleDC _Canvas{this};

    std::string _Display = (m_GameOver ? "Game over! Restart to play again!" : "Score: "+  std::to_string(m_Score) + " || Bombs left: " + std::to_string(m_Bombs.size() - m_BombsUnrevealed));

   _Canvas.draw_Text(QF::Utils::Math::g_TextCenteredPosition(
    {0, 0}, {g_Size().g_X(), m_TopSpaceY}, _Display
   ), ImColor(255,255,255,255), _Display);
 } 
 
 const float m_TopSpaceY = 20.0f; 
 std::array<int, 2> m_ButtonGrid = {20, 20};
 std::vector<QF::UI::Components::Button*> m_Buttons; 

 bool m_GameOver = false; 
 std::vector<size_t> m_Bombs;

 size_t m_BombsUnrevealed = 0;
 long m_Score = 0; 
};

class Application : public QF::UI::App
{
public:
	Application() : QF::UI::App() { Implement(); };

	bool on_Init() override
	{
		new Calculator(new QF::UI::Window(this, "QF testing: Calculator", {900.0f, 800.0f}));
	
		return true; 
	}

};

/**
 * @brief Entry point of the application.
 *
 * This function initializes debug print hints, sets global Vec2 hints,
 * initializes global hints, creates an Application instance, and runs the program.
 *
 * @return Integer value indicating the exit status of the program.
 *         Returns 0 on successful execution.
 */
int main()
{
    #ifdef _WIN64
    // Code for 64-bit architecture
    std::cout << "Built for x64 (64-bit)" << std::endl;
#else
    // Code for 32-bit architecture
    std::cout << "Built for x86 (32-bit)" << std::endl;
#endif

    QF::Utils::Debug::s_MultipleGlobalDebugPrintHints(
        {
            QF::Utils::Debug::PrintHint::PRINT_CRITICAL_ERROR,
            QF::Utils::Debug::PrintHint::PRINT_WARNING,
            QF::Utils::Debug::PrintHint::PRINT_MESSAGE,
            QF::Utils::Debug::PrintHint::PRINT_WITH_COLORS,
            QF::Utils::Debug::PrintHint::PRINT_IMPORTANT,
            QF::Utils::Debug::PrintHint::PRINT_EXCEPTION
        }, true
    );

    QF::Utils::Vec2::s_GlobalVec2Hint(QF::Utils::Vec2::Hint::DIVIDE_BY_ZERO, 
        QF::Utils::Vec2::HintValue::DIVIDE_BY_ZERO_RETURN_SELF);
    QF::Utils::Debug::global_InitializeHints();
    //printf(QF::Utils::Filesystem::Open("C:\\QF\\textd.vs").g_Buffer());

    new Application();

    return 0;
}


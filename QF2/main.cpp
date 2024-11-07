#include "QF/QF.h"


class Calculator : public QF::UI::Panel
{
public:
 Calculator(QF::UI::Window* _Window)
    : QF::UI::Panel(_Window, {0, 0}, _Window->g_Size())
 {
    g_EventHandler()->Subscribe<QF::UI::EventSystem::RenderEvent>(this,&Calculator::hk_Render);
   
    func_CreateSlots();
    m_Timer->Link(this->g_AbsoluteParent());
    m_Timer->Subscribe(this, &Calculator::hk_Timer);
    m_Timer->func_Start(std::chrono::milliseconds(250));

    printf("LINKED to window \n");
 }

 void func_CreateSlots()
 {
    
 }

 void hk_Timer(QF::UI::EventSystem::TimerEvent& _EVt)
 {
  //printf("call from window evt handler\n");
 }
private:
 void hk_Render(QF::UI::EventSystem::RenderEvent& _Event) 
 {
   
 } 

 QF::UI::EventSystem::Timer* m_Timer = new QF::UI::EventSystem::Timer();
 std::vector<QF::UI::Components::Button*> m_Buttons; 
};

class Application : public QF::UI::App
{
public:
	Application() : QF::UI::App() { Implement(); };

	bool on_Init() override
	{
		new Calculator(new QF::UI::Window(this, "QF testing: Calculator"));
	
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


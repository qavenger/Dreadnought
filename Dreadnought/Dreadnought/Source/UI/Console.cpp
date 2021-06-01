#include "pch.h"
#include "Console.h"

static Console* instance = nullptr;
Console& Console::GetInstance()
{
	if (!instance) instance = new Console();
	return *instance;
}

void Console::OnDestory()
{
	if (instance)delete instance;
	instance = nullptr;
}

void Console::Draw(const char* title, bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(title, p_open))
    {
        ImGui::End();
        return;
    }
    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            *p_open = false;
        ImGui::EndPopup();
    }

    if (ImGui::SmallButton("Clear")) { ClearLog(); }
    ImGui::SameLine();
    ImGui::End();

}

Console::Console()
    : 
    InputBuf{},
    ScrollToBottom(false),
    AutoScroll(true),
    HistoryPos(-1)
{
    ClearLog();

    // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
    Commands.push_back("HELP");
    Commands.push_back("HISTORY");
    Commands.push_back("CLEAR");
    Commands.push_back("CLASSIFY");
}

void Console::ClearLog()
{
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
}

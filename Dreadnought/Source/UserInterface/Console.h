#pragma once
#include <imgui.h>

class Console : public INoncopyable
{
    friend class Engine;
public:
	static Console& GetInstance();
    ~Console()
    {
        for (auto& command : ConsoleCommandList)
        {
            delete command.second;
            command.second = nullptr;
        }
    }
    Console();
private:

    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;
    std::vector<std::string> CommandQueue;

public:
    void Draw(const char* title, bool* p_open);
    void AddCommand(const char* cmd) { Commands.push_back(cmd); }
    void AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }
private:
    void ClearLog();
    static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }
    

    void ExecCommands();
    bool ProcessCommand(std::string cmd);
    int TextEditCallback(ImGuiInputTextCallbackData* data);


    class GenericCommand
    {
    public:
        virtual void operator()(std::vector<std::string>& Params) = 0;
    };

    template<typename LAMBDA>
    class ConsoleCommand : public GenericCommand
    {
    public:
        ConsoleCommand(LAMBDA&& lambda) : Lambda(std::forward<LAMBDA>(lambda)) {}
        virtual void operator()(std::vector<std::string>& Params) override { Lambda(Params); }
    private:
        LAMBDA Lambda;
    };

    std::map<std::string, GenericCommand*> ConsoleCommandList;
public:
    template<typename Name, typename LAMBDA>
    void AddConsoleCommand(LAMBDA&& lambda)
    {
        ConsoleCommandList[Name::GetName()] = new ConsoleCommand<LAMBDA>(std::forward<LAMBDA>(lambda));
        Console::GetInstance().AddCommand(Name::GetName());
    }

private:
    static Console* s_Instance;
    class Window* pWindow;
};



#define CreateConsoleCommand(Name) \
struct ConsoleCommand##Name{\
static const char* GetName(){ return #Name; }\
};\
Console::GetInstance().AddConsoleCommand<ConsoleCommand##Name>
// dear imgui: standalone example application for SDL2 + OpenGL
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <sqlite3.h>
#include "ImGuiColorTextEdit/TextEditor.h"

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

void DisplayTable(char **result, int rows, int cols)
{
    ImGuiTableFlags flags = 0
    | ImGuiTableFlags_Borders
    | ImGuiTableFlags_RowBg
    | ImGuiTableFlags_Resizable
// | ImGuiTableFlags_Sortable  // we would have to sort the data ourselves
    | ImGuiTableFlags_ScrollY
    ;

    if (cols>0 && ImGui::BeginTable("Result", cols, flags)) {

        for (int col=0; col<cols; col++) {
            ImGui::TableSetupColumn(result[col]);
        }
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (int row=0; row<rows; row++) {
            ImGui::TableNextRow();
            for (int col=0; col<cols; col++) {
                ImGui::TableSetColumnIndex(col);
                const char *text = result[(row+1)*cols+col];
                if (text == NULL) {
                    ImGui::TextDisabled("<NULL>");
                }else{
                    ImGui::TextUnformatted(text);
                }
            }
        }

        ImGui::EndTable();
    }
}

// Main code
int main(int argc, char**argv)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("SQL-GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_EnablePowerSavingMode;

    // Setup Dear ImGui style
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(10,10);
    style.FramePadding = ImVec2(10,4);
    style.CellPadding = ImVec2(8,4);
    style.ItemSpacing = ImVec2(8,4);
    style.ItemInnerSpacing = ImVec2(4,4);
    style.ScrollbarSize = 20;
    style.GrabMinSize = 20;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 0;
    style.WindowRounding = 5;
    style.ChildRounding = 5;
    style.FrameRounding = 3;
    style.ScrollbarRounding = 4;
    style.GrabRounding = 4;
    style.TabRounding = 4;

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("fonts/NotoSansMono-Regular.ttf", 16.0f);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    const char* db_path = argc>1 ? argv[1] : "";
    char *err_msg = NULL;
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(db_path, &db);
    if (rc) {
        fprintf(stderr, "Failed to open database %s: %s", db_path, sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    char query[1024];
    char **result = NULL;
    int result_rows = 0;
    int result_cols = 0;

    snprintf(query, sizeof(query), "%s", argc>2 ? argv[2] : "select * from sqlite_master");

    TextEditor editor;
    auto lang = TextEditor::LanguageDefinition::SQL();
    editor.SetLanguageDefinition(lang);
    editor.SetShowWhitespaces(false);
    TextEditor::Palette palette = TextEditor::GetLightPalette();
    // disable the current line highlight, by choosing transparent colors for it.
    palette[(int)TextEditor::PaletteIndex::CurrentLineFill] = 0x00000000;
    palette[(int)TextEditor::PaletteIndex::CurrentLineFillInactive] = 0x00000000;
    palette[(int)TextEditor::PaletteIndex::CurrentLineEdge] = 0x00000000;
    editor.SetPalette(palette);
    editor.SetText(query);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        ImGui_ImplSDL2_WaitForEvent();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            bool do_query = false;

            if (ImGui::GetFrameCount()==1) do_query = true;

            ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
            ImGui::Begin("Database");

            if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {

                if (ImGui::BeginTabItem("SQL")) {

                    ImVec2 size(
                        ImGui::GetContentRegionAvail().x - 100,
                        ImGui::GetTextLineHeight() * 5
                        );
                    editor.Render("SQL", size, true);
                    ImVec2 bottom_corner = ImGui::GetItemRectMax();

                    ImGui::SameLine();

                    if (ImGui::BeginChild("Query Buttons", ImVec2(100,50))) {
                        if (ImGui::Button("Run Query")) {
                            do_query = true;
                        }
                        ImGui::Text("%s+Enter", io.ConfigMacOSXBehaviors ? "Cmd" : "Ctrl");
                    }
                    ImGui::EndChild();

                    ImGuiIO& io = ImGui::GetIO();
                    auto shift = io.KeyShift;
                    auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
                    auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
                    if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter), false)) {
                        do_query = true;
                    }

                    auto cpos = editor.GetCursorPosition();
                    auto selection = editor.GetSelectedText();
                    char info_text[1024];
                    if (selection.empty()) {
                        snprintf(info_text, sizeof(info_text),
                            "line %d/%d, column %d | %s",
                            cpos.mLine + 1,
                            editor.GetTotalLines(),
                            cpos.mColumn + 1,
                            editor.IsOverwrite() ? "Ovr" : "Ins");
                    }else{
                        snprintf(info_text, sizeof(info_text),
                            "selected %d characters | %s",
                            (int)selection.length(),
                            editor.IsOverwrite() ? "Ovr" : "Ins");
                    }

                    ImVec2 pos = ImGui::GetCursorPos();
                    ImGui::SetCursorPosX(bottom_corner.x - ImGui::CalcTextSize(info_text).x);
                    ImGui::TextUnformatted(info_text);
                    // ImGui::SetItemAllowOverlap();
                    ImGui::SetCursorPos(pos);

                    if (do_query) {
                        if (err_msg) {
                            sqlite3_free(err_msg);
                            err_msg = NULL;
                        }

                        snprintf(query, sizeof(query), "%s", editor.GetText().c_str());

                        char **new_result = NULL;
                        int new_rows = 0;
                        int new_cols = 0;
                        rc = sqlite3_get_table(
                            db,
                            query,
                            &new_result,
                            &new_rows,
                            &new_cols,
                            &err_msg
                            );
                        if (rc != SQLITE_OK) {
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            if (new_result) {
                                sqlite3_free_table(new_result);
                            }
                        }else if (new_cols>64) {
                            fprintf(stderr, "Error %d > 64 columns\n", new_cols);
                            if (new_result) {
                                sqlite3_free_table(new_result);
                            }
                        }else{
                            if (result) {
                                sqlite3_free_table(result);
                                result = NULL;
                            }
                            result = new_result;
                            result_rows = new_rows;
                            result_cols = new_cols;
                        }
                    }

                    if (err_msg) {
                        ImGui::Text("%s", err_msg);
                    }

                    if (result) {
                        ImGui::Text("Result %d rows, %d cols", result_rows, result_cols);

                        DisplayTable(result, result_rows, result_cols);
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Tables")) {

                    char **result = NULL;
                    int rows = 0;
                    int cols = 0;

                    // which tables exist?
                    rc = sqlite3_get_table(
                        db,
                        "select name from sqlite_master where type='table'",
                        &result,
                        &rows,
                        &cols,
                        &err_msg
                        );
                    if (rc != SQLITE_OK) {
                        fprintf(stderr, "SQL error: %s\n", err_msg);
                        if (result) {
                            sqlite3_free_table(result);
                        }
                    }else{

                        // pick a table
                        static int selected_table_index = 0;
                        ImGui::Combo("Table", &selected_table_index, &result[1], rows);

                        static char filter[1024];
                        ImGui::InputText("Filter", filter, sizeof(filter));

                        char q[256];
                        const char *where = filter;
                        if (!strlen(where)) {
                            where = "1=1";
                        }
                        snprintf(q, sizeof(q), "select * from %s where %s", result[selected_table_index+1], where);

                        // free the list of tables
                        sqlite3_free_table(result);

                        // query the full contents of the table
                        rc = sqlite3_get_table(
                            db,
                            q,
                            &result,
                            &rows,
                            &cols,
                            &err_msg
                            );
                        if (rc != SQLITE_OK) {
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            if (result) {
                                sqlite3_free_table(result);
                            }
                        }else{
                            ImGui::Text("%d rows, %d cols", rows, cols);

                            DisplayTable(result, rows, cols);
                            sqlite3_free_table(result);
                        }
                    }


                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    sqlite3_close(db);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#if 1
#include <iostream>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui/rlImGui.h"
#include "TexRip.h"
#include "Input.h"
#include "shaders.h"
#include "oneHeaderLibs/VectorOperators.h"
//#include "fromJeffMTestframe/Application/platform_tools.h"

#define println(x) std::cout << x << std::endl;

void setup();
void draw();
void destroy();

int frameCnt = 0;

Vector2 lastMousePos;
Vector2 mouseDelta;

int main(void) {
    setup();

    while (!WindowShouldClose()) {
        draw();
    }

    destroy();

    return 0;
}

void setup() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1200, 800, "TexRip");
    
    SetWindowResizeDrawCallback(draw);
    SetTargetFPS(60);

    lastMousePos = GetMousePosition();
    mouseDelta = { 0,0 };

    SetupRLImGui(true);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsResizeFromEdges = true;
    io.WantSaveIniSettings = false;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ShaderManager::setup();
    Input::init();

    //PlatformTools::SetWindowHandle(GetWindowHandleNative());
    TexRip::TexRipper::init();
    TexRip::TexRipper::addImage("Thing", LoadTexture("assets/ressources/img2.png"));
    std::cout << GetWorkingDirectory() << std::endl;
}
void draw() {
    BeginDrawing();

    mouseDelta = GetMousePosition() - lastMousePos;

    ClearBackground(DARKGRAY);

    BeginRLImGui();

    //ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);
    TexRip::TexRipper::draw();
    //t->draw(GetMousePosition(), mouseDelta);

    ImGui::ShowDemoWindow(NULL);
    EndRLImGui();

    lastMousePos = GetMousePosition();

    EndDrawing();

    frameCnt++;
}
void destroy() {
    TexRip::TexRipper::destroy();
    ShutdownRLImGui();
    ShaderManager::destroy();
    CloseWindow();
}

#elif 0

#include "raylib.h"
#include "RenderTexture.h"

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    raylib::RenderTexture* renderTexArr[3];
    for (int i = 0; i < 3; i++) {
        renderTexArr[i] = new raylib::RenderTexture(200, 200, i);
    }
    //raylib::RenderTexture renderTex(200,200,raylib::RenderTexture::SMOOTH_1);
    //raylib::RenderTexture renderTex2(200,200,raylib::RenderTexture::NO_SMOOTH);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        for (int i = 0; i < 3; i++) {
            renderTexArr[i]->BeginTextureMode();
            ClearBackground(WHITE);

            DrawCircle(70, 70, 40, BLACK);
            DrawLineEx({ 50,130 }, { 100,442 }, 3.3f, BLACK);

            renderTexArr[i]->EndTextureMode();
        }

        ClearBackground(RAYWHITE);

        //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        
        for (int i = 0; i < 3; i++) {
            DrawTexture(renderTexArr[i]->getTex(), 50 + (10 + 200)*i, 50, WHITE);
        }
        
        //DrawTexture(renderTex2.getTex(), renderTex.getTex().width + 50 + 10, 50, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
#elif 0
#include "raylib.h"

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

#elif 1
#include "raylib.h"
#include <iostream>

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    Texture t = LoadTexture("assets/ressources/img2.png");

    if (t.id > 0) {
        std::cout << t.id << std::endl;
    }

    UnloadTexture(t);

    CloseWindow();

    return 0;
}

#elif 0
#include "imgui.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"

int main(void)
{
    const int screenWidth = 1400;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    SetupRLImGui(true);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        BeginRLImGui();

        ClearBackground(RAYWHITE);

        if (ImGui::Begin("shush")) {
            ImGui::TextUnformatted("SHUUUUUUUUSH");
        }
        ImGui::End();

        if (ImGui::Begin("shush")) {
            ImGui::TextUnformatted("LUUUUUUL");
        }
        ImGui::End();

        //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndRLImGui();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

#endif

/*


cv::Mat original_image = cv::imread("../img.jpg");

cv::Point2f src[4] = { {479, 359},{1929, 411},{586, 2046},{1949, 1916} };
cv::Point2f dst[4] = { {0,0}, {300,0}, {0,300}, {300,300} };

cv::Mat m = cv::getPerspectiveTransform(src,dst);
cv::Mat img;
cv::warpPerspective(original_image, img, m, cv::Size(300,300));
cv::imwrite("../out.jpg", img);


//DrawTextureEx(ripWin.targetTex.texture, { 0, 0 },0,0.1, WHITE);
//DrawRectangleLines(0,0,ripWin.targetTex.texture.width/10,ripWin.targetTex.texture.height/10, ORANGE);
//DrawTextureEx(ripWin.selWin.tex, { 0, (float)ripWin.targetTex.texture.height*0.1f },0,0.1, WHITE);
//DrawTextureEx(ripWin.selWin.persMats, { 0, 0 },0,10, WHITE);

//ImageSelectionViewer sourceImgViewer(LoadTexture("../img2.jpg"), "Thing", 0); //../img.jpg ImGuiWindowFlags_MenuBar raylib_512x512.png cv::imread("../img2.jpg")

enum Align {
VERTICAL = 0,
HORIZONTAL
};

while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(DARKGRAY);

        BeginRLImGui();

        if (ImGui::Begin("Image", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar)) {
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Orientation")) {
                    if (ImGui::MenuItem("Horizonzal")) {
                        sourceImgAlign = Align::HORIZONTAL;
                    }
                    if (ImGui::MenuItem("Vertical")) {
                        sourceImgAlign = Align::VERTICAL;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImVec2 imSize;
            ImVec2 cursPosOff = { 0,0 };

            if (sourceImgAlign == Align::HORIZONTAL) {
                imSize.x = size.x;
                imSize.y = size.x * (1 / sourceImg.ratio);
                cursPosOff.y = size.y / 2 - imSize.y / 2;
            }
            else if(sourceImgAlign == Align::VERTICAL){
                imSize.x = size.y * sourceImg.ratio;
                imSize.y = size.y;
                cursPosOff.x = size.x / 2 - imSize.x / 2;
            }
            ImVec2 currCursPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(currCursPos + cursPosOff);
            RLImGuiImageSize(&sourceImg.tex, imSize.x, imSize.y);
        }
        ImGui::End();

        ImGui::DockSpaceOverViewport();

        //ImGui::ShowDemoWindow(NULL);
        EndRLImGui();

        EndDrawing();
    }
*/
#ifndef _IMGUI_IMG_VIEWERS
#define _IMGUI_IMG_VIEWERS

#include "raylib.h"
#include "imgui.h"
#include <string>

class World2DViewer {
public:
    struct WinProps {
        std::string windowName;
        ImGuiWindowFlags flags;

        bool winOpen;
        bool* winOpenPtr = &winOpen;
        bool isWinHovered = false;

        ImGuiID dockID;
    };

    WinProps winProps;
    
    bool reRenderEveryFrame = true;
    bool noMoveOrZoom = false;

    World2DViewer(const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
    ~World2DViewer();

    void addWindowFlags(ImGuiWindowFlags flgs);
    void removeWindowFlags(ImGuiWindowFlags flgs);

    void menuBarOn(bool on);
    
    Vector2 getPosGlobalToWin(const Vector2& pos);
    Vector2 getPosWinToGlobal(const Vector2& pos);
    
    Vector2 getOffGlobalToWin(const Vector2& pos);
    Vector2 getOffWinToGlobal(const Vector2& pos);

    float zoomIndependent(float v);

    void draw(const Vector2& mousePos, const Vector2& mouseDelta);
    void queueRerender();

    static bool doesViewPortNeedResize(float needVX, float needVY, float currVX, float currVY, float resizeMargin, float safetyMargin, Vector2& newSize, bool alsoTrigger = false);
protected:
    virtual void drawWorld(const Vector2& mousePos, const Vector2& mouseDelta);
    virtual void drawMenuBar();
    virtual void afterWinDraw();
    virtual void sizeUpdate(ImVec2 size);
    virtual void renderTexInit();
    virtual bool ownUpdate(const Vector2& mousePos, const Vector2& mouseDelta);
    virtual void drawRaw();

    void setCamTarget(const Vector2& targ);
    Vector2 renderTexSize();
    bool renderTexReady();

    float getZoom();
    void setCamZoom(float z);
    void resetCam();
private:
    RenderTexture2D renderTex;
    bool needsReRender = true;
    Camera2D cam;
    Vector2 moveOff;
    bool isMoving;
    bool menuBarIsOn = false;

    bool update(const Vector2& mouseDelta);
    void updateRenderTex(const Vector2& mousePos, const Vector2& mouseDelta);
    ImVec2 getTexDrawCursorPos();
};

class TextureViewer : public World2DViewer {
public:
    Texture2D tex;
    bool mirrorVert;
    bool alwaysReCenter = true;
    
private:
    bool adjZoom;
    float ratio;
    Vector2 firstTexDispSize = { 0,0 };
    Vector2 texDispSize = {0, 0};
protected:
    virtual void drawOverlay(const Vector2& mousePos, const Vector2& mouseDelta);
    void drawWorld(const Vector2& mousePos, const Vector2& mouseDelta) override;
    void sizeUpdate(ImVec2 size) override;

    void reAdjZoom(const Vector2& size);
public:
    TextureViewer(Texture2D tex_, const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None, bool doAdjZoom = true);
    TextureViewer(const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None, bool doAdjZoom = true);
    ~TextureViewer();
    void setTex(Texture2D tex_, bool reAdjCam = true, bool reRender = true);

    Vector2 getPosTexToWin(const Vector2& pos);
    Vector2 getPosWinToTex(const Vector2& pos);

    Vector2 getOffTexToWin(const Vector2& pos);
    Vector2 getOffWinToTex(const Vector2& pos);

    void reCenterTex();
    void reAdjCamToTex();
    void queueReAdjustZoom();
};
/*
class CVImageViewer : public TextureViewer {
public:
    Image img_r;
    cv::Mat img_cv;

    CVImageViewer(cv::Mat img, const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
    ~CVImageViewer();

    void update_img_r();
};
*/
#endif
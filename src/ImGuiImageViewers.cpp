#include "ImGuiImageViewers.h"

#include "raymath.h"
#include "rlgl.h"

#include "utils/utils.h"
#include "oneHeaderLibs/VectorOperators.h"

#include "rlImGui/rlImGui.h"

World2DViewer::World2DViewer(const char* name, ImGuiWindowFlags flags) {
    winProps.windowName = name;
    winProps.flags = flags;
    winProps.winOpen = true;

    renderTex.id = 0;
    resetCam();
}
World2DViewer::~World2DViewer() {
    if (renderTex.id != 0) {
        UnloadRenderTexture(renderTex);
    }
}

void World2DViewer::drawWorld(const Vector2& mousePos, const Vector2& mouseDelta) {

}
void World2DViewer::drawMenuBar() {

}
void World2DViewer::afterWinDraw() {

}
void World2DViewer::sizeUpdate(ImVec2 size) {

}
void World2DViewer::renderTexInit() {

}
bool World2DViewer::ownUpdate(const Vector2& mousePos, const Vector2& mouseDelta) {
    return false;
}
void World2DViewer::drawRaw() {

}

void World2DViewer::addWindowFlags(ImGuiWindowFlags flgs) {
    winProps.flags |= flgs;
}
void World2DViewer::removeWindowFlags(ImGuiWindowFlags flgs) {
    winProps.flags &= ~flgs;
}
void World2DViewer::menuBarOn(bool on) {
    if (on) {
        addWindowFlags(ImGuiWindowFlags_MenuBar);
        menuBarIsOn = true;
    }else {
        removeWindowFlags(ImGuiWindowFlags_MenuBar);
        menuBarIsOn = false;
    }
}

bool World2DViewer::update(const Vector2& mouseDelta) {
    bool reRender = false;

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    Vector2 localMousePos = getPosGlobalToWin(GetMousePosition());//GetScreenToWorld2D(mousePos, cam);

    if (CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { pos.x,pos.y,size.x,size.y })) {
        float add = (GetMouseWheelMove() * 0.2f) * cam.zoom;
        if (add != 0 && cam.zoom + add > 0) {
            setCamTarget(localMousePos);
            
            cam.zoom += add;
            if (add != 0) {
                reRender = true;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            isMoving = true;
        }
    }

    if (isMoving && (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) || IsMouseButtonUp(MOUSE_BUTTON_MIDDLE))) {
        isMoving = false;
    }

    if (isMoving) {
        cam.offset += mouseDelta;
        if (mouseDelta != Vector2{0, 0}) {
            reRender = true;
        }
    }

    return reRender;
}
void World2DViewer::draw(const Vector2& mousePos, const Vector2& mouseDelta) {
    if (winProps.winOpen) {
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin(winProps.windowName.c_str(), winProps.winOpenPtr, winProps.flags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            if (menuBarIsOn) {
                ImGui::PopStyleVar();
                drawMenuBar();
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            }

            winProps.isWinHovered = ImGui::IsWindowHovered();
            winProps.dockID = ImGui::GetWindowDockID();

            if ((winProps.isWinHovered && !noMoveOrZoom && update(mouseDelta)) || ownUpdate(mousePos, mouseDelta)) {
                needsReRender = true;
            }

            ImVec2 size = ImGui::GetContentRegionAvail();

            sizeUpdate(size);

            constexpr float safetyMargin = 20;
            constexpr float resizeMargin = 100;
            Vector2 newSize = { 0,0 };
            bool addCond = renderTex.id == 0;
            if (doesViewPortNeedResize(size.x, size.y, (float)renderTex.texture.width, (float)renderTex.texture.height, resizeMargin, safetyMargin, newSize, addCond)) { //check if difference in Size is too big
                Vector2 off;
                bool renderTexInited = renderTex.id != 0;
                if (renderTexInited) {
                    off = { (newSize.x - renderTex.texture.width) / 2, (newSize.y - renderTex.texture.height) / 2 };
                }
                else {
                    off = { 0,0 };
                }
                cam.target += off; //adjust cam to not have it jump on resize
                cam.offset += off;
                if (renderTexInited) {
                    UnloadRenderTexture(renderTex);
                }
                renderTex = LoadRenderTexture((int)newSize.x, (int)newSize.y);
                if (!renderTexInited) {
                    renderTexInit();
                }
                needsReRender = true;
            }
            if (reRenderEveryFrame || needsReRender) {
                updateRenderTex(mousePos,mouseDelta);
            }

            ImGui::SetCursorScreenPos(getTexDrawCursorPos());
            RLImGuiImageRect(&renderTex.texture, renderTex.texture.width, renderTex.texture.height, { 0,0,(float)renderTex.texture.width,-((float)renderTex.texture.height) });

            drawRaw();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
    afterWinDraw();
}

bool World2DViewer::doesViewPortNeedResize(float needVX, float needVY, float currVX, float currVY, float resizeMargin, float safetyMargin, Vector2& newSize, bool alsoTrigger) {
    if (alsoTrigger ||
        std::min(currVX - needVX, currVY - needVY) < safetyMargin ||
        std::max(currVX - needVX, currVY - needVY) > resizeMargin * 2 + safetyMargin) 
    {
        newSize = { needVX + resizeMargin + safetyMargin, needVY + resizeMargin + safetyMargin };
        return true;
    }
    return false;
}

void World2DViewer::queueRerender() {
    needsReRender = true;
}

ImVec2 World2DViewer::getTexDrawCursorPos() {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();
    return { pos.x + size.x / 2 - renderTex.texture.width / 2, pos.y + size.y / 2 - renderTex.texture.height / 2 };
}

Vector2 World2DViewer::getPosGlobalToWin(const Vector2& pos) {
    ImVec2 texDrawPos = getTexDrawCursorPos();
    Vector2 texDrawPosV = { texDrawPos.x, texDrawPos.y };
    return GetScreenToWorld2D(Vector2Subtract(pos, texDrawPosV), cam);
}
Vector2 World2DViewer::getPosWinToGlobal(const Vector2& pos) {
    ImVec2 texDrawPos = getTexDrawCursorPos();
    Vector2 texDrawPosV = { texDrawPos.x, texDrawPos.y };
    return Vector2Add(GetWorldToScreen2D(pos, cam), texDrawPosV);
}

Vector2 World2DViewer::getOffGlobalToWin(const Vector2& pos) {
    return pos / cam.zoom;
}
Vector2 World2DViewer::getOffWinToGlobal(const Vector2& pos) {
    return pos * cam.zoom;
}

float World2DViewer::zoomIndependent(float v) {
    return v / cam.zoom;
}

inline void World2DViewer::updateRenderTex(const Vector2& mousePos, const Vector2& mouseDelta) {
    needsReRender = false;
    BeginTextureMode(renderTex);
    ClearBackground(BLANK);
    BeginMode2D(cam);

    drawWorld(mousePos, mouseDelta);

    //DrawRectangle(0, 0, 100, 100, RED);
    //DrawRectangle(renderTex.texture.width/2, renderTex.texture.height/2, 100, 100, PINK);

    EndMode2D();
    EndTextureMode();
}

void World2DViewer::setCamTarget(const Vector2& targ) {
    Vector2 off = Vector2Subtract(targ, cam.target);
    cam.target = targ;
    cam.offset += off * cam.zoom;
}
void World2DViewer::resetCam() {
    cam.offset = { 0,0 };
    cam.target = { 0,0 };
    cam.zoom = 1;
    cam.rotation = 0;
}

Vector2 World2DViewer::renderTexSize() {
    return Vector2{ (float)renderTex.texture.width, (float)renderTex.texture.height };
}
bool World2DViewer::renderTexReady() {
    return renderTex.id != 0;
}
float World2DViewer::getZoom() {
    return cam.zoom;
}
void World2DViewer::setCamZoom(float z) {
    cam.zoom = z;
}

// ###############################################################################################################################

TextureViewer::TextureViewer(Texture2D tex_, const char* name, ImGuiWindowFlags flags, bool doAdjZoom) : World2DViewer(name,flags) {
    setTex(tex_);
    mirrorVert = false;
    adjZoom = doAdjZoom;
}
TextureViewer::TextureViewer(const char* name, ImGuiWindowFlags flags, bool doAdjZoom) : World2DViewer(name,flags) {
    tex.id = 0;
    mirrorVert = false;
    adjZoom = doAdjZoom;
}
void TextureViewer::setTex(Texture2D tex_, bool reAdjCam, bool reRender) {
    tex = tex_;
    //SetTextureFilter(tex, TEXTURE_FILTER_ANISOTROPIC_16X  );
    //GenTextureMipmaps(&tex);
    ratio = (float)tex.width / (float)tex.height;
    texDispSize = {(float)tex.width,(float)tex.width*(1/ratio)};
    if (firstTexDispSize == Vector2{0, 0}) {
        firstTexDispSize = texDispSize;
    }

    if (reAdjCam) {
        reAdjCamToTex();
    }
    if (reRender) {
        queueRerender();
    }
}

TextureViewer::~TextureViewer() {
    UnloadTexture(tex);
}

void TextureViewer::drawWorld(const Vector2& mousePos, const Vector2& mouseDelta) {
    if (tex.id != 0) {
        Rectangle src = { 0, 0,(float)tex.width, (float)tex.height };
        if (mirrorVert) {
            src.height *= -1;
        }

        if (alwaysReCenter) {
            reCenterTex();
        }
        Vector2 renTexDim = renderTexSize();
        Rectangle dst = { renTexDim.x / 2 - firstTexDispSize.x / 2, renTexDim.y / 2 - firstTexDispSize.y / 2, texDispSize.x, texDispSize.y };
#if 0
        float pad = 5;
        DrawRectangleRec({dst.x-pad, dst.y-pad, dst.width+2*pad, dst.height+2*pad}, ORANGE);
#endif
        DrawTexturePro(tex, src, dst, {0,0},0, WHITE);
    }

    //DrawCircle(cam.target.x, cam.target.y, 20, GREEN);

    rlPushMatrix();
    Vector2 off = getPosTexToWin({ 0,0 });
    rlTranslatef(off.x, off.y, 0);

    drawOverlay(mousePos, mouseDelta);

    rlPopMatrix();
}
void TextureViewer::drawOverlay(const Vector2& mousePos, const Vector2& mouseDelta) {
    
}
void TextureViewer::sizeUpdate(ImVec2 size) {
    if (renderTexReady() && adjZoom) {
        reAdjZoom({ size.x,size.y });
        queueRerender();
    }
}

void TextureViewer::reCenterTex() {
    firstTexDispSize = texDispSize;
}
void TextureViewer::reAdjZoom(const Vector2& size) {
    adjZoom = false;
    setCamTarget(renderTexSize() / 2);
    Vector2 texSize = getOffTexToWin(texDispSize);
    Vector2 fact = Vector2{ size.x,size.y } / texSize;
    setCamZoom(std::min(fact.x, fact.y));
}
void TextureViewer::reAdjCamToTex() {
    queueReAdjustZoom();
    resetCam();
    reCenterTex();
}
void TextureViewer::queueReAdjustZoom() {
    adjZoom = true;
}

Vector2 TextureViewer::getPosTexToWin(const Vector2& pos) {
    Vector2 center = renderTexSize() / 2;
    return Vector2Add(Vector2Subtract(pos, Vector2Scale(firstTexDispSize, 0.5)), center);
}
Vector2 TextureViewer::getPosWinToTex(const Vector2& pos) {
    Vector2 center = renderTexSize() / 2;
    return Vector2Add(Vector2Subtract(pos, center), Vector2Scale(firstTexDispSize, 0.5));
}

Vector2 TextureViewer::getOffTexToWin(const Vector2& pos) {
    return pos;
}
Vector2 TextureViewer::getOffWinToTex(const Vector2& pos) {
    return pos;
}

// ###############################################################################################################################

/*
CVImageViewer::CVImageViewer(cv::Mat img, const char* name, ImGuiWindowFlags flags) : TextureViewer(name,flags), img_cv(img) {
    img_r.width = img_cv.cols;
    img_r.height = img_cv.rows;
    img_r.data = new uint8_t[img_r.width * img_r.height * 3];
    //img_r.data = img_cv.data;
    update_img_r();
    img_r.mipmaps = 1;
    img_r.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    //img_r.format = PIXELFORMAT_UNCOMPRESSED_B8G8R8;
    printf("START\n");
    setTex(LoadTextureFromImage(img_r));
    printf("END\n");
}
CVImageViewer::~CVImageViewer(){
    delete img_r.data;//UnloadImage(img_r);
}

void CVImageViewer::update_img_r() {
    for (int i = 0; i < img_r.width * img_r.height * 3; i += 3) {
        ((uint8_t*)img_r.data)[i]   = img_cv.data[i + 2];
        ((uint8_t*)img_r.data)[i+1] = img_cv.data[i + 1];
        ((uint8_t*)img_r.data)[i+2] = img_cv.data[i];
    }
}
*/

// ###############################################################################################################################


/*


//ImVec2 texDrawPos = getTexDrawCursorPos();


//Vector2 mousePos = GetMousePosition();
//mousePos.x -= texDrawPos.x;
//mousePos.y -= texDrawPos.y;

void TextureViewer::update(float mouseDeltaX, float mouseDeltaY) {
ImVec2 size = ImGui::GetContentRegionAvail();
ImVec2 pos = ImGui::GetCursorScreenPos();

if (CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { pos.x,pos.y,size.x,size.y })) {
float add = (GetMouseWheelMove() * 0.1f) * cam.zoom;
if (cam.zoom + add > 0) {
cam.zoom += add;
}

if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
isMoving = true;
}
if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
isMoving = false;
}
}

if (isMoving) {
moveOff.x += mouseDeltaX;
moveOff.y += mouseDeltaY;
}

cam.offset = cam.target + moveOff;
}
void TextureViewer::draw() {
ImVec2 size = ImGui::GetContentRegionAvail();
constexpr float safetyMargin = 20;
constexpr float resizeMargin = 50;
if (std::min(renderTex.texture.width - size.x, renderTex.texture.height - size.y) < safetyMargin ||
std::max(renderTex.texture.width - size.x, renderTex.texture.height - size.y) > resizeMargin * 2 + safetyMargin) { //check if diffecence is too great std::max(std::abs(size.x - renderTex.texture.width), std::abs(size.y - renderTex.texture.height)) > 30
UnloadRenderTexture(renderTex);
renderTex = LoadRenderTexture(size.x + resizeMargin + safetyMargin, size.y + resizeMargin + safetyMargin);
cam.target = { (float)renderTex.texture.width / 2, (float)renderTex.texture.height / 2 };
cam.offset = cam.target + moveOff;
}

ImVec2 pos = ImGui::GetCursorScreenPos();
ImGui::SetCursorScreenPos({ pos.x + size.x / 2 - renderTex.texture.width / 2, pos.y + size.y / 2 - renderTex.texture.height / 2 });

generateTex();
RLImGuiImageRect(&renderTex.texture, renderTex.texture.width, renderTex.texture.height, { 0,0,(float)renderTex.texture.width,-((float)renderTex.texture.height) });
}

#if 0
DrawCircle(cam.offset.x, cam.offset.y, 10, BLUE);
DrawCircle(cam.target.x, cam.target.y, 10, RED);
ImVec2 texDrawPos = getTexDrawCursorPos();
Vector2 mousePos = GetMousePosition();
//mousePos.x -= pos.x;
//mousePos.y -= pos.y;
mousePos.x -= texDrawPos.x;
mousePos.y -= texDrawPos.y;
Vector2 localMousePos = GetScreenToWorld2D(mousePos, cam);
DrawCircle(localMousePos.x, localMousePos.y, 10, ORANGE);
#endif

//DrawRectangleGradientEx({ 0,0,(float)renderTex.texture.width,(float)renderTex.texture.height }, PINK, YELLOW, GREEN, BLUE);
*/
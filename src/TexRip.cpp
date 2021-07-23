#include "TexRip.h"
#include "oneHeaderLibs/VectorOperators.h"
#include "raymath.h"
#include <string>
#include "shaders.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "utils/utils.h"
#include "Input.h"
#include "rlgl.h"
#include "Extensions/imgui/imguiExts.h"
#include <cmath>

//#include "fromJeffMTestframe/Application/platform_tools.h"


TexRip::ImgRec::ImgRec() {

}
TexRip::ImgRec::ImgRec(const Vector2& pos1, const Vector2& pos2, const Vector2& pos3, const Vector2& pos4,const Vector2& imgDim) {
    pnts[0] = { pos1, false, false };
    pnts[1] = { pos2, false, false };
    pnts[2] = { pos3, false, false };
    pnts[3] = { pos4, false, false };
    lastRipPos[0] = { -1,-1 };
    lastRipPos[1] = { -1,-1 };
    lastRipPos[2] = { -1,-1 };
    lastRipPos[3] = { -1,-1 };
    //calcPersp(imgDim);
}
void TexRip::ImgRec::calcPersp(const Vector2& imgDim) {
    Vector2 src[4];
    for (int i = 0; i < 4; i++) {
        src[i] = pnts[i].pos;
        lastRipPos[i] = pnts[i].pos; //update last pos
    }

    float width = std::max(Vector2Distance(pnts[0].pos, pnts[1].pos), Vector2Distance(pnts[3].pos, pnts[2].pos));
    float height = std::max(Vector2Distance(pnts[0].pos, pnts[3].pos), Vector2Distance(pnts[1].pos, pnts[2].pos));
    corrDim = { width, height };

    Vector2 dst[4] = { {0,0}, {imgDim.x,0}, {imgDim.x,imgDim.y}, {0,imgDim.y} };

    utils::getHomography(src, dst, persp);
    utils::getHomography(dst, src, inv_persp);
}
Rectangle TexRip::ImgRec::boundingBox() const {
    Vector2 min = { INFINITY, INFINITY };
    Vector2 max = { -INFINITY, -INFINITY };
    for (int i = 0; i < 4; i++) {
        min.x = std::min(pnts[i].pos.x, min.x);
        min.y = std::min(pnts[i].pos.y, min.y);
        max.x = std::max(pnts[i].pos.x, max.x);
        max.y = std::max(pnts[i].pos.y, max.y);
    }
    return Rectangle{ min.x, min.y, max.x - min.x, max.y - min.y };
}

// ###############################################################################################################################

TexRip::ImageRipChildWin::ImageRipChildWin(Texture2D tex_, const char* name, ImGuiWindowFlags flags, bool doAdjZoom) : TextureViewer(tex_,name,flags,doAdjZoom) {
    
}
TexRip::ImageRipChildWin::ImageRipChildWin(const char* name, ImGuiWindowFlags flags, bool doAdjZoom) : TextureViewer(name,flags,doAdjZoom) {
    
}

void TexRip::ImageRipChildWin::setWinDocked(bool docked) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize; // //ImGuiWindowFlags_NoDecoration ImGuiWindowFlags_NoTitleBar
    if(docked)
        addWindowFlags(flags);
    else
        removeWindowFlags(flags);
    menuBarOn(!docked);
    reAdjCamToTex();
    acceptIOKeyCombs = !docked;
}

// ###############################################################################################################################

TexRip::ImageSelectionViewer::RectManager::PersMat::PersMat() {
    img.data = NULL;
    img.height = 3;
    img.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32;
    img.mipmaps = 1;
    tex.id = 0;
}
TexRip::ImageSelectionViewer::RectManager::PersMat::~PersMat() {
    if (img.data != NULL) {
        delete[] img.data;
    }
    if (tex.id != 0) {
        UnloadTexture(tex);
    }
}
void TexRip::ImageSelectionViewer::RectManager::PersMat::updateWithRecs(const std::vector<ImgRec>& recs, bool inverse) {
    int targetLen = recs.size();
    if (targetLen != img.width) {
        img.width = targetLen;
        if (img.data != NULL) {
            delete[] img.data;
            UnloadTexture(tex);
            tex.id = 0;
        }
        img.data = new float[3 * targetLen * 3];

    }
    if (inverse) {
        for (int i = 0; i < img.width; i++) {
            if (recs[i].progress == ImgRec::Prog_FULL) {
                for (int j = 0; j < 3; j++) {
                    ((Vector3*)img.data)[j*img.width+i] = {recs[i].inv_persp[j*3+0],recs[i].inv_persp[j*3+1],recs[i].inv_persp[j*3+2]};
                }
            }
            else {
                for (int j = 0; j < 3; j++) {
                    ((Vector3*)img.data)[j*img.width+i] = {0,0,0};
                }
            }
        }
    }
    else {
        for (int i = 0; i < img.width; i++) {
            if (recs[i].progress == ImgRec::Prog_FULL) {
                for (int j = 0; j < 3; j++) {
                    ((Vector3*)img.data)[j*img.width+i] = {recs[i].persp[j*3+0],recs[i].persp[j*3+1],recs[i].persp[j*3+2]};
                }
            }
            else {
                for (int j = 0; j < 3; j++) {
                    ((Vector3*)img.data)[j*img.width+i] = {0,0,0};
                }
            }
        }
    }


    if (tex.id == 0) {
        tex = LoadTextureFromImage(img);
    }
    else {
        UpdateTexture(tex, img.data);
    }
}

// ###############################################################+
TexRip::ImageSelectionViewer::RectManager::RectManager() : undoBuffer(32) {

}

bool TexRip::ImageSelectionViewer::RectManager::startPointMode(const Vector2& mousePosTexRel) { 
    bool moveStarted = false;
    if (Input::isActionActive(Input::Action_move)) {
        moveStarted = startMove();
    }
    else if (Input::isActionActive(Input::Action_rotate)) {
        moveStarted = startRotate();
    }
    else if (Input::isActionActive(Input::Action_scale)) {
        moveStarted = startScale(mousePosTexRel);
    }
    else if (Input::isActionActive(Input::Action_add)) {
        moveStarted = addRectConstr();
    }

    if (moveStarted) {
        addUndoState();
    }
    return moveStarted;
}
bool TexRip::ImageSelectionViewer::RectManager::startMove() {
    mode = MODES::MOVE;
    moveOff = {0,0};
    worldMayMove = false;
    return true;
}
bool TexRip::ImageSelectionViewer::RectManager::startRotate() {
    rotOff = 0;
    int num;
    Vector2 meanPos = getMeanSelPntPos(num);
    if (num > 1) {
        mode = MODES::ROTATE;
        rotCenter = meanPos;
        worldMayMove = false;

        return true;
    }
    return false;
}
bool TexRip::ImageSelectionViewer::RectManager::startScale(const Vector2& mousePosTexRel) {
    int num;
    Vector2 meanPos = getMeanSelPntPos(num);
    if (num > 1) {
        mode = MODES::SCALE;
        sclCenter = meanPos;
        //globalSclCenter = getPosWinToGlobal(getPosTexToWin(sclCenter));
        sclMouseStartDist = Vector2Distance(sclCenter, mousePosTexRel);
        lastSclAmt = 1.0f;

        worldMayMove = false;
        return true;
    }
    return false;
}
bool TexRip::ImageSelectionViewer::RectManager::addRectConstr() {
    selectAllOrNone(false); // deselect all Recs

    ImgRec r;
    r.progress = ImgRec::Prog_0;
    for (int i = 0; i < 4; i++) {
        r.pnts[i].selected = true;
        r.pnts[i].candidate = false;
        r.pnts[i].pos = { 0,0 };

        r.lastRipPos[i] = { -1,-1 };
    }
    addRect(r);
    constrID = (recs.size()-1) << 2;

    mode = MODES::CONSTRUCT;
    worldMayMove = true;
    needsMatUpdateForce = true;
    return true;
}

bool TexRip::ImageSelectionViewer::RectManager::managePoints(const Vector2& mousePosTexRel, const Vector2& mouseDeltaTexRel, const float zoomF, const bool winIsHovered, const Vector2& texSize) {
    bool reRender = false;
    
#if 0
    static bool firstRun = true;
    if (firstRun) {
        firstRun = false;
        reRender = true;
        updateRecMats(texSize);
    }
#endif

    if (mode == MODES::NONE && winIsHovered) {
        if (startPointMode(mousePosTexRel)) {

        }
    }

    if (mode == MODES::NONE) {
        if (winIsHovered){
            if (Input::isActionActive(Input::Action_undo)) {
                if (undo()) reRender = true;
            }

            if(IsMouseButtonPressed(Input::mainMouseB())) {
                if (selectWithMouse(mousePosTexRel, zoomF)) {
                    reRender = true;
                }
            }
            else if (Input::isActionActive(Input::Action_selectAll)) { //select all/none
                selectAllOrNone();
                reRender = true;
            }else if (Input::isActionActive(Input::Action_selectLinked)) { // select linked
                selectLinked();
                reRender = true;
            }
        }
    }
    else if (mode == MODES::CONSTRUCT) {
        
        if (winIsHovered && IsMouseButtonPressed(Input::secMouseB())) {
            stopPointModeConstruct(true);
        }
        else {
            if (updatePointsConstruct(mousePosTexRel)) reRender = true;
        }
    }
    else { //mode != NONE && mode != CONSTRUCT
        if (winIsHovered && (IsMouseButtonPressed(Input::mainMouseB()) || IsMouseButtonPressed(Input::secMouseB()))) { // stop/cancel mode
            stopPointMode(IsMouseButtonPressed(Input::secMouseB()));
            reRender = true;
            needsMatUpdate = true;
        }
        else { // mode updates
            const float fact = Input::modShift() ? 0.25f : 1.0f;
            bool pointsChanged = false;
            switch (mode) {
                case MODES::MOVE:
                    pointsChanged = updatePointsMove(mouseDeltaTexRel, fact);
                    break;

                case MODES::ROTATE:
                    pointsChanged = updatePointsRotate(mousePosTexRel, mouseDeltaTexRel, fact);
                    break;

                case MODES::SCALE:
                    pointsChanged = updatePointsScale(mousePosTexRel, fact);
                    break;
            }
            if (pointsChanged) {
                needsMatUpdate = true;
            }
            reRender = reRender || pointsChanged;
        }
    }

    if (needsMatUpdate || needsMatUpdateForce) {
        updateRecMats(texSize, needsMatUpdateForce);
        needsMatUpdate = false;
        needsMatUpdateForce = false;
        reRender = true;
    }

    pointsWereMoved = reRender;
    return reRender;
}
bool TexRip::ImageSelectionViewer::RectManager::updatePointsMove(const Vector2& mouseDeltaTexRel, const float fact) {
    Vector2 moveAmt = mouseDeltaTexRel;

    if (moveAmt != Vector2{ 0,0 }) {
        moveAmt *= fact;

        moveOff += moveAmt;

        for (auto& r : recs) {
            for (auto& p : r.pnts) {
                if (p.selected) {
                    p.pos += moveAmt;
                }
            }
        }
        return true;
    }
    return false;
}
bool TexRip::ImageSelectionViewer::RectManager::updatePointsRotate(const Vector2& mousePosTexRel, const Vector2& mouseDeltaTexRel, const float fact) {
    float angle;
    {
        Vector2 mousePosRelCenter = mousePosTexRel - rotCenter;
        float a1 = atan2(mousePosRelCenter.y,mousePosRelCenter.x);
        Vector2 prevMousePosAdj = mousePosRelCenter - mouseDeltaTexRel;
        float a2 = atan2(prevMousePosAdj.y,prevMousePosAdj.x);

        angle = a1-a2;
    }

    //angle *= fact;
    rotOff += angle;

    if (angle != 0) {
        float s = sin(angle);
        float c = cos(angle);
        for (auto& r : recs) {
            for (auto& p : r.pnts) {
                if (p.selected) {
                    p.pos = utils::rotatePoint(p.pos, rotCenter, s, c);
                }
            }
        }
        return true;
    }
    return false;
}
bool TexRip::ImageSelectionViewer::RectManager::updatePointsScale(const Vector2& mousePosTexRel, const float fact) {
    float sclAmt = Vector2Distance(sclCenter, mousePosTexRel) / sclMouseStartDist;

    if (sclAmt != lastSclAmt) {
        //float diff = sclAmt - lastSclAmt;
        //diff *= fact;
        //float newSclAmt = lastSclAmt + diff;
        for (auto& r : recs) {
            for (auto& p : r.pnts) {
                if (p.selected) {
                    p.pos = (((p.pos-sclCenter) / lastSclAmt) * sclAmt)+sclCenter;
                }
            }
        }
        lastSclAmt = sclAmt;
        return true;
    }
    return false;
}
bool TexRip::ImageSelectionViewer::RectManager::updatePointsConstruct(const Vector2& mousePosTexRel) {
    bool reRender = false;
    ImgRec& rec = getRecFromID(constrID);

    if (Input::isActionActive(Input::Action_undo) && rec.progress > ImgRec::Prog_0) {
        rec.progress--;
        reRender = true;
    }

    ImgPoint& pnt = rec.pnts[rec.progress];
    if (pnt.pos != mousePosTexRel) {
        pnt.pos = mousePosTexRel;
        reRender = true;
    }

    if (IsMouseButtonPressed(Input::mainMouseB())) {
        rec.progress++;
        rec.pnts[rec.progress].pos = mousePosTexRel;
        if (rec.progress == ImgRec::Prog_FULL) {
            stopPointModeConstruct(false);
        }
        reRender = true;
    }

    return reRender;
}

void TexRip::ImageSelectionViewer::RectManager::stopPointMode(bool cancel) {
    if (cancel) {
        switch (mode) {
            case MODES::NONE: 
                break;

            case MODES::MOVE:
                stopPointModeMove();
                break;

            case MODES::ROTATE:
                stopPointModeRotate();
                break;

            case MODES::SCALE:
                stopPointModeScale();
                break;
        }
        undoBuffer.pop(NULL); // remove last undo state because recs didnt change anyway (because recs was reset)
    }
    else { // movement didn't get cancelled
        
    }

    mode = MODES::NONE;
    worldMayMove = true;
}
void TexRip::ImageSelectionViewer::RectManager::stopPointModeMove() {
    for (auto& r : recs) {
        for (auto& p : r.pnts) {
            if (p.selected) {
                p.pos.x -= moveOff.x;
                p.pos.y -= moveOff.y;
            }
        }
    }
}
void TexRip::ImageSelectionViewer::RectManager::stopPointModeRotate() {
    float s = sin(-rotOff);
    float c = cos(-rotOff);
    for (auto& r : recs) {
        for (auto& p : r.pnts) {
            if (p.selected) {
                p.pos = utils::rotatePoint(p.pos, rotCenter, s, c);
            }
        }
    }
}
void TexRip::ImageSelectionViewer::RectManager::stopPointModeScale() {
    for (auto& r : recs) {
        for (auto& p : r.pnts) {
            if (p.selected) {
                p.pos = ((p.pos-sclCenter) / lastSclAmt)+sclCenter;
            }
        }
    }
}
void TexRip::ImageSelectionViewer::RectManager::stopPointModeConstruct(bool cancel) {
    if (cancel) {
        recs.erase(recs.begin() + getRecIndFromID(constrID));
    }
    else {

    }

    needsMatUpdate = true;
    mode = MODES::NONE;
    constrID = -1;
}

bool TexRip::ImageSelectionViewer::RectManager::selectWithMouse(const Vector2& mousePosTexRel, const float zoomF) {
    bool doDeselect = !Input::modShift();

    RectPntID pnt = getSelPointInd(mousePosTexRel, zoomF,doDeselect);

    if (pnt != -1) {
        if (Input::modCtrl()) { //sel complete rect
            for (auto& p : getRecFromID(pnt).pnts)
                p.selected = true;
        }
        else {
            getRecPntFromID(pnt).selected = true;
        }
        return true;
    }
    return false;
}
void TexRip::ImageSelectionViewer::RectManager::selectAllOrNone(int force) {
    if (force == -1) { //auto detect
        bool allSel = true;
        for (auto& r : recs) {
            for (auto& p : r.pnts) {
                if (!p.selected) {
                    p.selected = true;
                    allSel = false;
                }
            }
        }

        if (allSel) {
            for (auto& r : recs) {
                for (auto& p : r.pnts) {
                    p.selected = false;
                }
            }
        }
    }
    else { //set all to force
        for (auto& r : recs) {
            for (auto& p : r.pnts) {
                p.selected = force;
            }
        }
    }
}
void TexRip::ImageSelectionViewer::RectManager::selectLinked() {
    for (auto& r : recs) {
        bool isSel = false;
        for (auto& p : r.pnts) {
            if (p.selected) {
                isSel = true;
            }
        }

        if (isSel) {
            for (auto& p : r.pnts) {
                p.selected = true;
            }
        }
    }
}

TexRip::ImageSelectionViewer::RectManager::RectPntID TexRip::ImageSelectionViewer::RectManager::getSelPointInd(const Vector2& mousePosTexRel, const float zoomF, bool deselect) {
    for (auto& r : recs) {
        for (auto& p : r.pnts) {
            p.candidate = CheckCollisionPointCircle(mousePosTexRel, p.pos, 50 * zoomF);
        }
    }

    float minDist = INFINITY;
    int nearest = -1;
    int backup = -1;
    for (size_t r = 0; r < recs.size();r++) {
        for (int p = 0; p < 4;p++) {
            if (recs[r].pnts[p].candidate) {
                float dist = Vector2Distance(mousePosTexRel, recs[r].pnts[p].pos); // TODO replace with distSq
                if (dist < minDist) {
                    if (!recs[r].pnts[p].selected) {
                        minDist = dist;
                        nearest = r<<2 | p;
                    }
                    else {
                        backup = r<<2 | p;
                    }
                }
                recs[r].pnts[p].candidate = false;
            }
            if(deselect)
                recs[r].pnts[p].selected = false;
        }
    }

    int pnt = -1;
    if (nearest != -1) {
        return nearest;
    }
    else {
        return backup;
    }
}
Vector2 TexRip::ImageSelectionViewer::RectManager::getMeanSelPntPos(int& numRef) {
    Vector2 sum = { 0,0 };
    int num = 0;
    for (auto& r : recs) {
        for (auto& p : r.pnts) {
            if (p.selected) {
                sum += p.pos;
                num++;
            }
        }
    }
    sum /= (float)num;
    numRef = num;
    return sum;
}

void TexRip::ImageSelectionViewer::RectManager::addUndoState() {
    undoBuffer.push(recs);
}
bool TexRip::ImageSelectionViewer::RectManager::undo() {
    if (!undoBuffer.pop(&recs)) {
        // undoBuffer empty
        return false;
    }
    return true;
}

const size_t TexRip::ImageSelectionViewer::RectManager::getRecIndFromID(RectPntID id) {
    return id >> 2;
}
TexRip::ImgRec& TexRip::ImageSelectionViewer::RectManager::getRecFromID(RectPntID id) {
    return recs[getRecIndFromID(id)];
}
TexRip::ImgPoint& TexRip::ImageSelectionViewer::RectManager::getRecPntFromID(RectPntID id) {
    return recs[id >> 2].pnts[id & 0b11];
}

void TexRip::ImageSelectionViewer::RectManager::updateRecMats(const Vector2& texSize, bool forceTexUpdate) { // returns true if at least 1 rect mat got updated
    bool wasUpdated = false;
    for (auto& r : recs) {
        if (r.progress == ImgRec::Prog_FULL) {
            for (int i = 0; i < 4; i++) {
                if (r.pnts[i].pos != r.lastRipPos[i]) {
                    r.calcPersp(texSize);
                    wasUpdated = true;
                    break;
                }
            }
        }
    }

    if (wasUpdated || forceTexUpdate) {
        matsTex.updateWithRecs(recs, false);
        invMatsTex.updateWithRecs(recs, true);
    }
}

std::vector<TexRip::ImgRec>& TexRip::ImageSelectionViewer::RectManager::getRecs() {
    return recs;
}
void TexRip::ImageSelectionViewer::RectManager::addRect(const TexRip::ImgRec& rect) {
    recs.push_back(rect);
    needsMatUpdate = true;
}
uint8_t TexRip::ImageSelectionViewer::RectManager::getMode() const {
    return mode;
}
bool TexRip::ImageSelectionViewer::RectManager::werePointsMoved() const {
    return pointsWereMoved;
}
bool TexRip::ImageSelectionViewer::RectManager::mayWorldMove() const{
    return worldMayMove;
}

const Vector2& TexRip::ImageSelectionViewer::RectManager::getRotCenter() const {
    return rotCenter;
}
const Vector2& TexRip::ImageSelectionViewer::RectManager::getSclCenter() const {
    return sclCenter;
}

const Texture2D& TexRip::ImageSelectionViewer::RectManager::getMatTex() const {
    return matsTex.tex;
}
const Texture2D& TexRip::ImageSelectionViewer::RectManager::getInvMatTex() const {
    return invMatsTex.tex;
}

// ###############################################################+

TexRip::ImageSelectionViewer::ImageSelectionViewer(const Texture2D& tex, const char* name, ImGuiWindowFlags flags)
    : rectManager(), ImageRipChildWin(tex, name, flags) {
    menuBarOn(true);
    ImgRec r(
        {.25f * tex.width,.25f * tex.height},
        {.75f * tex.width,.25f * tex.height},
        {.75f * tex.width,.75f * tex.height},
        {.25f * tex.width,.75f * tex.height},
        { (float)tex.width,(float)tex.height }
    );
    rectManager.addRect(r);
#if 0
    ImgRec r2(
        {.25f * tex.width,.25f * tex.height},
        {.75f * tex.width,.25f * tex.height},
        {.75f * tex.width,.75f * tex.height},
        {.25f * tex.width,.75f * tex.height},
        { (float)tex.width,(float)tex.height }
    );
    recs.push_back(r2);

    ImgRec r3(
        {.25f * tex.width,.25f * tex.height},
        {.75f * tex.width,.25f * tex.height},
        {.75f * tex.width,.75f * tex.height},
        {.25f * tex.width,.75f * tex.height},
        { (float)tex.width,(float)tex.height }
    );
    recs.push_back(r3);
#endif

    rectManager.updateRecMats(Vector2{ (float)tex.width, (float)tex.height });

    reRenderEveryFrame = false;
}
TexRip::ImageSelectionViewer::~ImageSelectionViewer(){
    
}

void TexRip::ImageSelectionViewer::drawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            drawMenuBarFile();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
bool TexRip::ImageSelectionViewer::ownUpdate(const Vector2& mousePos, const Vector2& mouseDelta) {
    return managePoints(mousePos,mouseDelta);
}
void TexRip::ImageSelectionViewer::drawOverlay(const Vector2& mousePos, const Vector2& mouseDelta) {
    for (auto& r : rectManager.getRecs()) {
        drawRec(r, { 200,200,200,255 }, {255, 185, 64, 255}, BLACK, ORANGE);
    }
    
#if 0
    BeginShaderMode(ShaderManager::projShader);
    float bgColor[] = { 1,0,1,.5 };
    ShaderManager::setProjShaderVals(tex, rectManager.getMatTex(), bgColor);

    const float indCentering = .5f / rectManager.getRecs().size();
    for (size_t i = 0; i < rectManager.getRecs().size(); i++) {
        float ind = ((float)i / rectManager.getRecs().size()) + indCentering;

        Rectangle rb = rectManager.getRecs()[i].boundingBox();
        Rectangle rbNorm = { rb.x / tex.width, rb.y / tex.height, rb.width / tex.width, rb.height / tex.height };

        ShaderManager::RectVertsSrc(rb, rbNorm, { ind,1,1,1 });
    }
    EndShaderMode();
#endif
    

    switch (rectManager.getMode()) {
        case RectManager::MODES::ROTATE: {
            DrawLineEx(rectManager.getRotCenter(), getPosWinToTex(getPosGlobalToWin(mousePos)), zoomIndependent(1), GRAY);
        }break;

        case RectManager::MODES::SCALE: {
            DrawLineEx(rectManager.getSclCenter(), getPosWinToTex(getPosGlobalToWin(mousePos)), zoomIndependent(1), GRAY);
        }break;
    }
}
void TexRip::ImageSelectionViewer::afterWinDraw() {
    if (winProps.isWinHovered && acceptIOKeyCombs) {
        if (Input::isActionActive(Input::Action_open)) {
            openFile();
        }
    }
}

std::vector<TexRip::ImgRec>& TexRip::ImageSelectionViewer::getRecs() {
    return rectManager.getRecs();
}

void TexRip::ImageSelectionViewer::drawRec(const ImgRec& rec, Color lineC, Color lineCSel, Color pntC, Color pntCSel){
    for (int i = 0; i < std::min(4, (int)rec.progress); i++) {
        auto& p1 = rec.pnts[i];
        auto& p2 = rec.pnts[(i + 1) % 4];
        Color LineColor = lineC;
        if (p1.selected && p2.selected) {
            LineColor = lineCSel;
        }
        DrawLineEx(p1.pos, p2.pos, zoomIndependent(1.5f),LineColor);
    }

    constexpr float recW = 4, recH = 4;
    for (int i = 0; i < std::min(4, (int)rec.progress+1); i++) {
        DrawRectanglePro(
            { rec.pnts[i].pos.x, rec.pnts[i].pos.y, zoomIndependent(recW), zoomIndependent(recH) },
            { zoomIndependent(recW / 2), zoomIndependent(recH / 2) },
            0, rec.pnts[i].selected ? pntCSel : pntC
        );
    }

    if (rec.progress == ImgRec::Prog_FULL) {
        constexpr float step = 1.0f / 5;
        for (float x = step/2; x < 1; x += step) {
            Vector2 p1 = {x*(float)tex.width,                   0};
            Vector2 p2 = {x*(float)tex.width, 1*(float)tex.height};
            Vector2 p1Warped = getPosPersp2(p1, rec.inv_persp);
            Vector2 p2Warped = getPosPersp2(p2, rec.inv_persp);
            DrawLineEx(p1Warped, p2Warped, 2, GREEN);
        }
        for (float y = step/2; y < 1; y += step) {
            Vector2 p1 = {0*(float)tex.width, y*(float)tex.height};
            Vector2 p2 = {1*(float)tex.width, y*(float)tex.height};
            Vector2 p1Warped = getPosPersp2(p1, rec.inv_persp);
            Vector2 p2Warped = getPosPersp2(p2, rec.inv_persp);
            DrawLineEx(p1Warped, p2Warped, 2, GREEN);
        }
    }
}

bool TexRip::ImageSelectionViewer::managePoints(const Vector2& mousePos, const Vector2& mouseDelta) {
    bool reRender = rectManager.managePoints(
        getPosWinToTex(getPosGlobalToWin(mousePos)), getOffWinToTex(getOffGlobalToWin(mouseDelta)), 1/getZoom(), 
        winProps.isWinHovered, Vector2{(float)tex.width, (float)tex.height}
    );

    noMoveOrZoom = !rectManager.mayWorldMove();
    return reRender;
}

void TexRip::ImageSelectionViewer::updateRecMats(const Vector2& texSize) {
    rectManager.updateRecMats(texSize);
}

Vector2 TexRip::ImageSelectionViewer::getPosPersp(const Vector2& pos, const float* mat) {
    Vector3 pos3 = { pos.x , pos.y , 1 };
    Vector3 persp = {
        pos3.x * mat[0] + pos3.y * mat[1] + pos3.z * mat[2],
        pos3.x * mat[3] + pos3.y * mat[4] + pos3.z * mat[5],
        pos3.x * mat[6] + pos3.y * mat[7] + pos3.z * mat[8],
    };
    Vector2 res = Vector2{ persp.x, persp.y } / persp.z;
    return res;
}
Vector2 TexRip::ImageSelectionViewer::getPosPersp2(const Vector2& pos, const float* mat) {
    Vector3 pos3 = { pos.x , pos.y , 1 };
    Vector3 persp = {
        pos3.x * mat[0] + pos3.y * mat[3] + pos3.z * mat[6],
        pos3.x * mat[1] + pos3.y * mat[4] + pos3.z * mat[7],
        pos3.x * mat[2] + pos3.y * mat[5] + pos3.z * mat[8],
    };
    Vector2 res = Vector2{ persp.x, persp.y } / persp.z;
    return res;
}

void TexRip::ImageSelectionViewer::openFile() {
    std::string path = "";//PlatformTools::ShowOpenFileDialog("some.png");
    if (path != "") {
        setTex(LoadTexture(path.c_str()));
        changedImage = true;
    }
}

bool TexRip::ImageSelectionViewer::wasImageChanged() {
    bool changed = changedImage;
    changedImage = false;
    return changed;
}

bool TexRip::ImageSelectionViewer::werePointsMoved() const {
    return rectManager.werePointsMoved();
}

const Texture& TexRip::ImageSelectionViewer::getInvMats() const {
    return rectManager.getInvMatTex();
}

void TexRip::ImageSelectionViewer::drawMenuBarFile() {
    if (ImGui::MenuItem("Open", Input::getActionKeyName(Input::Action_open).c_str())) {
        openFile();
    }
}

// ###############################################################################################################################

TexRip::ImageTargetViewer::ImageTargetViewer(const char* name, ImGuiWindowFlags flags) 
    : ImageRipChildWin(name,flags), settingsTitle(winProps.windowName + " - Settings")
{
    menuBarOn(true);

    targetTex.id = 0;
    setTex(targetTex.texture);
    mirrorVert = true;
    alwaysReCenter = false;
    reRenderEveryFrame = false;
}
TexRip::ImageTargetViewer::~ImageTargetViewer() {
    UnloadRenderTexture(targetTex);
}

Vector2 TexRip::ImageTargetViewer::layoutRecs(std::vector<ImgRec>* recs) {
    switch (settings.layOutMode) {
        case LayoutMode_Line:
            return layoutRecsLine(recs);
        case LayoutMode_LineWrap:
            return layoutRecsLineWrap(recs);
        case LayoutMode_Grid:
            return layoutRecsGrid(recs);
        //default:
            // TODO: error
    }
}
Vector2 TexRip::ImageTargetViewer::layoutRecsLine(std::vector<ImgRec>* recs) {
    Vector2 dim = { 0,0 };
    float xOff = 0;
    for (size_t i = 0; i < recs->size();i++) {
        auto& r = (*recs)[i];
        if (r.progress == ImgRec::Prog_FULL) {
            r.outPos = { xOff, 0 };

            xOff += r.corrDim.x + settings.padding;
            dim.x = xOff;
            dim.y = std::max(dim.y, r.corrDim.y + settings.padding);
        }
    }
    return dim;
}
Vector2 TexRip::ImageTargetViewer::layoutRecsLineWrap(std::vector<ImgRec>* recs) {
    Vector2 dim = { 0,0 };
    float xOff = 0;
    float yOff = 0;
    float maxYLine = 0;
    for (size_t i = 0; i < recs->size();i++) {
        auto& r = (*recs)[i];
        if (r.progress == ImgRec::Prog_FULL) {
            if (xOff != 0 && xOff + r.corrDim.x + settings.padding > settings.layOutMaxWidth) {
                xOff = 0;
                yOff += maxYLine;
                maxYLine = 0;
            }
            r.outPos = { xOff, yOff };

            xOff += r.corrDim.x + settings.padding;
            dim.x = std::max(dim.x, xOff);
            maxYLine = std::max(maxYLine, r.corrDim.y + settings.padding);
            dim.y = std::max(dim.y, yOff + maxYLine);
        }
    }
    return dim;
}
Vector2 TexRip::ImageTargetViewer::layoutRecsGrid(std::vector<ImgRec>* recs) {
    Vector2 maxRectDim = { 0,0 };
    for (auto& r : *recs) {
        maxRectDim.x = std::max(maxRectDim.x, r.corrDim.x);
        maxRectDim.y = std::max(maxRectDim.y, r.corrDim.y);
    }

    size_t sideLength = (size_t)std::ceil(std::sqrtf((float)recs->size()));

    Vector2 dim = { 
        (maxRectDim.x + settings.padding) * sideLength,
        (maxRectDim.y + settings.padding) * sideLength
    };

    for (size_t i = 0; i < recs->size(); i++) {
        (*recs)[i].outPos = {
            (maxRectDim.x + settings.padding) * (i%sideLength),
            (maxRectDim.y + settings.padding) * (i/sideLength)
        };
    }
    return dim;
}
void TexRip::ImageTargetViewer::rerenderTargetTex(const Texture2D& srcTex, const Texture2D& mats, std::vector<ImgRec>& recs) {
    //check if needs resize
    targetDim = layoutRecs(&recs);

    Vector2 newSize = { 0,0 };
    bool additionalCond = targetTex.id == 0;
    if (World2DViewer::doesViewPortNeedResize(targetDim.x, targetDim.y, (float)targetTex.texture.width, (float)targetTex.texture.height, 100, 10, newSize, additionalCond)) {
        if (targetTex.id != 0) {
            UnloadRenderTexture(targetTex);
        }
        targetTex = LoadRenderTexture((int)newSize.x, (int)newSize.y);
        setTex(targetTex.texture, false);
    }

    BeginTextureMode(targetTex);
    Color bg = imguiExt::imColortoRaylib(settings.mainBGColor);
    ClearBackground(bg);

    BeginShaderMode(ShaderManager::projShader);

    ShaderManager::setProjShaderVals(srcTex, mats, (const float*)&settings.imgBGColor);

    const float indCentering = .5f / recs.size();
    
    for (size_t i = 0; i < recs.size(); i++) {
        auto& r = recs[i];
        if (r.progress == ImgRec::Prog_FULL) {
            float ind = ((float)i / recs.size()) + indCentering;
            ShaderManager::RectVerts({ r.outPos.x, r.outPos.y, r.corrDim.x, r.corrDim.y }, {ind,1,1,1});
        }
    }

    EndShaderMode();

    EndTextureMode();
    queueRerender();
    editedSinceSavedB = true;
}

void TexRip::ImageTargetViewer::drawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            drawMenuBarFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            drawMenuBarSettings();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
void TexRip::ImageTargetViewer::afterWinDraw() {
    if (winProps.isWinHovered && acceptIOKeyCombs) {
        if (Input::isActionActive(Input::Action_save)) {
            save();
        }
    }

    drawSettings();
}
void TexRip::ImageTargetViewer::drawOverlay(const Vector2& mousePos, const Vector2& mouseDelta) {
    switch (settings.layOutMode) {
        case LayoutMode_LineWrap: {
            DrawLineEx({                       0, 0 }, {                       0, targetDim.y }, zoomIndependent(1), MAGENTA);
            DrawLineEx({ settings.layOutMaxWidth, 0 }, { settings.layOutMaxWidth, targetDim.y }, zoomIndependent(1), MAGENTA);
        } break;
    }
    DrawRectangleLinesEx({ 0,0,targetDim.x,targetDim.y }, zoomIndependent(1), GREEN);
}
void TexRip::ImageTargetViewer::drawSettings() {
    if (settingsWinOpen) {
        if (ImGui::Begin(settingsTitle.c_str(), &settingsWinOpen, 0)) {
            bool settingsChanged = false;

            if (imguiExt::imguiColorPickerButton("Main Background Color", settings.mainBGColor))
                settingsChanged = true;
            if (imguiExt::imguiColorPickerButton("Image Background Color", settings.imgBGColor))
                settingsChanged = true;

            ImGui::Separator();
            ImGui::TextUnformatted("Layout");
            static const char* layoutRecsModesStrs[] = {"Line", "Line Wrap", "Square Grid"};
            if (ImGui::Combo("Layout Mode", &settings.layOutMode, layoutRecsModesStrs, 3))
                settingsChanged = true;

            switch (settings.layOutMode) {
                case LayoutMode_LineWrap: {
                    if (ImGui::DragFloat("Max Width", &settings.layOutMaxWidth, 1.0f, 0.1f, FLT_MAX))
                        settingsChanged = true;
                } break;
            }

            if (ImGui::DragFloat("Padding", &settings.padding, 0.2f, 0.0f, FLT_MAX))
                settingsChanged = true;
            

            if (settingsChanged) {
                needsReRender = true;
            }
        }
        ImGui::End();
    }
}

bool TexRip::ImageTargetViewer::save() {
    if (lastSavePath != "") {
        return saveTex(lastSavePath.c_str());
    }
    else {
        return saveAs();
    }
}
bool TexRip::ImageTargetViewer::saveAs() {
    std::string savePath = "";//PlatformTools::ShowSaveFileDialog("unnamed.png");
    if (savePath != "") { // not cancelled
        return saveTex(savePath.c_str());
    }
    else {
        return false;
    }
}
bool TexRip::ImageTargetViewer::saveTex(const char* path) {
    bool success;
    Image img = LoadImageFromTexture(tex);
    ImageFlipVertical(&img);
    success = ExportImage(img, path);
    UnloadImage(img);
    
    lastSavePath = path;
    if (success) {
        editedSinceSavedB = false;
    }
    return success;
}
bool TexRip::ImageTargetViewer::editedSinceSaved() {
    return editedSinceSavedB;
}

void TexRip::ImageTargetViewer::drawMenuBarFile() {
    if (ImGui::MenuItem("Save", Input::getActionKeyName(Input::Action_save).c_str())) {
        save();
    }
    if (ImGui::MenuItem("Save As")) {
        saveAs();
    }
}
void TexRip::ImageTargetViewer::drawMenuBarSettings() {
    if (ImGui::MenuItem("Open Output Settings")) {
        settingsWinOpen = true;
    }
}

// ###############################################################################################################################
TexRip::ImageRipperWindow::ImageRipperWindow(const Texture2D& tex, const char* name, ImGuiWindowFlags flags) 
    : selWin(tex, (name+std::string(" - Editor")).c_str(), flags), 
    texWin((name + std::string(" - Output")).c_str(), flags), name(name)
{
    selWin.winProps.winOpenPtr = &winOpen;

    reRenderOutput();
    texWin.reCenterTex();
    texWin.winProps.winOpenPtr = &winOpen;
}

void TexRip::ImageRipperWindow::update() {
    manageTexRipping();
}

void TexRip::ImageRipperWindow::loadTex(const char* str) {
    setTex(LoadTexture(str));
}
void TexRip::ImageRipperWindow::setTex(Texture2D newTex) {
    selWin.setTex(newTex);

    reRenderOutput();
}
void TexRip::ImageRipperWindow::draw(const Vector2& mousePos, const Vector2& mouseDelta) {
    if (parentWinOpen) {
        drawParentWin();
    }

    selWin.draw(mousePos,mouseDelta);
    update();
    texWin.draw(mousePos,mouseDelta);
    if (!winOpen && !toDelete) {
        if (texWin.editedSinceSaved()) {
            dontYouWantToSave();
        }
    }
}

void TexRip::ImageRipperWindow::dockedWinView() {
    oneWinView();
    parentWinFlags |= ImGuiWindowFlags_NoMove;
    parentWinFlags &= ~ImGuiWindowFlags_MenuBar;
}
void TexRip::ImageRipperWindow::oneWinView() {
    parentWinOpen = true;

    ImGuiID centralNode = parentDockSpaceID;
    ImGuiID leftID = -1, rightID = -1;
    auto cNode = ImGui::DockBuilderGetNode(centralNode);

    if (cNode->IsSplitNode()) {
        ImGui::DockBuilderRemoveNodeChildNodes(centralNode);
    }

    ImGui::DockBuilderSplitNode(centralNode, ImGuiDir_Left, 0.5f, &leftID, &rightID);

    ImGui::DockBuilderDockWindow(selWin.winProps.windowName.c_str(), leftID);
    ImGui::DockBuilderDockWindow(texWin.winProps.windowName.c_str(), rightID);

    selWin.setWinDocked(true);
    texWin.setWinDocked(true);

    parentWinFlags &= ~ImGuiWindowFlags_NoMove;
    parentWinFlags |= ImGuiWindowFlags_MenuBar;
}
void TexRip::ImageRipperWindow::floatWinView() {
    parentWinOpen = false;

    ImGuiID centralNode = parentDockSpaceID;
    ImGui::DockBuilderRemoveNodeChildNodes(centralNode);
    
    selWin.setWinDocked(false);
    texWin.setWinDocked(false);
}

void TexRip::ImageRipperWindow::drawParentWin() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin(name.c_str(), &winOpen, parentWinFlags | ImGuiWindowFlags_NoCollapse)) { //ImGuiWindowFlags_NoBringToFrontOnFocus
        ImGui::PopStyleVar();
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                selWin.drawMenuBarFile();
                ImGui::Separator();
                texWin.drawMenuBarFile();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings")) {
                texWin.drawMenuBarSettings();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    }
    parentDockSpaceID = ImGui::DockSpace(ImGui::GetID(name.c_str()), ImGui::GetContentRegionAvail(), ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit); //ImGuiDockNodeFlags_NoDocking ImGuiDockNodeFlags_AutoHideTabBar ImGuiDockNodeFlags_NoWindowMenuButton
    
    ImGui::End();
    ImGui::PopStyleVar();
}

void TexRip::ImageRipperWindow::manageTexRipping() {
    if (selWin.wasImageChanged() || selWin.werePointsMoved() || texWin.needsReRender) {
        reRenderOutput();
    }
}
void TexRip::ImageRipperWindow::reRenderOutput() {
    texWin.rerenderTargetTex(selWin.tex, selWin.getInvMats(), selWin.getRecs());
}

void TexRip::ImageRipperWindow::dontYouWantToSave() {
    ImGui::OpenPopup("Close?");

    ImGui::SetNextWindowFocus();
    if (ImGui::BeginPopupModal("Close?", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text("The image will be closed.\nDo you want to save?");
        ImGui::Separator();

        if (ImGui::Button("Discard", ImVec2(120, 0))) {
            close();
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (texWin.save()) {
                close();
                ImGui::CloseCurrentPopup(); 
            }
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            winOpen = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
bool TexRip::ImageRipperWindow::isWinOpen() {
    return toDelete;
}
void TexRip::ImageRipperWindow::tryClose() {
    winOpen = false;
}
void TexRip::ImageRipperWindow::close() {
    selWin.winProps.winOpen = false;
    texWin.winProps.winOpen = false;
    toDelete = true;
}

// ###############################################################################################################################

int TexRip::TexRipper::WinViewManager::winViewMode = -1;
size_t TexRip::TexRipper::WinViewManager::activeWinID = 0;
bool TexRip::TexRipper::WinViewManager::winOpen = false;
bool TexRip::TexRipper::WinViewManager::changed = false;

void TexRip::TexRipper::WinViewManager::updateBefore() {
    if (winViewMode == WinViewModes::DOCKED) {
        drawWin();
    }
}
void TexRip::TexRipper::WinViewManager::updateWinView() {
    if (changed) {
        switch (winViewMode) {
        case WinViewModes::DOCKED:
            for (auto& w : wins) {
                w->dockedWinView();
            }
            break;
        case WinViewModes::ONE:
            for (auto& w : wins) {
                w->oneWinView();
            }
            break;
        case WinViewModes::FLOAT:
            for (auto& w : wins) {
                w->floatWinView();
            }
            break;
        }
        switch (winViewMode) {
            case WinViewModes::DOCKED:
                for (auto& w : wins) {
                    ImGui::DockBuilderDockWindow(w->name.c_str(), w->externalDockSpaceID);
                }
                break;
            case WinViewModes::ONE:
                break;
            case WinViewModes::FLOAT:
                break;
        }
    }

    changed = false;
}

void TexRip::TexRipper::WinViewManager::queueViewMode(WinViewMode mode) {
    switch (mode) {
        case WinViewModes::DOCKED:
            winViewMode = WinViewModes::DOCKED;
            winOpen = true;
            break;
        case WinViewModes::ONE:
            winViewMode = WinViewModes::ONE;
            winOpen = false;
            break;
        case WinViewModes::FLOAT:
            winViewMode = WinViewModes::FLOAT;
            winOpen = false;
            break;
    }
    changed = true;
}
void TexRip::TexRipper::WinViewManager::reQueueViewMode() {
    queueViewMode(winViewMode);
}
size_t TexRip::TexRipper::WinViewManager::getActiveWinID() {
    return activeWinID;
}

void TexRip::TexRipper::WinViewManager::drawWin() {
    if (winOpen) {
        static ImGuiWindowFlags WinFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        if (ImGui::Begin("WINS", NULL, WinFlags)) {
            //ImGui::SetCursorPos({ ImGui::GetCursorPosX(), 0 }); 
            ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_Reorderable; // ImGuiTabBarFlags_AutoSelectNewTabs
            if (ImGui::BeginTabBar("WindowContainerTabBar", tabFlags)){
                for (size_t i = 0; i < wins.size(); i++) {
                    ImageRipperWindow* win = wins[i];

                    ImGuiTabBarFlags tabItemFlags = ImGuiTabItemFlags_None;

                    if (changed && i == 0) {
                        tabItemFlags |= ImGuiTabItemFlags_SetSelected;
                    }

                    bool open = true;
                    if (ImGui::BeginTabItem(win->name.c_str(), &open, tabItemFlags)) {
                        activeWinID = i;
                        ImGui::EndTabItem();
                    }

                    if (!open)
                        win->tryClose();
                }
                ImGui::EndTabBar();
            }


            ImVec2 size = ImGui::GetContentRegionAvail();
            for (size_t i = 0; i < wins.size(); i++) {
                ImageRipperWindow* win = wins[i];

                ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_NoTabBar;
                if (i != activeWinID && !changed) { // && !changed is used to activate every dockspace on the first frame the docked mode is enabled to prevent issues with never used dockspaces
                    dockFlags |= ImGuiDockNodeFlags_KeepAliveOnly;
                }
                win->externalDockSpaceID = ImGui::DockSpace(ImGui::GetID((win->name + "ext").c_str()), size, dockFlags);
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }
}

// #

TexRip::ImageRipperWindow* TexRip::TexRipper::getActiveWin() {
    return wins[WinViewManager::getActiveWinID()];
}

Vector2 TexRip::TexRipper::lastMousePos = { 0,0 };
Vector2 TexRip::TexRipper::mouseDelta = { 0,0 };
bool TexRip::TexRipper::settingsWinOpen = false;

std::vector<TexRip::ImageRipperWindow*> TexRip::TexRipper::wins;
std::vector<TexRip::TexRipper::DroppedFile> TexRip::TexRipper::droppedFileNames;

size_t TexRip::TexRipper::currentSettingCategory = 0;
std::string TexRip::TexRipper::settingsCategories[SettingsCategories_COUNT];

void TexRip::TexRipper::init() {
    WinViewManager::queueViewMode(WinViewManager::WinViewModes::DOCKED);

    settingsCategories[SettingsCategories_General] = "General";
    settingsCategories[SettingsCategories_Key_Bindings] = "KeyBindings";
}
void TexRip::TexRipper::destroy() {
    for (auto& w : wins) {
        delete w;
    }
}

void TexRip::TexRipper::draw() {
    mouseDelta = GetMousePosition() - lastMousePos;

    drawMainMenuBar();

    drawSettingsWindow();

    WinViewManager::updateBefore();

    if (IsFileDropped()) {
        char** paths;
        int count;
        paths = GetDroppedFiles(&count);
        for (int i = 0; i < count; i++) {
            char* path = paths[i];
            DroppedFile f;
            f.name = GetFileName(path);
            f.ext = GetFileExtension(f.name.c_str());
            f.path = path;
            droppedFileNames.push_back(f);
        }
        ClearDroppedFiles();
    }

    if (!wins.empty()) { //delete closed Wins
        for (int i = wins.size() - 1; i >= 0; i--) {
            if (wins[i]->toDelete) {
                ImageRipperWindow* ptr = wins[i];
                wins.erase(wins.begin() + i);
                delete ptr;
            }
        }
    }

    for (auto& w : wins) {
        w->draw(GetMousePosition(), mouseDelta);
    }

    WinViewManager::updateWinView();

    if (!droppedFileNames.empty()) {
        drawDroppedFilesMenu();
    }
    
    lastMousePos = GetMousePosition();
}

void TexRip::TexRipper::openFileName(const char* name) {
    Texture t = LoadTexture(name);
    if (t.id != 0) {
        addImage(GetFileName(name), t);
    }
}
void TexRip::TexRipper::drawDroppedFilesMenu() {
    if (ImGui::Begin("Dropped Files")) {
        ImGuiTableFlags flags = 
            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | 
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
        if (ImGui::BeginTable("Dropped Files Table", 4, flags)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Path");
            ImGui::TableSetupColumn("Open");
            ImGui::TableHeadersRow();

            for (auto& f : droppedFileNames) { //ImGui::AlignTextToFramePadding();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(f.name.c_str());

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(f.ext.c_str());

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(f.path.c_str());

                ImGui::TableNextColumn();
                if (ImGui::Button("Open")) {
                    openFileName(f.path.c_str());
                }
            }
            ImGui::EndTable();
            if (ImGui::Button("Open All")) {
                for (auto& f : droppedFileNames) {
                    openFileName(f.name.c_str());
                }
            }
        }
    }
    ImGui::End();
}

void TexRip::TexRipper::drawMainMenuBar() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            getActiveWin()->selWin.drawMenuBarFile();
            ImGui::Separator();
            getActiveWin()->texWin.drawMenuBarFile();

            ImGui::Separator();
            if (ImGui::MenuItem("Quit")) {
                // TODO
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")) {
            ImGui::TextUnformatted("Active Window:");
            ImGui::Indent();
            getActiveWin()->texWin.drawMenuBarSettings();
            ImGui::Unindent();

            ImGui::Separator();
            if (ImGui::MenuItem("Settings")) {
                settingsWinOpen = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Docked")) {
                WinViewManager::queueViewMode(WinViewManager::WinViewModes::DOCKED);
            }
            if (ImGui::MenuItem("One")) {
                WinViewManager::queueViewMode(WinViewManager::WinViewModes::ONE);
            }
            if (ImGui::MenuItem("Float")) {
                WinViewManager::queueViewMode(WinViewManager::WinViewModes::FLOAT);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug")) {
            if (ImGui::MenuItem("new Win")) {
                addImage(("Thing"+std::to_string(rand())).c_str(), LoadTexture("assets/ressources/img2.png"));
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar();
}
void TexRip::TexRipper::drawSettingsWindow() {
    if (settingsWinOpen) {
        if (ImGui::Begin("Settings", &settingsWinOpen, 0)) {
            if (ImGui::BeginChild("CategoryChildWin", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.25f, 0), false, 0)) {
                if (ImGui::BeginListBox("listbox 1", {-1,ImGui::GetContentRegionAvail().y})) {
                    for (int n = 0; n < SettingsCategories_COUNT; n++)
                    {
                        const bool is_selected = (currentSettingCategory == n);
                        if (ImGui::Selectable(settingsCategories[n].c_str(), is_selected, ImGuiSelectableFlags_SelectOnClick))
                            currentSettingCategory = n;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndListBox();
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();

            switch (currentSettingCategory) {
                case SettingsCategories_General:
                    break;
                case SettingsCategories_Key_Bindings:
                    Input::drawSettingsTable();
                    break;
            }
            
        }
        ImGui::End();
    }
}

void TexRip::TexRipper::addImage(const char* name, const Texture2D& tex) {
    ImageRipperWindow* win = new ImageRipperWindow(tex, name);
    wins.push_back(win);

    WinViewManager::reQueueViewMode();
}

void TexRip::TexRipper::debugDraw() {
    if (wins.size() > 0) {
        const Texture& t = wins[0]->selWin.getInvMats();
        DrawTextureEx(t, { 0,0 }, 0, 10, WHITE);
    }
}

/*

void TexRip::TexRipper::WinViewManager::dockedWinView() {
winViewMode = WinViewModes::DOCKED;
winOpen = true;
for (auto& w : wins) {
w->dockedWinView();
ImGui::DockBuilderDockWindow(w->name.c_str(), w->externalDockSpaceID);
}
}
void TexRip::TexRipper::WinViewManager::oneWinView() {
winViewMode = WinViewModes::ONE;
winOpen = false;
//ImGui::DockBuilderRemoveNodeChildNodes(dockspaceID);
for (auto& w : wins) {
w->oneWinView();
}
}
void TexRip::TexRipper::WinViewManager::floatWinView() {
winViewMode = WinViewModes::FLOAT;
winOpen = false;
for (auto& w : wins) {
w->floatWinView();
}
}



if (rectManager.modeChanged()) {
if (rectManager.getMode() == RectManager::MODES::NONE) {
noMoveOrZoom = false;
reRender = true;
}
else {
noMoveOrZoom = true;
}
}



auto& p2 = rec.pnts[(i + 4 - 1) % 4];

bool TexRip::ImageSelectionViewer::RectManager::checkUpdateRec() const { // returns true if at least 1 rect mat got updated
bool isNotUpToDate = false;
for (auto& r : recs) {
for (int i = 0; i < 4; i++) {
if (r.pnts[i].pos != r.lastRipPos[i]) {
isNotUpToDate = true;
break;
}
}
}
return isNotUpToDate;
}

bool checkUpdateRec() const; // returns true if rects have been altered since last mat generation


switch (rectManager.getMode()) {
case RectManager::MODES::ROTATE: {
DrawLineEx(getPosTexToWin(rectManager.getRotCenter()), getPosGlobalToWin(mousePos), zoomIndependent(1), GRAY);
}break;

case RectManager::MODES::SCALE: {
DrawLineEx(getPosTexToWin(rectManager.getSclCenter()), getPosGlobalToWin(mousePos), zoomIndependent(1), GRAY);
}break;
}


for (auto& r : recs) {
for (auto& p : r.pnts) {
Vector2 pGlobalPos = getPosWinToGlobal( getPosTexToWin(p.pos));
p.candidate = CheckCollisionPointCircle(GetMousePosition(), pGlobalPos, 50);
}
}

float minDist = INFINITY;
int nearest = -1;
int backup = -1;
for (int r = 0; r < recs.size();r++) {
for (int p = 0; p < 4;p++) {
if (recs[r].pnts[p].candidate) {
Vector2 pGlobalPos = getPosWinToGlobal( getPosTexToWin(recs[r].pnts[p].pos));
float dist = Vector2Distance(GetMousePosition(), pGlobalPos);
if (dist < minDist) {
if (!recs[r].pnts[p].selected) {
minDist = dist;
nearest = r<<4 | p;
}
else {
backup = r<<4 | p;
}
}
recs[r].pnts[p].candidate = false;
}
if(deselect)
recs[r].pnts[p].selected = false;
}
}

int pnt = -1;
if (nearest != -1) {
return nearest;
}
else {
return backup;
}


//DrawCircle((int)pos.x, (int)pos.y, zoomIndependent(3), rec.pnts[i].selected ? pntCSel : pntC);

void TexRip::ImageSelectionViewer::drawRec(ImgRec& rec, Color lineC, Color lineCSel, Color pntC, Color pntCSel){
for (int i = 0; i < 4; i++) {
auto& p1 = rec.pnts[i];
auto& p2 = rec.pnts[(i + 4 - 1) % 4];
Color LineColor = lineC;
if (p1.selected && p2.selected) {
LineColor = lineCSel;
}
DrawLineEx(
getPosTexToWin(p1.pos),
getPosTexToWin(p2.pos)
,zoomIndependent(1.5f),LineColor
);
}

constexpr float recW = 4, recH = 4;
for (int i = 0; i < 4; i++) {
Vector2 pos = getPosTexToWin(rec.pnts[i].pos);
//DrawCircle((int)pos.x, (int)pos.y, zoomIndependent(3), rec.pnts[i].selected ? pntCSel : pntC);
DrawRectanglePro({ pos.x,pos.y,zoomIndependent(recW),zoomIndependent(recH) }, { zoomIndependent(recW / 2),zoomIndependent(recH / 2) }, 0, rec.pnts[i].selected ? pntCSel : pntC);
}
}

for (auto& r : recs) {
drawRec(r, { 200,200,200,255 }, {255, 185, 64, 255}, BLACK, ORANGE);
}


DrawCircle(0, 0, 30, RED);
viewerMats.updateWithRecs(recs, false);

BeginShaderMode(ShaderManager::projShader);
float bgColor[] = { 1,0,1,.5 };
ShaderManager::setProjShaderVals(tex, viewerMats.tex, bgColor);

const float indCentering = .5f / recs.size();
for (size_t i = 0; i < recs.size(); i++) {
float ind = ((float)i / recs.size()) + indCentering;

Rectangle rb = recs[i].boundingBox();
Rectangle rbNorm = { rb.x / tex.width, rb.y / tex.height, rb.width / tex.width, rb.height / tex.height };

Vector2 pos = getPosTexToWin({ rb.x,rb.y });
Vector2 size = getOffTexToWin({ rb.width, rb.height });

ShaderManager::RectVertsSrc({ pos.x, pos.y, size.x, size.y }, rbNorm, { ind,1,1,1 });
}
EndShaderMode();

//Vector2 p = getPosTexToWin({ 0,0 });
//ShaderManager::RectVerts({p.x,p.y,(float)tex.width,(float)tex.height},{ind,1,1,1});

SetShaderValueTexture(ShaderManager::projShader,GetShaderLocation(ShaderManager::projShader,"inputImageTexture"), srcTex);

SetShaderValueTexture(ShaderManager::projShader, GetShaderLocation(ShaderManager::projShader, "invMatTex"), mats);

float w = (float)srcTex.width, h = (float)srcTex.height;
SetShaderValue(ShaderManager::projShader, GetShaderLocation(ShaderManager::projShader, "width"), &w, SHADER_UNIFORM_FLOAT);
SetShaderValue(ShaderManager::projShader, GetShaderLocation(ShaderManager::projShader, "height"), &h, SHADER_UNIFORM_FLOAT);

SetShaderValueV(ShaderManager::projShader, GetShaderLocation(ShaderManager::projShader, "bgColor"), &settings.imgBGColor, SHADER_UNIFORM_VEC4, 1);


persMatsImg.data = NULL;
persMatsImg.height = 3;
persMatsImg.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32;
persMatsImg.mipmaps = 1;
persMats.id = 0;

int targetLen = recs.size();
if (targetLen != persMatsImg.width) {
persMatsImg.width = targetLen;
if (persMatsImg.data != NULL) {
delete[] persMatsImg.data;
UnloadTexture(persMats);
persMats.id = 0;
}
persMatsImg.data = new float[3 * targetLen * 3];

}
for (int i = 0; i < persMatsImg.width; i++) {
for (int j = 0; j < 3; j++) {
((Vector3*)persMatsImg.data)[j*persMatsImg.width+i] = {recs[i].inv_persp[j*3+0],recs[i].inv_persp[j*3+1],recs[i].inv_persp[j*3+2]};
}
}

if (persMats.id == 0) {
persMats = LoadTextureFromImage(persMatsImg);
}
else {
UpdateTexture(persMats, persMatsImg.data);
}

if (!leftEmpty) {
ImGui::DockBuilderRemoveNodeDockedWindows(leftID);
}

if (!rightEmpty) {
ImGui::DockBuilderRemoveNodeDockedWindows(rightID);
}

leftID = cNode->ChildNodes[0]->ID;
leftEmpty = cNode->ChildNodes[0]->IsEmpty();
rightID = cNode->ChildNodes[1]->ID;
rightEmpty = cNode->ChildNodes[1]->IsEmpty();


auto start_time = std::chrono::high_resolution_clock::now();

auto end_time = std::chrono::high_resolution_clock::now();
auto time = end_time - start_time;
std::cout << "took: " << time / std::chrono::milliseconds(1) << "ms" << std::endl;

//DrawRectangle((float)renderTex.texture.width / 2-50, (float)renderTex.texture.height / 2 - 50, 100, 100, ORANGE);

//DrawRectangleGradientEx({ 0,0,(float)targetTex.texture.width,(float)targetTex.texture.height }, PINK, YELLOW, GREEN, BLUE);

std::vector<std::array<float, 9>> mats(selWin.recs.size());
for (int i = 0; i < selWin.recs.size(); i++) {
mats[i] = selWin.recs[i].inv_persp;
}
//ShaderManager::SetShaderValueMat3(ShaderManager::projShader, locIndHom, &mats[0], selWin.recs.size());

//float numOfRecs = (float)selWin.recs.size();
//SetShaderValue(ShaderManager::projShader, GetShaderLocation(ShaderManager::projShader, "numOfRecs"), &numOfRecs, SHADER_UNIFORM_FLOAT);

//int locIndHom = GetShaderLocation(ShaderManager::projShader, "inverseHomographyMatrix");
//ShaderManager::SetShaderValueMat3(ShaderManager::projShader, locIndHom, &selWin.recs[0].inv_persp[0], 1);

//cv::Mat flatImg;
//cv::warpPerspective(img_cv, flatImg, persp, {(int)std::ceil(width), (int)std::ceil(height)});

img_r.width = tex.width;
img_r.height = tex.height;
img_r.data = new float[img_r.width * img_r.height * 3];
img_r.mipmaps = 1;
img_r.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32;


void updateImgs();

void ImageSelectionViewer::updateImgs() {
if (img_r.width != tex.width || img_r.height != tex.height) {
delete[] img_r.data;
img_r.data = new float[img_r.width * img_r.height * 3];
}
for (int y = 0; y < img_r.height; y++) {
for (int x = 0; x < img_r.width; x++) {
int ind = ((y * img_r.width) + x) * 3;
((float*)img_r.data)[ind] = (float)x;
((float*)img_r.data)[ind+1] = (float)y;
((float*)img_r.data)[ind+1] = (float)1;
}
}

img_cv = cv::Mat(img_r.width, img_r.height, CV_32FC3, (float*)img_r.data);
}



*/
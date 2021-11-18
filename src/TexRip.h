#ifndef _CV_IMAGE_VIEWER
#define _CV_IMAGE_VIEWER

#include "raylib.h"
#include <vector>
//#include "opencv2/core.hpp"
#include "ImGuiImageViewers.h"
#include "utils/overrideStack.h"
#include "utils/map.h"

namespace TexRip {
    struct ImgPoint {
        Vector2 pos = { 0,0 };
        bool selected;
        bool candidate = false;
    };

    struct Rec {
        ImgPoint pnts[4];
        bool isValidPerspRec() const;
        void expandPerspFitRec(const Rec& rec);
    };

    class ImgRec {
    public:
        enum {
            Type_Simple,
            Type_2Part
        };

        struct Colors {
            Color line;
            Color lineSel; 
            Color pnt; 
            Color pntSel;
        };

        static Colors standardColors;

        Rec rec;
        Vector2 lastRipPos[4];
        float persp[9];
        float inv_persp[9];
        Vector2 outPos;
        Vector2 corrDim;
        int8_t progress = 4;

        virtual bool needsUpdate() const = NULL;
        virtual void calcPersp(const Vector2& imgDim) = NULL;

        void drawRecRaw(const Rec& rec, int8_t progress_, const Colors& colors, float zoomf);
        void drawRecPersLines(float w, float h,float zoomf);

        virtual uint8_t getType() const = NULL;
        virtual bool isComplete() const = NULL;
        virtual std::pair<bool,bool> updateConstruct(const Vector2& mousePosTexRel) = NULL;
        virtual void draw(float w, float h,float zoomf) = NULL;
    };
    class ImgRecSimple : public ImgRec {
    public:
        enum PROGRESS {
            Prog_0 = 0,
            Prog_1 = 1,
            Prog_2 = 2,
            Prog_3 = 3,
            Prog_FULL = 4
        };

        ImgRecSimple();
        ImgRecSimple(const Vector2& pos1, const Vector2& pos2, const Vector2& pos3, const Vector2& pos4, const Vector2& imgDim);

        uint8_t getType() const override;
        bool needsUpdate() const override;
        void calcPersp(const Vector2& imgDim) override;
        bool isComplete() const override;
        std::pair<bool,bool> updateConstruct(const Vector2& mousePosTexRel) override;
        void draw(float w, float h,float zoomf) override;
    };

    class ImgRec2part : public ImgRec {
    public:
        enum PROGRESS {
            Prog_0_0 = 0,
            Prog_0_1 = 1,
            Prog_0_2 = 2,
            Prog_0_3 = 3,
            Prog_1_0 = 4,
            Prog_1_1 = 5,
            Prog_1_2 = 6,
            Prog_1_3 = 7,
            Prog_FULL = 8
        };

        static Colors displayColors;

        Rec recDisplay;
        Vector2 lastDisplayPos[4];

        uint8_t getType() const override;
        bool needsUpdate() const override;
        void calcPersp(const Vector2& imgDim) override;
        bool isComplete() const override;
        std::pair<bool,bool> updateConstruct(const Vector2& mousePosTexRel) override;
        void draw(float w, float h,float zoomf) override;
    };

    class ImageRipChildWin : public TextureViewer {
    public:
        ImageRipChildWin(Texture2D tex_, const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None, bool doAdjZoom = true);
        ImageRipChildWin(const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None, bool doAdjZoom = true);

        bool acceptIOKeyCombs = true;

        void setWinDocked(bool docked);
    };

    class ImageSelectionViewer : public ImageRipChildWin {
    protected:
        class RectManager {
        public:
            enum MODES {
                NONE = 0,
                MOVE,
                ROTATE,
                SCALE,
                CONSTRUCT
            };

            int addRectType = ImgRec::Type_Simple;
            
        private:

            class PersMat {
            protected:
                Image img;
            public:
                Texture tex;

                PersMat();
                ~PersMat();

                void updateWithRecs(const utils::Map<size_t, ImgRec*>& recs, bool inverse);
            };

            struct RectPointID {
                size_t rectID;
                uint8_t pointID;

                inline bool operator<(const RectPointID& b) const {
                    return ((rectID << 8) | pointID) < ((b.rectID << 8) | b.pointID);
                }
            };

            bool worldMayMove = true;

            PersMat matsTex;
            PersMat invMatsTex;

            bool pointsWereMoved = false;

            //std::vector<ImgRec*> recs;
            utils::Map<size_t, ImgRec*> recs;
            utils::Map<RectPointID, ImgPoint*> points;
            
            size_t ID_counter = 0;
            OverrideStack<utils::Map<size_t, ImgRec*>> undoBuffer;

            uint8_t mode = MODES::NONE;
            bool needsMatUpdate = true;
            bool needsMatUpdateForce = false;

            Vector2 moveOff = { 0,0 };

            float rotOff = 0;
            Vector2 rotCenter = { 0,0 };

            float lastSclAmt = 1;
            Vector2 sclCenter = { 0,0 };
            //Vector2 globalSclCenter = sclCenter;
            float sclMouseStartDist = 1;

            size_t constrID = -1;

        public:
            RectManager();
            ~RectManager();
            
            bool managePoints(const Vector2& mousePosTexRel, const Vector2& mouseDeltaTexRel, const float zoomF, const bool winIsHovered, const Vector2& texSize); // returns true if reRender is needed

            utils::Map<size_t, ImgRec*>& getRecs();
            uint8_t getMode() const;
            bool werePointsMoved() const;
            bool mayWorldMove() const;

            const Vector2& getRotCenter() const;
            const Vector2& getSclCenter() const;

            const Texture2D& getMatTex() const;
            const Texture2D& getInvMatTex() const;

            void updateRecMats(const Vector2& texSize, bool forceTexUpdate = false);

        private:
            bool startPointMode(const Vector2& mousePosTexRel); //returns true if mode started
            bool startMove();
            bool startRotate();
            bool startScale(const Vector2& mousePosTexRel);
            bool addRectConstr();

            bool updatePointsMove(const Vector2& mouseDeltaTexRel, const float fact);
            bool updatePointsRotate(const Vector2& mousePosTexRel, const Vector2& mouseDeltaTexRel, const float fact);
            bool updatePointsScale(const Vector2& mousePosTexRel, const float fact);
            bool updatePointsConstruct(const Vector2& mousePosTexRel);

            void stopPointMode(bool cancel);
            void stopPointModeMove();
            void stopPointModeRotate();
            void stopPointModeScale();
            void stopPointModeConstruct(bool cancel);

            bool selectWithMouse(const Vector2& mousePosTexRel, const float zoomF);
            void selectAllOrNone(int force = -1);
            void selectLinked();

            void removeRect(const size_t& id);

            RectPointID getSelPointInd(const Vector2& mousePosTexRel, const float zoomF, bool deselect);
            Vector2 getMeanSelPntPos(int& numRef) const;

            void addUndoState();
            bool undo(); // returns true if undo was successful
        };

        RectManager rectManager;

        bool changedImage = false;

    public:
        ImageSelectionViewer(const Texture2D& tex, const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
        ~ImageSelectionViewer();

        utils::Map<size_t, ImgRec*>& getRecs();

        void tryOpenFile();

        bool wasImageChanged();
        bool werePointsMoved() const;

        const Texture& getInvMats() const;

        void drawMenuBarFile();
        static Vector2 getPosPersp(const Vector2& pos, const float* mat);
    protected:
        void openFile(const char* path);
        void drawMenuBar() override;
        bool ownUpdate(const Vector2& mousePos, const Vector2& mouseDelta) override;
        void drawOverlay(const Vector2& mousePos, const Vector2& mouseDelta) override;
        void afterWinDraw() override;
        void drawRaw() override;

        // Point/Rect related stuff
        bool managePoints(const Vector2& mousePos, const Vector2& mouseDelta);
        void updateRecMats(const Vector2& texSize);
    };

    class ImageTargetViewer : public ImageRipChildWin {
    public:
        RenderTexture2D targetTex;
        Vector2 targetDim = { 0,0 };
        std::string lastSavePath = "";

        struct Settings {
            ImVec4 mainBGColor = { 0,0,0,0 };
            ImVec4 imgBGColor = { 0,0,0,0 };
            float padding = 10;
            int layOutMode = LayoutMode_Line;
            float layOutMaxWidth = 1000;
        };
        Settings settings;
        bool settingsWinOpen = false;
        std::string settingsTitle;

        bool needsReRender = false;

        ImageTargetViewer(const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
        ~ImageTargetViewer(); 

        void rerenderTargetTex(const Texture2D& srcTex, const Texture2D& mats, utils::Map<size_t, ImgRec*>& recs);
        void save();
        void saveAs();
        bool editedSinceSaved();

        void drawMenuBarFile();
        void drawMenuBarSettings();

        void setShowEdited(bool show);
    protected:
        bool showEdited = false;

        void drawMenuBar() override;
        void beforeWinDraw() override;
        void afterWinDraw() override;
        void drawOverlay(const Vector2& mousePos, const Vector2& mouseDelta) override;
        void drawSettings();

        void openSettingsWin();

        enum LayoutMode_ {
            LayoutMode_Line = 0,
            LayoutMode_LineWrap,
            LayoutMode_Grid
        };
        Vector2 layoutRecs(utils::Map<size_t, ImgRec*>* recs);
        Vector2 layoutRecsLine(utils::Map<size_t, ImgRec*>* recs);
        Vector2 layoutRecsLineWrap(utils::Map<size_t, ImgRec*>* recs);
        Vector2 layoutRecsGrid(utils::Map<size_t, ImgRec*>* recs);

        bool saveTex(const char* path);
        bool editedSinceSavedB = true;
    };

    class ImageRipperWindow {
    public:
        ImageSelectionViewer selWin;

        ImageTargetViewer texWin;

        std::string name;

        ImageRipperWindow(const Texture2D& tex, const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);

        void loadTex(const char* str);
        void setTex(Texture2D newTex);
        void draw(const Vector2& mousePos, const Vector2& mouseDelta);

        void dockedWinView();
        void oneWinView();
        void floatWinView();

        bool toDelete = false;
        
        bool isWinOpen();
        void tryClose();
        void close();
        bool wantsToClose() const;

        ImGuiID externalDockSpaceID;
    protected:
        bool winOpen = true;
        bool parentWinOpen = true;
        ImGuiWindowFlags parentWinFlags = ImGuiWindowFlags_MenuBar;
        ImGuiID parentDockSpaceID;

        

        void update();

        void drawParentWin();

        void manageTexRipping();
        void reRenderOutput();
        void dontYouWantToSave();
    };

    class TexRipper {
    public:
        static struct Settings{
            bool TargetViewerShowLayoutLines = true;
        } settings;
    protected:
        static Vector2 lastMousePos;
        static Vector2 mouseDelta;

        static std::vector<ImageRipperWindow*> wins;

        struct DroppedFile {
            std::string name;
            std::string ext;
            std::string path;
        };
        static std::vector<DroppedFile> droppedFileNames;

        static bool settingsWinOpen;
        static size_t currentSettingCategory;
        enum SettingsCategories_ {
            SettingsCategories_General = 0,
            SettingsCategories_Key_Bindings,
            SettingsCategories_COUNT
        };
        static std::string settingsCategories[];

        static void openFileName(const char* name);
        static void drawDroppedFilesMenu();

        class WinViewManager {
        private:
            
            typedef int WinViewMode;

            static size_t activeWinID;
            static bool winOpen;

            static int winViewMode;

            static bool changed;
        public:
            enum WinViewModes {
                DOCKED = 0,
                ONE,
                FLOAT
            };

            static void updateBefore();
            static void updateWinView();

            static void drawWin();

            static void queueViewMode(WinViewMode mode);
            static void reQueueViewMode();
            static size_t getActiveWinID();
        };
        static ImageRipperWindow* getActiveWin();

        static void drawMainMenuBar();
        static void drawSettingsWindow();
        static void drawSettingsGeneral();
    public:
        static void init();
        static void destroy();

        static void draw();
        static void addImage(const char* name, const Texture2D& tex);

        static void debugDraw();
    };
}

#endif
#ifndef _CV_IMAGE_VIEWER
#define _CV_IMAGE_VIEWER

#include "raylib.h"
#include <vector>
//#include "opencv2/core.hpp"
#include "ImGuiImageViewers.h"
#include "utils/overrideStack.h"

namespace TexRip {
    struct ImgPoint {
        Vector2 pos;
        bool selected;
        bool candidate;
    };
    class ImgRec {
    public:
        enum PROGRESS {
            Prog_0 = 0,
            Prog_1 = 1,
            Prog_2 = 2,
            Prog_3 = 3,
            Prog_FULL = 4
        };

        ImgPoint pnts[4];
        Vector2 lastRipPos[4];
        float persp[9];
        float inv_persp[9];
        Vector2 corrDim;
        uint8_t progress = Prog_FULL;

        ImgRec();
        ImgRec(const Vector2& pos1, const Vector2& pos2, const Vector2& pos3, const Vector2& pos4, const Vector2& imgDim);
        

        void calcPersp(const Vector2& imgDim);

        Rectangle boundingBox() const;
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
            
        private:
            typedef int RectPntID;

            class PersMat {
            protected:
                Image img;
            public:
                Texture tex;

                PersMat();
                ~PersMat();

                void updateWithRecs(const std::vector<ImgRec>& recs, bool inverse);
            };

            bool worldMayMove = true;

            PersMat matsTex;
            PersMat invMatsTex;

            bool pointsWereMoved = false;

            std::vector<ImgRec> recs;
            OverrideStack<std::vector<ImgRec>> undoBuffer;

            uint8_t mode = MODES::NONE;
            bool needsMatUpdate = true;

            Vector2 moveOff = { 0,0 };

            float rotOff = 0;
            Vector2 rotCenter = { 0,0 };

            float lastSclAmt = 1;
            Vector2 sclCenter = { 0,0 };
            //Vector2 globalSclCenter = sclCenter;
            float sclMouseStartDist = 1;

            RectPntID constrID = -1;

        public:
            RectManager();
            
            bool managePoints(const Vector2& mousePosTexRel, const Vector2& mouseDeltaTexRel, const float zoomF, const bool winIsHovered, const Vector2& texSize); // returns true if reRender is needed

            const std::vector<ImgRec>& getRecs();
            void addRect(const ImgRec& rect);
            uint8_t getMode() const;
            bool werePointsMoved() const;
            bool mayWorldMove() const;

            const Vector2& getRotCenter() const;
            const Vector2& getSclCenter() const;

            const Texture2D& getMatTex() const;
            const Texture2D& getInvMatTex() const;

            void updateRecMats(const Vector2& texSize);

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

            RectPntID getSelPointInd(const Vector2& mousePosTexRel, const float zoomF, bool deselect);
            Vector2 getMeanSelPntPos(int& numRef);

            const size_t getRecIndFromID(RectPntID id);
            ImgRec& getRecFromID(RectPntID id);
            ImgPoint& getRecPntFromID(RectPntID id);

            void addUndoState();
            bool undo(); // returns true if undo was successful
        };

        RectManager rectManager;

        bool changedImage = false;

    public:
        ImageSelectionViewer(const Texture2D& tex, const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
        ~ImageSelectionViewer();

        const std::vector<ImgRec>& getRecs();

        void openFile();

        bool wasImageChanged();
        bool werePointsMoved() const;

        const Texture& getInvMats() const;

        void drawMenuBarFile();
    protected:
        void drawMenuBar() override;
        bool ownUpdate(const Vector2& mousePos, const Vector2& mouseDelta) override;
        void drawOverlay(const Vector2& mousePos, const Vector2& mouseDelta) override;
        void afterWinDraw() override;

        // Point/Rect related stuff
        void drawRec(const ImgRec& rec, Color lineC, Color lineCSel, Color pntC, Color pntCSel);

        bool managePoints(const Vector2& mousePos, const Vector2& mouseDelta);
        void updateRecMats(const Vector2& texSize);

        // other stuff
        Vector2 getPosPersp(const Vector2& pos, const float* mat);
        Vector2 getPosPersp2(const Vector2& pos, const float* mat);
    };

    class ImageTargetViewer : public ImageRipChildWin {
    public:
        RenderTexture2D targetTex;
        std::string lastSavePath = "";

        struct Settings {
            ImVec4 mainBGColor = { 0,0,0,0 };
            ImVec4 imgBGColor = { 0,0,0,0 };
        };
        Settings settings;
        bool settingsWinOpen = false;
        std::string settingsTitle;

        bool needsReRender = false;

        ImageTargetViewer(const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
        ~ImageTargetViewer(); 

        void rerenderTargetTex(const Texture2D& srcTex, const Texture2D& mats, const std::vector<ImgRec>& recs);
        bool save();
        bool saveAs();
        bool editedSinceSaved();

        void drawMenuBarFile();
        void drawMenuBarSettings();
    protected:
        void drawMenuBar() override;
        void afterWinDraw() override;
        void drawSettings();

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
    public:
        static void init();
        static void destroy();

        static void draw();
        static void addImage(const char* name, const Texture2D& tex);
    };
}

#endif
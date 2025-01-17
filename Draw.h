#pragma once

#include <string>
#include <TTree.h>
#include <TTreeReader.h>
#include <qwidget.h>

#include "TCanvasWidget.h"

class Draw : public QObject
{
    Q_OBJECT
    private:
        #define nGraph 1000
        std::string fileName = "";
        std::string fileNameWoSuffix = "";
        std::string treeName = "tData";

        TFile* dFile = nullptr;
        TTree* dTree = nullptr;
        TTreeReader* tTreeReader = nullptr;
        TCanvasWidget* canvasWidget = nullptr;
        TCanvas* canvas = nullptr;
        const uint16_t cADCdata = 1024;
        const uint8_t cMaxNumChannel = 128;

        int wChannel = 0;
        std::string cFigName = "";
        std::uint64_t nEntries = 0;
        std::uint64_t currentEntry = 0;
        std::uint64_t nEvents = 0;
        /* ========================================================================== */
        /* ****************************** Draw Variable****************************** */
        /* -------------------------------------------------------------------------- */
        TTree* tTreeSigma = nullptr;
        std::unordered_map<uint8_t, double[3]> umMeanAndSigmaOfBase;
        std::unordered_map<int, std::vector<std::vector<uint16_t>>> umChannelWaves;
        std::unordered_map<int, std::vector<uint16_t>> umChannelPeak2PeakOfSignal;
        std::unordered_map<int, std::vector<uint16_t>> umChannelPeakOfSignal;
        std::unordered_map<int, std::vector<uint16_t>> umChannelMeanBaseline;

        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>> umEventChannelWaves;
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>> umEventChannelXWaves;
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>> umEventChannelYWaves;
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>> umEventChannelAWaves;

        typedef std::pair<uint16_t, uint16_t> IntPair;
        std::unordered_map<uint32_t, std::map<std::string, IntPair>> umEventChannelXIndexOfPeak;
        std::unordered_map<uint32_t, std::map<std::string, IntPair>> umEventChannelYIndexOfPeak;

        typedef std::pair<std::pair<std::string, uint16_t>, std::pair<std::string, uint16_t>> StringIntPair;
        std::unordered_map<uint32_t, StringIntPair> umEventMap;

        std::map<uint32_t, uint32_t> mEventEnergy;

        uint8_t pChannelIndex = 0;
        uint16_t pMeanOfBaseline = 0;
        double pSigmaOfBaseline = .0;
        std::vector<uint16_t> vADCdata = {};
        uint16_t pPeakOfWave = 0;
        uint16_t pPeak2PeakOfWave = 0;
        uint32_t pEventIndex = 0;
        //uint16_t pIndexOfPeak = 0;
        uint16_t pIndexOfHalfMax = 0;
        /* -------------------------------------------------------------------------- */
        /* ****************************** Draw Variable****************************** */
        /* ========================================================================== */
        /* ========================================================================== */
        /* ******************************  Channel Map ****************************** */
        /* -------------------------------------------------------------------------- */
        std::unordered_map<uint8_t, std::string> umChannelMap = {
            {0, "Y55"}, {1, "X55"}, {2, "Y53"}, {3, "X53"}, {4, "Y57"}, {5, "X57"}, {6, "Y59"}, {7, "X59"},
            {8, "Y47"}, {9, "X47"}, {10, "Y45"}, {11, "X45"}, {12, "Y49"}, {13, "X49"}, {14, "Y51"}, {15, "X51"},
            {16, "Y39"}, {17, "X39"}, {18, "Y37"}, {19, "X37"}, {20, "Y41"}, {21, "X41"}, {22, "Y43"}, {23, "X43"},
            {24, "Y31"}, {25, "X31"}, {26, "Y29"}, {27, "X29"}, {28, "Y33"}, {29, "X33"}, {30, "Y35"}, {31, "X35"},
            {32, "Y21"}, {33, "X21"}, {34, "Y23"}, {35, "X23"}, {36, "Y27"}, {37, "X27"}, {38, "Y25"}, {39, "X25"},
            {40, "Y19"}, {41, "X19"}, {42, "Y09"}, {43, "X09"}, {44, "Y17"}, {45, "X17"}, {46, "Y15"}, {47, "X15"},
            {48, "Y05"}, {49, "X05"}, {50, "Y07"}, {51, "X07"}, {52, "Y13"}, {53, "X13"}, {54, "Y11"}, {55, "X11"},
            {56, "Y01"}, {57, "X01"}, {58, "Y03"}, {59, "X03"}, {60, "A2"}, {61, "A0"}, {62, "A3"}, {63, "A1"},
            {64, "Y56"}, {65, "X56"}, {66, "Y52"}, {67, "X58"}, {68, "Y58"}, {69, "Y54"}, {70, "A4"}, {71, "A6"},
            {72, "A7"}, {73, "Y48"}, {74, "X48"}, {75, "A5"}, {76, "X52"}, {77, "X50"}, {78, "Y50"}, {79, "X54"},
            {80, "Y38"}, {81, "X38"}, {82, "Y40"}, {83, "X44"}, {84, "Y44"}, {85, "Y42"}, {86, "X46"}, {87, "Y46"},
            {88, "Y32"}, {89, "Y34"}, {90, "X34"}, {91, "X32"}, {92, "X40"}, {93, "X36"}, {94, "Y36"}, {95, "X42"},
            {96, "Y26"}, {97, "X26"}, {98, "Y22"}, {99, "X28"}, {100, "Y28"}, {101, "Y24"}, {102, "X30"}, {103, "Y30"},
            {104, "Y20"}, {105, "Y16"}, {106, "X16"}, {107, "X20"}, {108, "X22"}, {109, "X18"}, {110, "Y18"}, {111, "X24"},
            {112, "Y06"}, {113, "X06"}, {114, "X08"}, {115, "Y12"}, {116, "X12"}, {117, "Y10"}, {118, "X14"}, {119, "Y14"},
            {120, "Y00"}, {121, "X02"}, {122, "Y02"}, {123, "X00"}, {124, "Y08"}, {125, "X04"}, {126, "Y04"}, {127, "X10"}
        };
        /* -------------------------------------------------------------------------- */
        /* ******************************  Channel Map ****************************** */
        /* ========================================================================== */
        double Progress = 0.0;
        void PrintProgress();
    public:
        Draw(TCanvasWidget* tCanvasWidget, std::string sFileName);
        ~Draw();

        void drawSeveralWaves(int nBeg, int nFig, const char* figName);
        void drawSigmaOfBaseline(const char* figName);
        void drawChannelWave(int channel, int nBeg, int nFig, const char* figName);
        void drawEventWaves(int eventID, const char* figName);
        void drawTrack(int eventID, const char* figName);
        void drawMap(const char* figName);
        void drawSpectrum(const char* figName);
        void saveToCSV(const std::string& fileName, const std::vector<std::vector<int>>& data);
        void printCanvas();
        void printCanvas(const char* figName);
        void drawText();
        void setChannelWho(int wChannel) { this->wChannel = wChannel; };
        static void drawTest(TCanvasWidget* tCanvasWidget);
    
    signals:
        void progressChanged(double progress);
};

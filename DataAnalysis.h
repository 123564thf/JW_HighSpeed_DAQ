#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "qobject.h"

#include "TFile.h"
#include "TTree.h"

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

struct ConfigData
{
    uint16_t uTrigRiseStep;
    uint16_t uNSigmaCompres;
    uint8_t uNHitChannel;
    uint16_t uTrigDelayCycle;
    uint16_t uHitWidthCycle;
    uint32_t *puIIRFilter;
    uint8_t uIIRFilterNum;
    uint32_t *puBaseline;
    uint8_t uBaselineNum;
};

class DataAnalysis : public QObject
{   
    Q_OBJECT
    public:
        DataAnalysis();
        ~DataAnalysis();

        uint32_t nOfHeader = 0;
        uint32_t nOfBody = 0;
        uint32_t nOfTail = 0;
        uint32_t nOfFullPacket = 0;

        void SetDataFileName(const std::string& fileName);
        void GetFileHandle();
        void ReadRootFile();
        void Dat2Root();
        void findClosestToHalfMax(const std::vector<uint16_t>& vADCdata, uint16_t& indexOfPeak, uint16_t& indexOfHalfMax);
        void ExtractBaseline();
        void WriteConfigFile(std::string fileName, ConfigData configData);

        static void WriteFilterFile(const char* filterFileName, int* IIRFilter, int IIRFilterNum, int* Baseline, int BaselineByteNum);
        static void CalIIRFilterCRRParameter(double parameters[3], long double Ts, long double R1, long double R2, long double C1);
        static void CalIIRFilterCRParameter(double parameters[3], long double Ts, long double Magnification, long double R, long double C);
        static void CalIIRFilterRCParameter(double parameters[3], long double Ts, long double Magnification, long double R, long double C);

        static void Decimal2binary(double decimal, int precision, int& binary);
        static void Decimal2binary(double decimal, int intPrecision, int decPrecision, int& binary);

        void SetDataFormat(int dataFormat);
        void SetTestMode(bool testMode);

    private:
        std::string fileName;
        std::string fileNameWoSuffix;
        const uint8_t PACKETHEADER = 0x5a;
        const uint8_t SOE = 0x40;
        const uint8_t EOE = 0x20;
        const uint8_t PHEADER = 0x40;
        const uint8_t PWVBODY = 0x08;
        const uint8_t PTQBODY = 0x00;
        const uint8_t PTAIL = 0x20;
        const uint8_t HIGH3BIT = 0xE0;
        const uint8_t FIFTHSIXTH = 0x60;
        const uint16_t LOW13Bits = 0x1FFF;
        const uint8_t LOW6Bits = 0x3F;
        const uint8_t LOW7Bits = 0x7F;
        const uint16_t LOW12Bits = 0xFFF;

        const uint8_t bwHeaderPacketSize = 13;
        const uint8_t bwFEID = 6;
        const uint8_t bwTimeStamp = 48;
        const uint8_t bwEventID = 32;
        const uint8_t bwHitChannelNum = 6;
        const uint8_t bwHeaderCRC32 = 32;
        const uint8_t bwBodyPacketSize = 13;
        const uint8_t bwChannelIndex = 7;
        const uint8_t bwADCdata = 12;

        const uint16_t cADCdata = 1024;
        const size_t cMaxNumChannel = 128;

        const uint8_t bwBodyCRC32 = 32;
        //const unsigned short cADCdata = 1024;
        const uint8_t bwTailPakcetSize = 13;
        const uint8_t bwEventSize = 32;
        const uint8_t bwTailCRC32 = 32;
        
        const uint8_t BwHeaderPacketSize = (bwHeaderPacketSize + 7) / 8;
        const uint8_t BwFEID = (bwFEID + 7) / 8;
        const uint8_t BwTimeStamp = (bwTimeStamp + 7) / 8;
        const uint8_t BwEventID = (bwEventID + 7) / 8;
        const uint8_t BwHitChannelNum = (bwHitChannelNum + 7) / 8;
        const uint8_t BwHeaderCRC32 = (bwHeaderCRC32 + 7) / 8;
        const uint8_t BwBodyPacketSize = (bwBodyPacketSize + 7) / 8;
        const uint8_t BwChannelIndex = (bwChannelIndex + 7) / 8;
        const uint8_t BwADCdata = (bwADCdata + 7) / 8;
        const uint8_t BwBodyCRC32 = (bwBodyCRC32 + 7) / 8;
        const uint8_t BwTailPacketSize = (bwTailPakcetSize + 7) / 8;
        const uint8_t BwEventSize = (bwEventSize + 7) / 8;
        const uint8_t BwTailCRC32 = (bwTailCRC32 + 7) / 8;
        const uint8_t BwPeakOfWave = (bwADCdata + 7) / 8;
        const uint8_t BwTimingPosition = (bwADCdata + 7) / 8;

        std::ifstream ifs;
        std::streamsize fileSize = 0;

        std::vector<uint8_t> buffer;
        uint64_t bufferIndex = 0;

        TFile* tFile = nullptr;
        TTree* dataTree = nullptr;
        //TTree* tPHeader;
        //TTree* tPBody;
        //TTree* tPTail;

        uint8_t pDataFormat = 0;
        uint8_t pTestMode = 0;
        uint16_t pHeaderPacketSize = 0;
        uint8_t pFEID = 0;
        uint64_t pTimeStamp = 0;
        uint32_t pEventID = 0;
        uint32_t pEventIndex = 0;
        uint8_t pHitChannelNum = 0;
        uint32_t pHeaderCRC32 = 0;

        uint16_t pBodyPacketSize = 0;
        uint8_t pChannelIndex = 0;
        uint16_t pADCdata = 0;
        std::vector<uint16_t> pvADCdata;


        uint32_t pBodyCRC32 = 0;

        uint16_t pTailPacketSize = 0;
        uint32_t pEventSize = 0;
        uint32_t pTailCRC32 = 0;

        uint16_t pMeanOfBaseline = 0;
        double pSigmaOfBaseline = .0;
        uint16_t pRMSOfBaseline = 0;
        uint16_t pPeakOfWave = 0;
        uint16_t pPeak2PeakOfWave = 0;
        uint64_t pChargeOfEvent = 0;
        uint16_t pIndexOfPeak = 0;
        uint16_t pIndexOfHalfMax = 0;

        uint64_t nOfValidBody = 0;
        std::vector<uint64_t> vChargeOfEvent;
        //std::vector<uint16_t> vBaseOfChannel;
        std::unordered_map<uint16_t, std::array<double, 3>> umBaseOfChannel ;
        std::unordered_map<uint16_t, uint32_t> umThresholdOfChannel;
        //std::unordered_map<uint16_t, uint64_t> umBaseOfChannel;
        double Progress = 0.0;
        std::thread *tPrintProgress = nullptr;
        void PrintProgress();
    
    signals:
        void progressChanged(double progress);
};
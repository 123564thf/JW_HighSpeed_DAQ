#include "DataAnalysis.h"
#include <processthreadsapi.h>
#include "Welford.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <numeric>
#include <filesystem>
#include <thread>

#include "TTreeReader.h"

DataAnalysis::DataAnalysis()
    : nOfHeader(0)
    , nOfBody(0)
    , nOfTail(0)
    , nOfFullPacket(0)
    , tFile(nullptr)
    , dataTree(nullptr)
    , Progress(0.0)
{
    std::cout << "DataAnalysis constructor" << std::endl;
    tPrintProgress = new std::thread(&DataAnalysis::PrintProgress, this);
    tPrintProgress->detach();

    fileName = "";
    fileNameWoSuffix = "";
}

DataAnalysis::~DataAnalysis()
{
    TerminateThread(tPrintProgress->native_handle(), 0);
    delete tPrintProgress;
    tPrintProgress = nullptr;
    //delete dataTree;
    tFile->Close();
    delete tFile;
    dataTree = nullptr;
    tFile = nullptr;
}

void DataAnalysis::PrintProgress()
{
    while (true)
    {
        if (Progress != 0.0)
        {
            std::cout << std::fixed << std::setprecision(2) << "\rProgress: %" << Progress << "% " << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
}

void DataAnalysis::SetDataFileName(const std::string& fileName)
{
    this->fileName = fileName;
    fileNameWoSuffix = fileName.substr(0, fileName.find_last_of("."));
    std::cout << "Data file name: " << fileName << std::endl;
}

void DataAnalysis::GetFileHandle()
{
    std::cout << "File: " << fileName << " reading..." << std::endl;
    ifs.open(fileName, std::ios::binary | std::ios::ate);
    if (!ifs.is_open())
    {
        std::cerr << "Error: File open failed." << std::endl;
        return;
    }
    fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::cout << "File size: " << fileSize << " bytes" << std::endl;
    buffer.resize(fileSize);

    ifs.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    pvADCdata.resize(cADCdata);

    std::cout << "File: " << fileName << " read." << std::endl;
}

void DataAnalysis::ReadRootFile()
{
    std::string rootFileName = fileNameWoSuffix + ".root";
    std::cout << "File: " << rootFileName << " reading..." << std::endl;
    if (tFile != nullptr)
    {
        tFile->Close();
        delete tFile;
        tFile = nullptr;
    }
    tFile = new TFile(rootFileName.c_str(), "read");
    dataTree = (TTree*)tFile->Get("tData");
    if (dataTree == nullptr)
    {
        std::cerr << "Error: tData tree not found." << std::endl;
        return;
    }
    else 
    {
        std::cout << "DataTree entries: " << dataTree->GetEntries() << std::endl;
    }
    std::cout << "File: " << rootFileName << " read." << std::endl;
    Progress = 100.0;
    emit progressChanged(Progress);
    Progress = 0.0;
}

void DataAnalysis::Dat2Root()
{
    std::cout << "Converting data file to root file..." << std::endl;
    std::string rootFileName = fileNameWoSuffix + ".root";
    if (tFile != nullptr)
    {
        tFile->Close();
        delete tFile;
        tFile = nullptr;
    }
    tFile = new TFile(rootFileName.c_str(), "recreate");
    dataTree = new TTree("tData", "tData");

    dataTree->Branch("pHeaderPacketSize", &pHeaderPacketSize);
    dataTree->Branch("pFEID", &pFEID);
    dataTree->Branch("pTimeStamp", &pTimeStamp);
    dataTree->Branch("pEventID", &pEventID);
    dataTree->Branch("pEventIndex", &pEventIndex);
    dataTree->Branch("pHitChannelNum", &pHitChannelNum);
    dataTree->Branch("pHeaderCRC32", &pHeaderCRC32);

    dataTree->Branch("pBodyPacketSize", &pBodyPacketSize);
    dataTree->Branch("pChannelIndex", &pChannelIndex);
    dataTree->Branch("pvADCdata", &pvADCdata);
    dataTree->Branch("pBodyCRC32", &pBodyCRC32);

    dataTree->Branch("pTailPacketSize", &pTailPacketSize);
    dataTree->Branch("pEventSize", &pEventSize);
    dataTree->Branch("pTailCRC32", &pTailCRC32);

    dataTree->Branch("pMeanOfBaseline", &pMeanOfBaseline);
    dataTree->Branch("pSigmaOfBaseline", &pSigmaOfBaseline);
    dataTree->Branch("pPeakOfWave", &pPeakOfWave);
    dataTree->Branch("pPeak2PeakOfWave", &pPeak2PeakOfWave);
    dataTree->Branch("pChargeOfEvent", &pChargeOfEvent);
    dataTree->Branch("pIndexOfPeak", &pIndexOfPeak);
    dataTree->Branch("pIndexOfHalfMax", &pIndexOfHalfMax);

    //dataTree->Branch("pLastTailIndex", &pLastTailIndex);
    int debugNum = 0;

    bufferIndex = 0;
    while (bufferIndex < fileSize)
    {
        uint8_t currentByte = buffer[bufferIndex++];
        
        if (currentByte == PACKETHEADER)
        {
            uint8_t nextByte = buffer[bufferIndex];

            //if ((nextByte & HIGH3BIT) == SOE)
            if (nextByte == PHEADER)
            {
                //std::cout << "Number of Header: " << nOfHeader << std::endl;
                pHeaderPacketSize = 0;
                if (bufferIndex + BwHeaderPacketSize >= fileSize) break;
                std::memcpy(&pHeaderPacketSize, &buffer[bufferIndex], BwHeaderPacketSize);
                pHeaderPacketSize = ntohs(pHeaderPacketSize);  // Convert to host byte order
                //packetSizeAndFeID = ntohs(packetSizeAndFeID);  // Convert to host byte order
                pHeaderPacketSize &= LOW13Bits;
                if (buffer[bufferIndex+pHeaderPacketSize-1] != PACKETHEADER) continue;
                //numOfHeader++;
                nOfHeader++;
                //vHeaderIndex.push_back(bufferIndex);
                //uint16_t packetSizeAndFeID;
                //std::memcpy(&packetSizeAndFeID, &buffer[bufferIndex], sizeof(packetSizeAndFeID));
                bufferIndex += BwHeaderPacketSize;
                //std::cout << "Header Packet Size: " << pHeaderPacketSize << std::endl;

                pFEID = 0;
                if (bufferIndex + BwFEID >= fileSize) break;
                std::memcpy(&pFEID, &buffer[bufferIndex], BwFEID);
                pFEID &= LOW6Bits;
                bufferIndex += BwFEID;

                pTimeStamp = 0;
                if (bufferIndex + BwTimeStamp >= fileSize) break;
                std::memcpy(&pTimeStamp, &buffer[bufferIndex], BwTimeStamp);
                //std::cout << "TimeStamp: " << pTimeStamp << std::endl;
                // Convert to host byte order
                uint32_t low4Bytes = (pTimeStamp & 0xFFFFFFFF0000) >> 16;
                uint32_t high2Bytes = (pTimeStamp & 0x00000000FFFF);
                low4Bytes = ntohl(low4Bytes);
                high2Bytes = ntohl(high2Bytes);
                pTimeStamp = low4Bytes + ((uint64_t)high2Bytes << 16);
                bufferIndex += BwTimeStamp;
                //std::cout << "TimeStamp: " << pTimeStamp << std::endl;
                //if (nOfHeader == 3)
                //{
                //    exit(0);
                //}

                pEventID = 0;
                if (bufferIndex + BwEventID >= fileSize) break;
                std::memcpy(&pEventID, &buffer[bufferIndex], BwEventID);
                pEventID = ntohl(pEventID); // Convert to host byte order
                bufferIndex += BwEventID;
                //std::cout << "Event ID: " << pEventID << std::endl;

                pHitChannelNum = 0;
                if (bufferIndex + BwHitChannelNum >= fileSize) break;
                std::memcpy(&pHitChannelNum, &buffer[bufferIndex], BwHitChannelNum);
                pHitChannelNum &= LOW6Bits;
                bufferIndex += BwHitChannelNum;
                //std::cout << "Hit Channel Number: " << (uint16_t)pHitChannelNum << std::endl;

                bufferIndex += 1; // reserved 1 byte

                pHeaderCRC32 = 0;
                if (bufferIndex + BwHeaderCRC32 >= fileSize) break;
                std::memcpy(&pHeaderCRC32, &buffer[bufferIndex], BwHeaderCRC32);
                pHeaderCRC32 = ntohl(pHeaderCRC32); // Convert to host byte order
                bufferIndex += BwHeaderCRC32;

                pChargeOfEvent = 0;

                //dataTree->Fill();
            }
            //else if ((nextByte & HIGH3BIT) == 0 && nOfHeader > nOfTail)
            else if (((nextByte == PWVBODY) || (nextByte == PTQBODY)) && nOfHeader > nOfTail)
            {
                //numOfWaveform++;
                pBodyPacketSize = 0;
                if (bufferIndex + BwBodyPacketSize >= fileSize) break;
                std::memcpy(&pBodyPacketSize, &buffer[bufferIndex], BwBodyPacketSize);
                pBodyPacketSize = ntohs(pBodyPacketSize);  // Convert to host byte order
                pBodyPacketSize &= LOW13Bits;
                //if (buffer[bufferIndex+pBodyPacketSize-1] != PACKETHEADER) continue;
                if (buffer[bufferIndex+pBodyPacketSize-1] != PACKETHEADER || pBodyPacketSize == 0) continue;
                nOfBody++;
                //vBodyIndex.push_back(bufferIndex);
                bufferIndex += BwBodyPacketSize;
                //std::cout << "Body Packet Size: " << pBodyPacketSize << std::endl;

                bufferIndex += 1; // reserved 1 byte FE_ID

                pChannelIndex = 0;
                if (bufferIndex + BwChannelIndex >= fileSize) break;
                std::memcpy(&pChannelIndex, &buffer[bufferIndex], BwChannelIndex);
                pChannelIndex &= LOW7Bits;
                bufferIndex += BwChannelIndex;
                //std::cout << "ChannelIndex: " << (int)pChannelIndex << std::endl;
                //if (pBodyPacketSize != 14)
                    //std::cout << "Channel Index: " << (int)pChannelIndex << std::endl;

                bufferIndex += 1; // reserved 1 byte

                if (pBodyPacketSize == 14)
                {
                    if (bufferIndex + BwPeakOfWave >= fileSize) break;
                    std::memcpy(&pPeakOfWave, &buffer[bufferIndex], BwPeakOfWave);
                    pPeakOfWave = ntohs(pPeakOfWave);  // Convert to host byte order
                    pPeakOfWave &= LOW12Bits;
                    bufferIndex += BwPeakOfWave;
                    //std::cout << "pPeakOfWave: " << pPeakOfWave << std::endl;
                    if (bufferIndex + BwTimingPosition >= fileSize) break;
                    std::memcpy(&pIndexOfHalfMax, &buffer[bufferIndex], BwTimingPosition);
                    pIndexOfHalfMax = ntohs(pIndexOfHalfMax);  // Convert to host byte order
                    pIndexOfHalfMax &= LOW12Bits;
                    bufferIndex += BwTimingPosition;
                    //std::cout << "pIndexOfHalfMax: " << pIndexOfHalfMax << std::endl;
                }
                else if (pBodyPacketSize == 2060)
                {
                    Welford welford = Welford();
                    for (int i = 0; i < cADCdata; i++)
                    {
                        pADCdata = 0;
                        if (bufferIndex + BwADCdata >= fileSize) break;
                        std::memcpy(&pADCdata, &buffer[bufferIndex], BwADCdata);
                        pADCdata = ntohs(pADCdata);  // Convert to host byte order
                        pADCdata &= LOW12Bits;
                        bufferIndex += BwADCdata;
                        pvADCdata[i] = pADCdata;
                        welford.addDataPoint(pvADCdata[i]);
                        //std::cout << "pADCdata: " << i << ": " << pvADCdata[i] << std::endl;
                        //if (i == 1023) exit(0);
                    }
                    //#if TESTMODE == 0
                    if (pTestMode == 0)
                    {
                        pMeanOfBaseline = welford.mean();
                        //pMeanOfBaseline = (double)std::accumulate(pvADCdata.begin(), pvADCdata.end(), 0) / cADCdata;
                        pSigmaOfBaseline = welford.stddev();
                        //pSigmaOfBaseline = welford.stddev();
                    }
                    //#else
                    else
                    {
                        uint16_t pMeanOfStartBase = std::round((double)std::accumulate(pvADCdata.begin(), pvADCdata.begin()+25, 0) / 25);
                        uint16_t pMeanOfEndBase = std::round((double)std::accumulate(pvADCdata.end()-25, pvADCdata.end(), 0) / 25);
                        pMeanOfBaseline = (((pMeanOfStartBase) < (pMeanOfEndBase)) ? (pMeanOfStartBase)
                                         : (pMeanOfEndBase)); // Add missing semicolon
                        pSigmaOfBaseline = welford.stddev();
                    }
                    //#endif
                    //std::cout << "pMeanOfBaseline: " << pMeanOfBaseline << std::endl;
                    auto iterMaxElement = std::max_element(pvADCdata.begin(), pvADCdata.end());
                    pPeakOfWave = *iterMaxElement - pMeanOfBaseline;
                    pPeak2PeakOfWave = *iterMaxElement - *std::min_element(pvADCdata.begin(), pvADCdata.end());
                    //std::cout << "pPeakOfWave: " << pPeakOfWave << std::endl;
                    //pIndexOfPeak = std::distance(pvADCdata.begin(), iterMaxElement);
                    findClosestToHalfMax(pvADCdata, pIndexOfPeak, pIndexOfHalfMax);
                    //std::cout << "pChargeOfEvent: " << pChargeOfEvent << std::endl;

                    bufferIndex += 2; // reserved 2 byte
                }

                pChargeOfEvent += pPeakOfWave;
                pBodyCRC32 = 0;
                if (bufferIndex + BwBodyCRC32 >= fileSize) break;
                std::memcpy(&pBodyCRC32, &buffer[bufferIndex], BwBodyCRC32);
                pBodyCRC32 = ntohl(pBodyCRC32); // Convert to host byte order
                bufferIndex += BwBodyCRC32;

                dataTree->Fill();
            }
            else if ((nextByte == PTAIL) && (nOfHeader > nOfTail))
            //else if (((nextByte & HIGH3BIT) == EOE) && (nOfHeader > nOfTail))
            //else if (nextByte == EOE)
            {
                pEventIndex += 1;
                //std::cout << "Event Index: " << pEventIndex << std::endl;
                pTailPacketSize = 0;
                if (bufferIndex + BwTailPacketSize >= fileSize) break;
                std::memcpy(&pTailPacketSize, &buffer[bufferIndex], BwTailPacketSize);
                pTailPacketSize = ntohs(pTailPacketSize);
                //packetSizeAndFeID = ntohs(packetSizeAndFeID);  // Convert to host byte order
                pTailPacketSize &= LOW13Bits;
                if (buffer[bufferIndex+pTailPacketSize-1] != PACKETHEADER) continue;

                nOfTail++;
                nOfFullPacket++;
                nOfValidBody = nOfBody;
                //vTailIndex.push_back(bufferIndex);

                bufferIndex += BwTailPacketSize;
                //std::cout << "Tail Packet Size: " << pTailPacketSize << std::endl;

                bufferIndex += 1; // reserved 1 byte FE_ID

                pEventSize = 0;
                if (bufferIndex + BwEventSize >= fileSize) break;
                std::memcpy(&pEventSize, &buffer[bufferIndex], BwEventSize);
                pEventSize = ntohl(pEventSize);
                bufferIndex += BwEventSize;
                //std::cout << "Event Size: " << pEventSize << std::endl;

                pTailCRC32 = 0;
                if (bufferIndex + BwTailCRC32 >= fileSize) break;
                std::memcpy(&pTailCRC32, &buffer[bufferIndex], BwTailCRC32);
                pTailCRC32 = ntohl(pTailCRC32);
                bufferIndex += BwTailCRC32;

                vChargeOfEvent.push_back(pChargeOfEvent);
                //std::cout << "pChargeOfEvent: " << pChargeOfEvent << std::endl;

                //dataTree->Fill();
            }
            else 
            {
                std::cout << "Garbled byte!" << std::endl;
            }
        }
        else
        {
            if (bufferIndex >= fileSize) break;
            //std::cout << "Not started with PACKETHEADER" << std::endl;
        }
        //std::cout << "\rConverting Progress: " << (double)bufferIndex/fileSize*100 << "% " << std::flush;
        Progress = (double)bufferIndex/fileSize*100;
        emit progressChanged(Progress);
    }
    Progress = 100.0;
    emit progressChanged(Progress);
    std::cout << "DebugNum: " << debugNum << std::endl;
    Progress = 0.0;
    std::cout << std::endl;
    std::cout << "Converting data to root file done!" << std::endl;
    dataTree->Write();
    std::cout << "Number of Header: " << nOfHeader << std::endl;
    std::cout << "Number of Body: " << nOfBody << std::endl;
    std::cout << "Number of Tail: " << nOfTail << std::endl;
    std::cout << "Number of fully packet: " << nOfFullPacket << std::endl;
    std::cout << "Number of valid body: " << nOfValidBody << std::endl;
    ifs.close();
    tFile->Close();
}

void DataAnalysis::findClosestToHalfMax(const std::vector<uint16_t>& vADCdata, uint16_t& indexOfPeak, uint16_t& indexOfHalfMax)
{
    auto maxPosition = std::max_element(vADCdata.begin(), vADCdata.end());
    indexOfPeak = std::distance(vADCdata.begin(), maxPosition);
    uint16_t halfMax = pPeakOfWave / 2 + pMeanOfBaseline;
    uint16_t minDiff = 0xFFFF;
    for (auto it = vADCdata.begin(); it != maxPosition; it++)
    {
        if (std::abs(*it - halfMax) < minDiff)
        {
            minDiff = std::abs(*it - halfMax);
            indexOfHalfMax = std::distance(vADCdata.begin(), it);
        }
    }
}

void DataAnalysis::ExtractBaseline()
{
    if (!dataTree)
    {
        std::cerr << "Error: dataTree is nullptr!" << std::endl;
        return;
    }
    else  
    {
        std::cout << "dataTree entries: " << dataTree->GetEntries() << std::endl;
    }
    // 预分配内存，假设最多有 cMaxNumChannel 个不同的 pChannelIndex
    umBaseOfChannel.reserve(cMaxNumChannel);
    TTreeReader reader(dataTree);
    TTreeReaderValue<uint8_t> rChannelIndex(reader, "pChannelIndex");
    TTreeReaderValue<uint16_t> rMeanOfBaseline(reader, "pMeanOfBaseline");
    TTreeReaderValue<double> rSigmaOfBaseline(reader, "pSigmaOfBaseline");

    while (reader.Next())
    {
        uint8_t channelIndex = *rChannelIndex;
        uint16_t meanOfBaseline = *rMeanOfBaseline;
        double sigmaOfBaseline = *rSigmaOfBaseline;
        std::cout << (int)channelIndex << " ";
        if (umBaseOfChannel.find(channelIndex) == umBaseOfChannel.end())
        {
            umBaseOfChannel[channelIndex][0] = meanOfBaseline;
            umBaseOfChannel[channelIndex][1] = sigmaOfBaseline;
            umBaseOfChannel[channelIndex][2] = 1;
        }
        else
        {
            umBaseOfChannel[channelIndex][0] += meanOfBaseline;
            umBaseOfChannel[channelIndex][1] += sigmaOfBaseline;
            umBaseOfChannel[channelIndex][2] += 1;
        }
    }
    std::cout << std::endl;
    std::cout << "NumChannel: " << umBaseOfChannel.size() << std::endl;
    for (auto& [ChannelIndex, values] : umBaseOfChannel)
    {
        values[0] = values[0] / values[2];
        values[1] = values[1] / values[2];
    }
}

void DataAnalysis::WriteConfigFile(std::string fileName, ConfigData configData)
{
    std::string sConfigFileNameWoSuffix = fileName.substr(0, fileName.find_last_of("."));
    if (!std::filesystem::exists((std::filesystem::path)sConfigFileNameWoSuffix))
    {
        std::filesystem::create_directory((std::filesystem::path)sConfigFileNameWoSuffix);
    }
    else 
    {
        std::filesystem::remove_all((std::filesystem::path)sConfigFileNameWoSuffix);
        std::filesystem::create_directory((std::filesystem::path)sConfigFileNameWoSuffix);
    }
    ExtractBaseline();
    char configCMD[9] = {0x00};
    std::string sSubConfigFileName = "";

    if (configData.uIIRFilterNum != 0)
    {
        sSubConfigFileName = sConfigFileNameWoSuffix + "/IIRFilter.dat";
        std::ofstream ofs(sSubConfigFileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open())
        {
            std::cerr << "Error: IIRFilter.dat open failed." << std::endl;
            return;
        }
        for (int i = 0; i < configData.uIIRFilterNum; i++)
        {
            std::cout << "IIRFilter: " << configData.puIIRFilter[i] << std::endl;
            // IIR Filter, command address 1100 ~ 1110
            configCMD[0] = 0x00 + (2 * i) % 16;
            configCMD[1] = 0x10 + (2 * i) / 16;
            configCMD[2] = 0x21;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40 + ((uint16_t)configData.puIIRFilter[i] & 0x0F);
            configCMD[5] = 0x50 + (((uint16_t)configData.puIIRFilter[i] >> 4) & 0x0F);
            configCMD[6] = 0x60 + (((uint16_t)configData.puIIRFilter[i] >> 8) & 0x0F);
            configCMD[7] = 0x70 + (((uint16_t)configData.puIIRFilter[i] >> 12) & 0x0F);
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));
        }
        ofs.close();
        std::cout << "IIRFilter file: " << sSubConfigFileName << " written." << std::endl;
    }

    if (configData.uBaselineNum != 0)
    {
        sSubConfigFileName = sConfigFileNameWoSuffix + "/Baseline.dat";
        std::ofstream ofs(sSubConfigFileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open())
        {
            std::cerr << "Error: Baseline.dat open failed." << std::endl;
            return;
        }
        for (int i = 0; i < configData.uBaselineNum; i++)
        {
            std::cout << "Baseline: " << configData.puBaseline[i] << std::endl;
            // Baseline, command address 1112 ~ 1116
            configCMD[0] = 0x02 + (2 * i) % 16;
            configCMD[1] = 0x11 + (2 * i) / 16;
            configCMD[2] = 0x21;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40 + ((uint16_t)configData.puBaseline[i] & 0x0F);
            configCMD[5] = 0x50 + (((uint16_t)configData.puBaseline[i] >> 4) & 0x0F);
            configCMD[6] = 0x60 + (((uint16_t)configData.puBaseline[i] >> 8) & 0x0F);
            configCMD[7] = 0x70 + (((uint16_t)configData.puBaseline[i] >> 12) & 0x0F);
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));
        }
        ofs.close();
        std::cout << "Baseline file: " << sSubConfigFileName << " written." << std::endl;
    }

    if (configData.uNHitChannel != 0)
    {
        sSubConfigFileName = sConfigFileNameWoSuffix + "/NHitChannel.dat";
        std::ofstream ofs(sSubConfigFileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open())
        {
            std::cerr << "Error: NHitChannel.dat open failed." << std::endl;
            return;
        }

        // Hit Channel Number, command address 1802
        configCMD[0] = 0x02;
        configCMD[1] = 0x10;
        configCMD[2] = 0x28;
        configCMD[3] = 0x31;
        configCMD[4] = 0x40 + configData.uNHitChannel % 16;
        configCMD[5] = 0x50 + configData.uNHitChannel / 16;
        configCMD[6] = 0x60;
        configCMD[7] = 0x70;
        configCMD[8] = 0x83;
        ofs.write(configCMD, sizeof(configCMD));
        ofs.close();
        std::cout << "Hit Channel Number file: " << sSubConfigFileName << " written!" << std::endl;
    }
    if (configData.uTrigDelayCycle != 0)
    {
        sSubConfigFileName = sConfigFileNameWoSuffix + "/TrigDelayCycle.dat";
        std::ofstream ofs(sSubConfigFileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open())
        {
            std::cerr << "Error: TrigDelayCycle.dat open failed." << std::endl;
            return;
        }

        // Trigger Delay Cycle, command address 1804
        configCMD[0] = 0x04;
        configCMD[1] = 0x10;
        configCMD[2] = 0x28;
        configCMD[3] = 0x31;
        configCMD[4] = 0x40 + ((uint16_t)(configData.uTrigDelayCycle) & 0x0F);
        configCMD[5] = 0x50 + (((uint16_t)(configData.uTrigDelayCycle) >> 4) & 0x0F);
        configCMD[6] = 0x60 + (((uint16_t)(configData.uTrigDelayCycle) >> 8) & 0x0F);
        configCMD[7] = 0x70 + (((uint16_t)(configData.uTrigDelayCycle) >> 12) & 0x0F);
        configCMD[8] = 0x83;
        ofs.write(configCMD, sizeof(configCMD));
        ofs.close();
        std::cout << "Trigger Delay Cycle file: " << sSubConfigFileName << " written!" << std::endl;
    }
    if (configData.uNSigmaCompres != 0)
    {
        for (auto& [channelIndex, meanSigma] : umBaseOfChannel)
        {
            sSubConfigFileName = sConfigFileNameWoSuffix + "/CompressThreshold_" + std::to_string(channelIndex) + ".dat";
            std::ofstream ofs;
            ofs.open(sSubConfigFileName, std::ios::binary | std::ios::out);
            if (!ofs.is_open()) {
                std::cerr << "Failed to open file: " << sSubConfigFileName << std::endl;
                exit(1);
            }
            // FEID, command address 1806
            //configCMD[9] = {0x00};
            configCMD[0] = 0x06;
            configCMD[1] = 0x10;
            configCMD[2] = 0x28;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40 + pFEID % 16;
            configCMD[5] = 0x50 + pFEID / 16;
            configCMD[6] = 0x60;
            configCMD[7] = 0x70;
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));

            // Channel ID, command address 1808
            configCMD[0] = 0x08;
            configCMD[1] = 0x10;
            configCMD[2] = 0x28;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40 + channelIndex % 16;
            configCMD[5] = 0x50 + channelIndex / 16;
            configCMD[6] = 0x60;
            configCMD[7] = 0x70;
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));

            // Threshold, command address 180B
            configCMD[0] = 0x0B;
            configCMD[1] = 0x10;
            configCMD[2] = 0x28;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40;
            configCMD[5] = 0x50;
            configCMD[6] = 0x60;
            configCMD[7] = 0x70;
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));

            // Threshold, command address 180A
            configCMD[0] = 0x0A;
            configCMD[1] = 0x10;
            configCMD[2] = 0x28;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40 + ((uint16_t)(meanSigma[0] + configData.uNSigmaCompres*meanSigma[1]) & 0x0F);
            configCMD[5] = 0x50 + (((uint16_t)(meanSigma[0] + configData.uNSigmaCompres*meanSigma[1]) >> 4) & 0x0F);
            configCMD[6] = 0x60 + (((uint16_t)(meanSigma[0] + configData.uNSigmaCompres*meanSigma[1]) >> 8) & 0x0F);
            configCMD[7] = 0x70 + (((uint16_t)(meanSigma[0] + configData.uNSigmaCompres*meanSigma[1]) >> 12) & 0x0F);
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));

            // Config command, command address 180C
            configCMD[0] = 0x0C;
            configCMD[1] = 0x10;
            configCMD[2] = 0x28;
            configCMD[3] = 0x31;
            configCMD[4] = 0x41;
            configCMD[5] = 0x50;
            configCMD[6] = 0x60;
            configCMD[7] = 0x70;
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));
            ofs.close();
            std::cout << "Compress Threshold file: " << sSubConfigFileName << " written!" << std::endl;
        }
    }
    if (configData.uHitWidthCycle != 0)
    {
        sSubConfigFileName = sConfigFileNameWoSuffix + "/HitWidthCycle.dat";
        std::ofstream ofs(sSubConfigFileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open())
        {
            std::cerr << "Error: HitWidthCycle.dat open failed." << std::endl;
            return;
        }
        // Hit Width Cycle, command address 180E
        configCMD[0] = 0x0E;
        configCMD[1] = 0x10;
        configCMD[2] = 0x28;
        configCMD[3] = 0x31;
        configCMD[4] = 0x40 + configData.uHitWidthCycle % 16;
        configCMD[5] = 0x50 + configData.uHitWidthCycle / 16;
        configCMD[6] = 0x60;
        configCMD[7] = 0x70;
        configCMD[8] = 0x83;
        ofs.write(configCMD, sizeof(configCMD));
        ofs.close();
        std::cout << "Hit Width Cycle file: " << sSubConfigFileName << " written!" << std::endl;
    }
    if (configData.uTrigRiseStep != 0)
    {
        sSubConfigFileName = sConfigFileNameWoSuffix + "/TrigRiseStep.dat";
        std::ofstream ofs(sSubConfigFileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open())
        {
            std::cerr << "Error: TrigRiseStep.dat open failed." << std::endl;
            return;
        }
        // Trigger Rise Step, command address 1810
        configCMD[0] = 0x00;
        configCMD[1] = 0x11;
        configCMD[2] = 0x28;
        configCMD[3] = 0x31;
        configCMD[4] = 0x40 + configData.uTrigRiseStep % 16;
        configCMD[5] = 0x50 + configData.uTrigRiseStep / 16;
        configCMD[6] = 0x60;
        configCMD[7] = 0x70;
        configCMD[8] = 0x83;
        ofs.write(configCMD, sizeof(configCMD));
        ofs.close();
        std::cout << "Trigger Rise Step file: " << sSubConfigFileName << " written!" << std::endl;
    }

    if (pDataFormat == 1)
    {
        sSubConfigFileName = sConfigFileNameWoSuffix + "/DataFormat.dat";
        std::ofstream ofs(sSubConfigFileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open())
        {
            std::cerr << "Error: DataFormat.dat open failed." << std::endl;
            return;
        }
        // Data Format, command address 1812
        configCMD[0] = 0x02;
        configCMD[1] = 0x11;
        configCMD[2] = 0x28;
        configCMD[3] = 0x31;
        configCMD[4] = 0x40 + pDataFormat % 16;
        configCMD[5] = 0x50 + pDataFormat / 16;
        configCMD[6] = 0x60;
        configCMD[7] = 0x70;
        configCMD[8] = 0x83;
        ofs.write(configCMD, sizeof(configCMD));
        ofs.close();
        std::cout << "Data Format file: " << sSubConfigFileName << " written!" << std::endl;
    }
    std::cout << "All config files written!" << std::endl;
}

void DataAnalysis::WriteFilterFile(const char* filterFileName, int* IIRFilter, int IIRFilterNum, int* Baseline, int BaselineByteNum)
{
    std::ofstream ofs;
    ofs.open(filterFileName, std::ios::binary | std::ios::out);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file: " << filterFileName << std::endl;
        exit(1);
    }
    //extractBaseline();
    char configCMD[9] = {0x00};

    if (IIRFilterNum != 0)
    {
        for (int i = 0; i < IIRFilterNum; i++)
        {
            if (IIRFilter[i] == 0) continue;
            std::cout << "IIRFilter: " << IIRFilter[i] << std::endl;
            // IIR Filter, command address 1100 ~ 1110
            configCMD[0] = 0x00 + (2 * i) % 16;
            configCMD[1] = 0x10 + (2 * i) / 16;
            configCMD[2] = 0x21;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40 + ((uint16_t)IIRFilter[i] & 0x0F);
            configCMD[5] = 0x50 + (((uint16_t)IIRFilter[i] >> 4) & 0x0F);
            configCMD[6] = 0x60 + (((uint16_t)IIRFilter[i] >> 8) & 0x0F);
            configCMD[7] = 0x70 + (((uint16_t)IIRFilter[i] >> 12) & 0x0F);
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));
        }
    }
    
    if (BaselineByteNum != 0)
    {
        for (int i = 0; i < BaselineByteNum; i++)
        {
            if (Baseline[i] == 0) continue;
            // Baseline, command address 1112 ~ 1116
            configCMD[0] = 0x02 + (2 * i) % 16;
            configCMD[1] = 0x11 + (2 * i) / 16;
            configCMD[2] = 0x21;
            configCMD[3] = 0x31;
            configCMD[4] = 0x40 + ((uint16_t)Baseline[i] & 0x0F);
            configCMD[5] = 0x50 + (((uint16_t)Baseline[i] >> 4) & 0x0F);
            configCMD[6] = 0x60 + (((uint16_t)Baseline[i] >> 8) & 0x0F);
            configCMD[7] = 0x70 + (((uint16_t)Baseline[i] >> 12) & 0x0F);
            configCMD[8] = 0x83;
            ofs.write(configCMD, sizeof(configCMD));
        }
    }
}
void DataAnalysis::CalIIRFilterCRRParameter(double parameters[3], long double Ts, long double R1, long double R2, long double C1)
{
    double b0 = R2 / (R1 + R2) * (1 + 2 / Ts * C1 * R1) / (1 + 2 / Ts * C1 * R1 * R2 / (R1 + R2));
    double b1 = R2 / (R1 + R2) * (1 - 2 / Ts * C1 * R1) / (1 + 2 / Ts * C1 * R1 * R2 / (R1 + R2));
    double a1 = -(1 - 2 / Ts * C1 * R1 * R2 / (R1 + R2)) / (1 + 2 / Ts * C1 * R1 * R2 / (R1 + R2));
    std::cout << "b0: " << b0 << std::endl;
    std::cout << "b1: " << b1 << std::endl;
    std::cout << "a1: " << a1 << std::endl;
    parameters[0] = b0;
    parameters[1] = b1;
    parameters[2] = a1;
}

void DataAnalysis::CalIIRFilterCRParameter(double parameters[3], long double Ts, long double Magnification, long double R, long double C)
{
    double b0 = Magnification * 2 / Ts * R * C / (1 + 2 / Ts * R * C);
    double b1 = Magnification * 2 / Ts * R * C / (1 + 2 / Ts * R * C);
    double a1 = -(1 - 2 / Ts * R * C) / (1 + 2 / Ts * R * C);
    std::cout << "Mag: " << Magnification << std::endl;
    std::cout << "Ts: " << Ts << std::endl;
    std::cout << "C: " << C << std::endl;
    std::cout << "R: " << R << std::endl;
    std::cout << "b0: " << b0 << std::endl;
    std::cout << "b1: " << b1 << std::endl;
    std::cout << "a1: " << a1 << std::endl;
    parameters[0] = b0;
    parameters[1] = b1;
    parameters[2] = a1;
}

void DataAnalysis::CalIIRFilterRCParameter(double parameters[3], long double Ts, long double Magnification, long double R, long double C)
{
    double b0 = Magnification / (1 + 2 / Ts * R * C);
    double b1 = Magnification / (1 + 2 / Ts * R * C);
    double a1 = -(1 - 2 / Ts * R * C) / (1 + 2 / Ts * R * C);
    std::cout << "Mag: " << Magnification << std::endl;
    std::cout << "Ts: " << Ts << std::endl;
    std::cout << "C: " << C << std::endl;
    std::cout << "R: " << R << std::endl;
    std::cout << "b0: " << b0 << std::endl;
    std::cout << "b1: " << b1 << std::endl;
    std::cout << "a1: " << a1 << std::endl;
    parameters[0] = b0;
    parameters[1] = b1;
    parameters[2] = a1;
}

void DataAnalysis::Decimal2binary(double decimal, int precision, int& binary)
{
    binary = 0;
    bool isNegative = false;
    if (decimal < 0)
    {
        isNegative = true;
        decimal = -decimal;
    }

    for (int i = 0; i < (precision-1); ++i) {
        decimal *= 2;
        binary = binary * 2 + static_cast<int>(decimal);
        decimal -= static_cast<int>(decimal);
    }
    if (isNegative)
    {
        for (int i = 0; i < precision; ++i)
        {
            binary = binary ^ (1 << i);
        }
        binary += 1;
    }
}

void DataAnalysis::Decimal2binary(double decimal, int intPrecision, int decPrecision, int& binary)
{
    binary = 0;
    bool isNegative = false;
    if (decimal < 0)
    {
        isNegative = true;
        decimal = -decimal;
    }

    int integerPart = static_cast<int>(decimal);
    double decimalPart = decimal - integerPart;

    //int integerWidth = std::ceil(log2(integerPart+1));

    for (int j = 0; j < intPrecision; ++j)
    {
        binary += (integerPart % 2) * std::pow(2, j);
        integerPart /= 2;
    }

    for (int i = 0; i < decPrecision; ++i) {
        decimalPart *= 2;
        binary = binary * 2 + static_cast<int>(decimalPart);
        decimalPart -= static_cast<int>(decimalPart);
    }

    if (isNegative)
    {
        for (int i = 0; i < intPrecision+decPrecision+1; ++i)
        {
            binary = binary ^ (1 << i);
        }
        binary += 1;
    }
}

void DataAnalysis::SetDataFormat(int dataFormat)
{
    pDataFormat = dataFormat;
}

void DataAnalysis::SetTestMode(bool testMode)
{
    pTestMode = testMode;
}

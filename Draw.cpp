#include "Draw.h"

#include <iomanip>
#include <algorithm>
#include "TCanvasWidget.h"
#include "TFile.h"
#include "TGraph.h"
#include "TPaveText.h"
#include "TMultiGraph.h"
#include <fstream>
//#include <fstream>
#include "TLatex.h"
#include <TCanvas.h>
#include <TGLOrthoCamera.h>
#include <TH1.h>
#include <TMath.h>
#include <winsock2.h>
#include <TGraphErrors.h>
#include "TMarker.h"
#include "TH2.h"
#include "TF2.h"
#include <thread> // Include the header file for the Sleep function
#include "TF1.h"
#include <TStyle.h>

Draw::Draw(TCanvasWidget* tCanvasWidget, std::string tFileName)
    : fileName(tFileName),
      canvasWidget(tCanvasWidget)
{
    std::cout << "Start to read root file: " << fileName << std::endl;
    std::thread tPrintProgress(&Draw::PrintProgress, this);
    tPrintProgress.detach();
    fileNameWoSuffix = fileName.substr(0, fileName.find_last_of("."));
    dFile = new TFile(fileName.c_str(), "READ");
    dTree = (TTree*)dFile->Get(treeName.c_str());
    tTreeReader = new TTreeReader(dTree);
    nEntries = dTree->GetEntries();
    std::cout << "File: " << fileName << " has " << nEntries << " entries." << std::endl;
    //canvas = new TCanvas("c1", "c1", 0, 0, 800, 800);
    canvasWidget->getCanvas()->Clear();
}

Draw::~Draw()
{
    if (tTreeReader != nullptr)
    {
        delete tTreeReader;
        tTreeReader = nullptr;
    }
    if (dTree != nullptr)
    {
        delete dTree;
        dTree = nullptr;
    }
    if (dFile != nullptr)
    {
        dFile->Close();
        delete dFile;
        dFile = nullptr;
    }
}

void Draw::PrintProgress()
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

void Draw::drawSeveralWaves(int nBeg, int nFig, const char* figName)
{
    tTreeReader->Restart();
    TTreeReaderValue<std::vector<uint16_t>> rvADCdata(*tTreeReader, "pvADCdata");

    canvasWidget->getCanvas()->cd();
    canvasWidget->getCanvas()->Clear();
    canvasWidget->getCanvas()->SetBorderMode(0);
    canvasWidget->getCanvas()->SetFillColor(0);
    canvasWidget->getCanvas()->SetGrid();
    TMultiGraph* tMultiGraph = new TMultiGraph();
    TGraph* tGraph[nGraph];
    for (int i = nBeg; i < nBeg+nFig; i++)
    {
        tTreeReader->SetEntry(i);
        tGraph[i-nBeg] = new TGraph(cADCdata);
        for (int j = 0; j < cADCdata; j++)
        {
            //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
            tGraph[i-nBeg]->SetPoint(j, j, (*rvADCdata)[j]);
        }
        tGraph[i-nBeg]->SetLineWidth(1);
        tGraph[i-nBeg]->SetTitle(Form("%d", i));
        
        tMultiGraph->Add(tGraph[i-nBeg]);
    }
    //gPad->Modified();
    //tMultiGraph->GetXaxis()->SetLimits(0, 1024);
    //tMultiGraph->SetMinimum(0); tMultiGraph->SetMaximum(2000);
    tMultiGraph->SetTitle("Several Waves; Time; ADC");
    tMultiGraph->GetYaxis()->CenterTitle();
    tMultiGraph->GetXaxis()->CenterTitle();
    tMultiGraph->GetYaxis()->SetTitleOffset(2.2);
    if (nFig == 1)
    {
        tMultiGraph->Draw("a c l pmc plc");
    }
    else  
    {
        tMultiGraph->Draw("a fb l pmc plc 3d");
    }

    canvasWidget->getCanvas()->Update();
    cFigName = figName;
    //canvasWidget->getCanvas()->Print(figName);

    //tRootCanvas->Connect("CloseWindow()", "TApplication", tApp, "Terminate()"); // ref:https://root.cern/manual/creating_a_user_application/
}
void Draw::drawSigmaOfBaseline(const char* figName)
{   
    std::unordered_map<uint8_t, double[3]>().swap(umMeanAndSigmaOfBase);
    std::unordered_map<int, std::vector<std::vector<uint16_t>>>().swap(umChannelWaves);
    std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeak2PeakOfSignal);
    std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeakOfSignal);
    std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelMeanBaseline);
    std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelWaves);
    std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelXWaves);
    std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelYWaves);
    std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelAWaves);
    std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelXIndexOfPeak);
    std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelYIndexOfPeak);
    std::unordered_map<uint32_t, StringIntPair>().swap(umEventMap);
    std::map<uint32_t, uint32_t>().swap(mEventEnergy);

    tTreeReader->Restart();
    TTreeReaderValue<uint8_t> rChannelIndex(*tTreeReader, "pChannelIndex");
    TTreeReaderValue<uint16_t> rMeanOfBaseline(*tTreeReader, "pMeanOfBaseline");
    TTreeReaderValue<double> rSigmaOfBaseline(*tTreeReader, "pSigmaOfBaseline");

    umMeanAndSigmaOfBase.reserve(cMaxNumChannel);
    currentEntry = 0;
    while (tTreeReader->Next()) {
        uint8_t pChannelIndex = *rChannelIndex;
        uint16_t pMeanOfBaseline = *rMeanOfBaseline;
        double pSigmaOfBaseline = *rSigmaOfBaseline;

        umMeanAndSigmaOfBase[pChannelIndex][0] += pMeanOfBaseline;
        umMeanAndSigmaOfBase[pChannelIndex][1] += pSigmaOfBaseline;
        umMeanAndSigmaOfBase[pChannelIndex][2] += 1;
        //umMeanAndSigmaOfBase[pChannelIndex].addDataPoint(pMeanOfBaseline);
        currentEntry++;
        //std::cout << "\r" << "Processing: " << currentEntry << "/" << nEntries << " " << std::flush;
        //std::cout << "Mean and Standard Deviation of Baseline for Channel " << (int)pChannelIndex << ": " << pMeanOfBaseline << " " << pSigmaOfBaseline << std::endl;
        Progress = static_cast<double>(currentEntry) / nEntries * 100.0;
        emit progressChanged(Progress);
    }
    Progress = 100.0;
    emit progressChanged(Progress);
    //Sleep(300000);
    Progress = 0.0;
    std::cout << std::endl;
    
    std::cout << "umMeanAndSigmaOfBase.size(): " << umMeanAndSigmaOfBase.size() << std::endl;
    if (umMeanAndSigmaOfBase.empty())
    {
        std::cout << "umMeanAndSigmaOfBase is empty!" << std::endl;
        return;
    }

    canvasWidget->getCanvas()->cd();
    canvasWidget->getCanvas()->Clear();
    canvasWidget->getCanvas()->SetBorderMode(0);
    canvasWidget->getCanvas()->SetFillColor(0);
    canvasWidget->getCanvas()->SetGrid();
    canvasWidget->getCanvas()->Divide(2, 1);
    canvasWidget->getCanvas()->cd(1);
    TGraphErrors* tGraph1 = new TGraphErrors();
    int pointIndex = 0;
    for (const auto& item : umMeanAndSigmaOfBase) {
        uint8_t pChannelIndex = item.first;
        //const Welford& welford = item.second;
        uint16_t pMean = item.second[0] / item.second[2];
        double pSigma = item.second[1] / item.second[2];
        //std::cout << "pChannelIndex: " << (int)pChannelIndex << " pMean: " << pMean << " pSigma: " << pSigma << std::endl;

        tGraph1->SetPoint(pointIndex, pChannelIndex, pMean);
        tGraph1->SetPointError(pointIndex, 0, pSigma);
        pointIndex++;
    }
    //tGraph1->GetXaxis()->SetLimits(0, 32);
    //tGraph1->SetMinimum(0);    tGraph1->SetMaximum(2000);
    tGraph1->SetMarkerStyle(1);
    tGraph1->SetMarkerSize(1);
    tGraph1->SetTitle("Mean and Standard Deviation of Baseline for Each Channel");
    tGraph1->GetXaxis()->SetTitle("Channel Index");
    tGraph1->GetXaxis()->CenterTitle();
    tGraph1->GetYaxis()->SetTitle("Value");
    tGraph1->GetYaxis()->CenterTitle();
    tGraph1->Draw("AP pmc plc");

    //pad2->cd();
    canvasWidget->getCanvas()->cd(2);
    TGraph* tGraph2 = new TGraph();
    pointIndex = 0;
    for (const auto& item : umMeanAndSigmaOfBase) {
        uint8_t pChannelIndex = item.first;
        //const Welford& welford = item.second;
        double pSigma = item.second[1] / item.second[2];
        //std::cout << "pChannelIndex: " << (int)pChannelIndex << " pSigma: " << pSigma << std::endl;

        tGraph2->SetPoint(pointIndex, pChannelIndex, pSigma);
        pointIndex++;
    }
    //tGraph2->SetMinimum(0);    tGraph2->SetMaximum(2000);
    tGraph2->SetMarkerStyle(1);
    tGraph2->SetMarkerSize(1);
    tGraph2->SetFillColor(38);
    tGraph2->SetTitle("Standard Deviation of Baseline for Each Channel");
    tGraph2->GetXaxis()->SetTitle("Channel Index");
    tGraph2->GetXaxis()->CenterTitle();
    tGraph2->GetYaxis()->SetTitle("Value");
    tGraph2->GetYaxis()->CenterTitle();
    tGraph2->Draw("AB pmc plc");

    canvasWidget->getCanvas()->Update();
    cFigName = figName;
    //canvasWidget->getCanvas()->Print(figName);
    //tRootCanvas->Connect("CloseWindow()", "TApplication", tApp, "Terminate()"); // ref:https://root.cern/manual/creating_a_user_application/
    //tFileSigma->Close();
}
void Draw::drawChannelWave(int channel, int nBeg, int nFig, const char* figName)
{
    if (umChannelWaves.empty())
    {
        std::unordered_map<uint8_t, double[3]>().swap(umMeanAndSigmaOfBase);
        std::unordered_map<int, std::vector<std::vector<uint16_t>>>().swap(umChannelWaves);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeak2PeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelMeanBaseline);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelXWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelYWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelAWaves);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelXIndexOfPeak);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelYIndexOfPeak);
        std::unordered_map<uint32_t, StringIntPair>().swap(umEventMap);
        std::map<uint32_t, uint32_t>().swap(mEventEnergy);

        tTreeReader->Restart();
        TTreeReaderValue<std::vector<uint16_t>> rvADCdata(*tTreeReader, "pvADCdata");
        TTreeReaderValue<uint8_t> rChannelIndex(*tTreeReader, "pChannelIndex");
        TTreeReaderValue<uint16_t> rPeakOfWave(*tTreeReader, "pPeakOfWave");
        TTreeReaderValue<uint16_t> rMeanOfBaseline(*tTreeReader, "pMeanOfBaseline");
        TTreeReaderValue<uint16_t> rPeak2PeakOfWave(*tTreeReader, "pPeak2PeakOfWave");

        umChannelWaves.reserve(cMaxNumChannel);
        umChannelPeak2PeakOfSignal.reserve(cMaxNumChannel);
        umChannelPeakOfSignal.reserve(cMaxNumChannel);
        umChannelMeanBaseline.reserve(cMaxNumChannel);

        currentEntry = 0;
        while (tTreeReader->Next()) {
            uint8_t pChannelIndex = *rChannelIndex;
            std::vector<uint16_t> vADCdata = *rvADCdata;
            uint16_t pPeak2PeakOfWave = *rPeak2PeakOfWave;
            uint16_t pPeakOfWave = *rPeakOfWave;
            uint16_t pMeanOfBaseline = *rMeanOfBaseline;
            umChannelWaves[pChannelIndex].push_back(vADCdata);
            umChannelPeak2PeakOfSignal[pChannelIndex].push_back(pPeak2PeakOfWave);
            umChannelPeakOfSignal[pChannelIndex].push_back(pPeakOfWave);
            umChannelMeanBaseline[pChannelIndex].push_back(pMeanOfBaseline);
            currentEntry++;
            //std::cout << "\r" << "Processing: " << currentEntry << "/" << nEntries << " " << std::flush;
            Progress = static_cast<double>(currentEntry) / nEntries * 100.0;
            emit progressChanged(Progress);
        }
        Progress = 100.0;
        emit progressChanged(Progress);
        //Sleep(300000);
        Progress = 0.0;
        std::cout << std::endl;
    }

    //std::cout << "vChannelWaves.size(): " << vChannelWaves.size() << std::endl;
    if (umChannelWaves[channel].size() == 0 || umChannelWaves[channel].size() < nBeg+nFig)
    {
        std::cerr << "Error: No data for channel " << channel << "!" << std::endl;
        return;
    }

    canvasWidget->getCanvas()->cd();
    canvasWidget->getCanvas()->Clear();
    canvasWidget->getCanvas()->SetBorderMode(0);
    canvasWidget->getCanvas()->SetFillColor(0);
    canvasWidget->getCanvas()->SetGrid();
    TMultiGraph* tMultiGraph = new TMultiGraph();
    TGraph* tGraph[nGraph];
    TLatex* tLatex;

    double dMax = 0.0;
    double dMin = 2000.0;

    for (int i = nBeg;
      i<min(
        nBeg+nFig
        ,(int)umChannelWaves[channel].size());
      i++)
    {
        tGraph[i-nBeg] = new TGraph(cADCdata);
        //TExec *ex = new TExec("ex", "drawText();");
        //tGraph[i-nBeg]->GetListOfFunctions()->Add(ex);
        for (int j = 0; j < cADCdata; j++)
        {
            //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
            tGraph[i-nBeg]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(umChannelWaves[channel][i][j])/2048.0*1000.0);
            //tLatex = new TLatex(j, umChannelWaves[channel][i][j]+0.3, Form("%4.2d", umChannelWaves[channel][i][j]));
        }

        if (TMath::MaxElement(cADCdata, tGraph[i-nBeg]->GetY()) > dMax)
        {
            dMax = TMath::MaxElement(cADCdata, tGraph[i-nBeg]->GetY());
        }
        if (TMath::MinElement(cADCdata, tGraph[i-nBeg]->GetY()) < dMin)
        {
            dMin = TMath::MinElement(cADCdata, tGraph[i-nBeg]->GetY());
        }

        tLatex = new TLatex(1023*0.025, static_cast<double>(umChannelMeanBaseline[channel][i]+umChannelPeakOfSignal[channel][i])/2.048, Form("Peak to peak of wave: %4.2f mV", umChannelPeak2PeakOfSignal[channel][i]/2048.0*1000));
        tLatex->SetTextSize(0.025);
        tLatex->SetTextAlign(31);
        tGraph[i-nBeg]->SetLineWidth(1);
        tGraph[i-nBeg]->SetTitle(Form("%d", i));
        
        tMultiGraph->Add(tGraph[i-nBeg]);
    }

    tMultiGraph->GetXaxis()->SetLimits(0, 1024*0.025);
    tMultiGraph->SetMinimum(dMin); tMultiGraph->SetMaximum(dMax);
    tMultiGraph->SetTitle(("Channel " + std::to_string(channel) + " Signal; Time [us]; Amplitude [mV]").c_str());
    tMultiGraph->GetYaxis()->CenterTitle();
    tMultiGraph->GetXaxis()->CenterTitle();
    //tMultiGraph->GetYaxis()->SetTitleOffset(2.2);
    if (nFig == 1)
    {
        tMultiGraph->Draw("a l pmc plc");
        //pt->Draw();
        tLatex->Draw();
        //tText->Draw();
    }
    else  if (nFig > 1)
    {
        tMultiGraph->Draw("a fb l pmc plc 3d");
    }
    else {
        std::cerr << "Error: nFig is not valid!" << std::endl;
    }

    canvasWidget->getCanvas()->Update();
    cFigName = figName;
    //canvasWidget->getCanvas()->Print(figName);
}
void Draw::drawEventWaves(int eventID, const char* figName)
{
    if (umEventChannelWaves.empty())
    {
        std::unordered_map<uint8_t, double[3]>().swap(umMeanAndSigmaOfBase);
        std::unordered_map<int, std::vector<std::vector<uint16_t>>>().swap(umChannelWaves);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeak2PeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelMeanBaseline);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelXWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelYWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelAWaves);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelXIndexOfPeak);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelYIndexOfPeak);
        std::unordered_map<uint32_t, StringIntPair>().swap(umEventMap);
        std::map<uint32_t, uint32_t>().swap(mEventEnergy);

        tTreeReader->Restart();
        TTreeReaderValue<std::vector<uint16_t>> rvADCdata(*tTreeReader, "pvADCdata");
        TTreeReaderValue<uint32_t> rEventIndex(*tTreeReader, "pEventIndex");
        TTreeReaderValue<uint8_t> rChannelIndex(*tTreeReader, "pChannelIndex");

        currentEntry = 0;
        while (tTreeReader->Next()) {
            uint32_t pEventIndex = *rEventIndex;
            std::vector<uint16_t> vADCdata = *rvADCdata;
            // CONFIG: + 64 for beta
            uint8_t pChannelIndex = *rChannelIndex;
            //std::cout << "ChannelIndex: " << pChannelIndex << std::endl;
            //uint8_t uChannelIndex = *rChannelIndex;
            umEventChannelWaves[pEventIndex][umChannelMap[pChannelIndex]] = vADCdata;
            if (umChannelMap[pChannelIndex][0] == 'X')
            {
                umEventChannelXWaves[pEventIndex][umChannelMap[pChannelIndex]] = vADCdata;
            }
            else if (umChannelMap[pChannelIndex][0] == 'Y')
            {
                umEventChannelYWaves[pEventIndex][umChannelMap[pChannelIndex]] = vADCdata;
            }
            else if (umChannelMap[pChannelIndex][0] == 'A')
            {
                umEventChannelAWaves[pEventIndex][umChannelMap[pChannelIndex]] = vADCdata;
            }
            else
            {
                //std::cerr << "Warning: Channel name is not valid!" << std::endl;
            }
            currentEntry++;
            //std::cout << "\r" << "Processing: " << currentEntry << "/" << nEntries << " " << std::flush;
            Progress = static_cast<double>(currentEntry) / nEntries * 100.0;
            emit progressChanged(Progress);
        }
        Progress = 100.0;
        emit progressChanged(Progress);
        //Sleep(300000);
        Progress = 0.0;
        std::cout << std::endl;
    }

    canvasWidget->getCanvas()->cd();
    canvasWidget->getCanvas()->Clear();
    //canvasWidget->getCanvas()->SetBorderMode(0);
    canvasWidget->getCanvas()->SetFillColor(0);
    canvasWidget->getCanvas()->SetGrid();

    double dMax = 0.0;
    double dMin = 2000.0;

    if (wChannel == 0)
    {
        //TCanvas* c1 = new TCanvas("c1", "c1", 0, 0, 800, 800);
        TPad* p1 = new TPad("p1", "p1", 0.0, 0.5, 0.5, 1.0, 0, 0);
        p1->Draw();
        TPad* p2 = new TPad("p2", "p2", 0.5, 0.5, 1.0, 1.0, 0, 0);
        p2->Draw();
        TPad* p3 = new TPad("p3", "p3", 0.0, 0.0, 0.5, 0.5, 0, 0);
        p3->Draw();
        TPad* p4 = new TPad("p4", "p4", 0.5, 0.0, 1.0, 0.5, 0, 0);
        p4->Draw();

        TMultiGraph* tMultiGraphX = new TMultiGraph();
        TGraph* tGraph[nGraph];

        for (auto& item : umEventChannelXWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            if (TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1))]->GetY()) > dMax)
            {
                dMax = TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1))]->GetY());
            }
            if (TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1))]->GetY()) < dMin)
            {
                dMin = TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1))]->GetY());
            }
            tGraph[std::stoi(item.first.substr(1))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1))]->SetTitle(item.first.c_str());

            tMultiGraphX->Add(tGraph[std::stoi(item.first.substr(1))]);
        }
        tMultiGraphX->SetTitle(("Event " + std::to_string(eventID) + " X Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphX->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphX->SetMinimum(dMin); tMultiGraphX->SetMaximum(dMax);
        tMultiGraphX->GetYaxis()->CenterTitle();
        tMultiGraphX->GetXaxis()->CenterTitle();
        tMultiGraphX->GetYaxis()->SetTitleOffset(2.2);
        p1->cd();
        tMultiGraphX->Draw("a fb l pmc plc 3d");

        dMax = 0.0;
        dMin = 2000.0;
        TMultiGraph* tMultiGraphY = new TMultiGraph();
        for (auto& item : umEventChannelYWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1, 2))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1, 2))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            if (TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) > dMax)
            {
                dMax = TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            if (TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) < dMin)
            {
                dMin = TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            tGraph[std::stoi(item.first.substr(1, 2))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1, 2))]->SetTitle(item.first.c_str());

            tMultiGraphY->Add(tGraph[std::stoi(item.first.substr(1, 2))]);
        }
        tMultiGraphY->SetTitle(("Event " + std::to_string(eventID) + " Y Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphY->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphY->SetMinimum(dMin); tMultiGraphY->SetMaximum(dMax);
        tMultiGraphY->GetYaxis()->CenterTitle();
        tMultiGraphY->GetXaxis()->CenterTitle();
        tMultiGraphY->GetYaxis()->SetTitleOffset(2.2);
        p2->cd();
        tMultiGraphY->Draw("a fb l pmc plc 3d");
        dMax = 0.0;
        dMin = 2000.0;
    
        TMultiGraph* tMultiGraphA = new TMultiGraph();
        for (auto& item : umEventChannelAWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1, 2))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1, 2))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            if (TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) > dMax)
            {
                dMax = TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            if (TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) < dMin)
            {
                dMin = TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }

            tGraph[std::stoi(item.first.substr(1, 2))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1, 2))]->SetTitle(item.first.c_str());

            tMultiGraphA->Add(tGraph[std::stoi(item.first.substr(1, 2))]);
        }
        tMultiGraphA->SetTitle(("Event " + std::to_string(eventID) + " Anti-coincidence Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphA->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphA->SetMinimum(dMin); tMultiGraphA->SetMaximum(dMax);
        tMultiGraphA->GetYaxis()->CenterTitle();
        tMultiGraphA->GetXaxis()->CenterTitle();
        tMultiGraphA->GetYaxis()->SetTitleOffset(2.2);
        p3->cd();
        tMultiGraphA->Draw("a fb l pmc plc 3d");
        dMax = 0.0;
        dMin = 2000.0;

        TMultiGraph* tMultiGraphF = new TMultiGraph();
        for (auto& item : umEventChannelWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1, 2))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1, 2))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            if (TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) > dMax)
            {
                dMax = TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            if (TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) < dMin)
            {
                dMin = TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }

            tGraph[std::stoi(item.first.substr(1, 2))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1, 2))]->SetTitle(item.first.c_str());

            tMultiGraphF->Add(tGraph[std::stoi(item.first.substr(1, 2))]);
        }
        tMultiGraphF->SetTitle(("Event " + std::to_string(eventID) + " Full Channel Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphF->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphF->SetMinimum(dMin); tMultiGraphF->SetMaximum(dMax);
        tMultiGraphF->GetYaxis()->CenterTitle();
        tMultiGraphF->GetXaxis()->CenterTitle();
        tMultiGraphF->GetYaxis()->SetTitleOffset(2.2);
        p4->cd();
        tMultiGraphF->Draw("a fb l pmc plc 3d");
        dMax = 0.0;
        dMin = 2000.0;
        //c1->Print("a.pdf");
    }
    else if (wChannel == 1)
    {
        //TCanvas* c2 = new TCanvas("c2", "c2", 0, 0, 800, 800);
        canvasWidget->getCanvas()->cd(0);
        TMultiGraph* tMultiGraphX = new TMultiGraph();
        TGraph* tGraph[nGraph];

        for (auto& item : umEventChannelXWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1, 2))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1, 2))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            if (TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) > dMax)
            {
                dMax = TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            if (TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) < dMin)
            {
                dMin = TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            tGraph[std::stoi(item.first.substr(1, 2))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1, 2))]->SetTitle(item.first.c_str());

            tMultiGraphX->Add(tGraph[std::stoi(item.first.substr(1, 2))]);
        }
        tMultiGraphX->SetTitle(("Event " + std::to_string(eventID) + " X Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphX->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphX->SetMinimum(dMin); tMultiGraphX->SetMaximum(dMax);
        tMultiGraphX->GetYaxis()->CenterTitle();
        tMultiGraphX->GetXaxis()->CenterTitle();
        tMultiGraphX->GetYaxis()->SetTitleOffset(2.2);
        tMultiGraphX->Draw("a fb l pmc plc 3d");
        dMax = 0.0;
        dMin = 2000.0;
        //c2->Print("b.pdf");
    }
    else if (wChannel == 2)
    {
        TMultiGraph* tMultiGraphY = new TMultiGraph();
        TGraph* tGraph[nGraph];
        for (auto& item : umEventChannelYWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1, 2))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1, 2))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            if (TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) > dMax)
            {
                dMax = TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            if (TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) < dMin)
            {
                dMin = TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }

            tGraph[std::stoi(item.first.substr(1, 2))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1, 2))]->SetTitle(item.first.c_str());

            tMultiGraphY->Add(tGraph[std::stoi(item.first.substr(1, 2))]);
        }
        tMultiGraphY->SetTitle(("Event " + std::to_string(eventID) + " Y Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphY->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphY->SetMinimum(dMin); tMultiGraphY->SetMaximum(dMax);
        tMultiGraphY->GetYaxis()->CenterTitle();
        tMultiGraphY->GetXaxis()->CenterTitle();
        tMultiGraphY->GetYaxis()->SetTitleOffset(2.2);
        tMultiGraphY->Draw("a fb l pmc plc 3d");
        dMax = 0.0;
        dMin = 2000.0;
    }
    else if (wChannel == 3)
    {
        TMultiGraph* tMultiGraphA = new TMultiGraph();
        TGraph* tGraph[nGraph];
        for (auto& item : umEventChannelAWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1, 2))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1, 2))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            if (TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) > dMax)
            {
                dMax = TMath::MaxElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            if (TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY()) < dMin)
            {
                dMin = TMath::MinElement(cADCdata, tGraph[std::stoi(item.first.substr(1, 2))]->GetY());
            }
            tGraph[std::stoi(item.first.substr(1, 2))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1, 2))]->SetTitle(item.first.c_str());

            tMultiGraphA->Add(tGraph[std::stoi(item.first.substr(1, 2))]);
        }
        tMultiGraphA->SetTitle(("Event " + std::to_string(eventID) + " Anti-coincidence Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphA->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphA->SetMinimum(dMin); tMultiGraphA->SetMaximum(dMax);
        tMultiGraphA->GetYaxis()->CenterTitle();
        tMultiGraphA->GetXaxis()->CenterTitle();
        tMultiGraphA->GetYaxis()->SetTitleOffset(2.2);
        tMultiGraphA->Draw("a fb l pmc plc 3d");
    }
    else 
    {
        TMultiGraph* tMultiGraphF = new TMultiGraph();
        TGraph* tGraph[nGraph];
        for (auto& item : umEventChannelWaves[eventID])
        {
            tGraph[std::stoi(item.first.substr(1, 2))] = new TGraph(cADCdata);
            for (int j = 0; j < cADCdata; j++)
            {
                //std::cout << "pvADCdata->at(" << j << "): " << pvADCdata->at(j) << std::endl;
                tGraph[std::stoi(item.first.substr(1, 2))]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(item.second[j])/2048.0*1000.0);
            }
            tGraph[std::stoi(item.first.substr(1, 2))]->SetLineWidth(1);
            tGraph[std::stoi(item.first.substr(1, 2))]->SetTitle(item.first.c_str());

            tMultiGraphF->Add(tGraph[std::stoi(item.first.substr(1, 2))]);
        }
        tMultiGraphF->SetTitle(("Event " + std::to_string(eventID) + " Full Channel Signals; Time [us]; Amplitude [mV]").c_str());
        tMultiGraphF->GetXaxis()->SetLimits(0, 1024*0.025);
        tMultiGraphF->SetMinimum(0); tMultiGraphF->SetMaximum(2000/2048.0*1000.0);
        tMultiGraphF->GetYaxis()->CenterTitle();
        tMultiGraphF->GetXaxis()->CenterTitle();
        tMultiGraphF->GetYaxis()->SetTitleOffset(2.2);
        tMultiGraphF->Draw("a fb l pmc plc 3d");
    }

    canvasWidget->getCanvas()->Update();
    cFigName = figName;
    //canvasWidget->getCanvas()->Print(figName);
}
void Draw::drawTrack(int eventID, const char* figName)
{
    if (umEventChannelXIndexOfPeak.empty() || umEventChannelYIndexOfPeak.empty())
    {
        std::unordered_map<uint8_t, double[3]>().swap(umMeanAndSigmaOfBase);
        std::unordered_map<int, std::vector<std::vector<uint16_t>>>().swap(umChannelWaves);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeak2PeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelMeanBaseline);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelXWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelYWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelAWaves);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelXIndexOfPeak);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelYIndexOfPeak);
        std::unordered_map<uint32_t, StringIntPair>().swap(umEventMap);
        std::map<uint32_t, uint32_t>().swap(mEventEnergy);

        tTreeReader->Restart();
        TTreeReaderValue<uint32_t> rEventIndex(*tTreeReader, "pEventIndex");
        TTreeReaderValue<uint8_t> rChannelIndex(*tTreeReader, "pChannelIndex");
        //TTreeReaderValue<uint16_t> rIndexOfPeak(*tTreeReader, "pIndexOfPeak");
        TTreeReaderValue<uint16_t> rIndexOfHalfMax(*tTreeReader, "pIndexOfHalfMax");
        TTreeReaderValue<uint16_t> rPeakOfWave(*tTreeReader, "pPeakOfWave");

        currentEntry = 0;
        while (tTreeReader->Next())
        {
            uint32_t pEventIndex = *rEventIndex;
            uint8_t pChannelIndex = *rChannelIndex;
            //uint16_t pIndexOfPeak = *rIndexOfPeak;
            uint16_t pIndexOfHalfMax = *rIndexOfHalfMax;
            uint16_t pPeakOfWave = *rPeakOfWave;
            if (umChannelMap[pChannelIndex][0] == 'X')
            {
                umEventChannelXIndexOfPeak[pEventIndex][umChannelMap[pChannelIndex]] = std::make_pair(pIndexOfHalfMax, pPeakOfWave);
            }
            else if (umChannelMap[pChannelIndex][0] == 'Y')
            {
                umEventChannelYIndexOfPeak[pEventIndex][umChannelMap[pChannelIndex]] = std::make_pair(pIndexOfHalfMax, pPeakOfWave);
            }
            else
            {
                //std::cerr << "Warning: Channel name is not valid!" << std::endl;
            }
            currentEntry++;
            //std::cout << "\r" << "Processing: " << currentEntry << "/" << nEntries << " " << std::flush;
            Progress = static_cast<double>(currentEntry) / nEntries * 100.0;
            emit progressChanged(Progress);
        }
        Progress = 100.0;
        emit progressChanged(Progress);
        //Sleep(300000);
        Progress = 0.0;
        std::cout << std::endl;
    }
    canvasWidget->getCanvas()->cd();
    canvasWidget->getCanvas()->Clear();
    canvasWidget->getCanvas()->SetBorderMode(0);
    canvasWidget->getCanvas()->SetFillColor(0);
    canvasWidget->getCanvas()->SetGrid();

    const Int_t Nx = 2;
    const Int_t Ny = 1;

    Float_t lMargin = 0.05;
    Float_t rMargin = 0.05;
    Float_t bMargin = 0.15;
    Float_t tMargin = 0.15;

    canvasWidget->CanvasPartition(Nx, Ny, lMargin, rMargin, bMargin, tMargin);

    TPad* pad[Nx][Ny];

    canvasWidget->getCanvas()->cd(0);
    //pad[0][0] = (TPad*) canvasWidget->getCanvas()->FindObject(TString::Format("pad_%d_%d",0,0).Data());
    pad[0][0] = canvasWidget->getPad(0, 0);
    pad[0][0]->Draw();
    pad[0][0]->cd();

    TGraph* tGraphX = new TGraph();
    uint16_t minIndex = 1024;
    uint16_t maxIndex = 0;
    for (auto& item : umEventChannelXIndexOfPeak[eventID])
    {
        tGraphX->AddPoint(std::stoi(item.first.substr(1)), item.second.first);
        if (item.second.first < minIndex)
        {
            minIndex = item.second.first;
        }
        if (item.second.first > maxIndex)
        {
            maxIndex = item.second.first;
        }
        //std::cout << "item.first: " << item.first << " item.second.first: " << item.second.first << " item.second.second: " << item.second.second << std::endl;
    }
    tGraphX->SetMarkerStyle(1);
    tGraphX->SetMarkerSize(1);
    tGraphX->SetTitle("X Channel Track; Channel Index; Relative Time");
    if (umEventChannelXIndexOfPeak[eventID].empty())
    {
        TH1F *h = new TH1F("h", "X Channel Track; Channel Index; Relative Time", 60, 0, 60);
        h->SetStats(0);
        h->GetXaxis()->CenterTitle();
        h->Draw();
    }
    else 
    {
        tGraphX->Draw("ALP");
    }
    for (auto& item : umEventChannelXIndexOfPeak[eventID])
    {
        double x = std::stod(item.first.substr(1));
        double y = item.second.first;
        TMarker* tMarker = new TMarker(x, y, 20);
        TLatex* tLatex = new TLatex(x, y, Form("%4.1f", item.second.second/2048.0*1000.0));
        tLatex->SetTextSize(0.020);
        tLatex->SetTextAlign(31);
        tMarker->SetMarkerColor(kRed);
        //tMarker->SetMarkerSize(item.second.second/200.0);
        tMarker->SetMarkerSize(std::sqrt(item.second.second)/10.0);
        //tMarker->SetMarkerSize(std::log10(item.second.second)/2.0);
        tMarker->Draw("PMC");
        tLatex->Draw();
    }

    canvasWidget->getCanvas()->cd(0);
    //pad[1][0] = (TPad*) canvasWidget->getCanvas()->FindObject(TString::Format("pad_%d_%d",1,0).Data());
    pad[1][0] = canvasWidget->getPad(1, 0);
    pad[1][0]->Draw();
    pad[1][0]->cd();
    //canvasWidget->cd(2);
    TGraph* tGraphY = new TGraph();
    for (auto& item : umEventChannelYIndexOfPeak[eventID])
    {
        if (item.second.first < minIndex)
        {
            minIndex = item.second.first;
        }
        if (item.second.first > maxIndex)
        {
            maxIndex = item.second.first;
        }
        tGraphY->AddPoint(std::stoi(item.first.substr(1)), item.second.first);
    }
    tGraphY->SetMarkerStyle(1);
    tGraphY->SetMarkerSize(1);
    tGraphY->SetTitle("Y Channel Track; Channel Index; Relative Time");
    if (umEventChannelYIndexOfPeak[eventID].empty())
    {
        TH1F *h = new TH1F("h", "Y Channel Track; Channel Index; Relative", 60, 0, 60);
        h->SetStats(0);
        h->GetXaxis()->CenterTitle();
        h->Draw();
    }
    else 
    {
        tGraphY->Draw("ALP");
    }
    for (auto& item : umEventChannelYIndexOfPeak[eventID])
    {
        double x = std::stod(item.first.substr(1));
        double y = item.second.first;
        TMarker* tMarker = new TMarker(x, y, 20);
        TLatex* tLatex = new TLatex(x, y, Form("%4.1f", item.second.second/2048.0*1000.0));
        tLatex->SetTextSize(0.020);
        tLatex->SetTextAlign(31);
        tMarker->SetMarkerColor(kRed);
        //tMarker->SetMarkerSize(item.second.second/200.0);
        tMarker->SetMarkerSize(std::sqrt(item.second.second)/10.0);
        //tMarker->SetMarkerSize(std::log10(item.second.second)/2.0);
        tMarker->Draw("PMC");
        tLatex->Draw();
    }

    tGraphX->SetMinimum(minIndex-10); tGraphX->SetMaximum(maxIndex+10);
    tGraphY->SetMinimum(minIndex-10); tGraphY->SetMaximum(maxIndex+10);
    tGraphX->GetXaxis()->SetLimits(-0.5, 60.5);
    tGraphY->GetXaxis()->SetLimits(-0.5, 60.5);
    //tGraphX->GetXaxis()->CenterTitle();
    //tGraphX->GetYaxis()->CenterTitle();
    //tGraphY->GetXaxis()->CenterTitle();
    //tGraphY->GetYaxis()->CenterTitle();

    canvasWidget->getCanvas()->Update();
    cFigName = figName;
}
void Draw::drawMap(const char* figName)
{
    if (umEventMap.empty())
    {
        nEvents = 0;
        std::unordered_map<uint8_t, double[3]>().swap(umMeanAndSigmaOfBase);
        std::unordered_map<int, std::vector<std::vector<uint16_t>>>().swap(umChannelWaves);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeak2PeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelMeanBaseline);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelXWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelYWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelAWaves);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelXIndexOfPeak);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelYIndexOfPeak);
        std::unordered_map<uint32_t, StringIntPair>().swap(umEventMap);
        std::map<uint32_t, uint32_t>().swap(mEventEnergy);

        tTreeReader->Restart();
        TTreeReaderValue<uint32_t> rEventIndex(*tTreeReader, "pEventIndex");
        TTreeReaderValue<uint8_t> rChannelIndex(*tTreeReader, "pChannelIndex");
        //TTreeReaderValue<uint16_t> rIndexOfPeak(*tTreeReader, "pIndexOfPeak");
        TTreeReaderValue<uint16_t> rIndexOfHalfMax(*tTreeReader, "pIndexOfHalfMax");

        currentEntry = 0;
        while (tTreeReader->Next()) {
            uint32_t pEventIndex = *rEventIndex;
            uint8_t pChannelIndex = *rChannelIndex;
            //uint16_t pIndexOfPeak = *rIndexOfPeak;
            uint16_t pIndexOfHalfMax = *rIndexOfHalfMax;

            if (umEventMap.find(pEventIndex) == umEventMap.end())
            {
                if (umChannelMap[pChannelIndex][0] == 'X')
                {
                    umEventMap[pEventIndex].first = std::make_pair(umChannelMap[pChannelIndex], pIndexOfHalfMax);
                }
                else if (umChannelMap[pChannelIndex][0] == 'Y')
                {
                    umEventMap[pEventIndex].second = std::make_pair(umChannelMap[pChannelIndex], pIndexOfHalfMax);
                }
                else
                {
                    //std::cerr << "Warning: Channel name is Anti-coincidence or not valid!" << std::endl;
                }
                nEvents++;
            }
            else  
            {
                if (umChannelMap[pChannelIndex][0] == 'X' && umEventMap[pEventIndex].first.second < pIndexOfHalfMax)
                {
                    umEventMap[pEventIndex].first = std::make_pair(umChannelMap[pChannelIndex], pIndexOfHalfMax);
                }
                else if (umChannelMap[pChannelIndex][0] == 'Y' && umEventMap[pEventIndex].second.second < pIndexOfHalfMax)
                {
                    umEventMap[pEventIndex].second = std::make_pair(umChannelMap[pChannelIndex], pIndexOfHalfMax);
                }
                else
                {
                    //std::cerr << "Warning: Channel name is Anti-coincidence or not valid!" << std::endl;
                }
            }
            currentEntry++;
            //std::cout << "\r" << "Processing: " << currentEntry << "/" << nEntries << " " << std::flush;
            Progress = static_cast<double>(currentEntry)/nEntries*100.0;
            emit progressChanged(Progress);
        }
        Progress = 100.0;
        emit progressChanged(Progress);
        //Sleep(300000);
        Progress = 0.0;
        std::cout << std::endl;
    }

    canvasWidget->getCanvas()->cd();
    canvasWidget->getCanvas()->Clear();
    canvasWidget->getCanvas()->SetBorderMode(0);
    canvasWidget->getCanvas()->SetRightMargin(0.15);
    canvasWidget->getCanvas()->SetFillColor(0);
    canvasWidget->getCanvas()->SetGrid();
//#define BETAMODE 1
#ifdef BETAMODE
#if BETAMODE == 1
    TH2I* h2 = new TH2I("h2", (std::to_string(nEvents) + " Events Hit Map").c_str(), 30, 0, 30, 30, 0, 30);
#endif
#else
    TH2I* h2 = new TH2I("h2", "Event Hit Map", 60, 0, 60, 60, 0, 60);
#endif
    h2->SetStats(0);
    //TPaveText* pt = new TPaveText(0.1, 0.9, 0.9, 0.99);
    //pt->AddText((std::to_string(nEvents) + " Events Hit Map").c_str());
    h2->SetTitle((std::to_string(nEvents) + " Events Hit Map").c_str());
    //h2->SetTitle(pt->GetTitle());
    h2->GetXaxis()->SetTitle("X");
    h2->GetYaxis()->SetTitle("Y");
    h2->GetXaxis()->SetLabelSize(0.11);
    h2->GetYaxis()->SetLabelSize(0.11);
    h2->GetYaxis()->CenterTitle();
    h2->GetXaxis()->CenterTitle();
    for (auto& item : umEventMap)
    {
        if (item.second.first.first != "" && item.second.second.first != "")
        {
#ifdef BETAMODE
#if BETAMODE == 1
            h2->Fill((std::stoi(item.second.first.first.substr(1))/2.), (std::stoi(item.second.second.first.substr(1)))/2.);
#endif
#else
            h2->Fill(std::stoi(item.second.first.first.substr(1)), std::stoi(item.second.second.first.substr(1)));
#endif
        }
    }
    for (int i = 1; i <= h2->GetNbinsX(); ++i) {
        //std::cout << "i: " << i << std::endl;
        for (int j = 1; j <= h2->GetNbinsY(); ++j) {
            Int_t bin = h2->GetBin(i, j);
            if (i == 1 || j == 1 || i == 2 || j == 2 || i == 59 || j == 59 || i == 60 || j == 60)
            {
                h2->SetBinContent(bin, 0);
            }
            if ((j == 56) || (i == 11) || (i == 54))
            {
                h2->SetBinContent(bin, 0);
            }
            if ((i == 56 && j == 56) || (i == 2 && j == 2) || (i == 58 && j == 58))
            {
                h2->SetBinContent(bin, 0);
            }
            h2->AddBinContent(bin);
            //std::cout << h2->GetBinContent(i, j) << " ";
        }
        //std::cout << std::endl;
    }
    gStyle = new TStyle();
    gStyle->SetPalette(kRainBow);
    //h2->Draw("COLZ");
    h2->Draw("LEGO2");
    canvasWidget->getCanvas()->Update();
    cFigName = figName;
}
void Draw::drawSpectrum(const char* figName)
{
    if (mEventEnergy.empty())
    {
        std::unordered_map<uint8_t, double[3]>().swap(umMeanAndSigmaOfBase);
        std::unordered_map<int, std::vector<std::vector<uint16_t>>>().swap(umChannelWaves);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeak2PeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelPeakOfSignal);
        std::unordered_map<int, std::vector<uint16_t>>().swap(umChannelMeanBaseline);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelXWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelYWaves);
        std::unordered_map<uint32_t, std::map<std::string, std::vector<uint16_t>>>().swap(umEventChannelAWaves);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelXIndexOfPeak);
        std::unordered_map<uint32_t, std::map<std::string, IntPair>>().swap(umEventChannelYIndexOfPeak);
        std::unordered_map<uint32_t, StringIntPair>().swap(umEventMap);
        std::map<uint32_t, uint32_t>().swap(mEventEnergy);

        tTreeReader->Restart();
        TTreeReaderValue<uint16_t> rPeakOfWave(*tTreeReader, "pPeakOfWave");
        TTreeReaderValue<uint32_t> rEventIndex(*tTreeReader, "pEventIndex");
        //std::map<uint32_t, uint32_t> mEventEnergy;

        currentEntry = 0;
        while (tTreeReader->Next()) {
            uint16_t pPeakOfWave = *rPeakOfWave;
            uint32_t pEventIndex = *rEventIndex;
            if (mEventEnergy.find(pEventIndex) == mEventEnergy.end())
            {
                mEventEnergy[pEventIndex] = pPeakOfWave;
            }
            else
            {
                mEventEnergy[pEventIndex] += pPeakOfWave;
            }
            currentEntry++;
            //std::cout << "\r" << "Processing: " << currentEntry << "/" << nEntries << " " << std::flush;
            Progress = static_cast<double>(currentEntry)/nEntries*100.0;
            emit progressChanged(Progress);
        }
        Progress = 100.0;
        emit progressChanged(Progress);
        //Sleep(300000);
        Progress = 0.0;
        std::cout << std::endl;
    }
    auto maxEnergy = std::max_element(mEventEnergy.begin(), mEventEnergy.end(), [](const std::pair<uint32_t, uint32_t>& p1, const std::pair<uint32_t, uint32_t>& p2) { return p1.second < p2.second; })->second;
    TH1D* h1 = new TH1D("h1", "Spectrum", std::ceil(static_cast<double>(maxEnergy)/2.048), 0, std::ceil(static_cast<double>(maxEnergy)/2.048));
    TF1* f1 = new TF1("f1", "gaus", 0, std::ceil(static_cast<double>(maxEnergy)/2.048));

    h1->SetStats(0);
    h1->SetTitle("Spectrum; Energy [mV]; Counts");
    h1->GetXaxis()->CenterTitle();  // 将X轴的标题居中
    h1->GetXaxis()->SetTitleSize(0.04);  // 设置X轴标题的大小
    h1->GetXaxis()->SetLabelSize(0.04);  // 设置X轴标签的大小
    h1->GetYaxis()->CenterTitle();  // 将Y轴的标题居中
    h1->GetYaxis()->SetTitleSize(0.04);  // 设置Y轴标题的大小
    h1->GetYaxis()->SetLabelSize(0.04);  // 设置Y轴标签的大小

    // 设置直方图的颜色和样式
    h1->SetFillColor(kWhite);  // 设置填充颜色
    h1->SetLineColor(kBlack);  // 设置线条颜色
    h1->SetLineWidth(2);  // 设置线条宽度
    h1->SetMarkerStyle(20);  // 设置标记样式
    h1->SetMarkerColor(kBlack);  // 设置标记颜色
    h1->SetMarkerSize(1.5);  // 设置标记大小

    for (auto& item : mEventEnergy)
    {
        h1->Fill(static_cast<double>(item.second)/2.048);
    }
    // 使用高斯函数你和能谱
    h1->Fit(f1);
    // 计算FWHM和能量分辨率
    double sigma = f1->GetParameter(2);  // 获取sigma参数
    double FWHM = 2.35482 * sigma;  // 计算FWHM
    double peakEnergy = f1->GetParameter(1);  // 获取峰值能量
    double energyResolution = FWHM / peakEnergy * 100;  // 计算能量分辨率

    // 创建一个文本框
    TPaveText *pt = new TPaveText(0.6, 0.6, 0.9, 0.9, "brNDC");
    pt->SetFillColor(0);
    TText *text = pt->AddText(("Energy Resolution: " + std::to_string(energyResolution) + "%").c_str());
    text->SetTextColor(kRed);
    
    canvasWidget->getCanvas()->cd();
    canvasWidget->getCanvas()->Clear();
    canvasWidget->getCanvas()->SetGrid();

    h1->Draw();
    f1->Draw("same");
    pt->Draw();
    canvasWidget->getCanvas()->Update();
    cFigName = figName;
}
void Draw::saveToCSV(const std::string& fileName, const std::vector<std::vector<int>>& data)
{
    std::ofstream csvFile(fileName, std::ios::out);

    if (!csvFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    // Iterate through rows and columns and write to the file
    for (const auto& row : data) {
        for (const auto& cell : row) {
            csvFile << cell << ",";
        }
        csvFile << "\n";
    }

    csvFile.close();
    std::cout << "Data saved to " << fileName << std::endl;
}
void Draw::printCanvas()
{
    //canvasWidget->getCanvas()->Print(cFigName.c_str());
    //canvasWidget->getCanvas()->SaveAs(cFigName.c_str());
    canvas = new TCanvas("c", "c", 800, 600);
    canvasWidget->getCanvas()->DrawClonePad();
    //canvas->Print(cFigName.c_str());
    canvas->SaveAs(cFigName.c_str());
}
void Draw::printCanvas(const char* figName)
{
    //canvasWidget->getCanvas()->Print(figName);
    //canvasWidget->getCanvas()->SaveAs(figName);
    canvas = new TCanvas("c", "c", 800, 600);
    canvasWidget->getCanvas()->DrawClonePad();
    canvas->Print(figName);
}
void Draw::drawTest(TCanvasWidget* canvasWidget)
{
    canvasWidget->getCanvas()->cd();

    //const Int_t Nx = 2;
    //const Int_t Ny = 1;

    //Float_t lMargin = 0.05;
    //Float_t rMargin = 0.05;
    //Float_t bMargin = 0.15;
    //Float_t tMargin = 0.15;

    //canvasWidget->CanvasPartition(Nx, Ny, lMargin, rMargin, bMargin, tMargin);

    //TPad* pad[Nx][Ny];

    //canvasWidget->getCanvas()->cd(0);
    ////pad[0][0] = (TPad*) canvasWidget->getCanvas()->FindObject(TString::Format("pad_%d_%d",0,0).Data());
    //pad[0][0] = canvasWidget->getPad(0, 0);
    //pad[0][0]->Draw();
    //pad[0][0]->cd();

    //canvasWidget->getCanvas()->Divide(2, 2);
    //canvasWidget->getCanvas()->cd(1);
    //TH2F* h1 = new TH2F("h1", "Test", 60, -5, 5, 60, -5, 5);
    //h1->FillRandom("gaus", 10000);
    //h1->GetXaxis()->CenterTitle();
    //h1->GetYaxis()->CenterTitle();
    //h1->Draw("");
    //canvasWidget->getCanvas()->Divide(2, 1);
    //canvasWidget->getCanvas()->cd(1);
    TPad *p1 = new TPad("p1", "p1", 0.0, 0.0, 0.5, 1.0, 0, 0, 0);
    //p1->SetRightMargin(0);
    p1->Draw();
  
    TPad *p2 = new TPad("p2", "p2", 0.5, 0.0, 1.0, 1.0, 0, 0, 0);
    //p2->SetTopMargin(0);
    //p2->SetLeftMargin(0);
    p2->Draw();
    TMultiGraph* tMultiGraphX = new TMultiGraph();
    TGraph* tGraph[60];
    for (int i = 0; i < 60; i++)
    {
        tGraph[i] = new TGraph(100);
        for (int j = 0; j < 100; j++)
        {
            tGraph[i]->SetPoint(j, static_cast<double>(j)*0.025, static_cast<double>(i)/2048.0*1000.0);
        }
        tGraph[i]->SetLineWidth(1);
        tGraph[i]->SetTitle(std::to_string(i).c_str());
        tMultiGraphX->Add(tGraph[i]);
    }
    tMultiGraphX->SetTitle("Event X Signals; Time [us]; Amplitude [mV]");
    tMultiGraphX->GetYaxis()->CenterTitle();
    tMultiGraphX->GetXaxis()->CenterTitle();
    tMultiGraphX->GetYaxis()->SetTitleOffset(2.2);
    p1->cd();
    tMultiGraphX->Draw("a fb l pmc plc 3d");

    TH2F* h2 = new TH2F("h1", "Test", 60, -5, 5, 60, -5, 5);
    TF2 *xyg = new TF2("xyg", "xygaus", -5, 5, -5, 5);
    xyg->SetParameters(1, 0, 2, 0, 2);
    h2->FillRandom("xyg");
    h2->GetXaxis()->CenterTitle();
    h2->GetYaxis()->CenterTitle();
    p2->cd();
    h2->Draw();

    //canvasWidget->getCanvas()->cd(3);
    //TH2F* h3 = new TH2F("h1", "Test", 60, -5, 5, 60, -5, 5);
    //h3->FillRandom("gaus", 10000);
    //h3->GetXaxis()->CenterTitle();
    //h3->GetYaxis()->CenterTitle();
    //h3->Draw("");

    //canvasWidget->getCanvas()->cd(4);
    //TH2F* h4 = new TH2F("h1", "Test", 60, -5, 5, 60, -5, 5);
    //h4->FillRandom("gaus", 10000);
    //h4->GetXaxis()->CenterTitle();
    //h4->GetYaxis()->CenterTitle();
    //h4->Draw("");
    //canvasWidget->getCanvas()->cd(0);
    ////pad[1][0] = (TPad*) canvasWidget->getCanvas()->FindObject(TString::Format("pad_%d_%d",1,0).Data());
    //pad[1][0] = canvasWidget->getPad(1, 0);
    //pad[1][0]->Draw();
    //pad[1][0]->cd();
    //canvasWidget->cd(2);
    canvasWidget->getCanvas()->Update();
}

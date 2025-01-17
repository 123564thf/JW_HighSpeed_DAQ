// Author: Sergey Linev, GSI  13/01/2021

/*************************************************************************
 * Copyright (C) 1995-2021, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TCanvasWidget.h"

#include "TCanvas.h"
#include "TROOT.h"
#include "TClass.h"
#include "TEnv.h"

#include "TWebCanvas.h"

TCanvasWidget::TCanvasWidget(QWidget *parent) : QWidget(parent)
{
   setObjectName( "TCanvasWidget");

   setSizeIncrement( QSize( 100, 100 ) );

   setUpdatesEnabled( true );
   setMouseTracking(true);

   setFocusPolicy( Qt::TabFocus );
   setCursor( Qt::CrossCursor );

   setAcceptDrops(true);

   static int wincnt = 1;

   fCanvas = new TCanvas(kFALSE);
   fCanvas->SetName(Form("Canvas%d", wincnt++));
   fCanvas->SetTitle("title");
   fCanvas->ResetBit(TCanvas::kShowEditor);
   fCanvas->SetCanvas(fCanvas);
   fCanvas->SetBatch(kTRUE); // mark canvas as batch

   gPad = fCanvas;

   bool full_functional = gEnv->GetValue("WebGui.FullCanvas", (Int_t) 1) == 1;

   TWebCanvas *web = new TWebCanvas(fCanvas, "title", 0, 0, 800, 600, full_functional);

   fCanvas->SetCanvasImp(web);

   SetPrivateCanvasFields(true);

   web->SetCanCreateObjects(kFALSE); // not yet create objects on server side

   web->SetUpdatedHandler([this]() { emit CanvasUpdated(); });

   web->SetActivePadChangedHandler([this](TPad *pad){ emit SelectedPadChanged(pad); });

   web->SetPadClickedHandler([this](TPad *pad, int x, int y) { emit PadClicked(pad,x,y); });

   web->SetPadDblClickedHandler([this](TPad *pad, int x, int y) { emit PadDblClicked(pad,x,y); });

   auto where = ROOT::RWebDisplayArgs::GetQt5EmbedQualifier(this, "noopenui", QT_VERSION);

   web->ShowWebWindow(where);

   fView = findChild<QWebEngineView*>("RootWebView");
   if (!fView) {
      printf("FAIL TO FIND QWebEngineView - ROOT Qt5Web plugin does not work properly !!!!!\n");
      exit(11);
   }

   fView->resize(width(), height());
   fCanvas->SetCanvasSize(width(), height());
}

TCanvasWidget::~TCanvasWidget()
{
   if (fCanvas) {
      SetPrivateCanvasFields(false);

      gROOT->GetListOfCanvases()->Remove(fCanvas);

      fCanvas->Close();
      delete fCanvas;
      fCanvas = nullptr;
   }
}

void TCanvasWidget::SetPrivateCanvasFields(bool on_init)
{
   Long_t offset = TCanvas::Class()->GetDataMemberOffset("fCanvasID");
   if (offset > 0) {
      Int_t *id = (Int_t *)((char*) fCanvas + offset);
      if (*id == fCanvas->GetCanvasID()) *id = on_init ? 111222333 : -1;
   } else {
      printf("ERROR: Cannot modify fCanvasID data member\n");
   }

   offset = TCanvas::Class()->GetDataMemberOffset("fMother");
   if (offset > 0) {
      TPad **moth = (TPad **)((char*) fCanvas + offset);
      if (*moth == fCanvas->GetMother()) *moth = on_init ? fCanvas : nullptr;
   } else {
      printf("ERROR: Cannot set fMother data member in canvas\n");
   }
}

void TCanvasWidget::resizeEvent(QResizeEvent *event)
{
   fView->resize(width(), height());
   fCanvas->SetCanvasSize(width(), height());
}

void TCanvasWidget::activateEditor(TPad *pad, TObject *obj)
{
   TWebCanvas *cimp = dynamic_cast<TWebCanvas*> (fCanvas->GetCanvasImp());
   if (cimp) {
      cimp->ShowEditor(kTRUE);
      cimp->ActivateInEditor(pad, obj);
   }
}

void TCanvasWidget::setEditorVisible(bool flag)
{
   TCanvasImp *cimp = fCanvas->GetCanvasImp();
   if (cimp) cimp->ShowEditor(flag);
}

void TCanvasWidget::activateStatusLine()
{
   TCanvasImp *cimp = fCanvas->GetCanvasImp();
   if (cimp) cimp->ShowStatusBar(kTRUE);
}

void TCanvasWidget::CanvasPartition(const Int_t Nx,const Int_t Ny,
                     Float_t lMargin, Float_t rMargin,
                     Float_t bMargin, Float_t tMargin)
{
   if (!fCanvas) return;
 
   // Setup Pad layout:
   Float_t vSpacing = 0.0;
   Float_t vStep  = (1.- bMargin - tMargin - (Ny-1) * vSpacing) / Ny;
 
   Float_t hSpacing = 0.0;
   Float_t hStep  = (1.- lMargin - rMargin - (Nx-1) * hSpacing) / Nx;
 
   Float_t vposd,vposu,vmard,vmaru,vfactor;
   Float_t hposl,hposr,hmarl,hmarr,hfactor;
 
   for (Int_t i=0;i<Nx;i++) {
 
      if (i==0) {
         hposl = 0.0;
         hposr = lMargin + hStep;
         hfactor = hposr-hposl;
         hmarl = lMargin / hfactor;
         hmarr = 0.0;
      } else if (i == Nx-1) {
         hposl = hposr + hSpacing;
         hposr = hposl + hStep + rMargin;
         hfactor = hposr-hposl;
         hmarl = 0.0;
         hmarr = rMargin / (hposr-hposl);
      } else {
         hposl = hposr + hSpacing;
         hposr = hposl + hStep;
         hfactor = hposr-hposl;
         hmarl = 0.0;
         hmarr = 0.0;
      }
 
      for (Int_t j=0;j<Ny;j++) {
 
         if (j==0) {
            vposd = 0.0;
            vposu = bMargin + vStep;
            vfactor = vposu-vposd;
            vmard = bMargin / vfactor;
            vmaru = 0.0;
         } else if (j == Ny-1) {
            vposd = vposu + vSpacing;
            vposu = vposd + vStep + tMargin;
            vfactor = vposu-vposd;
            vmard = 0.0;
            vmaru = tMargin / (vposu-vposd);
         } else {
            vposd = vposu + vSpacing;
            vposu = vposd + vStep;
            vfactor = vposu-vposd;
            vmard = 0.0;
            vmaru = 0.0;
         }
 
         fCanvas->cd(0);
 
         auto name = TString::Format("pad_%d_%d",i,j);
         auto pad = (TPad*) fCanvas->FindObject(name.Data());
         if (pad) delete pad;
         pad = new TPad(name.Data(),"",hposl,vposd,hposr,vposu);
         pad->SetLeftMargin(hmarl);
         pad->SetRightMargin(hmarr);
         pad->SetBottomMargin(vmard);
         pad->SetTopMargin(vmaru);
 
         pad->SetFrameBorderMode(0);
         pad->SetBorderMode(0);
         pad->SetBorderSize(0);
 
         pad->Draw();
      }
   }
}
// Author: Sergey Linev, GSI  13/01/2021

/*************************************************************************
 * Copyright (C) 1995-2021, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef TCanvasWidget_H
#define TCanvasWidget_H

#include <QWidget>
#include <QWebEngineView>
#include <TCanvas.h>

class TCanvas;
class TPad;
class TObject;

class TCanvasWidget : public QWidget {

   Q_OBJECT

public:
   TCanvasWidget(QWidget *parent = nullptr);
   virtual ~TCanvasWidget();

   /// returns canvas shown in the widget
   TCanvas *getCanvas() { return fCanvas; }
   TPad* getPad(const Int_t& xPad, const Int_t& yPad) const { return (TPad*) fCanvas->FindObject(TString::Format("pad_%d_%d",xPad,yPad).Data()); };
   void CanvasPartition(const Int_t Nx = 2,const Int_t Ny = 2,
                 Float_t lMargin = 0.15, Float_t rMargin = 0.05,
                 Float_t bMargin = 0.15, Float_t tMargin = 0.05);

signals:

   void CanvasUpdated();

   void SelectedPadChanged(TPad*);

   void PadClicked(TPad*,int,int);

   void PadDblClicked(TPad*,int,int);

public slots:

   void activateEditor(TPad *pad = nullptr, TObject *obj = nullptr);

   void activateStatusLine();

   void setEditorVisible(bool flag = true);

protected:

   void resizeEvent(QResizeEvent *event) override;

   void SetPrivateCanvasFields(bool on_init);

   QWebEngineView *fView{nullptr};  ///< qt webwidget to show

   TCanvas *fCanvas{nullptr};
};

#endif

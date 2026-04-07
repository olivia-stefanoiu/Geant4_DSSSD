#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TKey.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ─── globals ──────────────────────────────────────────────────────────────────
TFile*              gRootFile   = nullptr;
vector<int>         gStrips;          // strip indices found in the file
int                 gCurrentIdx = 0;  // index into gStrips
TString             gRunTag;          // e.g. "r1081_r1082_r1083"

// ─── GUI class ────────────────────────────────────────────────────────────────
class StripBrowser {
    RQ_OBJECT("StripBrowser")

    TGMainFrame*         fMain;
    TRootEmbeddedCanvas* fEmbCanvas;
    TGLabel*             fLabel;
    TGNumberEntry*       fEntry;
    TGTextButton*        fPrev;
    TGTextButton*        fNext;

public:
    StripBrowser();
    virtual ~StripBrowser() { fMain->Cleanup(); delete fMain; }

    void DrawStrip(int strip);
    void DrawCurrent();
    void PrevStrip();
    void NextStrip();
    void GoToStrip();        // called when user types a number and hits Enter
    void HandleKey(Event_t* ev);
};

// ─── constructor ──────────────────────────────────────────────────────────────
StripBrowser::StripBrowser()
{
    fMain = new TGMainFrame(gClient->GetRoot(), 950, 780);
    fMain->SetWindowName("DSSSD Strip Browser");

    // ── canvas ──────────────────────────────────────────────────────────────
    fEmbCanvas = new TRootEmbeddedCanvas("EmbCanvas", fMain, 940, 680);
    fMain->AddFrame(fEmbCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

    // ── bottom control bar ──────────────────────────────────────────────────
    TGHorizontalFrame* hf = new TGHorizontalFrame(fMain, 940, 60);

    fPrev = new TGTextButton(hf, "  ◀  Prev  ");
    fPrev->Connect("Clicked()", "StripBrowser", this, "PrevStrip()");
    hf->AddFrame(fPrev, new TGLayoutHints(kLHintsCenterY, 10, 5, 5, 5));

    fLabel = new TGLabel(hf, "Strip:");
    hf->AddFrame(fLabel, new TGLayoutHints(kLHintsCenterY, 10, 2, 5, 5));

    fEntry = new TGNumberEntry(hf, 0, 4, -1,
                               TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEANonNegative,
                               TGNumberFormat::kNELLimitMinMax, 0, 63);
    fEntry->Connect("ReturnPressed()", "StripBrowser", this, "GoToStrip()");
    hf->AddFrame(fEntry, new TGLayoutHints(kLHintsCenterY, 2, 10, 5, 5));

    fNext = new TGTextButton(hf, "  Next  ▶  ");
    fNext->Connect("Clicked()", "StripBrowser", this, "NextStrip()");
    hf->AddFrame(fNext, new TGLayoutHints(kLHintsCenterY, 5, 10, 5, 5));

    fMain->AddFrame(hf, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5, 5, 5, 5));

    // ── keyboard: connect key press on the main frame ───────────────────────
    fMain->Connect("ProcessedEvent(Event_t*)", "StripBrowser", this, "HandleKey(Event_t*)");

    fMain->MapSubwindows();
    fMain->Resize(fMain->GetDefaultSize());
    fMain->MapWindow();

    DrawCurrent();
}

// ─── draw a given strip number ───────────────────────────────────────────────
void StripBrowser::DrawStrip(int strip)
{
    TString hName = Form("h_%s_Str_%d", gRunTag.Data(), strip);
    TString fName = Form("f_%s_Str_%d", gRunTag.Data(), strip);

    TH1* h = (TH1*)gRootFile->Get(hName);
    TF1* f = (TF1*)gRootFile->Get(fName);

    TCanvas* c = fEmbCanvas->GetCanvas();
    c->cd();
    c->Clear();

    if (!h) {
        TPad* pad = (TPad*)c;
        pad->cd();
        TLatex* ltx = new TLatex(0.5, 0.5, Form("Strip %d: not found", strip));
        ltx->SetNDC(); ltx->SetTextAlign(22); ltx->SetTextSize(0.05);
        ltx->Draw();
    } else {
        h->SetTitle(Form("Strip %d  —  %s", strip, gRunTag.Data()));
        h->SetLineColor(kBlue+1);
        h->SetLineWidth(2);
        h->Draw("HIST");

        if (f) {
            f->SetLineColor(kRed);
            f->SetLineWidth(2);
            f->Draw("SAME");
        }
    }

    c->Modified();
    c->Update();

    // update the number entry to reflect current strip
    fEntry->SetIntNumber(strip);
}

void StripBrowser::DrawCurrent()
{
    if (gStrips.empty()) return;
    DrawStrip(gStrips[gCurrentIdx]);
}

// ─── navigation ───────────────────────────────────────────────────────────────
void StripBrowser::PrevStrip()
{
    if (gCurrentIdx > 0) { --gCurrentIdx; DrawCurrent(); }
}

void StripBrowser::NextStrip()
{
    if (gCurrentIdx < (int)gStrips.size()-1) { ++gCurrentIdx; DrawCurrent(); }
}

void StripBrowser::GoToStrip()
{
    int requested = (int)fEntry->GetIntNumber();
    // find closest available strip
    for (int i = 0; i < (int)gStrips.size(); ++i) {
        if (gStrips[i] == requested) { gCurrentIdx = i; DrawCurrent(); return; }
    }
    // if exact not found, find nearest
    int best = 0;
    int bestDist = abs(gStrips[0] - requested);
    for (int i = 1; i < (int)gStrips.size(); ++i) {
        int d = abs(gStrips[i] - requested);
        if (d < bestDist) { bestDist = d; best = i; }
    }
    gCurrentIdx = best;
    DrawCurrent();
}

// ─── keyboard handler ─────────────────────────────────────────────────────────
void StripBrowser::HandleKey(Event_t* ev)
{
    if (ev->fType != kGKeyPress) return;
    UInt_t keysym;
    char   buf[20];
    gVirtualX->LookupString(ev, buf, sizeof(buf), keysym);

    if (keysym == kKey_Right || keysym == kKey_n) NextStrip();
    if (keysym == kKey_Left  || keysym == kKey_p) PrevStrip();
}

// ─── helper: collect strip numbers from the file ─────────────────────────────
void collectStrips(TFile* f, const TString& runTag)
{
    gStrips.clear();
    TIter next(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        TString name = key->GetName();
        if (!name.BeginsWith("h_")) continue;
        // name format: h_<runTag>_Str_<N>
        TString prefix = Form("h_%s_Str_", runTag.Data());
        if (!name.BeginsWith(prefix)) continue;
        TString numStr = name(prefix.Length(), name.Length() - prefix.Length());
        if (numStr.IsDigit()) gStrips.push_back(numStr.Atoi());
    }
    sort(gStrips.begin(), gStrips.end());
    // deduplicate (TFile may store multiple cycles)
    gStrips.erase(unique(gStrips.begin(), gStrips.end()), gStrips.end());
}

// ─── entry point ─────────────────────────────────────────────────────────────
void browse_dsssd_strips(const char* fileName   = "dsssd_calib_Str_r1081_r1082_r1083.root",
                         const char* runTag     = "r1081_r1082_r1083")
{
    gRunTag = runTag;

    gRootFile = TFile::Open(fileName, "READ");
    if (!gRootFile || gRootFile->IsZombie()) {
        cerr << "ERROR: cannot open " << fileName << "\n";
        return;
    }

    collectStrips(gRootFile, gRunTag);

    if (gStrips.empty()) {
        cerr << "ERROR: no histograms found for runTag=" << runTag << "\n";
        gRootFile->Close();
        return;
    }

    cout << "Found " << gStrips.size() << " strips: ";
    for (int s : gStrips) cout << s << " ";
    cout << "\n";
    cout << "Navigation: ◀/▶ buttons, Left/Right arrow keys, or type strip number + Enter\n";

    new StripBrowser();   // GUI owns itself; ROOT event loop keeps it alive
}
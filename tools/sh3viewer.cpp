#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <wx/listbox.h>
#include <wx/treectrl.h>
#include <wx/splitter.h>

#include <filesystem>
namespace fs = std::filesystem;
#include <vector>
#include <fstream>

#include <arc/front.h>
#include <mem/front.h>
 
class SH3ViewerApp : public wxApp
{
public:
    bool OnInit() override;
};
 
wxIMPLEMENT_APP(SH3ViewerApp);

class SH3ViewerMainFrame : public wxFrame
{
public:
    SH3ViewerMainFrame();
 
private:
    void OnOpenArc(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnArcTreeChange(wxTreeEvent& event);
    void OnExportVfile(wxCommandEvent& event);

    wxBoxSizer *m_TreeSizer;
    wxTreeCtrl *m_ArcTree;
    wxPanel *m_DrawPanel;
    wxSplitterWindow *m_Splitter;
    wxBitmap m_Texture;
    wxStaticBitmap *m_TexturePanel;
};
 
enum
{
    ID_OpenArc = 1,
    ID_ExportVfile,
    ID_ArcTreeChange,
    ID_OnPaint,
};
 
bool SH3ViewerApp::OnInit()
{
    mem_CreateContext();
    arc_CreateContext();

    SetAppearance(Appearance::Dark);
    SH3ViewerMainFrame *frame = new SH3ViewerMainFrame();
    frame->Show(true);
    return true;
}
 
SH3ViewerMainFrame::SH3ViewerMainFrame()
    : wxFrame(nullptr, wxID_ANY, "Silent Hill 3 Viewer", 
      wxDefaultPosition, wxSize(800, 600)),
      m_ArcTree(nullptr)
{
    SetIcon(wxICON(aaa_sh3viewer_icon));
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_OpenArc, "&Open 'arc.arc'...\tCtrl-O",
                     "You should select the arc.arc file located in your 'data/' directory");
    menuFile->AppendSeparator();
    menuFile->Append(ID_ExportVfile, "Export file");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar( menuBar );

    m_Splitter = new wxSplitterWindow(this, wxID_ANY);

    m_ArcTree = new wxTreeCtrl(m_Splitter, ID_ArcTreeChange);
    m_ArcTree->SetMinSize(wxSize(256, 256));
    auto root = m_ArcTree->AddRoot(_("arc.arc"));

    //auto panel = new wxPanel(this, 3);
    //Apanel->SetMinSize(wxSize(128, 256));
    //m_DrawPanel = new wxPanel(m_Splitter, wxID_ANY, wxDefaultPosition, wxSize(256,256), wxTAB_TRAVERSAL, "View");
    m_TexturePanel = new wxStaticBitmap(m_Splitter, wxID_ANY, wxNullBitmap);

    m_Splitter->SplitVertically(m_ArcTree, m_TexturePanel);

    m_TreeSizer = new wxBoxSizer(wxHORIZONTAL);
    m_TreeSizer->Add(m_ArcTree, wxSizerFlags(0).Expand());
    m_TreeSizer->Add(m_TexturePanel, wxSizerFlags(1).Expand());
    m_Splitter->SetSizerAndFit(m_TreeSizer);
    m_Splitter->Layout();
 
    CreateStatusBar();
    SetStatusText("Please open 'arc.arc' to continue...");

    Bind(wxEVT_TREE_SEL_CHANGED, &SH3ViewerMainFrame::OnArcTreeChange, this, ID_ArcTreeChange);
    Bind(wxEVT_MENU, &SH3ViewerMainFrame::OnOpenArc, this, ID_OpenArc);
    Bind(wxEVT_MENU, &SH3ViewerMainFrame::OnExportVfile, this, ID_ExportVfile);
    Bind(wxEVT_MENU, &SH3ViewerMainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &SH3ViewerMainFrame::OnExit, this, wxID_EXIT);
}

void SH3ViewerMainFrame::OnArcTreeChange(wxTreeEvent& event)
{
    std::cout << "Selection change" << std::endl;
    auto selection = m_ArcTree->GetSelection();
    auto wx_string = m_ArcTree->GetItemText(selection);
    std::string vfile_name = std::string(wx_string.mb_str());
    fs::path vfile_path = vfile_name;
    if (vfile_path.extension().string() == ".tex")
    {
        //wxImage im("heather.bmp", wxBITMAP_TYPE_BMP);

        auto wx_string = m_ArcTree->GetItemText(selection);
        std::string vfile_name = std::string(wx_string.mb_str());

        mem_Chunk vfile_data = arc_LoadVfile(vfile_name.c_str());
        {
            m_Texture = wxBitmap((const char*)vfile_data.data + 0x80, *(uint16_t*)((uint8_t*)vfile_data.data + 0x28), 
                *(uint16_t*)((uint8_t*)vfile_data.data + 0x2A), 32);

            m_TexturePanel->SetBitmap(m_Texture);
        }
        arc_FreeVfile(vfile_data);
        //m_Texture.IsOk()
    }
    this->Refresh();
}
 
void SH3ViewerMainFrame::OnExit(wxCommandEvent& event)
{
    arc_FreeData();
    delete m_ArcTree;
    Close(true);
}
 
void SH3ViewerMainFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo info;
    info.SetIcon(wxIcon(L"sh3viewer_icon_hq", wxICON_DEFAULT_TYPE, 128, 128));
    info.SetName(_("Silent Hill 3 Viewer"));
    info.SetVersion(_("0.1"));
    info.SetDescription(_("View && extract various Silent 3 (PC)'s data files."));
    info.SetCopyright(wxT("(C) 2025 kidneyclark <@kidneyclark>"));
    wxAboutBox(info);
}

void SH3ViewerMainFrame::OnExportVfile(wxCommandEvent& event)
{
    if (!arc_IsDataLoaded())
        return; // No loaded game data
    auto selection = m_ArcTree->GetSelection();
    if (!selection.IsOk())
        return; // No selection
    auto wx_selec_str = m_ArcTree->GetItemText(selection);
    std::string selec_str = std::string(wx_selec_str.mb_str());
    if (!m_ArcTree->GetItemParent(selection).IsOk())
        return; // Selecting arc.arc
    auto selection_parent = m_ArcTree->GetItemParent(selection);
    if (!m_ArcTree->GetItemParent(selection_parent).IsOk()) // Selecting whole cluster
    { 
        arc_Cluster cluster = arc_GetCluster(selec_str.c_str());
        auto vfiles = arc_GetVfiles();
        for (int i = 0; i < vfiles.count; i++)
        {
            if (vfiles[i].cluster_id != cluster.id)
                continue;
            std::string vfile_name = vfiles[i].name;
            fs::path vfile_path = vfile_name;
            fs::create_directories(vfile_path.parent_path());

            mem_Chunk vfile_data = arc_LoadVfile(vfiles[i].name);
            {
                std::ofstream out_file(vfile_path, std::ios_base::binary);
                out_file.write((const char*)vfile_data.data, vfile_data.size);
                out_file.close();
                std::cout << "Exporting: " << vfile_path << std::endl;
            }
            arc_FreeVfile(vfile_data);
        }

        return;
    }
        // Selecting specific file
    fs::path vfile_path = selec_str;
    fs::create_directories(vfile_path.parent_path());

    mem_Chunk vfile_data = arc_LoadVfile(selec_str.c_str());
    {
        std::ofstream out_file(vfile_path, std::ios_base::binary);
        out_file.write((const char*)vfile_data.data, vfile_data.size);
        out_file.close();
        std::cout << "Exporting: " << vfile_path << std::endl;
    }
    arc_FreeVfile(vfile_data);
}
 
void SH3ViewerMainFrame::OnOpenArc(wxCommandEvent& event)
{
    wxFileDialog
    openFileDialog(this, _("Open arc.arc file"), "", "",
                    "ARC files (*.arc)|*.arc", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;
    fs::path data_path(openFileDialog.GetPath().mb_str().data());
    arc_SetDataDirectory(data_path.parent_path().string().c_str());
    arc_LoadData();
    auto clusters = arc_GetClusters();
    auto vfiles = arc_GetVfiles();
    for (int i = 0; i < clusters.count; i++)
    {
        std::cout << clusters[i].name << std::endl;
        
        m_ArcTree->AppendItem(m_ArcTree->GetRootItem(), wxString(clusters[i].name, wxConvUTF8));
    }
    for (int i = 0; i < vfiles.count; i++)
    {
        std::string vfile_name = vfiles[i].name;
        //std::cout << vfile_name << std::endl;
        auto root = m_ArcTree->GetRootItem();
        wxTreeItemIdValue cookie;
        auto child = m_ArcTree->GetFirstChild(root, cookie);
        for (int j = 0; j < vfiles[i].cluster_id; j++)
            child = m_ArcTree->GetNextChild(child, cookie);
        auto item = m_ArcTree->AppendItem(child, wxString(vfile_name.c_str(), wxConvUTF8));
    }
    wxLogMessage("Found file! %i clusters.", (int)clusters.count);
}
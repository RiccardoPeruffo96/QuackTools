/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file file_operations.hpp
 * @brief Simple gui implementation
 * using wxWidgets
 * @version 0.0.1 alpha [exam]
 */

#ifndef MAIN_HEADER_QUACKTOOLS_PROJECT_HPP_
#define MAIN_HEADER_QUACKTOOLS_PROJECT_HPP_

  #include <filesystem> //std::filesystem::path
  #include <fstream> //std::fstream
  #include <iostream> //std::cout
  #include <list> //std::list
  #include <string> //std::string

  #include "aes256.hpp"
  #include "base64.hpp"
  #include "diffie_hellman.hpp"
  #include "file_operations.hpp"
  #include "log_operations.hpp"
  #include "sha256.hpp"
  #include "steganography.hpp"

  #include <jsoncpp/json/json.h> //Json::Value

  #include <wx/wxprec.h> //wxWidgets

  #ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/secretstore.h>
    #include <wx/simplebook.h>
  #endif

  #ifndef wxHAS_IMAGES_IN_RESOURCES
      #include "../../data/media/duck.xpm"
  #endif

  #define GREEN_BACKGROUND wxColor(0x30, 0xBA, 0x30)
  #define BLACK_BACKGROUND wxColor(0x10, 0x14, 0x10)
  #define TEXT_COLOR_ON_BLACK_BACKGROUND wxColor(0xAA, 0xAF, 0xAA)

  const uint64_t SIZE_IN_LBL_WDT = 150;
  const uint64_t SIZE_IN_LBL_HGT = 25;
  const uint64_t SIZE_BTN_HGT = 25;

  const uint64_t NUM_PANELS = 7;
  const uint64_t NUM_FULL_PANELS = 3;

  const uint64_t PWD_MIN_L = 8;
  const uint64_t PWD_MAX_L = 100;
  const uint64_t NUMBER_FILE_RECOVERY = 5;

  const uint32_t BITS256 = 256/4;

  const char* ERROR_FILE_NOT_OPEN = "ERROR: you cannot open this file for some reason (example: has the file been deleted or is it in use by other processes?)";
  const char* WARNING_IMG_NEEDED = "You need to select an image to run this function";
  const char* WARNING_FILE_NEEDED = "You need to select a file to run this function";
  const char* WARNING_NAME_NEEDED = "The file to save need a new name";
  const char* ERROR_FILE_NOT_SAVED = "I can't save the new file";
  const char* SUCCESS_FILE_SAVED = "Operation complete";
  const char*& SUCCESS_OPERATION = SUCCESS_FILE_SAVED;

  const char* SUBFOLDER_NOT_EXISTS = "The subfolder for this user\ndoesn't exists in /users\nBut it should.";
  const char* FATAL_ERROR = "Fatal error";

  const char* WARN_LABEL = "Warning";
  const char* SUCC_LABEL = "Success";
  const char* USER_FIELD = "user";
  const char* ERROR_WARN = "User or password\nnot correct";
  const char* UNCORRECT_PASSWORD = "Uncorrect password";
  const char* USER_NOT_UNIQUE = "The chosen username\nhas already been taken";
  const char* ALREADY_HOMEPAGE = "You are already on homepage";

  const char* IMG_EXTENSION = ".png";

  enum wxWindow_ids
  {
    wlcm_idpanel = 100, //id welcome panel
    wlcm_txtctrl_user,
    wlcm_txtctrl_pwd,

    add_user_idpanel, //id add user panel
    add_user_txtctrl_user,
    add_user_txtctrl_pwd,

    recovery_pwd_idpanel, //id recovery pwd panel
    recovery_pwd_txtctrl_user,
    recovery_pwd_txtctrl_pwd,
    recovery_pwd_stctxt_counter,

    login_successfully_idpanel, //id after login panel

    sha256_idpanel, //id panel
    sha256_cbox_fromfile, //id cbox input is from file
    sha256_cbox_eachline, //id cbox every line is a different input
    sha256_stctxt_choose_file, //id text name input file
    sha256_txtctrl_input, //id
    sha256_txtctrl_output,

    b64_idpanel,
    b64_radbtn_enc,
    b64_radbtn_dec,
    b64_cbox_fromfile,
    b64_cbox_eachline,
    b64_stctxt_choose_file,
    b64_txtctrl_input,
    b64_txtctrl_output,
    b64_combobox_standard,

    hide_idpanel,
    hide_txtctrl_outname,
    hide_stctxt_choose_file,
    hide_stctxt_choose_img,
    hide_cbox_enc, //id enable encrypt function
    hide_cbox_custom_pwd,
    hide_txtctrl_pwd, //id password insert

    seek_idpanel,
    seek_txtctrl_outname,
    seek_stctxt_choose_img,
    seek_cbox_dec, //id enable decrypt function
    seek_cbox_custom_pwd,
    seek_txtctrl_pwd, //id password insert

    aes256_idpanel,
    aes256_txtctrl_outname,
    aes256_cbox_enc, //id enable custom pwd
    aes256_txtctrl_pwd, //id password insert
    aes256_stctxt_choose_file,
    aes256_radio_opt,
    aes256_radio_enc,
    aes256_radio_dec,

    dfhl_idpanel,
    dfhl_txtctrl_pub_input,
    dfhl_txtctrl_priv_input,
    dfhl_txtctrl_output,
    dfhl_radio_first,
    dfhl_radio_second,

    personal_idpanel,
    personal_txtctrl_usr, //id user
    personal_txtctrl_pwd_old, //id password actual
    personal_txtctrl_pwd_new, //id password new
    personal_txtctrl_pwd_check, //id reinsert password new

    //ID BUTTONS START
    id_button_back,

    id_button_new_user,
    id_button_enter_login,
    id_button_add_user,
    id_button_rec_pwd,

    id_button_open_file,
    id_button_open_img,
    id_button_run_recovery,
    id_button_calc_sha256,
    id_button_calc_b64,
    id_button_calc_hide,
    id_button_calc_seek,
    id_button_calc_aes256,
    id_button_calc_dfhl,
    id_button_change_personal,
    id_button_recovery_personal
    //ID BUTTONS END
  };

  class MyApp : public wxApp
  {
  public:
      virtual bool OnInit();
  };

  class MyFrame : public wxFrame
  {
  public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

  private:

    std::filesystem::path user_datas{"users"};

    wxSecretString local_hash_pwd;

    wxString stcstr_rec_pwd;
    wxString stcstr_sha256;
    wxString stcstr_b64;
    wxString stcstr_aes256;
    wxString stcstr_seek_img;
    wxString stcstr_hide_file;
    wxString stcstr_hide_img;

    std::fstream user_json;
    Json::Value file_json;

    wxBoxSizer* boxer_fullsize;
    int64_t _username_in_use;
    int64_t _password_in_use;

    void button_back(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void about_me(wxCommandEvent& event);
    //search first avaible file in current folder
    uint64_t check_non_exists_file(std::string name, std::string extension);

    /**
     * @author Peruffo Riccardo
     * @brief return true if input_key it's has a valid format like hex string
     * @param input_key the key
     * @param size_key expect size
     */
    bool string_is_hex_key(std::string& input_key, const uint32_t size_key);
    int32_t ctrl_user_at_start();

    wxPanel* wlcm_panel_cr();
    wxPanel* add_user_panel_cr();
    wxPanel* recovery_pwd_panel_cr();
    wxPanel* login_successfully_panel_cr();

    wxPanel* sha256_panel_cr(wxWindow* parent = nullptr);
    wxPanel* hide_panel_cr(wxWindow* parent = nullptr);
    wxPanel* seek_panel_cr(wxWindow* parent = nullptr);
    wxPanel* aes256_panel_cr(wxWindow* parent = nullptr);
    wxPanel* b64_panel_cr(wxWindow* parent = nullptr);
    wxPanel* dfhl_panel_cr(wxWindow* parent = nullptr);
    wxPanel* personal_panel_cr(wxWindow* parent = nullptr);

    void switch_panel(int64_t id_new_panel);
    void clear_pwd_label();
    //void save_new_user(std::string& user_value, std::string& scrt_value, char* pwd, uint64_t pwd_len);
    void save_new_user(std::string& user_value, std::string& scrt_value);

    void button_login(wxCommandEvent& event);

    void button_new_user(wxCommandEvent& event);
    void button_enter_login(wxCommandEvent& event);
    void button_add_user(wxCommandEvent& event);
    void button_rec_pwd(wxCommandEvent& event);

    void button_open_file(wxCommandEvent& event);
    void button_open_img(wxCommandEvent& event);
    void button_run_recovery(wxCommandEvent& event);
    void button_calc_sha256(wxCommandEvent& event);
    void button_calc_b64(wxCommandEvent& event);
    void button_calc_hide(wxCommandEvent& event);
    void button_calc_seek(wxCommandEvent& event);
    void button_calc_aes256(wxCommandEvent& event);
    void button_calc_dfhl(wxCommandEvent& event);
    void button_change_personal(wxCommandEvent& event);
    void button_recovery_personal(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
  };

  //start macro table
  wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(id_button_back, MyFrame::button_back)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::about_me)

    EVT_BUTTON(id_button_new_user, MyFrame::button_new_user)
    EVT_BUTTON(id_button_enter_login, MyFrame::button_enter_login)
    EVT_BUTTON(id_button_add_user, MyFrame::button_add_user)
    EVT_BUTTON(id_button_rec_pwd, MyFrame::button_rec_pwd)

    EVT_BUTTON(id_button_open_file, MyFrame::button_open_file)
    EVT_BUTTON(id_button_open_img, MyFrame::button_open_img)
    EVT_BUTTON(id_button_run_recovery, MyFrame::button_run_recovery)
    EVT_BUTTON(id_button_calc_sha256, MyFrame::button_calc_sha256)
    EVT_BUTTON(id_button_calc_b64, MyFrame::button_calc_b64)
    EVT_BUTTON(id_button_calc_hide, MyFrame::button_calc_hide)
    EVT_BUTTON(id_button_calc_seek, MyFrame::button_calc_seek)
    EVT_BUTTON(id_button_calc_aes256, MyFrame::button_calc_aes256)
    EVT_BUTTON(id_button_calc_dfhl, MyFrame::button_calc_dfhl)
    EVT_BUTTON(id_button_change_personal, MyFrame::button_change_personal)
    EVT_BUTTON(id_button_recovery_personal, MyFrame::button_recovery_personal)

  wxEND_EVENT_TABLE()
    wxIMPLEMENT_APP(MyApp); //entry point
  //end macro table
  //...until here


#endif //MAIN_HEADER_QUACKTOOLS_PROJECT_HPP_

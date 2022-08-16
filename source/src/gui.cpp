/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file gui.cpp
 * @brief this file contains the definitions of gui.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "gui.hpp"

bool MyApp::OnInit()
{
  MyFrame* frame = new MyFrame("QuackTools", wxPoint(0, 0), wxSize(720, 400));
  frame->Show(true);
  return true;
} //bool MyApp::OnInit()

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
  this->one_or_more_user = true;
  int32_t control_user = ctrl_user_at_start();

  //check if i can use users.json
  if(control_user < 0)
  {
    std::cout << "users.json is blocked\n";
    return;
  }

  SetIcon(wxICON(duck));
  // ----------------------------------------------------------------------------
  // SET MENU BAR
  // ----------------------------------------------------------------------------
  wxMenu* menu_file = new wxMenu;

    menu_file->Append(id_button_back, "&Back home\tCtrl-A",
                     "Return to previous window");
    menu_file->AppendSeparator(); //come unica opzione ci collega una finestra
    menu_file->Append(wxID_EXIT); //come unica opzione ci collega una finestra

  wxMenu* menu_help = new wxMenu; //crea un menu a tendina
    //menu_help->Append(wxID_ABOUT); //come unica opzione ci collega una finestra
    menu_help->Append(wxID_ABOUT,
      "&About\tCtrl-H", //Nome della voce nel menu +tab+ Shortcut da tastiera
      "Can I help you?"); //messaggio a piè di pagina

  wxMenuBar* menuBar = new wxMenuBar; //crea l'oggetto barra del menu

  menuBar->Append(menu_file, "&File"); // setta l'opzione di menu a tendina menu_file, precedentemente settato con il nome "File"
  menuBar->Append(menu_help, "&Help"); // setta l'opzione di menu a tendina menu_help, precedentemente settato con il nome "Help"

  menuBar->SetBackgroundColour(GREEN_BACKGROUND);
  SetMenuBar(menuBar); // aggancia la barra di menu alla finestra che crea

  // ----------------------------------------------------------------------------
  // SET STATUS BAR
  // ----------------------------------------------------------------------------

  wxStatusBar* status_bar = new wxStatusBar(
    this,
    wxID_ANY,
    wxSTB_DEFAULT_STYLE,
    "status_bar"
  );
  status_bar->SetBackgroundColour(GREEN_BACKGROUND);
  status_bar->SetStatusText("Welcome to QuackTools!"); //testo nella barra a piè di pagina
  SetStatusBar(status_bar);

  // ----------------------------------------------------------------------------
  // SET ACTIVITY PANEL
  // ----------------------------------------------------------------------------

  //declare user/pwd label where i can insert credentials
  this->_username_in_use = 0;
  this->_password_in_use = 0;

  //I declare every panel that I need
  wxPanel* p_wlcm = wlcm_panel_cr();
  wxPanel* p_new_user = add_user_panel_cr();
  wxPanel* p_recovery_pwd = recovery_pwd_panel_cr();
  wxPanel* p_login_successfully = login_successfully_panel_cr();

  //control if i have already a user
  if(control_user == 0)
  {
    p_wlcm->Show(true);
    p_new_user->Show(false);
    this->_username_in_use = wlcm_txtctrl_user;
    this->_password_in_use = wlcm_txtctrl_pwd;
  }
  else
  {
    p_wlcm->Show(false);
    p_new_user->Show(true);
    this->_username_in_use = add_user_txtctrl_user;
    this->_password_in_use = add_user_txtctrl_pwd;
  }

  //every other panel has to be hide
  p_recovery_pwd->Show(false);
  p_login_successfully->Show(false);

  this->boxer_fullsize = new wxBoxSizer(wxVERTICAL);

  this->boxer_fullsize->Add(p_wlcm, 1, wxEXPAND);
  this->boxer_fullsize->Add(p_new_user, 1, wxEXPAND);
  this->boxer_fullsize->Add(p_recovery_pwd, 1, wxEXPAND);
  this->boxer_fullsize->Add(p_login_successfully, 1, wxEXPAND);

  this->SetSizer(this->boxer_fullsize);

  stcstr_rec_pwd = wxString();
  stcstr_sha256 = wxString();
  stcstr_b64 = wxString();
  stcstr_aes256 = wxString();
  stcstr_seek_img = wxString();
  stcstr_hide_file = wxString();
  stcstr_hide_img = wxString();
} //MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)

// ----------------------------------------------------------------------------
// AUXILIARY FUNCTIONS
// ----------------------------------------------------------------------------

bool MyFrame::string_is_hex_key(std::string& input_key, const uint32_t size_key)
{
  //input_key.clear();
  //std::cin >> input_key;
  if(input_key.length() != size_key)
  {
    //std::cout << "input length is not equals to " << size_key << "\n";
    return false;
  }
  if (!std::all_of(input_key.begin(), input_key.end(), ::isxdigit))
  {
    //std::cout << input_key << " doesn't contains only hexadecimal digits\n";
    return false;
  }
  return true;
} //bool MyFrame::string_is_hex_key(std::string& input_key, const uint32_t size_key)

int32_t MyFrame::ctrl_user_at_start()
{
  __LOG_HEADER(__LOG_GET) << "open the program\n"; //save it in log file

  //check if users directory exists
  //const std::filesystem::path user_datas{"users"};
  bool user_exixs = std::filesystem::exists(this->user_datas);
  if(user_exixs == false) //users folder doesn't exists
  {
    __LOG_HEADER(__LOG_GET) << "users folder doesn't found\n"; //save it in log file
    std::filesystem::create_directory(this->user_datas); //and create the folder
    this->one_or_more_user = false;
  }

  std::filesystem::current_path(this->user_datas); //change directory in /users/
  this->user_datas = std::filesystem::current_path();

  //check if users.json exists
  const char* json_name_file = "users.json";
  const char* start_json = "\n[]\n";
  const std::filesystem::path user_json_path{json_name_file};
  bool user_j_exixs = std::filesystem::exists(user_json_path);

  if(user_j_exixs == false) //if doesn't exists i force to create it
  {
    user_json.open(json_name_file, std::ifstream::in | std::ifstream::out | std::ifstream::trunc);
    user_json << start_json;
    user_json.close();
    this->one_or_more_user = false;
  }

  user_json.open(json_name_file, std::ifstream::in);

  //if user.json it's blocked, i turn off the program
	if(user_json.is_open() == false)
	{
    this->one_or_more_user = false;
		__LOG_HEADER(__LOG_GET) << "I can't open user.json for some reason, maybe an third process?\n";
		return -1;
	}

  Json::Reader reader;
  //reader.parse(file_json, user_json);
  if(reader.parse(user_json, file_json, true) == false)
  {
    this->one_or_more_user = false;
    //there are file corruptions of json file
    __LOG_HEADER(__LOG_GET) << "I can't open user.json for some reason, maybe an users/users.json corruptions file?\n";
		return -1;
  }
  user_json.close();

  //if the are no elements, return 1, so the main panel is "new_user"
  if(file_json.size() == 0)
  {
    this->one_or_more_user = false;
    return 1;
  }

  //if user_exixs OR user_j_exixs are false, return 1, else 0
  return ((user_exixs == false || user_j_exixs == false) ? 1 : 0);
} //int32_t MyFrame::ctrl_user_at_start()

// ----------------------------------------------------------------------------
// BUTTONS MENU
// ----------------------------------------------------------------------------

void MyFrame::OnExit(wxCommandEvent& event)
{
  Close(true); //
  event.Skip();
} //void MyFrame::OnExit(wxCommandEvent& event)

void MyFrame::about_me(wxCommandEvent& event)
{
  wxMessageBox(wxT("author email: riccardoperuffo96@gmail.com\nlicense: GNU GENERAL PUBLIC LICENSE v3\nhttps://www.gnu.org/licenses/gpl-3.0.html"), //text
              wxT("Contact me"), //title
              wxOK | wxICON_INFORMATION); //type of message
  event.Skip();
} //void MyFrame::about_me(wxCommandEvent& event)

void MyFrame::button_back(wxCommandEvent& event)
{
  if(this->one_or_more_user == false)
  {
    wxMessageBox(ZERO_USERS, //text
                FATAL_ERROR, //title
                wxOK | wxICON_INFORMATION); //type of message
    event.Skip();
    return;
  }

  if (FindWindowById(wlcm_idpanel)->IsShown() == false)
  {
    switch_panel(wlcm_idpanel);
    event.Skip();
    return;
  }

  wxMessageBox(ALREADY_HOMEPAGE, //text
              WARN_LABEL, //title
              wxOK | wxICON_INFORMATION); //type of message

  event.Skip();
  return;
} //void MyFrame::button_back(wxCommandEvent& event)

uint64_t MyFrame::check_non_exists_file(const std::string& name, const std::string& extension)
{
  uint64_t counter = 0;
  std::filesystem::path hypotetical_new_file{name};

  do
  {
    hypotetical_new_file.clear();

    std::string tmp_string = std::string(name);
    tmp_string += std::to_string(++counter);
    tmp_string += extension;

    hypotetical_new_file += tmp_string.c_str();
  } //i have found a non-used name
  while(std::filesystem::exists(hypotetical_new_file));

  return counter;
} //uint64_t MyFrame::check_non_exists_file(std::string name, std::string extension)

// ----------------------------------------------------------------------------
// FUNCTIONS TO SWAP PANELS
// ----------------------------------------------------------------------------

void MyFrame::button_new_user(wxCommandEvent& event)
{
  switch_panel(add_user_idpanel);
  event.Skip();
} //void MyFrame::button_new_user(wxCommandEvent& event)

void MyFrame::button_rec_pwd(wxCommandEvent& event)
{
  switch_panel(recovery_pwd_idpanel);
  event.Skip();
} //void MyFrame::button_rec_pwd(wxCommandEvent& event)

void MyFrame::button_enter_login(wxCommandEvent& event)
{
  wxTextCtrl* this_password = reinterpret_cast<wxTextCtrl*>(FindWindowById(this->_password_in_use));
  wxTextCtrl* this_username = reinterpret_cast<wxTextCtrl*>(FindWindowById(this->_username_in_use));

  /*
  std::cout << "Hey, button button_enter_login > change to login_successfully\n";
  std::cout << this->_username_in_use << "\n";
  std::cout << this->_password_in_use << "\n";
  std::cout << "username = " << this_username->GetValue() << "\n";
  std::cout << "password = " << this_password->GetValue() << "\n";
  */

  uint64_t len = static_cast<uint64_t>(this_password->GetValue().length());
  //overwrite the old var in a dedicated
  wxSecretString pwd(this_password->GetValue());
  //overwrite old value
  this->clear_pwd_label();

  int32_t user_found = -1;
  for (Json::Value::ArrayIndex idx_user = 0; idx_user < file_json.size(); ++idx_user)
  {
    if(file_json[idx_user].isMember(USER_FIELD) == false)
    {
      continue;
    }

    if(file_json[idx_user][USER_FIELD].asString().compare(this_username->GetValue()) == 0) //are equals
    {
      user_found = idx_user;
      idx_user = file_json.size();
    }
  }

  //user not found
  if(user_found < 0)
  {
    wxMessageBox(ERROR_WARN, UNCORRECT_PASSWORD, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  //else
  //i need to calc sha256(concat(this_username->GetValue();";";sha256(pwd)))
  char* text = new char[len];
  for(uint64_t i = 0; i < len; ++i)
  {
    text[i] = pwd[i];
  }

  //step 1: calc sha256(pwd)
  _sha256_rp96_::sha256 hash_pwd = _sha256_rp96_::sha256(text, len);
  hash_pwd.compute_hash();

  for(uint64_t i = 0; i < len; ++i)
  {
    text[i] = '0';
  }
  delete[] text;
  text = nullptr;

  //step 2: concat(this_username->GetValue();";";sha256(pwd))
  uint64_t composite_hash_len = static_cast<uint64_t>(this_username->GetValue().length()) + 1 + hash_pwd.hash_length_bytes(); //username + ";" + hash_pwd
  text = new char[composite_hash_len];
  for(uint64_t i = 0; i < this_username->GetValue().length(); ++i)
  {
    text[i] = this_username->GetValue()[i];
  }

  uint64_t idx_text = this_username->GetValue().length();
  text[idx_text] = ';';

  for(uint64_t i = 0; i < static_cast<uint64_t>(hash_pwd.hash_length_bytes()); ++i)
  {
    text[++idx_text] = hash_pwd.get_SHA256_string()[i];
  }

  //step 3: sha256(concat(this_username->GetValue();";";sha256(pwd)))
  _sha256_rp96_::sha256 composite_hash = _sha256_rp96_::sha256(text, composite_hash_len);
  composite_hash.compute_hash();

  delete[] text;
  text = nullptr;

  //step 4: check if final sha256 is associated at user found
  const char* pwd_field = "secret";
  if(file_json[user_found][pwd_field].asString().compare(composite_hash.get_SHA256_string()) != 0) //are equals
  {
    wxMessageBox(ERROR_WARN, UNCORRECT_PASSWORD, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  //set the user name in personal view
  wxStaticText* txt_user_prs_lbl = reinterpret_cast<wxStaticText*>(FindWindowById(personal_txtctrl_usr));
  txt_user_prs_lbl->SetLabel(this_username->GetValue());

  //save the hash
  this->local_hash_pwd = wxSecretString(hash_pwd.get_SHA256_string());

  //finally change the directory
  std::filesystem::current_path(this->user_datas); //change directory in /users
  //search /$username inside /users
  const std::filesystem::path user_subfolder{this_username->GetValue().ToStdString().c_str()};
  bool user_exixs = std::filesystem::exists(user_subfolder);
  if(user_exixs == false)
  {
    wxMessageBox(FATAL_ERROR, SUBFOLDER_NOT_EXISTS, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }
  std::filesystem::current_path(user_subfolder);

  switch_panel(login_successfully_idpanel);

  event.Skip();
} //void MyFrame::button_enter_login(wxCommandEvent& event)

void MyFrame::button_add_user(wxCommandEvent& event)
{
  //std::cout << "add_user\n";
  //std::cout << "username = " << this_username->GetValue() << "\n";
  //std::cout << "password = " << this_password->GetValue() << "\n";

  std::string error_string("Password doesn't follow\nthe correct pattern:\n");
  wxTextCtrl* this_password = reinterpret_cast<wxTextCtrl*>(FindWindowById(this->_password_in_use));
  wxTextCtrl* this_username = reinterpret_cast<wxTextCtrl*>(FindWindowById(this->_username_in_use));
  uint64_t len = static_cast<uint64_t>(this_password->GetValue().length());

  if(this_username->GetValue().IsEmpty() == true)
  {
    wxMessageBox(USER_EMPTY,
               ERROR_WARN,
               wxOK | wxICON_INFORMATION);

    event.Skip();
    return;
  }

  //username is unique
  //const char* user_field = "user";
  //const char* USER_NOT_UNIQUE = "The chosen username\nhas already been taken";
  auto json_size = file_json.size();
  for (Json::Value::ArrayIndex idx_user = 0; idx_user < json_size; idx_user++)
  {
    if(file_json[idx_user].isMember(USER_FIELD) == false)
    {
      continue;
    }

    if(file_json[idx_user][USER_FIELD].asString().compare(this_username->GetValue()) == 0) //are equals
    {
      wxMessageBox(USER_NOT_UNIQUE,
                 UNCORRECT_PASSWORD,
                 wxOK | wxICON_INFORMATION);

      event.Skip();
      return;
    }
  }

  //password has (length chars >= 8 ^ lower and upper and number ^ one or more symbols)"
  if(len < PWD_MIN_L || len > PWD_MAX_L)
  {
    //std::cout << "wrong password - length\n";
    if(len < PWD_MIN_L)
    {
      error_string += "length less than " + std::to_string(PWD_MIN_L);
    }
    else
    {
      error_string += "length longer than " + std::to_string(PWD_MAX_L);
    }

    error_string += " letters";
    wxMessageBox(error_string.c_str(),
               UNCORRECT_PASSWORD,
               wxOK | wxICON_INFORMATION);

    event.Skip();
    return;
  }

  //overwrite the old var in a dedicated
  wxSecretString pwd(this_password->GetValue());
  //overwrite old value
  this->clear_pwd_label();

  //check every single letter, max pwd length == 100
  bool lower = false, upper = false, number = false, symbol = false;
  for(auto letter : pwd)
  {
    if(letter > 96 && letter < 123)
    {
      lower = true;
      continue;
    }
    else if (letter > 47 && letter < 58)
    {
      number = true;
      continue;
    }
    else if (letter > 64 && letter < 91)
    {
      upper = true;
      continue;
    }
    else
    {
      symbol = true;
    }
  }

  //every check need to be true
  if(!(lower && upper && number && symbol))
  {
    if(!lower)
    {
      error_string += "lowercase letter miss";
    }
    else if(!upper)
    {
      error_string += "uppercase letter miss";
    }
    else if(!number)
    {
      error_string += "number miss";
    }
    else if(!symbol)
    {
      error_string += "symbol miss";
    }

    wxMessageBox(error_string.c_str(),
               UNCORRECT_PASSWORD,
               wxOK | wxICON_INFORMATION);

    event.Skip();
    return;
  }

  //std::cout << "password correct\n";
  char* text = new char[len];
  for(uint64_t i = 0; i < len; ++i)
  {
    text[i] = pwd[i];
  }

  _sha256_rp96_::sha256 hash_pwd = _sha256_rp96_::sha256(text, len);
  hash_pwd.compute_hash();

  for(uint64_t i = 0; i < len; ++i)
  {
    text[i] = '0';
  }
  delete[] text;
  text = nullptr;

  //Qwerty1!
  //195f62edd40a1d9b0c6407104596d6c29e2b408ecc1cc964e803376917ab1fd0

  //std::cout << "hash: " << hash_pwd.get_SHA256_string() << "\n";

  uint64_t composite_hash_len = static_cast<uint64_t>(this_username->GetValue().length()) + 1 + hash_pwd.hash_length_bytes(); //username + ";" + hash_pwd
  text = new char[composite_hash_len];
  for(uint64_t i = 0; i < this_username->GetValue().length(); ++i)
  {
    text[i] = this_username->GetValue()[i];
  }

  uint64_t idx_text = this_username->GetValue().length();
  text[idx_text] = ';';

  for(uint64_t i = 0; i < static_cast<uint64_t>(hash_pwd.hash_length_bytes()); ++i)
  {
    text[++idx_text] = hash_pwd.get_SHA256_string()[i];
  }

  _sha256_rp96_::sha256 composite_hash = _sha256_rp96_::sha256(text, composite_hash_len);
  composite_hash.compute_hash();
  //std::cout << "hash composite: " << composite_hash.get_SHA256_string() << "\n";

  delete[] text;
  text = nullptr;

  //save datas
  std::string user_v(this_username->GetValue().utf8_string());
  std::string scrt_v(composite_hash.get_SHA256_string());
  text = new char[len];
  for(uint64_t i = 0; i < len; ++i)
  {
    text[i] = pwd[i];
  }

  this->save_new_user(user_v, scrt_v);//, text, len);

  for(uint64_t i = 0; i < len; ++i)
  {
    text[i] = '0';
  }
  delete[] text;
  text = nullptr;

  this->one_or_more_user = true;

  this->switch_panel(wlcm_idpanel);

  event.Skip();
} //void MyFrame::button_add_user(wxCommandEvent& event)

//void MyFrame::save_new_user(std::string& user_value, std::string& scrt_value, char* pwd, uint64_t pwd_len)
void MyFrame::save_new_user(std::string& user_value, std::string& scrt_value)
{
  //CREATE NEW VALUE AND UPDATE .JSON
  Json::Value new_record;

  new_record["user"] = user_value;
  new_record["secret"] = scrt_value;

  this->file_json.append(new_record);

  user_json.open("users.json", std::ios::out);
  user_json << this->file_json.toStyledString() << std::endl;
  user_json.close();

  //CREATE NEW SUBFOLDER
  const std::filesystem::path user_datas{user_value.c_str()}; //check if users directory exists
  bool user_exixs = std::filesystem::exists(user_datas);
  if(user_exixs == false) //users folder doesn't exists
  {
    __LOG_HEADER(__LOG_GET) << "create new directory /"<< user_value.c_str() << "\n"; //save it in log file
  }
  else
  {
    __LOG_HEADER(__LOG_GET) << "overwrite the directory /"<< user_value.c_str() << "\n"; //save it in log file
    std::filesystem::remove_all(user_datas);
  }
  std::filesystem::create_directory(user_datas); //and create the folder
  __LOG_HEADER(__LOG_GET) << "create user: "<< user_value.c_str() << "\n"; //save it in log file

  //CREATE x5 FILE WHERE I CAN SEEK THE PWD WITH RECOVERY FUNCTION
  /*
  std::filesystem::current_path(user_datas); //change directory in /users/$user

  const char* extension = ".qt";
  const char* name_file = "rec0";
  std::ofstream* rec_file = new std::ofstream[NUMBER_FILE_RECOVERY];
  for(auto i = 0; i < NUMBER_FILE_RECOVERY; ++i)
  {
    std::string name(name_file + std::to_string(i) + extension);
    rec_file[i].open(name.c_str(), std::ios::out | std::ios::trunc);
    rec_file[i].close();

    char* text = new char[pwd_len];
    for(uint64_t i = 0; i < pwd_len; ++i)
    {
      text[i] = pwd[i];
    }

    _sha256_rp96_::sha256 hash_pwd = _sha256_rp96_::sha256(text, pwd_len);
    hash_pwd.compute_hash();

    for(auto i = 0; i < pwd_len; ++i)
    {
      text[i] = '0';
    }
    delete[] text;
    text = nullptr;

    _file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(name);

    //start to encrypt
    uint64_t len_to_save = user_value.length() + 2 + pwd_len + hash_pwd.get_SHA256_string().length();
    uint8_t* value_to_save = new uint8_t[len_to_save];

    for(uint64_t i = 0; i < user_value.length(); ++i) //save user
    {
      value_to_save[i] = user_value[i];
    }
    value_to_save[user_value.length()] = ';'; //save ;
    for(uint64_t i = 0, j = user_value.length() + 1; i < pwd_len; ++j, ++i) //save pwd
    {
      value_to_save[j] = pwd[i];
    }
    value_to_save[user_value.length() + 1 + pwd_len] = ';'; //save ;
    for(uint64_t i = 0, j = user_value.length() + 2 + pwd_len; i < hash_pwd.get_SHA256_string().length(); ++j, ++i) //save pwd
    {
      value_to_save[j] = hash_pwd.get_SHA256_string()[i];
    }

    std::cout << "value to save: " << value_to_save << "\n";
    //rew;qwe123Q!;0910a55383c969b0ec4614157837203b1487ef163c30732f6648826e85a20e23
    _aes256_rp96_::aes256 aes256 = _aes256_rp96_::aes256(value_to_save, len_to_save, hash_pwd.get_SHA256_string());
    aes256.encrypt();

    //obtain results
    std::list<_file_operations_rp96_::file_pointer> list_file;
    list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_payload(), aes256.get_length_payload()));
    list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_final_block(), aes256.get_final_block_size()));

    //save results in the same file
    if(input_file.save_this_file(list_file, false, std::ofstream::trunc) == -1) //update the empty file
    {
      std::cout << "error with save_this_file\n";
    }
    else
    {
      std::cout << "file " << input_file.get_file_name() << " encrypted\n";
    }

    for(auto i = 0; i < len_to_save; ++i)
    {
      value_to_save[i] = '0';
    }
    delete[] value_to_save;
    value_to_save = nullptr;
  }
  */
} //void MyFrame::save_new_user(std::string& user_value, std::string& scrt_value, char* pwd, uint64_t pwd_len)

void MyFrame::switch_panel(int64_t id_new_panel)
{
  wxPanel* old_panel; //to hide
  wxPanel* new_panel = reinterpret_cast<wxPanel*>(FindWindowById(id_new_panel)); //to show

  if ( FindWindowById(wlcm_idpanel)->IsShown() ) //welcome it's showed
  {
    //clear welcome credentials labels
    this->clear_pwd_label();

    //hide welcome and show new panel
    old_panel = reinterpret_cast<wxPanel*>(FindWindowById(wlcm_idpanel));
    old_panel->Show(false);
    new_panel->Show(true);
  }
  else if ( FindWindowById(add_user_idpanel)->IsShown() )
  {
    //clear add_user credentials labels
    this->clear_pwd_label();

    //hide add_user and show new panel
    old_panel = reinterpret_cast<wxPanel*>(FindWindowById(add_user_idpanel));
    old_panel->Show(false);
    new_panel->Show(true);
  }
  else if ( FindWindowById(recovery_pwd_idpanel)->IsShown() )
  {
    //clear recovery_pwd credentials labels
    this->clear_pwd_label();

    //hide recovery_pwd and show new panel
    old_panel = reinterpret_cast<wxPanel*>(FindWindowById(recovery_pwd_idpanel));
    old_panel->Show(false);
    new_panel->Show(true);
  }
  else if ( FindWindowById(login_successfully_idpanel)->IsShown() )
  {
    //clear login_successfully credentials labels
    this->clear_pwd_label();

    //hide recovery_pwd and show new panel
    old_panel = reinterpret_cast<wxPanel*>(FindWindowById(login_successfully_idpanel));
    old_panel->Show(false);
    new_panel->Show(true);
  }

  //set new user/pwd active field
  switch(id_new_panel)
  {
    //if i want to show welcome then i set welcome credentials labels
    case wlcm_idpanel:
      this->_username_in_use = wlcm_txtctrl_user;
      this->_password_in_use = wlcm_txtctrl_pwd;
      break;
    //if i want to show add user then i set add user credentials labels
    case add_user_idpanel:
      this->_username_in_use = add_user_txtctrl_user;
      this->_password_in_use = add_user_txtctrl_pwd;
      break;
    //if i want to show recovery pwd then i set recovery pwd credentials labels
    case recovery_pwd_idpanel:
      this->_username_in_use = recovery_pwd_txtctrl_user;
      this->_password_in_use = recovery_pwd_txtctrl_pwd;
      break;
    //if i want to show login successfully then i set login successfully credentials labels
    default:
      this->_username_in_use = personal_txtctrl_usr;
      this->_password_in_use = personal_txtctrl_pwd_old;
      break;
  }

  Layout();
} //void MyFrame::switch_panel(int64_t id_new_panel)

void MyFrame::clear_pwd_label()
{
  wxTextCtrl* _pwd = reinterpret_cast<wxTextCtrl*>(FindWindowById(this->_password_in_use));
  _pwd->SetValue("usernameusernameusernameusernameusernameusernameusernameusernameusernameusernameusernameusernameusername");
  _pwd->SetValue("");
} //void MyFrame::clear_pwd_label()

// ----------------------------------------------------------------------------
// FUNCTIONS TO CREATE PANELS
// ----------------------------------------------------------------------------

wxPanel* MyFrame::wlcm_panel_cr()
{
  wxPanel* wlcm_wxpanel = new wxPanel(this, wlcm_idpanel, wxDefaultPosition, wxDefaultSize);
  wlcm_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* wlcm_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = wlcm_wxpanel;

  wxGridSizer* log_grid_sizer = new wxGridSizer( //2, 2, 5, 5
      2, //# rows
      2, //# cols
      5, //# vgap
      5 //# hgap
    );

  wxGridSizer* log_texts = new wxGridSizer(2, 1, 5, 5);
  wxTextCtrl* usr_wlcm = new wxTextCtrl(this_wxpanel, wlcm_txtctrl_user, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT));
  wxTextCtrl* pwd_wlcm = new wxTextCtrl(this_wxpanel, wlcm_txtctrl_pwd, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT), wxTE_PASSWORD);
  usr_wlcm->SetHint(wxT("USERNAME"));
  pwd_wlcm->SetHint(wxT("PASSWORD"));
  log_texts->Add(usr_wlcm, 1, wxALIGN_CENTER);
  log_texts->Add(pwd_wlcm, 1, wxALIGN_CENTER);

  wxStaticText* welcome = new wxStaticText(this_wxpanel, wxID_ANY, wxT("QuackTools Welcome"));
  welcome->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);

  wxGridSizer* btn_grid = new wxGridSizer(2, 1, 5, 5);
  wxButton* newuser_btn = new wxButton(this_wxpanel, id_button_new_user, wxT("NEW USER"), wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT - 5));
  wxButton* recpwd_btn = new wxButton(this_wxpanel, id_button_rec_pwd, wxT("RECOVERY PWD"), wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT - 5));
  btn_grid->Add(newuser_btn, 1, wxALIGN_CENTER);

  btn_grid->Add(recpwd_btn, 1, wxALIGN_CENTER);

  wxButton* enter_btn = new wxButton(this_wxpanel, id_button_enter_login, wxT("ENTER"), wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, (SIZE_IN_LBL_HGT * 2) + 5));

  log_grid_sizer->Add(welcome, 1, wxALIGN_CENTER);
  log_grid_sizer->Add(btn_grid, 1, wxALIGN_CENTER);
  log_grid_sizer->Add(log_texts, 1, wxALIGN_CENTER);
  log_grid_sizer->Add(enter_btn, 1, wxALIGN_CENTER);

  wlcm_wxboxsizer->Add(log_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(wlcm_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return wlcm_wxpanel;
} //wxPanel* MyFrame::wlcm_panel_cr()

wxPanel* MyFrame::add_user_panel_cr()
{
  wxPanel* add_user_wxpanel = new wxPanel(this, add_user_idpanel, wxDefaultPosition, wxDefaultSize);
  add_user_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* add_user_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = add_user_wxpanel;

  wxGridSizer* new_user_grid_sizer = new wxGridSizer(
    1, //# rows
    2, //# cols
    0, //# vgap
    0 //# hgap
  );

  wxGridSizer* new_user_enter = new wxGridSizer(2, 1, 0, 0); //has buttons and param

  wxGridSizer* new_user_params = new wxGridSizer(2, 1, 5, 5); //has only params
  wxTextCtrl* usr_add_usr = new wxTextCtrl(this_wxpanel, add_user_txtctrl_user, wxEmptyString, wxDefaultPosition, wxSize(200, 25));
  wxTextCtrl* pwd_add_usr = new wxTextCtrl(this_wxpanel, add_user_txtctrl_pwd, wxEmptyString, wxDefaultPosition, wxSize(200, 25), wxTE_PASSWORD);
  usr_add_usr->SetHint(wxT("NEW USERNAME"));
  pwd_add_usr->SetHint(wxT("NEW PASSWORD"));
  new_user_params->Add(usr_add_usr, 1, wxALIGN_CENTER);
  new_user_params->Add(pwd_add_usr, 1, wxALIGN_CENTER);

  wxButton* add_user_btn = new wxButton(this_wxpanel, id_button_add_user, wxT("CREATE USER"), wxDefaultPosition, wxSize(200, 75));

  new_user_enter->Add(add_user_btn, 1, wxALIGN_CENTER);
  new_user_enter->Add(new_user_params, 1, wxALIGN_CENTER);

  std::string pwd_pattern("Create a new user:\n");
  pwd_pattern += "user name has to be unique\n";
  pwd_pattern += "password length between 8-100\n";
  pwd_pattern += "password must have a lowercase letter\n";
  pwd_pattern += "password must have a uppercase letter\n";
  pwd_pattern += "password must have a number\n";
  pwd_pattern += "password must have a symbol\n";
  wxStaticText* new_pwd_format = new wxStaticText(this_wxpanel, wxID_ANY,
    pwd_pattern.c_str(),
    wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT
    //wxT("Add new user with pwd pattern:\n1. length chars >= 8;\n2. lower and upper and number;\n3. one or more symbols.")
  );
  new_pwd_format->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);

  new_user_grid_sizer->Add(new_pwd_format, 1, wxALIGN_CENTER);
  new_user_grid_sizer->Add(new_user_enter, 1, wxALIGN_CENTER);

  add_user_wxboxsizer->Add(
      new_user_grid_sizer, 1, wxEXPAND//, wxSizerFlags().Proportion().Expand().Border(wxALL, 10)
  );

  this_wxpanel->SetSizer(add_user_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return add_user_wxpanel;
} //wxPanel* MyFrame::add_user_panel_cr()

wxPanel* MyFrame::recovery_pwd_panel_cr()
{
  wxPanel* rec_pwd_wxpanel = new wxPanel(this, recovery_pwd_idpanel, wxDefaultPosition, wxDefaultSize);
  rec_pwd_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* rec_pwd_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = rec_pwd_wxpanel;

  wxGridSizer* rec_pwd_grid_sizer = new wxGridSizer(2, 2, 0, 0);

  wxGridSizer* rec_pwd_texts = new wxGridSizer(2, 1, 0, 0); //has only texts
  wxStaticText* user_text = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Insert user:"));
  wxStaticText* scrt_word_text = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Insert secret word:"));
  user_text->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  scrt_word_text->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  rec_pwd_texts->Add(user_text, 1, wxALIGN_CENTER);
  rec_pwd_texts->Add(scrt_word_text, 1, wxALIGN_CENTER);

  wxGridSizer* rec_pwd_params = new wxGridSizer(2, 1, 0, 0); //has only params
  wxTextCtrl* usr_rec_pwd = new wxTextCtrl(this_wxpanel, recovery_pwd_txtctrl_user, wxEmptyString, wxDefaultPosition, wxSize(200, 25));
  wxTextCtrl* word_rec_pwd = new wxTextCtrl(this_wxpanel, recovery_pwd_txtctrl_pwd, wxEmptyString, wxDefaultPosition, wxSize(200, 25), wxTE_PASSWORD);
  usr_rec_pwd->SetHint(wxT("USERNAME"));
  word_rec_pwd->SetHint(wxT("SECRET WORD"));
  rec_pwd_params->Add(usr_rec_pwd, 1, wxALIGN_CENTER);
  rec_pwd_params->Add(word_rec_pwd, 1, wxALIGN_CENTER);

  uint64_t recovery_idx = 10;
  std::string pwd_pattern("You can use this function " + std::to_string(recovery_idx) + " times.\n");
  pwd_pattern += "You can reset the counter\nwith a successful login.";
  wxStaticText* text_warning = new wxStaticText(this_wxpanel, wxID_ANY, pwd_pattern.c_str(), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
  text_warning->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);

  wxButton* rec_pwd_btn = new wxButton(this_wxpanel, id_button_run_recovery, wxT("RECOVERY PASSWORD"), wxDefaultPosition, wxSize(200, 75));

  rec_pwd_grid_sizer->Add(rec_pwd_texts, 1, wxALIGN_CENTER);
  rec_pwd_grid_sizer->Add(rec_pwd_params, 1, wxALIGN_CENTER);
  rec_pwd_grid_sizer->Add(text_warning, 1, wxALIGN_CENTER);
  rec_pwd_grid_sizer->Add(rec_pwd_btn, 1, wxALIGN_CENTER);

  rec_pwd_wxboxsizer->Add(rec_pwd_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(rec_pwd_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return rec_pwd_wxpanel;
} //wxPanel* MyFrame::recovery_pwd_panel_cr()

wxPanel* MyFrame::login_successfully_panel_cr()
{
  wxPanel* logsuc_wxpanel = new wxPanel(this, login_successfully_idpanel, wxDefaultPosition, wxDefaultSize);
  logsuc_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* logsuc_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = logsuc_wxpanel;

  //wxNotebook (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxNotebookNameStr)
  wxNotebook* function_menu = new wxNotebook(this_wxpanel, wxID_ANY, wxDefaultPosition, wxSize(540, 300));
  //function_menu->AddPage(wxWindow* page, const wxString& text, bool select = false, int imageId = NO_IMAGE)
  function_menu->AddPage(sha256_panel_cr(function_menu), wxT("SHA256"), true);
  function_menu->AddPage(b64_panel_cr(function_menu), wxT("BASE64"), false);
  function_menu->AddPage(hide_panel_cr(function_menu), wxT("Hide"), false);
  function_menu->AddPage(seek_panel_cr(function_menu), wxT("Seek"), false);
  function_menu->AddPage(aes256_panel_cr(function_menu), wxT("AES256"), false);
  function_menu->AddPage(dfhl_panel_cr(function_menu), wxT("Key gen"), false);
  function_menu->AddPage(personal_panel_cr(function_menu), wxT("User"), false);
  function_menu->Layout();

  logsuc_wxboxsizer->Add(function_menu, 1, wxEXPAND);

  this_wxpanel->SetSizer(logsuc_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return logsuc_wxpanel;
} //wxPanel* MyFrame::login_successfully_panel_cr()

wxPanel* MyFrame::sha256_panel_cr(wxWindow* parent)
{
  wxPanel* sha256_wxpanel;
  if(parent == nullptr)
  {
    sha256_wxpanel = new wxPanel(this, sha256_idpanel, wxDefaultPosition, wxDefaultSize);
  }
  else
  {
    sha256_wxpanel = new wxPanel(parent, sha256_idpanel, wxDefaultPosition, wxDefaultSize);
  }

  sha256_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* sha256_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = sha256_wxpanel;

  wxGridSizer* sha256_grid_sizer = new wxGridSizer( //4, 1, 5, 15
    4, //# rows
    1, //# cols
    5, //# vgap
    15 //# hgap
  );

  wxGridSizer* upper_grid = new wxGridSizer(2, 2, 0, 5);

  //wxCheckBox (wxWindow *parent, wxWindowID id, const wxString &label, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxCheckBoxNameStr)
  wxCheckBox* from_file = new wxCheckBox(this_wxpanel, sha256_cbox_fromfile, wxT("From file"), wxDefaultPosition, wxSize(150, 25));
  from_file->SetValue(false);
  from_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxCheckBox* each_line = new wxCheckBox(this_wxpanel, sha256_cbox_eachline, wxT("Each line it's a\ndifferent input"), wxDefaultPosition, wxSize(150, 40));
  each_line->SetValue(false);
  each_line->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);

  wxButton* choose_file_btn = new wxButton(this_wxpanel, id_button_open_file, wxT("Select file"), wxDefaultPosition, wxSize(150, 20));
  wxStaticText* choose_file = new wxStaticText(this_wxpanel, sha256_stctxt_choose_file, wxT("No file choose"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  choose_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  choose_file->SetFocus();

  upper_grid->Add(from_file, 1, wxALIGN_CENTER);
  upper_grid->Add(choose_file_btn, 0, wxEXPAND);
  upper_grid->Add(each_line, 1, wxALIGN_CENTER);
  upper_grid->Add(choose_file, 0, wxALIGN_CENTER);

  //end first line
  wxTextCtrl* input_sha256 = new wxTextCtrl(this_wxpanel, sha256_txtctrl_input, wxEmptyString, wxDefaultPosition, wxSize(425, 50), wxTE_MULTILINE);
  wxTextCtrl* output_sha256 = new wxTextCtrl(this_wxpanel, sha256_txtctrl_output, wxEmptyString, wxDefaultPosition, wxSize(425, 50), wxTE_MULTILINE | wxTE_READONLY);
  input_sha256->SetHint(wxT("Text to calculate SHA256"));
  output_sha256->SetHint(wxT("Result"));

  wxButton* sha256_btn_start = new wxButton(this_wxpanel, id_button_calc_sha256, wxT("START"), wxDefaultPosition, wxSize(425, 50));

  sha256_grid_sizer->Add(upper_grid, 1, wxEXPAND);
  sha256_grid_sizer->Add(input_sha256, 0, wxEXPAND);
  sha256_grid_sizer->Add(output_sha256, 0, wxEXPAND);
  sha256_grid_sizer->Add(sha256_btn_start, 0, wxEXPAND);

  sha256_wxboxsizer->Add(sha256_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(sha256_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return sha256_wxpanel;
} //wxPanel* MyFrame::sha256_panel_cr(wxWindow* parent)

wxPanel* MyFrame::hide_panel_cr(wxWindow* parent)
{
  /*
  hide_idpanel,
  hide_txtctrl_outname,
  hide_cbox_enc,
  hide_txtctrl_pwd
  */
  wxPanel* hide_wxpanel;
  if(parent == nullptr)
  {
    hide_wxpanel = new wxPanel(this, hide_idpanel, wxDefaultPosition, wxDefaultSize);
  }
  else
  {
    hide_wxpanel = new wxPanel(parent, hide_idpanel, wxDefaultPosition, wxDefaultSize);
  }

  hide_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* hide_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = hide_wxpanel;

  wxGridSizer* hide_grid_sizer = new wxGridSizer( 5, 1, 5, 5);

  wxGridSizer* file_grid = new wxGridSizer(1, 3, 5, 5);
  wxStaticText* txt_choose_file = new wxStaticText(this_wxpanel, wxID_ANY, wxT("File to hide"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  txt_choose_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxButton* choose_file_btn = new wxButton(this_wxpanel, id_button_open_file, wxT("Select file"), wxDefaultPosition, wxSize(150, 20));
  wxStaticText* choose_file = new wxStaticText(this_wxpanel, hide_stctxt_choose_file, wxT("No file choose"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  choose_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  file_grid->Add(txt_choose_file, 1, wxEXPAND);
  file_grid->Add(choose_file_btn, 1, wxEXPAND);
  file_grid->Add(choose_file, 1, wxALIGN_CENTER);

  wxGridSizer* img_grid = new wxGridSizer(1, 3, 5, 5);
  wxStaticText* txt_choose_img = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Img where to hide"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  txt_choose_img->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxButton* choose_img_btn = new wxButton(this_wxpanel, id_button_open_img, wxT("Select image"), wxDefaultPosition, wxSize(150, 20));
  wxStaticText* choose_img = new wxStaticText(this_wxpanel, hide_stctxt_choose_img, wxT("No img choose"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  choose_img->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  img_grid->Add(txt_choose_img, 1, wxEXPAND);
  img_grid->Add(choose_img_btn, 1, wxEXPAND);
  img_grid->Add(choose_img, 1, wxALIGN_CENTER);

  wxGridSizer* encrypt_grid = new wxGridSizer(1, 3, 5, 5);
  wxCheckBox* use_encrypt = new wxCheckBox(this_wxpanel, hide_cbox_enc, wxT("Encrypt file"), wxDefaultPosition, wxSize(150, 40));
  use_encrypt->SetValue(false);
  use_encrypt->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxCheckBox* use_custom_pwd = new wxCheckBox(this_wxpanel, hide_cbox_custom_pwd, wxT("Use custom pwd"), wxDefaultPosition, wxSize(150, 40));
  use_custom_pwd->SetValue(false);
  use_custom_pwd->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxTextCtrl* pwd_hide = new wxTextCtrl(this_wxpanel, hide_txtctrl_pwd, wxEmptyString, wxDefaultPosition, wxSize(200, 40), wxTE_PASSWORD);
  pwd_hide->SetHint(wxT("PASSWORD"));
  encrypt_grid->Add(use_encrypt, 1, wxEXPAND);
  encrypt_grid->Add(use_custom_pwd, 1, wxEXPAND);
  encrypt_grid->Add(pwd_hide, 1, wxEXPAND);

  wxGridSizer* output_grid = new wxGridSizer(1, 2, 5, 5);
  wxStaticText* txt_choose_out = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Name output (you can't\nchange extension *.png)"), wxDefaultPosition, wxSize(200, 40), wxALIGN_CENTRE);
  txt_choose_out->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxTextCtrl* outname_hide = new wxTextCtrl(this_wxpanel, hide_txtctrl_outname, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT));
  outname_hide->SetHint(wxT("NAME OUTPUT"));
  output_grid->Add(txt_choose_out, 1, wxEXPAND);
  output_grid->Add(outname_hide, 1, wxEXPAND);

  wxGridSizer* btn_grid = new wxGridSizer(1, 1, 5, 5);
  wxButton* hide_btn_start = new wxButton(this_wxpanel, id_button_calc_hide, wxT("START"), wxDefaultPosition, wxSize(425, 50));
  hide_btn_start->SetFocus();

  btn_grid->Add(hide_btn_start, 1, wxEXPAND);

  hide_grid_sizer->Add(file_grid, 0, wxALIGN_CENTER);
  hide_grid_sizer->Add(img_grid, 0, wxALIGN_CENTER);
  hide_grid_sizer->Add(encrypt_grid, 0, wxALIGN_CENTER);
  hide_grid_sizer->Add(output_grid, 0, wxALIGN_CENTER);
  hide_grid_sizer->Add(btn_grid, 0, wxALIGN_CENTER);


  hide_wxboxsizer->Add(hide_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(hide_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return hide_wxpanel;
} //wxPanel* MyFrame::hide_panel_cr(wxWindow* parent)

wxPanel* MyFrame::seek_panel_cr(wxWindow* parent)
{
  /*
  seek_idpanel,
  seek_txtctrl_outname,
  seek_cbox_dec,
  seek_txtctrl_pwd
  */
  wxPanel* seek_wxpanel;
  if(parent == nullptr)
  {
    seek_wxpanel = new wxPanel(this, seek_idpanel, wxDefaultPosition, wxDefaultSize);
  }
  else
  {
    seek_wxpanel = new wxPanel(parent, seek_idpanel, wxDefaultPosition, wxDefaultSize);
  }

  seek_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* seek_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = seek_wxpanel;

  wxGridSizer* seek_grid_sizer = new wxGridSizer(4, 1, 5, 5);

  wxGridSizer* img_grid = new wxGridSizer(1, 3, 5, 5);
  wxStaticText* txt_choose_img = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Img where to seek"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  txt_choose_img->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxButton* choose_img_btn = new wxButton(this_wxpanel, id_button_open_img, wxT("Select image"), wxDefaultPosition, wxSize(150, 20));
  wxStaticText* choose_img = new wxStaticText(this_wxpanel, seek_stctxt_choose_img, wxT("No image choose"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  choose_img->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  img_grid->Add(txt_choose_img, 1, wxEXPAND);
  img_grid->Add(choose_img_btn, 1, wxEXPAND);
  img_grid->Add(choose_img, 1, wxALIGN_CENTER);

  wxGridSizer* encrypt_grid = new wxGridSizer(1, 3, 5, 5);
  wxCheckBox* use_decrypt = new wxCheckBox(this_wxpanel, seek_cbox_dec, wxT("Decrypt file"), wxDefaultPosition, wxSize(200, 40));
  use_decrypt->SetValue(false);
  use_decrypt->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxCheckBox* use_custom_pwd = new wxCheckBox(this_wxpanel, seek_cbox_custom_pwd, wxT("Use custom pwd"), wxDefaultPosition, wxSize(200, 40));
  use_custom_pwd->SetValue(false);
  use_custom_pwd->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxTextCtrl* pwd_seek = new wxTextCtrl(this_wxpanel, seek_txtctrl_pwd, wxEmptyString, wxDefaultPosition, wxSize(200, 40), wxTE_PASSWORD);
  pwd_seek->SetHint(wxT("PASSWORD"));
  encrypt_grid->Add(use_decrypt, 1, wxEXPAND);
  encrypt_grid->Add(use_custom_pwd, 1, wxEXPAND);
  encrypt_grid->Add(pwd_seek, 1, wxEXPAND);

  wxGridSizer* output_grid = new wxGridSizer(1, 2, 5, 5);
  wxStaticText* txt_choose_out = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Name output (you can't\nchange the extension)"), wxDefaultPosition, wxSize(200, 40), wxALIGN_CENTRE);
  txt_choose_out->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxTextCtrl* outname_seek = new wxTextCtrl(this_wxpanel, seek_txtctrl_outname, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT));
  outname_seek->SetHint(wxT("NAME OUTPUT"));
  output_grid->Add(txt_choose_out, 1, wxEXPAND);
  output_grid->Add(outname_seek, 1, wxEXPAND);

  wxGridSizer* btn_grid = new wxGridSizer(1, 1, 5, 5);
  wxButton* seek_btn_start = new wxButton(this_wxpanel, id_button_calc_seek, wxT("START"), wxDefaultPosition, wxSize(425, 50));
  seek_btn_start->SetFocus();

  btn_grid->Add(seek_btn_start, 1, wxEXPAND);

  seek_grid_sizer->Add(img_grid, 0, wxALIGN_CENTER);
  seek_grid_sizer->Add(encrypt_grid, 0, wxALIGN_CENTER);
  seek_grid_sizer->Add(output_grid, 0, wxALIGN_CENTER);
  seek_grid_sizer->Add(btn_grid, 0, wxALIGN_CENTER);

  seek_wxboxsizer->Add(seek_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(seek_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return seek_wxpanel;
} //wxPanel* MyFrame::seek_panel_cr(wxWindow* parent)

wxPanel* MyFrame::aes256_panel_cr(wxWindow* parent)
{
  /*
  aes256_idpanel,
  aes256_txtctrl_outname,
  aes256_cbox_enc,
  aes256_txtctrl_pwd,
  aes256_radio_opt,
  */
  wxPanel* aes256_wxpanel;
  if(parent == nullptr)
  {
    aes256_wxpanel = new wxPanel(this, aes256_idpanel, wxDefaultPosition, wxDefaultSize);
  }
  else
  {
    aes256_wxpanel = new wxPanel(parent, aes256_idpanel, wxDefaultPosition, wxDefaultSize);
  }

  aes256_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* aes256_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = aes256_wxpanel;

  wxGridSizer* aes256_grid_sizer = new wxGridSizer( 5, 1, 5, 5);

  wxGridSizer* radiobox_grid = new wxGridSizer(1, 2, 5, 5);
  wxRadioButton* encr = new wxRadioButton(this_wxpanel, aes256_radio_enc, wxT("&encrypt"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);//, wxRA_SPECIFY_COLS);
  wxRadioButton* decr = new wxRadioButton(this_wxpanel, aes256_radio_dec, wxT("&decrypt"), wxDefaultPosition, wxDefaultSize);//, wxRA_SPECIFY_COLS);
  encr->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  decr->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  encr->SetValue(true);
  radiobox_grid->Add(encr, 1, wxEXPAND);
  radiobox_grid->Add(decr, 1, wxEXPAND);

  wxGridSizer* file_grid = new wxGridSizer(1, 3, 5, 5);
  wxStaticText* txt_choose_file = new wxStaticText(this_wxpanel, wxID_ANY, wxT("File to work with"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  txt_choose_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxButton* choose_file_btn = new wxButton(this_wxpanel, id_button_open_file, wxT("Select file"), wxDefaultPosition, wxSize(150, 20));
  wxStaticText* choose_file = new wxStaticText(this_wxpanel, aes256_stctxt_choose_file, wxT("No file choose"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  choose_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  file_grid->Add(txt_choose_file, 1, wxEXPAND);
  file_grid->Add(choose_file_btn, 1, wxEXPAND);
  file_grid->Add(choose_file, 1, wxALIGN_CENTER);

  wxGridSizer* encrypt_grid = new wxGridSizer(1, 2, 5, 5);
  wxCheckBox* use_custom_pwd = new wxCheckBox(this_wxpanel, aes256_cbox_enc, wxT("Use custom\npassword"), wxDefaultPosition, wxSize(200, 40));
  use_custom_pwd->SetValue(false);
  use_custom_pwd->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxTextCtrl* pwd_aes256 = new wxTextCtrl(this_wxpanel, aes256_txtctrl_pwd, wxEmptyString, wxDefaultPosition, wxSize(200, 40), wxTE_PASSWORD);
  pwd_aes256->SetHint(wxT("PASSWORD"));
  encrypt_grid->Add(use_custom_pwd, 1, wxEXPAND);
  encrypt_grid->Add(pwd_aes256, 1, wxEXPAND);

  wxGridSizer* output_grid = new wxGridSizer(1, 2, 5, 5);
  wxStaticText* txt_choose_out = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Name output (extension\nwill be the same as the input)"), wxDefaultPosition, wxSize(200, 40), wxALIGN_CENTRE);
  txt_choose_out->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxTextCtrl* outname_aes256 = new wxTextCtrl(this_wxpanel, aes256_txtctrl_outname, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT));
  outname_aes256->SetHint(wxT("NAME OUTPUT"));
  output_grid->Add(txt_choose_out, 1, wxEXPAND);
  output_grid->Add(outname_aes256, 1, wxEXPAND);

  wxGridSizer* btn_grid = new wxGridSizer(1, 1, 5, 5);
  wxButton* aes256_btn_start = new wxButton(this_wxpanel, id_button_calc_aes256, wxT("START"), wxDefaultPosition, wxSize(425, 50));
  aes256_btn_start->SetFocus();

  btn_grid->Add(aes256_btn_start, 1, wxEXPAND);

  aes256_grid_sizer->Add(radiobox_grid, 0, wxALIGN_CENTER);
  aes256_grid_sizer->Add(file_grid, 0, wxALIGN_CENTER);
  aes256_grid_sizer->Add(encrypt_grid, 0, wxALIGN_CENTER);
  aes256_grid_sizer->Add(output_grid, 0, wxALIGN_CENTER);
  aes256_grid_sizer->Add(btn_grid, 0, wxALIGN_CENTER);


  aes256_wxboxsizer->Add(aes256_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(aes256_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return aes256_wxpanel;
} //wxPanel* MyFrame::aes256_panel_cr(wxWindow* parent)

wxPanel* MyFrame::b64_panel_cr(wxWindow* parent)
{
  wxPanel* b64_wxpanel;
  if(parent == nullptr)
  {
    b64_wxpanel = new wxPanel(this, b64_idpanel, wxDefaultPosition, wxDefaultSize);
  }
  else
  {
    b64_wxpanel = new wxPanel(parent, b64_idpanel, wxDefaultPosition, wxDefaultSize);
  }

  b64_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* b64_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = b64_wxpanel;

  wxGridSizer* b64_grid_sizer = new wxGridSizer( //4, 1, 0, 0
    4, //# rows
    1, //# cols
    5, //# vgap
    15 //# hgap
  );

  wxGridSizer* upper_grid = new wxGridSizer(2, 3, 0, 5);

  wxRadioButton* radbtn_enc = new wxRadioButton(this_wxpanel,b64_radbtn_enc, wxT("&encode"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);//, wxRA_SPECIFY_COLS);
  wxRadioButton* radbtn_dec = new wxRadioButton(this_wxpanel, b64_radbtn_dec, wxT("&decode"), wxDefaultPosition, wxDefaultSize);//, wxRA_SPECIFY_COLS);
  radbtn_enc->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  radbtn_dec->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  radbtn_enc->SetValue(true);

  //wxCheckBox (wxWindow *parent, wxWindowID id, const wxString &label, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxCheckBoxNameStr)
  wxCheckBox* from_file = new wxCheckBox(this_wxpanel, b64_cbox_fromfile, wxT("From file"), wxDefaultPosition, wxSize(150, 25));
  from_file->SetValue(false);
  from_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  wxCheckBox* each_line = new wxCheckBox(this_wxpanel, b64_cbox_eachline, wxT("Each line it's a\ndifferent input"), wxDefaultPosition, wxSize(150, 40));
  each_line->SetValue(false);
  each_line->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);

  wxString default_opts("RFC4648 STD");
  wxArrayString b64_standard_opts;
  b64_standard_opts.Add(default_opts);
  b64_standard_opts.Add(wxT("RFC4648 URL"));
  b64_standard_opts.Add(wxT("RFC3501 STD"));
  /*parent	Parent window. Must not be NULL.
    id	Window identifier. The value wxID_ANY indicates a default value.
    value	Initial selection string. An empty string indicates no selection.
    pos	Window position.
    size	Window size. If wxDefaultSize is specified then the window is sized appropriately.
    choices	An array of strings with which to initialise the control.
    style	Window style. See wxComboBox.
    validator	Window validator.
    name	Window name.
*/
  wxComboBox* b64_standard = new wxComboBox(
    this_wxpanel,
    b64_combobox_standard,
    default_opts,
    wxDefaultPosition,
    wxDefaultSize,
    b64_standard_opts,
    wxCB_READONLY
  );

  wxButton* choose_file_btn = new wxButton(this_wxpanel, id_button_open_file, wxT("Select file"), wxDefaultPosition, wxSize(150, 20));
  wxStaticText* choose_file = new wxStaticText(this_wxpanel, b64_stctxt_choose_file, wxT("No file choose"), wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTRE);
  choose_file->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  choose_file->SetFocus();

  wxGridSizer* select_file_n_standard = new wxGridSizer(1, 2, 0, 5);

  select_file_n_standard->Add(b64_standard, 1, wxEXPAND);
  select_file_n_standard->Add(choose_file_btn, 0, wxEXPAND);

  upper_grid->Add(radbtn_enc, 1, wxALIGN_CENTER);
  upper_grid->Add(from_file, 1, wxALIGN_CENTER);
  upper_grid->Add(select_file_n_standard, 0, wxEXPAND);
  upper_grid->Add(radbtn_dec, 1, wxALIGN_CENTER);
  upper_grid->Add(each_line, 1, wxALIGN_CENTER);
  upper_grid->Add(choose_file, 0, wxALIGN_CENTER);

  //end first line
  wxTextCtrl* input_b64 = new wxTextCtrl(this_wxpanel, b64_txtctrl_input, wxEmptyString, wxDefaultPosition, wxSize(425, 50), wxTE_MULTILINE);
  wxTextCtrl* output_b64 = new wxTextCtrl(this_wxpanel, b64_txtctrl_output, wxEmptyString, wxDefaultPosition, wxSize(425, 50), wxTE_MULTILINE | wxTE_READONLY);
  input_b64->SetHint(wxT("Text to calculate BASE64"));
  output_b64->SetHint(wxT("Result"));

  wxButton* b64_btn_start = new wxButton(this_wxpanel, id_button_calc_b64, wxT("START"), wxDefaultPosition, wxSize(425, 50));

  b64_grid_sizer->Add(upper_grid, 1, wxEXPAND);
  b64_grid_sizer->Add(input_b64, 0, wxEXPAND);
  b64_grid_sizer->Add(output_b64, 0, wxEXPAND);
  b64_grid_sizer->Add(b64_btn_start, 0, wxEXPAND);

  b64_wxboxsizer->Add(b64_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(b64_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return b64_wxpanel;
} //wxPanel* MyFrame::b64_panel_cr(wxWindow* parent)

wxPanel* MyFrame::dfhl_panel_cr(wxWindow* parent)
{
  /*
  dfhl_idpanel,
  dfhl_txtctrl_pub_input,
  dfhl_txtctrl_output,
  dfhl_radio_first,
  dfhl_radio_second
  */
  wxPanel* dfhl_wxpanel;
  if(parent == nullptr)
  {
    dfhl_wxpanel = new wxPanel(this, dfhl_idpanel, wxDefaultPosition, wxDefaultSize);
  }
  else
  {
    dfhl_wxpanel = new wxPanel(parent, dfhl_idpanel, wxDefaultPosition, wxDefaultSize);
  }

  dfhl_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* dfhl_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = dfhl_wxpanel;

  wxGridSizer* dfhl_grid_sizer = new wxGridSizer(5, 1, 5, 5);

  wxGridSizer* radiobox_grid = new wxGridSizer(1, 2, 5, 5);
  wxRadioButton* first = new wxRadioButton(this_wxpanel, dfhl_radio_first, wxT("I &don't have any key"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);//, wxRA_SPECIFY_COLS);
  wxRadioButton* second = new wxRadioButton(this_wxpanel, dfhl_radio_second, wxT("I &have a key"), wxDefaultPosition, wxDefaultSize);//, wxRA_SPECIFY_COLS);
  first->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  second->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  first->SetValue(true);
  radiobox_grid->Add(first, 1, wxEXPAND);
  radiobox_grid->Add(second, 1, wxEXPAND);

  wxTextCtrl* input_public_dfhl = new wxTextCtrl(this_wxpanel, dfhl_txtctrl_pub_input, wxEmptyString, wxDefaultPosition, wxSize(520, 50), wxTE_MULTILINE);
  input_public_dfhl->SetHint(wxT("Input public half-key"));
  wxTextCtrl* input_secret_dfhl = new wxTextCtrl(this_wxpanel, dfhl_txtctrl_priv_input, wxEmptyString, wxDefaultPosition, wxSize(520, 50), wxTE_MULTILINE);
  input_secret_dfhl->SetHint(wxT("Input private half-key"));
  wxTextCtrl* output_dfhl = new wxTextCtrl(this_wxpanel, dfhl_txtctrl_output, wxEmptyString, wxDefaultPosition, wxSize(520, 50), wxTE_MULTILINE | wxTE_READONLY);
  output_dfhl->SetHint(wxT("Result"));

  wxGridSizer* btn_grid = new wxGridSizer(1, 1, 5, 5);
  wxButton* dfhl_btn_start = new wxButton(this_wxpanel, id_button_calc_dfhl, wxT("START"), wxDefaultPosition, wxSize(520, 50));
  dfhl_btn_start->SetFocus();

  btn_grid->Add(dfhl_btn_start, 1, wxEXPAND);

  dfhl_grid_sizer->Add(radiobox_grid, 0, wxALIGN_CENTER);
  dfhl_grid_sizer->Add(input_public_dfhl, 0, wxALIGN_CENTER);
  dfhl_grid_sizer->Add(input_secret_dfhl, 0, wxALIGN_CENTER);
  dfhl_grid_sizer->Add(output_dfhl, 0, wxALIGN_CENTER);
  dfhl_grid_sizer->Add(btn_grid, 0, wxALIGN_CENTER);

  dfhl_wxboxsizer->Add(dfhl_grid_sizer, 1, wxEXPAND);

  this_wxpanel->SetSizer(dfhl_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return dfhl_wxpanel;
} //wxPanel* MyFrame::dfhl_panel_cr(wxWindow* parent)

wxPanel* MyFrame::personal_panel_cr(wxWindow* parent)
{
  /*
  personal_idpanel,
  personal_txtctrl_usr,
  personal_txtctrl_pwd_old,
  personal_txtctrl_pwd_new,
  personal_txtctrl_pwd_check
  */
  wxPanel* personal_wxpanel;
  if(parent == nullptr)
  {
    personal_wxpanel = new wxPanel(this, personal_idpanel, wxDefaultPosition, wxDefaultSize);
  }
  else
  {
    personal_wxpanel = new wxPanel(parent, personal_idpanel, wxDefaultPosition, wxDefaultSize);
  }

  personal_wxpanel->SetBackgroundColour(BLACK_BACKGROUND);
  wxBoxSizer* personal_wxboxsizer = new wxBoxSizer(wxVERTICAL);

  auto this_wxpanel = personal_wxpanel;

  wxGridSizer* grid_texts = new wxGridSizer(5, 2, 10, 10);

  wxStaticText* usr_ = new wxStaticText(this_wxpanel, personal_txtctrl_usr, wxEmptyString); //"user_name->GetValue()");
  usr_->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  //wxTextCtrl* usr_ = new wxTextCtrl(this_wxpanel, personal_txtctrl_usr, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT));
  wxTextCtrl* pwd_old = new wxTextCtrl(this_wxpanel, personal_txtctrl_pwd_old, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT), wxTE_PASSWORD);
  wxTextCtrl* pwd_new = new wxTextCtrl(this_wxpanel, personal_txtctrl_pwd_new, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT), wxTE_PASSWORD);
  wxTextCtrl* pwd_check = new wxTextCtrl(this_wxpanel, personal_txtctrl_pwd_check, wxEmptyString, wxDefaultPosition, wxSize(SIZE_IN_LBL_WDT, SIZE_IN_LBL_HGT), wxTE_PASSWORD);
  //usr_->SetHint(wxT("USERNAME"));
  pwd_old->SetHint(wxT("OLD PASSWORD"));
  pwd_new->SetHint(wxT("NEW PASSWORD"));
  pwd_check->SetHint(wxT("CONFIRM NEW PASSWORD"));

  wxStaticText* usr_text = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Insert user:"));
  wxStaticText* scrt_old_text = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Insert old password:"));
  wxStaticText* scrt_new_text = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Insert new password:"));
  wxStaticText* scrt_check_text = new wxStaticText(this_wxpanel, wxID_ANY, wxT("Re-insert new password:"));
  usr_text->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  scrt_old_text->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  scrt_new_text->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);
  scrt_check_text->SetForegroundColour(TEXT_COLOR_ON_BLACK_BACKGROUND);

  wxButton* personal_btn_recovery = new wxButton(this_wxpanel, id_button_recovery_personal, wxT("RECOVERY PASSWORD"));
  wxButton* personal_btn_change = new wxButton(this_wxpanel, id_button_change_personal, wxT("CHANGE PASSWORD"));

  grid_texts->Add(usr_text, 1, wxALIGN_CENTER);
  grid_texts->Add(usr_, 1, wxALIGN_CENTER);
  grid_texts->Add(scrt_old_text, 1, wxALIGN_CENTER);
  grid_texts->Add(pwd_old, 1, wxEXPAND);
  grid_texts->Add(scrt_new_text, 1, wxALIGN_CENTER);
  grid_texts->Add(pwd_new, 1, wxEXPAND);
  grid_texts->Add(scrt_check_text, 1, wxALIGN_CENTER);
  grid_texts->Add(pwd_check, 1, wxEXPAND);
  grid_texts->Add(personal_btn_recovery, 1, wxEXPAND);
  grid_texts->Add(personal_btn_change, 1, wxEXPAND);

  personal_wxboxsizer->Add(grid_texts, 1, wxEXPAND);

  this_wxpanel->SetSizer(personal_wxboxsizer);
  this_wxpanel->Centre();
  this_wxpanel->SetFocus();

  return personal_wxpanel;
} //wxPanel* MyFrame::personal_panel_cr(wxWindow* parent)

// ----------------------------------------------------------------------------
// BUTTONS FOR MAKING CALCULATIONS
// ----------------------------------------------------------------------------

void MyFrame::button_change_personal(wxCommandEvent& event)
{
  //std::cout << "Hey, button button_change_personal\n";
  wxMessageBox(wxT("This function is not ready yet"), //text
              wxT("Coming soon"), //title
              wxOK | wxICON_INFORMATION); //type of message
  event.Skip();
} //void MyFrame::button_change_personal(wxCommandEvent& event)

void MyFrame::button_recovery_personal(wxCommandEvent& event)
{
  //std::cout << "Hey, button button_recovery_personal\n";
  wxMessageBox(wxT("This function is not ready yet"), //text
              wxT("Coming soon"), //title
              wxOK | wxICON_INFORMATION); //type of message

  clear_pwd_label();

  event.Skip();
} //void MyFrame::button_recovery_personal(wxCommandEvent& event)

void MyFrame::button_run_recovery(wxCommandEvent& event)
{
  //std::cout << "Hey, button button_run_recovery\n";
  //wxTextCtrl* this_username = reinterpret_cast<wxTextCtrl*>(FindWindowById(this->_username_in_use));
  //wxTextCtrl* this_password = reinterpret_cast<wxTextCtrl*>(FindWindowById(this->_password_in_use));
  //std::cout << "username = " << this_username->GetValue() << "\n";
  //std::cout << "password = " << this_password->GetValue() << "\n";

  wxMessageBox(wxT("This function is not ready yet"), //text
              wxT("Coming soon"), //title
              wxOK | wxICON_INFORMATION); //type of message

  event.Skip();
} //void MyFrame::button_run_recovery(wxCommandEvent& event)

void MyFrame::button_calc_sha256(wxCommandEvent& event)
{
  //I consider only the file like an input
  const uint64_t FLAGGED = 1;
  wxTextCtrl* txtctrl_output = reinterpret_cast<wxTextCtrl*>(FindWindowById(sha256_txtctrl_output));

  wxCheckBox* cbox_fromfile = reinterpret_cast<wxCheckBox*>(FindWindowById(sha256_cbox_fromfile));
  if(cbox_fromfile->GetValue() == FLAGGED)
  {
    txtctrl_output->Clear();

    _file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(stcstr_sha256.ToStdString(), true);

    if(input_file.get_data_file() == nullptr) //file not found
    {
      txtctrl_output->AppendText(ERROR_FILE_NOT_OPEN);
      return;
    }

    _sha256_rp96_::sha256 hash = _sha256_rp96_::sha256(reinterpret_cast<char*>(input_file.get_data_file()), input_file.get_size_file());
    hash.compute_hash();

    txtctrl_output->AppendText(hash.get_SHA256_string());

    return;
  }

  wxTextCtrl* txtctrl_input = reinterpret_cast<wxTextCtrl*>(FindWindowById(sha256_txtctrl_input));
  uint8_t* txt_input = nullptr;

  //I consider every line a different input
  wxCheckBox* cbox_eachline = reinterpret_cast<wxCheckBox*>(FindWindowById(sha256_cbox_eachline));
  if(cbox_eachline->GetValue() == FLAGGED)
  {
    txtctrl_output->Clear();

    std::istringstream input_all_txt;
    input_all_txt.str(txtctrl_input->GetValue().ToStdString());
    for (std::string line; std::getline(input_all_txt, line); )
    {
      uint64_t len_input = static_cast<uint64_t>(line.length());
      txt_input = new uint8_t[len_input];

      for(uint64_t i = 0; i < len_input; ++i)
      {
        txt_input[i] = static_cast<uint8_t>(line[i]);
      }

      _sha256_rp96_::sha256 hash_input = _sha256_rp96_::sha256(reinterpret_cast<char*>(txt_input), len_input);
      hash_input.compute_hash();

      txtctrl_output->AppendText(hash_input.get_SHA256_string());
      txtctrl_output->AppendText("\n");

      delete[] txt_input;
      txt_input = nullptr;
    }
    delete[] txt_input;
    txt_input = nullptr;
    return;
  }

  //I consider whole input like a single value
  uint64_t len_input = static_cast<uint64_t>(txtctrl_input->GetValue().length());
  txt_input = new uint8_t[len_input];

  for(uint64_t i = 0; i < len_input; ++i)
  {
    txt_input[i] = static_cast<uint8_t>(txtctrl_input->GetValue()[i]);
  }

  _sha256_rp96_::sha256 hash_input = _sha256_rp96_::sha256(reinterpret_cast<char*>(txt_input), len_input);
  hash_input.compute_hash();

  txtctrl_output->Clear();
  txtctrl_output->AppendText(hash_input.get_SHA256_string());

  delete[] txt_input;
  txt_input = nullptr;

  event.Skip();
} //void MyFrame::button_calc_sha256(wxCommandEvent& event)

//TODO:
//i file li codifica e basta
//non check sull'input inserito altre lingue
void MyFrame::button_calc_b64(wxCommandEvent& event)
{
  //I consider only the file like an input
  const uint64_t FLAGGED = 1;
  const char* b64_warning = "invalid input to encode or decode";

  wxRadioButton* do_i_have_enc = reinterpret_cast<wxRadioButton*>(FindWindowById(b64_radbtn_enc));
  wxCheckBox* cbox_fromfile = reinterpret_cast<wxCheckBox*>(FindWindowById(b64_cbox_fromfile));
  wxTextCtrl* txtctrl_output = reinterpret_cast<wxTextCtrl*>(FindWindowById(b64_txtctrl_output));
  wxComboBox* standard_used = reinterpret_cast<wxComboBox*>(FindWindowById(b64_combobox_standard));

  txtctrl_output->Clear();

  int64_t input_length = -1;
  uint8_t* input_text = nullptr;

  //###################
  // ENCODE-DECODE FILE
  //###################
  if(cbox_fromfile->GetValue() == FLAGGED) //if input it's a file
  {
    //obtains data from file
    _file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(stcstr_b64.ToStdString(), true);

    if(input_file.get_data_file() == nullptr) //file not found
    {
      txtctrl_output->AppendText(ERROR_FILE_NOT_OPEN);
      event.Skip();
      return;
    }

    //first check if I want to work with each line differently or not
    wxCheckBox* cbox_eachline = reinterpret_cast<wxCheckBox*>(FindWindowById(b64_cbox_eachline));
    if(cbox_eachline->GetValue() == FLAGGED)
    {
      std::istringstream input_all_txt;
      input_all_txt.str(reinterpret_cast<char*>(input_file.get_data_file()));
      //for each line to input
      for (std::string line; std::getline(input_all_txt, line); )
      {
        if(line.length() == 0)
        {
          continue;
        }

        std::string out_text("");
        //i copy the line and static convert it uint8_t
        int64_t buffer_len = static_cast<int64_t>(line.length());
        uint8_t* buffer_txt = new uint8_t[buffer_len];
        for(int64_t i = 0; i < buffer_len; ++i)
        {
          buffer_txt[i] = static_cast<uint8_t>(line.at(i));
        }
        if(do_i_have_enc->GetValue() == FLAGGED)
        {
          input_text = _base64_rp96_::base64::base64_encode(buffer_txt,
          buffer_len, input_length, standard_used->GetSelection());
        }
        else
        {
          input_text = _base64_rp96_::base64::base64_decode(buffer_txt,
          buffer_len, input_length);
        }
        //then if i have valid results, print them
        if(input_text == nullptr)
        {
          out_text += std::string(b64_warning);
        }
        else
        {
          out_text = std::string(reinterpret_cast<char*>(input_text));
          out_text.resize(input_length);
        }
        //update the gui
        txtctrl_output->AppendText(out_text);
        txtctrl_output->AppendText("\n");
        //then clean the value
        delete[] buffer_txt;
        buffer_txt = nullptr;
        delete[] input_text;
        input_text = nullptr;
      }
    }
    else
    {
      std::string out_text("");
      //after that i want to understand if i need to encode or decode
      if(do_i_have_enc->GetValue() == FLAGGED)
      {
        input_text = _base64_rp96_::base64::base64_encode(input_file.get_data_file(),
        input_file.get_size_file(), input_length, standard_used->GetSelection());
      }
      else
      {
        input_text = _base64_rp96_::base64::base64_decode(input_file.get_data_file(),
        input_file.get_size_file(), input_length);
      }
      //then if i have valid results, print them
      if(input_text == nullptr)
      {
        out_text += std::string(b64_warning);
      }
      else
      {
        out_text = std::string(reinterpret_cast<char*>(input_text));
        out_text.resize(input_length);
      }
      //update the gui
      txtctrl_output->AppendText(out_text);
      txtctrl_output->AppendText("\n");
      //then clean the value
      delete[] input_text;
      input_text = nullptr;

    }

    //then clean the value and return
    delete[] input_text;
    input_text = nullptr;

    event.Skip();
    return;
  }

  //else i take input text control

  //I take all input
  wxTextCtrl* txtctrl_input = reinterpret_cast<wxTextCtrl*>(FindWindowById(b64_txtctrl_input));

  //###################
  // ENCODE-DECODE PLAINTEXT MULTILINE
  //###################
  //I change work tipe if eachline it's flagged

  wxCheckBox* cbox_eachline = reinterpret_cast<wxCheckBox*>(FindWindowById(b64_cbox_eachline));
  if(cbox_eachline->GetValue() == FLAGGED)
  {
    std::istringstream input_all_txt;
    input_all_txt.str(txtctrl_input->GetValue().ToStdString());
    //for each line to input
    for (std::string line; std::getline(input_all_txt, line); )
    {
      if(line.length() == 0)
      {
        continue;
      }

      std::string out_text("");
      //i copy the line and static convert it uint8_t
      int64_t buffer_len = static_cast<int64_t>(line.length());
      uint8_t* buffer_txt = new uint8_t[buffer_len];
      for(int64_t i = 0; i < buffer_len; ++i)
      {
        buffer_txt[i] = static_cast<uint8_t>(line.at(i));
      }
      if(do_i_have_enc->GetValue() == FLAGGED)
      {
        input_text = _base64_rp96_::base64::base64_encode(buffer_txt,
        buffer_len, input_length, standard_used->GetSelection());
      }
      else
      {
        input_text = _base64_rp96_::base64::base64_decode(buffer_txt,
        buffer_len, input_length);
      }
      //then if i have valid results, print them
      if(input_text == nullptr)
      {
        out_text += std::string(b64_warning);
      }
      else
      {
        out_text = std::string(reinterpret_cast<char*>(input_text));
        out_text.resize(input_length);
      }
      //update the gui
      txtctrl_output->AppendText(out_text);
      txtctrl_output->AppendText("\n");
      //then clean the value
      delete[] buffer_txt;
      buffer_txt = nullptr;
      delete[] input_text;
      input_text = nullptr;
    }
    event.Skip();
    return;
  }

  //###################
  // ENCODE-DECODE PLAINTEXT
  //###################
  //convert it in uint8_t* (i can't only cast "const char*" to "uint8_t*")
  //std::string input_requested = txtctrl_input->GetValue().ToStdString();
  uint64_t buffer_len = static_cast<uint64_t>(txtctrl_input->GetValue().length());
  uint8_t* buffer_txt = new uint8_t[buffer_len];
  for(uint64_t i = 0; i < buffer_len; ++i)
  {
    buffer_txt[i] = static_cast<uint8_t>(txtctrl_input->GetValue()[i]);
  }

  std::string out_text("");
  input_text = nullptr;
  int64_t len_output = -1;

  if(do_i_have_enc->GetValue() == FLAGGED)
  {
    input_text = _base64_rp96_::base64::base64_encode(buffer_txt, buffer_len, len_output, standard_used->GetSelection());
  }
  else
  {
    input_text = _base64_rp96_::base64::base64_decode(buffer_txt, buffer_len, len_output);
  }

  if(input_text == nullptr)
  {
    out_text += std::string(b64_warning);
  }
  else
  {
    out_text = std::string(reinterpret_cast<char*>(input_text));
    out_text.resize(len_output);
  }

  txtctrl_output->AppendText(out_text);
  txtctrl_output->AppendText("\n");
  delete[] buffer_txt;
  buffer_txt = nullptr;
  delete[] input_text;
  input_text = nullptr;

  event.Skip();
  return;
} //void MyFrame::button_calc_b64(wxCommandEvent& event)

void MyFrame::button_calc_hide(wxCommandEvent& event)
{
  const uint64_t FLAGGED = 1;

  //#######################################
  //HIDE NEEDS THE FOLLOWS INSTRUCTIONS:
  //1. Check if can I use the file choosen;
  //2. Check if can I use the img choosen;
  //3. Check the name of output img (.png):
  //  a. if name output label is empty
  //     i'll use img name;
  //  b. else use custom name;
  //  c. if the file already exists
  //     then append "_n" before the extension
  //     where n is between 1..infinite
  //     name check is O(n)
  //4. Control if I want to encrypt:
  //  a. Control if I want to use custom password:
  //    I. if I want I calc sha256(custom pwd);
  //    II. if I want but custom pwd is empty,
  //        i calc sha256 on empty string;
  //  b. else use local pwd;
  //  c. start to encrypt;
  //5. Save the file:
  //  a. If i have encrpyt and CMS != 0
  //     then i need to merge the pointer
  //6. Show a pop-up to declare success
  //#######################################

  //#######################################
  //1. Check if can I use the file choosen
  //#######################################

  //check if the file is selected
  if(stcstr_hide_file.IsEmpty())
  {
    wxMessageBox(WARNING_FILE_NEEDED, WARN_LABEL, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  //read the file to hide
  _file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(stcstr_hide_file.ToStdString());

  if(input_file.get_data_file() == nullptr) //file not found
  {
    wxMessageBox(ERROR_FILE_NOT_OPEN, WARN_LABEL, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  //#######################################
  //2. Check if can I use the img choosen;
  //#######################################

  //check if the img is selected
  if(stcstr_hide_img.IsEmpty())
  {
    wxMessageBox(WARNING_IMG_NEEDED, WARN_LABEL, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  //check if can i open the img
  std::ifstream test_open_img(stcstr_hide_file.ToStdString());
  if (test_open_img.is_open() == false)
  {
    wxMessageBox(ERROR_FILE_NOT_OPEN, WARN_LABEL, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }
  test_open_img.close();

  //#######################################
  //3. Check the name of output img (.png):
  //#######################################

  //obtain the custom name
  std::string custom_name("");
  wxTextCtrl* lbl_cstm_name = reinterpret_cast<wxTextCtrl*>(FindWindowById(hide_txtctrl_outname));
  if(lbl_cstm_name->GetValue().IsEmpty() == true)
  {
    //#######################################
    // 3a. if name output label is empty
    //     i'll use img name;
    //#######################################
    std::filesystem::path img_path{stcstr_hide_img.ToStdString()};
    custom_name += img_path.stem();
  }
  else
  {
    //#######################################
    // 3b. else use custom name;
    //#######################################
    custom_name += lbl_cstm_name->GetValue().ToStdString();
  }

  //#######################################
  // 3c. if the file already exists
  //     then append "_n" before the extension
  //     where n is between 1..infinite
  //     name check is O(n)
  //#######################################

  //check if the name already exists
  std::string tmp_name = custom_name;
  tmp_name += IMG_EXTENSION;

  //now tmp_name contains the "standard name" like: file.png
  //check if file.png already exists
  //if true transform file.png into file_.png
  //then search file_1.png, file_2.png ... file_n.png
  //until i found a file that non exists
  std::filesystem::path hypotetical_new_file{tmp_name};
  if(std::filesystem::exists(hypotetical_new_file))
  {
    custom_name += "_";
    custom_name += std::to_string(check_non_exists_file(custom_name, IMG_EXTENSION));
  }

  //#######################################
  //4. Control if I want to encrypt:
  //  a. Control if I want to use custom password:
  //    I. if I want I calc sha256(custom pwd);
  //    II. if I want but custom pwd is empty,
  //        i calc sha256 on empty string;
  //  b. else use local pwd;
  //  c. start to encrypt;
  //#######################################

  //#######################################
  //4. Control if I want to encrypt:
  //#######################################
  uint8_t* output_data = input_file.get_data_file();
  int64_t output_length = input_file.get_size_file();
  bool need_to_clean = false;

  wxCheckBox* cbox_encrypt = reinterpret_cast<wxCheckBox*>(FindWindowById(hide_cbox_enc));
  if(cbox_encrypt->GetValue() == FLAGGED)
  {
    //#######################################
    //4a. Control if I want to use custom password:
    //#######################################
    wxCheckBox* cbox_custom_pwd = reinterpret_cast<wxCheckBox*>(FindWindowById(hide_cbox_custom_pwd));
    std::string pwd_to_use;

    if(cbox_custom_pwd->GetValue() == FLAGGED)
    {
      //#######################################
      //4aI. if I want I calc sha256(custom pwd);
      //4aII. if I want but custom pwd is empty,
      //      i calc sha256 on empty string;
      //#######################################

      //take pwd
      wxTextCtrl* pwd_custom = reinterpret_cast<wxTextCtrl*>(FindWindowById(hide_txtctrl_pwd));

      //calc sha256
      int64_t tmp_len = pwd_custom->GetValue().ToStdString().length();
      char* tmp_data = new char[tmp_len];

      for(int64_t idx = 0; idx < tmp_len; ++idx)
      {
        tmp_data[idx] = pwd_custom->GetValue().ToStdString().at(idx);
      }

      _sha256_rp96_::sha256 hash_input = _sha256_rp96_::sha256(tmp_data, pwd_custom->GetValue().ToStdString().length());
      hash_input.compute_hash();

      for(int64_t idx = 0; idx < tmp_len; ++idx)
      {
        tmp_data[idx] = static_cast<char>(0xFF);
      }

      delete[] tmp_data;
      tmp_data = nullptr;
      //tmp_len = -1;

      //save in pwd_to_use
      pwd_to_use = std::string(hash_input.get_SHA256_string());
    }
    else
    {
      //#######################################
      //b. else use local pwd;
      //#######################################
      pwd_to_use = std::string(this->local_hash_pwd.ToStdString());
    }

    //#######################################
    //c. start to encrypt;
    //#######################################
    _aes256_rp96_::aes256 a256 = _aes256_rp96_::aes256(input_file.get_data_file(), input_file.get_size_file(), pwd_to_use);
    a256.encrypt();

    //then clear the hash to avoid security leak
    for(uint64_t idx = 0; idx < pwd_to_use.length(); ++idx)
    {
      pwd_to_use[idx] = static_cast<char>(0xFF);
    }

    //#######################################
    //5a. If i have encrpyt and CMS != 0
    //     then i need to merge the pointer
    //
    //I prepare the merge now because a256
    //it'll be out-of-scope
    //#######################################
    if(a256.get_final_block() == nullptr)
    {
      output_data = a256.get_payload();
      output_length = a256.get_length_payload();
    }
    else
    {
      output_length = a256.get_length_payload();
      output_length += a256.get_final_block_size();
      output_data = new uint8_t[output_length];

      for(auto idx = 0; idx < a256.get_length_payload(); ++idx)
      {
        output_data[idx] = a256.get_payload()[idx];
      }

      for(auto idx = 0; idx < a256.get_final_block_size(); ++idx)
      {
        output_data[idx + a256.get_length_payload()] = a256.get_final_block()[idx];
      }

      need_to_clean = true;
    }
  }

  //#######################################
  //5. Save the file:
  //  a. If i have encrpyt and CMS != 0
  //     then i need to merge the pointer
  //#######################################

  _steganography_rp96_::steganography::hide_file(output_data, output_length, stcstr_hide_img.ToStdString(), stcstr_hide_file.ToStdString(), custom_name);

  if(need_to_clean == true)
  {
    delete[] output_data;
    output_data = nullptr;
  }

  //#######################################
  //6. Show a pop-up to declare success
  //#######################################
  wxMessageBox(SUCCESS_OPERATION, //text
              SUCC_LABEL, //title
              wxOK | wxICON_INFORMATION); //type of message
  event.Skip();
} //void MyFrame::button_calc_hide(wxCommandEvent& event)

void MyFrame::button_calc_seek(wxCommandEvent& event)
{
  const uint64_t FLAGGED = 1;

  //#######################################
  //SEEK NEEDS THE FOLLOWS INSTRUCTIONS:
  //1. Check if can I use the img choosen;
  //2. Extract the file found;
  //3. Control if I want to decrypt:
  //  a. Control if I want to use custom password:
  //    I. if I want I calc sha256(custom pwd);
  //    II. if I want but custom pwd is empty,
  //        i calc sha256 on empty string;
  //  b. else use local pwd;
  //  c. start to decrypt;
  //4. Choose the name new file;
  //5. Save the file:
  //  a. If i have encrpyt and CMS != 0
  //     then i need to merge the pointer
  //6. Show a pop-up to declare success
  //#######################################

  //#######################################
  //1. Check if can I use the file choosen
  //#######################################
  //check if the img is selected
  if(stcstr_seek_img.IsEmpty())
  {
    wxMessageBox(WARNING_IMG_NEEDED, WARN_LABEL, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  //check if can i open the img
  std::ifstream test_open_img(stcstr_seek_img.ToStdString());
  if (test_open_img.is_open() == false)
  {
    wxMessageBox(ERROR_FILE_NOT_OPEN, WARN_LABEL, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }
  test_open_img.close();

  //#######################################
  //2. Extract the file found;
  //#######################################

  //file name
  std::string seek_name;
  //length file to seek
  int64_t seek_found_length = 0;
  //data file to seek
  uint8_t* seek_found_data = _steganography_rp96_::steganography::seek_file(stcstr_seek_img.ToStdString(), seek_found_length, seek_name);

  //#######################################
  //3. Choose the name new file;
  //#######################################

  //path new file
  wxTextCtrl* label_name_file = reinterpret_cast<wxTextCtrl*>(FindWindowById(seek_txtctrl_outname));
  std::string name_file_found;
  std::filesystem::path seek_name_path{seek_name};

  //if no name custom
  if(label_name_file->GetValue().IsEmpty() == true)
  {
    //then use the real
    name_file_found = std::string(seek_name_path.stem());
  }
  else
  {
    //else use custom
    name_file_found = std::string(label_name_file->GetValue().ToStdString());
  }

  std::string tmp_str = name_file_found;
  tmp_str += seek_name_path.extension();
  std::filesystem::path new_file_check_path{tmp_str};
  if(std::filesystem::exists(new_file_check_path))
  {
    name_file_found += "_";
    name_file_found += std::to_string(check_non_exists_file(name_file_found, seek_name_path.extension()));
  }

  //std::cout << "name_file_found: " << name_file_found << "\n";
  name_file_found += seek_name_path.extension(); //then add extension

  /*
  if(name_file_found)
  {
    std::filesystem::path img_path{stcstr_seek_img.ToStdString()};
    std::string name_file_found(img_path.stem());

    //check if the name already exists
    std::string tmp_name = name_file_found;
    //tmp_name += EXTENSION_NEW_FILE;

    //now tmp_name contains the "standard name" like: file.png
    //check if file.png already exists
    //if true transform file.png into file_.png
    //then search file_1.png, file_2.png ... file_n.png
    //until i found a file that non exists
    std::filesystem::path hypotetical_new_file{tmp_name};
    if(std::filesystem::exists(hypotetical_new_file))
    {
      name_file_found += "_";
      name_file_found += std::to_string(check_non_exists_file(name_file_found, seek_name_path.extension()));
    }
  }
  */

  //#######################################
  //4. Control if I want to decrypt:
  //  a. Control if I want to use custom password:
  //    I. if I want I calc sha256(custom pwd);
  //    II. if I want but custom pwd is empty,
  //        i calc sha256 on empty string;
  //  b. else use local pwd;
  //  c. start to decrypt;
  //#######################################

  //I create a list of a number between 1 and N, the file will contains data
  std::list<_file_operations_rp96_::file_pointer> list_file;

  wxCheckBox* cbox_decrypt = reinterpret_cast<wxCheckBox*>(FindWindowById(seek_cbox_dec));
  if(cbox_decrypt->GetValue() == FLAGGED)
  {
    //#######################################
    //4a. Control if I want to use custom password:
    //#######################################
    wxCheckBox* cbox_custom_pwd = reinterpret_cast<wxCheckBox*>(FindWindowById(seek_cbox_custom_pwd));
    std::string pwd_to_use;

    if(cbox_custom_pwd->GetValue() == FLAGGED)
    {
      //#######################################
      //4aI. if I want I calc sha256(custom pwd);
      //4aII. if I want but custom pwd is empty,
      //      i calc sha256 on empty string;
      //#######################################

      //take pwd
      wxTextCtrl* pwd_custom = reinterpret_cast<wxTextCtrl*>(FindWindowById(seek_txtctrl_pwd));

      //calc sha256
      int64_t tmp_len = pwd_custom->GetValue().ToStdString().length();
      char* tmp_data = new char[tmp_len];

      for(int64_t idx = 0; idx < tmp_len; ++idx)
      {
        tmp_data[idx] = pwd_custom->GetValue().ToStdString().at(idx);
      }

      _sha256_rp96_::sha256 hash_input = _sha256_rp96_::sha256(tmp_data, pwd_custom->GetValue().ToStdString().length());
      hash_input.compute_hash();

      for(int64_t idx = 0; idx < tmp_len; ++idx)
      {
        tmp_data[idx] = static_cast<char>(0xFF);
      }

      delete[] tmp_data;
      tmp_data = nullptr;
      //tmp_len = -1;

      //save in pwd_to_use
      pwd_to_use = std::string(hash_input.get_SHA256_string());
    }
    else
    {
      //#######################################
      //3b. else use local pwd;
      //#######################################
      pwd_to_use = std::string(this->local_hash_pwd.ToStdString());
    }

    //#######################################
    //c. start to decrypt;
    //#######################################
    _aes256_rp96_::aes256 a256 = _aes256_rp96_::aes256(seek_found_data, seek_found_length, pwd_to_use);
    a256.decrypt();

    //then clear the hash to avoid security leak
    for(uint64_t idx = 0; idx < pwd_to_use.length(); ++idx)
    {
      pwd_to_use[idx] = static_cast<char>(0xFF);
    }

    list_file.push_back(_file_operations_rp96_::file_pointer(a256.get_payload(), a256.get_length_payload(), false));
  }
  else
  {
    list_file.push_back(_file_operations_rp96_::file_pointer(seek_found_data, seek_found_length, false));
  }

  //#######################################
  //5. Save the file:
  //  a. If i have encrpyt and CMS != 0
  //     then i need to merge the pointer
  //#######################################

  //check if i can save the new file
  if(_file_operations_rp96_::file_operations::save_third_file(name_file_found, list_file, std::ofstream::trunc) == -1)
  {
    wxMessageBox(ERROR_FILE_NOT_SAVED, FATAL_ERROR, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  delete[] seek_found_data;
  seek_found_data = nullptr;

  //#######################################
  //6. Show a pop-up to declare success
  //#######################################
  wxMessageBox(SUCCESS_OPERATION, //text
              SUCC_LABEL, //title
              wxOK | wxICON_INFORMATION); //type of message
  event.Skip();
} //void MyFrame::button_calc_seek(wxCommandEvent& event)

void MyFrame::button_calc_aes256(wxCommandEvent& event)
{
  const uint64_t FLAGGED = 1;

  //#######################################
  //AES256 NEEDS THE FOLLOWS INSTRUCTIONS:
  //1. Check if can I use the file choosen;
  //2. Check if I need to use the local pwd or custom:
  //2a. If custom, i need to calc sha256(custom pwd)
  //3. Call AES256 to encrypt or decrypt
  //4. Put the new file with old extension in /users/$username/
  //5. Show a pop-up to declare the finish encrypt/decrypt
  //#######################################

  //#######################################
  //1. Check if can I use the file choosen
  //#######################################
  //open the choosen file
  _file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(stcstr_aes256.ToStdString());

  //file not found
  if(input_file.get_data_file() == nullptr)
  {
    wxMessageBox(ERROR_FILE_NOT_OPEN,
                FATAL_ERROR,
                wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }
  //If I'm here I can use the file

  //#######################################
  //2. Check if I need to use the local pwd or custom
  //#######################################

  //actual 256 bits pwd
  std::string pwd_to_use;

  //check the flag custom pwd
  wxCheckBox* cbox_custom_pwd = reinterpret_cast<wxCheckBox*>(FindWindowById(aes256_cbox_enc));
  if(cbox_custom_pwd->GetValue() == FLAGGED)
  {
    //#######################################
    //2a. If custom, i need to calc sha256(custom pwd)
    //#######################################
    //take pwd
    wxTextCtrl* pwd_custom = reinterpret_cast<wxTextCtrl*>(FindWindowById(aes256_txtctrl_pwd));
    //calc sha256
    int64_t tmp_len = pwd_custom->GetValue().ToStdString().length();
    char* tmp_data = new char[tmp_len];
    for(int64_t idx = 0; idx < tmp_len; ++idx)
    {
      tmp_data[idx] = pwd_custom->GetValue().ToStdString().at(idx);
    }
    _sha256_rp96_::sha256 hash_input = _sha256_rp96_::sha256(tmp_data, pwd_custom->GetValue().ToStdString().length());
    hash_input.compute_hash();
    for(int64_t idx = 0; idx < tmp_len; ++idx)
    {
      tmp_data[idx] = static_cast<char>(0xFF);
    }
    delete[] tmp_data;
    tmp_data = nullptr;
    //tmp_len = -1;
    //save in pwd_to_use
    pwd_to_use = std::string(hash_input.get_SHA256_string());
  }
  else //i want to use user's password
  {
    pwd_to_use = std::string(this->local_hash_pwd.ToStdString());
  }

  //#######################################
  //3. Call AES256 to encrypt or decrypt
  //#######################################
  cbox_custom_pwd = reinterpret_cast<wxCheckBox*>(FindWindowById(aes256_radio_enc));
  //load the input file
  _aes256_rp96_::aes256 a256 = _aes256_rp96_::aes256(input_file.get_data_file(), input_file.get_size_file(), pwd_to_use);

  //very important: overwrite the hash-key to avoid security leak
  for(uint64_t idx = 0; idx < pwd_to_use.length(); ++idx)
  {
    pwd_to_use[idx] = static_cast<char>(0xFF);
  }

  if(cbox_custom_pwd->GetValue() == FLAGGED) // I want to encrypt
  {
    a256.encrypt();
  }
  else  // I want to decrypt
  {
    a256.decrypt();
  }

  //#######################################
  //4. Put the new file with old extension in /users/$username/
  //#######################################

  std::string new_file_name;
  //i have the path to the user file
  const std::filesystem::path user_file{stcstr_aes256.ToStdString().c_str()};

  //custom name
  wxTextCtrl* custom_name = reinterpret_cast<wxTextCtrl*>(FindWindowById(aes256_txtctrl_outname));

  //check if there is a custom name
  if(custom_name->GetValue().IsEmpty() == true)
  {
    //the label name output is empty
    //so I re-use the initial name
    new_file_name = std::string(user_file.stem());
  }
  else
  {
    //the label name output is not empty
    //so I have a name to change
    new_file_name = std::string(custom_name->GetValue().ToStdString());
  }

  //example: desktop/personal/file.txt
  //user_file == desktop/personal/file.txt
  //user_file.parent_path() == desktop/personal/
  //user_file.parent_path().parent_path() == desktop/
  if(user_file.parent_path().parent_path() == this->user_datas &&
    new_file_name.compare(user_file.stem()) == 0)
  {
    new_file_name += "_";
    new_file_name += std::to_string(check_non_exists_file(new_file_name, user_file.extension()));
  }

  //if file doesn't have extension I only change the entire name
  new_file_name += user_file.extension();

  //obtain results and save it
  std::list<_file_operations_rp96_::file_pointer> data_output;
  data_output.push_back(_file_operations_rp96_::file_pointer(a256.get_payload(), a256.get_length_payload(), false));
  if(a256.get_final_block() != nullptr)
  {
    data_output.push_back(_file_operations_rp96_::file_pointer(a256.get_final_block(), a256.get_final_block_size(), false));
  }

  //check if i can save the new file
  if(_file_operations_rp96_::file_operations::save_third_file(new_file_name, data_output, std::ifstream::out | std::ifstream::trunc) == -1)
  {
    wxMessageBox(ERROR_FILE_NOT_SAVED, FATAL_ERROR, wxOK | wxICON_INFORMATION);
    event.Skip();
    return;
  }

  //#######################################
  //5. Show a pop-up to declare the finish encrypt/decrypt
  //#######################################

  wxMessageBox(SUCCESS_OPERATION, //text
              SUCC_LABEL, //title
              wxOK | wxICON_INFORMATION); //type of message
  event.Skip();
} //void MyFrame::button_calc_aes256(wxCommandEvent& event)

void MyFrame::button_calc_dfhl(wxCommandEvent& event)
{
  const uint64_t FLAGGED = 1;

  wxTextCtrl* output_label = reinterpret_cast<wxTextCtrl*>(FindWindowById(dfhl_txtctrl_output));
  output_label->Clear();

  wxRadioButton* without_input = reinterpret_cast<wxRadioButton*>(FindWindowById(dfhl_radio_first));

  if(without_input->GetValue() == FLAGGED)
  {
    _diffie_hellman_rp96_::diffie_hellman diffie_hellman = _diffie_hellman_rp96_::diffie_hellman();
    diffie_hellman.halfkey_generator_256bits();
    output_label->AppendText("256-bits public half-key generated is:\n");
    output_label->AppendText(diffie_hellman.get_public_string());
    output_label->AppendText("\n256-bits secret half-key generated is:\n");
    output_label->AppendText(diffie_hellman.get_private_string());

    event.Skip();
    return;
  }

  wxTextCtrl* input_label_pub = reinterpret_cast<wxTextCtrl*>(FindWindowById(dfhl_txtctrl_pub_input));
  wxTextCtrl* input_label_priv = reinterpret_cast<wxTextCtrl*>(FindWindowById(dfhl_txtctrl_priv_input));

  std::string pub(input_label_pub->GetValue().ToStdString());
  std::string priv(input_label_priv->GetValue().ToStdString());

  bool need_i_exit = false;
  if(string_is_hex_key(pub, BITS256) == false)
  {
    output_label->AppendText("256-bits public key is invalid\n");
    need_i_exit = true;
  }
  if(string_is_hex_key(priv, BITS256) == false)
  {
    output_label->AppendText("256-bits private key is invalid\n");
    need_i_exit = true;
  }
  if(need_i_exit == true)
  {
    event.Skip();
    return;
  }

  _diffie_hellman_rp96_::diffie_hellman diffie_hellman = _diffie_hellman_rp96_::diffie_hellman(priv, pub);
  diffie_hellman.key_generator_from_halfkey_256bits();

  output_label->AppendText("256-bits key generated is:\n");
  output_label->AppendText(diffie_hellman.get_shared_string());

  event.Skip();
  return;
} //void MyFrame::button_calc_dfhl(wxCommandEvent& event)

// ----------------------------------------------------------------------------
// AUXILIARY BUTTONS
// ----------------------------------------------------------------------------

void MyFrame::button_open_file(wxCommandEvent& event)
{
  wxPanel* in_use_panel;
  wxString msg_title;

  if ( FindWindowById(sha256_idpanel)->IsShown() )
  {
    in_use_panel = reinterpret_cast<wxPanel*>(FindWindowById(sha256_idpanel));
    //idx_string = sha256_str;

    msg_title = wxString(wxT("Choose a file to calculate SHA256"));
    //std::cout << "vedo sha256\n";
  }
  else if ( FindWindowById(b64_idpanel)->IsShown() )
  {
    in_use_panel = reinterpret_cast<wxPanel*>(FindWindowById(b64_idpanel));
    //idx_string = b64_str;

    msg_title = wxString(wxT("Choose a file to calculate BASE64"));
    //std::cout << "vedo b64\n";
  }
  else if ( FindWindowById(hide_idpanel)->IsShown() )
  {
    in_use_panel = reinterpret_cast<wxPanel*>(FindWindowById(hide_idpanel));
    //idx_string = hide_file_str;

    msg_title = wxString(wxT("Choose a file to hide"));
    //std::cout << "vedo hide\n";
  }
  else //if ( FindWindowById(aes256_idpanel)->IsShown() )
  {
    const uint64_t FLAGGED = 1;

    in_use_panel = reinterpret_cast<wxPanel*>(FindWindowById(aes256_idpanel));
    //idx_string = aes256_str;

    wxRadioButton* do_i_want_encode = reinterpret_cast<wxRadioButton*>(FindWindowById(aes256_radio_enc));
    if(do_i_want_encode->GetValue() == FLAGGED)
    {
      msg_title = wxString(wxT("Choose a file to encrypt"));
    }
    else
    {
      msg_title = wxString(wxT("Choose a file to decrypt"));
    }
    //std::cout << "vedo aes256\n";
  }

  //open the windows where user can choose the file
  wxFileDialog* choose_file = new wxFileDialog(in_use_panel,
    msg_title,
    wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr,
    wxFD_OPEN | wxFD_FILE_MUST_EXIST
  );

  //if user don't choose a file
  if (choose_file->ShowModal() == wxID_CANCEL)
  {
    return;
  }

  //this->file_open_dir[idx_string].Clear();
  //this->file_open_dir[idx_string] += choose_file->GetPath();
  wxString namefile_str = choose_file->GetFilename();

  if ( FindWindowById(sha256_idpanel)->IsShown() )
  {
    this->stcstr_sha256.Clear();
    this->stcstr_sha256 += choose_file->GetPath();

    wxStaticText* stctxt_choose_file = reinterpret_cast<wxStaticText*>(FindWindowById(sha256_stctxt_choose_file));
    stctxt_choose_file->SetLabel(namefile_str);

    wxCheckBox* cbox_fromfile = reinterpret_cast<wxCheckBox*>(FindWindowById(sha256_cbox_fromfile));
    cbox_fromfile->SetValue(true);

    event.Skip();
    return;
  }
  else if ( FindWindowById(b64_idpanel)->IsShown() )
  {
    this->stcstr_b64.Clear();
    this->stcstr_b64 += choose_file->GetPath();

    wxStaticText* stctxt_choose_file = reinterpret_cast<wxStaticText*>(FindWindowById(b64_stctxt_choose_file));
    stctxt_choose_file->SetLabel(namefile_str);

    wxCheckBox* cbox_fromfile = reinterpret_cast<wxCheckBox*>(FindWindowById(b64_cbox_fromfile));
    cbox_fromfile->SetValue(true);

    event.Skip();
    return;
  }
  else if ( FindWindowById(hide_idpanel)->IsShown() )
  {
    this->stcstr_hide_file.Clear();
    this->stcstr_hide_file += choose_file->GetPath();

    wxStaticText* stctxt_choose_file = reinterpret_cast<wxStaticText*>(FindWindowById(hide_stctxt_choose_file));
    stctxt_choose_file->SetLabel(namefile_str);

    event.Skip();
    return;
  }
  //default
  this->stcstr_aes256.Clear();
  this->stcstr_aes256 += choose_file->GetPath();

  wxStaticText* stctxt_choose_file = reinterpret_cast<wxStaticText*>(FindWindowById(aes256_stctxt_choose_file));
  stctxt_choose_file->SetLabel(namefile_str);

  event.Skip();
  return;
} //void MyFrame::button_open_file(wxCommandEvent& event)

void MyFrame::button_open_img(wxCommandEvent& event)
{
  wxPanel* in_use_panel;
  wxString msg_title;
  wxString wildcard = "All image supported files (*.bmp;*.pbm;*.pgm;*.ppm;*.sr;*.ras;*.jpeg;*.jpg;*.jpe;*.jp2;*.tiff;*.tif;*.png)";
  wildcard += "|*.bmp;*.pbm;*.pgm;*.ppm;*.sr;*.ras;*.jpeg;*.jpg;*.jpe;*.jp2;*.tiff;*.tif;*.png|BMP files (*.bmp)|*.bmp|";
  wildcard += "Portable image formats files (*.pbm;*.pgm;*.ppm)|*.pbm;*.pgm;*.ppm|Sun raster files (*.sr;*.ras)|*.sr;*.ras|";
  wildcard += "JPEG files (*.jpeg;*.jpg;*.jpe)|*.jpeg;*.jpg;*.jpe|JPEG 2000 files (*.jp2)|*.jp2|TIFF files files (*.tiff;*.tif)|*.tiff;*.tif|";
  wildcard += "Portable network graphics files (*.png)|*.png";

  if ( FindWindowById(hide_idpanel)->IsShown() )
  {
    in_use_panel = reinterpret_cast<wxPanel*>(FindWindowById(hide_idpanel));
    //idx_string = hide_img_str;

    msg_title = wxString(wxT("Choose an img where to hide"));
  }
  else //if ( FindWindowById(seek_idpanel)->IsShown() )
  {
    in_use_panel = reinterpret_cast<wxPanel*>(FindWindowById(seek_idpanel));
    //idx_string = seek_img_str;

    msg_title = wxString(wxT("Choose an img where to seek"));
  }

  //open the windows where user can choose the file
  wxFileDialog* choose_file = new wxFileDialog(in_use_panel,
    msg_title,
    wxEmptyString, wxEmptyString,
    wildcard,
    wxFD_OPEN | wxFD_FILE_MUST_EXIST
  );

  //if user don't choose a file
  if (choose_file->ShowModal() == wxID_CANCEL)
  {
    return;
  }

  //this->file_open_dir[idx_string].Clear();
  //this->file_open_dir[idx_string] += choose_file->GetPath();
  wxString namefile_str = choose_file->GetFilename();

  if ( FindWindowById(hide_idpanel)->IsShown() )
  {
    this->stcstr_hide_img.Clear();
    this->stcstr_hide_img += choose_file->GetPath();

    wxStaticText* stctxt_choose_file = reinterpret_cast<wxStaticText*>(FindWindowById(hide_stctxt_choose_img));
    stctxt_choose_file->SetLabel(namefile_str);

    event.Skip();
    return;
  }
  //default
  this->stcstr_seek_img.Clear();
  this->stcstr_seek_img += choose_file->GetPath();

  wxStaticText* stctxt_choose_file = reinterpret_cast<wxStaticText*>(FindWindowById(seek_stctxt_choose_img));
  stctxt_choose_file->SetLabel(namefile_str);

  event.Skip();
  return;
} //void MyFrame::button_open_file(wxCommandEvent& event)

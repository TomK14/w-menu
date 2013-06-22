//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------
//
// Adapted from "tutorial05" of HGE. To use HGE's hepler classes, you need to include
// libhgetools in the PSP makefile.
//
//-------------------------------------------------------------------------------------

#include "PrecompiledHeader.h"

#include <stdio.h>

#include <JRenderer.h>
#include <JLBFont.h>

#include "dirent.h"
#include <iostream>
#include <fstream>
#include "GameApp.h"
#include "utils.h"
#include "ZipSupport.h"


#if defined (WIN32) || defined (LINUX)
#include <time.h>
#else
#endif

#ifdef WIN32
#pragma warning(disable : 4786)
#include <direct.h>
#define MAKEDIR(name) _mkdir(name)
#else
#include <sys/stat.h>
#define MAKEDIR(name) mkdir(name, 0777)
#endif


JSample * GameApp::menuSfx = NULL;

int GameApp::notHomebrew(const char * name)
{
    int i;
    
    if (strlen(name) == 0) return 1;
    //official games are in the form: XXXX12345
    if (strlen(name) != 9) return 0;
    for (i = 4; i < 9; ++i)
    {
        if (name[i] < '0' || name[i] > '9')
            return 0;
    }
    
    return 1;
}



void GameApp::loadFiles(const char * folder){

  struct dirent *mDit;

  homebrews.clear();
  files.clear();
  SAFE_DELETE(menu);
  menu = new SimpleMenu(0, this,mFont,10,10,"",15);

  base = folder;
  if (base[base.size()-1] != '/') {
    base.append("/");
  }

  DIR *mDip = opendir(base.c_str());

  if (!mDip)
      return;

  int count = std::count(base.begin(), base.end(), '/');
  if (count > 1)
      homebrews.push_back("..");

  while ((mDit = readdir(mDip))){
    /*if (notHomebrew(mDit->d_name)) {
      continue;
    }*/

    if (strcmp(mDit->d_name, ".") == 0) continue;
    if (strcmp(mDit->d_name, "..") == 0) continue; //added above

    //Dirty way to bypass files
    string folder = base;
    folder +=  mDit->d_name;
    std::ifstream file(folder.c_str());
    if(file){
      file.close();
      files.push_back(mDit->d_name);
      continue;
    }

    homebrews.push_back(mDit->d_name);
  }
  closedir(mDip);

  for (size_t i = 0; i < homebrews.size(); ++i) {
      menu->Add(i,homebrews[i].c_str(), "", (i == 0));
  }

  for (size_t i = 0; i < files.size(); ++i) {
      menu->Add(homebrews.size() + i,files[i].c_str());
  }
}

void GameApp::loadWalls(const char * folder){

  struct dirent *mDit;

  DIR *mDip = opendir(folder);

  int i = 0;
  while ((mDit = readdir(mDip))){
    if (strstr(mDit->d_name, ".jpg")==NULL && strstr(mDit->d_name, ".JPG")==NULL)
    {
      continue;
    }

    wallpapers.push_back(mDit->d_name);
  }
  closedir(mDip);
}

/*
 * hexascii to integer conversion
 */
int GameApp::xstrtoi(const char *str, int len) {
	int val;
	int c;
	int i;

	val = 0;
    for (i = 0; i < len; i++){
        c = *(str + i);
        fprintf(stderr, "character: %c", c);
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c = (c - 'A') + 10;
		} else if (c >= 'a' && c <= 'f') {
			c = (c - 'a') + 10;
		} else {
			return 0;
		}
		val *= 16;
		val += c;
	}
	return (val);
}

//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameApp::GameApp()
{
	backTex = NULL;
  backQuad = NULL;
	backTex2 = NULL;
  backQuad2 = NULL;
	mFont = NULL;
  settings = NULL;
  menu = NULL;
  scroller = NULL;
  settings = NULL;
  gameSelected = -1;
  srand((unsigned int)time(0));  // initialize random
  transition = 255;
  timer = 0;
  runSfx = NULL;
  menuSfx = NULL;
  gameToExtract = "";
  currentGameState = GAME_STATE_NONE;
  subMenu = NULL;
  extractFolder = "";
  extractError = 0;

}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameApp::~GameApp()
{

}


// a small function to avoid hexeditors
int GameApp::stringCode(string stuff){
  int result = 0;
  for (size_t i = 0; i < stuff.size();++i){
    result+= stuff[i];
  }
  if (result != 3454) {
      //we create an intentional crash here if somebody hexedited the string
    JLBFont * a = NULL;
    a->GetScale();
  }
 return result;
}

//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Create()
{
JLOG("GameApp:Create 1");
    JFileSystem::init("./"); //to avoid having subfolders
JLOG("GameApp:Create 2");
	JRenderer* renderer = JRenderer::GetInstance();	
JLOG("GameApp:Create 3");    
  loadWalls(".");
  if (wallpapers.size()) {
  JLOG("GameApp:Create 4");
    int rnd = (rand() % wallpapers.size());
    backTex=renderer->LoadTexture(wallpapers[rnd].c_str());
    if (backTex) backQuad = new JQuad(backTex, 0, 0, 480, 272);
  }
JLOG("GameApp:Create 5");  
	// Load a font
	mFont=new JLBFont("font", 11);
JLOG("GameApp:Create 6");    
  scroller = new TextScroller(mFont, SCREEN_WIDTH/2 - 190 , SCREEN_HEIGHT-20,380);
  const char * const CreditsText[] = {
     "Thanks to n00b81, Tyranid, devs of the PSPSDK, Hitmen, Fanjita & Noobz, psp-hacks.com",
     "wMenu uses the JGE++ library ( http://code.google.com/p/wagic )"
  };

  menu = NULL;

#ifdef WIN32
      strcpy(dummy, "D:/Stuff/Programming/wmenu/projects/wmenu/bin/samples");
#else
      strcpy(dummy, "ms0:/PSP/GAME");
#endif
      ebootPath = dummy;
JLOG("GameApp:Create 7");
      JGE* engine = JGE::GetInstance();
  vector<string>argv = engine->GetARGV();
  int settingsAddr = 0;
JLOG("GameApp:Create 8");
    if (argv.size() > 1) {
        string hex = argv[1];
        hex[8] = 0; //bug in HBL ?
        fprintf(stderr, "Location: 0x %s\n", hex.c_str());
        settingsAddr = xstrtoi(hex.c_str(), 8);
    }
    fprintf(stderr, " settingsAddr : %d\n", settingsAddr);
    if (settingsAddr) {
        settings = (tMenuApi *) settingsAddr;
        ebootPath = (void *) settings->filename;
    }

      int size = sizeof(CreditsText) / sizeof(CreditsText[0]);
      string scroll = "wMenu 0.4 by Wololo  http://wololo.net    ";
      stringCode(scroll);
      scroll.append("--- HBL Info ---");

      if (settings) {
        scroll.append(settings->VersionName);
        scroll.append(" by ");
        scroll.append(settings->Credits);
        scroll.append("     ");
      }

      for (int i = 0; i < size; i++){
        scroll.append(CreditsText[i]);
        scroll.append("     ");
      }
      scroller->Add(scroll);
JLOG("GameApp:Create 9");
    loadFiles((char *)ebootPath);
    if (fileExists("menu.wav")) {
        menuSfx = JSoundSystem::GetInstance()->LoadSample("menu.wav");
        //runSfx = JSoundSystem::GetInstance()->LoadSample("run.wav");

        JSoundSystem::GetInstance()->SetSfxVolume(100);
    }
JLOG("GameApp:Create 10");

    currentGameState = GAME_STATE_NONE;
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Destroy()
{

    SAFE_DELETE(backQuad);
    SAFE_DELETE(backTex);
    SAFE_DELETE(backQuad2);
    SAFE_DELETE(backTex2);
    SAFE_DELETE(mFont);
    SAFE_DELETE(menuSfx);
    SAFE_DELETE(runSfx);
    SAFE_DELETE(menu);
    SAFE_DELETE(subMenu);
    currentZipFile.close();

}



void GameApp::startTransition() {
    if (wallpapers.size() > 1) {
      JRenderer* renderer = JRenderer::GetInstance();
    int rnd = (rand() % wallpapers.size());
    backTex2=renderer->LoadTexture(wallpapers[rnd].c_str());
    if (backTex2) backQuad2 = new JQuad(backTex2, 0, 0, 480, 272);
  }
  
  transition = 0;

}

void GameApp::endTransition() {
  if (!backQuad2) return;
  SAFE_DELETE(backQuad);
  SAFE_DELETE(backTex);
  backQuad = backQuad2;
  backTex=backTex2;
  backQuad->SetColor(ARGB(255,255,255,255));
  backQuad2 = NULL;
  backTex2 = NULL;
  transition = 255;
}

//Load the list of files in the install zip if they're not loaded yet
void GameApp::ensureExtractFilesList()
{
    if (extractError)
        return;

    if (!zipFiles.empty())
        return;

    ZipSupport::PreloadZip(gameToExtract.c_str(), zipFiles);
    currentZipFile.open(gameToExtract.c_str(), ios::binary);

    extractFolder = MEMSTICK_ROOT;
    //Look for eboot and guess output folder from there.
    for (map<std::string, ZipSupport::limited_file_info>::iterator it = zipFiles.begin(); it !=zipFiles.end(); ++it) {
         string name = it->first;
         std::transform(name.begin(), name.end(), name.begin(), ::tolower);
         if ((name.find("eboot.pbp") != string::npos) || (name.find("wmenu.bin") != string::npos)) {
             std::stringstream out;
             out << MEMSTICK_ROOT;

             int count = std::count(name.begin(), name.end(), '/');
             switch (count) {
             case 0:
                 {
                 // eboot.pbp
                 int rnd = rand()%10000;
                 out << "PSP/GAME/WMENU_" << rnd << "/";
                 break;
                 }
             case 1:
                 //mygamegame/eboot.pbp
                 out << "PSP/GAME/";
                 break;
             case 2:
                 //game/mygame/eboot.pbp
                 out << "PSP/";
                 break;
             case 3:
                 //psp/game/mygame/eboot.pbp
                 break;
             default:
                 extractError = 1;
                 currentZipFile.close();
             }

             extractFolder = out.str();
             return;

         }
         
    }
    currentZipFile.close();
    return;

}

void GameApp::ensureFolders(string file)
{
    vector<string> folders =  split(file, '/');
    string folder = "";

    //last one is potentially a file name, not a folder
    size_t nb_folders = (file[file.length() - 1] == '/' || file[file.length() - 1] == '\\') ? folders.size() : folders.size() - 1;

    for (size_t i = 0; i < nb_folders; ++i) { 
        folder.append(folders[i]).append("/");
        MAKEDIR(folder.c_str());
    }
}

bool GameApp::extractOneFile()
{
    if (zipFiles.empty())
        return false;

    map<std::string, ZipSupport::limited_file_info>::iterator it = zipFiles.begin();
    ZipSupport::limited_file_info * mCurrentFileInZip = &(it->second);

    size_t size = mCurrentFileInZip->m_Size;
    std::streamoff offset = ZipSupport::SkipLFHdr(currentZipFile, mCurrentFileInZip->m_Offset);
    if (!currentZipFile.seekg(offset))
        return false;

    size_t total = 0;
    char buffer[1024 * 64];
    
    string outputfile = it->first;
    string lcoutputfile = outputfile;
    std::transform(lcoutputfile.begin(), lcoutputfile.end(), lcoutputfile.begin(), ::tolower);
    size_t found = lcoutputfile.find("eboot.pbp");
    if (found != string::npos)
        outputfile = it->first.substr(0, found) + "wmenu.bin" + it->first.substr(found + 9);

    string outputFile = extractFolder + outputfile;
    ensureFolders(outputFile);

    std::ofstream output(outputFile.c_str(), ios::binary);
    currentExtractFile = it->first;
    
    while (total < size) {
        size_t toRead = MIN(size - total, 1024 * 64); // 64kB buffer
        if (currentZipFile.read(buffer, toRead)) {
            output.write(buffer, toRead);
            total+=toRead;
        }
        else
        {
            output.close();
            extractError = 3;
            return false;
        }
    }
    zipFiles.erase(zipFiles.begin());
    output.close();
    return true;
}

//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameApp::Update()
{
    JGE* engine = JGE::GetInstance();

    float dt = engine->GetDelta();
    if (dt > 35.0f)		// min 30 FPS ;)
    dt = 35.0f;

    scroller->Update(dt);

    timer+=dt;
    if (timer > 15){
    startTransition();
    timer = 0;
    };

    transition+=(int)(dt * 300);
    if (transition > 255)
    endTransition();


    if (gameToExtract.size()) {
        if (!subMenu)
        {
            switch(currentGameState) {
            case GAME_STATE_EXTRACTING_DONE:
                subMenu = new SimpleMenu(1, this,mFont,10,200, "installed to " + extractFolder);
                subMenu->Add(MENU_NO, "OK","", true);
                break;
            default:
                //TODO Check contents of file to see if it matches the zip format.
                subMenu = new SimpleMenu(1, this,mFont,10,200, "Do you want to install this homebrew?");
                subMenu->Add(MENU_YES, "yes");
                subMenu->Add(MENU_NO, "Cancel");
                break;
            }
        }
        subMenu->Update(dt);
        switch(currentGameState) {
        case GAME_STATE_EXTRACTING:
            SAFE_DELETE(subMenu);
            ensureExtractFilesList();
            if (extractError) {
                extractError = 0;
                currentGameState = GAME_STATE_EXTRACTING_CANCEL;
                return;
            }
            extractOneFile();
            if (zipFiles.empty()) {
                currentZipFile.close();
                loadFiles(base.c_str()); //reload menu to update folders
                currentGameState = GAME_STATE_EXTRACTING_DONE;
            }
            break;
        case GAME_STATE_EXTRACTING_CANCEL:
            SAFE_DELETE(subMenu);
            gameToExtract = "";
            currentGameState = GAME_STATE_MENU;
            break;
        case GAME_STATE_EXTRACTING_DONE:
            break;
        default:
            break;
        }

        return;
    }

    menu->Update(dt);

  if (gameSelected == -2) {
        loadFiles(base.c_str());
        gameSelected = -1;
  }
  if (gameSelected > -1) {
    engine->End();
    return;
  }
	// exit when the TRIANGLE button is pressed
	if ( engine->GetButtonState(JGE_BTN_CANCEL))		
	{
      strcpy((char *)ebootPath, "quit");
      engine->End();
      return;
	}

	if ( engine->GetButtonState(JGE_BTN_PRI))		
	{
      JRenderer::GetInstance()->ScreenShot("ms0:/picture/wMenu.png");
    }

	if ( engine->GetButtonState(JGE_BTN_PREV))		
	{
      base = "ms0:/PSP/SAVEDATA";
      loadFiles(base.c_str());
      return;
	}

	if ( engine->GetButtonState(JGE_BTN_NEXT))		
	{
      base = "ms0:/PSP/GAME";
      loadFiles(base.c_str());
      return;
	}
    

}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameApp::Render()
{

	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();		

	// clear screen to black
	renderer->ClearScreen(ARGB(0,0,0,0));
  if (backQuad)renderer->RenderQuad(backQuad,0,0);

  if (transition <= 255 && backQuad2) {
    backQuad2->SetColor(ARGB(transition,255,255,255));
    renderer->RenderQuad(backQuad2,0,0);
  }

  if (gameSelected > -1){
      renderer->FillRoundRect(10,110,450,72,5,ARGB(200,0,50,0));
      mFont->DrawString("Launching the homebrew...", 15,115);
      mFont->DrawString("If you see this screen and nothing happens for", 15,127);
      mFont->DrawString("1 minute, please restart your psp and try again.", 15, 138);
      mFont->DrawString("If this problem happens repeatedly for this homebrew,", 15,150);
      mFont->DrawString("it is probably not compatible with HBL", 15,162);
  }else if (menu){
    if (!menu->mCount) {
      renderer->FillRoundRect(10,110,450,60,5,ARGB(200,50,0,0));
      mFont->DrawString("ERROR!", 15,115);
      mFont->DrawString("couldn't find any homebrew in", 15,127);
      mFont->DrawString(base, 15, 138);
      mFont->DrawString("Please check your config file and/or restart HBL", 15,150);
    }else {
      menu->Render();
      mFont->SetScale(1.0);
      mFont->SetColor(ARGB(255,200,200,200));
      mFont->DrawString(base.c_str(), 10, 10);
      mFont->SetColor(ARGB(255,255,255,255));
    }
  }
  mFont->SetScale(1.0);
  mFont->SetColor(ARGB(255,255,255,255));
  if (scroller) scroller->Render();
  renderer->FillRoundRect(270,10,190,28,5,ARGB(200,50,0,0));
   mFont->DrawString("wMenu 0.4 - http://wololo.net", 280,15); 
  mFont->DrawString("X or O to run game, /\\ to exit", 280,30);


    if (gameToExtract.size()) {
        if (subMenu)
            subMenu->Render();

        switch(currentGameState) {
        case GAME_STATE_EXTRACTING:
            renderer->FillRoundRect(5,120,470,30,5,ARGB(200,0,0,0));
            mFont->SetColor(ARGB(255,255,255,255));
            mFont->DrawString(currentExtractFile, 10,130);
            char buffer[512];
            sprintf(buffer, "%d remaining Files", zipFiles.size());
            mFont->DrawString(buffer, 10, 145);
            break;
        default:
            break;
        }

    }


}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameApp::Pause()
{
	
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameApp::Resume()
{
	
}

void GameApp::ButtonPressed(int controllerId, int controlId)
{
    if (controllerId == 1) { //subMenu extract
        if (controlId == MENU_NO) {
            currentGameState = GAME_STATE_EXTRACTING_CANCEL;
            return;
        }else if (controlId == MENU_YES) {
            currentGameState = GAME_STATE_EXTRACTING;
            return;
        }
    }
    else {
        gameSelected = controlId;
        string folder = base;
        if (gameSelected >= 0 && (size_t)gameSelected < homebrews.size()) {
            if (homebrews[gameSelected].compare("..") == 0) {
                // fprintf(stderr, "%s\n", base.c_str());
                int found = folder.find_last_of("/", folder.size() - 2);
                folder = folder.substr(0, found);
                // fprintf(stderr, "%s\n", folder.c_str());
            } else {
                folder +=  homebrews[gameSelected];
            }
        }
        string filename = folder + "/wmenu.bin";
        std::ifstream file(filename.c_str());
        if(!file){
            filename = folder + "/EBOOT.PBP";
            file.open(filename.c_str());
        }

        if (file) {
            strcpy((char *)ebootPath,filename.c_str());
            file.close();
        } else {
            gameSelected = -1; //revert since it's not a game after all
            string filename2 = folder + "/install.zip";
            std::ifstream file2(filename2.c_str());
            if(file2){
                gameToExtract = filename2;
                file2.close();
            }
            else {
            base = folder;
            gameSelected = -2;
            }
        }
    }
  //if (runSfx) JSoundSystem::GetInstance()->PlaySample(runSfx);
}

void GameApp::OnScroll(int inXVelocity, int inYVelocity)
{

}
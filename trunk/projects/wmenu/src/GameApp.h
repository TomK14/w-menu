//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAMEAPP_H_
#define _GAMEAPP_H_

#include <JApp.h>
#include <JGui.h>
#include <JSoundSystem.h>
#include <vector>
#include <map>
#include <string>
using namespace std;
#include "TextScroller.h"
#include "SimpleMenu.h"
#include "ZipSupport.h"
using namespace zip_support;

class JTexture;

#define TEXTURE_WIDTH		256
#define TEXTURE_HEIGHT		256

#ifdef WIN32
#define MEMSTICK_ROOT "D:/programming/wth/projects/wmenu/bin/"
#else
#define MEMSTICK_ROOT "ms0:/"
#endif

typedef struct
{
	unsigned long        APIVersion;
	char       Credits[32];
	char       VersionName[32];
	char       *BackgroundFilename;   
    char        * filename;   
}	tMenuApi;

class GameApp:	public JApp, public JGuiListener
{
	
public:
    enum GAME_STATE
{
    GAME_STATE_NONE = -1,
    GAME_STATE_MENU = 1,
    GAME_STATE_EXTRACTING = 2,
    GAME_STATE_EXTRACTING_CANCEL = 3,
    GAME_STATE_EXTRACTING_DONE = 4,
};

    enum MENU_SELECTIONS
{
    MENU_YES = 100,
    MENU_NO = 200,
};


	GameApp();
	virtual ~GameApp();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();
   virtual void OnScroll(int inXVelocity, int inYVelocity);
  void ButtonPressed(int controllerId, int controlId);
  static JSample * menuSfx;
private:
  JTexture * backTex2;
  JTexture * backTex;
  TextScroller * scroller;
  JQuad * backQuad;
  JQuad * backQuad2;
 	JLBFont* mFont;
  JSample * runSfx;
  string base;
  string gameToExtract;
  int currentGameState;

  tMenuApi * settings;
  void * ebootPath;
  SimpleMenu * menu;
  SimpleMenu * subMenu;
  vector<string>homebrews;
  vector<string>files;
  vector<string>wallpapers;
  map<std::string, ZipSupport::limited_file_info>zipFiles;
  std::ifstream currentZipFile;
  string extractFolder;
  int extractError;
  string currentExtractFile;

  void loadFiles(const char * folder);
  void loadWalls(const char * folder);
  static int xstrtoi(const char *str, int len);
  char dummy[512];
  int stringCode(string stuff);
  int notHomebrew(const char * name);
  int gameSelected;
  float timer;
  int transition;
  void startTransition();
  void endTransition();
  void ensureExtractFilesList();
  bool extractOneFile();
  void ensureFolders(string file);


};

#endif

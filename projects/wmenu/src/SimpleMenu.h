/*
  A class for very simple menus structure
*/
#ifndef _SIMPLEMENU_H_
#define _SIMPLEMENU_H_

#include <string>
#include <JGui.h>
#include <JLBFont.h>

class SimpleMenu:public JGuiController{
 private:
  // See SimpleMenu.cpp for values
  static const unsigned SIDE_SIZE;
  static const unsigned VMARGIN;
  static const unsigned HMARGIN;
  static const signed LINE_HEIGHT;

 private:
  int mHeight, mWidth, mX, mY;
  JLBFont * mFont;
  int maxItems,startId;
  string desc;



 public:
  SimpleMenu(int id, JGuiListener* listener, JLBFont * font , int x, int y, string desc = "", int _maxItems = 20);
  void Render();
  void Update(float dt);
  void Add(int id, const char * Text,string desc = "", bool forceFocus = false);
  void destroy();
  virtual bool CheckUserInput(JButton key);
};


#endif

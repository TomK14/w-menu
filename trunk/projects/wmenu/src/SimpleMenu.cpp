#include "PrecompiledHeader.h"
#include "SimpleMenu.h"
#include "SimpleMenuItem.h"
#include "JTypes.h"



const unsigned SimpleMenu::SIDE_SIZE = 7;
const unsigned SimpleMenu::VMARGIN = 16;
const unsigned SimpleMenu::HMARGIN = 30;
const signed SimpleMenu::LINE_HEIGHT = 13;

SimpleMenu::SimpleMenu(int id, JGuiListener* listener, JLBFont * font, int x, int y, string desc, int _maxItems): JGuiController(JGE::GetInstance(), id, listener), mFont(font), desc(desc){
  mHeight = 2 * VMARGIN;
  mWidth = 0;
  mX = x;
  mY = y;
  startId = 0;
  maxItems = _maxItems;

}


void SimpleMenu::Render() {
  if (0 == mWidth) {
    float sY = (float)(mY + VMARGIN);
    for (int i = startId; i < startId + mCount; ++i) {
      int width = (static_cast<SimpleMenuItem*>(mObjects[i]))->GetWidth();
      if (mWidth < width) mWidth = width;
    }
    int width = (int) mFont->GetStringWidth(desc.c_str());
    if (mWidth < width) mWidth = width;

    mWidth += 2*HMARGIN;
    for (int i = startId; i < startId + mCount; ++i) {
      float y = (float)(mY + VMARGIN + i * LINE_HEIGHT);
      SimpleMenuItem * smi = static_cast<SimpleMenuItem*>(mObjects[i]);
      smi->Relocate(mX + mWidth / 2, (int)y);
      if (smi->hasFocus()) sY = y;
    }
  }

  JRenderer * renderer = JRenderer::GetInstance();

  float height = (float)mHeight;
  int radius = 5;
  renderer->FillRoundRect((float)mX, (float)mY, (float)(mWidth - radius), (float)(height - 2* radius), (float)radius, ARGB(180,0,0,50));

  mFont->SetScale(1.0f);
  for (int i = startId; i < startId + maxItems ; i++){
    if (i > mCount-1) break;
    if ((static_cast<SimpleMenuItem*>(mObjects[i]))->mY - LINE_HEIGHT * startId < mY + height - LINE_HEIGHT + 7) {
      if (static_cast<SimpleMenuItem*>(mObjects[i])->hasFocus()){
        //resources.GetJLBFont(Constants::MAIN_FONT)->DrawString(static_cast<SimpleMenuItem*>(mObjects[i])->desc.c_str(),mX+mWidth+10,mY+15);
        mFont->SetColor(ARGB(255,255,255,0));
      } else {
        mFont->SetColor(ARGB(255,255,255,255));
      }
      (static_cast<SimpleMenuItem*>(mObjects[i]))->RenderWithOffset((float)(-LINE_HEIGHT*startId));
    }
  }
  mFont->SetScale(1.0);
  mFont->SetColor(ARGB(255,255,255,255));
  mFont->DrawString(desc, (float)mX + 5.f, (float)mY + 5.f);
}

void SimpleMenu::Update(float dt){

  JGuiController::Update(dt);
  if (mCurr > startId + maxItems-1)
    startId = mCurr - maxItems +1;
  else if (mCurr < startId)
    startId = mCurr;
}

void SimpleMenu::Add(int id, const char * text,string desc, bool forceFocus){
  SimpleMenuItem * smi = NEW SimpleMenuItem(this, id, mFont, text, 0, mY + VMARGIN + mCount*LINE_HEIGHT, (mCount == 0));
  smi->desc = desc;
  JGuiController::Add(smi);
  if (mCount <= maxItems) mHeight += LINE_HEIGHT;
  if (forceFocus){
    mObjects[mCurr]->Leaving(JGE_BTN_DOWN);
    mCurr = mCount-1;
    smi->Entering();
  }
}


void SimpleMenu::destroy(){
}

bool SimpleMenu::CheckUserInput(JButton key) {
  JButton key2 = key;
  if (key2 == JGE_BTN_SEC) {
    key2 = JGE_BTN_OK;
  }
  return JGuiController::CheckUserInput(key2);
}
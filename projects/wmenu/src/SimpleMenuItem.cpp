#include "PrecompiledHeader.h"

#include "SimpleMenuItem.h"
#include "GameApp.h"
#include <JSoundSystem.h>

SimpleMenuItem::SimpleMenuItem(SimpleMenu* _parent, int id, JLBFont * font, string text, int x, int y, bool hasFocus): JGuiObject(id), parent(_parent), mFont(font), mX(x), mY(y)
{
  mText = text;
  mHasFocus = hasFocus;

  mScale = 1.0f;
  mTargetScale = SCALE_NORMAL;

  if (hasFocus)
    Entering();
}


void SimpleMenuItem::RenderWithOffset(float yOffset)
{
  mFont->SetScale(mScale);
  mFont->DrawString(mText.c_str(), (float)mX, mY + yOffset, JGETEXT_CENTER);
}

void SimpleMenuItem::Render()
{
  RenderWithOffset(0);
}

void SimpleMenuItem::Update(float dt)
{
    if (mScale < mTargetScale)
    {
      mScale += 2.0f*dt;
      if (mScale > mTargetScale)
	mScale = mTargetScale;
    }
  else if (mScale > mTargetScale)
    {
      mScale -= dt;
      if (mScale < mTargetScale)
	mScale = mTargetScale;
	}
}


void SimpleMenuItem::Entering()
{
  mHasFocus = true;
  mTargetScale = SCALE_SELECTED;
  if (GameApp::menuSfx){
    JSoundSystem::GetInstance()->PlaySample(GameApp::menuSfx);
  }
}


bool SimpleMenuItem::Leaving(JButton key)
{
  mHasFocus = false;
  mTargetScale = SCALE_NORMAL;
  return true;
}


bool SimpleMenuItem::ButtonPressed()
{
  return true;
}

void SimpleMenuItem::Relocate(int x, int y)
{
  mX = x;
  mY = y;
}

int SimpleMenuItem::GetWidth()
{
  mFont->SetScale(SCALE_SELECTED);
  return (int) mFont->GetStringWidth(mText.c_str());
}

bool SimpleMenuItem::hasFocus()
{
  return mHasFocus;
}

ostream& SimpleMenuItem::toString(ostream& out) const
{
  return out << "SimpleMenuItem ::: mHasFocus : " << mHasFocus
	     << " ; parent : " << parent
	     << " ; mText : " << mText
	     << " ; mScale : " << mScale
	     << " ; mTargetScale : " << mTargetScale
	     << " ; mX,mY : " << mX << "," << mY;
}

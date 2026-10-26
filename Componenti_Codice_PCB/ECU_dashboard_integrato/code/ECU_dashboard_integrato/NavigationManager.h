#pragma once
#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include "common.h"

// Unico proprietario della navigazione: chiamare da un solo task UI.
class NavigationManager {
public:
  enum Type : uint8_t { SUBMENU, SCREEN, VALUE, ACTION };
  struct Item {
    const char* name;
    Type type;
    const Item* children;
    uint8_t count;
    int* value;
    int minimum, maximum, step;
    void (*action)();
    int screen;
  };
private:
  Adafruit_ILI9341* tft_;
  const Item* root_;
  uint8_t rootCount_;
  struct Level {const Item* items; uint8_t count, selected;};
  Level stack_[8]{};
  uint8_t depth_=0;
  bool menu_=false, editing_=false, dirty_=true;
  int screen_=DASH_MAIN;
  uint32_t lastDraw_=0;
  // Valore temporaneo: durante la modifica NON tocchiamo la variabile reale.
  int pendingValue_=0;
  int originalValue_=0;
public:
  NavigationManager(Adafruit_ILI9341* tft,const Item* root,uint8_t count):tft_(tft),root_(root),rootCount_(count){stack_[0]={root,count,0};}
  bool inMenu()const{return menu_;}
  int screen()const{return screen_;}
  void openMenu(){menu_=true;editing_=false;depth_=0;stack_[0]={root_,rootCount_,0};dirty_=true;}
  void rotate(int steps){
    if(!menu_ || !steps)return;
    Level &l=stack_[depth_];if(!l.count)return;
    if(editing_){const Item &i=l.items[l.selected];if(i.value){
      int64_t v=(int64_t)pendingValue_+(int64_t)steps*i.step;
      if(v<i.minimum)v=i.minimum;
      if(v>i.maximum)v=i.maximum;
      pendingValue_=(int)v; // SOLO anteprima grafica
    }}else{
      int n=(int)l.selected+steps;
      n%=l.count;if(n<0)n+=l.count;l.selected=n;
    }dirty_=true;
  }
  void click(){
    if(!menu_){openMenu();return;}
    Level &l=stack_[depth_];if(!l.count)return;
    const Item &i=l.items[l.selected];
    if(editing_){
      // Commit atomico dal punto di vista del task UI: una sola scrittura,
      // esclusivamente quando l'utente conferma con CLICK.
      const Item &selected=l.items[l.selected];
      if(selected.value && *selected.value!=pendingValue_){
        *selected.value=pendingValue_;
      }
      editing_=false;dirty_=true;return;
    }
    switch(i.type){
      case SUBMENU: if(i.children && i.count && depth_<7){++depth_;stack_[depth_]={i.children,i.count,0};dirty_=true;}break;
      case SCREEN:screen_=i.screen;menu_=false;dirty_=true;break;
      case VALUE:if(i.value){
        originalValue_=*i.value;
        pendingValue_=originalValue_;
        editing_=true;dirty_=true;
      }break;
      case ACTION:if(i.action)i.action();dirty_=true;break;
    }
  }
  void back(){
    if(!menu_){openMenu();return;}
    if(editing_){
      // Pressione lunga = annulla: il puntatore non viene modificato.
      editing_=false;dirty_=true;return;
    }
    if(depth_){--depth_;dirty_=true;}else{menu_=false;screen_=DASH_MAIN;dirty_=true;}
  }
  void render(){
    if(!menu_||!dirty_)return;
    dirty_=false;
    tft_->setRotation(1);tft_->fillScreen(0x0842);
    tft_->fillRect(0,0,320,30,0x18E3);
    tft_->setTextSize(2);tft_->setTextColor(ILI9341_WHITE);
    tft_->setCursor(8,7);tft_->print(depth_?"SOTTOMENU":"MENU PRINCIPALE");
    Level &l=stack_[depth_];
    // Scorrimento verticale: massimo cinque voci per pagina
    int first=l.selected>=5?l.selected-4:0;
    for(int j=0;j<5 && first+j<l.count;j++){
      int idx=first+j,y=36+j*37;
      bool active=idx==l.selected;
      tft_->fillRoundRect(8,y,304,32,4,active?0x1455:0x1906);
      tft_->setTextSize(2);tft_->setTextColor(ILI9341_WHITE);
      tft_->setCursor(15,y+8);tft_->print(l.items[idx].name);
      const Item &it=l.items[idx];
      if(it.type==VALUE && it.value){
        tft_->setCursor(244,y+8);
        tft_->print(active && editing_ ? pendingValue_ : *it.value);
        if(active&&editing_)tft_->drawRect(240,y+3,70,26,ILI9341_YELLOW);
      }else if(it.type==SUBMENU){tft_->setCursor(290,y+8);tft_->print(">");}
    }
    tft_->setTextSize(1);tft_->setTextColor(0x9CD3);
    tft_->setCursor(8,227);tft_->print(editing_?"RUOTA: cambia  CLICK: salva  LUNGO: annulla":"CLICK: entra  LUNGO: indietro");
  }
};
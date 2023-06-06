#include <LiquidCrystal.h>

#define lim 16

void vypocet();
void my_strcat(char *s, char *r);
void ftoa(double d);
bool isOperator(char z);
void lcd_vypis_update();
void lcd_vypis_clear();
void lcd_vysledek();
void lcd_vysledek_clear();
void debug();


char vypis[lim] = "";
char my_znak[2] = {'\0'};
bool flag0 = false;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;

enum Stavy{OPR1, OPE, OPR2, VYS};

Stavy stav = OPR1;

struct KalkulData{
  char opr1[lim];
  char opr2[lim];
  char opr2_copy[lim];
  char ope;
  char vys[lim];
  bool dot1;
  bool dot2;
};

KalkulData kalkul_data = {
  "",
  "",
  "",
  '\0',
  "",
  false,
  false
};

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);


void zpracuj_znak(char znak){
  my_znak[0] = znak;
  //debug();
  switch(stav){
      
      case OPR1:{
                if(!strcmp(kalkul_data.opr1, "") && znak == '-'){
                   strcpy(kalkul_data.opr1, my_znak);
                    //vypis 
                   my_strcat(vypis, my_znak);                      
                }else if(!isdigit(znak) && flag0 && znak != '.'){
                        if(isOperator(znak)){
                          strcpy(kalkul_data.opr1, kalkul_data.vys);
                          kalkul_data.ope = znak;
                          flag2 = true;
                          stav = OPR2;
                          //vypis logika
                          strcpy(vypis, "");
                          my_strcat(vypis, "ANS");
                          my_strcat(vypis, my_znak);                                                      
                        }else if(znak == '=' && flag2){
                              strcpy(kalkul_data.opr1, kalkul_data.vys);
                              strcpy(kalkul_data.opr2, kalkul_data.opr2_copy);                          
                              stav = VYS;
                              //vypis
                              lcd_vypis_clear();
                              my_strcat(vypis, kalkul_data.opr1);
                              my_znak[0] = kalkul_data.ope;
                              my_strcat(vypis, my_znak);
                              my_strcat(vypis, kalkul_data.opr2);                     
                        }
                }else if(isdigit(znak) || znak =='.'){
                          flag0 = false;
                          if(flag3){
                            flag3 = flag2 = false;
                            strcpy(kalkul_data.opr1, "");
                            my_strcat(kalkul_data.opr1, my_znak);                            
                            //vypis logika
                            lcd_vypis_clear();
                            my_strcat(vypis, kalkul_data.opr1);                            
                          }else{ //flag3
                            if(znak != '.' || !kalkul_data.dot1){
                              if(znak == '.') kalkul_data.dot1 = true;
                              my_strcat(kalkul_data.opr1, my_znak);
                              flag2 = false;
                              //vypis
                            my_strcat(vypis, my_znak);
                            }                    
                          }                      
                      if(kalkul_data.opr1[0] == '.') {
                        strcpy(kalkul_data.opr1, "");
                        kalkul_data.dot1 = false;
                        strcpy(vypis, "");
                      }                      
                }else if(isOperator(znak)){
                  kalkul_data.ope = znak;
                  stav = OPR2;
                  //vypis
                  my_strcat(vypis, my_znak);            
                }
      }break;
      case OPR2: {
                   if(!strcmp(kalkul_data.opr2, "") && znak == '-'){
                      strcpy(kalkul_data.opr2, my_znak);                      
                      //vypis
                      my_strcat(vypis, my_znak);                      
                   }else if(isdigit(znak) || znak == '.'){
                          if(znak != '.' || !kalkul_data.dot2){
                              if(znak == '.') kalkul_data.dot2 = true;
                              my_strcat(kalkul_data.opr2, my_znak);
                              if(kalkul_data.opr2[0] == '.'){
                                strcpy(kalkul_data.opr2, "");
                                kalkul_data.dot2 = false;
                                //strcpy(vypis, "");
                                return;
                              }                             
                            //vypis
                            my_strcat(vypis, my_znak);
                          }                            
                   }else if(znak == '=' && strcmp(kalkul_data.opr1, "") != 0 && strcmp(kalkul_data.opr2, "") != 0){
                     stav = VYS;
                   } 
      }break;
      case VYS: {
                  vypocet();
                  flag0 = flag3 =true;
                  strcpy(kalkul_data.opr2_copy, kalkul_data.opr2);
                  strcpy(kalkul_data.opr2, "");
                  stav = OPR1;
                  kalkul_data.dot1 = false;
                  kalkul_data.dot2 = false;
                  //vypis
                  lcd_vysledek_clear();
                  lcd_vysledek();
                  lcd_vypis_clear();                  
      }break;
  }
}


void lcd_vysledek_clear(){
  lcd.setCursor(0,1);
  for(uint8_t i = 0; i < 16; i++)
     lcd.print(' ');
}


void lcd_vypis_update(){
  lcd.setCursor(0,0);
  lcd.print(vypis);
}

void lcd_vypis_clear(){
  strcpy(vypis, "");
  for(uint8_t i = 0; i < 16; i++){
     lcd.setCursor(i,0);   
     lcd.print(' ');
  }
}

void lcd_vysledek(){
  lcd.setCursor(0,1);
  lcd.print(kalkul_data.vys);
}

bool isOperator(char z){
  return (z == '+' || z == '-' || z == '*' || z == '/') ? true : false;
}

void vypocet(){
  // memory, current
  double opr1 = atof(kalkul_data.opr1);
  double opr2 = atof(kalkul_data.opr2);
  double vys = 0;
  
  switch(kalkul_data.ope){
    case '+': 
              vys = opr1 + opr2;
    break;
    case '-':
              vys = opr1 - opr2;
    break;
    case '/': 
              if(opr2 == 0) break;
              vys = opr1 / opr2;
    break;
    case '*':
              vys = opr1 * opr2;
    break;
  }
  ftoa(vys);
}


void my_strcat(char *s, char *r){
  if(strlen(s) > lim) return;
  strcat(s, r);
}

void ftoa(double d){
  int64_t id = d * 100;
  if(id == 0){
    strcpy(kalkul_data.vys, "0.00");
    return;
  }
  int64_t pom = id;
  int8_t dec, i, kon, znamenko;
  int znak;
  for(dec = 0; id; id /= 10, dec++);
  
  char* cislo = NULL;
  
  if(d > -1 && d < 1){
    znamenko = (d < 0) ? 1 : 0;
    cislo = new char[5 + znamenko]{'\0'};
    i = 0;
    if(znamenko) {
      cislo[i++] = '-';
      pom *= -1;
    }
    cislo[i++] = '0';
    cislo[i] = '.';
    i = 3 + znamenko;
    while(cislo[i] != '.'){
      znak = pom % 10;
      cislo[i--] = '0' + znak;
      pom /= 10;
    }
    cislo[4 + znamenko] = '\0';
    strcpy(kalkul_data.vys, cislo);
  delete [] cislo;
    return;
  }
  
  if(pom < 0){
    cislo = new char[dec + 3]{'\0'};
    i = dec + 1;
    dec += 1;
    cislo[0] = '-';
    kon = 1;
    pom *= -1;
  }else{
    cislo = new char[dec + 2]{'\0'};
    i = dec;
    kon = 0;
  }
  while(i >= kon){
     znak = pom % 10;
     if(dec - i == 2){
      cislo[i--] = '.';
      if(i < 0) break;
      cislo[i] = '0' + znak;
     }else cislo[i] = '0' + znak; 
     
     pom /= 10;
     i--;
  }
  cislo[dec + 1] = '\0';
  strcpy(kalkul_data.vys, cislo);
  delete [] cislo;
}

void debug(){
  Serial.print("flag2: ");
  Serial.println(flag2);
}

#ifndef bmp_cpp
#define bmp_cpp
#define cr "  Company \"Do\"  Колянов Алексей  WinLogoChanger v2.1 from 10.02.02"
#include <scr\screen.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Tscreen scr;
fstream bmpf;

typedef unsigned char byte;

byte DaysInMonth[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

char *Month[12]={"Январь ","Февраль ","Март ","Апрель ","Май ","Июнь ",
                 "Июль ","Август ","Сентябрь ","Октябрь ","Ноябрь ","Декабрь "};

byte *font;
byte fonth=19,//16,
     fontw=8;

#define MAXERRORST 3
char *errst[MAXERRORST]={"",
                         "data file corrupt...",
                         "unable to create file..."};

char ppath[128]={0};

void error(byte errn)
{
  scr.color=12;
  scr<<" Error: "<<errst[errn];
  scr.color=7;
  scr<<"\n";
  delete []font;
  exit(2);
}

int wftxt(fstream &bmpf, int x, int y, byte symb, byte col)
{
  bmpf.seekp((long)320*(400-y)+x+1078, ios::beg);
  for (int j=0; j<fonth; j++)
  {
    byte smb=*(font+symb*fonth+j);
    for (int i=0; i<fontw; i++)
    {
      if (smb&((int)pow(2, 7-i))) bmpf.put(col); else bmpf.seekp(1, ios::cur);
    }
    bmpf.seekp(-328, ios::cur);
  }
  return 1;
}

void wr(fstream &fb, int x, int y, char *st, byte colr)
{
  int k=strlen(st);
  for (int i=0; i<k; i++)
  {
     wftxt(fb, x+10*i+1, y+1, st[i], 253);
     wftxt(fb, x+10*i, y, st[i], colr);
  }
}

void helper()
{
  scr.color=14;
  scr<<" Программа для замены стандартной заставки Windows календарем на любой месяц.\n";
  scr.color=10;
  scr<<"    Ключи Программы :\n";
  scr.color=9;
  scr<<"   /? ";
  scr.color=11;
  scr<<"        -  Эта справка.\n";
  scr.color=9;
  scr<<"   /d<m>.<y> ";
  scr.color=11;
  scr<<" - Создание календаря на m месяц y года (в файле logo.sys).\n";
  scr.color=9;
  scr<<"   \\<dir> ";
  scr.color=11;
  scr<<" - Каталог, в котором будет создан файл logo.sys (умлч. - текущий).\n";
  scr.color=3;
  scr<<"  Например: ";
  scr.color=9;
  scr<<"logochng.exe /d12.2002 ";
  scr.color=3;
  scr<<"- создание календаря на декабрь 2002 года\n";
  scr<<"   в bmp-файле logo.sys.  Скопируйте этот файл в корневой каталог диска C:\n";
  scr.color=5;
  scr<<cr;
  scr.color=7;
  scr<<"\n";
}

int leapy(int OnYear)
{
  if ((OnYear%4==0)&&!((OnYear%100==0)&&(OnYear%400!=0))) return 1;
                                                     else return 0;
}

void main(int argc, char *argv[])
{
/*  if (argc==1)
  {
    helper();
    return;
  }*/
  char *st=argv[0];
  unsigned int year;
  byte month;
  asm{
    mov ah, 0x2A
    int 0x21
    mov year, cx
    mov month, dh
  }
  for (int i=1; i<argc; i++)
  {
    char *strk=argv[i];
    if (*strk=='/') strk++;
    if (*strk=='\\')
    {
      strcpy(ppath, strk+1);
      int l=strlen(ppath);
      if (ppath[l-1]!='\\')
      {
        ppath[l-1]='\\';
        ppath[l]=0;
      }
    }
    if (*strk=='d')
    {
      strk++;
      char *cPtr1=strchr(strk, '.');
      if (!cPtr1) continue;
      *cPtr1=0;
      cPtr1++;
      int nm;
      if ((nm=atoi(strk))>12) continue;
      month=nm;
      if ((nm=atoi(cPtr1))<1980) continue;
      year=nm;
    }else
    if (*strk=='?') {helper(); return;} else;
  }
  strcat(ppath, "logo.sys");
  i=strlen(st);
  st[i-3]='D';
  st[i-2]='A';
  st[i-1]='T';
  fstream fs(st, ios::in|ios::binary);
  if (!fs) error(1);
  byte smb;
  fs.get(smb);
  if (smb!=15) error(1);
  font=new byte[256*fonth];
  for (i=0; i<256*fonth; i++) fs.get(font[i]);
  bmpf.open(ppath, ios::out|ios::binary);
  if (!bmpf) error(2);
  for (long l=0; l<129078; l++)
  {
    fs.get(smb);
    bmpf.put(smb);
  }
  bmpf.close();
  fs.close();
  bmpf.open(ppath, ios::in|ios::out|ios::binary);
  strcpy(st, Month[month-1]);
  char st1[30];
  itoa(year, st1, 10);
  strcat(st, st1);
  strcat(st, "г.");
  wr(bmpf, 165-strlen(st)*5, 140, st, 173);
  if (month==1&&leapy(year)) DaysInMonth[1]=29;
  byte fdm=2;
  for (i=1980; i<year; i++)
  {
    fdm+=1+leapy(i);
    fdm%=7;
  }
  for (i=0; i<month-1; i++) fdm+=DaysInMonth[i]%7;
  fdm%=7;
  if (fdm==0) fdm=7;
  char mst[7];
  int maxj=0;
  for (i=2; i<9; i++)
  {
    st[0]=0;
    for (int j=0; j<6; j++)
    {
      int a=j*7+i-fdm;
      if (a>DaysInMonth[month-1]) break;
      if (a<=0) { strcat(st, "   "); continue; }
      itoa(a, mst, 10);
      for (a=strlen(mst); a<3; a++) strcat(mst, " ");
      strcat(st, mst);
    }
    if (i==2) maxj=j;
    if (j<maxj) strcat(st, "   ");
    j=165-strlen(st)*5;
    wr(bmpf, j, 170+20*(i-2), st, 1);
  }
  bmpf.close();
  delete []font;
  scr.color=5;
  scr<<cr;
  scr.color=7;
  scr<<"\n";
}

#endif
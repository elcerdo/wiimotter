#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <time.h>
#include <sys/timeb.h>

#include <math.h>

#include <cwiid.h>

#include "sdl_utils.h"

#define CENTER_NUNCHUNK_X 126
#define CENTER_NUNCHUNK_Y 132
#define RADIUS_OUTER 75
#define FPS_COMMAND 33  // time between 2 frames (in ms)

// Faire un asservissement en FPS avec usleep



SDL_Surface* SDL_screen = NULL;
int SDL_screen_x = 500;
int SDL_screen_y = 450;
int SDL_screen_bpp = 32;
Uint32 SDL_screen_flags = SDL_HWSURFACE; // |SDL_FULLSCREEN;

cwiid_wiimote_t* wiimote;

//-------------------------------
void SDL_window_resize(int width,int height);

void SDL_draw_pixel(SDL_Surface* s,int x, int y, Uint32);

void SDL_draw_bilinear();

void SDL_draw_blocs();

void SDL_eventPump();
//-------------------------------

int nunchunk_x;
int nunchunk_y;

int mainloop=1;


cwiid_err_t err;

void err(cwiid_wiimote_t *wiimote, const char *s, va_list ap){
  if (wiimote)
    printf("%d:", cwiid_get_id(wiimote));
  else printf("-1:");
  vprintf(s, ap);
  printf("\n");
}

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count, union cwiid_mesg mesg[], struct timespec *timestamp){
  int i;

  for (i=0; i < mesg_count; i++){
    switch (mesg[i].type){
      case CWIID_MESG_NUNCHUK:
        nunchunk_x=(int)mesg[i].nunchuk_mesg.stick[CWIID_X];
        nunchunk_y=(int)mesg[i].nunchuk_mesg.stick[CWIID_Y];
        break;
      default: break;
    }
  }
  return;
}

void drawCross(int x, int y, int size, Uint32 color){
  int i;
  for(i=-(size-1)/2;i<(size-1)/2;i++){
    SDL_draw_pixel(SDL_screen, x+i-1, y+i, color);
    SDL_draw_pixel(SDL_screen, x+i, y+i, color);
    SDL_draw_pixel(SDL_screen, x+i+1, y+i, color);
    SDL_draw_pixel(SDL_screen, x-i, y+i-1, color);
    SDL_draw_pixel(SDL_screen, x-i, y+i, color);
    SDL_draw_pixel(SDL_screen, x-i, y+i+1, color);
  }
}

void drawCircle(double center_x, double center_y, int radius, Uint32 color){
    int tetha;
    double x,y;
    for (tetha=0;tetha<12*radius;tetha++){
        x=radius*cos((double)(tetha)/2)+center_x;
        y=radius*sin((double)(tetha)/2)+center_y;
        SDL_draw_pixel(SDL_screen, (int)x, (int)y, color);
    }
}

void drawSq(int center_x, int center_y, int dim, Uint32 color){
  int i,j;
  for (i=center_x-(dim-1)/2;i<=center_x+(dim-1)/2;i++){
    for (j=center_y-(dim-1)/2;j<=center_y+(dim-1)/2;j++){
      SDL_draw_pixel(SDL_screen, i, j, color);
    }
  }
}


int main(int argc, char** argv)
{
  const char adresse[]="00:21:BD:15:09:4F";
  double calibr_nunchunk_x, calibr_nunchunk_y;
  int fps_asserv;
  int time_off=300;
  int score=0;
  struct timeb fps_ti,fps_tf;
  int32_t fps_delta=0;
  Uint32 couleur_gros_cercle;
  Uint32 couleur_petit_cercle;
  int toswii_param,theta;
  int inner_radius=20;
  double rho;
  int center_small_x,center_small_y;


  bdaddr_t bdaddr;  /* bluetooth device address */

  str2ba(adresse,&bdaddr);
  cwiid_set_err(err);

  /* Connect to the wiimote */

  printf("\nSwii-le by Nounours (http://francetest.free.fr/)\n\n\n");
  printf("Put Wiimote in discoverable mode now (press 1+2)...\n\n");
  fflush(stdout);
  
  // --------------------- Initialisation
  if (!(wiimote = cwiid_open(&bdaddr, 0))) {  fprintf(stderr, "Unable to connect to wiimote\n"); return -1; }
  if (cwiid_set_mesg_callback(wiimote, cwiid_callback)) {fprintf(stderr, "Unable to set message callback\n"); return -1; }

  if (cwiid_enable(wiimote, CWIID_FLAG_MESG_IFC)) { fprintf(stderr, "Error enabling messages\n"); return -1; }

  if (cwiid_command(wiimote,CWIID_CMD_RPT_MODE, CWIID_RPT_NUNCHUK) ) { fprintf(stderr, "Error setting report mode\n"); return -1; }

  printf("Wiimote connection: OK\n\n");
  printf("Nunchunk's calibration, hands off ;)\n\n");
  fflush(stdout);
  do{
    calibr_nunchunk_x=nunchunk_x;
    calibr_nunchunk_y=nunchunk_y;
    usleep(10000);
  }while(abs(calibr_nunchunk_x-CENTER_NUNCHUNK_X)+abs(calibr_nunchunk_y-CENTER_NUNCHUNK_Y)>10);


    // Init SDL
  if( SDL_Init(SDL_INIT_VIDEO) < 0 ){ fprintf(stderr, "Unable to initialize SDL: %s\n",SDL_GetError()); return -1; }

  // trap exit()
  atexit(SDL_Quit);

  srand(clock());

  // Init video
  SDL_screen = SDL_SetVideoMode(SDL_screen_x, SDL_screen_y, SDL_screen_bpp, SDL_screen_flags);

  SDL_ShowCursor(SDL_ENABLE);
  
  fps_asserv=10000;
  ftime(&fps_ti);

  toswii_param=0;theta=0;
  while(mainloop){
    toswii_param+=rand()%11-5;
    theta++;
    rho=(double)(RADIUS_OUTER-inner_radius)*cos((double)(toswii_param/10+80)*(double)(theta*M_PI/180000.0));

    center_small_x=calibr_nunchunk_x+(int)(rho*cos((double)(theta)*M_PI/180.0));
    center_small_y=calibr_nunchunk_y+(int)(rho*sin((double)(theta)*M_PI/180.0));

    if ( ((double)nunchunk_x-calibr_nunchunk_x)*((double)nunchunk_x-calibr_nunchunk_x)+((double)nunchunk_y-calibr_nunchunk_y)*((double)nunchunk_y-calibr_nunchunk_y)>((double)RADIUS_OUTER*(double)RADIUS_OUTER)){
     couleur_gros_cercle=SDL_MapRGB(SDL_screen->format, 255, 0, 0);
     score--;
     time_off--;
    }else{
     couleur_gros_cercle=SDL_MapRGB(SDL_screen->format, 255, 255, 255);
     score++;
    }
    if ( ((double)center_small_x-nunchunk_x)*((double)center_small_x-nunchunk_x)+((double)center_small_y-nunchunk_y)*((double)center_small_y-nunchunk_y)>((double)inner_radius*(double)inner_radius)){
     couleur_petit_cercle=SDL_MapRGB(SDL_screen->format, 255, 0, 0);
     score--;
     time_off--;
    }else{
     couleur_petit_cercle=SDL_MapRGB(SDL_screen->format, 0, 255, 0);
     score++;
    }


    SDL_LockSurface(SDL_screen);
    SDL_FillRect(SDL_screen, 0,0);
    drawCircle(2*calibr_nunchunk_x,2*(255-calibr_nunchunk_y),2*RADIUS_OUTER,couleur_gros_cercle);
    drawSq(2*calibr_nunchunk_x,2*(255-calibr_nunchunk_y), 4, SDL_MapRGB(SDL_screen->format, 255, 255, 255));
    drawSq(2*center_small_x,2*(255-center_small_y), 8, SDL_MapRGB(SDL_screen->format, 128, 128, 255));
    drawCircle(2*center_small_x,2*(255-center_small_y), 2*inner_radius, couleur_petit_cercle);
    SDL_UnlockSurface(SDL_screen);
    SDL_UpdateRect(SDL_screen,0,0,0,0); 

    SDL_WarpMouse(2*nunchunk_x,2*(255-nunchunk_y));
    
    if (time_off<=0) mainloop=0;
    if (score>10000) { mainloop=0; printf("MAXIMUM SCORE: TRY NEXT LEVEL\nor with the Nunchunk's joystick between your teeth ;)\n");}
    // printf("fps_asserv=%3.3lf",1000.0/(double)fps_delta);
    printf(" SCORE=%6d, TIME:%3d \r",score,time_off);
    fflush(stdout);
    ftime(&fps_tf);
    fps_delta=((int)fps_tf.time-(int)fps_ti.time)*1000+(fps_tf.millitm-fps_ti.millitm);
    ftime(&fps_ti);
    fps_asserv=fps_asserv-10*(fps_delta-FPS_COMMAND); // I don't care about stability    


    usleep(fps_asserv);
    SDL_eventPump();
  }

  
  printf("\n\n           GAME OVER\n      Votre score est de: %8d \n\n\n",score);

  // pour avoir bonne concience
  SDL_Quit();
  cwiid_disconnect(wiimote);

  return 0;
}

void SDL_draw_pixel(SDL_Surface* s, int x, int y, Uint32 color){
  if(x<=0) return;
  if(y<=0) return;
  if(x>=SDL_screen_x) return;
  if(y>=SDL_screen_y) return;

  SDL_PIXEL_S(s,x,y) = color;
}

void SDL_eventPump(){
  SDL_Event event;
  while(SDL_PollEvent(&event)){
    switch(event.type){
      case SDL_QUIT:
        mainloop=0;
        break;
      case SDL_VIDEORESIZE:
        //SDL_window_resize(event.resize.w,event.resize.h);
        break;
      case SDL_KEYDOWN:
        switch(event.key.keysym.sym){
          case SDLK_ESCAPE:
            SDL_Quit();
            cwiid_disconnect(wiimote);
            exit(0);
            break;
          case SDLK_a:
            break;
          default:break;
        }
        break;
    }
  }
}



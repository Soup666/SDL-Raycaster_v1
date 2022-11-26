//------------------------YouTube-3DSage----------------------------------------
//Full video: https://www.youtube.com/watch?v=gYRrGTC7GtA 
//WADS to move player.

#include <iostream>
#include <SDL2/SDL.h>

//-----------------------------MAP----------------------------------------------
#define mapX  8      //map width
#define mapY  8      //map height
#define mapS 64      //map cube size
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 510
#define R_WINDOW_WIDTH 512
#define R_WINDOW_HEIGHT 320
#define PI   3.141592
#define RAYCOUNT 60

int map[] =
{
    1,1,1,1,1,1,1,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,1,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,	
};


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool W,A,S,D;

void drawMap2D()
{
    int x, y, xo, yo;

    for(y = 0; y < mapY; y++)
    {
        for(x = 0; x < mapX; x++)
        {

            if (map[y * mapX + x] == 1){ 
                SDL_SetRenderDrawColor(renderer, 255,0,0,255);
            } 
            else
            { 
                SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            }

            xo = x * mapS; yo = y * mapS;

            SDL_Rect rect = {xo+1, yo+1, mapS-2, mapS-2};
            SDL_RenderFillRect(renderer, &rect);
        } 
    } 
}
//-----------------------------------------------------------------------------


//------------------------PLAYER------------------------------------------------
float degToRad(int a) { return a*PI/180.0;}
int FixAng(int a){ if(a>359){ a-=360;} if(a<0){ a+=360;} return a;}

float px,py,pdx,pdy,pa;

void drawPlayer2D()
{
    SDL_SetRenderDrawColor(renderer, 255,255,0,255);
    SDL_Rect rect = {px, py, 8, 8};
    SDL_RenderFillRect(renderer, &rect);
}


//---------------------------Draw Rays and Walls--------------------------------
float distance(float ax,float ay,float bx,float by,float ang){ return cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay);}

void drawRays2D()
{
    // Clear background
    SDL_SetRenderDrawColor(renderer, 0,255,255,255); 	
    SDL_Rect rect = {WINDOW_WIDTH / 2, 0, R_WINDOW_WIDTH, 160};
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0,0,255,255); 
    SDL_Rect rect2 = {WINDOW_WIDTH / 2, R_WINDOW_HEIGHT / 2, R_WINDOW_WIDTH, R_WINDOW_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &rect2);
	
    int r,mx,my,mp,dof,side; float vx,vy,rx,ry,ra,xo,yo,disV,disH; 
 
    ra=FixAng(pa+30); //ray set back 30 degrees
 
    int rayCount = 0;
    for(r = 0; r<60; r += 60 / RAYCOUNT)
    {
        //---Vertical--- 
        dof=0; side=0; disV=100000;
        float Tan=tan(degToRad(ra));

        if     (cos(degToRad(ra))> 0.001){ rx=(((int)px>>6)<<6)+64;      ry=(px-rx)*Tan+py; xo= 64; yo=-xo*Tan;} //looking left
        else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6) -0.0001; ry=(px-rx)*Tan+py; xo=-64; yo=-xo*Tan;} //looking right
        else {                             rx=px;                        ry=py; dof=8;}                          //looking up or down. no hit  

        while(dof<8) 
        { 
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                     
            if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);} //hit         
            else{ rx+=xo; ry+=yo; dof+=1;}                                                                            //check next horizontal
        } 
        vx=rx; vy=ry;

        //---Horizontal---
        dof=0; disH=100000;
        Tan=1.0/Tan; 

        if     (sin(degToRad(ra))> 0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;} //looking up 
        else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;      rx=(py-ry)*Tan+px; yo= 64; xo=-yo*Tan;} //looking down
        else{ rx=px; ry=py; dof=8;}                                                                              //looking straight left or right
        
        while(dof<8) 
        { 
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                          
            if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);} //hit         
            else{ rx+=xo; ry+=yo; dof+=1;}                                                                            //check next horizontal
        } 
        
        SDL_SetRenderDrawColor(renderer, 0,200,0,255);
        if(disV<disH){ rx=vx; ry=vy; disH=disV; SDL_SetRenderDrawColor(renderer, 0,150,0,255); }   //horizontal hit first
        SDL_RenderDrawLine(renderer, px, py, rx, ry);                                              //draw 2D ray
            
        int ca=FixAng(pa-ra); disH=disH*cos(degToRad(ca));                                                    //fix fisheye 
        int lineH = (mapS * R_WINDOW_HEIGHT) / (disH); if(lineH > R_WINDOW_HEIGHT){ lineH = R_WINDOW_HEIGHT;} //line height and limit
        int lineOff = (R_WINDOW_HEIGHT / 2) - (lineH>>1);                                                     //line offset
        
        SDL_Rect rect3 = { (WINDOW_WIDTH / 2) + (rayCount++ * (R_WINDOW_WIDTH / RAYCOUNT)), lineOff, (R_WINDOW_WIDTH / RAYCOUNT), lineH};
        SDL_RenderFillRect(renderer, &rect3);


        ra=FixAng(ra-(60 / RAYCOUNT)); //go to next ray
    }
}

//-----------------------------------------------------------------------------

void display()
{   
 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    drawMap2D();
    drawPlayer2D();
    drawRays2D();
}

void buttons(SDL_Window *window, SDL_Renderer *renderer, SDL_Event event) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                SDL_DestroyWindow(window);
                SDL_Quit();
                exit(0);
                break;
            case SDL_KEYDOWN: 
                switch(event.key.keysym.sym) {
                    case SDLK_w: W = true; break;
                    case SDLK_s: S = true; break;
                    case SDLK_a: A = true; break;
                    case SDLK_d: D = true; break;
                    case SDLK_ESCAPE: SDL_DestroyWindow(window); SDL_Quit(); exit(0); break;
                }
                break;
            
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_w: W = false; break;
                    case SDLK_s: S = false; break;
                    case SDLK_a: A = false; break;
                    case SDLK_d: D = false; break;
                }
                break;
        }
    }

    
    if(A){ pa+=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));} 	
    if(D){ pa-=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));} 
    if(W){ px+=pdx*5; py+=pdy*5;}
    if(S){ px-=pdx*5; py-=pdy*5;}
}

int main(int argc, char* argv[])
{ 

    window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_RENDERER_ACCELERATED);
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    px=150; py=400; pa=90;
    pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); 

    bool quit = false;
    while (!quit)
    {
        SDL_Event event;

        buttons(window, renderer, event);
        display();
        SDL_RenderPresent(renderer);
    }
}


#include <stdio.h>
#include "allegro.h"
#include "time.h"
#include "math.h"
#include "conio.h"
#define LONGUEURMAP 10
#define TAILLEMAPX 25
#define TAILLEMAPY 12
#define FOV 90
#define zFar 10000
#define zNear 0.1
#define PI 3.14159265359


typedef struct player
{
    int x,y;
}t_player;

typedef struct Vector3D
{
    double x,y,z;
}t_Vector3D;



void initAllegro()
{
    allegro_init();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0)!=0)
    {
        allegro_message("Prblm gfx");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    install_keyboard();
    install_mouse();
}

int ** loadMapRand()
{
    int **tabRenvoi= malloc(sizeof (int)*TAILLEMAPX);
    for(int i=0;i<TAILLEMAPX;i++)
    {
        tabRenvoi[i]= malloc(sizeof (int)*TAILLEMAPY);
    }

    for(int i=0;i<TAILLEMAPX;i++)
    {
        for(int j=0;j<TAILLEMAPY;j++)
        {
            tabRenvoi[i][j]=rand()%2;
            printf("%d ",tabRenvoi[i][j]);
        }
        printf("\n");
    }
    return tabRenvoi;
}


void mouseMvmtHorizontale(int mousePosX,double *fTheta)
{
    if(mousePosX<mouse_x)
    {
        *fTheta+=0.05f;
        return;
    }
    else if(mousePosX>mouse_x)
    {
        *fTheta-=0.05f;
        return;
    }
    else
    {
        return;
    }
}

void mouseMvmtVerticale(int mousePosY,double *fTheta)
{
    if(mousePosY<mouse_y)
    {
        *fTheta+=0.05f;
        return;
    }
    else if(mousePosY>mouse_y)
    {
        *fTheta-=0.05f;
        return;
    }
    else
    {
        return;
    }
}

int loadMap()
{
    FILE *fp=NULL;
    fp= fopen("../map.txt","r");
    if(fp==NULL)
    {
        allegro_message("prblm ouverture map");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    int **tabRenvoi= malloc(sizeof (int)*TAILLEMAPY);
    for(int i=0;i<TAILLEMAPY;i++)
    {
        tabRenvoi[i]= malloc(sizeof (int)*TAILLEMAPX);
    }
    for(int i=0;i<TAILLEMAPY;i++)
    {
        for(int j=0;j<TAILLEMAPX;j++)
        {
            fscanf(fp,"%d",&tabRenvoi[i][j]);
            printf("%d ",tabRenvoi[i][j]);
        }
        printf("\n");
    }
    return (int) tabRenvoi;
}

void displayMiniMap(t_player player, int **map,BITMAP *buffer)
{
    for(int i=0;i<TAILLEMAPY;i++)
    {
        for(int j=0;j<TAILLEMAPX;j++)
        {
            if(map[i][j]==1)
            {
                rectfill(buffer,i*LONGUEURMAP,j*LONGUEURMAP,(i+1)*LONGUEURMAP,(j+1)*LONGUEURMAP, makecol(255,0,0));
            }
            if(map[i][j]==0)
            {
                rectfill(buffer,i*LONGUEURMAP,j*LONGUEURMAP,(i+1)*LONGUEURMAP,(j+1)*LONGUEURMAP, makecol(255,255,255));
            }
        }
    }
    circlefill(buffer,player.x,player.y,LONGUEURMAP/2, makecol(255,255,0));
}

int mouvementPlayer(t_player *player,char *mvmt)
{
    if(keypressed())
    {
        *mvmt=(char)readkey();
        return 1;
    }
    else
    {
        return 0;
    }
}


void MultiplicationMatrix(t_Vector3D *vector1,t_Vector3D *vector2, double projectionMatrix[4][4])
{
    vector2->x=vector1->x*projectionMatrix[0][0]+vector1->y*projectionMatrix[1][0]+vector1->z*projectionMatrix[2][0]+projectionMatrix[3][0];
    vector2->y=vector1->x*projectionMatrix[0][1]+vector1->y*projectionMatrix[1][1]+vector1->z*projectionMatrix[2][1]+projectionMatrix[3][1];
    vector2->z=vector1->x*projectionMatrix[0][2]+vector1->y*projectionMatrix[1][2]+vector1->z*projectionMatrix[2][2]+projectionMatrix[3][2];
    double w=vector1->x*projectionMatrix[0][3]+vector1->y*projectionMatrix[1][3]+vector1->z*projectionMatrix[2][3]+projectionMatrix[3][3];
    if(w!=0)
    {
        vector2->x/=w;
        vector2->y/=w;
        vector2->z/=w;

    }
}

void MultiplicationMatrixForRotation(t_Vector3D *vector1,t_Vector3D *vector2, double projectionMatrix[4][4])
{
    vector2->x=vector1->x*projectionMatrix[0][0]+vector1->y*projectionMatrix[1][0]+vector1->z*projectionMatrix[2][0]+projectionMatrix[3][0];
    vector2->y=vector1->x*projectionMatrix[0][1]+vector1->y*projectionMatrix[1][1]+vector1->z*projectionMatrix[2][1]+projectionMatrix[3][1];
    vector2->z=vector1->x*projectionMatrix[0][2]+vector1->y*projectionMatrix[1][2]+vector1->z*projectionMatrix[2][2]+projectionMatrix[3][2];
    double w=vector1->x*projectionMatrix[0][3]+vector1->y*projectionMatrix[1][3]+vector1->z*projectionMatrix[2][3]+projectionMatrix[3][3];

}


void rotationMatrixCube(t_Vector3D *rotatedVector,double matRotz[4][4],double matRotx[4][4],double matRoty[4][4],t_Vector3D *cube)
{
    for(int i=0;i<8;i++)
    {
        MultiplicationMatrixForRotation(&cube[i],&rotatedVector[i],matRotz);
        MultiplicationMatrixForRotation(&rotatedVector[i],&rotatedVector[i],matRoty);
        MultiplicationMatrixForRotation(&rotatedVector[i],&rotatedVector[i],matRotx);
    }
}

void rotationMatrixTriangle(t_Vector3D *rotatedVector,double matRotz[4][4],double matRotx[4][4],double matRoty[4][4],t_Vector3D *triangle)
{
    for(int i=0;i<5;i++)
    {
        MultiplicationMatrix(&triangle[i], &rotatedVector[i],  matRotz);
        MultiplicationMatrix(&rotatedVector[i],&rotatedVector[i],matRoty);
        MultiplicationMatrix(&rotatedVector[i],&rotatedVector[i],matRotx);
    }
}


void rotationMatrixAxes(t_Vector3D *rotatedVector,double matRotz[4][4],double matRotx[4][4],double matRoty[4][4],t_Vector3D *axes)
{
    for(int i=0;i<4;i++)
    {
        MultiplicationMatrix(&axes[i],&rotatedVector[i],matRotz);
        MultiplicationMatrix(&rotatedVector[i],&rotatedVector[i],matRoty);
        MultiplicationMatrix(&rotatedVector[i],&rotatedVector[i],matRotx);
    }
}



void displayProjectedVectorCube(t_Vector3D *projectedVector,BITMAP *buffer)
{
    circlefill(buffer,projectedVector[0].x,projectedVector[0].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"0",projectedVector[0].x,projectedVector[0].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[1].x,projectedVector[1].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"1",projectedVector[1].x,projectedVector[1].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[2].x,projectedVector[2].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"2",projectedVector[2].x,projectedVector[2].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[3].x,projectedVector[3].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"3",projectedVector[3].x,projectedVector[3].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[4].x,projectedVector[4].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"4",projectedVector[4].x,projectedVector[4].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[5].x,projectedVector[5].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"5",projectedVector[5].x,projectedVector[5].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[6].x,projectedVector[6].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"6",projectedVector[6].x,projectedVector[6].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[7].x,projectedVector[7].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"7",projectedVector[7].x,projectedVector[7].y, makecol(255,255,255),-1);

    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[1].x,projectedVector[1].y, makecol(255,0,0));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[3].x,projectedVector[3].y, makecol(255,0,0));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[5].x,projectedVector[5].y, makecol(255,0,0));
    line(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[2].x,projectedVector[2].y, makecol(255,0,0));
    line(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[4].x,projectedVector[4].y, makecol(255,0,0));
    line(buffer,projectedVector[2].x,projectedVector[2].y,projectedVector[3].x,projectedVector[3].y, makecol(255,0,0));
    line(buffer,projectedVector[2].x,projectedVector[2].y,projectedVector[6].x,projectedVector[6].y, makecol(255,0,0));
    line(buffer,projectedVector[3].x,projectedVector[3].y,projectedVector[7].x,projectedVector[7].y, makecol(255,0,0));
    line(buffer,projectedVector[4].x,projectedVector[4].y,projectedVector[5].x,projectedVector[5].y, makecol(255,0,0));
    line(buffer,projectedVector[5].x,projectedVector[5].y,projectedVector[7].x,projectedVector[7].y, makecol(255,0,0));
    line(buffer,projectedVector[6].x,projectedVector[6].y,projectedVector[7].x,projectedVector[7].y, makecol(255,0,0));
    line(buffer,projectedVector[6].x,projectedVector[6].y,projectedVector[4].x,projectedVector[4].y, makecol(255,0,0));
}

void displayProjectedVectorTriangle(t_Vector3D *projectedVector,BITMAP *buffer)
{
    circlefill(buffer,projectedVector[0].x,projectedVector[0].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"0",projectedVector[0].x,projectedVector[0].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[1].x,projectedVector[1].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"1",projectedVector[1].x,projectedVector[1].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[2].x,projectedVector[2].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"2",projectedVector[2].x,projectedVector[2].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[3].x,projectedVector[3].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"3",projectedVector[3].x,projectedVector[3].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[4].x,projectedVector[4].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"4",projectedVector[4].x,projectedVector[4].y, makecol(255,255,255),-1);

    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[1].x,projectedVector[1].y, makecol(0,0,255));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[2].x,projectedVector[2].y, makecol(0,0,255));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[3].x,projectedVector[3].y, makecol(0,0,255));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[4].x,projectedVector[4].y, makecol(0,0,255));

    line(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[2].x,projectedVector[2].y, makecol(0,0,255));
    line(buffer,projectedVector[2].x,projectedVector[2].y,projectedVector[3].x,projectedVector[3].y, makecol(0,0,255));
    line(buffer,projectedVector[3].x,projectedVector[3].y,projectedVector[4].x,projectedVector[4].y, makecol(0,0,255));
    line(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[4].x,projectedVector[4].y, makecol(0,0,255));

}

void displayProjectedVectorAxes(t_Vector3D *projectedVector,BITMAP *buffer)
{
    circlefill(buffer,projectedVector[0].x,projectedVector[0].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"0",projectedVector[0].x,projectedVector[0].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[1].x,projectedVector[1].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"x",projectedVector[1].x,projectedVector[1].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[2].x,projectedVector[2].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"y",projectedVector[2].x,projectedVector[2].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[3].x,projectedVector[3].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"z",projectedVector[3].x,projectedVector[3].y, makecol(255,255,255),-1);

    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[1].x,projectedVector[1].y, makecol(0,0,255));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[2].x,projectedVector[2].y, makecol(255,255,255));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[3].x,projectedVector[3].y, makecol(0,255,0));
}

void displayProjectedVectorSol(t_Vector3D *projectedVector,BITMAP *buffer)
{
    circlefill(buffer,projectedVector[0].x,projectedVector[0].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"0",projectedVector[0].x,projectedVector[0].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[1].x,projectedVector[1].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"1",projectedVector[1].x,projectedVector[1].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[2].x,projectedVector[2].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"2",projectedVector[2].x,projectedVector[2].y, makecol(255,255,255),-1);
    circlefill(buffer,projectedVector[3].x,projectedVector[3].y,1, makecol(255,255,255));
    textout_ex(buffer,font,"3",projectedVector[3].x,projectedVector[3].y, makecol(255,255,255),-1);

    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[1].x,projectedVector[1].y, makecol(0,0,255));
    line(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[3].x,projectedVector[3].y, makecol(0,0,255));
    line(buffer,projectedVector[2].x,projectedVector[2].y,projectedVector[3].x,projectedVector[3].y, makecol(0,0,255));
    line(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[2].x,projectedVector[2].y, makecol(0,0,255));
}


void MiniMap(t_player player, int **map,BITMAP *buffer,char mouvementJoueur)
{
    displayMiniMap(player,map,buffer);
    if(mouvementPlayer(&player,&mouvementJoueur))
    {
        if((mouvementJoueur=='z'||mouvementJoueur=='Z')&&!(player.y-(LONGUEURMAP/2)<=0))
        {
            player.y-=LONGUEURMAP/2;
        }
        if((mouvementJoueur=='s'||mouvementJoueur=='S')&&!(player.y+(LONGUEURMAP/2)>=SCREEN_H))
        {
            player.y+=LONGUEURMAP/2;
        }
        if((mouvementJoueur=='d'||mouvementJoueur=='D')&&!(player.x+(LONGUEURMAP/2)>=SCREEN_W))
        {
            player.x+=LONGUEURMAP/2;
        }
        if((mouvementJoueur=='q'||mouvementJoueur=='Q')&&!(player.x-(LONGUEURMAP/2)<=0))
        {
            player.x-=LONGUEURMAP/2;
        }
    }
}


int main() {
    initAllegro();
    BITMAP *buffer= create_bitmap(SCREEN_W,SCREEN_H);
    t_player player;
    char mouvementJoueur;
    player.x=TAILLEMAPY*LONGUEURMAP/2;
    player.y=TAILLEMAPX*LONGUEURMAP/2;
    int **map=loadMap();
    printf("load rerussi\n");
    char renvoi[60];
    int iteration=0,nbAle;
    srand(time(NULL));
    set_keyboard_rate(1000,100);
    float a=(float)(SCREEN_H)/SCREEN_W;
    double f= 1/ tanf(FOV*PI*0.5/180);
    double fThetaX=0;
    double fThetaY=0;
    double fThetaZ=0;
    double scaling= (zFar/(zFar-zNear))-(zFar*zNear/(zFar-zNear));
    float translationFactorZ=5;
    float translationFactorX=0;
    float translationFactorY=0;
    float scalingFactorX=1;
    float scalingFactorY=1;
    float scalingFactorZ=1;

    int mouseAvantX=0;
    int mouseAvantY=0;

    t_Vector3D cube[8] = {
            { 1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f }
    };


    t_Vector3D triangle3D[5] = {
            { 0, 1.0f, 0 },
            { -1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f },
    };

    t_Vector3D axes[4]=
            {
                    {0,0,0},
                    {1,0,0},
                    {0,1,0},
                    {0,0,1},
            };

    t_Vector3D sol[4]=
            {
                    {-1 ,-1,-1},
                    {1,-1,-1},
                    {-1,1,-1},
                    {1,1,-1},
            };

    double projectionMatrix[4][4]={0};

    projectionMatrix[0][0]=a*f;
    projectionMatrix[1][1]=f;
    projectionMatrix[2][2]=zFar/(zFar-zNear);
    projectionMatrix[3][2]=(-zFar*zNear)/(zFar-zNear);
    projectionMatrix[2][3]=1;


    t_Vector3D translatedVector[8]={0};
    t_Vector3D scalingVector[8]={0};
    t_Vector3D rotatedVector[8]={0};
    t_Vector3D projectedVector[8]={0};

    t_Vector3D rotatedVector2[8]={0};
    t_Vector3D projectedVector2[8]={0};


    t_Vector3D rotatedVector3[5]={0};
    t_Vector3D projectedVector3[5]={0};

    t_Vector3D translatedVector4[8]={0};
    t_Vector3D rotatedVector4[5]={0};
    t_Vector3D projectedVector4[5]={0};

    t_Vector3D translatedVector5[8]={0};
    t_Vector3D rotatedVector5[5]={0};
    t_Vector3D projectedVector5[5]={0};

    double matRotz[4][4]={0};
    double matRotx[4][4]={0};
    double matRoty[4][4]={0};

    double matTranslation[4][4]={0};

    double matScaling[4][4]={0};

    while(!key[KEY_ESC])
    {



        matRotz[0][0]=cos(fThetaZ);
        matRotz[0][1]=sin(fThetaZ);
        matRotz[1][0]=-sin(fThetaZ);
        matRotz[1][1]=cos(fThetaZ);
        matRotz[2][2]=1;
        matRotz[3][3]=1;



        matRotx[0][0]=1;
        matRotx[1][1]=cos(fThetaX*0.5);
        matRotx[1][2]=sin(fThetaX*0.5);
        matRotx[2][1]=-sin(fThetaX*0.5);
        matRotx[2][2]=cos(fThetaX*0.5);
        matRotx[3][3]=1;

        matRoty[0][0]=cos(fThetaY*0.25);
        matRoty[0][2]=-sin(fThetaY*0.25);
        matRoty[1][1]=1;
        matRoty[2][0]=sin(fThetaY*0.25);
        matRoty[2][2]=cos(fThetaY*0.25);
        matRoty[3][3]=1;



        matTranslation[0][0]=1;
        matTranslation[1][1]=1;
        matTranslation[2][2]=1;
        matTranslation[3][3]=1;
        matTranslation[3][0]=translationFactorX;
        matTranslation[3][1]=translationFactorY;
        matTranslation[3][2]=translationFactorZ;


        matScaling[0][0]=scalingFactorX;
        matScaling[1][1]=scalingFactorY;
        matScaling[2][2]=scalingFactorZ;
        matScaling[3][3]=1;

        rotationMatrixCube(rotatedVector,matRotz,matRotx,matRoty,cube);
        //rotationMatrixCube(rotatedVector2,matRotz,matRotx,matRoty,cube2);

        //rotationMatrixTriangle(rotatedVector3,matRotz,matRotx,matRoty,triangle);

        rotationMatrixAxes(rotatedVector4,matRotz,matRotx,matRoty,axes);


        if(key[KEY_E])
        {
            fThetaZ+=0.001f;
        }
        if(key[KEY_R])
        {
            fThetaY+=0.001f;
        }
        if(key[KEY_W])
        {
            fThetaX+=0.001f;
        }


        for(int i=0;i<8;i++)
        {
            MultiplicationMatrix(&rotatedVector[i],&translatedVector[i],matTranslation);
            MultiplicationMatrix(&translatedVector[i],&scalingVector[i],matScaling);
            MultiplicationMatrix(&scalingVector[i],&projectedVector[i],projectionMatrix);
        }
        for(int i=0;i<4;i++)
        {
            translatedVector4[i]=rotatedVector4[i];
            translatedVector4[i].z+=(5.0f+rotatedVector4[i].z);
        }


        for(int i=0;i<4;i++)
        {
            MultiplicationMatrix(&translatedVector4[i],&projectedVector4[i],projectionMatrix);
            MultiplicationMatrix(&translatedVector5[i],&projectedVector5[i],projectionMatrix);
        }

        for(int i=0;i<8;i++)
        {
            projectedVector[i].x=(projectedVector[i].x+1)*SCREEN_W/2;
            projectedVector[i].y=(projectedVector[i].y+1)*SCREEN_H/2;
        }

        for(int i=0;i<4;i++)
        {
            projectedVector4[i].x=(projectedVector4[i].x+1)*SCREEN_W/2;
            projectedVector4[i].y=(projectedVector4[i].y+1)*SCREEN_H/2;
        }


        clear_bitmap(buffer);
        for(int i=0;i<8;i++)
        {
            if(projectedVector[i].x>SCREEN_W)
            {
                printf("en dehors sup x: %d : %f\n",i,projectedVector[i].x);
            }
            else if(projectedVector[i].x<0)
            {
                printf("en dehors inf x: %d : %f\n",i,projectedVector[i].x);
            }
            else if(projectedVector[i].y>SCREEN_H)
            {
                printf("en dehors sup y: %d : %f\n",i,projectedVector[i].y);
            }
            else if(projectedVector[i].y<0)
            {
                printf("en dehors inf y: %d : %f\n",i,projectedVector[i].y);
            }
            else
            {
                /*triangle(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[2].x,projectedVector[2].y,projectedVector[1].x,projectedVector[1].y,
                         makecol(0,0,255));
                triangle(buffer,projectedVector[3].x,projectedVector[3].y,projectedVector[2].x,projectedVector[2].y,projectedVector[0].x,projectedVector[0].y,
                         makecol(0,0,255));
                triangle(buffer,projectedVector[5].x,projectedVector[5].y,projectedVector[7].x,projectedVector[7].y,projectedVector[0].x,projectedVector[0].y,
                         makecol(0,255,255));
                triangle(buffer,projectedVector[3].x,projectedVector[3].y,projectedVector[7].x,projectedVector[7].y,projectedVector[0].x,projectedVector[0].y,
                         makecol(0,255,255));
                triangle(buffer,projectedVector[5].x,projectedVector[5].y,projectedVector[4].x,projectedVector[4].y,projectedVector[0].x,projectedVector[0].y,
                         makecol(255,255,0));
                triangle(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[4].x,projectedVector[4].y,projectedVector[0].x,projectedVector[0].y,
                         makecol(255,255,0));
                triangle(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[4].x,projectedVector[4].y,projectedVector[2].x,projectedVector[2].y,
                         makecol(255,255,0));
                triangle(buffer,projectedVector[6].x,projectedVector[6].y,projectedVector[4].x,projectedVector[4].y,projectedVector[2].x,projectedVector[2].y,
                         makecol(255,255,0));
                triangle(buffer,projectedVector[6].x,projectedVector[6].y,projectedVector[4].x,projectedVector[4].y,projectedVector[5].x,projectedVector[5].y,
                         makecol(255,0,0));
                triangle(buffer,projectedVector[6].x,projectedVector[6].y,projectedVector[7].x,projectedVector[7].y,projectedVector[5].x,projectedVector[5].y,
                         makecol(255,0,0));
                triangle(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[1].x,projectedVector[1].y,projectedVector[2].x,projectedVector[2].y,
                         makecol(255,0,0));
                triangle(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[3].x,projectedVector[3].y,projectedVector[2].x,projectedVector[2].y,
                         makecol(255,0,0));
                triangle(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[1].x,projectedVector[1].y,projectedVector[4].x,projectedVector[4].y,
                         makecol(0,255,0));
                triangle(buffer,projectedVector[0].x,projectedVector[0].y,projectedVector[5].x,projectedVector[5].y,projectedVector[4].x,projectedVector[4].y,
                         makecol(0,255,0));
                triangle(buffer,projectedVector[1].x,projectedVector[1].y,projectedVector[2].x,projectedVector[2].y,projectedVector[4].x,projectedVector[4].y,
                         makecol(0,255,0));
                triangle(buffer,projectedVector[6].x,projectedVector[6].y,projectedVector[2].x,projectedVector[2].y,projectedVector[4].x,projectedVector[4].y,
                         makecol(0,255,0));*/
                displayProjectedVectorCube(projectedVector,buffer);
                displayProjectedVectorAxes(projectedVector4,buffer);
            }

        }
        if(key[KEY_K]){

            translationFactorZ+=0.01f;
        }
        if(key[KEY_L])
        {
            translationFactorZ-=0.01f;
        }
        if(key[KEY_A])
        {
            translationFactorX+=0.01f;
        }
        if(key[KEY_S])
        {
            translationFactorX-=0.01f;
        }
        if(key[KEY_D])
        {
            translationFactorY+=0.01f;
        }
        if(key[KEY_F])
        {
            translationFactorY-=0.01f;
        }
        if(key[KEY_T])
        {
            scalingFactorX+=0.01f;
        }
        if(key[KEY_Y])
        {
            scalingFactorX-=0.01f;
        }
        if(key[KEY_U])
        {
            scalingFactorY+=0.01f;
        }
        if(key[KEY_I])
        {
            scalingFactorY-=0.01f;
        }
        if(key[KEY_O])
        {
            scalingFactorZ+=0.01f;
        }
        if(key[KEY_P])
        {
            scalingFactorZ-=0.01f;
        }
        if(mouse_b==1)
        {
            mouseAvantX=mouse_x;
            mouseAvantY=mouse_y;
            rest(1);
            mouseMvmtHorizontale(mouseAvantX,&fThetaX);
            mouseMvmtVerticale(mouseAvantY,&fThetaY);
        }
        blit(buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
        iteration++;
        iteration%=20;
    }
    allegro_exit();
    return 0;
}
END_OF_MAIN()
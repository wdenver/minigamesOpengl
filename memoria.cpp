
#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <ctime>

struct TCarta{
 int  Tipo ;
 float Scala;
 float Rotacao;
 bool Virada;
 bool Fixa;
};

struct TObjetoFundo{
float x,y,vx,vy,rx,vrx,Tipo,scala,alpha;              
};

struct TSelecCarta{
  int x,y;
};

class TMemoria {    
public:
char texto[30];
GLfloat win;
GLint view_w, view_h;
int cx,cy;
TSelecCarta ultimaSelect;
TSelecCarta penultimaSelect;
TObjetoFundo objetosFundo[100];
TCarta   matrixMemoria[4][4];
int NumObjetosFundo;
int mousex,mousey;
int CartasViradas;
float TempoEspera;
float TempoFimDeJogo;
float TempoMostra;
int Tentativas;
bool JogoFacil;
TObjetoFundo Explode[200];
float ExplodeAlpha;
float contTime;


 
void DesenhaInterrogacao(void)
{
     glBegin(GL_TRIANGLE_STRIP);
       glVertex2f( 0.0f, -6.0f); //1
       glVertex2f( 6.0f, -6.0f); //2
       glVertex2f( 0.0f, 0.0f); //4
       glVertex2f( 6.0f, 0.0f); //3
       glVertex2f( 6.0f, 6.0f); //6
       glVertex2f( 12.0f,6.0f);//5
       glVertex2f( 6.0f, 12.0f);     //8  
       glVertex2f( 12.0f, 12.0f); //7   
       glVertex2f( 10.0f, 16.0f); //    
       glVertex2f( 6.0f, 18.0f);     //9 
       glVertex2f( 6.0f, 12.0f);     //8   
       glVertex2f( 0.0f, 18.0f);     //10        
       glVertex2f( 0.0f, 12.0f);     //11 
       glVertex2f(-4.0f, 16.0f);     // 
       glVertex2f(-6.0f, 12.0f);     //12
       glVertex2f( 0.0f, 12.0f);     //11 
       glVertex2f(-6.0f, 6.0f);     //13  
       glVertex2f( 0.0f, 6.0f);     //14                          
     glEnd();
     
      glBegin(GL_QUADS);
        glVertex2f( 0.0f,-10.0f);     
        glVertex2f( 6.0f,-10.0f);     
        glVertex2f( 6.0f,-16.0f);     
        glVertex2f( 0.0f,-16.0f);
      glEnd();     
} 
 


// Função que desenha um quadrado
void DesenhaQuadrado(void)
{
     glBegin(GL_QUADS);
               glVertex2f(-25.0f, -25.0f);
               glVertex2f(-25.0f, 25.0f);
               glVertex2f(25.0f, 25.0f);
               glVertex2f(25.0f, -25.0f);               
     glEnd();
}


// Função que desenha uma Estrela
void DesenhaEstrela(void)
{
    int curvalados = 5; 
    float cost = curvalados/(6.28);
          
     glBegin(GL_TRIANGLE_FAN); 
      glVertex2f( 0, 0); 
      for(int i=0;i<=curvalados;i++){
            glVertex2f( sin(i/cost)*30, cos(i/cost)*30);
            if (i<curvalados)
             glVertex2f( sin((i+0.5)/cost)*15, cos((i+0.5)/cost)*15);
      }
     glEnd();  
}


// Função que desenha um triângulo
void DesenhaTriangulo(void)
{
     glBegin(GL_TRIANGLES);
               glVertex2f(-25.0f, -25.0f);
               glVertex2f(0.0f, 25.0f);
               glVertex2f(25.0f, -25.0f);              
     glEnd();
}

// Função que desenha um losango
void DesenhaLosango(void)
{
     glBegin(GL_POLYGON);
               glVertex2f(-25.0f, 0.0f);
               glVertex2f(0.0f, 25.0f);
               glVertex2f(25.0f, 0.0f);
               glVertex2f(0.0f, -25.0f);               
     glEnd();
}



void PreecheMatrix(){
int auxX,auxY;
      
 for (int i = 1; i <= 8;i++){
     auxX = rand()%4;
     auxY = rand()%4;    
     
       while(matrixMemoria[auxX][auxY].Tipo!=0){ 
         auxX = rand()%4;
         auxY = rand()%4;   
       }
       matrixMemoria[auxX][auxY].Tipo=i;
       
       while(matrixMemoria[auxX][auxY].Tipo!=0){ 
         auxX = rand()%4;
         auxY = rand()%4;   
       }
       matrixMemoria[auxX][auxY].Tipo=i;       
  }    
}



void DesenhaObjetosExplode(void){   
     
  if (ExplodeAlpha<=-1)
   return;
        
  for (int i = 0; i < 200;i++){      
        glPushMatrix();
        glEnable(GL_BLEND); 
        glColor4f(1.0,1.0,0.5,Explode[i].alpha+ExplodeAlpha);
        glTranslatef(Explode[i].x ,Explode[i].y,0);
        glRotatef(Explode[i].rx,0,0,1);  
        glScalef(Explode[i].scala,Explode[i].scala,1); 
        DesenhaEstrela();        
        glDisable(GL_BLEND);  
        glPopMatrix();        
        Explode[i].x += Explode[i].vx;
        Explode[i].y += Explode[i].vy;  
        Explode[i].rx += Explode[i].vrx;
     
      } 
      
      ExplodeAlpha -= 0.003 ;          
}


void GeraExplode(float x1, float y1, float x2, float y2 ){
    ExplodeAlpha = 0; 
    for (int i = 0; i < 100;i++){
            Explode[i].x = x1;
            Explode[i].y = y1;    
    }
    
    for (int i = 100; i < 200;i++){
            Explode[i].x = x2;
            Explode[i].y = y2;    
    }  
}




void DesenhaObjetosFundo(void){
        
  for (int i = 0; i < NumObjetosFundo;i++){      
   glPushMatrix();
    glEnable(GL_BLEND); 
     glColor4f(1,1.0,1.0,objetosFundo[i].alpha); 
     
         glTranslatef(objetosFundo[i].x ,objetosFundo[i].y,0);
         glRotatef(objetosFundo[i].rx,0,0,1);
     
        glScalef(objetosFundo[i].scala,objetosFundo[i].scala,1); 
      if(objetosFundo[i].Tipo==0) 
        DesenhaLosango();
      else
      if(objetosFundo[i].Tipo==1) 
        DesenhaTriangulo();
      else
       if(objetosFundo[i].Tipo==2) 
        DesenhaEstrela();
      else
        DesenhaQuadrado();  
        
      
      glDisable(GL_BLEND);  
      glPopMatrix();
      
      objetosFundo[i].x += objetosFundo[i].vx;
      objetosFundo[i].y += objetosFundo[i].vy;
      
      objetosFundo[i].rx += objetosFundo[i].vrx;
      
      if (objetosFundo[i].y > 160){
        objetosFundo[i].y = -160;
        objetosFundo[i].x = rand()%300 - 150;
      }    
  }            
}


// desenha o degrade do fundo
void DesenhaFundo(void)
{    
    glBegin(GL_QUADS);
      glColor4f(0.6,0.3,0.5,1); 
      glVertex2f( -win, win );
      glVertex2f( win, win );
      glColor4f(0.2,0.1,0.15,1); 
      glVertex2f( win, -win );  
      glVertex2f( -win, -win );
     glEnd();     
}


// desenha o contorno de uma carta
void DesenhaBordaCarta(float cor1[], float cor2[])
{
   int i;
   float altura = 30;
   float largura = 25;
   float raioBorda = 8;
   int   curvalados = 10;
   float r1,g1,b1,a1;
   float r2,g2,b2,a2;
   
   r1=cor1[0]; g1=cor1[1]; b1=cor1[2];a1=cor1[3];
   r2=cor2[0]; g2=cor2[1]; b2=cor2[2];a2=cor2[3];
   
   
   float cost = curvalados*2/(3.14);
    
    glBegin(GL_TRIANGLE_FAN); 
         glColor4f(r1,g1,b1,a1);   
         glVertex2f(-largura+raioBorda, -altura+raioBorda );
         glColor4f(r2,g2,b2,a2);          
         for(i=0;i<=curvalados;i++)
            glVertex2f(-largura+raioBorda - sin(i/cost)*raioBorda, -altura+raioBorda - cos(i/cost)*raioBorda);   
     glEnd();
     glBegin(GL_TRIANGLE_FAN); 
         glColor4f(r1,g1,b1,a1);    
         glVertex2f(-largura+raioBorda,  altura-raioBorda );
         glColor4f(r2,g2,b2,a2);        
         for(i=0;i<=curvalados;i++)
             glVertex2f(-largura+raioBorda - cos(i/cost)*raioBorda, altura-raioBorda + sin(i/cost)*raioBorda);     glEnd();
     glBegin(GL_TRIANGLE_FAN); 
         glColor4f(r1,g1,b1,a1);  
         glVertex2f( largura-raioBorda,  altura-raioBorda );
         glColor4f(r2,g2,b2,a2);    
         for(i=0;i<=curvalados;i++)
            glVertex2f( largura-raioBorda + sin(i/cost)*raioBorda, altura-raioBorda + cos(i/cost)*raioBorda);
     glEnd();
     glBegin(GL_TRIANGLE_FAN); 
         glColor4f(r1,g1,b1,a1);     
         glVertex2f( largura-raioBorda, -altura+raioBorda );
         glColor4f(r2,g2,b2,a2);      
         for(i=0;i<=curvalados;i++)
            glVertex2f( largura-raioBorda + cos(i/cost)*raioBorda, -altura+raioBorda - sin(i/cost)*raioBorda);                              
     glEnd();
     
     glBegin(GL_QUADS);
      glColor4f(r1,g1,b1,a1);  
      glVertex2f(-largura+raioBorda, -altura+raioBorda );
      glVertex2f(-largura+raioBorda,  altura-raioBorda );
    glColor4f(r2,g2,b2,a2);  
      glVertex2f(-largura,  altura-raioBorda );
      glVertex2f(-largura, -altura+raioBorda );
     glEnd(); 
     
     glBegin(GL_QUADS);
     glColor4f(r1,g1,b1,a1); 
      glVertex2f(largura-raioBorda, -altura+raioBorda );
      glVertex2f(largura-raioBorda,  altura-raioBorda );
     glColor4f(r2,g2,b2,a2); 
      glVertex2f( largura,  altura-raioBorda );
      glVertex2f( largura, -altura+raioBorda );
     glEnd(); 
     
     glBegin(GL_QUADS);
      glColor4f(r1,g1,b1,a1);  
      glVertex2f( -largura+raioBorda, -altura+raioBorda );
      glVertex2f(  largura-raioBorda, -altura+raioBorda );   
     glColor4f(r2,g2,b2,a2); 
      glVertex2f(largura-raioBorda,-altura);
      glVertex2f( -largura+raioBorda,-altura );
     glEnd(); 
     
     glBegin(GL_QUADS);
     glColor4f(r1,g1,b1,a1); 
      glVertex2f( -largura+raioBorda, altura-raioBorda );
      glVertex2f(  largura-raioBorda, altura-raioBorda );   
     glColor4f(r2,g2,b2,a2);  
      glVertex2f(largura-raioBorda,altura);
      glVertex2f( -largura+raioBorda,altura );
     glEnd(); 
     
     glBegin(GL_QUADS);
      glColor4f(r1,g1,b1,a1); 
      glVertex2f( -largura+raioBorda, altura-raioBorda );
      glVertex2f(  largura-raioBorda, altura-raioBorda );   
      glVertex2f(largura-raioBorda, -altura+raioBorda);
      glVertex2f( -largura+raioBorda,-altura+raioBorda );
     glEnd();   
}


void DesenhaCarta(bool brilha){
   float c1[] ={0.0 , 0.0 , 0.0 , 1.0};
   float c2[] ={0.0 , 0.0 , 0.0 , 0.0};   
   float c3[] ={1.0 , 0.9 , 0.9 , 1.0};  
   float c4[] ={0.6 , 0.4 , 0.2 , 1.0};  
   float c5[] ={1.0 , 0.9 , 0.0 , 0.4};  
   float c6[] ={1.0 , 1.0 , 0.0 , 0.0};    
  
  //SOMBRA 
   if (!brilha){  
  glEnable(GL_BLEND);  
   glPushMatrix();
     glScalef(1.2,1.2,1);
      glTranslatef(2,-2,0);
       DesenhaBordaCarta(c1,c2);  
   glPopMatrix(); 
   glDisable(GL_BLEND); 
} 
   
   
     DesenhaBordaCarta(c3,c4);  
   
   if (brilha){  
   glEnable(GL_BLEND);  
   glPushMatrix();
     //glScalef(1.2+sin(contTime)*0.05,1.2+sin(contTime)*0.05,1);
      glScalef(1.2,1.2,1);
      c5[3] = 0.2+(sin(contTime*1)+1)*0.2;
       DesenhaBordaCarta(c5,c6);  
   glPopMatrix(); 
   glDisable(GL_BLEND);  
   }
        
   
  //   glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,'?');       
}

void DesenhaJogo(void){ 
  for(int i = -2  ; i <2 ; i++)  
  for(int j = -2  ; j <2 ; j++){
   if (matrixMemoria[i+2][j+2].Tipo==0)
     continue;       
          
    glPushMatrix();
      glTranslatef(i*60+30,-j*70-35,0);
      //glRotatef(matrixRotacao[i+2][j+2],0,0,1);
      glScalef(matrixMemoria[i+2][j+2].Rotacao,1,1);
      glScalef(matrixMemoria[i+2][j+2].Scala,matrixMemoria[i+2][j+2].Scala,1);
      
     // matrixMemoria[i+2][j+2].Fixa = matrixMemoria[i+2][j+2].Virada;
      
      DesenhaCarta(matrixMemoria[i+2][j+2].Fixa);  
      
      
      if(matrixMemoria[i+2][j+2].Rotacao< 0){
       glPushMatrix();       
        if (matrixMemoria[i+2][j+2].Tipo==1){
            glColor4f(1,0,0,1); 
            glScalef(0.5,0.5,1);
            DesenhaTriangulo();                        
        }else
        if (matrixMemoria[i+2][j+2].Tipo==2){
            glColor4f(0,1,0,1); 
            glScalef(0.5,0.5,1);
            DesenhaEstrela();                        
        }else
        if (matrixMemoria[i+2][j+2].Tipo==3){
            glColor4f(0,0,1,1); 
             glScalef(0.5,0.5,1);
            DesenhaQuadrado();                        
        }
        else
        if (matrixMemoria[i+2][j+2].Tipo==4){
            glColor4f(1,0,1,1); 
             glScalef(0.5,0.5,1);
            DesenhaLosango();                        
        }
        else
        if (matrixMemoria[i+2][j+2].Tipo==5){
            glColor4f(1,0,1,1); 
             glScalef(0.5,0.5,1);
            DesenhaTriangulo();                            
        }
        else
        if (matrixMemoria[i+2][j+2].Tipo==6){
            glColor4f(0,0,1,1); 
             glScalef(0.5,0.5,1);
            DesenhaEstrela();                        
        }
        else
        if (matrixMemoria[i+2][j+2].Tipo==7){
            glColor4f(0,1,0,1); 
             glScalef(0.5,0.5,1);
           DesenhaQuadrado();                           
        }
        else
        if (matrixMemoria[i+2][j+2].Tipo==8){
            glColor4f(1,0,0,1); 
             glScalef(0.5,0.5,1);
             DesenhaLosango();                           
        }    
       glPopMatrix();    
       }else
       {
             glPushMatrix();
             glColor4f(1,1,1,1); 
             glScalef(0.9,0.9,1); 
             glTranslatef(-4.0,0.0,1); 
             DesenhaInterrogacao(); 
             glPopMatrix();
            
       }
      
    glPopMatrix();                
  }       
}
 


void ConfiguraJogo(bool f){
int aux=0;           
   srand (time(0));    
   JogoFacil = f;  
    
     for (int i = 0; i < 4;i++)
     for (int j = 0; j < 4;j++){      
       matrixMemoria[i][j].Scala= 1;
       matrixMemoria[i][j].Tipo = 0;
       matrixMemoria[i][j].Rotacao = 1;
        if (JogoFacil)
          matrixMemoria[i][j].Virada = true;
        else
          matrixMemoria[i][j].Virada = false;  
           
       matrixMemoria[i][j].Fixa   = false;
     }
     
  PreecheMatrix();
        
     
  NumObjetosFundo = 100;

  for (int i = 0; i < NumObjetosFundo;i++){ 
        objetosFundo[i].x = rand()%300-150;
        objetosFundo[i].y = rand()%600-300;
        objetosFundo[i].Tipo = rand()%4;  
        objetosFundo[i].vx = ((rand()%10)/10.0)-0.5;
        objetosFundo[i].vy = ((rand()%11)/10.0)+0.3;
        objetosFundo[i].scala = ((rand()%8)/10.0)+0.2;
        objetosFundo[i].vrx = ((rand()%40)-20)/5.0;
        objetosFundo[i].alpha = 0.05/objetosFundo[i].scala; 
  }     
  
  
   for (int i = 0; i < 200;i++){ 
        Explode[i].x = 0;
        Explode[i].y = 0;
        Explode[i].Tipo = rand()%4;  
        Explode[i].vx = ((rand()%20)/10.0)-1.0;
        Explode[i].vy = ((rand()%20)/10.0)-1.0;
        Explode[i].scala = ((rand()%10)/50.0)+0.1;
        Explode[i].vrx = ((rand()%40)-20)/5.0;
        Explode[i].alpha = 0.5;//0.2/objetosFundo[i].scala; 
   }
  
  if (JogoFacil)
    TempoEspera = 35;
  else
    TempoEspera = 1;
  
    CartasViradas = 0;
    contTime = 0;
    Tentativas = 0;
     
    TempoFimDeJogo = 0; 
    TempoMostra = 0;  
     
    ultimaSelect.x = -1; 
    ultimaSelect.y = -1;   
    penultimaSelect = ultimaSelect;  
    
    ExplodeAlpha = -1;          

}

// Desenha um texto na janela GLUT
void DesenhaTexto(char *string) 
{  
  	glPushMatrix();
        // Posição no universo onde o texto será colocado          
        glRasterPos2f(-win,win-(win*0.08)); 
        // Exibe caracter a caracter
        while(*string)
             glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++); 
	glPopMatrix();
}


// Função callback chamada para fazer o desenho
void Desenha(void)
{  
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
                   
     glClear(GL_COLOR_BUFFER_BIT);    
   
     DesenhaFundo();
     DesenhaObjetosFundo();
     DesenhaJogo();
     DesenhaObjetosExplode();

     glColor3f(1.0f,1.0f,1.0f);
     DesenhaTexto(texto);
     
     glutSwapBuffers();
}


void DesviraCartas(){
  for (int i = 0; i < 4;i++)
  for (int j = 0; j < 4;j++){ 
     if (!matrixMemoria[i][j].Fixa)                  
         matrixMemoria[i][j].Virada = false;
  } 
    CartasViradas = 0;    
    TempoMostra = 0;
}


void LimpaCartasSeIguais(){
     
  for (int i = 0; i < 4;i++)
  for (int j = 0; j < 4;j++){ 
     if (matrixMemoria[i][j].Tipo!=0)                  
         matrixMemoria[i][j].Virada = false;
  } 
    CartasViradas = 0;       
}


void MemoriaUpdate(){
char texto2[30];  

int auxcol,auxLin;

   if (TempoFimDeJogo > 0){
     TempoFimDeJogo-= 0.1;
     
      if (TempoFimDeJogo <= 0)
       ConfiguraJogo(JogoFacil);                  
   } 
   
   
   if (TempoMostra > 0){
     TempoMostra-= 0.1;
     
      if (TempoMostra <= 0)
       DesviraCartas();                  
   } 
   
   

  if (TempoEspera>0){ 
    TempoEspera-= 0.1;
    
    auxLin = 3-((int)TempoEspera)/4;;
    auxcol = 3-((int)TempoEspera)%4;
    
    if (TempoEspera<16) 
      matrixMemoria[auxcol][auxLin].Virada = false;      
    
    if (TempoEspera <= 0)
     DesviraCartas();
  }     
  
  contTime += 0.1;
  
  sprintf(texto, "Erros: %d", Tentativas); 
  
    
  cx = (((mousex+view_w/10)*5)/view_w)-1;
  cy = (((mousey+view_h/4)*4)/view_h)-1;
  
     for (int i = 0; i < 4;i++)
     for (int j = 0; j < 4;j++){
         
     if ((matrixMemoria[i][j].Rotacao >-1)&&(matrixMemoria[i][j].Virada))                    
         matrixMemoria[i][j].Rotacao -= 0.1;
     else
     if ((matrixMemoria[i][j].Rotacao < 1)&&(!matrixMemoria[i][j].Virada)) 
         matrixMemoria[i][j].Rotacao += 0.1;                   
      
       if ((cx==i)&&(cy==j)){
         if (!matrixMemoria[i][j].Virada)                         
         if(matrixMemoria[i][j].Scala <1.1) 
         matrixMemoria[i][j].Scala+= 0.01;  
       }else   
       if (matrixMemoria[i][j].Scala >1) 
         matrixMemoria[i][j].Scala-= 0.01;
     }
}

void SelecionaCarta(){ 
     
  if (TempoEspera > 0)
   return;  
     
  if ((cx>=0)&&(cx<4)&&(cy>=0)&&(cy<4)) 
  if (matrixMemoria[cx][cy].Virada == false){  
                                   
    penultimaSelect = ultimaSelect;
    ultimaSelect.x = cx;  
    ultimaSelect.y = cy;                                  
   
     if (CartasViradas==1) 
    if((ultimaSelect.x>-1)&&(penultimaSelect.x>-1)){
          if (matrixMemoria[ultimaSelect.x][ultimaSelect.y].Tipo==matrixMemoria[penultimaSelect.x][penultimaSelect.y].Tipo){
            matrixMemoria[ultimaSelect.x][ultimaSelect.y].Fixa = true;
            matrixMemoria[penultimaSelect.x][penultimaSelect.y].Fixa = true;
            
            GeraExplode((ultimaSelect.x)*60-90,(-ultimaSelect.y)*70+105,
                        (penultimaSelect.x)*60-90,(-penultimaSelect.y)*70+105); 
             
            
            TempoFimDeJogo = 45;
             TempoEspera = 35; 
            
            for(int i=0 ; i< 4 ; i++)
              for(int j=0 ; j< 4 ; j++)
              if (matrixMemoria[i][j].Fixa == false){                     
                TempoFimDeJogo = 0;
               TempoEspera = 0; 
              }
                                 
          }else
           Tentativas++;

      }
      
      if (CartasViradas==1) 
        TempoMostra = 10;
    
     if (CartasViradas>1){                                                                     
      DesviraCartas();    
     }                           
                                   
    matrixMemoria[cx][cy].Virada = true; 
      
    CartasViradas ++;
  }
          
}     
      
};

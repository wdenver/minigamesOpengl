#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <ctime>

#define TFUNDO  70
#define TAMANHO 125.0
#define TAMANHOEXT 127.0
#define POSY 20
#define ESPACOENTREBARCOS 0
#define NUMPEIXES 100
#define NUMDEVIDAS 2
#define NUMDEBOMBAS 10


// estrutura para a malha de agua
struct TPropFundo{
    float px,py,vx,vy,r,g,b,a;
    float tx,ty,vel;
};

// estrutura para cada célula clicável
struct TCelula{
    int Tipo;       
};


// estrutura de propriedade dos peixes
struct TPeixe{
    float px,py,vx,vy,r,velocidade; 
    float ir; 
    float ivelocidade;  
    float Tempo;  
    float fVira;
    float dvira;
    float Increment;
    float Tamanho;
    bool assustado;
};

// estrutura para Lista de particulas
struct Particula{
    float px,py,vx,vy,rx,vrx;
    float fx,fy;
    float origy;
    float r,g,b;
    float vr,vg,vb;
    float vida;
    float Tamanho;
    float vTamanho;
    Particula *proximo;
    Particula *anterior;       
};

// estrutura para Lista de ondas
struct Onda{
    float px,py;
    float alpha;
    float tamanho;
    Onda *proximo;
    Onda *anterior;       
};



// Classe do Game Batalha Naval
class TBnaval {    

public:
        
int   Vidas;  
bool  Visiveis;    
float Increment;  

bool JogoTerminado;
bool Venceu;
       
char texto[30]; 
char textoVidas[30]; 
char textoBarcos[30];
int embarcacaoAcerto;
int Disparos;
int Faltando;
int TempoFogo;
int TempoExplodindo;   

TCelula Celulas[50][50];

TPeixe Peixe[NUMPEIXES];      

GLfloat win;
GLint view_w, view_h;
int mousex,mousey;
int PosGridX,PosGridY;

int Grid;
float LargGrid;

Particula *ListaParticula;  
Particula *UltimaParticula;  

Onda *ListaOnda;  
Onda *UltimaOnda;  



TPropFundo PontoFundo[TFUNDO][TFUNDO];

//******************************************************************************
// construtor da classe
//******************************************************************************
TBnaval(){        
Grid = 20;
LargGrid = 2*TAMANHO/Grid; 
 for(int j = 0 ; j < TFUNDO ; j++)
  for(int i = 0 ; i < TFUNDO ; i++){                
    PontoFundo[i][j].px = (350.0/TFUNDO)*(i- TFUNDO/2) ; 
    PontoFundo[i][j].py = (350.0/TFUNDO)*(j- TFUNDO/2); 
    
    PontoFundo[i][j].vx = ((rand()%100 )/100.0 -0.5)*1.5; 
    PontoFundo[i][j].vy = ((rand()%100 )/100.0 -0.5)*1.5; 
    
    PontoFundo[i][j].vel = (rand()%80 )/100.0;
    
    PontoFundo[i][j].g = (rand()%20 )/100.0; 
    PontoFundo[i][j].b = (rand()%24 )/100.0;
    
  //  PontoFundo[i][j].g += PontoFundo[i][j].py/300.0+0.2; 
  //  PontoFundo[i][j].b += PontoFundo[i][j].py/600.0+0.1;
  
     PontoFundo[i][j].g += ((160-  fabs(PontoFundo[i][j].py)-fabs(PontoFundo[i][j].px))/300.0)+0.2; 
     PontoFundo[i][j].b += ((180-  fabs(PontoFundo[i][j].py)-fabs(PontoFundo[i][j].px))/600.0)+0.1;  
     PontoFundo[i][j].a =  0.8+(fabs(PontoFundo[i][j].py)+fabs(PontoFundo[i][j].px))/1500;
  
    PontoFundo[i][j].r =  0;   
   } 
}


//******************************************************************************
// Calcula a posição X de uma célula na tela
//******************************************************************************
float CalcPosX(int x){
return x*(((float)TAMANHO*2)/Grid)-TAMANHO;          
}


//******************************************************************************
// Calcula a posição Y de uma célula na tela
//******************************************************************************
float CalcPosY(int y){
return -y*(((float)TAMANHO*2)/Grid)+TAMANHO + POSY ;         
}


//******************************************************************************
// Desenha um círculo que representa uma onda
//******************************************************************************
void DesenhaOnda(Onda  * p){
   
  float tOnda  = 0.12;
  float tOnda2 = p->tamanho;
  float x1,y1,x2,y2,luz;
    
    glPushMatrix(); 
      glTranslatef(p->px,p->py,1);   
    
  glEnable(GL_BLEND);          
   glBegin(GL_QUADS);
    for(int i = 0 ; i < 360 ; i++){
          
          x1 = sin(i/57.3); 
          y1 = cos(i/57.3);       
                
          x2 = sin((i+1)/57.3); 
          y2 = cos((i+1)/57.3);  
          luz = x1; 
          
          x1*=tOnda2;
          x2*=tOnda2;
          y1*=tOnda2;
          y2*=tOnda2;
          
          
           glColor4f(2-(-luz+1),2-(-luz+1),2-(-luz+1),p->alpha);
            
           glVertex2f(  x1-x1*tOnda,y1-y1*tOnda );
           glVertex2f(  x2-x2*tOnda,y2-y2*tOnda);  
           
           glColor4f(0.0,0.0,0.0,0.0); 
           glVertex2f(  x2,y2 );   
           glVertex2f(  x1,y1 );                   
           glVertex2f(  x2,y2 );   
           glVertex2f(  x1,y1 );  
           
           glColor4f(2-(luz+1),2-(luz+1),2-(luz+1),p->alpha);         
           glVertex2f(  x1+x1*tOnda,y1+y1*tOnda );
           glVertex2f(  x2+x2*tOnda,y2+y2*tOnda);   
                               
    }  
   glEnd();
   glPopMatrix();
   
}


//******************************************************************************
// Varre a lista de ondas e chama a função Desenha Onda para cada onda da lista
//******************************************************************************
void DesenhaOndas(){  
 Onda * aux =  UltimaOnda;   
 Onda * aux2; 
  while(aux!= NULL){   
  
  if (aux->alpha>0)aux->alpha -= 0.001; 
  aux->tamanho += 0.1;
  
   aux2 = aux; 
  DesenhaOnda(aux);
 
  aux = aux->anterior; 
  
  if ((aux2->alpha<=0.0))  
    DestroyOnda(aux2);  
 }
}


//******************************************************************************
// Remove Uma onda da lista de ondas e destroy esta onda
//******************************************************************************
void DestroyOnda(Onda  * p){ 
 Onda  * auxproximo; 
 Onda  * auxanterior; 
   
 if (p == NULL) 
    return;
     
 if (p == ListaOnda)
   ListaOnda = p->proximo;
   
 if (p == UltimaOnda)
   UltimaOnda = p->anterior;
 
 auxproximo = p->proximo; 
 auxanterior =  p->anterior; 
 
 delete p; 
 
 if (auxproximo!= NULL){  
  auxproximo->anterior = auxanterior;  
 }
  if (auxanterior!= NULL){  
  auxanterior->proximo = auxproximo;  
 }   
}



//******************************************************************************
// insere uma onda na lista de ondas
//******************************************************************************
void InserOnda(Onda  * p){ 
if (ListaOnda == NULL){
   ListaOnda = p;                 
 }else{
   UltimaOnda->proximo = p;
   p->anterior = UltimaOnda;      
 }     
 UltimaOnda = p; 
}


//******************************************************************************
// Cria uma onda e inser esta onda na lista de ondas
//******************************************************************************
void CriarParticulaOnda(float x, float y){
 Onda * aux;
 aux = new Onda;
 aux->proximo = NULL;
 aux->anterior = NULL;
 aux->px = x;
 aux->py = y;
 aux->alpha = 0.25;
 aux->tamanho = 5;
 InserOnda(aux);
}



//******************************************************************************
// Remove Uma partícula da lista de partículas e destroy esta partícula
//******************************************************************************
void DestroyParticula(Particula  * p){ 
 Particula  * auxproximo; 
 Particula  * auxanterior; 
   
 if (p == NULL) 
    return;
     
 if (p == ListaParticula)
   ListaParticula = p->proximo;
   
 if (p == UltimaParticula)
   UltimaParticula = p->anterior;
 
 auxproximo = p->proximo; 
 auxanterior =  p->anterior; 
 
 delete p; 
 
 if (auxproximo!= NULL){  
  auxproximo->anterior = auxanterior;  
 }
 
  if (auxanterior!= NULL){  
  auxanterior->proximo = auxproximo;  
 }   
}


//******************************************************************************
// Insere uma Partícula na lista de Partículas
//******************************************************************************
void InserParticula(Particula  * p){ 
if (ListaParticula == NULL){
   ListaParticula = p;                 
 }else{
   UltimaParticula->proximo = p;
   p->anterior = UltimaParticula;      
 }     
 UltimaParticula = p; 
}


//******************************************************************************
// Cria uma Partícula do tipo água e insere esta Partícula na lista de Partículas
//******************************************************************************
void CriarParticulaAgua(float x, float y){
 Particula * aux;
 aux = new Particula;
 aux->proximo = NULL;
 aux->anterior = NULL;
 aux->fx = 0;
 aux->fy = -0.01;
 aux->px = x;
 aux->py = y;
 aux->origy = y;
 aux->vx = ((rand()%20)/100.0)-0.10;
 aux->vy = ((rand()%20)/100.0)+0.2 - fabs(aux->vx);
 aux->vida = (rand()%10/100.0)+0.3;
 aux->vrx  = aux->vx*20;
 aux->r =1;
 aux->g =1;
 aux->b =1; 
 aux->vr =0;
 aux->vg =0;
 aux->vb =0; 
 aux->Tamanho = 2.5; 
 aux->vTamanho = 0;
 InserParticula(aux);
}

//******************************************************************************
// Cria uma Partícula do tipo fogo e insere esta Partícula na lista de Partículas
//******************************************************************************
void CriarParticulaFogo(float x, float y){
 Particula * aux;
 aux = new Particula;
 aux->proximo = NULL;
 aux->anterior = NULL;
 aux->fx = 0;
 aux->fy = +0.001;
 aux->px = x + ((rand()%100)/100.0)*(LargGrid)-(LargGrid)/2;
 aux->py = y + (((rand()%100)/100.0)*(LargGrid)) - (LargGrid)/2;
 aux->origy = aux->py;
 aux->vx = ((rand()%10)/100.0)-0.05;
 aux->vy = ((rand()%10)/100.0)-0.05;
 aux->vida = (rand()%10/100.0)+0.25;
 aux->vrx  = 0;
 aux->r =1.0;
 aux->g =1.0;
 aux->b =0.8; 
 aux->vr =0.001;
 aux->vg =-0.006;
 aux->vb =-0.02; 
 aux->Tamanho = LargGrid*0.3; 
 aux->vTamanho = 0.01;
 InserParticula(aux);
}

//******************************************************************************
// Cria uma Partícula do tipo explosão e insere esta Partícula na lista de Partículas
//******************************************************************************
void CriarParticulaExplode(float x, float y){
     
 float aleatorio,aleatorio2;   
 
 aleatorio = (rand()%360)/57.3; 
 aleatorio2 = ((rand()%30)+20)/100.0;
  
 Particula * aux;
 aux           = new Particula;
 aux->proximo  = NULL;
 aux->anterior = NULL;
 aux->fx    = 0;
 aux->fy    = 0;
 aux->px    = x;
 aux->py    = y ;
 aux->origy = -win;
 aux->vx    = sin(aleatorio)*aleatorio2;
 aux->vy    = cos(aleatorio)*aleatorio2;
 aux->vida  = (rand()%15/100.0)+0.15;
 aux->vrx   = 0;
 aux->r     = 1.5;
 aux->g     = 1.5;
 aux->b     = 1.0; 
 aux->vr    = 0.007;
 aux->vg    = -0.008;
 aux->vb    = -0.009; 
 aux->Tamanho = LargGrid*0.4;  
 aux->vTamanho = 0.03;
 InserParticula(aux);
}

//******************************************************************************
// Gera várias partículas do tipo explosão
//******************************************************************************
void GeraExplosao(int x, int y){
  for(int i = 0 ; i< 500 ; i++){   
   CriarParticulaExplode(CalcPosX(x)+LargGrid/2,CalcPosY(y)-LargGrid/2);
 }    
 TempoExplodindo = 30;
}






//******************************************************************************
// Desenha um quadrado que representa uma partícula
//******************************************************************************
void DesenhaParticulas(){  
 Particula * aux =  UltimaParticula;   
 Particula * aux2; 
  while(aux!= NULL){ 
 
  
  glPushMatrix(); 
  glTranslatef(aux->px,aux->py,0);
  glRotatef(aux->rx,0,0,1);
  glBegin(GL_TRIANGLE_FAN);
    glColor4f(aux->r,aux->g,aux->b,aux->vida);  
  //  glColor4f(aux->r,aux->g,aux->b,1);
    glVertex2f( 0,0);
    glColor4f(aux->r,aux->g,aux->b,0.0);
   
    glVertex2f( -aux->Tamanho,-aux->Tamanho);
    glVertex2f( +aux->Tamanho,-aux->Tamanho);
    glVertex2f( +aux->Tamanho,+aux->Tamanho);
    glVertex2f( -aux->Tamanho,+aux->Tamanho);
    glVertex2f( -aux->Tamanho,-aux->Tamanho);
  glEnd();
  glPopMatrix(); 
  
  aux->vy += aux->fy; 
  aux->vx += aux->fx;      
  aux->px += aux->vx;
  aux->py += aux->vy;  
  aux->rx += aux->vrx;
  aux->r  +=aux->vr;
  aux->g  +=aux->vg;
  aux->b  +=aux->vb;
  aux->Tamanho += aux->vTamanho;
  
  
  if (aux->vida>0)aux->vida -= 0.001; 
  
  aux2 = aux; 
  aux = aux->anterior; 
  
  if ((aux2->vida<=0.01)||(aux2->origy > aux2->py))  
    DestroyParticula(aux2);  
 }
}



//******************************************************************************
// Atulaiza alguns elementos do jogo
//******************************************************************************
void Update(){    
   PosGridX =  trunc((win*((float)mousex/view_w) - ((win-TAMANHO)/2)));
   PosGridY =  trunc((win*((float)mousey/view_h) - ((win-TAMANHO)/2)+POSY/2));  
   PosGridX = PosGridX/(TAMANHO/Grid);
   PosGridY = PosGridY/(TAMANHO/Grid);  
   
   if (PosGridX>Grid-1) PosGridX = Grid-1;
   if (PosGridY>Grid-1) PosGridY = Grid-1;
   if (PosGridX<0) PosGridX = 0;
   if (PosGridY<0) PosGridY = 0;
   
   sprintf(texto, "Coordenadas: (%d, %d)", PosGridX,PosGridY);
 
   if(JogoTerminado)  
     return;

   TempoFogo-= 1;
   
   if (TempoFogo<=0){
     TempoFogo = 2;
       for(int j = 0 ; j < Grid ; j++)
       for(int i = 0 ; i < Grid ; i++){ 
          if (Celulas[i][j].Tipo == 2){
              CriarParticulaFogo(CalcPosX(i)+LargGrid/2,CalcPosY(j)-LargGrid/2); 
              AgitaPeixesPerto(CalcPosX(i),CalcPosY(j),3);
            }
        }     
   }  
}


//******************************************************************************
// Usado na Inserção das embarcações na matrix
// Verifica se o espaço está disponível
//******************************************************************************
bool VerificaOcupado(int x1,int y1,int x2,int y2){
 
  x2+=ESPACOENTREBARCOS;
  y2+=ESPACOENTREBARCOS;
  
  if (x1>0)
  x1-=ESPACOENTREBARCOS;
  
  if (y1>0)
  y1-=ESPACOENTREBARCOS;
  
  
  for(int i = x1; i< x2; i++)
  for(int j = y1; j< y2; j++)
   if (Celulas[i][j].Tipo<0)
     return true;
     
  return false;     
}     
     

//******************************************************************************
// Posiciona todos os barcos na matrix
//******************************************************************************
void PosiconaBarcos(){     
int Tipo;  
int px,py;       
     
     
    for(int i = 0; i< 20 ; i++){
      Tipo = rand()%5 +2;      
          // destroyer  
          if(Tipo == 2){   
             do{                        
              px = rand()%(Grid-3) ;
              py = rand()%(Grid-1) ; 
             }while(VerificaOcupado(px,py,px+4,py+2)); 
                 
            Celulas[px+0][py].Tipo = -Tipo;
            Celulas[px+1][py].Tipo = -Tipo;  
            Celulas[px+1][py+1].Tipo = -Tipo;  
            Celulas[px+2][py+1].Tipo = -Tipo; 
            Celulas[px+3][py+1].Tipo = -Tipo;                
          }else
          //Porta-Avioes
           if(Tipo == 3){   
             do{                        
              px = rand()%(Grid-5) ;
              py = rand()%(Grid-1) ; 
             }while(VerificaOcupado(px,py,px+6,py+2)); 
                 
            Celulas[px+0][py].Tipo = -Tipo;
            Celulas[px+1][py].Tipo = -Tipo;  
            Celulas[px+2][py].Tipo = -Tipo;  
            Celulas[px+3][py].Tipo = -Tipo; 
            Celulas[px+2][py+1].Tipo = -Tipo; 
            Celulas[px+3][py+1].Tipo = -Tipo;
            Celulas[px+4][py+1].Tipo = -Tipo;  
            Celulas[px+5][py+1].Tipo = -Tipo;  
               
          }else
          // Lança de Ataque
           if(Tipo == 4){   
             do{                        
              px = rand()%(Grid-2) ;
              py = rand()%(Grid-1) ; 
             }while(VerificaOcupado(px,py,px+3,py+2)); 
                 
            Celulas[px+0][py].Tipo = -Tipo;
            Celulas[px+1][py].Tipo = -Tipo;  
            Celulas[px+2][py+1].Tipo = -Tipo;                 
          }else
           // Submarino
           if(Tipo == 5){   
             do{                        
              px = rand()%(Grid-4) ;
              py = rand()%(Grid-1) ; 
             }while(VerificaOcupado(px,py,px+5,py+2)); 
                 
            Celulas[px+0][py+1].Tipo = -Tipo;
            Celulas[px+1][py+1].Tipo = -Tipo;  
            Celulas[px+2][py+1].Tipo = -Tipo;  
            Celulas[px+3][py+1].Tipo = -Tipo; 
            Celulas[px+4][py+1].Tipo = -Tipo;
            Celulas[px+2][py+0].Tipo = -Tipo;                  
          }else
           //corveta
           if(Tipo == 6){   
             do{                        
              px = rand()%(Grid-2) ;
              py = rand()%(Grid-1) ; 
             }while(VerificaOcupado(px,py,px+2,py+2)); 
                 
            Celulas[px+0][py].Tipo = -Tipo;
            Celulas[px+1][py].Tipo = -Tipo;  
            Celulas[px+1][py+1].Tipo = -Tipo;               
          }       
    }  
    
    
         for(int i = 0; i< NUMDEBOMBAS ; i++){
     // Bomba   
             do{                        
              px = rand()%(Grid-2);
              py = rand()%(Grid-2); 
             }while(VerificaOcupado(px,py,px+1,py+1));                 
            Celulas[px][py].Tipo = -1;                    
         }
    
}


//******************************************************************************
// Olha quantas partes de embracações ainda faltam
//******************************************************************************
int GetPartesFaltando(){
 
 int auxF = 0;    
     
  for(int i = 0; i< Grid; i++)
  for(int j = 0; j< Grid; j++)
   if (Celulas[i][j].Tipo<-1)
     auxF ++; 
     
    return  auxF;  
}



//******************************************************************************
// Chamado no início de cada jogo
// Reposiciona os barcos e limpa as variáveis
//******************************************************************************
void ConfiguraJogo(int T){


 srand (time(0)); 
 Visiveis = false;


for (int i = 0 ; i < NUMPEIXES ; i++){
  Peixe[i].velocidade = 0.1;
  Peixe[i].Tempo = 10;
  Peixe[i].px = (rand()%200)-100;
  Peixe[i].py = (rand()%200)-100;
  Peixe[i].r = ((rand()%360)-180);
  Peixe[i].ir = Peixe[i].r;
  Peixe[i].ivelocidade = 2;
  Peixe[i].velocidade = 2;
  Peixe[i].Tamanho = (rand()%10)/15.0+0.5;
  Peixe[i].assustado = false;
}

//Peixe[0].Tamanho = 3;
//Peixe[0].px = 100;
//Peixe[0].py = 100;


 if (T==0){ Grid = 20;
 }else
  if (T==1){Grid = 30;
 }else
  if (T==2){Grid = 40;
 }else
  if (T==3){Grid = 50;
 }else
  if (T==4){
   Grid = 30;
   Visiveis = true;
 }
 
    

 LargGrid = 2*TAMANHO/Grid;    
     
     
 for(int j = 0 ; j < Grid ; j++)
   for(int i = 0 ; i < Grid ; i++){  
    Celulas[i][j].Tipo = 0;  
 } 
     
 PosiconaBarcos();
   
 ListaParticula = NULL;  
 UltimaParticula = NULL;   

 ListaOnda = NULL;  
 UltimaOnda = NULL;
 
 embarcacaoAcerto = 0;
 
 Disparos = 0;
 Faltando = GetPartesFaltando(); 
 Vidas = NUMDEVIDAS; 
 JogoTerminado = false;
 Venceu = false;
 TempoFogo = 0;
 TempoExplodindo = 0;

}


//******************************************************************************
// Torna os peixes agitados por um intervalo de tempo
//******************************************************************************
void AgitaPeixes(){
  for (int i =0; i< NUMPEIXES ; i++){
   Peixe[i].ivelocidade = 6;  
   Peixe[i].Tempo = (rand()%5) +15; 
   Peixe[i].ir+= (rand()%360) -180; ;
   Peixe[i].assustado = true;      
  }
}


//******************************************************************************
// Torna os peixes que estão perto, agitados por um intervalo de tempo
//******************************************************************************
void AgitaPeixesPerto(float px , float py , float Tamanho){
  for (int i =0; i< NUMPEIXES ; i++)
   if (((fabs(Peixe[i].px - px)<Tamanho*10)&&(fabs(Peixe[i].py - py)<Tamanho*10))
   && (Peixe[i].Tamanho<Tamanho*0.6)&&(!Peixe[i].assustado)){
     Peixe[i].ivelocidade += 2;
     Peixe[i].Tempo = 10; 
     Peixe[i].ir+= ((rand()%2)-1)*180;
     Peixe[i].assustado = true;
  }         
}


//******************************************************************************
// Desenha e atualiza todos os peixes
//******************************************************************************
void DesenhaPeixes(){  
for (int i =0; i< NUMPEIXES ; i++){    
  
      if (Peixe[i].Tempo<=0){ 
        Peixe[i].ir += ((rand()%360)-180);
        Peixe[i].Tempo = rand()%20 + 5.0;
        Peixe[i].ivelocidade = (rand()%20)/10.0 + 1;
        Peixe[i].assustado = false;
        if (fabs(Peixe[i].ir)>30)
           AgitaPeixesPerto(Peixe[i].px , Peixe[i].py,Peixe[i].Tamanho);       
       }
       
      
      if (Peixe[i].py>160){ Peixe[i].ir=Peixe[i].r = 180;}
      else                 
      if (Peixe[i].py<-160){Peixe[i].ir=Peixe[i].r = 0;}
      else
      if (Peixe[i].px>160){ Peixe[i].ir=Peixe[i].r = 90;}
      else 
      if (Peixe[i].px<-160){ Peixe[i].ir=Peixe[i].r = -90;}
      
       
     
      Peixe[i].Tempo -= 0.1;       
      Peixe[i].dvira = (Peixe[i].ir-Peixe[i].r)/50.0;  
      Peixe[i].r+= Peixe[i].dvira;
      Peixe[i].fVira += (Peixe[i].dvira-Peixe[i].fVira)/5;
      Peixe[i].vx = sin(Peixe[i].r/57.3)*Peixe[i].velocidade*0.1; 
      Peixe[i].vy = cos(Peixe[i].r/57.3)*Peixe[i].velocidade*0.1; 
      Peixe[i].px -= Peixe[i].vx; 
      Peixe[i].py += Peixe[i].vy; 
       
      // Peixe[i].ivelocidade -= fabs(Peixe[i].fVira)*0.05;
       if (Peixe[i].ivelocidade > 6) Peixe[i].ivelocidade = 6; 
       
       Peixe[i].velocidade += (Peixe[i].ivelocidade -Peixe[i].velocidade)/30;  
      
          
      Peixe[i].Increment+= fabs(Peixe[i].ivelocidade*1.5-Peixe[i].velocidade ) *0.1;
       
      glPushMatrix(); 
      glTranslatef(Peixe[i].px,Peixe[i].py,0); 
      glRotatef(Peixe[i].r,0,0,1);        
      glRotatef(-sin(Peixe[i].Increment)*10,0,0,1);
      glTranslatef(0,-1,0);  
      glScalef(Peixe[i].Tamanho,Peixe[i].Tamanho*1.5,1); 
      glColor4f(0.0,0.0,0.5,1);  
        glBegin(GL_POLYGON);
           glVertex2f( -3,0 );
           glVertex2f( -3.5,0 );
           glVertex2f( -3,2 );
           glVertex2f( -2,4 );         
           glVertex2f( -1,5 );     
           glVertex2f(  1,5 );
           glVertex2f(  2,4 );
           glVertex2f(  3,2 );
           glVertex2f(  3.5,0 );
           glVertex2f(  3,0 );
           glVertex2f(  2,-3 ); 
           glVertex2f( -2,-3 ); 
          glEnd();
          
        glTranslatef(0,-2,0);       
        glRotatef(sin(Peixe[i].Increment)*10 - Peixe[i].fVira*15,0,0,1); 
        glBegin(GL_POLYGON); 
           glVertex2f(  0, 1 );  
           glVertex2f(  2,-0 );    
           glVertex2f(  1,-4 );       
           glVertex2f( -1,-4 );
           glVertex2f( -2,-0 );                                       
        glEnd(); 
         
         glTranslatef(0,-4,0);
         glRotatef(sin(Peixe[i].Increment)*10 - Peixe[i].fVira*15,0,0,1); 
          glBegin(GL_POLYGON);          
           glVertex2f( -1,-0 ); 
           glVertex2f( 0, 1 );
           glVertex2f(  1,-0 ); 
           glVertex2f(  2,-1.5 );   
           glVertex2f(  2,-2 );      
           glVertex2f(  1,-2 );
           glVertex2f(  0,-1.5 );                          
         glEnd();
         glBegin(GL_POLYGON);          
           glVertex2f( -1,-0 ); 
           glVertex2f(  0,-1.5 ); 
           glVertex2f( -1,-2 );   
           glVertex2f( -2,-2 );   
           glVertex2f( -2,-1.5 );                         
         glEnd();     
       glPopMatrix();      
    }           
}




//******************************************************************************
// Desenha e atualiza a agua ao fundo
//******************************************************************************
void DesenhaFundo(void)
{    
  
 float largura =(win/10) ;
 float Altura = (win/10);    
 Increment += 0.1;  
 

  for(int j = 0 ; j < TFUNDO ; j++)
  for(int i = 0 ; i < TFUNDO ; i++){
    PontoFundo[i][j].tx = PontoFundo[i][j].px + sin(Increment*PontoFundo[i][j].vel)*PontoFundo[i][j].vx*5;
    PontoFundo[i][j].ty = PontoFundo[i][j].py + sin(Increment*PontoFundo[i][j].vel)*PontoFundo[i][j].vy*5;  
  }
   
    glBegin(GL_QUADS);
      glColor4f(0.5,0.5,0.5,1); 
      glVertex2f( -win-10, win+10 );
      glVertex2f( win+10, win+10 );
      glVertex2f( win+10, -win-10 );  
      glVertex2f( -win-10, -win-10 );
     glEnd();

   DesenhaPeixes();

     
 for(int j = 0 ; j < TFUNDO-1 ; j++)
  for(int i = 0 ; i < TFUNDO-1 ; i++){
     glBegin(GL_QUADS);
     //glBegin(GL_LINE_STRIP);
       glColor4f(PontoFundo[i][j].r,PontoFundo[i][j].g,PontoFundo[i][j].b,PontoFundo[i][j].a);       
       glVertex2f( PontoFundo[i][j].tx,PontoFundo[i][j].ty );
       
       glColor4f(PontoFundo[i][j+1].r,PontoFundo[i][j+1].g,PontoFundo[i][j+1].b,PontoFundo[i][j].a);  
       glVertex2f( PontoFundo[i][j+1].tx,PontoFundo[i][j+1].ty );
       
       glColor4f(PontoFundo[i+1][j+1].r,PontoFundo[i+1][j+1].g,PontoFundo[i+1][j+1].b,PontoFundo[i][j].a);  
       glVertex2f( PontoFundo[i+1][j+1].tx,PontoFundo[i+1][j+1].ty );
       
       glColor4f(PontoFundo[i+1][j].r,PontoFundo[i+1][j].g,PontoFundo[i+1][j].b,PontoFundo[i][j].a);  
       glVertex2f( PontoFundo[i+1][j].tx,PontoFundo[i+1][j].ty );
     glEnd();                     
  }  
}


//******************************************************************************
// Torna as embarcações visíveis
//******************************************************************************
void DesenhaBarcos(void){
    for(int j = 0 ; j < Grid ; j++)
     for(int i = 0 ; i < Grid ; i++){ 
             
       if(Celulas[i][j].Tipo == -1)                     
         glColor4f(0,0,0,1.0);       
       else
       if(Celulas[i][j].Tipo == -2)
         glColor4f(1,0,0,0.6); 
       else 
       if(Celulas[i][j].Tipo == -3)
         glColor4f(0,1,0,0.6);
       else   
       if(Celulas[i][j].Tipo == -4)                     
         glColor4f(0,0,1,0.6);       
       else
       if(Celulas[i][j].Tipo == -5)
         glColor4f(1,1,0,0.6); 
       else 
       if(Celulas[i][j].Tipo == -6)
         glColor4f(1,0,1,0.6);
       else 
       if(Celulas[i][j].Tipo == -7)
         glColor4f(0,1,1,0.6);  
       
       if (Celulas[i][j].Tipo<0){  
           glBegin(GL_QUADS); 
             glVertex2f(CalcPosX(i),         CalcPosY(j));
             glVertex2f(CalcPosX(i),         CalcPosY(j)-LargGrid);
             glVertex2f(CalcPosX(i)+LargGrid,CalcPosY(j)-LargGrid);
             glVertex2f(CalcPosX(i)+LargGrid,CalcPosY(j));
           glEnd();
       }
     }               
}



//******************************************************************************
// Desenha os quadrinhos na tela
//******************************************************************************
void DesenhaGrid(void){
float tamanho;
tamanho = ((float)TAMANHO*2)/Grid; 
      float px = CalcPosX(PosGridX);
      float py = CalcPosY(PosGridY);
      float meiLarg = LargGrid/2; 
      float cx = px + meiLarg;
      float cy = py - meiLarg;

glEnable(GL_BLEND); 

    for(int j = 0 ; j < Grid ; j++)
     for(int i = 0 ; i < Grid ; i++){ 
       if(Celulas[i][j].Tipo == 0)                     
         glColor4f(0.1,0.3,0.5,0.0);       
       else
       if(Celulas[i][j].Tipo == 1)
         glColor4f(0.6,0.7,0.8,0.5);
       else 
       if(Celulas[i][j].Tipo == 2)
         glColor4f(0.4,0.4,0.0,0.8);
       else 
       if(Celulas[i][j].Tipo == 3)
         glColor4f(0.0,0.0,0.0,0.8);
      
      if (Celulas[i][j].Tipo > 0){   
       glBegin(GL_QUADS); 
         glVertex2f(CalcPosX(i),         CalcPosY(j));
         glVertex2f(CalcPosX(i),         CalcPosY(j)-LargGrid);
         glVertex2f(CalcPosX(i)+LargGrid,CalcPosY(j)-LargGrid);
         glVertex2f(CalcPosX(i)+LargGrid,CalcPosY(j));
       glEnd();
      }
      
      glLineWidth(1);
      if(Celulas[i][j].Tipo == 3){
      glColor4f(1,1,1,0.8); 
      glBegin(GL_LINE_STRIP); 
         for(int k = 0 ; k < 36 ; k++)
           glVertex2f(CalcPosX(i)+ sin(k/5.7)*meiLarg*0.5 +meiLarg ,CalcPosY(j)+ cos(k/5.7)*meiLarg*0.5-meiLarg);
      glEnd();         
         }   
     } 

  glLineWidth(1);        
  for(int j = 0 ; j <Grid ; j++){
     glColor4f(0.6,0.8,1,0.4);  
      glBegin(GL_LINES); 
         glVertex2f((-j*tamanho+TAMANHO),(-TAMANHO+POSY));
         glVertex2f((-j*tamanho+TAMANHO),(+TAMANHO+POSY));
         
         glVertex2f((+TAMANHO),(-j*tamanho+TAMANHO+POSY));
         glVertex2f((-TAMANHO),(-j*tamanho+TAMANHO+POSY));
      glEnd(); 
  }    
      
       
  for(int j = 0 ; j <Grid ; j++)
    for(int i = 0 ; i <Grid ; i++){  
      glColor4f(0.7,0.8,1,0.6);       
      glBegin(GL_LINES);
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.5,( i*tamanho-TAMANHO+POSY));
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.5,( i*tamanho-TAMANHO+POSY)+tamanho*0.1);            
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.5,( i*tamanho-TAMANHO+POSY)+tamanho);           
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.5,( i*tamanho-TAMANHO+POSY)+tamanho*0.9);       
        glVertex2f((-j*tamanho+TAMANHO)            ,( i*tamanho-TAMANHO+POSY)+tamanho*0.5);       
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.1,( i*tamanho-TAMANHO+POSY)+tamanho*0.5);      
        glVertex2f((-j*tamanho+TAMANHO)-tamanho    ,( i*tamanho-TAMANHO+POSY)+tamanho*0.5);       
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.9,( i*tamanho-TAMANHO+POSY)+tamanho*0.5); 
        
         glColor4f(0.7,0.8,1,0.2);        
        glVertex2f((-j*tamanho+TAMANHO)-tamanho    ,( i*tamanho-TAMANHO+POSY)+tamanho*0.25);       
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.9,( i*tamanho-TAMANHO+POSY)+tamanho*0.25); 
        glVertex2f((-j*tamanho+TAMANHO)-tamanho    ,( i*tamanho-TAMANHO+POSY)+tamanho*0.75);       
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.9,( i*tamanho-TAMANHO+POSY)+tamanho*0.75);     
        glVertex2f((-j*tamanho+TAMANHO)            ,( i*tamanho-TAMANHO+POSY)+tamanho*0.25);       
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.1,( i*tamanho-TAMANHO+POSY)+tamanho*0.25); 
        glVertex2f((-j*tamanho+TAMANHO)            ,( i*tamanho-TAMANHO+POSY)+tamanho*0.75);       
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.1,( i*tamanho-TAMANHO+POSY)+tamanho*0.75);  
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.25,( i*tamanho-TAMANHO+POSY));
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.25,( i*tamanho-TAMANHO+POSY)+tamanho*0.1);    
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.75,( i*tamanho-TAMANHO+POSY));
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.75,( i*tamanho-TAMANHO+POSY)+tamanho*0.1);                
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.25,( i*tamanho-TAMANHO+POSY)+tamanho);           
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.25,( i*tamanho-TAMANHO+POSY)+tamanho*0.9); 
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.75,( i*tamanho-TAMANHO+POSY)+tamanho);           
        glVertex2f((-j*tamanho+TAMANHO)-tamanho*0.75,( i*tamanho-TAMANHO+POSY)+tamanho*0.9);            
      glEnd();          
    }
    
    
    glLineWidth(2);
    glColor4f(0.0,1.0,0.5,0.3); 
      glBegin(GL_LINE_STRIP); 
         glVertex2f(+TAMANHO,-TAMANHO+POSY);
         glVertex2f(+TAMANHO,+TAMANHO+POSY);
         glVertex2f(-TAMANHO,+TAMANHO+POSY);
         glVertex2f(-TAMANHO,-TAMANHO+POSY);
         glVertex2f(+TAMANHO,-TAMANHO+POSY);
      glEnd();  
    
    glLineWidth(3);  
    glColor4f(0.0,1.0,0.5,0.2); 
      glBegin(GL_LINE_STRIP); 
         glVertex2f(+TAMANHOEXT,-TAMANHOEXT+POSY);
         glVertex2f(+TAMANHOEXT,+TAMANHOEXT+POSY);
         glVertex2f(-TAMANHOEXT,+TAMANHOEXT+POSY);
         glVertex2f(-TAMANHOEXT,-TAMANHOEXT+POSY);
         glVertex2f(+TAMANHOEXT,-TAMANHOEXT+POSY);
      glEnd();
      
      glLineWidth(1);
      glColor4f(0.5,1,0.5,0.7);       
      glBegin(GL_LINE_STRIP); 
         glVertex2f(px+1,         py-1);
         glVertex2f(px+1,         py-LargGrid+1);
         glVertex2f(px+LargGrid-1,py-LargGrid+1);
         glVertex2f(px+LargGrid-1,py-1);
         glVertex2f(px+1,  py-1);  
      glEnd();
      
      glColor4f(1,1,1,0.8); 
      glBegin(GL_LINE_STRIP); 
         for(int i = 0 ; i < 360 ; i++)
           glVertex2f(cx+ sin(i/57.3)*meiLarg*0.6,cy+ cos(i/57.3)*meiLarg*0.6);
      glEnd();
      
      glBegin(GL_LINES); 
         glVertex2f(px+1, cy);
         glVertex2f(px-1 +LargGrid , cy);
         glVertex2f(cx, py-1);
         glVertex2f(cx, py+1-LargGrid);
      glEnd();    
}


//******************************************************************************
// Chamado quando clica em aguma célula
//******************************************************************************
void SelecionaCelula(){ 

embarcacaoAcerto = 0;

 if (Celulas[PosGridX][PosGridY].Tipo==-1){  
   GeraExplosao(PosGridX,PosGridY); 
   AgitaPeixes(); 
   if (Vidas == 0) JogoTerminado = true;
   Vidas --;
   Celulas[PosGridX][PosGridY].Tipo = 3;  
 }else    
 if (Celulas[PosGridX][PosGridY].Tipo<-1){ 
   embarcacaoAcerto = -Celulas[PosGridX][PosGridY].Tipo;      
   Celulas[PosGridX][PosGridY].Tipo = 2;
   Faltando = GetPartesFaltando(); 
     if (Faltando==0) {
       Venceu = true;
       JogoTerminado = true;
     }
  }
 else
  if (Celulas[PosGridX][PosGridY].Tipo==0)       
   Celulas[PosGridX][PosGridY].Tipo = 1;
 
 float cx = CalcPosX(PosGridX)+LargGrid/2;  
 float cy = CalcPosY(PosGridY)-LargGrid/2;
 
 for(int i = 0 ; i< 100 ; i++){   
   CriarParticulaAgua(cx,cy);
 }
 
 CriarParticulaOnda(cx,cy);
 AgitaPeixesPerto(cx,cy,4);  
 Disparos ++;
 
  
}


//******************************************************************************
// Desenha um texto na janela GLUT
//******************************************************************************
void DesenhaTexto(char *string , int x, int y) 
{  
     glColor4f(1.0f,1.0f,1.0f,1.0f);
  	glPushMatrix();
        // Posição no universo onde o texto será colocado          
        glRasterPos2f(x,y-(win*0.08)); 
        // Exibe caracter a caracter
        while(*string)
             glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*string++); 
	glPopMatrix();
}

//******************************************************************************
// Desenha um texto na janela GLUT
//******************************************************************************
void DesenhaTexto2(char *string , int x, int y) 
{  
     glColor4f(1.0f,1.0f,1.0f,1.0f);
  	glPushMatrix();
        // Posição no universo onde o texto será colocado          
        glRasterPos2f(x,y-(win*0.08)); 
        // Exibe caracter a caracter
        while(*string)
             glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*string++); 
	glPopMatrix();
}

//******************************************************************************
// Desenha um texto na janela GLUT
//******************************************************************************
void DesenhaTexto3(char *string , int x, int y) 
{  
     glColor4f(1.0f,1.0f,1.0f,1.0f);
  	glPushMatrix();
        // Posição no universo onde o texto será colocado          
        glRasterPos2f(x,y-(win*0.08)); 
        // Exibe caracter a caracter
        while(*string)
             glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++); 
	glPopMatrix();
}


void DesenhaGradeLegenda(float x, float  y,float t, int  larg ){
     
     glBegin(GL_LINES); 
            for (int i = 0; i <= larg ; i++){
                 glColor4f(0.8f,1.0f,0.8f,1.0f);     
                 glVertex2f(x+i*t,y);
                 glVertex2f(x+i*t,y-2*t);
                           
        
                 glVertex2f(x+i*t,y);
                 glColor4f(0.8f,1.0f,0.8f,0.0f);  
                 glVertex2f(x+i*t,y+t);   
                 glVertex2f(x+i*t,y-3*t); 
                 glColor4f(0.8f,1.0f,0.8f,1.0f);  
                 glVertex2f(x+i*t,y-2*t);
          
            }        
              for (int i = 0; i <3 ; i++){
                 glColor4f(0.8f,1.0f,0.8f,1.0f); 
                 glVertex2f(x,y-t*i);
                 glVertex2f(x+t*larg,y-t*i);                
                 glVertex2f(x,y-t*i);
                 glColor4f(0.8f,1.0f,0.8f,0.0f); 
                 glVertex2f(x-t,y-t*i);  
                 glVertex2f(x+t*larg+t,y-t*i); 
                 glColor4f(0.8f,1.0f,0.8f,1.0f);
                 glVertex2f(x+t*larg,y-t*i);  
              }               
      glEnd(); 
          
}

void DesenhaQuadroLegenda(float x, float  y, float  t){
      glBegin(GL_QUADS); 
             glVertex2f(x,y);
             glVertex2f(x,y-t);
             glVertex2f(x+t,y-t);
             glVertex2f(x+t,y);
      glEnd();     
}


void DesenhaLegenda(void){
    
    
    
    float PosX,PosY,Tamanho = 6,alpha;
    
    if (embarcacaoAcerto == 2)
      alpha = sin(Increment)*0.5+0.5;
    else
      alpha = 0.5;     
    sprintf(textoBarcos, "Destroyer"); 
    glColor4f(1.0f,0.0f,0.0f,alpha);
    PosX = -65; PosY =-120;   
    DesenhaQuadroLegenda(PosX, PosY, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho, PosY, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho,PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*2,PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*3, PosY-Tamanho, Tamanho); 
    DesenhaGradeLegenda(PosX,PosY,Tamanho,4);
    DesenhaTexto(textoBarcos,PosX,PosY-8); 
    
    
    if (embarcacaoAcerto == 3)
      alpha = sin(Increment)*0.5+0.5;
    else
      alpha = 0.5;  
    sprintf(textoBarcos, "Porta Avioes"); 
    glColor4f(0.0f,1.0f,0.0f,alpha);
    PosX = -25; PosY =-120; 
    DesenhaQuadroLegenda(PosX          , PosY        , Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho  , PosY        , Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*2, PosY        , Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*3, PosY        , Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*2, PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*3, PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*4, PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*5, PosY-Tamanho, Tamanho); 
    DesenhaGradeLegenda(PosX,PosY,Tamanho,6);
    DesenhaTexto(textoBarcos,PosX,PosY-8); 
    
    if (embarcacaoAcerto == 6)
      alpha = sin(Increment)*0.5+0.5;
    else
      alpha = 0.5;   
    sprintf(textoBarcos, "Corveta"); 
    glColor4f(1.0f,0.0f,1.0f,alpha);
    PosX = 30; PosY =-120;
    DesenhaQuadroLegenda(PosX        , PosY, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho, PosY, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho, PosY-Tamanho, Tamanho); 
    DesenhaGradeLegenda(PosX,PosY,Tamanho,2);
    DesenhaTexto(textoBarcos,PosX-5,PosY-8); 
    
    if (embarcacaoAcerto == 5)
      alpha = sin(Increment)*0.5+0.5;
    else
      alpha = 0.5;  
    sprintf(textoBarcos, "Submarino"); 
    glColor4f(1.0f,1.0f,0.0f,alpha);
    PosX = 60; PosY =-120;  
    DesenhaQuadroLegenda(PosX+Tamanho*2, PosY        , Tamanho); 
    DesenhaQuadroLegenda(PosX  , PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho, PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*2, PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*3, PosY-Tamanho, Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*4, PosY-Tamanho, Tamanho); 
    DesenhaGradeLegenda(PosX,PosY,Tamanho,5);
    DesenhaTexto(textoBarcos,PosX,PosY-8); 


    if (embarcacaoAcerto == 4)
      alpha = sin(Increment)*0.5+0.5;
    else
      alpha = 0.5;  
    sprintf(textoBarcos, "Lanca de ataque"); 
    glColor4f(0.0f,0.0f,1.0f,alpha);
    PosX = 110; PosY =-120; 
    DesenhaQuadroLegenda(PosX          , PosY        , Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho  , PosY        , Tamanho); 
    DesenhaQuadroLegenda(PosX+Tamanho*2, PosY-Tamanho, Tamanho); 
    DesenhaGradeLegenda(PosX,PosY,Tamanho,3);
    DesenhaTexto(textoBarcos,PosX-10,PosY-8); 
}


//******************************************************************************
// Função callback chamada para fazer o desenho
//******************************************************************************
void Desenha(void)
{  
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
                   
     glClear(GL_COLOR_BUFFER_BIT);
  
    glPushMatrix();
      if (TempoExplodindo>0){
         TempoExplodindo-=1;
         glTranslatef(((rand()%4)-2.0)*TempoExplodindo/20.0,((rand()%4)-2.0)*TempoExplodindo/20.0 ,0);
       }
       
       DesenhaFundo();
       DesenhaOndas();
  
     
     
     if(!JogoTerminado){
         if(Visiveis)
           DesenhaBarcos();
           
         DesenhaGrid();
         DesenhaLegenda();
         
     glColor4f(1.0f,1.0f,1.0f,0.6f);
     DesenhaTexto(texto,-win+2,win+2);
     
     sprintf(texto, "Disparos : %d",Disparos); 
     DesenhaTexto2(texto,-win+7,-110);
     
     sprintf(texto, "Restam   : %d",Faltando); 
     DesenhaTexto2(texto,-win+7,-120);
     
     sprintf(texto, "Vidas    : %d",Vidas); 
     DesenhaTexto2(texto,-win+7,-130);
     }else
     {
       if (Venceu){
           sprintf(texto, "Conseguiu com %d disparos!",Disparos); 
           DesenhaTexto3(texto,-56,20);
           sprintf(texto, "Click com o botao direito."); 
           DesenhaTexto2(texto,-50,10);       
       }else
       {    
           sprintf(texto, "Perdeu!"); 
           DesenhaTexto3(texto,-14,20);
           sprintf(texto, "Click com o botao direito."); 
           DesenhaTexto2(texto,-50,10);
       }
     }
     
     
     DesenhaParticulas();
     glPopMatrix(); 
        

     
     glutSwapBuffers();
}

      
};

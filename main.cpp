#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include "memoria.cpp"
#include "BNaval.cpp"

#define TELA_LARGURA 700
#define TELA_ALTURA 700


int GameAtivo =-1;


TMemoria Memoria;
TBnaval Bnaval;
char texto[30]; 

GLfloat win;

void Timer(int value)
{
     
    if(GameAtivo==0) 
     Memoria.MemoriaUpdate();
    else
    if(GameAtivo==1) 
      Bnaval.Update();
      
    
    glutPostRedisplay();
    glutTimerFunc(10,Timer, 1);   
}




// Inicializa parâmetros de rendering
void Inicializa (void)
{   
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    win=150.0f;
    Memoria.win = win; 
    Bnaval.win = win;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{ 
  
    // Especifica as dimensões da Viewport
    glViewport(0, 0, w, h);
    Memoria.view_w = w;
    Memoria.view_h = h;  
    Bnaval.view_w = w;  
    Bnaval.view_h = h;                 

    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D (-win, win, -win, win);
}

// Função callback chamada sempre que o mouse é movimentado
// sobre a janela GLUT com um botão pressionado
void MoveMouseBotaoPressionado(int x, int y)
{
    // sprintf(texto, "Botao pressionado (%d,%d)", x, y);
    // glutPostRedisplay();
}

// Função callback chamada sempre que o mouse é movimentado
// sobre a janela GLUT 
void MoveMouse(int x, int y)
{
     Memoria.mousex=x;
     Memoria.mousey=y;
     Bnaval.mousex=x;
     Bnaval.mousey=y;
}

// Função callback chamada para gerenciar eventos do teclado   
// para teclas especiais, tais como F1, PgDn e Home
void TeclasEspeciais(int key, int x, int y)
{
    if(key == GLUT_KEY_UP) {
           win -= 10;
           if (win < 10) win = 10;
           glMatrixMode(GL_PROJECTION);
           glLoadIdentity();
           gluOrtho2D (-win, win, -win, win);
    }
    if(key == GLUT_KEY_DOWN) {
           win += 10;
           if (win > 500) win = 500;
           glMatrixMode(GL_PROJECTION);
           glLoadIdentity();
           gluOrtho2D (-win, win, -win, win);
    }
    glutPostRedisplay();
}
           
// Gerenciamento do menu com as opções de cores           
void MenuMemoria(int op)
{
    if(op==0)
      Memoria.ConfiguraJogo(true);
    else
      Memoria.ConfiguraJogo(false);
      
    GameAtivo = 0;
}           


// Gerenciamento do menu com as opções de cores           
void MenuNaval(int op)
{
    if (op==4){
    Bnaval.Visiveis = true;  
      if  (GameAtivo != 1)
       Bnaval.ConfiguraJogo(op);
    
      }else
      Bnaval.ConfiguraJogo(op);
      
      GameAtivo = 1; 
}   
        
// Gerenciamento do menu principal           
void MenuPrincipal(int op)
{
    exit(0);
}
              
// Criacao do Menu
void CriaMenu() 
{
    int menu,submenu1,submenu2;
    submenu1 = glutCreateMenu(MenuMemoria);
    glutAddMenuEntry("Facil",0);
    glutAddMenuEntry("Difícil",1);
    submenu2 = glutCreateMenu(MenuNaval);
    glutAddMenuEntry("Pequeno (20x20)",0);
    glutAddMenuEntry("Medio   (30x30)",1); 
    glutAddMenuEntry("Grande  (40x40)",2);
    glutAddMenuEntry("Gigante (50x50)",3);
    glutAddMenuEntry("Revelar Embarcações",4);
    menu = glutCreateMenu(MenuPrincipal);
    glutAddSubMenu("Memoria",submenu1);
    glutAddSubMenu("Batalha Naval",submenu2);
    glutAddMenuEntry("Sair",0);  
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
       
           
// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{        
    if (button == GLUT_RIGHT_BUTTON)
         if (state == GLUT_DOWN) 
            CriaMenu();
    
     if (button == GLUT_LEFT_BUTTON)
         if (state == GLUT_DOWN){      
             if(GameAtivo==0) 
               Memoria.SelecionaCarta();
              else
              if(GameAtivo==1) 
               Bnaval.SelecionaCelula();
            }           
    glutPostRedisplay();   
}
    
void Desenha(void){   
    if(GameAtivo==0) 
     Memoria.Desenha();
    else
    if(GameAtivo==1) 
      Bnaval.Desenha(); 
    else{  
       glMatrixMode(GL_MODELVIEW);
       glLoadIdentity();            
       glClear(GL_COLOR_BUFFER_BIT);     
       Memoria.DesenhaFundo();
       glColor4f(0.8,0.7,0.8,1); 
       Memoria.DesenhaInterrogacao(); 
       Bnaval.DesenhaTexto2(texto,-50,-40); 
       glutSwapBuffers();     
    }     
}    
    
                      
// Programa Principal 
int main( int argc, char *argv[] )
{   
     Memoria.view_w = TELA_LARGURA;
     Memoria.view_h = TELA_ALTURA;
     Bnaval.view_w = TELA_LARGURA;
     Bnaval.view_h = TELA_ALTURA;
     Memoria.ConfiguraJogo(true);
     Bnaval.ConfiguraJogo(0);
     sprintf(texto, "Click com o botao direito.");    
     glutInit ( &argc, argv );
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);     
     glutInitWindowSize(TELA_LARGURA,TELA_ALTURA);
     glutInitWindowPosition(10,10);
     glutCreateWindow("Jogo da Memoria e Batalha Naval");
     glutDisplayFunc(Desenha);
     glutTimerFunc(10, Timer, 1);
     glutReshapeFunc(AlteraTamanhoJanela);
     glutMotionFunc(MoveMouseBotaoPressionado); 
     glutPassiveMotionFunc(MoveMouse);
     glutMouseFunc(GerenciaMouse);    
     glutSpecialFunc(TeclasEspeciais); 
     Inicializa();
     glutMainLoop();
}

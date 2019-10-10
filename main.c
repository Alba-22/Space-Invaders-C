#include "SOIL.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <string.h>
#define tamanhoSprite 0.1f
#define tamanhoScore 0.0700f
#define tamanhoTiro 0.05f

double direcaoInimigo = 1;
double posicaoInimigoX = 0; //Incremento da posição dos inimigos em X
double posicaoInimigoY = 0; //Incremento da posição dos inimigos em Y
int Inimigos[5][9]={1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1}; // Matriz que representa os inimigos
int InimigosEliminados = 0;
int IniciaJogo = 0;
int Score = 0;
char ScoreS[5] = "0000";
int menor_linha_viva_da_coluna[9];
int ColisaoTex;

struct ALIADO{
    double xi;
    double yi;
    double x;
    double y;
    double status;
    double direcao;
    double posicao;
};

struct TIRO{
    int status, pos_sett, playerpress, on_screen, colisao;
    double x, xi;
    double y, yi;
};

// ============================================================================================================================

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void DesenhaMenu();
void DesenhaCenario();
void DesenhaCenarioGameOver();
void DesenhaCenarioVictory();
void DesenhaInimigos();
void DesenhaAliado();
void DesenhaTiro();
void DesenhaScore();
void ResetaJogo();
void DesenhaTiroInimigo();

struct TIRO tiro_inimigo[3];

struct ALIADO aliado;
struct TIRO tiro;

// OPENGL + RESOLUÇÃO
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    IniciaAliado();
    tiro.status = 0;

    //Inicializacao das variaveis para o Tiro Inimigo
    int i;
    for(i = 0; i < 3; i++){
        tiro_inimigo[i].status = 0;
    }
    for(i = 0; i < 9; i++){
        menor_linha_viva_da_coluna[i] = 4;
    }

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            DesenhaMenu();
            if (IniciaJogo == 1){
                DesenhaCenario(); // Desenha o cenário de inicio
                if (aliado.status > 0 && InimigosEliminados < 45){ // Inicia o jogo desenhando os objetos
                    DesenhaScore();
                    DesenhaInimigos();
            	    DesenhaTiro();
                    DesenhaAliado();
                    DesenhaTiroInimigo();
        	   }
                else if (aliado.status > 0 && InimigosEliminados == 45){ // Se todos os inimigos forem eliminados
                    IniciaJogo = 2;
                }
                else{ // Se o aliado morrer
                    IniciaJogo = 3;
                }
            }
            else if (IniciaJogo == 2){
                DesenhaCenarioVictory();
                ResetaJogo();
            }
            else if (IniciaJogo == 3){
                DesenhaCenarioGameOver();
                aliado.status = 1;
                ResetaJogo();
            }

            glPopMatrix();

            SwapBuffers(hDC);

            Sleep(10);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

// CONTOLES
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
                case VK_LEFT:
                    aliado.direcao = -1;
                    break;
                case VK_RIGHT:
                    aliado.direcao = 1;
                    break;
                case VK_SPACE:
                    tiro.playerpress = 1;
                    break;
                case '1':
                case VK_NUMPAD1:
                    if (IniciaJogo == 3){
                    	Score = 0;
                    	strcpy(ScoreS, "0000");
                	}
                	IniciaJogo = 1;
            }
        }
        break;


        case WM_KEYUP:
        {
            switch (wParam)
            {
                case VK_LEFT:
                    aliado.direcao = 0;
                    break;
                case VK_RIGHT:
                    aliado.direcao = 0;
                    break;
                case VK_SPACE:
                    tiro.playerpress = 0;
                    break;
            }
        }


        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

// OPENGL
void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

    //textura
    carregaTexturas();
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // Linear Filtering
}
// MAIS OPENGL
void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

// ============================================================================================================================
// ============================================================================================================================
// ============================================================================================================================

GLuint InimigoTex[3];
GLuint AliadoTex;
GLuint CenarioTex;
GLuint CenarioGameOverTex;
GLuint CenarioVictoryTex;
GLuint MenuTex;
GLuint MorteTex[3];
GLuint NumbersTex[10];

static void desenhaSprite(float coluna,float linha, GLuint tex);
static GLuint carregaArqTextura(char *str);

// CARREGA AS TEXTURAS DOS ARQUIVOS DO SPRITE
void carregaTexturas(){
    int i;
    char str[50];

    for (i = 0; i < 3; i++){
        sprintf(str,".//Sprites//Enemy%d.png", i);
        InimigoTex[i] = carregaArqTextura(str);
    }

    sprintf(str,".//Sprites//Cannon.png");
    AliadoTex = carregaArqTextura(str);

    sprintf(str,".//Sprites//Cenario.v2.png");
    CenarioTex = carregaArqTextura(str);

    sprintf(str,".//Sprites//GameOver.v2.png");
    CenarioGameOverTex = carregaArqTextura(str);

    sprintf(str,".//Sprites//Victory.v2.png");
    CenarioVictoryTex = carregaArqTextura(str);

    sprintf(str,".//Sprites//Menu.v2.png");
    MenuTex = carregaArqTextura(str);

    for (i = 0; i < 3; i++){
        sprintf(str,".//Sprites//Morte%d.png", i);
        MorteTex[i] = carregaArqTextura(str);
    }

    for (i = 0; i < 10; i++){
        sprintf(str, ".//Sprites//Numbers//Number%d.png", i);
        NumbersTex[i] = carregaArqTextura(str);
    }

    sprintf(str,".//Sprites//Colisao.png");
    ColisaoTex = carregaArqTextura(str);
}

// CARREGA AS TEXTURAS OPENGL
static GLuint carregaArqTextura(char *str){
//     http:1//www.lonesock.net/soil.html
    GLuint tex = SOIL_load_OGL_texture
        (
            str,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
            SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
        );

    /* check for an error during the load process */
    if(0 == tex){
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    return tex;
}

// Desenha a textura dos inimigos e do canhão
void desenhaSprite(float coluna,float linha, GLuint tex){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(coluna, linha);
        glTexCoord2f(1.0f,1.0f); glVertex2f(coluna + tamanhoSprite, linha);
        glTexCoord2f(1.0f,0.0f); glVertex2f(coluna + tamanhoSprite, linha - tamanhoSprite);
        glTexCoord2f(0.0f,0.0f); glVertex2f(coluna, linha - tamanhoSprite);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

// Desenha a textura na tela toda
void desenhaFullscreen(GLuint tex){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(-1,  1);
        glTexCoord2f(1.0f,1.0f); glVertex2f( 1,  1);
        glTexCoord2f(1.0f,0.0f); glVertex2f( 1, -1);
        glTexCoord2f(0.0f,0.0f); glVertex2f(-1, -1);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

// Desenha a textura do Score
void desenhaNumero(float x, float y, GLuint tex){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f,1.0f); glVertex2f(x + tamanhoScore, y);
        glTexCoord2f(1.0f,0.0f); glVertex2f(x + tamanhoScore, y - tamanhoScore);
        glTexCoord2f(0.0f,0.0f); glVertex2f(x, y - tamanhoScore);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

// ============================================================================================================================
// ============================================================================================================================
// ============================================================================================================================

void DesenhaCenario(){
    desenhaFullscreen(CenarioTex);
}

void DesenhaCenarioGameOver(){
    desenhaFullscreen(CenarioGameOverTex);
}

void DesenhaCenarioVictory(){
    desenhaFullscreen(CenarioVictoryTex);
}

void DesenhaMenu(){
    desenhaFullscreen(MenuTex);
}

void ResetaJogo(){
	DesenhaScore();
    IniciaAliado();
    InimigosEliminados = 0;
    posicaoInimigoX = 0;
    posicaoInimigoY = 0;
    int i, j;
    for (i = 0; i < 5; i++){
        for (j = 0; j < 9;j++){
            Inimigos[i][j] = 1;
        }
    }
    for(i = 0; i < 3; i++){
    	tiro_inimigo[i].status = 0;
    }
    for(i = 0; i < 9; i++){
    	menor_linha_viva_da_coluna[i] = 4;
    }
}

void DesenhaScore(){
    int i, n;
    for (i = 0; i < 4; i++){
        double posXScore = -0.67;
        double posYScore =  0.964;
        n = ScoreS[i] - '0'; // 'Converte' cada char da string para um inteiro equivalente
        desenhaNumero(posXScore + (i * 0.0550), posYScore, NumbersTex[n]);
    }
}

void IniciaAliado()
{
    aliado.xi = -0.05;
    aliado.yi = -0.83;
    aliado.direcao = 0;
    aliado.posicao = 0;
    aliado.status = 1;
}

void DesenhaAliado()
{
    //Altera a posicao do aliado para cada direcao setada, tambem impedindo que o ele saia da tela 
    if(aliado.direcao == 1 && aliado.x + 0.1f < 0.97){
        aliado.posicao += 0.02;
    }
    if(aliado.direcao == -1 && aliado.xi + aliado.posicao > -0.97){
            aliado.posicao -= 0.02;
    }

    aliado.x = aliado.xi + aliado.posicao;
    aliado.y = aliado.yi;

    desenhaSprite(aliado.x, aliado.y, AliadoTex);
}

void DesenhaTiro()
{
    //Caso o tiro esteja fora da tela e o player aperte o botao p/ tiro
    if(tiro.on_screen == 0 && tiro.playerpress == 1){
        tiro.status = 1;
        tiro.pos_sett = 0;
        tiro.on_screen = 1;
    }

    //Para setar a posicao inicial do tiro 
    if(tiro.pos_sett == 0){
        tiro.yi = aliado.y;
        tiro.xi = aliado.x + 0.045f;

        tiro.pos_sett = 1;

        tiro.y = tiro.yi;
        tiro.x = tiro.xi;
    }

    tiro.y += 0.06f; //Movimentacao do tiro

    if(tiro.status == 1){
        //Desenha o tiro caso ele esteja 'ativado'
        glPushMatrix();
        glBegin(GL_QUADS);

        glColor3f(1.0f, 0.0f, 0.0f);    glVertex2f(tiro.x, tiro.y);
                                        glVertex2f((tiro.x + 0.01f),  tiro.y);
                                        glVertex2f((tiro.x + 0.01f), (tiro.y - 0.1f));
                                        glVertex2f(tiro.x,   (tiro.y - 0.1f));
        glEnd();
    }
    if(tiro.y > 1.0 ){
        //Desativa o tiro caso ele saia da tela 
        tiro.on_screen = 0;
        tiro.status = 0;
        tiro.y = aliado.y;
    }
}

void DesenhaInimigos()
{
    // Se inimigo está pro lado direito, o desloca pra direita
    if (direcaoInimigo == 1){
        posicaoInimigoX += 0.004; // Valor irá interferir na velocidade em que o inimigo se desloca
    }

    // Se inimigo está pro lado esquerdo, o desloca pra esquerda
    if (direcaoInimigo == -1){
        posicaoInimigoX -= 0.004; // Valor irá interferir na velocidade em que o inimigo se desloca
    }

    float i, j;
    for(i = -1; i < 4; i++){
        for(j = -4; j <= 4; j++){
            int linha = i + 1, coluna = j + 4;

            double CoordsXDir =  0.05f + j / 5; // Lado direito  do quadrado
            double CoordsXEsq = -0.05f + j / 5; // Lado esquerdo do quadrado
            double CoordsYSup =  0.25f + i / 5; // Lado superior do quadrado
            double CoordsYInf =  0.15f + i / 5; // Lado inferior do quadrado

            double CoordsXDir_Atual = CoordsXDir + posicaoInimigoX;
            double CoordsXEsq_Atual = CoordsXEsq + posicaoInimigoX;
            double CoordsYSup_Atual = CoordsYSup + posicaoInimigoY;
            double CoordsYInf_Atual = CoordsYInf + posicaoInimigoY;

            if(Inimigos[linha][coluna] == 1){ // Se Invader estiver vivo
                // Desenha diferentes inimigos de acordo com a linha
                if (i == 3){
                    desenhaSprite(CoordsXEsq_Atual, CoordsYSup_Atual, InimigoTex[2]);
                }
                else if (i == 2){
                    desenhaSprite(CoordsXEsq_Atual, CoordsYSup_Atual, InimigoTex[1]);
                }
                else{
                    desenhaSprite(CoordsXEsq_Atual, CoordsYSup_Atual, InimigoTex[0]);
                }

                // Verifica se o inimigo encostou no lado direito, invertendo sua direção e descendo
                if (CoordsXDir + posicaoInimigoX >= 1){
                    direcaoInimigo = -1;
                    posicaoInimigoY -= 0.01;
                }

                // Verifica se o inimigo encostou no lado esquerdo, invertendo sua direção e descendo
                if (CoordsXEsq + posicaoInimigoX <= -1){
                    direcaoInimigo = 1;
                    posicaoInimigoY -= 0.01;
                }

                // Realiza a verificação da colisão
                if (tiro.status == 1 && ((tiro.x >= CoordsXEsq_Atual && tiro.x <= CoordsXDir_Atual && tiro.y <= CoordsYSup_Atual && tiro.y >= CoordsYInf_Atual)
                	|| (tiro.x + 0.02 >= CoordsXEsq_Atual && tiro.x + 0.02 <= CoordsXDir_Atual && tiro.y <= CoordsYSup_Atual && tiro.y >= CoordsYInf_Atual))){
                	tiro.status = 0;
                    // Adição na pontuação, dependendo do inimigo
                    if (i == 3) Score += 50;
                    else if ( i == 2) Score += 30;
                    else Score += 10;

                    // Transformação da pontuação inteira em string
                    if (Score < 100){
                        sprintf(ScoreS, "00%d", Score);
                    }
                    if (Score >= 100 && Score < 1000){
                        sprintf(ScoreS, "0%d", Score);
                    }
                    if (Score >= 1000){
                        sprintf(ScoreS, "%d", Score);
                    }

                	Inimigos[linha][coluna] = 0;
                	InimigosEliminados++;
                }

                // Verifica se o inimigo chegou ao final do caminho, finalizando o jogo
                if (CoordsYInf_Atual <= -0.880){
                    aliado.status = 0;
                }

                //Pega a menor linha entre os invaders vivos daquela coluna
                if(linha < menor_linha_viva_da_coluna[coluna]){
                    menor_linha_viva_da_coluna[coluna] = linha;
                }
            }
            // Se Invader estiver morto, inicia a animação de morte
            else if (Inimigos[linha][coluna] <= 0 && Inimigos[linha][coluna] >= -27){
            	//Seta 4 como menor linha p/ que o calculo seja feito novamente na coluna do invader morto
                menor_linha_viva_da_coluna[coluna] = 4;

                if (Inimigos[linha][coluna] < 0 && Inimigos[linha][coluna] >= -9){
                    desenhaSprite(CoordsXEsq_Atual, CoordsYSup_Atual, MorteTex[0]);
                }
                else if (Inimigos[linha][coluna] < -9 && Inimigos[linha][coluna] >= -18){
                    desenhaSprite(CoordsXEsq_Atual, CoordsYSup_Atual, MorteTex[1]);
                }
                else if (Inimigos[linha][coluna] < -18 && Inimigos[linha][coluna] >= -27){
                    desenhaSprite(CoordsXEsq_Atual, CoordsYSup_Atual, MorteTex[2]);
                }
                Inimigos[linha][coluna]--;
            }
        }
    }
}

void DesenhaTiroInimigo(){
    int idx;
    int coluna_count;

    idx = 0;
    //Para cada coluna na matriz de inimigos
    for(coluna_count = 0; coluna_count < 9; coluna_count++){
        //Calcula qual posicao do vetor de tiros sera chamada
        idx++;
        if(idx > 2) idx = 0;

        //Pega a menor linha da respectiva coluna e calcula as coordenadas atuais daquele inimigo
        int linha = menor_linha_viva_da_coluna[coluna_count];
        double x = -0.05f + (coluna_count - 4)/5.0 + posicaoInimigoX;
        double y = 0.15f + (linha - 1)/5.0 + posicaoInimigoY;

        tiro_inimigo[idx].y -= 0.0025f; //Movimentacao do tiro

        if(Inimigos[linha][coluna_count] == 1){
            //Simula o tiro para os inimigos vivos
            int random = rand() % 15000;
            if(random > 50*(idx) && random < 50*(idx+1)){
                tiro_inimigo[idx].playerpress = 1;
                random = 20;
            }else{
                tiro_inimigo[idx].playerpress = 0;
            }


            if(tiro_inimigo[idx].playerpress == 1 && tiro_inimigo[idx].on_screen == 0){
                //Caso o inimigo 'atire' e o tiro esteja fora da tela
                tiro_inimigo[idx].playerpress == 0;
                tiro_inimigo[idx].status = 1;
                tiro_inimigo[idx].pos_sett = 0;
                tiro_inimigo[idx].on_screen = 1;
            }

            if(tiro_inimigo[idx].pos_sett == 0){
                //Seta a posicao inicial do tiro inimigo baseado nas coordenadas do inimigo
                tiro_inimigo[idx].xi = x + 0.045f;
                tiro_inimigo[idx].yi = y;

                tiro_inimigo[idx].pos_sett = 1;

                tiro_inimigo[idx].x = tiro_inimigo[idx].xi;
                tiro_inimigo[idx].y = tiro_inimigo[idx].yi;
            }

            if(tiro_inimigo[idx].status == 1){
                //Desenha o tiro caso esteja 'ativado'
                tiro_inimigo[idx].colisao = 0; //Marca a colisao como ainda nao ocorrida

                glPushMatrix();
                glBegin(GL_QUADS);

                glColor3f(1.0f, 0.0f, 0.0f);    glVertex2f(tiro_inimigo[idx].x, tiro_inimigo[idx].y);
                                                glVertex2f((tiro_inimigo[idx].x + 0.01f),  tiro_inimigo[idx].y);
                                                glVertex2f((tiro_inimigo[idx].x + 0.01f), (tiro_inimigo[idx].y - 0.05f));
                                                glVertex2f(tiro_inimigo[idx].x,   (tiro_inimigo[idx].y - 0.05f));
                glEnd();
            }

            //Quando o tiro sai da tela
            if(tiro_inimigo[idx].y < -1.0){
                tiro_inimigo[idx].on_screen = 0;
                tiro_inimigo[idx].status = 0;
            }

            //Colisao de tiro com tiro (TT)
            int ColisaoTTPontoInfEsq = tiro.x <= tiro_inimigo[idx].x && tiro_inimigo[idx].x <= tiro.x + 0.02f && tiro.y - 0.05f <= tiro_inimigo[idx].y - 0.05f && tiro_inimigo[idx].y - 0.05f <= tiro.y;
            int ColisaoTTPontoInfDir = tiro.x <= tiro_inimigo[idx].x + 0.02f && tiro_inimigo[idx].x + 0.02f <= tiro.x + 0.02f  && tiro.y - 0.05f <= tiro_inimigo[idx].y - 0.05f && tiro_inimigo[idx].y - 0.05f <= tiro.y;
            if(tiro_inimigo[idx].status == 1 && tiro.status == 1 && (ColisaoTTPontoInfEsq || ColisaoTTPontoInfDir)){
                tiro_inimigo[idx].status = 0;
                tiro.status = 0;
                tiro_inimigo[idx].colisao--;
            }
            //Animacao de colisao
            if(tiro_inimigo[idx].colisao < 0){
                if(tiro_inimigo[idx].colisao > -25 && tiro_inimigo[idx].colisao <= 0){
                    desenhaSprite(tiro_inimigo[idx].x - 0.045f, tiro_inimigo[idx].y, ColisaoTex);
                }
                tiro_inimigo[idx].colisao--;
            }
         	
            //Colisao de tiro com o aliado (TA)
            int ColisaoTAPontoInfEsq = aliado.x <= tiro_inimigo[idx].x && tiro_inimigo[idx].x <= aliado.x + 0.1f && aliado.y - 0.05f <= tiro_inimigo[idx].y - 0.05f && tiro_inimigo[idx].y - 0.05f <= aliado.y;
            int ColisaoTAPontoInfDir = aliado.x <= tiro_inimigo[idx].x + 0.02f && tiro_inimigo[idx].x + 0.02f <= aliado.x + 0.1f  && aliado.y - 0.05f <= tiro_inimigo[idx].y - 0.05f && tiro_inimigo[idx].y - 0.05f <= aliado.y;
            int ColisaoTAPontoSupEsq = aliado.x <= tiro_inimigo[idx].x && tiro_inimigo[idx].x <= aliado.x + 0.1f && aliado.y - 0.05f <= tiro_inimigo[idx].y && tiro_inimigo[idx].y <= aliado.y;
            int ColisaoTAPontoSupDir = aliado.x <= tiro_inimigo[idx].x + 0.02f && tiro_inimigo[idx].x + 0.02f <= aliado.x + 0.1f  && aliado.y - 0.05f <= tiro_inimigo[idx].y && tiro_inimigo[idx].y <= aliado.y;
            if(tiro_inimigo[idx].status == 1 && aliado.status == 1 && (ColisaoTAPontoInfDir || ColisaoTAPontoInfEsq || ColisaoTAPontoSupEsq || ColisaoTAPontoSupDir)){
                tiro_inimigo[idx].status = 0;
                aliado.status = 0;
            }
        }
    }
}

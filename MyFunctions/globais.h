#ifndef GLOBAIS_H
#define GLOBAIS_H

//BIBLIOTECAS PADRÕES IMPORTADAS
#include <iostream>
#include <cmath>
#include <string.h>	
#include <vector>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include "parserxml/tinyxml.h"

using namespace std;

#define PATH_CONFIG "config.xml"


typedef enum {TIPO_PISTA, TIPO_LARG_CHEGADA, TIPO_INIMIGO, TIPO_CARRO, TIPO_RODA_SUP, TIPO_RODA_INF,
			 TIPO_EIXO, TIPO_CHASSI, TIPO_CANHAO, TIPO_CIRC, TIPO_VEL_NORMAL, TIPO_VEL_TURBO}tipo;

typedef enum{LIMPAR_FOG, CRIAR_TIRO_INIMIGO, RESETA_CHECK, RESTAURA_CHECK}time_type;

typedef enum{INIT, RUN, GAME_OVER, WINNER}status_game;
 

//ESTRUTURAS DE DADOS
typedef struct tansformacoes_geometricas{
	float tx, ty, tz; //translação
	float ex, ey, ez; //escala
	float rx, ry, rz; //rotação
}trans;

typedef struct rgb{
	float r, g, b;
}cor;

typedef struct retangulos{
	int tipo;
	trans t;
	cor c;
	float comp, larg;
}rect;

typedef struct circulos{
	//reservadas para o tiro do inimigo, onde sera calculado apenas o cos e o sin do ang do tiro
	float cos, sin;
	int tipo;
	float raio;
	trans t;
	cor c;
}circ;

typedef struct sensor{
	float x, y, raio;
}sensor;

typedef struct params{
	float vel_tiro_carro;
	float freq_tiro_inimigo, vel_tiro_inimigo, vel_inimigo;
}params;

//VARIÁVEIS GLOBAIS DE CARRO


////////////////////////////  OPEN GL ///////////////////////////
#define NUM_LINHAS 360  //desenho de circulos 
#define VEL_TURBO 3.0 //multiplicador da velocidade normal
#define START 1
#define STOP 2
#define ANG_REF 90
#define TAXA_ROT_RODA 10
#define TAXA_ROT_CARRO 15
#define FATOR_ANG_CANHAO 2.0 //diminui o tempo das animacoes do canhao 

int p_maior, p_menor;		//circulos que compoem a arena
float raio_maior, raio_menor; 	//circulos que compoem a arena
float x_ini, x_fim, y_ini, y_fim;

float x_centro, y_centro;

float x_car_ini, y_car_ini;
vector<sensor> s_ini;

bool teclas[256]; 	//256 qtd de teclas contidas no padrao ASCII

int status = START;

int qtd_checks_arena = 4;
int checkpoints_arena[4];

bool flagJoystick = false;

/////////////////////////////  CARRO //////////////////////////////
float angMaxRoda = 45.0, angMaxCanhao = 45.0;
///////
float angReferenciaCarro = 90.0; //os retangulos sao desenhados com o comprimento no eixo Y
//mecher aqui somente se a funcao de desenhar retangulo for alterada

float velelocidade_carro = 1.2;
float velelocidade_tiro = 0.0;

bool flag_turbo = false;

bool flagSensores = false;

status_game status_atual = INIT;

char texto_fim[] = "DEU RUIM";
char text_fim_reseta[] = "press r para recomecar";
char texto_vencedor[] = "YOU WIN!!!";
char text_init[] = "press space para iniciar...";

float ESCALA = 1.0;

////////
bool vitoria = false;
int QTD_MAX_VIDAS = 5;
bool flagsVolta[4];
int num_voltas = 0;
int voltas_max = 5;

float angRoda = 0, angCarro = 0, angCanhao = 0;
int x_ant = 0; //captura as infs do mouse para virar o canhao

int l_chassi = 20, c_chassi = 45; //retangulo central
int l_canhao = 3, c_canhao = 12;
int l_eixo = 5, c_eixo = 17.5 + l_chassi;
int l_roda = 7.5, c_roda = 12.5;

float dist_centro_rot_chassi = (c_chassi / 2.0) - (c_roda / 2.0); //em cima do eixo trazeiro 
float dist_centro_rot_canhao = (c_canhao / 2) * 0.9; //rotaciona a 90% do braco do chassi a partir do centro

/////////////////////////////  CARRO //////////////////////////////

//////////////////////////// JOYSTICK /////////////////////////////
bool btn_r2 = false;
bool btn_l2 = false;
bool btn_r1 = false;
bool btn_l1 = false;
bool btn_triangle = false;
int  x_axis  = 0;
int  y_axis  = 0;
int  z_axis  = 0;

#endif

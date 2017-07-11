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

/*
#define TIPO_PISTA 0
#define TIPO_LARG_CHEGADA 1
#define TIPO_INIMIGO 2
#define TIPO_CARRO 3
#define TIPO_RODA_SUP 4
#define TIPO_RODA_INF 5
#define TIPO_EIXO 6
#define TIPO_CHASSI 7
#define TIPO_CANHAO 8
#define TIPO_CIRC 9
#define TIPO_VEL_NORMAL 10
#define TIPO_VEL_TURBO 11
*/
 

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
	int tipo;
	float raio;
	trans t;
	cor c;
}circ;

typedef struct sensor{
	float x, y;
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

bool teclas[256]; 	//256 qtd de teclas contidas no padrao ASCII

int status = START;

bool flagJoystick = false;

bool gameOver = false;

/////////////////////////////  CARRO //////////////////////////////
float angMaxRoda = 45.0, angMaxCanhao = 45.0;
///////
float angReferenciaCarro = 90.0; //os retangulos sao desenhados com o comprimento no eixo Y
//mecher aqui somente se a funcao de desenhar retangulo for alterada

float velelocidade_carro = 1.2;
float velelocidade_tiro = 0.0;

bool flag_turbo = false;

bool flagSensores = false;

float ESCALA = 1.0;

////////
float angRoda = 0, angCarro = 0, angCanhao = 0;
int x_ant = 0; //captura as infs do mouse para virar o canhao

int l_chassi = 20, c_chassi = 45; //retangulo central
int l_canhao = 3, c_canhao = 12;
int l_eixo = 5, c_eixo = 17.5 + l_chassi;
int l_roda = 7.5, c_roda = 12.5;

float dist_centro_rot_chassi = (c_chassi / 2.0) - (c_roda / 2.0); //em cima do eixo trazeiro 
float dist_centro_rot_canhao = (c_canhao / 2) * 0.9; //rotaciona a 90% do braco do chassi a partir do centro

/////////////////////////////  CARRO //////////////////////////////

#endif

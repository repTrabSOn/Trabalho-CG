#include "globais.h"
#include "parseXML.cpp"

Arena arena;

void cria_carro(void); //funcao responsável por criar todas as partes do carro
void desenha_arena(void);
void desenha_carro(void);
void desenha_tiros(void);
void desenha_fogs(void);
void tecladoOps(void);
void draw_circle(float r, cor rgb);
void desenha_sensores(bool flag);
void desenha_tiros_inimigos(void);
void cria_tiros_inimigos(void);
bool detectaColisaoTiroPlayerInimigo (Carro car, circ tiro);
void respawnEnemy (int i);
void joystickOPS(void);


/*
	Funcao que carrega as estruturas de dados de arena
*/
void load_arena(char * path){ //OK
	arena = load_config_file(path); //declarada globalmente em "globais.h"
	//identificando as dimensoes da arena
	p_maior = 0;
	p_menor = 1;
	for(int i = 1; i < arena.enemys.size(); i++){
		if(arena.enemys[i].c.raio > arena.enemys[i].c.raio){
			p_menor = p_maior;
			p_maior = i;
		}
	}

	cor c_arena_centro;
	c_arena_centro.r = 1.0;
	c_arena_centro.g = 1.0;
	c_arena_centro.b = 1.0;

	arena.enemys[p_menor].c.c = c_arena_centro;
	//apenas colocando a cor do centro como branca..

	raio_maior = arena.enemys[p_maior].c.raio;
	raio_menor = arena.enemys[p_menor].c.raio;

	x_ini = arena.enemys[p_maior].c.t.tx - raio_maior;
	x_fim = arena.enemys[p_menor].c.t.tx + raio_maior;
	y_ini = x_ini;
	y_fim = x_fim;

	x_centro = x_ini + raio_maior;
	y_centro = y_ini + raio_maior;

	//iniciando movimento dos inimigos
	for(int i = 0; i < arena.enemys.size(); i++){
		if(arena.enemys[i].c.tipo == TIPO_INIMIGO){
			if(arena.enemys[i].c.t.tx < x_centro){
				arena.enemys[i].c.t.rz = 180 - arena.enemys[i].c.t.rz;
				if(arena.enemys[i].c.t.ty < y_centro){
					arena.enemys[i].c.t.rz += 90;
				}
			}else{
				if(arena.enemys[i].c.t.ty < y_centro){
					arena.enemys[i].c.t.rz = 360 - arena.enemys[i].c.t.rz;
				}
			}
		}
	}

	//iniciar checkpoints aqui
	//1 chekpoint para cada inimigo
	int ang_ant = 0;
	for(int i = 0; i < arena.enemys.size(); i++){
		if(arena.enemys[i].c.tipo == TIPO_INIMIGO){
			CheckPoint c;
			c.flag_ativo = true;
			c.flag_direcao = rand()%2;
			c.vel = 2.5;
			c.num_tiros = rand()%21;
			c.c.t.rz = rand()%360;
			c.c.raio = arena.enemys[i].c.raio / 2.0;
			cor C;
			C.r = 1.0; C.g = 0.0; C.b = 1.0;
			c.c.c = C;

			if(i > 0){
				ang_ant += rand()%130;
				c.c.t.rz = ang_ant;
			}

			float d = (raio_menor + (raio_maior - raio_menor)/2.0);
			c.c.t.tx = x_centro + d*cos(c.c.t.rz*M_PI/180.0);
			c.c.t.ty = y_centro + d*sin(c.c.t.rz*M_PI/180.0);
			arena.checks.push_back(c);
		}		
	}

	cria_carro();
}

void inicializa(void){ //OK
	gluOrtho2D(x_ini, x_fim, y_ini, y_fim);
}		

/*
	Funcao principal de desenhos
*/
void desenha(void){

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	desenha_arena();

	desenha_sensores(flagSensores);

	desenha_tiros();

	desenha_tiros_inimigos();

	desenha_fogs();

	desenha_carro();
	
	if(flagJoystick){
		joystickOPS();
	}else{
		tecladoOps();
	}
	
	glutSwapBuffers();	
}

//////////////////////////////////// DESENHOS ///////////////////////////////
void desenha_sensores(bool flag){
	if(flag){
		for(int i = 0; i < arena.cars.size(); i++) {
			glBegin(GL_LINE_LOOP);
				float r = (c_chassi/2.0 + c_canhao/2.0 + dist_centro_rot_canhao) * ESCALA;
				int x = arena.cars[i].sensores[0].x;
				int y = arena.cars[i].sensores[0].y;
				glColor3f(0.0, 1.0, 0.0);
				for(int i = 0; i < NUM_LINHAS; i++){
					float ang = i * M_PI / 180.0;
					glVertex3f(x + r * cos(ang), y + r * sin(ang), 0.0);
				}
			glEnd();
		}
	}
}

void draw_circle(float r, cor rgb){ //desenha centro na origem
	glBegin(GL_POLYGON);
		glColor3f(rgb.r, rgb.g, rgb.b);
		for(int i = 0; i < NUM_LINHAS; i++){
			float ang = i * M_PI / 180.0;
			glVertex3f(r * cos(ang), r * sin(ang), 0.0);
		}
	glEnd();
}

void draw_rect(float larg, float comp, cor rgb){ //desenha centro na origem
	glBegin(GL_QUADS);
		glColor3f(rgb.r, rgb.g, rgb.b);
		glVertex3f(-larg/2.0, -comp/2.0, 0.0);
		glVertex3f(larg/2.0, -comp/2.0, 0.0);
		glVertex3f(larg/2.0, comp/2.0, 0.0);
		glVertex3f(-larg/2.0, comp/2.0, 0.0);
	glEnd();
}

void draw_tires(float larg, float comp, cor rgb){//desenha o centro dos pneus na origem
	float r_i = 3; //suavidade da curvatura dos pneus
	glBegin(GL_POLYGON);
		glColor3f(rgb.r, rgb.g, rgb.b);

		for(int i = 3*NUM_LINHAS/4; i < NUM_LINHAS; i++){
			float ang = i * 2 * M_PI / NUM_LINHAS; //quero 1/4 de circulo
			glVertex3f(larg/2.0 - r_i + (cos(ang) * r_i), -comp/2.0 + (sin(ang) * r_i) + r_i, 0);
		}

		for(int i = 0; i < NUM_LINHAS / 4; i++){
			float ang = i * 2 * M_PI / NUM_LINHAS; //quero 1/4 de circulo
			glVertex3f(larg/2.0 - r_i + (cos(ang) * r_i), comp/2.0 + (sin(ang) * r_i) - r_i, 0);
		}

		for(int i = NUM_LINHAS/4; i < NUM_LINHAS / 2; i++){
			float ang = i * 2 * M_PI / NUM_LINHAS; //quero 1/4 de circulo
			glVertex3f(-larg/2.0 + r_i + (cos(ang) * r_i), comp/2.0 + (sin(ang) * r_i) - r_i, 0);
		}

		for(int i = NUM_LINHAS / 2; i < 3*NUM_LINHAS / 4; i++){
			float ang = i * 2 * M_PI / NUM_LINHAS; //quero 1/4 de circulo
			glVertex3f(-larg/2.0 + r_i + (cos(ang) * r_i), -comp/2.0 + (sin(ang) * r_i) + r_i, 0);
		}

	glEnd();
}

void respawnEnemy (int i){
	arena.enemys[i].vida = 3;
	arena.enemys[i].c.c.r = 1;
	arena.enemys[i].flag_vivo = true;
}

void reseta_checkpoints(int arg){
	if(!arena.checks.empty()){
		int ang_ant = 0;
		for(int i = 0; i < arena.checks.size(); i++){
			arena.checks[i].num_tiros = rand()%21;
			arena.checks[i].c.t.rz = rand()%360;
			if(i > 0){
				ang_ant += rand()%130;
				arena.checks[i].c.t.rz = ang_ant;
			}

			float d = (raio_menor + (raio_maior - raio_menor)/2.0);
			arena.checks[i].c.t.tx = x_centro + d*cos(arena.checks[i].c.t.rz*M_PI/180.0);
			arena.checks[i].c.t.ty = y_centro + d*sin(arena.checks[i].c.t.rz*M_PI/180.0);
		}
	}
	glutPostRedisplay();
	glutTimerFunc(10000, reseta_checkpoints, 1);
}

void restaura_checkpoints(int arg){
	if(!arena.checks.empty()){
		for(int i = 0; i < arena.checks.size(); i++){
			arena.checks[i].flag_ativo = true;
			arena.checks[i].num_tiros = rand()%21;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(10000, restaura_checkpoints, 1);
}

void desenha_arena(void){

	for(int i = 0; i < arena.enemys.size(); i++){
		if(arena.enemys[i].c.tipo == TIPO_PISTA || arena.enemys[i].flag_vivo){
			glPushMatrix();
				glTranslatef(arena.enemys[i].c.t.tx, arena.enemys[i].c.t.ty, 0.0);
				draw_circle(arena.enemys[i].c.raio, arena.enemys[i].c.c);
			glPopMatrix();

			if(arena.enemys[i].c.tipo == TIPO_INIMIGO){
				arena.enemys[i].movimento();
			}
		}
		else if (!arena.enemys[i].flag_vivo)
			glutTimerFunc(15000, respawnEnemy, i);
	}

	for(int i = 0; i < arena.rects.size(); i++){
		glPushMatrix();
			glTranslatef(arena.rects[i].t.tx, arena.rects[i].t.ty, 0.0);
			draw_rect(arena.rects[i].larg, arena.rects[i].comp, arena.rects[i].c);
		glPopMatrix();
	}

	for(int i = 0; i < arena.checks.size(); i++){
		if(arena.checks[i].flag_ativo){
			glPushMatrix();
				glTranslatef(arena.checks[i].c.t.tx, arena.checks[i].c.t.ty, 0.0);
				draw_circle(arena.checks[i].c.raio, arena.checks[i].c.c);
			glPopMatrix();

			arena.checks[i].movimento();
		}
	}

	glutPostRedisplay();
}

void desenha_carro(void){
	for(int j = 0; j < arena.cars.size(); j++){
		glPushMatrix();
			glTranslatef(arena.cars[j].t_carro.tx, arena.cars[j].t_carro.ty - dist_centro_rot_chassi, 0);
			glRotatef(arena.cars[j].t_carro.rz, 0, 0, 1);
			glTranslatef(0.0, dist_centro_rot_chassi, 0.0);
			glScalef(ESCALA, ESCALA, 1.0);

			
			if(status == START){
				int ind = 2; //referente ao indice do detail normal
				if(flag_turbo){
					ind = 1; //referente ao indice do detail turbo
				}
				glPushMatrix();
					glTranslatef(arena.cars[j].details[ind].t.tx, arena.cars[j].details[ind].t.ty, 0);
					glScalef(arena.cars[j].details[ind].t.ex, arena.cars[j].details[ind].t.ey, 1.0);
					draw_circle(arena.cars[j].details[ind].raio, arena.cars[j].details[ind].c);
				glPopMatrix();
			}	
				
			for(int i = 0; i < arena.cars[j].partes_estaticas.size(); i++){
				glPushMatrix();
					glTranslatef(arena.cars[j].partes_estaticas[i].t.tx, arena.cars[j].partes_estaticas[i].t.ty, 0);
					glRotatef(arena.cars[j].partes_estaticas[i].t.rz, 0.0, 0.0, 1.0);
					draw_rect(arena.cars[j].partes_estaticas[i].larg, arena.cars[j].partes_estaticas[i].comp, arena.cars[j].partes_estaticas[i].c);
				glPopMatrix();
			}
			for(int i = 0; i < arena.cars[j].armas.size(); i++){
				glPushMatrix();
					glTranslatef(arena.cars[j].armas[i].t.tx, arena.cars[j].armas[i].t.ty, 0);
					glRotatef(arena.cars[j].armas[i].t.rz, 0.0, 0.0, 1.0);
					glTranslatef(0.0, dist_centro_rot_canhao, 0.0);
					draw_rect(arena.cars[j].armas[i].larg, arena.cars[j].armas[i].comp, arena.cars[j].armas[i].c);
				glPopMatrix();
			}
			for(int i = 0; i < arena.cars[j].rodas_estaticas.size(); i++){
				glPushMatrix();
					glTranslatef(arena.cars[j].rodas_estaticas[i].t.tx, arena.cars[j].rodas_estaticas[i].t.ty, 0);
					draw_tires(arena.cars[j].rodas_estaticas[i].larg, arena.cars[j].rodas_estaticas[i].comp, arena.cars[j].rodas_estaticas[i].c);
				glPopMatrix();
			}
			for(int i = 0; i < arena.cars[j].rodas_articuldas.size(); i++){
				glPushMatrix();
					glTranslatef(arena.cars[j].rodas_articuldas[i].t.tx, arena.cars[j].rodas_articuldas[i].t.ty, 0);
					glRotatef(arena.cars[j].rodas_articuldas[i].t.rz, 0.0, 0.0, 1.0);
					draw_tires(arena.cars[j].rodas_articuldas[i].larg, arena.cars[j].rodas_articuldas[i].comp, arena.cars[j].rodas_articuldas[i].c);
				glPopMatrix();
			}		

			//oval
			glPushMatrix();
				glTranslatef(arena.cars[j].details[0].t.tx, arena.cars[j].details[0].t.ty, 0);
				glScalef(arena.cars[j].details[0].t.ex, arena.cars[j].details[0].t.ey, 1.0);
				draw_circle(arena.cars[j].details[0].raio, arena.cars[j].details[0].c);
			glPopMatrix();

		glPopMatrix();
	}
	glutPostRedisplay();
}

void cria_carro(void){
	//desenhando na ordem que o desenho fique correto
	//variaveis numéricas definidas em "globais.h"
	for(int i = 0; i < arena.cars.size(); i++){

		arena.cars[i].qtd_tiros = 10;

		arena.cars[i].vel_carro = velelocidade_carro;
		arena.cars[i].range_tiro = raio_maior * 2;
		arena.cars[i].t_carro.rz = 0;

		//sensores

		sensor s1;
		s1.x = arena.cars[i].t_carro.tx;
		s1.y = arena.cars[i].t_carro.ty;

		arena.cars[i].sensores.push_back(s1);


		//Eixos
		rect e_sup;
		e_sup.c.r = 1.0; e_sup.c.g = 1.0; e_sup.c.b = 1.0;
		e_sup.comp = l_eixo; e_sup.larg = c_eixo;
		//esta trocado de proposito, pq desse jeito não precisa rotacionar
		//draw_rect desenha com o comprimento em Y
		e_sup.t.tx = 0;
		e_sup.t.ty = c_chassi/2.0 - c_roda/2.0;

		arena.cars[i].partes_estaticas.push_back(e_sup);

		rect e_inf = e_sup;
		e_inf.t.ty = - c_chassi/2.0 + c_roda/2.0;

		arena.cars[i].partes_estaticas.push_back(e_inf);

		//CANHAO
		rect canhao;
		canhao.tipo = TIPO_CANHAO;
		canhao.c.r = 0.0; canhao.c.g = 1.0; canhao.c.r = 127/255.0;
		canhao.comp = c_canhao; canhao.larg = l_canhao;
		canhao.t.tx = 0;
		canhao.t.ty = c_chassi/2.0;

		arena.cars[i].armas.push_back(canhao);


		//CHASSI	
		rect chassi;
		chassi.tipo = TIPO_CHASSI;
		chassi.c.r = 0.0; chassi.c.g = 1.0; chassi.c.b = 0.0;
		chassi.comp = c_chassi; chassi.larg = l_chassi;
		chassi.t.tx = 0;
		chassi.t.ty = 0;

		arena.cars[i].partes_estaticas.push_back(chassi);

		//RODAS
		rect roda_sup_esq;
		roda_sup_esq.tipo = TIPO_RODA_SUP;
		roda_sup_esq.c.r = 1.0; roda_sup_esq.c.g = 0.0; roda_sup_esq.c.b = 0.0;
		roda_sup_esq.comp = c_roda; roda_sup_esq.larg = l_roda;
		roda_sup_esq.t.tx = -c_eixo/2.0;
		roda_sup_esq.t.ty = c_chassi/2.0 - c_roda/2.0;
		roda_sup_esq.t.rz = 0;

		arena.cars[i].rodas_articuldas.push_back(roda_sup_esq);

		rect roda_sup_dir = roda_sup_esq;
		roda_sup_dir.t.tx = c_eixo/2.0;

		arena.cars[i].rodas_articuldas.push_back(roda_sup_dir);

		rect roda_inf_esq = roda_sup_esq;
		roda_inf_esq.tipo = TIPO_RODA_INF;
		roda_inf_esq.t.ty = -c_chassi/2.0 + c_roda/2.0;

		arena.cars[i].rodas_estaticas.push_back(roda_inf_esq);

		rect roda_inf_dir = roda_sup_dir;

		roda_inf_dir.tipo = TIPO_RODA_INF;
		roda_inf_dir.t.ty = -c_chassi/2.0 + c_roda/2.0;

		arena.cars[i].rodas_estaticas.push_back(roda_inf_dir);

		//pintura central e fogo
		circ oval;
		oval.tipo = TIPO_CIRC;
		oval.raio = 10;
		oval.c.r = 0.0; oval.c.g = 0.8; oval.c.b = 0.0;
		oval.t.tx = 0.0;
		oval.t.ty = 0.0;
		oval.t.ex = -1/2.0;
		oval.t.ey = oval.t.ez = 1.0;
		
		arena.cars[0].details.push_back(oval);

		circ vel_t;					//indice 1
		vel_t.tipo = TIPO_VEL_TURBO;
		vel_t.raio = 10;
		vel_t.c.r = 0.0; vel_t.c.g = 191/255.0; vel_t.c.b = 1.0;
		vel_t.t.ex = -1/2.0;
		vel_t.t.ey = vel_t.t.ez = 1.0;
		vel_t.t.tx = 0.0;
		vel_t.t.ty = -25;
		
		arena.cars[0].details.push_back(vel_t);

		circ vel_n;					//indice 2
		vel_n.tipo = TIPO_VEL_NORMAL;
		vel_n.raio = 10;
		vel_n.c.r = 1.0; vel_n.c.g = 140/255.0; vel_n.c.b = 0.0;
		vel_n.t.ex = -1/2.0;
		vel_n.t.ey = vel_n.t.ez = 1.0;
		vel_n.t.tx = 0.0;
		vel_n.t.ty = -20; 
		
		arena.cars[0].details.push_back(vel_n);
	}	
}

////// tiros

// Faz oq o nome diz
bool detectaColisaoTiroPlayerInimigo (Carro car, circ tiro){
	bool flagResp = false;
	for(int i = 0; i < arena.enemys.size(); i++){
		if(!arena.enemys[i].tiros.empty()){
			float d = car.dist(tiro.t.tx, tiro.t.ty, arena.enemys[i].c.t.tx, arena.enemys[i].c.t.ty);
			if(arena.enemys[i].c.tipo == TIPO_INIMIGO && arena.enemys[i].flag_vivo){
				if(d - tiro.raio < arena.enemys[i].c.raio){
					flagResp  = true;
					arena.enemys[i].vida--;
					arena.enemys[i].c.c.r -= 0.3;
					if (arena.enemys[i].vida == 0)
						arena.enemys[i].flag_vivo = false;
				}
			}
			
		}

	}
	return flagResp;

}

bool detectaColisaoTiroInimigoPlayer (Carro car, circ tiro){
	bool flagResp = false;
	std::vector<sensor> s_aux = car.sensores;
	for(int i = 0; i < car.sensores.size(); i++){
			float d = car.dist(tiro.t.tx, tiro.t.ty, car.sensores[i].x, car.sensores[i].y);
			if(car.flag_vivo){
				if(d - tiro.raio < arena.enemys[i].c.raio){
					flagResp  = true;
					car.vida--;
					if (car.vida == 0)
						car.flag_vivo = false;
				}
			}

	}
	return flagResp;

}

void desenha_tiros_inimigos(void){
	if(arena.enemys.size() > 0){
		for(int i = 0; i < arena.enemys.size(); i++){
			if(!arena.enemys[i].tiros.empty()){
				for(int j = 0; j < arena.enemys[i].tiros.size(); j++){

					//Verifica aki a colisao de tiros inimigos com o player
					if (detectaColisaoTiroInimigoPlayer(arena.cars[0], arena.enemys[i].tiros[i])){
						arena.enemys[i].tiros[i] = arena.enemys[i].tiros[arena.enemys[i].tiros.size() - 1];
						arena.enemys[i].tiros.pop_back();
						i--;
					}else{

						glPushMatrix();
							glTranslatef(arena.enemys[i].tiros[j].t.tx, arena.enemys[i].tiros[j].t.ty, 0.0);
							draw_circle(arena.enemys[i].tiros[j].raio, arena.enemys[i].tiros[j].c);
						glPopMatrix();

						arena.enemys[i].tiros[j].t.tx += arena.enemys[i].vel_tiro * sin((arena.enemys[i].tiros[j].t.rz) * M_PI / 180.0);
						arena.enemys[i].tiros[j].t.ty -= arena.enemys[i].vel_tiro * cos((arena.enemys[i].tiros[j].t.rz) * M_PI / 180.0);
					}
		
				}
				arena.enemys[i].limpa_tiros();
			}			
		}
	}
}


void cria_tiros_inimigos(int arg){
	int time = 0;
	for(int i = 0; i < arena.enemys.size(); i++){
		if(arena.enemys[i].c.tipo == TIPO_INIMIGO && arena.enemys[i].flag_vivo){
			arena.enemys[i].atira(arena.cars[0].t_carro.tx, arena.cars[0].t_carro.ty);
			time = 1 / arena.enemys[i].freq_tiro;
		}
	}
	//int time = 1500 + rand()%5000;
	glutTimerFunc(time, cria_tiros_inimigos, 1);
	glutPostRedisplay();
}

void desenha_tiros(void){
	if(!arena.cars[0].tiros.empty()){
		for(int i = 0; i < arena.cars[0].tiros.size(); i++){

			// verificar aki se tiro colide com inimigo
			// se colide entao nao o desenha
			if (detectaColisaoTiroPlayerInimigo(arena.cars[0], arena.cars[0].tiros[i])){
				arena.cars[0].tiros[i] = arena.cars[0].tiros[arena.cars[0].tiros.size() - 1];
				arena.cars[0].tiros.pop_back();
				i--;
			}else{
				glPushMatrix();
					glTranslatef(arena.cars[0].tiros[i].t.tx, arena.cars[0].tiros[i].t.ty, 0.0);
					glScalef(ESCALA, ESCALA, 0.0);
					draw_circle(arena.cars[0].tiros[i].raio, arena.cars[0].tiros[i].c);
				glPopMatrix();

				arena.cars[0].tiros[i].t.tx += arena.cars[0].vel_tiro * sin((arena.cars[0].tiros[i].t.rz + ANG_REF) * M_PI / 180.0);
				arena.cars[0].tiros[i].t.ty -= arena.cars[0].vel_tiro * cos((arena.cars[0].tiros[i].t.rz + ANG_REF) * M_PI / 180.0);
			}
		}

		arena.cars[0].limpa_tiros();

		glutPostRedisplay();
	}
}

void desenha_fogs(void){
	if(!arena.cars[0].fogs.empty()){
		for(int i = 0; i < arena.cars[0].fogs.size(); i++){
			glPushMatrix();
				glTranslatef(arena.cars[0].fogs[i].t.tx, arena.cars[0].fogs[i].t.ty, 0.0);
				glScalef(ESCALA, ESCALA, 0.0);
				draw_circle(arena.cars[0].fogs[i].raio, arena.cars[0].fogs[i].c);
			glPopMatrix();

			arena.cars[0].fogs[i].t.tx += sin((arena.cars[0].fogs[i].t.rz + ANG_REF) * M_PI / 180.0);
			arena.cars[0].fogs[i].t.ty -= cos((arena.cars[0].fogs[i].t.rz + ANG_REF) * M_PI / 180.0);
		}

		glutPostRedisplay();
	}
}

void limpa_fogs(int p){
	for(int i = 0; i < arena.cars.size(); i++){
		if(!arena.cars[i].fogs.empty()){
			arena.cars[i].fogs.erase(arena.cars[i].fogs.begin(), arena.cars[i].fogs.begin() + arena.cars[i].fogs.size()/2 + 1);
		}
	}
	glutPostRedisplay();
	glutTimerFunc(500, limpa_fogs, 1);
}

//////////////////////////////// TECLADO /////////////////////////////////////////

circ novoFog(bool turbo){
	circ novo;
	int angAbertura = 70;
	float dist_chassi = c_chassi/2.0 + dist_centro_rot_chassi;
	float x_carro = arena.cars[0].t_carro.tx;
	float y_carro = arena.cars[0].t_carro.ty;
	float angCarro = arena.cars[0].t_carro.rz;
	float angSort = (float)(rand()%angAbertura);
	novo.t.tx = x_carro + cos((angCarro + ANG_REF + (180 - angAbertura)/2.0 + 90 + angSort) * M_PI / 180.0) * dist_chassi/2.0;
	novo.t.ty = y_carro + sin((angCarro + ANG_REF + (180 - angAbertura)/2.0 + 90 + angSort) * M_PI / 180.0) * dist_chassi/2.0 - dist_centro_rot_chassi;
	novo.t.rz = angCarro + ANG_REF + (180 - angAbertura)/2.0 + 90 + angSort;
	if(turbo){
		novo.raio = 2;
		novo.c.r = 0.0; novo.c.g = 191/255.0; novo.c.b = 1.0;
	}else{
		novo.raio = 1;
		novo.c.r = 1.0; novo.c.g = 140/255.0; novo.c.b = 0.0;
	}
	
	return novo;
}

void teclaUp(unsigned char key, int x, int y){
	teclas[key] = false;
}

void teclaPress(unsigned char key, int x, int y){
	if(key == 'j' || key == 'J'){
		flagJoystick = !flagJoystick;
		cout << endl << "Controle atual: ";
		if(flagJoystick){
			cout << "JOYSTICK" << endl;
		}else{
			cout << "TECLADO" << endl;
		}
	}
	teclas[key] = true;
}

void tecladoOps(void){
	//para manter a roda sempre na posicao original caso nao tenha nada pressionado
	if(teclas['a'] == false && teclas['d'] == false){
		for(int i = 0; i < arena.cars[0].rodas_articuldas.size(); i++){
			arena.cars[0].rodas_articuldas[i].t.rz = 0;
		}
	}

	if(teclas['-']){
		flagSensores = false;
	}
	else if(teclas['=']){
		flagSensores = true;
	}

	if(teclas[32]){ //space
		flag_turbo = true;
	}else{
		flag_turbo = false;
	}

	if(teclas['w'] == true || teclas['s'] == true){
		status = START;	
		if(teclas['s'] == true){
			flag_turbo = false;
			
		}
		arena.cars[0].fogs.push_back(novoFog(flag_turbo));
	}else{
		status = STOP;
	}
	
	if(teclas['w'] == true && teclas['a'] == true){
		if(arena.cars[0].rodas_articuldas[0].t.rz <= angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz += TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz += TAXA_ROT_RODA;
		}
		arena.cars[0].acelerar('w', flag_turbo, arena.cars[0].rodas_articuldas[0].t.rz / TAXA_ROT_CARRO, raio_menor, raio_maior, arena.enemys);
	}
	else if(teclas['w'] == true && teclas['d'] == true){
		if(arena.cars[0].rodas_articuldas[0].t.rz >= -angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz -= TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz -= TAXA_ROT_RODA;
		}
		arena.cars[0].acelerar('w', flag_turbo, arena.cars[0].rodas_articuldas[0].t.rz / TAXA_ROT_CARRO, raio_menor, raio_maior, arena.enemys);
	}
	else if(teclas['s'] == true && teclas['d'] == true){
		if(arena.cars[0].rodas_articuldas[0].t.rz >= -angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz -= TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz -= TAXA_ROT_RODA;
		}
		arena.cars[0].acelerar('s', flag_turbo, -arena.cars[0].rodas_articuldas[0].t.rz / TAXA_ROT_CARRO, raio_menor, raio_maior, arena.enemys);
	}
	else if(teclas['s'] == true && teclas['a'] == true){
		if(arena.cars[0].rodas_articuldas[0].t.rz <= angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz += TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz += TAXA_ROT_RODA;
		}
		arena.cars[0].acelerar('s', flag_turbo, -arena.cars[0].rodas_articuldas[0].t.rz / TAXA_ROT_CARRO, raio_menor, raio_maior, arena.enemys);
	}
	else if(teclas['w']){
		arena.cars[0].acelerar('w', flag_turbo, 0, raio_menor, raio_maior, arena.enemys);
	}
	else if(teclas['s']){
		arena.cars[0].acelerar('s', flag_turbo,  0, raio_menor, raio_maior, arena.enemys);
	}
	else if(teclas['a']){
		if(arena.cars[0].rodas_articuldas[0].t.rz <= angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz += TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz += TAXA_ROT_RODA;
		}
	}
	else if(teclas['d']){
		if(arena.cars[0].rodas_articuldas[0].t.rz >= -angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz -= TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz -= TAXA_ROT_RODA;
		}
	}

	if(teclas['w'] || teclas['s']){
		arena.cars[0].coletar_chekpoints(&arena.checks);
	}

	if(arena.cars[0].t_carro.rz > 360){
		arena.cars[0].t_carro.rz -= 360;
	} 
	else if(arena.cars[0].t_carro.rz < -360){
		arena.cars[0].t_carro.rz += 360;
	}

	glutPostRedisplay();
}

//////////////////////////////// MOUSE /////////////////////////////////////////
circ novoTiro(void){
	circ novo;
	float dist_chassi = c_chassi/2.0 + dist_centro_rot_chassi; 
	float dist_canhao = c_canhao/2.0 + dist_centro_rot_canhao;
	float x_carro = arena.cars[0].t_carro.tx;
	float y_carro = arena.cars[0].t_carro.ty;
	float angCarro = arena.cars[0].t_carro.rz;
	float angCanhao = arena.cars[0].armas[0].t.rz;
	//pega o do sensor pq esta rotacionado junto com o carrinho
	novo.t.tx = arena.cars[0].sensores[0].x + (cos((angCarro + ANG_REF) * M_PI / 180.0) * (dist_chassi/2.0)
						+ cos((angCanhao + angCarro + ANG_REF) * M_PI / 180.0)
						*(dist_centro_rot_canhao + dist_canhao/2.0))* ESCALA;

	novo.t.ty = arena.cars[0].sensores[0].y + (sin((angCarro + ANG_REF) * M_PI / 180.0) * (dist_chassi/2.0) 
						+ sin((angCanhao + angCarro + ANG_REF) * M_PI / 180.0)
						*(dist_centro_rot_canhao + dist_canhao/2.0))* ESCALA;
	novo.raio = 3;
	novo.t.rz = angCarro + angCanhao + ANG_REF;
	novo.c.r = 1.0; novo.c.g = 1.0; novo.c.b = 0.0;
	return novo;
}

void mouse(int btn, int state, int x, int y){
	if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		if(arena.cars[0].qtd_tiros > 0){
			arena.cars[0].tiros.push_back(novoTiro());
			arena.cars[0].qtd_tiros--;
		}
	} 
	if ((btn == 3) || (btn == 4)){//scroll
       // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
       if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
       	//printf("Scroll %s At %d %d\n", (btn == 3) ? "Up" : "Down", x, y);
       if(btn == 3){
       		ESCALA += 0.3;
       }else{
       		if(ESCALA - 0.3 > 0)
       			ESCALA -= 0.3;
       }
   }
	glutPostRedisplay();
}

void passiveMotion(int x, int y){
	//captura as infs de x e y anteriores ao clique
	float deltaX = x - x_ant;

	deltaX /= FATOR_ANG_CANHAO;

	float ang_ref = abs(arena.cars[0].t_carro.rz);

	if(ang_ref >= 90 && ang_ref < 270){
		deltaX *= -1;
	}

	if(deltaX > 0){//rotaciona Anti horário
		if(arena.cars[0].armas[0].t.rz - deltaX >= -angMaxCanhao){
			arena.cars[0].armas[0].t.rz -= deltaX;
		}else{
			arena.cars[0].armas[0].t.rz = -angMaxCanhao;
		}
	}
	else if(deltaX < 0){//rotaciona no sentido horário
		if(arena.cars[0].armas[0].t.rz - deltaX <= angMaxCanhao){
			arena.cars[0].armas[0].t.rz -= deltaX;
		}else{
			arena.cars[0].armas[0].t.rz = angMaxCanhao;
		}
	}
	
	x_ant = x;	
}


//////////////////////////////   JOYSTICK

bool btn_r2 = false;
bool btn_l2 = false;
bool btn_r1 = false;
bool btn_l1 = false;
bool btn_triangle = false;
int  x_axis  = 0;
int  y_axis  = 0;
int  z_axis  = 0;

void joystick(unsigned int btn, int x, int y, int z){
     x_axis  = x;
     y_axis  = y * -1;
     z_axis  = z * -1;

     //cout << "[x, y, z] : [" <<x_axis<<", "<<y_axis<<", " << z_axis<<"]" << endl;  
     //cout << "BOTAO: "<<btn << endl << endl;

    /*
		btn = 32 -> R2
		btn = 16 -> L2
		btn = 128 -> R1
		btn = 64 = L1
		x -> analogico esquerdo, movimento horizontal
		z -> analogico direito, movimento vertical
		btn = 1 -> triangulo
    */ 

     if(btn == 32){
     	btn_r2 = true;
     }else{
     	btn_r2 = false;
     }

     if(btn == 64){
     	btn_l1 = true;
     }else{
     	btn_l1 = false;
     }
     
     if(btn == 16){
     	btn_l2 = true;
     }else{
     	btn_l2 = false;
     }
     
     if(btn == 128){
     	btn_r1 = true;
     }else{
     	btn_r1 = false;
     }
     
     if(btn == 1){
     	btn_triangle = true;
     }
 	
 
}  

void joystickOPS(void){

	if(x_axis == 0){
		arena.cars[0].rodas_articuldas[0].t.rz = 0;
		arena.cars[0].rodas_articuldas[1].t.rz = 0;
	}	
	else if(x_axis < 0){
		if(arena.cars[0].rodas_articuldas[0].t.rz <= angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz += TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz += TAXA_ROT_RODA;
		}
	}
	else if(x_axis > 0){
		if(arena.cars[0].rodas_articuldas[0].t.rz >= -angMaxRoda){
			arena.cars[0].rodas_articuldas[0].t.rz -= TAXA_ROT_RODA;
			arena.cars[0].rodas_articuldas[1].t.rz -= TAXA_ROT_RODA;
		}
	}

	float ang = 0;
	if(z_axis > 0){
		status = START;
		if(btn_l1){
			flag_turbo = true;
		}else{
			flag_turbo = false;
		}

		arena.cars[0].fogs.push_back(novoFog(flag_turbo));
		if(x_axis != 0){
			ang = arena.cars[0].rodas_articuldas[0].t.rz / TAXA_ROT_CARRO;
		}

		float vel_aux = arena.cars[0].vel_carro;
		arena.cars[0].vel_carro *= ((z_axis + 300) / 1000.0);
		arena.cars[0].acelerar('w', btn_l1, ang, raio_menor, raio_maior, arena.enemys);
		arena.cars[0].vel_carro = vel_aux;
	}
	else if(z_axis < 0){
		status = START;
		flag_turbo = false;
		arena.cars[0].fogs.push_back(novoFog(flag_turbo));
		if(x_axis != 0){
			ang = -arena.cars[0].rodas_articuldas[0].t.rz / TAXA_ROT_CARRO;
		}
		float vel_aux = arena.cars[0].vel_carro;
		arena.cars[0].vel_carro *= ((-z_axis + 300) / 1000.0);
		arena.cars[0].acelerar('s', btn_l1, ang, raio_menor, raio_maior, arena.enemys);
		arena.cars[0].vel_carro = vel_aux;
	}else{
		status = STOP;
	}

	if(btn_triangle){
		flagSensores = !flagSensores;
		btn_triangle = false;
	}


	float ang_ref = abs(arena.cars[0].t_carro.rz);
	float inc = 5;

	if(ang_ref >= 90 && ang_ref < 270){
		inc *= -1;
	} 

	if(btn_l2){
		if(arena.cars[0].armas[0].t.rz + inc <= angMaxCanhao){
			arena.cars[0].armas[0].t.rz += inc;
		}else{
			arena.cars[0].armas[0].t.rz = angMaxCanhao;
		}

	}
	else if(btn_r2){
		if(arena.cars[0].armas[0].t.rz - inc >= -angMaxCanhao){
			arena.cars[0].armas[0].t.rz -= inc;
		}else{
			arena.cars[0].armas[0].t.rz = -angMaxCanhao;
		}
	}
	else if(btn_r1){
		arena.cars[0].tiros.push_back(novoTiro());
	}
}
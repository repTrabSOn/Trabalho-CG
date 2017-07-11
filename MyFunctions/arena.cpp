#include "globais.h"

class CheckPoint{
public:
	bool flag_ativo, flag_direcao;
	circ c;
	int num_tiros;
	float vel;
	void movimento(void);

};

void CheckPoint::movimento(void){
	float d_x = vel * cos((c.t.rz) * M_PI / 180.0);
	float d_y = vel * sin((c.t.rz) * M_PI / 180.0);

	float d = sqrt(pow(c.t.tx + d_x - x_centro, 2) + pow(c.t.ty + d_y - y_centro, 2));
	if(d + c.raio > raio_maior || d - c.raio < raio_menor){
		flag_direcao = !flag_direcao;
	}

	//estou indo para direcao do centro
	if(!flag_direcao){
		d_x *= -1;
		d_y *= -1;
	}

	c.t.tx += d_x;
	c.t.ty += d_y;
}
 

class Enemy{
public:
	bool flag_vivo, flag_direcao;
	int id, vida;
	float vel, vel_tiro, freq_tiro;
	circ c;
	std::vector<circ> tiros;
	void limpa_tiros(void);
	void movimento(void);
	void atira(int x, int y);
	//void especial(void);
};

void Enemy::atira(int x, int y){
	circ novo;
	//int ang_aux = rand()%361;
	float d = sqrt(pow(c.t.tx - x, 2) + pow(c.t.ty - y, 2));
	float ang_aux = acos((c.t.ty - y)/d)*180.0/M_PI;

	if(c.t.tx < x){
		ang_aux = 180 - ang_aux;
		if(c.t.ty < y){
			ang_aux += 90;
		}
	}else{
		if(c.t.ty < y){
			ang_aux = 360 - c.t.rz;
		}
	}		

	novo.t.tx = c.t.tx + cos(ang_aux * M_PI / 180.0) * c.raio;
	novo.t.ty = c.t.ty + sin(ang_aux * M_PI / 180.0) * c.raio;

	novo.raio = 3 * 1.0/(float)ESCALA;
	novo.t.rz = ang_aux;
	novo.c.r = 1.0; novo.c.g = 1.0; novo.c.b = 0.0;
	tiros.push_back(novo);
}


void Enemy::limpa_tiros(void){
	if(!tiros.empty()){
		for(int i = 0; i < tiros.size(); i++){
			float d = sqrt(pow(tiros[i].t.tx - x_centro, 2) + pow(tiros[i].t.ty - y_centro, 2));
			if(d > raio_maior || d < raio_menor){
				tiros[i] = tiros[tiros.size() - 1];
				tiros.pop_back();
				i--;
			}
		}
	}
}

void Enemy::movimento(void){
	float d_x = vel * cos((c.t.rz) * M_PI / 180.0);
	float d_y = vel * sin((c.t.rz) * M_PI / 180.0);

	float d = sqrt(pow(c.t.tx + d_x - x_centro, 2) + pow(c.t.ty + d_y - y_centro, 2));
	if(d + c.raio > raio_maior || d - c.raio < raio_menor){
		flag_direcao = !flag_direcao;
	}

	//estou indo para direcao do centro
	if(!flag_direcao){
		d_x *= -1;
		d_y *= -1;
	}

	c.t.tx += d_x;
	c.t.ty += d_y;
}



class Carro{
public:
	bool flag_vivo;
	int vida, qtd_tiros;
	std::vector<rect> rodas_articuldas;
	std::vector<rect> rodas_estaticas;
	std::vector<rect> partes_estaticas;
	std::vector<rect> armas;
	std::vector<circ> details;
	std::vector<circ> tiros;
	std::vector<circ> fogs;
	std::vector<sensor> sensores;
	trans t_carro;
	float vel_carro, vel_tiro, range_tiro;
	void acelerar(unsigned char key, bool turbo, float rot, float l_inf, float l_sup, std::vector<Enemy> inimigos);
	void limpa_tiros(void);
	bool colisao(float new_x, float new_y, float rot, float l_inf, float l_sup, std::vector<Enemy> inimigos);
	float dist(float x1, float y1, float x2, float y2);
	bool colisao_tiros();
	bool coletar_chekpoints(std::vector<CheckPoint> * c);
};

void Carro::acelerar(unsigned char key, bool turbo, float rot, float l_inf, float l_sup, std::vector<Enemy> inimigos){
	float desloc_x = vel_carro * cos((t_carro.rz + ANG_REF) * M_PI / 180.0);
	float desloc_y = vel_carro * sin((t_carro.rz + ANG_REF) * M_PI / 180.0);


	if(key == 'w'){//para frente
		if(turbo){
			desloc_x *= VEL_TURBO;
			desloc_y *= VEL_TURBO;
		}
		if(!colisao(desloc_x, desloc_y, rot, l_inf, l_sup, inimigos)){
			t_carro.tx += desloc_x;
			t_carro.ty += desloc_y;
			t_carro.rz += rot;
			for(int i = 0; i < sensores.size(); i++){
				sensores[i].x += desloc_x;
				sensores[i].y += desloc_y;
			}
		}
	}
	else if(key == 's'){//ré
		if(!colisao(-desloc_x, -desloc_y, rot, l_inf, l_sup, inimigos)){
			t_carro.tx -= desloc_x;
			t_carro.ty -= desloc_y;
			t_carro.rz += rot;
			for(int i = 0; i < sensores.size(); i++){
				sensores[i].x -= desloc_x;
				sensores[i].y -= desloc_y;
			}	
		}
	}
}

void Carro::limpa_tiros(void){
	if(!tiros.empty()){
		for(int i = 0; i < tiros.size(); i++){
			float d = sqrt(pow(tiros[i].t.tx - x_centro, 2) + pow(tiros[i].t.ty - y_centro, 2));
			if(d > raio_maior || d < raio_menor){
				tiros[i] = tiros[tiros.size() - 1];
				tiros.pop_back();
				i--;
			}

		}
	}
}

float Carro::dist(float x1, float y1, float x2, float y2){
	float dx = x1 - x2;
	float dy = y1 - y2;
	return sqrt(dx*dx + dy*dy);
}


bool Carro::colisao(float new_x, float new_y, float rot, float l_inf, float l_sup, std::vector<Enemy> inimigos){
	//colisao com os limites da arena
	std::vector<sensor> s_aux = sensores;

	if(rot != 0){
		float c_ang = cos((rot) * M_PI / 180.0);
		float s_ang = sin((rot) * M_PI / 180.0);


		for(int i = 0; i < s_aux.size(); i++){
				float d_x = s_aux[i].x - t_carro.tx;
				float d_y = s_aux[i].y - t_carro.ty + dist_centro_rot_chassi;
				float aux_dx = d_x;
				float aux_dy = d_y;

				d_x = d_x*c_ang + d_y*-s_ang;
				d_y = aux_dx*s_ang + aux_dy*c_ang;

				d_x += t_carro.tx;
				d_y += t_carro.ty;

				s_aux[i].x = d_x;
				s_aux[i].y = d_y - dist_centro_rot_chassi;

		}
	}

	float r = (c_chassi/2.0 + c_canhao/2.0 + dist_centro_rot_canhao)* ESCALA;

	bool flagResp = false;

	for(int j = 0; j < s_aux.size(); j++){
		for(int i = 0; i < inimigos.size(); i++){
			float d = dist(s_aux[j].x + new_x, s_aux[j].y + new_y, inimigos[i].c.t.tx, inimigos[i].c.t.ty);
			if(inimigos[i].c.tipo == TIPO_PISTA){
				if(d - r < l_inf || d + r > l_sup){
					flagResp = true;
				}

			}
			else if(inimigos[i].flag_vivo && inimigos[i].c.tipo == TIPO_INIMIGO){
				if(d - r < inimigos[i].c.raio){
					flagResp  = true;
					flag_vivo = false;
					if (vida == 0)
						gameOver = true;
				}
			}
		}
	}

	if(!flagResp){
		sensores = s_aux;
	}

	return flagResp;
}

bool Carro::coletar_chekpoints(std::vector<CheckPoint> * c){
	if(!(*c).empty()){
		float r = (c_chassi/2.0 + c_canhao/2.0 + dist_centro_rot_canhao)* ESCALA;
		for(int i = 0; i < (*c).size(); i++){
			if((*c)[i].flag_ativo){
				float d = sqrt(pow((*c)[i].c.t.tx - t_carro.tx, 2) + pow((*c)[i].c.t.ty - t_carro.ty, 2));
				if(d - r < (*c)[i].c.raio){
					(*c)[i].flag_ativo = false;
					qtd_tiros += (*c)[i].num_tiros;
				}
			}
		}
	}

}





class Arena{
public:
	//std::vector<circ> circs;
	std::vector<Enemy> enemys;
	std::vector<rect> rects;
	std::vector<Carro> cars;
	std::vector<CheckPoint> checks;
	trans t_arena;
};


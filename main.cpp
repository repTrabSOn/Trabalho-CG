#include "MyFunctions/globais.h"
#include "MyFunctions/openGlFunctions.cpp"


int main(int argc, char * argv[]){
    srand(time(NULL));

	load_arena(argv[1]);

    printf("\n+------------------------------------+\n");
    printf("\n+            TRAB 3 CG               +\n");
    printf("\n+   Caio Vianna, David Morosini      +\n");
    printf("\n+------------------------------------+\n");
    printf("\n+  w, a, s, d -> controlam o carro   +\n");
    printf("\n+  space -> nitro                    +\n");
    printf("\n+  '-' e '=' -> habilitam hitbox     +\n");
    printf("\n+  j - alterna (teclado ou joystick) +\n");
    printf("\n+------------------------------------+\n");
    printf("\n\nCaso tenha um Joystick USB conectado, pressione (J/j) para ativalo.\n");

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(raio_maior * 2, raio_maior * 2);
    glutInitWindowPosition(100,100);
    glutCreateWindow("DC Studios - Trabalho 3 de CG");

    inicializa();
    glutDisplayFunc(desenha);
    glutKeyboardFunc(teclaPress);
    glutKeyboardUpFunc(teclaUp);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
    glutTimerFunc(1500, limpa_fogs, 1);
    glutTimerFunc(500, cria_tiros_inimigos, 1);
    glutTimerFunc(10000, reseta_checkpoints, 1);
    glutTimerFunc(10000, restaura_checkpoints, 1);


    //joystick
    glutJoystickFunc(joystick, 25);

    glutMainLoop();

	return 0;
}
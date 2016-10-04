#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <string.h>
#include <Windows.h>

#define stage_width  15
#define stage_height 10
#define ascii_0 48
#define null -1
#define s_size 1.0	/*1�v�f�̑傫��*/
#define px -7.5		/*�����v�f�̈ʒu*/
#define py -4.5

/*�Q�[���̏��*/
enum{
	NORMAL,
	FINISH,
	DROP
};

typedef struct{
	int color;
	int erase;
	int old;
}block;

block stage[10][15];	/*�e�u���b�N�̃f�[�^*/
int block_pattern = 3;	/*�u���b�N�̎��*/
int score;				/*�X�R�A*/
int erase_blocks = 0;	/*�����v�f�̐�*/
int sx = 0, sy = 0;		/*���I��ł���v�f*/
int condition;	/*�Q�[����ԃt���O*/
static double size_rate = 1.0;	/*�������A�j���[�V�����p�k������*/
void *font = GLUT_BITMAP_TIMES_ROMAN_24;	/*�t�H���g*/


/*�F�f�[�^*/
static GLfloat color[][4] = {
	{0.4, 0.4, 1.0, 1.0},	/*���F*/
	{0.5, 0.7, 0.2, 1.0},	/*�ΐF*/
	{1.0, 0.5, 1.0, 1.0},	/*�s���N�F*/
	{1.0, 0.6, 0.0, 1.0},	/*�I�����W*/
	{0.2, 0.1, 0.0, 1.0},	/*���F*/
	{0.5, 0.5, 0.5, 1.0},	/*�D�F*/
};

/*�֐��v���g�^�C�v*/
void game_start(void);
void display(void);
void draw_stage(void);
void myBox(double, double);
void draw_string(char, void *, int, float ,float ,float);
void draw_score(void);
void draw_rank(void);
void idle(void);
void resize(int, int);
void erase_area(int, int);
void put_null(void);
void erase_matrix(void);
void condition_judge(void);
void gameover(void);
void keyboard(unsigned char, int, int);
void specialkeydown(int, int, int);
void init(void);
int main(int, char *[]);


/*�Q�[���X�^�[�g�i�X�e�[�W�����j*/
void game_start(void){
	int i,j;

	score = 0;
	condition = NORMAL;

	/*�����ŃX�e�[�W�𐶐�*/
	for(i=0;i<stage_height;i++){
		for(j=0;j<stage_width;j++){
			stage[i][j].color = (int)rand() % block_pattern;
		}
	}

	/*BGM�Đ�*/
	PlaySound("../samegame/Sounds/bgm",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
}

/*�����̂��o��*/
void myBox(double x, double y)
{
  GLdouble vertex[][3] = {
    { x-s_size*0.5, y-s_size*0.5, -0.20 },
    { x+s_size*0.5, y-s_size*0.5, -0.20 },
    { x+s_size*0.5, y+s_size*0.5, -0.20 },
    { x-s_size*0.5, y+s_size*0.5, -0.20 },
    { x-s_size*0.5, y-s_size*0.5, -0.18 },
    { x+s_size*0.5, y-s_size*0.5, -0.18 },
    { x+s_size*0.5, y+s_size*0.5, -0.18 },
    { x-s_size*0.5, y+s_size*0.5, -0.18 }
  };

  const static int face[][4] = {
    { 0, 1, 2, 3 },
    { 1, 5, 6, 2 },
    { 5, 4, 7, 6 },
    { 4, 0, 3, 7 },
    { 4, 5, 1, 0 },
    { 3, 2, 6, 7 }
  };

  const static GLdouble normal[][3] = {
    { 0.0, 0.0,-1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    {-1.0, 0.0, 0.0 },
    { 0.0,-1.0, 0.0 },
    { 0.0, 1.0, 0.0 }
  };

  int i, j;

  glMaterialfv(GL_FRONT, GL_DIFFUSE, color[5]);

  glBegin(GL_QUADS);
  for (j = 0; j < 6; ++j) {
    glNormal3dv(normal[j]);
    for (i = 4; --i >= 0;) {
      glVertex3dv(vertex[face[j][i]]);
    }
  }
  glEnd();
}

void idle(void){
  glutPostRedisplay();
}

/*�X�e�[�W�`��p*/
void draw_stage(void){
	int i, j;

	/*�k��������ς���*/
	if(condition == DROP){
		size_rate = size_rate - 0.2;
	}

	for(i=0;i<stage_height;i++){
		for(j=0;j<stage_width;j++){
			glPushMatrix();
			glTranslated(px+s_size*j, py+s_size*i, 0.0);
			if(stage[i][j].color != null){
				glMaterialfv(GL_FRONT, GL_DIFFUSE, color[stage[i][j].color]);
				if(stage[i][j].color < 3){
					//�e�B�[�|�b�g�`��
					if(stage[i][j].erase == 1){
						glRotated(360*size_rate, 0.0, 1.0, 0.0);
						glutSolidTeapot(0.3*size_rate);
					}else{
						glRotated(0.0, 0.0, 1.0, 0.0);
						glutSolidTeapot(0.3);
					}
				} else {
					//�h�[�i�c�`��
					glRotated(0.0, 0.0, 0.0, 0.0);
					if(stage[i][j].erase == 1){
						glRotated(360*size_rate, 0.0, 0.0, 0.0);
						glutSolidTorus(0.15*size_rate, 0.21*size_rate, 20, 20);
					}else{
						glRotated(0.0, 0.0, 0.0, 0.0);
						glutSolidTorus(0.15, 0.21, 20, 20);
					}
				}
			}
			glPopMatrix();
		}
	}
}

/*������`��p*/
void draw_string(char *str,void *font, int length, float x,float y,float z){
	int i = 0;
	glRasterPos3f(x,y,z);
	while(i < length){
		glutBitmapCharacter(font, *(str+i));
		++i;
	}
}

/*�X�R�A�\���p*/
void draw_score(void){
	char str[15];
	strcpy(str, "score ");
	itoa(score, str + 6, 10);
	draw_string(str, font, 11, 4.0, -5.5, 2.0);
}

/*�̍��\���p*/
void draw_rank(void){
	char str[35];

	strcpy(str, "your rank:");
	if(score >= 3500){
		strcpy(str+10, "cosmic teatime");
	} else if (score >= 2500){
		strcpy(str+10, "unrealistic teatime");
	} else if (score >= 1500){
		strcpy(str+10, "billionaire's teatime");
	} else if (score >= 1000){
		strcpy(str+10, "rich teatime");
	} else if (score >= 700){
		strcpy(str+10, "usual teatime");
	} else if (score >= 500){
		strcpy(str+10, "economic teatime");
	} else if (score >= 300){
		strcpy(str+10, "poor teatime");
	} else if (score >= 100){
		strcpy(str+10, "worst teatime");
	} else {
		strcpy(str+10, "a hell on the earth");
	}
	draw_string(str, font, 32, -3.0, 1.0, 2.0);
}

/*��ʕ\��*/
void display(void){
	const static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /*�����̈ʒu*/
	char str[35];

	glLoadIdentity();

	/* ��ʃN���A */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* ���f���r���[�ϊ��s��̏����� */
	glLoadIdentity();

	/* �����̈ʒu��ݒ� */
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* ���_�̈ړ��i���̂̕������Ɉڂ��j*/
	glTranslated(0.0, 0.0, -25.0);
	glRotated(0.0, 1.0, 0.0, 0.0);

	/*�X�e�[�W�\��*/
	draw_stage();

	/*���݈ʒu�����������`��\��*/
	myBox(px+s_size*sx, py+s_size*sy);

	/*�X�R�A��\��*/
	draw_score();

	/*�Q�[���N���A��͏̍����\��*/
	if(condition == FINISH){
		draw_rank();
		strcpy(str, "push 'q' to quit");
		draw_string(str, font, 17, -3.0, 0.0, 2.0);
		strcpy(str, "push '3' or '4' or '5'");
		draw_string(str, font, 23, -3.0, -1.0, 2.0);
		strcpy(str, "to select difficulty");
		draw_string(str, font, 21, -3.0, -2.0, 2.0);
	} else {
		strcpy(str, "push 'f' to erase blocks");
		draw_string(str, font, 25, -8.0, -5.5, 2.0);
	}

	glFlush();

}

void resize(int w, int h)
{
	/* �E�B���h�E�S�̂��r���[�|�[�g�ɂ��� */
	glViewport(0, 0, w, h);

	/* �����ϊ��s��̎w�� */
	glMatrixMode(GL_PROJECTION);

	/* �����ϊ��s��̏����� */
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	/* ���f���r���[�ϊ��s��̎w�� */
	glMatrixMode(GL_MODELVIEW);
}

/*�����G���A�𔻒�*/
void erase_area(int i, int j){
	stage[i][j].erase = 1;
	erase_blocks++;
	if(stage[i][j].color == stage[i][j-1].color && stage[i][j-1].erase != 1 && j != 0){
		erase_area(i,j-1);
	}
	if(stage[i][j].color == stage[i-1][j].color && stage[i-1][j].erase != 1 && i != 0){
		erase_area(i-1,j);
	}
	if(stage[i][j].color == stage[i][j+1].color && stage[i][j+1].erase != 1 && j != stage_width - 1){
		erase_area(i,j+1);
	}
	if(stage[i][j].color == stage[i+1][j].color && stage[i+1][j].erase != 1 && i != stage_height - 1){
		erase_area(i+1,j);
	}
}

/*�����u���b�N��null�ɂ���*/
void put_null(void){
	int i,j;

	for(i=0;i<stage_height;i++){
		for(j=0;j<stage_width;j++){
			if(stage[i][j].erase == 1){
				stage[i][j].color = null;
			}
		}
	}
}



/*�s���Ō��Ԃ��l�߂�*/
void erase_matrix(void){
	int i, j, k, l;

	/*���ɋl�߂�*/
	for(j=0;j<stage_width;j++){/*������E�֑{��*/
		for(i=0;i<stage_height;i++){/*�������֑{��*/
			if(stage[i][j].color == null){/*���Ԃ������Ă��邩*/
				for(k=i+1;k<stage_height;k++){/*i+1�����֑{��*/
					if(stage[k][j].color != null){/*�u���b�N����������*/
						/*�u���b�N�����ɋl�߂�*/
						stage[i][j].color = stage[k][j].color;
						stage[k][j].color = null;
						break;
					}
				}
			}
		}
	}

	/*���ɋl�߂�*/
	for(j=0;j<stage_width;j++){/*������E�֑{��*/
		if(stage[0][j].color == null){/*���Ԃ��󂢂Ă��邩*/
			for(k=j+1;k<stage_width;k++){/*j+1����E�֑{��*/
				if(stage[0][k].color != null){/*�u���b�N����������*/
					/*�u���b�N�����ɋl�߂�*/
					for(l=0;l<stage_height;l++){/*�c�̗���܂邲�Ə���*/
						if(stage[l][k].color == null)	break;	/*���点��u���b�N���Ȃ���Δ�����*/
						stage[l][j].color = stage[l][k].color;
						stage[l][k].color = null;
					}
					break;
				}
			}
		}
	}
}

/*�Q�[���I������*/
void gameover(void){
	int i,j;
	int bonus = 10;

	condition = FINISH;	//�Q�[���I���t���O�𗧂Ă�
	
	/*�{�[�i�X���Z����*/
	for(i=0;i<stage_height;i++){
		for(j=0;j<stage_width;j++){
			if(stage[i][j].color != null) bonus--;	//�u���b�N���c��Ύc��قǃ{�[�i�X�_����
		}
	}
	if(bonus == 10){
		score += 1000;	//�S����������1000�_�{�[�i�X
	}else if(bonus >= 1){
		score += bonus * bonus;	//10�����Ȃ�c���Ă��{�[�i�X
	}

	/*�Q�[���I�����ʉ�*/
	PlaySound("../samegame/Sounds/finish",NULL,SND_FILENAME | SND_ASYNC);

	/*�L�[�����t�E�A�j���[�V�����J�n*/
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeydown);
	glutIdleFunc(idle);
}

/*�Q�[���I������*/
void finish_judge(void){
	int i, j;

	for(i=0;i<stage_height;i++){
		for(j=0;j<stage_width;j++){
			if(stage[i][j].color != null){
				if(j != 0 && stage[i][j].color == stage[i][j-1].color){
					return;
				}
				if(i != 0 && stage[i][j].color == stage[i-1][j].color){
					return;
				}
				if(j != stage_width - 1 && stage[i][j].color == stage[i][j+1].color){
					return;
				}
				if(i != stage_height - 1 && stage[i][j].color == stage[i+1][j].color){
					return;
				}
			}
		}
	}
	gameover();
}

void keyboard(unsigned char key, int x, int y){
	int i, j, count;
	clock_t start, end;

	if( key == 'f' && stage[sy][sx].color != null){
		/*�L�[�����t�E�A�j���[�V�������~*/
		glutKeyboardFunc(NULL);
		glutSpecialFunc(NULL);
		glutIdleFunc(NULL);

		/*�u���b�N�����֘A�̕ϐ���������*/
		erase_blocks = 0;
		size_rate = 1.0;
		count = 0;

		/*�u���b�N��������*/
		erase_area(sy,sx);	/*�����G���A�̔���*/
		if(erase_blocks >= 2){
			/*���̏�Ԃ��L�����Ă���
			���łɍ��̒n�_�ł̃u���b�N�̐��𐔂���*/
			for(i=0;i<stage_height;i++){
				for(j=0;j<stage_width;j++){
					stage[i][j].old = stage[i][j].color;
					if(stage[i][j].color)	count++;
				}
			}

			/*�����u���b�N���������Ȃ��ď�����*/
			condition = DROP;
			for(i=0;i<5;i++){
				start = clock();
				display();
				end = clock();
				/*display�ɂ����������ԂɊ�Â��đ҂����Ԃ�����*/
				if((end-start)/CLOCKS_PER_SEC < 200)
					Sleep(200-1000*(end-start)/CLOCKS_PER_SEC);
			}

			put_null();	/*����*/
			erase_matrix();	/*�X�e�[�W�̃f�[�^��ς��ău���b�N���l�߂�*/
			score += (int)pow((double)(erase_blocks - 2), 2);	/*�X�R�A�𑝂₷*/

			condition = NORMAL;
		}

		/*erase�z���������*/
		for(i=0;i<stage_height;i++){
			for(j=0;j<stage_width;j++){
				stage[i][j].erase = 0;
			}
		}

		/*�Q�[���I������*/
		finish_judge();

		/*�L�[�����t�E�A�j���[�V�����ĊJ*/
		glutKeyboardFunc(keyboard);
		glutSpecialFunc(specialkeydown);
		glutIdleFunc(idle);
	}

	if( key == 'q' ){
		exit(0);
	}

	if( key == 'b'){
		for(i=0;i<10;i++){
			for(j=0;j<15;j++){
				stage[i][j].color = stage[i][j].old;
			}
		}
	}

	if( key == '3'){
		block_pattern = 3;
		game_start();
	}

	if( key == '4'){
		block_pattern = 4;
		game_start();
	}

	if( key == '5'){
		block_pattern = 5;
		game_start();
	}
}

void specialkeydown(int key, int x, int y){
	if( key == GLUT_KEY_UP ){
		if(sy == stage_height - 1){
			sy = 0;
		}else{
			sy++;
		}
     }

	if( key == GLUT_KEY_DOWN){
		if(sy == 0){
			sy = stage_height - 1;
		}else{
			sy--;
		}
	}

	if( key == GLUT_KEY_LEFT ){
		if(sx == 0){
			sx = stage_width - 1;
		}else{
			sx--;
		}
	}

	if( key == GLUT_KEY_RIGHT ){
		if(sx == stage_width - 1){
			sx = 0;
		}else{
			sx++;
		}
	}

}

void init(void){
	/* �����ݒ� */
	glClearColor(0.95, 1.0, 0.4, 1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

int main(int argc, char *argv[]){
	srand((unsigned int)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(960, 720);
	glutCreateWindow("Teatime Samegame");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeydown);
	glutIdleFunc(idle);
	init();
	game_start();
	glutMainLoop();
	return 0;
}
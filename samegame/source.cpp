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
#define s_size 1.0	/*1要素の大きさ*/
#define px -7.5		/*左下要素の位置*/
#define py -4.5

/*ゲームの状態*/
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

block stage[10][15];	/*各ブロックのデータ*/
int block_pattern = 3;	/*ブロックの種類*/
int score;				/*スコア*/
int erase_blocks = 0;	/*消す要素の数*/
int sx = 0, sy = 0;		/*今選んでいる要素*/
int condition;	/*ゲーム状態フラグ*/
static double size_rate = 1.0;	/*消去時アニメーション用縮小割合*/
void *font = GLUT_BITMAP_TIMES_ROMAN_24;	/*フォント*/


/*色データ*/
static GLfloat color[][4] = {
	{0.4, 0.4, 1.0, 1.0},	/*水色*/
	{0.5, 0.7, 0.2, 1.0},	/*緑色*/
	{1.0, 0.5, 1.0, 1.0},	/*ピンク色*/
	{1.0, 0.6, 0.0, 1.0},	/*オレンジ*/
	{0.2, 0.1, 0.0, 1.0},	/*茶色*/
	{0.5, 0.5, 0.5, 1.0},	/*灰色*/
};

/*関数プロトタイプ*/
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


/*ゲームスタート（ステージ生成）*/
void game_start(void){
	int i,j;

	score = 0;
	condition = NORMAL;

	/*乱数でステージを生成*/
	for(i=0;i<stage_height;i++){
		for(j=0;j<stage_width;j++){
			stage[i][j].color = (int)rand() % block_pattern;
		}
	}

	/*BGM再生*/
	PlaySound("../samegame/Sounds/bgm",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
}

/*直方体を出す*/
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

/*ステージ描画用*/
void draw_stage(void){
	int i, j;

	/*縮小割合を変える*/
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
					//ティーポット描画
					if(stage[i][j].erase == 1){
						glRotated(360*size_rate, 0.0, 1.0, 0.0);
						glutSolidTeapot(0.3*size_rate);
					}else{
						glRotated(0.0, 0.0, 1.0, 0.0);
						glutSolidTeapot(0.3);
					}
				} else {
					//ドーナツ描画
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

/*文字列描画用*/
void draw_string(char *str,void *font, int length, float x,float y,float z){
	int i = 0;
	glRasterPos3f(x,y,z);
	while(i < length){
		glutBitmapCharacter(font, *(str+i));
		++i;
	}
}

/*スコア表示用*/
void draw_score(void){
	char str[15];
	strcpy(str, "score ");
	itoa(score, str + 6, 10);
	draw_string(str, font, 11, 4.0, -5.5, 2.0);
}

/*称号表示用*/
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

/*画面表示*/
void display(void){
	const static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /*光源の位置*/
	char str[35];

	glLoadIdentity();

	/* 画面クリア */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* モデルビュー変換行列の初期化 */
	glLoadIdentity();

	/* 光源の位置を設定 */
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* 視点の移動（物体の方を奥に移す）*/
	glTranslated(0.0, 0.0, -25.0);
	glRotated(0.0, 1.0, 0.0, 0.0);

	/*ステージ表示*/
	draw_stage();

	/*現在位置を示す正方形を表示*/
	myBox(px+s_size*sx, py+s_size*sy);

	/*スコアを表示*/
	draw_score();

	/*ゲームクリア後は称号も表示*/
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
	/* ウィンドウ全体をビューポートにする */
	glViewport(0, 0, w, h);

	/* 透視変換行列の指定 */
	glMatrixMode(GL_PROJECTION);

	/* 透視変換行列の初期化 */
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	/* モデルビュー変換行列の指定 */
	glMatrixMode(GL_MODELVIEW);
}

/*消すエリアを判定*/
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

/*消すブロックをnullにする*/
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



/*行列上で隙間を詰める*/
void erase_matrix(void){
	int i, j, k, l;

	/*下に詰める*/
	for(j=0;j<stage_width;j++){/*左から右へ捜査*/
		for(i=0;i<stage_height;i++){/*下から上へ捜査*/
			if(stage[i][j].color == null){/*隙間があいているか*/
				for(k=i+1;k<stage_height;k++){/*i+1から上へ捜査*/
					if(stage[k][j].color != null){/*ブロックがあったら*/
						/*ブロックを下に詰める*/
						stage[i][j].color = stage[k][j].color;
						stage[k][j].color = null;
						break;
					}
				}
			}
		}
	}

	/*左に詰める*/
	for(j=0;j<stage_width;j++){/*左から右へ捜査*/
		if(stage[0][j].color == null){/*隙間が空いているか*/
			for(k=j+1;k<stage_width;k++){/*j+1から右へ捜査*/
				if(stage[0][k].color != null){/*ブロックがあったら*/
					/*ブロックを左に詰める*/
					for(l=0;l<stage_height;l++){/*縦の列をまるごと処理*/
						if(stage[l][k].color == null)	break;	/*ずらせるブロックがなければ抜ける*/
						stage[l][j].color = stage[l][k].color;
						stage[l][k].color = null;
					}
					break;
				}
			}
		}
	}
}

/*ゲーム終了処理*/
void gameover(void){
	int i,j;
	int bonus = 10;

	condition = FINISH;	//ゲーム終了フラグを立てる
	
	/*ボーナス加算処理*/
	for(i=0;i<stage_height;i++){
		for(j=0;j<stage_width;j++){
			if(stage[i][j].color != null) bonus--;	//ブロックが残れば残るほどボーナス点減少
		}
	}
	if(bonus == 10){
		score += 1000;	//全部消したら1000点ボーナス
	}else if(bonus >= 1){
		score += bonus * bonus;	//10個未満なら残ってもボーナス
	}

	/*ゲーム終了効果音*/
	PlaySound("../samegame/Sounds/finish",NULL,SND_FILENAME | SND_ASYNC);

	/*キー操作受付・アニメーション開始*/
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeydown);
	glutIdleFunc(idle);
}

/*ゲーム終了判定*/
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
		/*キー操作受付・アニメーション中止*/
		glutKeyboardFunc(NULL);
		glutSpecialFunc(NULL);
		glutIdleFunc(NULL);

		/*ブロック消し関連の変数を初期化*/
		erase_blocks = 0;
		size_rate = 1.0;
		count = 0;

		/*ブロック消し処理*/
		erase_area(sy,sx);	/*消すエリアの判定*/
		if(erase_blocks >= 2){
			/*今の状態を記憶しておく
			ついでに今の地点でのブロックの数を数える*/
			for(i=0;i<stage_height;i++){
				for(j=0;j<stage_width;j++){
					stage[i][j].old = stage[i][j].color;
					if(stage[i][j].color)	count++;
				}
			}

			/*消すブロックが小さくなって消える*/
			condition = DROP;
			for(i=0;i<5;i++){
				start = clock();
				display();
				end = clock();
				/*displayにかかった時間に基づいて待ち時間を決定*/
				if((end-start)/CLOCKS_PER_SEC < 200)
					Sleep(200-1000*(end-start)/CLOCKS_PER_SEC);
			}

			put_null();	/*消す*/
			erase_matrix();	/*ステージのデータを変えてブロックを詰める*/
			score += (int)pow((double)(erase_blocks - 2), 2);	/*スコアを増やす*/

			condition = NORMAL;
		}

		/*erase配列を初期化*/
		for(i=0;i<stage_height;i++){
			for(j=0;j<stage_width;j++){
				stage[i][j].erase = 0;
			}
		}

		/*ゲーム終了判定*/
		finish_judge();

		/*キー操作受付・アニメーション再開*/
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
	/* 初期設定 */
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
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <unistd.h>
#define SQ_SIZE 4
#define CB_SIZE 8
#define PHI_SPD 0.1
#define SHAPE_SCALE 8

typedef struct{
	float x;
	float y;
}point;

typedef struct{
	float x;
	float y;
	float z;
}point_3D;



typedef struct{
	point points[SQ_SIZE];
}square;


typedef struct{
	point_3D points[CB_SIZE];
}cube;


void rotatePoint_2D_XY(point *p,float *phi){
	float tempx, tempy;
	tempx = (p->x * cos(*phi)) - (p->y*sin(*phi));
	tempy = (p->x * sin(*phi)) + (p->y*cos(*phi));
	p->x=tempx;
	p->y=tempy;

}


void rotateSquare_2D_XY(square *sq, float *phi){
	for(int i=0; i<SQ_SIZE; i++){
		rotatePoint_2D_XY(&(sq->points[i]), phi);
	}
}




//indexes are from 0-SIZE
//input will range from -2 to 2
//so -2->0 ; 2->SIZE
int normalize(float value, int *SIZE) {
    // Define the input range [-2, 2]
    float min_input = -2.0;
    float max_input = 2.0;

    // Normalize value to the range [0, SIZE]
	int normalized_value = floor((value - min_input) / (max_input - min_input) * (*SIZE));

    // Ensure the result is within the bounds
    //if (normalized_value < 0) return 0;
    //if (normalized_value > SIZE) return SIZE;
    //printf("%d\n", normalized_value);
    if (normalized_value < 0) normalized_value=0;
    if (normalized_value > *SIZE) normalized_value=*SIZE;
    return normalized_value;
}




void makeLineB_H(int x0, int y0, int x1, int y1, int *SIZE, char (*cords)[*SIZE]){
    int dx, dy, p, x, y;
	
	if(x0>x1){
		int tx,ty;
		tx=x0;
		x0=x1;
		x1=tx;
		ty=y0;
		y0=y1;
		y1=ty;
	}

	dx=x1-x0;
	dy=y1-y0;

	int dir = (dy<0)?-1:1;
	dy = dy*dir;

	x=x0;
	y=y0;

	p=2*dy-dx;

	while(x<x1){
		cords[y][x]='.';
		if(p>=0)
		{
			y=y+dir;
			p=p+2*dy-2*dx;
		}
		else
		{
			p=p+2*dy;
		}
		x=x+1;
	}
}
void makeLineB_V(int x0, int y0, int x1, int y1, int *SIZE, char (*cords)[*SIZE]){
    int dx, dy, p, x, y;
	
	if(y0>y1){
		int tx,ty;
		tx=x0;
		x0=x1;
		x1=tx;
		ty=y0;
		y0=y1;
		y1=ty;
	}

	dx=x1-x0;
	dy=y1-y0;

	int dir = (dx<0)?-1:1;
	dx=dx*dir;

	x=x0;
	y=y0;

	p=2*dx-dy;

	while(y<y1){
		cords[y][x]='.';
		if(p>=0)
		{
			x=x+dir;
			p=p+2*dx-2*dy;
		}
		else
		{
			p=p+2*dx;
		}
		y=y+1;
	}
}

void makeLineB(int x0, int y0, int x1, int y1, int *SIZE, char (*cords)[*SIZE]){
	if(abs(x1-x0)>abs(y1-y0)){
		makeLineB_H(x0,y0,x1,y1,SIZE,cords);
	}else{
		makeLineB_V(x0,y0,x1,y1,SIZE,cords);
	}
}

void makeLineB_point(point_3D *a, point_3D *b, int *SIZE, char (*cords)[*SIZE]){

	float x0,x1,y0,y1;
	x0 = normalize(a->x,SIZE);
	x1 = normalize(b->x,SIZE);
	y0 = normalize(a->y,SIZE);
	y1 = normalize(b->y,SIZE);


	if(abs(x1-x0)>abs(y1-y0)){
		makeLineB_H(x0,y0,x1,y1,SIZE,cords);
	}else{
		makeLineB_V(x0,y0,x1,y1,SIZE,cords);
	}
}

void matmul3x3(cube *cb, float m[3][3]){
	for (int i=0;i<CB_SIZE;i++) {
		float x = cb->points[i].x;
		float y = cb->points[i].y;
		float z = cb->points[i].z;
      		float tempx = m[0][0]*x + m[0][1]*y + m[0][2]*z;
      		float tempy = m[1][0]*x + m[1][1]*y + m[1][2]*z;
      		float tempz = m[2][0]*x + m[2][1]*y + m[2][2]*z;
		cb->points[i].x = tempx;
		cb->points[i].y = tempy;
		cb->points[i].z = tempz;

	}	
}

void connectCube(cube *cb, int *SIZE, char (*cords)[*SIZE]){
	makeLineB_point(&(cb->points[0]), &(cb->points[1]),SIZE,cords);
	makeLineB_point(&(cb->points[1]), &(cb->points[2]),SIZE,cords);
	makeLineB_point(&(cb->points[2]), &(cb->points[3]),SIZE,cords);
	makeLineB_point(&(cb->points[3]), &(cb->points[0]),SIZE,cords);
	makeLineB_point(&(cb->points[4]), &(cb->points[5]),SIZE,cords);
	makeLineB_point(&(cb->points[5]), &(cb->points[6]),SIZE,cords);
	makeLineB_point(&(cb->points[6]), &(cb->points[7]),SIZE,cords);
	makeLineB_point(&(cb->points[7]), &(cb->points[4]),SIZE,cords);
	makeLineB_point(&(cb->points[0]), &(cb->points[4]),SIZE,cords);
	makeLineB_point(&(cb->points[1]), &(cb->points[5]),SIZE,cords);
	makeLineB_point(&(cb->points[2]), &(cb->points[6]),SIZE,cords);
	makeLineB_point(&(cb->points[3]), &(cb->points[7]),SIZE,cords);
}

int main(){
	

	// clearing the screen and setting the cursor to the home position
	printf("\x1b[2J");
	printf("\x1b[H");

	printf("\e[?25l"); // hide the cursor
	

	int SIZE = 50;
	char background = ' ';
	char cords[SIZE][SIZE];

	
	//projection 3d matrix
	float ProjectionMatrix[3][3] =  {
		{1,0,0},
		{0,1,0},
      		{0,0,1}
	};

	
	/*
	//defining square 
	//cords are normalized between <-2;2>
	
	square sq, sqr;
	sq.points[0].x=-0.1*SHAPE_SCALE;
	sq.points[0].y=0.1*SHAPE_SCALE;
	sq.points[1].x=0.1*SHAPE_SCALE;
	sq.points[1].y=0.1*SHAPE_SCALE;
	sq.points[2].x=0.1*SHAPE_SCALE;
	sq.points[2].y=-0.1*SHAPE_SCALE;
	sq.points[3].x=-0.1*SHAPE_SCALE;
	sq.points[3].y=-0.1*SHAPE_SCALE;	
	*/

	//defining a cube
	cube cb, cbr;
	cb.points[0].x = 0.1*SHAPE_SCALE;
	cb.points[0].y = 0.1*SHAPE_SCALE;
	cb.points[0].z = 0.1*SHAPE_SCALE;

	cb.points[1].x = 0.1*SHAPE_SCALE;
	cb.points[1].y = -0.1*SHAPE_SCALE;
	cb.points[1].z = 0.1*SHAPE_SCALE;

	cb.points[2].x = -0.1*SHAPE_SCALE;
	cb.points[2].y = -0.1*SHAPE_SCALE;
	cb.points[2].z = 0.1*SHAPE_SCALE;

	cb.points[3].x = -0.1*SHAPE_SCALE;
	cb.points[3].y = 0.1*SHAPE_SCALE;
	cb.points[3].z = 0.1*SHAPE_SCALE;

	cb.points[4].x = 0.1*SHAPE_SCALE;
	cb.points[4].y = 0.1*SHAPE_SCALE;
	cb.points[4].z = -0.1*SHAPE_SCALE;

	cb.points[5].x = 0.1*SHAPE_SCALE;
	cb.points[5].y = -0.1*SHAPE_SCALE;
	cb.points[5].z = -0.1*SHAPE_SCALE;

	cb.points[6].x = -0.1*SHAPE_SCALE;
	cb.points[6].y = -0.1*SHAPE_SCALE;
	cb.points[6].z = -0.1*SHAPE_SCALE;

	cb.points[7].x = -0.1*SHAPE_SCALE;
	cb.points[7].y = 0.1*SHAPE_SCALE;
	cb.points[7].z = -0.1*SHAPE_SCALE;


	float phi=0;

	//printing
	int frame = 0;
	

	while(1){
	
		printf("\x1b[H"); // home position cursor
				// initialize with blanks and the border
		for(int i =0; i<SIZE; i++){
			for(int j = 0; j<SIZE; j++){
				cords[i][j] = background;
				if(i==0 || i==SIZE-1){
					cords[i][j] = '-';
				}
				if(j==0  || j==SIZE-1){
					cords[i][j] = '|';
				}
				if((j==0 && i==0) || (j==SIZE-1 && i==SIZE-1) || (j==0 && i==SIZE-1) || (j==SIZE-1 && i==0) ){
				cords[i][j] = '+';
				}	
			}
		}
	
		/*
		// code to generate sppinning 2d square
		sqr = sq;
		rotateSquare_2D_XY(&sqr, &phi);

		makeLineB(normalize(sqr.points[0].x, &SIZE),normalize(sqr.points[0].y, &SIZE),normalize(sqr.points[1].x, &SIZE),normalize(sqr.points[1].y, &SIZE), &SIZE, cords);
		makeLineB(normalize(sqr.points[1].x, &SIZE),normalize(sqr.points[1].y, &SIZE),normalize(sqr.points[2].x, &SIZE),normalize(sqr.points[2].y, &SIZE), &SIZE, cords);
		makeLineB(normalize(sqr.points[2].x, &SIZE),normalize(sqr.points[2].y, &SIZE),normalize(sqr.points[3].x, &SIZE),normalize(sqr.points[3].y, &SIZE), &SIZE, cords);
		makeLineB(normalize(sqr.points[3].x, &SIZE),normalize(sqr.points[3].y, &SIZE),normalize(sqr.points[0].x, &SIZE),normalize(sqr.points[0].y, &SIZE), &SIZE, cords);

		for(int u=0;u<SQ_SIZE;u++){
			cords[normalize(sqr.points[u].y, &SIZE)][normalize(sqr.points[u].x, &SIZE)]='o';
		}
		*/

		cbr = cb;
		matmul3x3(&cbr, ProjectionMatrix);

		// Rotation matrix around the X-axis
		float rotation_X[3][3] = {
    			{1, 0, 0},
    			{0, cos(phi), -sin(phi)},
    			{0, sin(phi), cos(phi)}
		};

		// Rotation matrix around the Y-axis
		float rotation_Y[3][3] = {
    			{cos(phi), 0, sin(phi)},
    			{0, 1, 0},
    			{-sin(phi), 0, cos(phi)}
		};

		// Rotation matrix around the Z-axis
		float rotation_Z[3][3] = {
    			{cos(phi), -sin(phi), 0},
    			{sin(phi), cos(phi), 0},
    			{0, 0, 1}
		};

		matmul3x3(&cbr, rotation_X);
		matmul3x3(&cbr, rotation_Y);
		matmul3x3(&cbr, rotation_Z);
		
		connectCube(&cbr, &SIZE, cords);	

		for(int u=0;u<CB_SIZE;u++){
			cords[normalize(cbr.points[u].y, &SIZE)][normalize(cbr.points[u].x, &SIZE)]='o';
		}
		

		//printing the whole screen
		for(int i =0; i<SIZE; i++){
			for(int j = 0; j<SIZE; j++){
				putchar(cords[i][j]);
			}
			printf("\n");
		}
	
		

		/*	
		//frame data
		frame =  (frame<2000)?frame+1 : 0;
		printf("%d\n",frame);
		*/
		//calctulating phi
		phi = (phi<M_PI*2)?phi+PHI_SPD:0;
		//printf("phi: %f\n", phi);
		//if (phi>2*M_PI) return 0;

		//freezing
		Sleep(50); //frame rate is * the size of the board
		//sleep(50);
	}
  return 0;
}

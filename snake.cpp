#include<cv.h>
#include<highgui.h>

#include<stdio.h>
#include<stdlib.h>

#define WINDOW_TITLE "Snake v2.0 By Xuan, SeyoWork"
#define INIT_WINDOW_SEYO "resources\\init_window_seyo.jpg"
#define INIT_WINDOW_LOGO "resources\\init_window_logo.jpg"
#define INIT_WINDOW_INPUT "resources\\init_window_input.jpg"

struct Snake
{
	int x;
	int y;
	Snake *next;
};
bool blocks[20][20];
Snake *LSnake = new Snake;
Snake *LFood = new Snake;
Snake food;
CvMat *mainWindow = cvCreateMat(400, 500, CV_8UC3);
char move;
int score;
int speed;
int wktime;
int status;

void on_mouse(int event, int x, int y, int flags, void *usfc);

void graphFood(int x, int y)
{
	for (int i = 20 * x + 4; i < 20 * (x + 1) - 4; i++)
	{
		for (int j = 20 * y + 4; j < 20 * (y + 1) - 4; j++)
		{
			cvSet2D(mainWindow, i, j, cvScalar(0, 255, 0, 0));
		}
	}
}

void loadFood()
{
	Snake *TFood = new Snake[390 - score];
	int index = 0;
	Snake *p = LFood;
	while (p->next)
	{
		p = p->next;
		TFood[index].x = p->x;
		TFood[index].y = p->y;
		index++;
	}
	srand((unsigned)time(NULL));
	int rn = rand() % (390 - score);
	food.x = TFood[rn].x;
	food.y = TFood[rn].y;
	graphFood(food.x, food.y);
	delete[]TFood;
}

void eraseScore()
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 1.0, 1.0, 0, 1, CV_AA);
	char temp[4];
	sprintf(temp, "%d", score);
	cvPutText(mainWindow, temp, cvPoint(415, 80), &font, cvScalar(0, 0, 0, 0));
}

void graphScore()
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 1.0, 1.0, 0, 1, CV_AA);
	char temp[4];
	sprintf(temp, "%d", score);
	cvPutText(mainWindow, temp, cvPoint(415, 80), &font, cvScalar(255, 0, 0, 0));
}

void addFood(int x, int y)
{
	Snake *newFood = new Snake;
	newFood->x = x;
	newFood->y = y;
	newFood->next = LFood->next;
	LFood->next = newFood;
}

void deleteFood(int x, int y)
{
	Snake *p = LFood;
	Snake *dp;
	while (p->next)
	{
		if (p->next->x == x&&p->next->y == y) break;
		p = p->next;
	}
	dp = p->next;
	p->next = dp->next;
	delete dp;
}

void releaseFood()
{
	Snake *p = LFood->next;
	Snake *dp;
	while (p)
	{
		dp = p;
		LFood->next = p->next;
		p = p->next;
		delete dp;
	}
}

void setWindowValue(int a, int b, int value)
{
	for (int i = 20 * a; i < 20 * (a + 1); i++)
	{
		for (int j = 20 * b; j < 20 * (b + 1); j++)
		{
			cvSet2D(mainWindow, i, j, cvScalar(0, 0, value, 0));
		}
	}
}

void graphSnake()
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (blocks[i][j] == true)
			{
				setWindowValue(i, j, 255);
			}
			else
			{
				setWindowValue(i, j, 0);
			}
		}
	}
}

void addSnake(int x, int y)
{
	Snake *newSnake = new Snake;
	newSnake->x = x;
	newSnake->y = y;
	newSnake->next = LSnake->next;
	LSnake->next = newSnake;
	blocks[x][y] = true;
}

void deleteSnake(int *x, int *y)
{
	Snake *p = LSnake;
	Snake *dp;
	while (p->next->next)
	{
		p = p->next;
	}
	dp = p->next;
	p->next = NULL;
	*x = dp->x;
	*y = dp->y;
	blocks[dp->x][dp->y] = false;
	delete dp;
}

void releaseSnake()
{
	Snake *p = LSnake->next;
	Snake *dp;
	while (p)
	{
		dp = p;
		LSnake->next = p->next;
		p = p->next;
		delete dp;
	}
}

void snakeDead()
{
	CvFont font;
	bool win = false;
	if (LFood->next == NULL) win = true;
	if (win == true)
	{
		cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 5.0, 5.0, 0, 10, CV_AA);
		cvPutText(mainWindow, "WIN", cvPoint(50, 240), &font, cvScalar(255, 0, 0, 0));
	}
	else
	{
		cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 4.0, 4.0, 0, 10, CV_AA);
		cvPutText(mainWindow, "DEAD", cvPoint(40, 230), &font, cvScalar(255, 0, 0, 0));
	}
	cvShowImage(WINDOW_TITLE, mainWindow);
	status = 2;
}

void snakeMoving(char key)
{
	bool eat = false;
	if (key + move == 'a' + 'd' || key + move == 'w' + 's') return;
	if (key == 'a' || key == 's' || key == 'd' || key == 'w')
	{
		move = key;
	}
	int x = LSnake->next->x;
	int y = LSnake->next->y;
	int xn, yn, xf, yf;
	if (move == 'a')
	{
		xn = x;
		yn = y - 1;
		if (yn < 0 || blocks[xn][yn] == true)
		{
			snakeDead();
			return;
		}
	}
	if (move == 'w')
	{
		xn = x - 1;
		yn = y;
		if (xn < 0 || blocks[xn][yn] == true)
		{
			snakeDead();
			return;
		}
	}
	if (move == 'd')
	{
		xn = x;
		yn = y + 1;
		if (yn > 19 || blocks[xn][yn] == true)
		{
			snakeDead();
			return;
		}
	}
	if (move == 's')
	{
		xn = x + 1;
		yn = y;
		if (xn > 19 || blocks[xn][yn] == true)
		{
			snakeDead();
			return;
		}
	}
	if (xn == food.x&&yn == food.y)
	{
		eat = !eat;
	}
	addSnake(xn, yn);
	deleteFood(xn, yn);
	if (!eat)
	{
		deleteSnake(&xf, &yf);
		addFood(xf, yf);
	}
	graphSnake();
	if (eat)
	{
		eraseScore();
		score++;
		graphScore();
		loadFood();
	}
	graphFood(food.x, food.y);
	cvShowImage(WINDOW_TITLE, mainWindow);
}

void eraseSpeed()
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 1.0, 1.0, 0, 1, CV_AA);
	char temp[3];
	sprintf(temp, "%d", speed);
	cvPutText(mainWindow, temp, cvPoint(445, 180), &font, cvScalar(0, 0, 0, 0));
}

void graphSpeed()
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 1.0, 1.0, 0, 1, CV_AA);
	char temp[3];
	sprintf(temp, "%d", speed);
	cvPutText(mainWindow, temp, cvPoint(445, 180), &font, cvScalar(255, 0, 0, 0));
	cvShowImage(WINDOW_TITLE, mainWindow);
}

void changeSpeed(int param)
{
	int speed2wktime[] = { 1000, 500, 200, 100, 50, 20, 10, 5, 2, 1 };
	eraseSpeed();
	if (param == 0 && speed > 0) speed--;
	if (param == 1 && speed < 9) speed++;
	graphSpeed();
	wktime = speed2wktime[speed];
}

void initSpeed()
{
	speed = 0;
}

void initGraph()
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			blocks[i][j] = false;
		}
	}
	cvZero(mainWindow);
	graphSnake();
	for (int i = 0; i < 400; i++)
	{
		cvSet2D(mainWindow, i, 401, cvScalar(255, 255, 255, 0));
		cvSet2D(mainWindow, i, 402, cvScalar(255, 255, 255, 0));
		cvSet2D(mainWindow, i, 403, cvScalar(255, 255, 255, 0));
	}
	CvFont font1, font2;
	cvInitFont(&font1, CV_FONT_HERSHEY_TRIPLEX, 0.7, 0.7, 0, 1, CV_AA);
	cvInitFont(&font2, CV_FONT_HERSHEY_TRIPLEX, 1.0, 1.0, 0, 1, CV_AA);
	cvPutText(mainWindow, "SCORE", cvPoint(415, 50), &font1, cvScalar(255, 0, 0, 0));
	cvPutText(mainWindow, "SPEED", cvPoint(415, 150), &font1, cvScalar(255, 0, 0, 0));
	cvPutText(mainWindow, "-  +", cvPoint(415, 180), &font2, cvScalar(255, 0, 0, 0));
	cvPutText(mainWindow, "PAUSE", cvPoint(415, 290), &font1, cvScalar(255, 0, 0, 0));
	cvPutText(mainWindow, "START", cvPoint(415, 320), &font1, cvScalar(255, 0, 0, 0));
	cvPutText(mainWindow, "EXIT", cvPoint(422, 350), &font1, cvScalar(255, 0, 0, 0));
}

void initFood()
{
	LFood->next = NULL;
	for (int i = 10; i < 20; i++)
	{
		addFood(0, i);
	}
	for (int i = 1; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			addFood(i, j);
		}
	}
}

void initSnake()
{
	LSnake->next = NULL;
	for (int i = 0; i < 10; i++)
	{
		addSnake(0, i);
	}
}

void initGame()
{
	for (int i = 0; i < 10; i++)
	{
		blocks[0][i] = true;
	}
	graphSnake();
	initSnake();
	initFood();
	move = 'd';
	score = 0;
	wktime = 1000;
	status = 0;
	graphScore();
	loadFood();
	graphSpeed();
	cvShowImage(WINDOW_TITLE, mainWindow);
}

void initWindow()
{
	cvNamedWindow(WINDOW_TITLE, 0);
	cvResizeWindow(WINDOW_TITLE, 500, 400);
	cvMoveWindow(WINDOW_TITLE, 100, 100);
	cvSetMouseCallback(WINDOW_TITLE, on_mouse);
	IplImage *img = NULL;
	img = cvLoadImage(INIT_WINDOW_SEYO);
	cvShowImage(WINDOW_TITLE, img);
	cvWaitKey(1500);
	cvReleaseImage(&img);
	img = cvLoadImage(INIT_WINDOW_LOGO);
	cvShowImage(WINDOW_TITLE, img);
	cvWaitKey(1500);
	cvReleaseImage(&img);
	img = cvLoadImage(INIT_WINDOW_INPUT);
	cvShowImage(WINDOW_TITLE, img);
	cvWaitKey();
	cvReleaseImage(&img);
}

void preExit()
{
	releaseSnake();
	releaseFood();
	cvReleaseMat(&mainWindow);
	cvDestroyWindow(WINDOW_TITLE);
	exit(0);
}

void on_mouse(int event, int x, int y, int flags, void *usfc)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		if (x >= 419 && x <= 438 && y >= 158 && y <= 183) changeSpeed(0);
		if (x >= 476 && x <= 495 && y >= 158 && y <= 183) changeSpeed(1);
		if (x >= 416 && x <= 488 && y >= 275 && y <= 293) status = 3;
		if (x >= 416 && x <= 488 && y >= 305 && y <= 323) status = 1;
		if (x >= 416 && x <= 488 && y >= 335 && y <= 353) preExit();
	}
}

void mouseCallBack()
{
	cvSetMouseCallback(WINDOW_TITLE, on_mouse);
}

void keyBoardCallBack(char key)
{
	if (key == '-') changeSpeed(0);
	if (key == '=') changeSpeed(1);
	if (key == '0' && status == 1) status = 3;
	if (key == '9' && status != 1) status = 1;
	if (key == '8') preExit();
}

void callBack()
{
	mouseCallBack();
	char key = cvWaitKey(10);
	keyBoardCallBack(key);
}

void pauseGame()
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 3.0, 3.0, 0, 10, CV_AA);
	cvPutText(mainWindow, "PAUSE", cvPoint(40, 220), &font, cvScalar(255, 0, 0, 0));
	cvShowImage(WINDOW_TITLE, mainWindow);
	while (true)
	{
		callBack();
		if (status == 1) break;
	}
}

int main()
{
	initWindow();
	initGraph();
	initSpeed();
	initGame();
	while (true)
	{
		callBack();
		if (status == 1) break;
	}
	while (true)
	{
		while (true)
		{
			mouseCallBack();
			char key = cvWaitKey(wktime);
			keyBoardCallBack(key);
			snakeMoving(key);
			if (status == 2) break;
			if (status == 3)
			{
				pauseGame();
				graphSnake();
				graphFood(food.x, food.y);
			}
		}
		while (true)
		{
			callBack();
			if (status == 1) break;
		}
		releaseSnake();
		releaseFood();
		initGraph();
		initGame();
		changeSpeed(2);
	}
	return 0;
}
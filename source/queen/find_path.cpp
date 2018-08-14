#include "find_path.h"


//global var
static int n = 0;//全排列计数
static double minDist = 5000.0;
#define START_POINT_X 4
#define START_POINT_Y 9
int NUM = 8;//will be changed in main
chessNode* routeA;// will be malloc in main
chessNode* routeB;//chessNode routeA[NUM], routeB[NUM]

void swap(chessNode *a, chessNode *b)
{
	chessNode m;
	m = *a;
	*a = *b;
	*b = m;
}

double dist_count(int x1, int x2, int y1, int y2)
{
	double d_x_2,d_y_2;
	d_x_2 = (x1-x2)*(x1-x2);
	d_y_2 = (y1-y2)*(y1-y2);
	return sqrt(d_x_2+d_y_2);
}

void perm(chessNode list[], int low, int high,int low2, chessNode list2[])//list[] for solution; list2[] for chess.
{
	int i,j;
	if (low >high)
	{//perm has been done one times for list1.
		if (low2 >high)
		{//perm has been done two times for two list.

			n++;
			// int newDist = abs(list2[0].x - START_POINT_X) + abs(list2[0].y - START_POINT_Y);//起点到第一个棋子距离
			// for (i = 0; i <= high-1; i++)
			// {
			// 	newDist += abs(list2[i].x - list[i].x) + abs(list2[i].y - list[i].y)+ abs(list2[i+1].x - list[i].x) + abs(list2[i+1].y - list[i].y);
			// }
			// newDist += abs(list2[high].x - list[high].x) + abs(list2[high].y - list[high].y);

			double newDist = dist_count(START_POINT_X,list2[0].x,START_POINT_Y,list2[0].y);//起点到第一个棋子距离
			for (i = 0; i <= high-1; i++)
			{
				newDist += dist_count(list2[i].x , list[i].x , list2[i].y , list[i].y) + dist_count(list2[i+1].x , list[i].x , list2[i+1].y , list[i].y);
			}
			newDist += dist_count(list2[high].x , list[high].x , list2[high].y , list[high].y);

			/*
			 *
			 *for to end policy
			 *
			 */
			int x_to_end = 0;
			int y_to_end = 0;
			if(list[high].x<=4)
				x_to_end = list[high].x;
			else
				x_to_end = 9 - list[high].x;

			if(list[high].y<=4)
				y_to_end = list[high].y;
			else
				y_to_end = 9 - list[high].y;

			if(x_to_end<y_to_end)
				newDist += x_to_end;
			else
				newDist += y_to_end;
			/*
			 *
			 *for to end policy
			 *
			 */
			if (newDist < minDist)
			{
				minDist = newDist;
				for (int i = 0; i <= high; i++)
				{
					routeA[i].x = list[i].x;
					routeB[i].x = list2[i].x;
					routeA[i].y = list[i].y;
					routeB[i].y = list2[i].y;
				}
			}
		}
		else
		{
			for (i = low2; i <= high; i++)
			{
				swap(&list2[low2], &list2[i]);
				perm(list, low , high,low2+1,list2);
				swap(&list2[low2], &list2[i]);
			}
		}


	}
	else
	{
		for (i = low; i <= high; i++)
		{
			swap(&list[low], &list[i]);
			perm(list, low + 1, high,low2,list2);
			swap(&list[low], &list[i]);
		}
	}
}

void path()
{
	clock_t start, end;
	start = clock();
	//…calculating…
	perm(solution, 0, NUM-1,0,chess);
	cout << "minDistance = " << minDist << endl;
	cout << " ("<<START_POINT_X<<","<<START_POINT_Y<<")" << endl;
	for (int i = 0; i <= NUM-1; i++)
	{
		cout << " (" << routeB[i].x << "," << routeB[i].y << ") " ;//B:original chess position
		cout << " (" << routeA[i].x << "," << routeA[i].y << ") " << endl;//A:target solution position
	}
	cout << n <<endl;
	//
	end = clock();
	printf("time for path-planning = %f\n", (double)(end - start) / CLOCKS_PER_SEC);
}

// int main()
// {
// 	//chessNode solution[NUM],chess[NUM];
// 	for (int i = 0; i <= NUM-1; i++)
// 	{
// 		//input chess & solution
// 		scanf("%d%d", &chess[i].x, &chess[i].y);
// 		scanf("%d%d", &solution[i].x, &solution[i].y);
// 		//chess[i].match = NULL;
// 		//solution[i].match = NULL;


// 	}

// 	//char list[] = "1234";

// 	clock_t start, end;
// 	start = clock();
// 	//…calculating…
// 	perm(chess, 0, NUM-1,0,solution);
// 	//cout << "total:" << n <<","<<n2<< endl;
// 	cout << "minDistance = " << minDist << endl;
// 	cout << "(0,0)" << endl;
// 	for (int i = 0; i <= NUM-1; i++)
// 	{
// 		cout << " (" << routeB[i].x << "," << routeB[i].y << ") " ;
// 		cout << " (" << routeA[i].x << "," << routeA[i].y << ") " << endl;
// 	}
// 	cout << n <<endl;
// 	//
// 	end = clock();
// 	printf("time=%f\n", (double)(end - start) / CLOCKS_PER_SEC);
// 	//system("pause");
// 	return 0;
// }

// 	cout << "(0,0)" << endl;
// 	for (int i = 0; i <= NUM-1; i++)
// 	{
// 		cout << " (" << routeB[i].x << "," << routeB[i].y << ") " ;
// 		cout << " (" << routeA[i].x << "," << routeA[i].y << ") " << endl;
// 	}
// 	cout << n <<endl;
// 	//
// 	end = clock();
// 	printf("time=%f\n", (double)(end - start) / CLOCKS_PER_SEC);
// 	//system("pause");
// 	return 0;
// }


#include "find_path.h"


//global var
static int n = 0;//全排列计数
static int minDist = 65535;

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

void perm(chessNode list[], int low, int high,int low2, chessNode list2[])
{
	int i,j;
	if (low >high)
	{


		if (low2 >high)
		{

			n++;
			int newDist = list2[0].x + list2[0].y;//起点到第一个棋子距离
			for (i = 0; i <= high-1; i++)
			{
				newDist += abs(list2[i].x - list[i].x) + abs(list2[i].y - list[i].y)+ abs(list2[i+1].x - list[i].x) + abs(list2[i+1].y - list[i].y);
			}
			newDist += abs(list2[high].x - list[high].x) + abs(list2[high].y - list[high].y);
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
	perm(chess, 0, NUM-1,0,solution);
	cout << "minDistance = " << minDist << endl;
	cout << " (0,0)" << endl;
	for (int i = 0; i <= NUM-1; i++)
	{
		cout << " (" << routeB[i].x << "," << routeB[i].y << ") " ;
		cout << " (" << routeA[i].x << "," << routeA[i].y << ") " << endl;
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


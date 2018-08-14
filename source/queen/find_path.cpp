#include "find_path.h"


//global var
static int n = 0;//閸忋劑鏁撻弬銈嗗闁跨喎褰ㄧ涵閿嬪闁跨喐鏋婚幏锟�
static double minDist = 5000.0;
#define START_POINT_X 4
#define START_POINT_Y 9
int NUM = 8;//will be changed in main
chessNode routeA[MAX_SOLUTION_AMOUNT][8];// will be malloc in main
chessNode routeB[MAX_SOLUTION_AMOUNT][8];//chessNode routeA[NUM], routeB[NUM]
int min_route_index = 1000;
using namespace std;
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

void perm(chessNode list[], int low, int high,int low2, chessNode list2[], int num)//list[] for solution; list2[] for chess.
{
	int i,j;
	if (low >high)
	{//perm has been done one times for list1.
		if (low2 >high)
		{//perm has been done two times for two list.

			n++;
			// int newDist = abs(list2[0].x - START_POINT_X) + abs(list2[0].y - START_POINT_Y);//闁跨喐鏋婚幏閿嬪壏閺傘倖瀚归柨鐔稿疆娴兼瑦瀚归柨鐔告灮閹风兘鏁撻弬銈嗗濞夋娊鏁撻弬銈嗗闁跨噦鎷�
			// for (i = 0; i <= high-1; i++)
			// {
			// 	newDist += abs(list2[i].x - list[i].x) + abs(list2[i].y - list[i].y)+ abs(list2[i+1].x - list[i].x) + abs(list2[i+1].y - list[i].y);
			// }
			// newDist += abs(list2[high].x - list[high].x) + abs(list2[high].y - list[high].y);

			double newDist = dist_count(START_POINT_X,list2[0].x,START_POINT_Y,list2[0].y);//闁跨喐鏋婚幏閿嬪壏閺傘倖瀚归柨鐔稿疆娴兼瑦瀚归柨鐔告灮閹风兘鏁撻弬銈嗗濞夋娊鏁撻弬銈嗗闁跨噦鎷�
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
					routeA[num][i].x = list[i].x;
					//cout<<list[i].x<<endl;
					routeB[num][i].x = list2[i].x;
					routeA[num][i].y = list[i].y;
					routeB[num][i].y = list2[i].y;
				}
			}
		}
		else
		{
			for (i = low2; i <= high; i++)
			{
				swap(&list2[low2], &list2[i]);
				perm(list, low , high,low2+1,list2,num);
				swap(&list2[low2], &list2[i]);
			}
		}


	}
	else
	{
		for (i = low; i <= high; i++)
		{
			swap(&list[low], &list[i]);
			perm(list, low + 1, high,low2,list2,num);
			swap(&list[low], &list[i]);
		}
	}
}

void path()
{
	clock_t start, end;
	start = clock();
	//闁跨喐鏋婚幏绌媋lculating闁跨喐鏋婚幏锟�
	double all_minDist = 4000.0 ;
	// cout<<n<<"..."<<MAX_SOLUTION_AMOUNT<<"[[["<<amount_coincidence<<endl;
	for(int num=0;num<amount_coincidence;num++)
    {   
		// cout<<num<<"..."<<MAX_SOLUTION_AMOUNT<<endl;
        if(num<MAX_SOLUTION_AMOUNT)
        {
			// cout<<"she is here"<<endl;
			perm(solution[num], 0, NUM-1,0,chess[num],num);
			printf("distance No.%d = %f\n",num,minDist);
			if(num==0)
			{
				all_minDist = minDist;
				min_route_index = num;
			}

			else
			{
				if(minDist < all_minDist)
				{
					all_minDist = minDist;
					min_route_index = num;
				}
			}
			minDist = 5000.0;
		}
	}
	cout << "min_route_index = " << min_route_index << endl;
	cout << "\n\nminDistance = " << all_minDist << endl;
	cout << " ("<<START_POINT_X<<","<<START_POINT_Y<<")" << endl;
	for (int i = 0; i <= NUM-1; i++)
	{
		cout << " (" << routeB[min_route_index][i].x << "," << routeB[min_route_index][i].y << ") " ;//B:original chess position
		cout << " (" << routeA[min_route_index][i].x << "," << routeA[min_route_index][i].y << ") " << endl;//A:target solution position
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
// 	//闁跨喐鏋婚幏绌媋lculating闁跨喐鏋婚幏锟�
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


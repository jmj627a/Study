#include <stdio.h>


int quickArr[10] = { 7,4,8,2,0,5,6,3,1,9 };
//int pivot = 0; //0��° ���� �Ǻ�
//int right = 9; //�迭 �� ������ 9��° ���� right
//int left = 1; //�Ǻ� ���� ���� left

void swap(int l, int r) {
	int temp = quickArr[l];
	quickArr[l] = quickArr[r];
	quickArr[r] = temp;
}

void quickSort(int _left, int _right)
{
	if (_right <= _left)
		return;

	printf("( %d ~ %d )", _left, _right);
	for (int i = 0; i < 10; ++i)
		printf("%d ", quickArr[i]);
	printf("\n");

	int pivot = _left;

	int left = pivot + 1;
	int right = _right;

	while (true)
	{
		for (int i = left; i <= _right; ++i)
		{
			if (quickArr[i] > quickArr[pivot])
			{
				left = i;
				break;
			}
		}

		for (int i = right; i >= _left; --i)
		{
			if (quickArr[i] <= quickArr[pivot])
			{
				right = i;
				break;
			}
		}

		if (right <= left)
			break;


		swap(left, right);
	}

	int temp = quickArr[pivot];
	for (int i = _left; i <= left - 1; ++i)
	{
		quickArr[i] = quickArr[i + 1];
	}
	quickArr[right] = temp;


	quickSort(_left, right - 1);
	quickSort(right + 1, _right);

}



void main()
{
	quickSort(0, 9);

	printf("\n");
	for (int i = 0; i < 10; ++i)
		printf("%d ", quickArr[i]);
	printf("\n");
}

/*
int qSort(int left, int right)
{
	if (left >= right)
	{
		return 0;
	}

	int pivot = left - 1;
	int first = left;
	int temp = quickArr[pivot];
	quickArr[pivot] = quickArr[left];
	quickArr[left] = temp;

	for (int i = left + 1; i <= right; i++)
	{
		if (quickArr[left] < quickArr[i])
		{
			first++;
			temp = quickArr[first];
			quickArr[first] = quickArr[i];
			quickArr[i] = temp;
		}
	}

	temp = quickArr[first];
	quickArr[first] = quickArr[left];
	quickArr[left] = temp;

	qSort(left, first - 1);
	qSort(first + 1, right);

}
*/
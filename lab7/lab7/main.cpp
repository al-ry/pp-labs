#include <memory>
#include "CTask.h"
#include "CWorker.h"
#include <iostream>

int main()
{
	CTask task1(5);
	CTask task2(2);
	CTask task3(8);
	CWorker worker;
	worker.ExecuteTask(&task1);
	worker.ExecuteTask(&task2);
	worker.ExecuteTask(&task3);
	return 0;
}
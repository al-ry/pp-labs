#include "bmp.h"
#include "BlurTask.h"
#include "Pool.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <cctype>
#include <algorithm>
#include <vector>

std::vector<std::filesystem::path> ReadDirectory(std::string path)
{
	std::vector<std::filesystem::path> files;
	auto dirIt = std::filesystem::directory_iterator(path);
	for (const auto& entry : dirIt)
	{
		std::string sPath = entry.path().string();
		std::string exst = std::filesystem::path(sPath).extension().string();
		if (exst == ".bmp")
		{
			files.push_back(entry.path());
		}
	}
	return files;
}

std::vector<ITask*> CreateTasks(bitmap& bmp, int blocksCount)
{
	auto width = bmp.getWidth();
	std::vector<ITask*> result;
	div_t divider;
	if (blocksCount != 0)
	{
		divider = div(width, blocksCount);
	}
	for (size_t i = 0; i < blocksCount; i++)
	{
		BlurParams prms;
		prms.bmp = &bmp;
		prms.startHeight = 0;
		prms.endHeight = bmp.getHeight();
		prms.startWidth = divider.quot * i;
		if (i == blocksCount - 1)
		{
			prms.endWidth = divider.quot * (i + 1) + divider.rem;
		}
		else
		{
			prms.endWidth = divider.quot * (i + 1);
		}

		auto task = new BlurTask(5, prms);
		result.push_back(task);
	}
	return result;
}

void CreateDirectory(const std::string& path)
{
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directory(path);
	}
}



int main(int argc, char ** argv)
{
	std::string mode = argv[1];
	int blocksCount = std::atoi(argv[2]);
	std::string inDirectory = argv[3];
	std::string outDirectory = argv[4];
	int threadsCount = std::atoi(argv[5]);

	auto start = std::chrono::high_resolution_clock::now();

	auto dirFiles = ReadDirectory(inDirectory);
	CreateDirectory(outDirectory);


	for (size_t i = 0; i < dirFiles.size(); i++)
	{
		bitmap bitmap(dirFiles[i].string().c_str());
		std::vector<ITask*> tasks = CreateTasks(bitmap, blocksCount);

		Pool pool(tasks, threadsCount);
		if (mode == "1")
		{
			pool.Execute();
		}
		else if (mode == "0")
		{
			HANDLE* handles = new HANDLE[blocksCount];
			for (int j = 0; j < blocksCount; j++)
			{
				handles[j] = CreateThread(NULL, 0, &ThreadProc, tasks[j], 0, NULL);
			}

			WaitForMultipleObjects(blocksCount, handles, true, INFINITE);
			delete[] handles;
		}
		else
		{
			std::cout << "Error mode\n";
			return 1;
		}

		std::string out = outDirectory + "/" + dirFiles[i].filename().string().c_str();
		bitmap.save(out.c_str());

		for (auto *item: tasks)
		{
			delete item;
		}
	}

	return 0;
}
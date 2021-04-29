#pragma once
#include <fstream>
#include "List.h"


class LogWriter {
private:
	std::ofstream output{ "output.txt" };

public:
	void Log(CMyList<std::string>& list) {
		for (auto &item: list)
		{
			output << list.GetFrontElement();
		}
		list.Clear();
	}
};
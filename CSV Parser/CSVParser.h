#pragma once
#include <vector>
#include <tuple>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>


template <typename T, int index>
typename std::enable_if<(index == 0), void>::type parseInTuple(T& t, std::vector<std::string> elements) {}

template <typename T, int index>
typename std::enable_if<(index != 0), void>::type parseInTuple(T& t, std::vector<std::string> elements) {
	writeToTuple(elements[index - 1], std::get<index - 1>(t));
	parseInTuple<T, index - 1>(t, elements);
}


bool isNumber(std::string& x) {
	std::regex rx("^([1-9]+|0)($|(.[0-9]+)$)");
	return(x != "." && std::regex_match(x, rx));
}

template <typename T>
void writeToTuple(std::string& element, T& x) {
	if (isNumber(element)) {
		std::stringstream tmp(element);
		tmp >> x;
	}
	else {
		std::cerr << "Can't recognize element: " + element << std::endl;
		exit(1);
	}
}

void writeToTuple(std::string& element, std::string& x) {
	x = element;
}

void writeToTuple(std::string& element, char& x) {
	if (element.size() != 1) {
		std::cerr << element + " - recognized as char, but it is not a char" << std::endl;
		exit(1);
	}
	x = element[0];
}

void writeToTuple(std::string& element, bool& x) {
	if (element == "true") {
		x = true;
	}
	else {
		if (element == "false") {
			x = false;
		}
		else {
			std::cerr << element + " - recognized as bool, but it is not a bool" << std::endl;
			exit(1);
		}
	}
}

template <typename... Args>
class CSVParser {
private:
	std::vector<std::tuple<Args...>> data;
	char columnDelimiter;
	char strDelimiter;
	char screenSymbol;
	std::ifstream& file;

	void getElements(std::vector<std::string>& elements, std::string& line) {
		
		int start = 0;
		bool screenFlag = false;

		for (int i = 0; i < line.size(); ++i) {
			if (line[i] == screenSymbol) {
				line.erase(line.begin() + i);
				if (line[i] != screenSymbol) {
					screenFlag = !screenFlag;
					i--;
				}
			}
			else {
				if (line[i] == columnDelimiter && !screenFlag) {
					elements.push_back(line.substr(start, i - start));
					start = i + 1;
				}
			}
		}

		if (start != line.size()) {
			elements.push_back(line.substr(start, line.size()));
		}
	}


public:
	CSVParser(std::ifstream input, int linesToSkip, char columnDel = ',', char strDel = '\n', char screenSymb = '\"'):
		file(input), columnDelimiter(columnDel), strDelimiter(strDel), screenSymbol(screenSymb) {
		try {
			if (!file.is_open()) {
				throw std::invalid_argument("Can't open file");
			}
			if (columnDelimiter == strDelimiter || columnDelimiter == screenSymbol || strDelimiter == screenSymbol) {
				throw std::invalid_argument("Matches between column delimiter, string delimiter and screen symbol");
			}
			// skipping lines
			std::string tmp;
			for (int i = 0; !file.eof() && i < linesToSkip; i++) {
				std::getline(file, tmp, strDelimiter);
			}

			try {
				for (int i = 0; !file.eof(); i++) {
					std::getline(file, tmp, strDelimiter);
					if (!tmp.empty()) {
						std::vector<std::string> elements;
						getElements(elements, tmp);

						if (elements.size() != sizeof...(Args)) {
							throw std::invalid_argument("Not enough or too much arguments in string ¹ " + std::to_string(i + 1));
						}
                        std::tuple<Args...> t;
						parseInTuple<decltype(t), sizeof...(Args)>(t, elements);

						data.push_back(t);

					}
					else {
						throw std::invalid_argument("Empty string ¹ " + std::to_string(i + 1) + " . This string won't be include");
					}
				}
			}
			catch (std::invalid_argument& i){
				std::cerr << i.what() << std::endl;
			}
		}
		catch (std::invalid_argument &i) {
			std::cerr << i.what() << std::endl;
			exit(1);
		}
	}

    class Iterator : public std::iterator<std::input_iterator_tag, std::tuple<Args...>> {
    private:
        std::vector <std::tuple<Args...>>* dataPtr;
        int index;
    public:
        Iterator(int index, std::vector <std::tuple<Args...>>* dataPtr) : index(index), dataPtr(dataPtr) { };
        bool operator==(const Iterator& it) const {
            return (it.dataPtr == dataPtr && it.index == index);
        }
        bool operator!=(const Iterator& it) const {
            return !(it == *this);
        }
        const std::tuple<Args...>& operator*() const {
            return (*dataPtr)[index];
        }
        Iterator& operator++() {
            ++index;
            return *this;
        }
    };
    Iterator begin() {
        return Iterator(0, &data);
    }
    Iterator end() {
        return Iterator(data.size(), &data);
    }
};
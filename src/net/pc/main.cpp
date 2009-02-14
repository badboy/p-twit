#include <iostream>

using namespace std;

int str_gsub(std::string pattern, std::string replacement, std::string& input)
{
	size_t found = input.find(pattern);
	while(found > 0)
	{
		size_t size = input.size();
		if(found < size)
		{
			input.replace(found, pattern.size(), replacement);
		}
		else break;
		found = input.find(pattern);
		cout << found << endl;
	}
	return 1;
}

int str_sub(std::string pattern, std::string replacement, std::string& input)
{
	size_t found = input.find(pattern);
	size_t size = input.size();
	if(found < size)
	{
		input.replace(found, pattern.size(), replacement);
		return 1;
	}
	else return 0;
}

int main(int argc, char** argv)
{
	string str = "hallo!hi!rofl!hihi!";
	cout << str << endl;
	str_sub("!", "", str);
	cout << str << endl;
	str = "hallo!hi!rofl!hihi!";
	cout << str << endl;
	str_gsub("!", "", str);
	cout << str << endl;
return 0;
}


#ifndef _TOP10_H_
#define _TOP10_H_

#include <SDL.h>
#include <string>

struct Top10Line {
	char player[32];
	int score;
};

class Top10
{
public:
	Top10(const char* fileName, const char* key);
	~Top10();
	bool load();
	int isIn(int score) const;
	bool save();
	int highScore() const;
	const char* playerName(int index) const;
	int score(int index) const;
	int insert(const char* player, int score);
private:
	Top10Line _top10[10];
	std::string _fileName;
	std::string _key;
	void _encrypt(const char *strSrc, char *strTarget) const;
	void _decrypt(const char *strSrc, char *strTarget) const;
	bool _fileExist() const;
	bool _createFile() const;
	bool _writeTextLine(const char *line) const;
	bool _readTextFile(std::string* line) const;
	bool _readPlayer(char* player, const char* stClear, int* offset);
	bool _readScore(int* score, const char* stClear, int* offset);
};

#endif _TOP10_H_
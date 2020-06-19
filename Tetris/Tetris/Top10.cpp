#include <Windows.h>
#include <iostream>
#include <fstream>
#include "Top10.h"

using namespace std;

// Initialise l'objet top10 avec le nom du fichier et la clef de chiffrement (masquer les données)
Top10::Top10(const char* fileName, const char* key)
{
	memset(_top10, 0, sizeof(_top10));
	_fileName = fileName;
	_key = key;
}

Top10::~Top10()
{
}

// Chargement du fichier depuis le disque
bool Top10::load() {
	bool ret = _fileExist();
	if (ret) {
		string line;
		ret = _readTextFile(&line);
		if (ret || line.length() > 760) {
			char stClear[400];
			int offset = 0;
			_decrypt(line.c_str(), stClear);
			for (int i = 0; i < 10; i++) {
				ret = _readPlayer(_top10[i].player, stClear, &offset);
				if (!ret)
					break;
				ret = _readScore(&_top10[i].score, stClear, &offset);
				if (!ret)
					break;
			}
		}
	}
	if (!ret)
		for (int i = 0; i < 10; i++)
			strcpy_s(_top10[i].player, "-");
	return ret;
}

// Lecture d'un nom de joueur
bool Top10::_readPlayer(char* player, const char* stClear, int* offset) {
	int startOffset = *offset;
	while (stClear[*offset] != '|' && stClear[*offset] != '\0' && *offset - startOffset < sizeof(_top10[0].player) - 1) {
		player[*offset - startOffset] = stClear[*offset];
		(*offset)++;
	}
	player[*offset - startOffset] = '\0';
	if (stClear[*offset] != '|')
		return false;
	(*offset)++;
	return true;
}

// Lecture d'un score
bool Top10::_readScore(int* score, const char* stClear, int* offset) {
	char stScore[10];
	int startOffset = *offset;
	while (stClear[*offset] != '|' && stClear[*offset] != '\0' && *offset - startOffset < sizeof(stScore) - 1) {
		if (stClear[*offset] < '0' || stClear[*offset] > '9')
			return false;
		stScore[*offset - startOffset] = stClear[*offset];
		(*offset)++;
	}
	stScore[*offset - startOffset] = '\0';
	if (stClear[*offset] != '|')
		return false;
	(*offset)++;
	*score = atoi(stScore);
	return true;
}

// Retourne l'éventuelle position dans le top 10 d'un score
int Top10::isIn(int score) const {
	for (int i = 0; i < 10; i++)
		if (score > _top10[i].score)
			return i + 1;
	return -1;
}

// Sauve les données du top 10 sur le disque
bool Top10::save() {
	char stClear[400];
	char stEncrypt[800];
	stClear[0] = '\0';
	for (int i = 0; i < 10; i++) {
		strcat_s(stClear, _top10[i].player);
		strcat_s(stClear, "|");
		char buffer[10];
		_itoa_s(_top10[i].score, buffer, 10);
		strcat_s(stClear, buffer);
		strcat_s(stClear, "|");
	}
	_encrypt(stClear, stEncrypt);
	bool ret = _createFile();
	if (ret)
		ret = _writeTextLine(stEncrypt);
	return ret;
}

// Retourne le meilleur score du top 10
int Top10::highScore() const {
	return _top10[0].score;
}

// Retourne le nom du joueur correspondant à l'index (position) passé en paramètre
const char* Top10::playerName(int index) const {
	if (index < 1 || index > 10)
		return NULL;
	return _top10[index - 1].player;
}

// Retourne le score du joueur correspondant à l'index (position) passé en paramètre
int Top10::score(int index) const {
	if (index < 1 || index > 10)
		return -1;
	return _top10[index - 1].score;
}

// Chiffre une chaîne de caractères
void Top10::_encrypt(const char *strSrc, char *strTarget) const
{
	int lenPwd = _key.length();
	int offsetPwd = 0;
	while (*strSrc != '\0')
	{
		int v = (byte)*strSrc ^ (byte)_key[offsetPwd];
		sprintf_s(strTarget, 3, "%02x", v);
		strSrc++;
		strTarget += 2;
		offsetPwd = (offsetPwd + 1) % lenPwd;
	}
	*strTarget = '\0';
}

// Déchiffre une chaîne de caractères
void Top10::_decrypt(const char *strSrc, char *strTarget) const
{
	int lenPwd = _key.length();
	int offsetPwd = 0;
	while (*strSrc != '\0')
	{
		int v = *strSrc - '0';
		if (*strSrc > '9')
			v = v - ('a' - '0') + 10;
		strSrc++;
		v = v * 16 + *strSrc - '0';
		if (*strSrc > '9')
			v = v - ('a' - '0') + 10;
		*strTarget = (byte)v ^ (byte)_key[offsetPwd];
		strSrc++;
		strTarget++;
		offsetPwd = (offsetPwd + 1) % lenPwd;
	}
	*strTarget = '\0';
}

// Retourne true si le fichier top10.dat existe
bool Top10::_fileExist() const
{
	ifstream file(_fileName.c_str());
	if (file)
		return true;
	return false;
}

// Créé le fichier top10
bool Top10::_createFile() const
{
	ofstream file(_fileName.c_str());
	if (file)
		return true;
	return false;
}

// Ecriture de la ligne de donnée dans le fichier top10
bool Top10::_writeTextLine(const char *line) const
{
	ofstream file(_fileName.c_str(), ios::app);
	if (file)
		file << line << endl;
	else
		return false;
	return true;
}

// Lecture de la ligne du fichier top10
bool Top10::_readTextFile(string* line) const
{
	ifstream file(_fileName.c_str());
	if (file)
		getline(file, *line);
	else
		return false;
	return true;
}

// Insère une nouvelle ligne dans le top10
int Top10::insert(const char* player, int score) {
	int i = isIn(score) - 1;
	for (int j = 9; j > i; j--)
		memcpy(&_top10[j], &_top10[j - 1], sizeof(Top10Line));
	_top10[i].score = score;
	strcpy_s(_top10[i].player, player);
	return i;
}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

char translation(char c, char key)
{
	if ((c >= 'A' && c <= 'Z') && (key >= 'A' && key <= 'Z'))
		c = ((c - key + 26) % 26) + 'A';
	return c;
}

char *decodeCipher(char *cipherText, char *key, int keyLen)
{
	int keyIdx = 0;
	int idx = -1;

	while (++idx < strlen(cipherText))
	{
		cipherText[idx] = translation(cipherText[idx], key[keyIdx]);
		keyIdx = (keyIdx + 1) % keyLen;
	}

	return cipherText;
}

char *makeKey(void)
{
	int keyLen;
	char tmpKey[21];

	printf("key를 입력하세요 : ");
	scanf("%s", tmpKey);
	char *key = (char *)malloc(sizeof(char) * strlen(tmpKey) + 1);
	strncpy(key, tmpKey, strlen(tmpKey));
	key[strlen(tmpKey)] = '\0';

	return (key);
}

void devideCol(char *cipherText, char *key)
{
	int idx;
	int keyLen = strlen(key);

	int **alphabet = (int **)malloc(sizeof(int *) * keyLen);
	char **split = (char **)malloc(sizeof(char *) * keyLen + 1);
	for (idx = 0; idx < keyLen; idx++)
	{
		split[idx] = (char *)malloc(sizeof(char) * BUFFER_SIZE / keyLen + 1);
		alphabet[idx] = (int *)malloc(sizeof(int) * 26);
	}
	idx = -1;
	while (cipherText[++idx])
	{
		split[idx % keyLen][idx / keyLen] = cipherText[idx];
		alphabet[idx % keyLen][cipherText[idx] - 'A']++;
	}
	for (idx = 0; idx < keyLen; idx++)
	{
		printf("COL %d : %s\nFREQUENCIES %d :", idx + 1, split[idx], idx + 1);
		for (int i = 0; i < 26; i++)
			printf(" %c:%d ", 'A' + i, alphabet[idx][i]);
		printf("\n");
	}
	for (idx = 0; idx < keyLen; idx++)
	{
		free(split[idx]);
		free(alphabet[idx]);
	}
	free(split);
	free(alphabet);
}
int main(void)
{
	char cipherText[BUFFER_SIZE];
	int fd = open("vigenereCipher.txt", O_RDONLY);
	int bytesRead;

	if (fd == -1)
		return 0;
	while ((bytesRead = read(fd, cipherText, sizeof(cipherText) - 1)) > 0)
		cipherText[bytesRead] = '\0';
	close(fd);

	char *key = makeKey();
	devideCol(cipherText, key);
	printf("__RESULT_START__\n%s\n__RESULT_END__\n", decodeCipher(cipherText, key, strlen(key)));
	free(key);

	return (0);
}

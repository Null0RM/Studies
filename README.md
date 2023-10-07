# 2.  비제네르 암호(Vigenère Cipher)로 암호화된 다음 암호문의 키를 찾고 복호화하시오(풀이과정 포함)

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/a0e0f601-27c9-4a59-a57f-d32a6b22c8ef/ab71b0a9-4477-49d1-9aa0-dc8587dbffdc/Untitled.png)

우선, str에서 반복적으로 등장하는 단어를 찾고싶어 아래와 같이 코드를 작성해보았다. 

세번 이상은 나와야할 것 같아 찾아보니,

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int count_occurrences(char *str, char *word)
{
	int count = 0;
	char *pos = str;

	while ((pos = strstr(pos, word)) != NULL)
	{
		count++;
		pos += strlen(word);
	}

	return count;
}

int main()
{
	char str[] = "주어진 Cipher Text"
	char *word = (char *)malloc(sizeof(char) * 4);
	int idx = 0;

	while (str[idx])
	{
		strncpy(word, (str + idx), 3);
		if (count_occurrences(str, word) > 3)
			printf("%s\n", word);
		idx++;
	}

	return 0;
}
```

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/a0e0f601-27c9-4a59-a57f-d32a6b22c8ef/3910db09-2dfa-4195-baed-efbdab95728f/Untitled.png)

이러한 결과가 나왔다. 

그래서 직접 거리를 세어보니 가장 짧게 나오는 거리가 LGE가 21칸 떨어져 있는 것이었다. 

21의 약수는 1, 3, 7, 21이고, 아무래도 귀여운 학부생들이 푸는 문제에 21개 단어를 키 길이로 하진 않으셨을 것 같아 키 길이를 3, 7중 하나로 생각하면 될 것이라고 생각하고 접근해보았다.

## 코드 작성

C언어 코딩 연습도 할 겸 직접 코드를 작성해보았다. 1번 문제에서 조금 비효율적에다 가독성도 좋지 않은 코드를 작성했던 것 같다는 생각이 들어서 이번에는 좀 더 잘 짜보려고 노력한 것 같다. 

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// 1024로 해준 이유는 ... 그냥 갑자기 이 숫자가 생각나서 ... 이다.
#define BUFFER_SIZE 1024

// 시저암호 느낌으로 c에서 key만큼 빼주는 함수.
char translation(char c, char key)
{
	if ((c >= 'A' && c <= 'Z') && (key >= 'A' && key <= 'Z'))
		c = ((c - key + 26) % 26) + 'A';
	return c;
}

// 암호문을 복호화하는 함수이다.
char *decodeCipher(char *cipherText, char *key, int keyLen)
{
	int keyIdx = 0;
	int idx = -1;

	while (++idx < strlen(cipherText))
	{
		cipherText[idx] = translation(cipherText[idx], key[keyIdx]);
		keyIdx = (keyIdx + 1) % keyLen; // 키 길이만큼 세었으면 다시 0부터.
	}

	return cipherText;
}

// 맨 처음 key값을 입력받는 함수.
char *makeKey(void)
{
	int keyLen;
	char tmpKey[21];

	printf("key를 입력하세요 : ");
	scanf("%s", tmpKey);
	// malloc 해준 이유는 그저 key 길이만큼만 배열을 할당하고싶었기 때문이다. 별 의미는 없다..
	char *key = (char *)malloc(sizeof(char) * strlen(tmpKey) + 1);
	strncpy(key, tmpKey, strlen(tmpKey));
	key[strlen(tmpKey)] = '\0';

	return (key);
}

/*
비제니르 암호를 복호화하기 위해, 열을 나누어 저장하고, 출력해주는 함수.
그 과정에서 각 열 별로 등장하는 단어들의 빈도수를 체크해준다.
사실 동적할당 없이 그냥 출력만 해도 되긴 하지만, 이중 배열 할당을 연습해보고 싶어서
이렇게 작성해보았다.
*/
void devideCol(char *cipherText, char *key)
{
	int idx;
	int keyLen = strlen(key);

	// 빈도수 분석을 진행할 alphabet 이중배열 할당
	int **alphabet = (int **)malloc(sizeof(int *) * keyLen);
	// 이중배열 할당
	char **split = (char **)malloc(sizeof(char *) * keyLen + 1);
	// keyLen 개수만큼의 열 분해, alphabet 서브 배열 할당
	for (idx = 0; idx < keyLen; idx++)
	{
		split[idx] = (char *)malloc(sizeof(char) * BUFFER_SIZE / keyLen + 1);
		alphabet[idx] = (int *)malloc(sizeof(int) * 26);
	}
	idx = -1;
	// 열을 divide 해주고, alphabet의 개수를 count해준다.
	while (cipherText[++idx])
	{
		split[idx % keyLen][idx / keyLen] = cipherText[idx];
		alphabet[idx % keyLen][cipherText[idx] - 'A']++;
	}
	// divide 해준 열과 빈도수를 출력해줌.
	for (idx = 0; idx < keyLen; idx++)
	{
		printf("COL %d : %s\nFREQUENCIES :", idx + 1, split[idx]);
		for (int i = 0; i < 26; i++)
			printf(" %c:%d ", 'A' + i, alphabet[idx][i]);
		printf("\n");
	}

	// 동적할당 해제
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
	int fd = open("vigenereCipher.txt", O_RDONLY); //암호문 txt 파일을 불러온다.
	int bytesRead;
	
	if (fd == -1)
		return 0;
	while ((bytesRead = read(fd, cipherText, sizeof(cipherText) - 1)) > 0)
		cipherText[bytesRead] = '\0';
	close(fd);

	char *key = makeKey(); 
	devideCol(cipherText, key); // 열 분해 및 출력
	printf("__RESULT_START__\n%s\n__RESULT_END__\n", 
			decodeCipher(cipherText, key, strlen(key)));
	
	// 메모리 관리는 코딩에 필수적인 요소라고 생각하기 떄문에 할당을 해제시켜주었다.
	free(key);
	
	return (0);
}

```

### 코드 테스트 결과

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/a0e0f601-27c9-4a59-a57f-d32a6b22c8ef/9edbb5b5-2b57-4d4e-ab22-b204f5974f78/Untitled.png)

코드가 정상적으로 잘 작동하는 것은 확인 했으니 이제 문제를 풀면 될 것 같다.

## 복호화 시작

### 키 길이가 7

키 길이 7 먼저 시작해보자.

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/a0e0f601-27c9-4a59-a57f-d32a6b22c8ef/0e76c033-5a3a-418b-b160-6b49dc8c901b/Untitled.png)

빈도수를 체크해보고, 각 열 별로 많이 등장하는 단어들을 ‘E’로 치환한다고 가정하고 들어가면,

| COl | 최다 빈도수 | E와의 차이 |
| --- | --- | --- |
| 1 | M 10회 | 8 (I) |
| 2 | J 15회 | 5 (F) |
| 3 | P 15회 | 11 (L) |
| 4 | S 10회 | 14 (O) |
| 5 | D 14회 | 10 (R) |
| 6 | E 17회 | 0 (A) |
| 7 | Q 12회 | 12 (M) |

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/a0e0f601-27c9-4a59-a57f-d32a6b22c8ef/5e24dab8-a3fe-44bc-93c8-a15fea546145/Untitled.png)

카시스키 테스트를 통해 우선 IFLORAM 이라는 첫 번째 key 후보를 얻었다. 이걸 key로 넣어보자.

<aside>
💡 JHIWQSGENNEPURJBUXQBLQMEQGSEBFFMZSTSAUWMIIWNOVMDTXETVCTHTRURSOFVTHEBLOLEFYKKMOHEELUPJHEVMSNEFOVOETJINKGOUOOUZMAWEKERUEBKTYSCRESHOOQNGCEIAISSEOBWMSSUDGEDEYEUAPTOFCEARLNOMHORMSTBYIKWTNETHMVGLUFTMTOVUDYSCDADGEVWUSBYMSZETXANXPEAYRTLITIRREEBHEANEAEEWEULHKRAIHAXBHEIPEILTHQTWIEERUGOMVGDYDNXKARUIFXPEENPLSAIODRUMVEDCEBEJYIBOVILYOKDAROEREUSPGMMCMIPWVETYOYLANWERSCSLOUSYILLOIHSTDTXEPSEERMITLJOTXMYLINDITIILBEXINHUYBQCKPWOKQTHSETHYNGWKHADGEGIUSUNOAGOUHETLMTRQINEVDICTIILTOJHEXZACAYOYDEAMOKIVMERUTCWURUCHSSINWMEMEASIOOFAESIEDKIVEOOUETLOVMEEVDNEWHSVESJLYMOOTDOTLQNGBEFXKAUIEIPWVETYOYLANWERSCSLOMOVMTHQNTLMAIHTHEBIBHEAXPEKDEWAMWOKLDGZASXATXPESFEEHBHAJWEAMREWOIRODITNTGIREYFTLMEXFLOWQONHUIRMDMUBAFGILEVEHGOUTANKMROKSLC

</aside>

이러한 글이 등장한다. 이렇게만 보면 잘 모르겠긴 하다. 위의 빈도수 표에서 가장 빈도가 적었던 COL1과 COL4를 제외하고 살펴보면

<aside>
💡 _HI_QSG_NN_PUR_BU_QBL_ME_GSE_FF_ZST_AU_MII_NO_MDT_ET_CTH_RU_SOF_T …

</aside>

이렇게 볼 수 있을 것 같다. 그런데, 이 문장이 correct 하다는 가정 하에, 문장의 처음에 HI가 연속으로 등장하면, THIS가 떠오른다. 그래서 1열과 4열의 key를 조금 수정해볼 수 있을 것 같다.

1열 : R에서 T로 :  key=Y

4열 : K에서 S로 : key=S

수정된 key를 다시 입력해보자. (YFLSRAM)

<aside>
💡 THISQSGONNAPURTBUTQBLAMEMGSELFFIZSTCAUSMIIGNORMDTHETRCTHDRUNSOFFTHABLOVEFUKKMYHEALUPTHERMSNOFOROETTINGGOUYOUVMAWOKENUEBUTYOCRECHOKQNGMEIWISSOOBSMSSEDGADEYOUALTOFMEANLNOWHONMSTLYIGWTNOTHIVGLEFTITOVEDYOCDANGERWUSLYMOZETHANTPEAIRTHITIBREABHEKNEWEEWOULDKRASHATBHESPEELTHATWEEEREGOIVGDIDNTKAREIFTPEEXPLOAIONRUIVEDMEBAJYILOVELYOUDANOEROUSLGMMMMILWVEDYOULANGEROCSLYUSUILLYIHOTDTHEPOEERWITHJOTHMYHINDSTIELBEHINDUYBACKLWOKATHOETHINGSKHANGECIUSENOWGOURETHMTRAINAVDIMTIELTOTHETZACKYOUDEAWOKEVMEBUTYWURECHOSINGMEIEASSOOBAESSEDGIVEYOUATLOFMEAVDNOWHOVESTLYIOOTNOTHQNGLEFTKAUSEILWVEDYOULANGEROCSLYMORMTHANTHMAIRTHABIBREATPEKNEWWMWOULDCZASHATTPESPEEDBHATWEWMREGOINODIDNTCIREIFTHMEXPLOSQONRUINMDMEBABGILOVEDGOUDANGMROUSLY

</aside>

어떤 결과가 나올까 … 하고 읽고 있었는데 어느새 나도 모르게 흥얼거리고 있었다. Charlie Puth의 dangerously 가사인 것 같다.

그렇다면 5열의 key만 수정하면 될 것 같다.

H에서 I로 : key=Z

자, 이제 key를 넣어보자.

![Untitled](https://prod-files-secure.s3.us-west-2.amazonaws.com/a0e0f601-27c9-4a59-a57f-d32a6b22c8ef/0488987b-6a85-43a0-8f48-9f1303162830/Untitled.png)

dangerously의 노래 가사가 나온다.

<aside>
💡 KEY : YFLSZAM

</aside>

<aside>
💡 PLAIN TEXT :
THISISGONNAHURTBUTIBLAMEMYSELFFIRSTCAUSEIIGNOREDTHETRUTHDRUNKOFFTHATLOVEFUCKMYHEADUPTHERESNOFORGETTINGYOUYOUVEAWOKENMEBUTYOURECHOKINGMEIWASSOOBSESSEDGAVEYOUALLOFMEANDNOWHONESTLYIGOTNOTHINGLEFTILOVEDYOUDANGEROUSLYMORETHANTHEAIRTHATIBREATHEKNEWWEWOULDCRASHATTHESPEEDTHATWEWEREGOINGDIDNTCAREIFTHEEXPLOSIONRUINEDMEBABYILOVEDYOUDANGEROUSLYMMMMILOVEDYOUDANGEROUSLYUSUALLYIHOLDTHEPOWERWITHBOTHMYHANDSTIEDBEHINDMYBACKLOOKATHOWTHINGSCHANGECAUSENOWYOURETHETRAINANDIMTIEDTOTHETRACKYOUVEAWOKENMEBUTYOURECHOKINGMEIWASSOOBSESSEDGAVEYOUALLOFMEANDNOWHONESTLYIGOTNOTHINGLEFTCAUSEILOVEDYOUDANGEROUSLYMORETHANTHEAIRTHATIBREATHEKNEWWEWOULDCRASHATTHESPEEDTHATWEWEREGOINGDIDNTCAREIFTHEEXPLOSIONRUINEDMEBABYILOVEDYOUDANGEROUSLY

</aside>

# 드디어 !! 끝났다 !! 야호 ^^~*
/*
#include "walsh_lib\FTW1R_C.h"


//-----------------------------------------------------------------
// БЫСТРОЕ ПРЕОБРАЗОВАНИЕ УОЛША-АДАМАРА
// (на основе листинга из 5-го издания учебника
// Гоноровский, Демин "Радиотехнические цепи и сигналы")
//
// s - исходная последовательность;  S - результат преобразования
// n - двоичный логарифм от длины последовательности (n=2,3,4,...)
// flag = 0 - прямое преобразование
// != 0 - обратное преобразование
//-----------------------------------------------------------------
void FastWT(int *s, int *S, int n, char flag)
{ 
	register int M, MM, vz, i, j, k, N;

	M = N = 1<<n; // Вычисление длины последовательности

	for(k=1;k<=n;k++) {
		MM = M;

		M >>= 1;

		for(j=1;j<=N;j+=MM) { // Для всех подпоследовательностей

			vz = -1;

			for(i=1;i<=M;i++) { // Для всех пар в подпоследовательности j

				vz=-vz;

				S[i+j-1] = s[i+i+j-2]+s[i+i+j-1];

				S[i+j+M-1] = vz * (s[i+i+j-2] - s[i+i+j-1]);

			}

		}

		for(i=1;i<=N;i++) s[i]=S[i]; // Копируем  результат  преобразования

	}

	if(flag) for(i=1;i<=N;i++)  S[i]=S[i]/N;   // В случае обратного преобразования
}



// image must be 64 x 64
void GetWalshFunction64(int WalshFunction[64][64], int nX, int nY )
{
	
	const int nSize = 64;
	int W [nSize][nSize];

	
		
	W[0][0] = 1; W[1][0] =  1;
	W[0][1] = 1; W[1][1] = -1;

	//now we build walsh matrix
	for (int curSize = 2; curSize < nSize; curSize *= 2)
	{
		// 1 top right matrix
		for (int m = curSize; m < curSize*2; m++)
			for (int l = 0; l < curSize; l++)
			{
				W[m][l] = W[m - curSize][l];
			};

		// 1 bottom left matrix
		for (m = 0; m < curSize; m++)
			for (int l = curSize; l < curSize*2; l++)
			{
				W[m][l] = W[m][l - curSize];
			}
	
		// 1 bottom right matrix
		for ( m = curSize; m < curSize*2; m++)
			for (int l = curSize; l < curSize*2; l++)
			{
				W[m][l] = - W[m - curSize][l - curSize];
			}
	}

	CString s, tmp;
	for (int i = 0; i < nSize; i++)
	{
		for (int j = 0; j < nSize; j++)
		{
			if (W[i][j] > 0) 
				tmp.Format(" %d  ", W[i][j]);
			else
				tmp.Format("%d  ", W[i][j]);
			if (j % 4 == 0)
				s = s + "\n";
			s = s + tmp;
		}
		s = s + "\n\n"; 
	}
	AfxMessageBox(s);


	CString s, tmp;
	for(int i = 0; i < nSize; i++)
	{
		for (int j = 0; j < nSize; j++)
		{
			WalshFunction[i][j] = W[nX][i]*W[nY][j];
		}
	}

}
*/


int ftwwr_c(int n, float *a)
{
    int i1, i2, i3;
    int i, j, k, l;
    float r;
    int ii;

    /* Parameter adjustments */
    --a;

    /* Function Body */
    l = 1;
L1:
    j = l;
    l += l;
    i1 = j;
    for (k = 1; k <= i1; ++k) {
        i2 = n;
        i3 = l;
        for (i = k; i3 < 0 ? i >= i2 : i <= i2; i += i3) {
            ii = i + j;
            r = a[i];
            a[i] = r + a[ii];
/* L2: */
            a[ii] = r - a[ii];
        }
    }
    if (l - n >= 0) {
        goto L3;
    } else {
        goto L1;
    }
L3:
    return 0;
} /* ftwwr_c */


/* Subroutine */ int fti1r_c(int n, float *a)
{
    int i, j, k;
    float r;
    int k1, n1, n2, ii, jj;

    /* Parameter adjustments */  
	--a;

    /* Function Body */
    n1 = n / 2;
    k1 = n1 / 2;
    n2 = n1 + 1;
    j = 1;
    jj = 1;
L1:
    ++j;
    if (j - n1 <= 0) {
        goto L2;
    } else {
        goto L7;
    }
L2:
    ii = jj + n1;
    r = a[j];
    a[j] = a[ii];
    a[ii] = r;
    ++j;
    k = k1;
L3:
    if (jj - k <= 0) {
        goto L5;
    } else {
        goto L4;
    }
L4:
    jj -= k;
    k /= 2;
    goto L3;
L5:
    jj += k;
    if (jj - j <= 0) {
        goto L1;
    } else {
        goto L6;
    }
L6:
    r = a[j];
    a[j] = a[jj];
    a[jj] = r;
    i = j + n2;
    ii = jj + n2;
    r = a[i];
    a[i] = a[ii];
    a[ii] = r;
    goto L1;
L7:
    return 0;
} /* fti1r_c */


/* Subroutine */ int ftg1r_c(int n, float *a)
{
    /* System generated locals */
    int i1, i2, i3;

    /* Local variables */
    int i, j, k, l;
    float r;
    int l1, l2, ii;

    /* Parameter adjustments */
   --a;

    /* Function Body */
    j = n / 2;
L1:
    l = j + j;
    l1 = j + 1;
    j /= 2;
    l2 = l - j;
    i1 = l2;
    for (k = l1; k <= i1; ++k) {
        i2 = n;
        i3 = l;
        for (i = k; i3 < 0 ? i >= i2 : i <= i2; i += i3) {
            ii = i + j;
            r = a[i];
            a[i] = a[ii];
/* L2: */
            a[ii] = r;
        }
    }
    if (1 - j >= 0) {
        goto L3;
    } else {
        goto L1;
    }
L3:
    return 0;
} /* ftg1r_c */



/* Performs walsh transorm and sort
*/
int ftw1r_c(float *a, int n, int L)
{

   //a;
   ftwwr_c(n, a);
    
	if (L <= 1) 
	{
		return 0;
    }
    
	fti1r_c(n, a);
    
	if (L <= 2) 
	{
		return 0;
    }
    
	ftg1r_c(n, a);

	return 0;

} /* ftw1r_c */

// fills image array with Walsh transform koefs. Size must be of the form 2^k, k - natural number
// sort order may be 1, 2, 3; 1 - no sort, 2 - bit reverse sort, 3 - frquency sort
void DoWalshTransform(float* image, int size, int sortOrder)
{
	ftw1r_c(image, size, sortOrder);
	//for(int i = 0; i < size; i++)  image[i] = image[i];	
}

// fills image array with Walsh transform koefs
void DoReverseWalshTransform(float* image, int size, int sortOrder)
{
	ftw1r_c(image, size, sortOrder);
	for(int i = 0; i < size; i++)  image[i] = image[i]/size;
	
}





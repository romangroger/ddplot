#include <time.h>

#include "darray.h"
#include "qarray.h"


/*----------------------------------------------------------------------------------------------------
  Test of basic operations with dynamically allocated arrays.
----------------------------------------------------------------------------------------------------*/

void test_array_basic()
{
  DArray<int,1> a(3);
  printf("a.dim = %i\n", a.dim);
  printf("a.len[:] = %i\n", a.len[0]);
  printf("a.estep[:] = %i\n", a.estep[0]);
  a(1) = 1;
  a(2) = 2;
  a(3) = 3;
  printf("a.data[:] ... %i %i %i\n", a.data[0], a.data[1], a.data[2]);
  printf("     a(:) ... %i %i %i\n", a(1), a(2), a(3));

  printf("\n");

  DArray<int,2> aa(2,3);
  printf("aa.dim = %i\n", aa.dim);
  printf("aa.len[:] = %i %i\n", aa.len[0], aa.len[1]);
  printf("aa.estep[:] = %i %i\n", aa.estep[0], aa.estep[1]);
  aa(1,1) = 11;
  aa(1,2) = 12;
  aa(1,3) = 13;
  aa(2,1) = 21;
  aa(2,2) = 22;
  aa(2,3) = 23;
  printf("aa.data[:] ... %i %i %i %i %i %i\n", aa.data[0], aa.data[1], aa.data[2],
	 aa.data[3], aa.data[4], aa.data[5]);
  printf("   aa(1,:) ... %i %i %i\n", aa(1,1), aa(1,2), aa(1,3));
  printf("   aa(2,:) ... %i %i %i\n", aa(2,1), aa(2,2), aa(2,3));

  printf("\n");

  DArray<int,3> aaa(2,3,4);
  printf("aaa.dim = %i\n", aaa.dim);
  printf("aaa.len[:] = %i %i %i\n", aaa.len[0], aaa.len[1], aaa.len[2]);
  printf("aaa.estep[:] = %i %i %i\n", aaa.estep[0], aaa.estep[1], aaa.estep[2]);
  aaa(1,1,1) = 111;
  aaa(1,1,2) = 112;
  aaa(1,1,3) = 113;
  aaa(1,1,4) = 114;
  aaa(1,2,1) = 121;
  aaa(1,2,2) = 122;
  aaa(1,2,3) = 123;
  aaa(1,2,4) = 124;
  aaa(1,3,1) = 131;
  aaa(1,3,2) = 132;
  aaa(1,3,3) = 133;
  aaa(1,3,4) = 134;
  aaa(2,1,1) = 211;
  aaa(2,1,2) = 212;
  aaa(2,1,3) = 213;
  aaa(2,1,4) = 214;
  aaa(2,2,1) = 221;
  aaa(2,2,2) = 222;
  aaa(2,2,3) = 223;
  aaa(2,2,4) = 224;
  aaa(2,3,1) = 231;
  aaa(2,3,2) = 232;
  aaa(2,3,3) = 233;
  aaa(2,3,4) = 234;
  printf("aaa.data[:] ... %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i\n", 
	 aaa.data[0], aaa.data[1], aaa.data[2], aaa.data[3],
	 aaa.data[4], aaa.data[5], aaa.data[6], aaa.data[7],
	 aaa.data[8], aaa.data[9], aaa.data[10], aaa.data[11],
	 aaa.data[12], aaa.data[13], aaa.data[14], aaa.data[15],
	 aaa.data[16], aaa.data[17], aaa.data[18], aaa.data[19],
	 aaa.data[20], aaa.data[21], aaa.data[22], aaa.data[23]);
  printf("   aaa(1,1,:) ... %i %i %i %i\n", aaa(1,1,1), aaa(1,1,2), aaa(1,1,3), aaa(1,1,4));
  printf("   aaa(1,2,:) ... %i %i %i %i\n", aaa(1,2,1), aaa(1,2,2), aaa(1,2,3), aaa(1,2,4));
  printf("   aaa(1,3,:) ... %i %i %i %i\n", aaa(1,3,1), aaa(1,3,2), aaa(1,3,3), aaa(1,3,4));
  printf("   aaa(2,1,:) ... %i %i %i %i\n", aaa(2,1,1), aaa(2,1,2), aaa(2,1,3), aaa(2,1,4));
  printf("   aaa(2,2,:) ... %i %i %i %i\n", aaa(2,2,1), aaa(2,2,2), aaa(2,2,3), aaa(2,2,4));
  printf("   aaa(2,3,:) ... %i %i %i %i\n", aaa(2,3,1), aaa(2,3,2), aaa(2,3,3), aaa(2,3,4));

  printf("\n---\n\n");
}


/*----------------------------------------------------------------------------------------------------
  Test of automatic reallocation of the size of arrays.
----------------------------------------------------------------------------------------------------*/

void test_array_resize()
{
  DArray<int,1> a(3);
  printf("a.dim = %i\n", a.dim);
  printf("a.len = %i\n", a.len[0]);
  printf("a.estep = %i\n", a.estep[0]);
  a(1) = 1;
  a(2) = 2;
  a(3) = 3;
  printf("a.data[:] ... %i %i %i\n", a.data[0], a.data[1], a.data[2]);
  printf("     a(:) ... %i %i %i\n", a(1), a(2), a(3));
  a.EnlargeStep(3);
  printf("a.estep = %i\n", a.estep[0]);
  printf("a.len = %i\n", a.len[0]);
  a(4) = 4;
  printf("a.len = %i\n", a.len[0]);
  printf("a.data[:] ... %i %i %i %i %i %i\n", a.data[0], a.data[1], a.data[2], a.data[3], a.data[4], a.data[5]);

  printf("\n");

  DArray<int,2> aa(2,3);
  printf("aa.dim = %i\n", aa.dim);
  printf("aa.len[:] = %i %i\n", aa.len[0], aa.len[1]);
  printf("aa.estep[:] = %i %i\n", aa.estep[0], aa.estep[1]);
  aa(1,1) = 11;
  aa(1,2) = 12;
  aa(1,3) = 13;
  aa(2,1) = 21;
  aa(2,2) = 22;
  aa(2,3) = 23;
  printf("aa.data[:] ... %i %i %i %i %i %i\n", aa.data[0], aa.data[1], aa.data[2],
	 aa.data[3], aa.data[4], aa.data[5]);
  aa.EnlargeStep(1,1);
  aa(4,4) = 44;
  printf("aa.len[:] = %i %i\n", aa.len[0], aa.len[1]);
  printf("aa.data[:] ... %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i\n", 
	 aa.data[0], aa.data[1], aa.data[2],
	 aa.data[3], aa.data[4], aa.data[5], aa.data[6], aa.data[7], aa.data[8],
	 aa.data[9], aa.data[10], aa.data[11], aa.data[12], aa.data[13], aa.data[14], aa.data[15]);
}


void test_qarray()
{
  clock_t t1, t2;
  const int ni=10000, nj=10000;
  double time, val;
  QDMatrix arr(ni,nj);
  int i, j;

  printf("QArray - Writing ... ");
  t1 = clock();
  for (i=1; i<=ni; i++) {
      for (j=1; j<=nj; j++) {
          arr(i,j) = 1.0;
        }
    }
  t2 = clock();
  time = (t2-t1)*1000/CLOCKS_PER_SEC;
  printf("Elapsed time: %0.1lf ms\n", time);

  printf("QArray - Reading ... ");
  t1 = clock();
  for (i=1; i<=ni; i++) {
      for (j=1; j<=nj; j++) {
          val = arr(i,j);
        }
    }
  t2 = clock();
  time = (t2-t1)*1000/CLOCKS_PER_SEC;
  printf("Elapsed time: %0.1lf ms\n", time);
}


void test_darray()
{
  clock_t t1, t2;
  const int ni=10000, nj=10000;
  double time, val;
  DMatrix arr(ni,nj);
  int i, j;

  printf("DArray - Writing ... ");
  t1 = clock();
  for (i=1; i<=ni; i++) {
      for (j=1; j<=nj; j++) {
          arr(i,j) = 1.0;
        }
    }
  t2 = clock();
  time = (t2-t1)*1000/CLOCKS_PER_SEC;
  printf("Elapsed time: %0.1lf ms\n", time);

  printf("DArray - Reading ... ");
  t1 = clock();
  for (i=1; i<=ni; i++) {
      for (j=1; j<=nj; j++) {
          val = arr(i,j);
        }
    }
  t2 = clock();
  time = (t2-t1)*1000/CLOCKS_PER_SEC;
  printf("Elapsed time: %0.1lf ms\n", time);
}

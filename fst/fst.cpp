#include <stdio.h>
#include <iostream>
#include "fst/fstlib.h"


using namespace std;
using namespace fst;
int main(){
	printf("fst demo\n");
	StdVectorFst fst;
	fst.AddState();
	fst.SetStart(0);
	fst.AddArc(0, StdArc(1, 1, 0.5, 1));
	fst.AddArc(0, StdArc(2, 2, 1.5, 1));
	fst.AddState();
	fst.AddArc(1, StdArc(3, 3, 2.5, 2));

	fst.AddState();
	fst.SetFinal(2, 3.5);

	fst.Write("binary.fst");
	
	return 0;
}

all:
	make -f fakeps.mak
	make -f fakeps-vector.mak
	make -f fakeps-simple.mak
	make -f processtree.mak

clean:
	make -f fakeps.mak clean
	make -f fakeps-vector.mak clean
	make -f fakeps-simple.mak clean
	make -f processtree.mak clean


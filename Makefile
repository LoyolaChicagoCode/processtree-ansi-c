all:
	make -f fakeps.mak
	make -f processtree.mak

clean:
	make -f fakeps.mak clean
	make -f processtree.mak clean


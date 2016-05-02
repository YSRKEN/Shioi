.PHONY: Debug
Debug:
	cd test/PackedStoneTest; make Debug
	cd I401; make Debug
	cd Iona; make Debug
.PHONY: Release
Release:
	cd test/PackedStoneTest; make Release
	cd I401; make Release
	cd Iona; make Release
.PHONY: clean
clean:
	cd test/PackedStoneTest; make clean
	cd I401; make clean
	cd Iona; make clean

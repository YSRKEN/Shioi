.PHONY: Debug
Debug:
	cd test/PackedStoneTest; make Debug
	cd I401; make Debug
.PHONY: Release
Release:
	cd test/PackedStoneTest; make Release
	cd I401; make Release
.PHONY: clean
clean:
	cd test/PackedStoneTest; make clean
	cd I401; make clean

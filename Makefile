.PHONY: Debug
Debug:
	cd test/PackedStoneTest; make Debug
.PHONY: Release
Release:
	cd test/PackedStoneTest; make Release
.PHONY: clean
clean:
	cd test/PackedStoneTest; make clean

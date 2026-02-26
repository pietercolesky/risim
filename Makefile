# Default target: show usage
.PHONY: default
default:
	@echo "Usage: make [local|hpc|test]"

# Include the local Makefile
.PHONY: local
local:
	$(MAKE) -f Makefile.local

# Include the HPC Makefile
.PHONY: hpc
hpc:
	$(MAKE) -f Makefile.hpc

# Include the local Makefile
.PHONY: test
test:
	$(MAKE) -f Makefile.test
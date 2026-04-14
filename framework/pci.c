#include "framework/pci.h"
#include "mocks/mock_hw.h"

unsigned int pci_scan(void) { return mock_pci_scan(); }

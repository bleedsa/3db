#include <vm.h>

namespace VM {
	std::mutex mut;
	VMs vms = VMs();
}

VM::VM::~VM() {
	delete[] ins;
	delete[] bods;
}

#include "AccessibleValue.hpp"
#include <cstdio>

int main(int argc, char* argv[]) {
	std::string test;
	Unvirt::AccessibleValue::GetClassIdHierarchy(test, LibCmo::CK_CLASSID::CKCID_TARGETCAMERA);
	printf("%s\n", test.c_str());
	Unvirt::AccessibleValue::GetCkErrorDescription(test, LibCmo::CKERROR::CKERR_OBSOLETEVIRTOOLS);
	printf("%s\n", test.c_str());
	return 0;
}

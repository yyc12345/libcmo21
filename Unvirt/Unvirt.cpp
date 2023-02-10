#include "AccessibleValue.hpp"
#include <cstdio>

int main(int argc, char* argv[]) {
	std::string test;
	Unvirt::AccessibleValue::GetClassIdHierarchy(test, LibCmo::CK_CLASSID::CKCID_TARGETCAMERA);
	printf("%s\n", test.c_str());
	Unvirt::AccessibleValue::GetCkErrorDescription(test, LibCmo::CKERROR::CKERR_OBSOLETEVIRTOOLS);
	printf("%s\n", test.c_str());

	Unvirt::AccessibleValue::GetEnumName<LibCmo::CK_FO_OPTIONS>(Unvirt::AccessibleValue::EnumDesc::CK_FO_OPTIONS, test, LibCmo::CK_FO_OPTIONS::CK_FO_RENAMEOBJECT);
	printf("%s\n", test.c_str());
	
	Unvirt::AccessibleValue::GetFlagEnumName<LibCmo::CK_LOAD_FLAGS>(Unvirt::AccessibleValue::EnumDesc::CK_LOAD_FLAGS, test, LibCmo::CK_LOAD_FLAGS::CK_LOAD_DEFAULT);
	printf("%s\n", test.c_str());
	auto v = LibCmo::EnumHelper::FlagEnumAdd(LibCmo::CK_LOAD_FLAGS::CK_LOAD_ANIMATION, LibCmo::CK_LOAD_FLAGS::CK_LOAD_ASCHARACTER);
	Unvirt::AccessibleValue::GetFlagEnumName<LibCmo::CK_LOAD_FLAGS>(Unvirt::AccessibleValue::EnumDesc::CK_LOAD_FLAGS, test, v);
	printf("%s\n", test.c_str());
	return 0;
}

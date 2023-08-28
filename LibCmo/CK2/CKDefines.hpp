#pragma once

#include "CKTypes.hpp"
#include "../XContainer/XTypes.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <cinttypes>
#include <functional>

namespace LibCmo::CK2 {
	/**
	 * @brief The identifier of Virtools file.
	*/
	constexpr const char CKNEMOFI[] = "Nemo Fi";
	/**
	 * @brief Current Version of CK Engine (Day/Month/Year)
	*/
	constexpr const CKDWORD CKVERSION = 0x13022002u;
	/**
	 * @brief Current Version of Dev
	*/
	constexpr const CKDWORD DEVVERSION = 0x02010001u;
	constexpr const CKDWORD DEVBUILD = 0u;
	constexpr const CKGUID VIRTOOLS_GUID = CKGUID(0x56495254u, 0x4f4f4c53u);

	// ========== Class registration utilities ==========

	//using CKClassRegisterFct = std::function<void()>;
	using CKClassCreationFct = std::function<ObjImpls::CKObject* (CKContext*, CK_ID, CKSTRING)>;
	using CKClassReleaseFct = std::function<void(CKContext*, ObjImpls::CKObject*)>;
	using CKClassNameFct = std::function<CKSTRING()>;
	//using CKClassDependenciesFct = std::function<CKSTRING(CKINT, CKINT)>;
	//using CKClassDependenciesCountFct = std::function<CKINT(CKINT)>;

	struct CKClassDesc {
		CKBOOL Done;
		// Initialized upon class registration
		CK_CLASSID Self;
		CK_CLASSID Parent; // Class Identifier of parent class
		//CKClassRegisterFct RegisterFct; // Pointer to Class Specific Registration function
		CKClassCreationFct CreationFct; // Pointer to Class instance creation function
		CKClassReleaseFct ReleaseFct; // Pointer to Class instance release function
		CKClassNameFct NameFct; // Pointer to Class name function
		//CKClassDependenciesFct DependsFct; // Pointer to Class dependencies function (Copy,delete,replace...)
		//CKClassDependenciesCountFct DependsCountFct; // Pointer to Class dependencies Count function (Copy,delete,replace...)

		//// Initialized by class specific registration function
		//CKDWORD DefaultOptions; // Default options for this class	
		//CKDWORD DefaultCopyDependencies;
		//CKDWORD DefaultDeleteDependencies;
		//CKDWORD DefaultReplaceDependencies;
		//CKDWORD DefaultSaveDependencies;
		//CKGUID Parameter; // Associated parameter GUID

		// Initialized when building class hierarchy table
		CKINT DerivationLevel; // O => CKObject , etc..
		XContainer::XBitArray Parents; // Bit Mask of parents classes
		XContainer::XBitArray Children;	 // Bit Mask of children classes
		//XContainer::XBitArray ToBeNotify; // Mask for Classes that should warn the objects of this class when they are deleted
		//XContainer::XBitArray CommonToBeNotify; // idem but merged with sub classes masks
		//XContainer::XSArray<CK_CLASSID> ToNotify; // List of ClassID to notify when an object of this class is deleted (inverse of ToBeNotify) 					

		CKClassDesc() :
			Done(CKFALSE),
			Self(CK_CLASSID::CKCID_OBJECT), Parent(CK_CLASSID::CKCID_OBJECT),
			CreationFct(nullptr), ReleaseFct(nullptr), NameFct(nullptr),
			DerivationLevel(0),
			Parents(), Children()
		{}
		LIBCMO_DEFAULT_COPY_MOVE(CKClassDesc);
	};

}
